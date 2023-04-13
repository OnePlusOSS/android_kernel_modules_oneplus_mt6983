/*
 *
 * (C) COPYRIGHT 2012-2013, 2017-2018 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-2.0.html.
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 */

#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/atomic.h>
#include <linux/reservation.h>
#include <linux/dma-buf.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/anon_inodes.h>
#include <linux/file.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))

#include <linux/fence.h>

#define dma_fence_context_alloc(a) fence_context_alloc(a)
#define dma_fence_init(a, b, c, d, e) fence_init(a, b, c, d, e)
#define dma_fence_get(a) fence_get(a)
#define dma_fence_put(a) fence_put(a)
#define dma_fence_signal(a) fence_signal(a)
#define dma_fence_is_signaled(a) fence_is_signaled(a)
#define dma_fence_add_callback(a, b, c) fence_add_callback(a, b, c)
#define dma_fence_remove_callback(a, b) fence_remove_callback(a, b)

#if (KERNEL_VERSION(4, 9, 68) > LINUX_VERSION_CODE)
#define dma_fence_get_status(a) (fence_is_signaled(a) ? (a)->status ?: 1 : 0)
#else
#define dma_fence_get_status(a) (fence_is_signaled(a) ? (a)->error ?: 1 : 0)
#endif

#else

#include <linux/dma-fence.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0))
#define dma_fence_get_status(a) (dma_fence_is_signaled(a) ? \
	(a)->status ?: 1 \
	: 0)
#endif

#endif /* < 4.10.0 */

#include "dma_buf_lock.h"

/* Maximum number of buffers that a single handle can address */
#define DMA_BUF_LOCK_BUF_MAX 32

#define DMA_BUF_LOCK_DEBUG 1

#define DMA_BUF_LOCK_INIT_BIAS  0xFF

static dev_t dma_buf_lock_dev;
static struct cdev dma_buf_lock_cdev;
static struct class *dma_buf_lock_class;
static char dma_buf_lock_dev_name[] = "dma_buf_lock";

#ifdef HAVE_UNLOCKED_IOCTL
static long dma_buf_lock_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int dma_buf_lock_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif

static struct file_operations dma_buf_lock_fops =
{
	.owner   = THIS_MODULE,
#ifdef HAVE_UNLOCKED_IOCTL
	.unlocked_ioctl   = dma_buf_lock_ioctl,
#else
	.ioctl   = dma_buf_lock_ioctl,
#endif
	.compat_ioctl   = dma_buf_lock_ioctl,
};

typedef struct dma_buf_lock_resource
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
	struct fence fence;
#else
	struct dma_fence fence;
#endif
	int *list_of_dma_buf_fds;               /* List of buffers copied from userspace */
	atomic_t locked;                        /* Status of lock */
	struct dma_buf **dma_bufs;
	unsigned long exclusive;                /* Exclusive access bitmap */
	atomic_t fence_dep_count;		/* Number of dma-fence dependencies */
	struct list_head dma_fence_callbacks;	/* list of all callbacks set up to wait on other fences */
	wait_queue_head_t wait;
	struct kref refcount;
	struct list_head link;
	struct work_struct work;
	int count;
} dma_buf_lock_resource;

/**
 * struct dma_buf_lock_fence_cb - Callback data struct for dma-fence
 * @fence_cb: Callback function
 * @fence:    Pointer to the fence object on which this callback is waiting
 * @res:      Pointer to dma_buf_lock_resource that is waiting on this callback
 * @node:     List head for linking this callback to the lock resource
 */
struct dma_buf_lock_fence_cb {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
	struct fence_cb fence_cb;
	struct fence *fence;
#else
	struct dma_fence_cb fence_cb;
	struct dma_fence *fence;
#endif
	struct dma_buf_lock_resource *res;
	struct list_head node;
};

static LIST_HEAD(dma_buf_lock_resource_list);
static DEFINE_MUTEX(dma_buf_lock_mutex);

static inline int is_dma_buf_lock_file(struct file *);
static void dma_buf_lock_dounlock(struct kref *ref);


/*** dma_buf_lock fence part ***/

/* Spin lock protecting all Mali fences as fence->lock. */
static DEFINE_SPINLOCK(dma_buf_lock_fence_lock);

static const char *
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
dma_buf_lock_fence_get_driver_name(struct fence *fence)
#else
dma_buf_lock_fence_get_driver_name(struct dma_fence *fence)
#endif
{
	return "dma_buf_lock";
}

static const char *
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
dma_buf_lock_fence_get_timeline_name(struct fence *fence)
#else
dma_buf_lock_fence_get_timeline_name(struct dma_fence *fence)
#endif
{
	return "dma_buf_lock.timeline";
}

static bool
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
dma_buf_lock_fence_enable_signaling(struct fence *fence)
#else
dma_buf_lock_fence_enable_signaling(struct dma_fence *fence)
#endif
{
	return true;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
const struct fence_ops dma_buf_lock_fence_ops = {
	.wait = fence_default_wait,
#else
const struct dma_fence_ops dma_buf_lock_fence_ops = {
	.wait = dma_fence_default_wait,
#endif
	.get_driver_name = dma_buf_lock_fence_get_driver_name,
	.get_timeline_name = dma_buf_lock_fence_get_timeline_name,
	.enable_signaling = dma_buf_lock_fence_enable_signaling,
};

static void
dma_buf_lock_fence_init(dma_buf_lock_resource *resource)
{
	dma_fence_init(&resource->fence,
		       &dma_buf_lock_fence_ops,
		       &dma_buf_lock_fence_lock,
		       0,
		       0);
}

static void
dma_buf_lock_fence_free_callbacks(dma_buf_lock_resource *resource)
{
	struct dma_buf_lock_fence_cb *cb, *tmp;

	/* Clean up and free callbacks. */
	list_for_each_entry_safe(cb, tmp, &resource->dma_fence_callbacks, node) {
		/* Cancel callbacks that hasn't been called yet and release the
		 * reference taken in dma_buf_lock_fence_add_callback().
		 */
		dma_fence_remove_callback(cb->fence, &cb->fence_cb);
		dma_fence_put(cb->fence);
		list_del(&cb->node);
		kfree(cb);
	}
}

static void
dma_buf_lock_fence_work(struct work_struct *pwork)
{
	dma_buf_lock_resource *resource =
		container_of(pwork, dma_buf_lock_resource, work);

	WARN_ON(atomic_read(&resource->fence_dep_count));
	WARN_ON(!atomic_read(&resource->locked));
	WARN_ON(!resource->exclusive);

	mutex_lock(&dma_buf_lock_mutex);
	kref_put(&resource->refcount, dma_buf_lock_dounlock);
	mutex_unlock(&dma_buf_lock_mutex);
}

static void
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
dma_buf_lock_fence_callback(struct fence *fence, struct fence_cb *cb)
#else
dma_buf_lock_fence_callback(struct dma_fence *fence, struct dma_fence_cb *cb)
#endif
{
	struct dma_buf_lock_fence_cb *dma_buf_lock_cb = container_of(cb,
				struct dma_buf_lock_fence_cb,
				fence_cb);
	dma_buf_lock_resource *resource = dma_buf_lock_cb->res;

#if DMA_BUF_LOCK_DEBUG
	printk(KERN_DEBUG "dma_buf_lock_fence_callback\n");
#endif

	/* Callback function will be invoked in atomic context. */

	if (atomic_dec_and_test(&resource->fence_dep_count)) {
		atomic_set(&resource->locked, 1);
		wake_up(&resource->wait);

		if (resource->exclusive) {
			/* Warn if the work was already queued */
			WARN_ON(!schedule_work(&resource->work));
		}
	}
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
static int
dma_buf_lock_fence_add_callback(dma_buf_lock_resource *resource,
				struct fence *fence,
				fence_func_t callback)
#else
static int
dma_buf_lock_fence_add_callback(dma_buf_lock_resource *resource,
				struct dma_fence *fence,
				dma_fence_func_t callback)
#endif
{
	int err = 0;
	struct dma_buf_lock_fence_cb *fence_cb;

	if (!fence)
		return -EINVAL;

	fence_cb = kmalloc(sizeof(*fence_cb), GFP_KERNEL);
	if (!fence_cb)
		return -ENOMEM;

	fence_cb->fence = fence;
	fence_cb->res   = resource;
	INIT_LIST_HEAD(&fence_cb->node);

	err = dma_fence_add_callback(fence, &fence_cb->fence_cb,
				     callback);

	if (err == -ENOENT) {
		/* Fence signaled, get the completion result */
		err = dma_fence_get_status(fence);

		/* remap success completion to err code */
		if (err == 1)
			err = 0;

		kfree(fence_cb);
	} else if (err) {
		kfree(fence_cb);
	} else {
		/*
		 * Get reference to fence that will be kept until callback gets
		 * cleaned up in dma_buf_lock_fence_free_callbacks().
		 */
		dma_fence_get(fence);
		atomic_inc(&resource->fence_dep_count);
		/* Add callback to resource's list of callbacks */
		list_add(&fence_cb->node, &resource->dma_fence_callbacks);
	}

	return err;
}

static int
dma_buf_lock_add_fence_reservation_callback(dma_buf_lock_resource *resource,
					    struct reservation_object *resv,
					    bool exclusive)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
	struct fence *excl_fence = NULL;
	struct fence **shared_fences = NULL;
#else
	struct dma_fence *excl_fence = NULL;
	struct dma_fence **shared_fences = NULL;
#endif
	unsigned int shared_count = 0;
	int err, i;

	err = reservation_object_get_fences_rcu(resv,
						&excl_fence,
						&shared_count,
						&shared_fences);
	if (err)
		return err;

	if (excl_fence) {
		err = dma_buf_lock_fence_add_callback(resource,
						      excl_fence,
						      dma_buf_lock_fence_callback);

		/* Release our reference, taken by reservation_object_get_fences_rcu(),
		 * to the fence. We have set up our callback (if that was possible),
		 * and it's the fence's owner is responsible for singling the fence
		 * before allowing it to disappear.
		 */
		dma_fence_put(excl_fence);

		if (err)
			goto out;
	}

	if (exclusive) {
		for (i = 0; i < shared_count; i++) {
			err = dma_buf_lock_fence_add_callback(resource,
							      shared_fences[i],
							      dma_buf_lock_fence_callback);
			if (err)
				goto out;
		}
	}

	/* Release all our references to the shared fences, taken by
	 * reservation_object_get_fences_rcu(). We have set up our callback (if
	 * that was possible), and it's the fence's owner is responsible for
	 * signaling the fence before allowing it to disappear.
	 */
out:
	for (i = 0; i < shared_count; i++)
		dma_fence_put(shared_fences[i]);
	kfree(shared_fences);

	return err;
}

static void
dma_buf_lock_release_fence_reservation(dma_buf_lock_resource *resource,
				       struct ww_acquire_ctx *ctx)
{
	unsigned int r;

	for (r = 0; r < resource->count; r++)
		ww_mutex_unlock(&resource->dma_bufs[r]->resv->lock);
	ww_acquire_fini(ctx);
}

static int
dma_buf_lock_acquire_fence_reservation(dma_buf_lock_resource *resource,
				       struct ww_acquire_ctx *ctx)
{
	struct reservation_object *content_resv = NULL;
	unsigned int content_resv_idx = 0;
	unsigned int r;
	int err = 0;

	ww_acquire_init(ctx, &reservation_ww_class);

retry:
	for (r = 0; r < resource->count; r++) {
		if (resource->dma_bufs[r]->resv == content_resv) {
			content_resv = NULL;
			continue;
		}

		err = ww_mutex_lock(&resource->dma_bufs[r]->resv->lock, ctx);
		if (err)
			goto error;
	}

	ww_acquire_done(ctx);
	return err;

error:
	content_resv_idx = r;

	/* Unlock the locked one ones */
	while (r--)
		ww_mutex_unlock(&resource->dma_bufs[r]->resv->lock);

	if (content_resv)
		ww_mutex_unlock(&content_resv->lock);

	/* If we deadlock try with lock_slow and retry */
	if (err == -EDEADLK) {
#if DMA_BUF_LOCK_DEBUG
		printk(KERN_DEBUG "deadlock at dma_buf fd %i\n",
		       resource->list_of_dma_buf_fds[content_resv_idx]);
#endif
		content_resv = resource->dma_bufs[content_resv_idx]->resv;
		ww_mutex_lock_slow(&content_resv->lock, ctx);
		goto retry;
	}

	/* If we are here the function failed */
	ww_acquire_fini(ctx);
	return err;
}

static int dma_buf_lock_handle_release(struct inode *inode, struct file *file)
{
	dma_buf_lock_resource *resource;

	if (!is_dma_buf_lock_file(file))
		return -EINVAL;

	resource = file->private_data;
#if DMA_BUF_LOCK_DEBUG
	printk("dma_buf_lock_handle_release\n");
#endif
	mutex_lock(&dma_buf_lock_mutex);
	kref_put(&resource->refcount, dma_buf_lock_dounlock);
	mutex_unlock(&dma_buf_lock_mutex);

	return 0;
}

static unsigned int dma_buf_lock_handle_poll(struct file *file,
                                             struct poll_table_struct *wait)
{
	dma_buf_lock_resource *resource;
	unsigned int ret = 0;

	if (!is_dma_buf_lock_file(file))
		return POLLERR;

	resource = file->private_data;
#if DMA_BUF_LOCK_DEBUG
	printk("dma_buf_lock_handle_poll\n");
#endif
	if (1 == atomic_read(&resource->locked))
	{
		/* Resources have been locked */
		ret = POLLIN | POLLRDNORM;
		if (resource->exclusive)
		{
			ret |=  POLLOUT | POLLWRNORM;
		}
	}
	else
	{
		if (!poll_does_not_wait(wait)) 
		{
			poll_wait(file, &resource->wait, wait);
		}
	}
#if DMA_BUF_LOCK_DEBUG
	printk("dma_buf_lock_handle_poll : return %i\n", ret);
#endif
	return ret;
}

static const struct file_operations dma_buf_lock_handle_fops = {
	.owner		= THIS_MODULE,
	.release	= dma_buf_lock_handle_release,
	.poll		= dma_buf_lock_handle_poll,
};

/*
 * is_dma_buf_lock_file - Check if struct file* is associated with dma_buf_lock
 */
static inline int is_dma_buf_lock_file(struct file *file)
{
	return file->f_op == &dma_buf_lock_handle_fops;
}



/*
 * Start requested lock.
 *
 * Allocates required memory, copies dma_buf_fd list from userspace,
 * acquires related reservation objects, and starts the lock.
 */
static int dma_buf_lock_dolock(dma_buf_lock_k_request *request)
{
	dma_buf_lock_resource *resource;
	struct ww_acquire_ctx ww_ctx;
	int size;
	int fd;
	int i;
	int ret;

	if (NULL == request->list_of_dma_buf_fds)
	{
		return -EINVAL;
	}
	if (request->count <= 0)
	{
		return -EINVAL;
	}
	if (request->count > DMA_BUF_LOCK_BUF_MAX)
	{
		return -EINVAL;
	}
	if (request->exclusive != DMA_BUF_LOCK_NONEXCLUSIVE &&
	    request->exclusive != DMA_BUF_LOCK_EXCLUSIVE)
	{
		return -EINVAL;
	}

	resource = kzalloc(sizeof(dma_buf_lock_resource), GFP_KERNEL);
	if (NULL == resource)
	{
		return -ENOMEM;
	}

	atomic_set(&resource->locked, 0);
	kref_init(&resource->refcount);
	INIT_LIST_HEAD(&resource->link);
	INIT_WORK(&resource->work, dma_buf_lock_fence_work);
	resource->count = request->count;

	/* Allocate space to store dma_buf_fds received from user space */
	size = request->count * sizeof(int);
	resource->list_of_dma_buf_fds = kmalloc(size, GFP_KERNEL);

	if (NULL == resource->list_of_dma_buf_fds)
	{
		kfree(resource);
		return -ENOMEM;
	}

	/* Allocate space to store dma_buf pointers associated with dma_buf_fds */
	size = sizeof(struct dma_buf *) * request->count;
	resource->dma_bufs = kmalloc(size, GFP_KERNEL);

	if (NULL == resource->dma_bufs)
	{
		kfree(resource->list_of_dma_buf_fds);
		kfree(resource);
		return -ENOMEM;
	}

	/* Copy requested list of dma_buf_fds from user space */
	size = request->count * sizeof(int);
	if (0 != copy_from_user(resource->list_of_dma_buf_fds, (void __user *)request->list_of_dma_buf_fds, size))
	{
		kfree(resource->list_of_dma_buf_fds);
		kfree(resource->dma_bufs);
		kfree(resource);
		return -ENOMEM;
	}
#if DMA_BUF_LOCK_DEBUG
	for (i = 0; i < request->count; i++)
	{
		printk("dma_buf %i = %X\n", i, resource->list_of_dma_buf_fds[i]);
	}
#endif

	/* Initialize the fence associated with dma_buf_lock resource */
	dma_buf_lock_fence_init(resource);

	INIT_LIST_HEAD(&resource->dma_fence_callbacks);

	atomic_set(&resource->fence_dep_count, DMA_BUF_LOCK_INIT_BIAS);

	/* Add resource to global list */
	mutex_lock(&dma_buf_lock_mutex);

	list_add(&resource->link, &dma_buf_lock_resource_list);

	mutex_unlock(&dma_buf_lock_mutex);

	for (i = 0; i < request->count; i++)
	{
		/* Convert fd into dma_buf structure */
		resource->dma_bufs[i] = dma_buf_get(resource->list_of_dma_buf_fds[i]);

		if (IS_ERR_VALUE(PTR_ERR(resource->dma_bufs[i])))
		{
			mutex_lock(&dma_buf_lock_mutex);
			kref_put(&resource->refcount, dma_buf_lock_dounlock);
			mutex_unlock(&dma_buf_lock_mutex);
			return -EINVAL;
		}

		/*Check the reservation object associated with dma_buf */
		if (NULL == resource->dma_bufs[i]->resv)
		{
			mutex_lock(&dma_buf_lock_mutex);
			kref_put(&resource->refcount, dma_buf_lock_dounlock);
			mutex_unlock(&dma_buf_lock_mutex);
			return -EINVAL;
		}
#if DMA_BUF_LOCK_DEBUG
		printk(KERN_DEBUG "dma_buf_lock_dolock : dma_buf_fd %i dma_buf %p dma_fence reservation %p\n",
		       resource->list_of_dma_buf_fds[i], resource->dma_bufs[i], resource->dma_bufs[i]->resv);
#endif
	}

	init_waitqueue_head(&resource->wait);

	kref_get(&resource->refcount);

	/* Create file descriptor associated with lock request */
	fd = anon_inode_getfd("dma_buf_lock", &dma_buf_lock_handle_fops,
	                      (void *)resource, 0);
	if (fd < 0)
	{
		mutex_lock(&dma_buf_lock_mutex);
		kref_put(&resource->refcount, dma_buf_lock_dounlock);
		kref_put(&resource->refcount, dma_buf_lock_dounlock);
		mutex_unlock(&dma_buf_lock_mutex);
		return fd;
	}

	resource->exclusive = request->exclusive;

	/* Start locking process */
	ret = dma_buf_lock_acquire_fence_reservation(resource, &ww_ctx);
	if (ret) {
#if DMA_BUF_LOCK_DEBUG
		printk(KERN_DEBUG "dma_buf_lock_dolock : Error %d locking reservations.\n", ret);
#endif
		put_unused_fd(fd);
		mutex_lock(&dma_buf_lock_mutex);
		kref_put(&resource->refcount, dma_buf_lock_dounlock);
		kref_put(&resource->refcount, dma_buf_lock_dounlock);
		mutex_unlock(&dma_buf_lock_mutex);
		return ret;
	}

	/* Take an extra reference for exclusive access, which will be dropped
	 * once the pre-existing fences attached to dma-buf resources, for which
	 * we have commited for exclusive access, are signaled.
	 * At a given time there can be only one exclusive fence attached to a
	 * reservation object, so the new exclusive fence replaces the original
	 * fence and the future sync is done against the new fence which is
	 * supposed to be signaled only after the original fence was signaled.
	 * If the new exclusive fence is signaled prematurely then the resources
	 * would become available for new access while they are already being
	 * written to by the original owner.
	 */
	if (resource->exclusive)
		kref_get(&resource->refcount);

	for (i = 0; i < request->count; i++) {
		struct reservation_object *resv = resource->dma_bufs[i]->resv;

		if (!test_bit(i, &resource->exclusive)) {
			ret = reservation_object_reserve_shared(resv);
			if (ret) {
#if DMA_BUF_LOCK_DEBUG
				printk(KERN_DEBUG "dma_buf_lock_dolock : Error %d reserving space for shared fence.\n", ret);
#endif
				break;
			}

			ret = dma_buf_lock_add_fence_reservation_callback(resource,
									  resv,
									  false);
			if (ret) {
#if DMA_BUF_LOCK_DEBUG
				printk(KERN_DEBUG "dma_buf_lock_dolock : Error %d adding reservation to callback.\n", ret);
#endif
				break;
			}

			reservation_object_add_shared_fence(resv, &resource->fence);
		} else {
			ret = dma_buf_lock_add_fence_reservation_callback(resource,
									  resv,
									  true);
			if (ret) {
#if DMA_BUF_LOCK_DEBUG
				printk(KERN_DEBUG "dma_buf_lock_dolock : Error %d adding reservation to callback.\n", ret);
#endif
				break;
			}

			reservation_object_add_excl_fence(resv, &resource->fence);
		}
	}

	dma_buf_lock_release_fence_reservation(resource, &ww_ctx);

	/* Test if the callbacks were already triggered */
	if (!atomic_sub_return(DMA_BUF_LOCK_INIT_BIAS, &resource->fence_dep_count)) {
		atomic_set(&resource->locked, 1);

		/* Drop the extra reference taken for exclusive access */
		if (resource->exclusive)
			dma_buf_lock_fence_work(&resource->work);
	}

	if (IS_ERR_VALUE((unsigned long)ret))
	{
		put_unused_fd(fd);

		mutex_lock(&dma_buf_lock_mutex);
		kref_put(&resource->refcount, dma_buf_lock_dounlock);
		kref_put(&resource->refcount, dma_buf_lock_dounlock);
		mutex_unlock(&dma_buf_lock_mutex);

		return ret;
	}

#if DMA_BUF_LOCK_DEBUG
	printk("dma_buf_lock_dolock : complete\n");
#endif
	mutex_lock(&dma_buf_lock_mutex);
	kref_put(&resource->refcount, dma_buf_lock_dounlock);
	mutex_unlock(&dma_buf_lock_mutex);

	return fd;
}

static void dma_buf_lock_dounlock(struct kref *ref)
{
	int i;
	dma_buf_lock_resource *resource = container_of(ref, dma_buf_lock_resource, refcount);

	atomic_set(&resource->locked, 0);

	/* Signal the resource's fence. */
	dma_fence_signal(&resource->fence);

	dma_buf_lock_fence_free_callbacks(resource);

	list_del(&resource->link);

	for (i = 0; i < resource->count; i++)
	{
		if (resource->dma_bufs[i])
			dma_buf_put(resource->dma_bufs[i]);
	}

	kfree(resource->dma_bufs);
	kfree(resource->list_of_dma_buf_fds);
	dma_fence_put(&resource->fence);
}

static int __init dma_buf_lock_init(void)
{
	int err;
#if DMA_BUF_LOCK_DEBUG
	printk("dma_buf_lock_init\n");
#endif
	err = alloc_chrdev_region(&dma_buf_lock_dev, 0, 1, dma_buf_lock_dev_name);

	if (0 == err)
	{
		cdev_init(&dma_buf_lock_cdev, &dma_buf_lock_fops);

		err = cdev_add(&dma_buf_lock_cdev, dma_buf_lock_dev, 1);

		if (0 == err)
		{
			dma_buf_lock_class = class_create(THIS_MODULE, dma_buf_lock_dev_name);
			if (IS_ERR(dma_buf_lock_class))
			{
				err = PTR_ERR(dma_buf_lock_class);
			}
			else
			{
				struct device *mdev;
				mdev = device_create(dma_buf_lock_class, NULL, dma_buf_lock_dev, NULL, dma_buf_lock_dev_name);
				if (!IS_ERR(mdev))
				{
					return 0;
				}

				err = PTR_ERR(mdev);
				class_destroy(dma_buf_lock_class);
			}
			cdev_del(&dma_buf_lock_cdev);
		}

		unregister_chrdev_region(dma_buf_lock_dev, 1);
	}
#if DMA_BUF_LOCK_DEBUG
	printk("dma_buf_lock_init failed\n");
#endif
	return err;
}

static void __exit dma_buf_lock_exit(void)
{
#if DMA_BUF_LOCK_DEBUG
	printk("dma_buf_lock_exit\n");
#endif

	/* Unlock all outstanding references */
	while (1)
	{
		mutex_lock(&dma_buf_lock_mutex);
		if (list_empty(&dma_buf_lock_resource_list))
		{
			mutex_unlock(&dma_buf_lock_mutex);
			break;
		}
		else
		{
			dma_buf_lock_resource *resource = list_entry(dma_buf_lock_resource_list.next, 
			                                             dma_buf_lock_resource, link);
			kref_put(&resource->refcount, dma_buf_lock_dounlock);
			mutex_unlock(&dma_buf_lock_mutex);
		}
	}

	device_destroy(dma_buf_lock_class, dma_buf_lock_dev);

	class_destroy(dma_buf_lock_class);

	cdev_del(&dma_buf_lock_cdev);

	unregister_chrdev_region(dma_buf_lock_dev, 1);
}

#ifdef HAVE_UNLOCKED_IOCTL
static long dma_buf_lock_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int dma_buf_lock_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
	dma_buf_lock_k_request request;
	int size = _IOC_SIZE(cmd);

	if (_IOC_TYPE(cmd) != DMA_BUF_LOCK_IOC_MAGIC)
	{
		return -ENOTTY;

	}
	if ((_IOC_NR(cmd) < DMA_BUF_LOCK_IOC_MINNR) || (_IOC_NR(cmd) > DMA_BUF_LOCK_IOC_MAXNR))
	{
		return -ENOTTY;
	}

	switch (cmd)
	{
		case DMA_BUF_LOCK_FUNC_LOCK_ASYNC:
			if (size != sizeof(dma_buf_lock_k_request))
			{
				return -ENOTTY;
			}
			if (copy_from_user(&request, (void __user *)arg, size))
			{
				return -EFAULT;
			}
#if DMA_BUF_LOCK_DEBUG
			printk("DMA_BUF_LOCK_FUNC_LOCK_ASYNC - %i\n", request.count);
#endif
			return dma_buf_lock_dolock(&request);
	}

	return -ENOTTY;
}

module_init(dma_buf_lock_init);
module_exit(dma_buf_lock_exit);

MODULE_LICENSE("GPL");

