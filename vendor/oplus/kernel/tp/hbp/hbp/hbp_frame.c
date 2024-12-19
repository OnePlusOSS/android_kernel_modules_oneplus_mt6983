#include "hbp_frame.h"
#include "hbp_trace.h"

static struct list_head *frame_create(unsigned int buf_size);

int frame_init(struct frame_queue *queue, int buf_size)
{
	int i = MAX_FRAME_COUNT;
	struct list_head *flist;

	/*destory first*/
	frame_destory(queue);

	mutex_lock(&queue->lock);

	do {
		flist = frame_create(buf_size);
		if (!flist) {
			hbp_err("failed to create frame %d\n", i);
			break;
		}
		list_add(flist, &queue->freed);
	} while (--i);

	hbp_info("%d frames create\n", MAX_FRAME_COUNT - i);
	mutex_unlock(&queue->lock);
	return 0;
}

static struct list_head *frame_create(unsigned int buf_size)
{
	struct frame_list *frame;

	frame = (struct frame_list *)kzalloc(sizeof(struct frame_list), GFP_KERNEL);
	if (!frame) {
		return NULL;
	}
	INIT_LIST_HEAD(&frame->list);

	frame->buf = kzalloc(buf_size, GFP_KERNEL);
	if (!frame->buf) {
		kfree(frame);
		return NULL;
	}

	frame->buf_size = buf_size;
	frame->data_size = buf_size - sizeof(struct frame_buf) + 1;
	frame->cunsumed = true;

	return &frame->list;
}

void frame_destory(struct frame_queue *queue)
{
	struct frame_list *frame, *tmp;

	mutex_lock(&queue->lock);
	/*need to free queue list, freed and cunsume list*/
	while (!list_empty(&queue->cunsume)) {
		list_move_tail(queue->cunsume.next, &queue->freed);
	}

	list_for_each_entry_safe(frame, tmp, &queue->freed, list) {
		kfree(frame->buf);
		kfree(frame);
	}

	INIT_LIST_HEAD(&queue->freed);
	INIT_LIST_HEAD(&queue->cunsume);

	mutex_unlock(&queue->lock);
}

static inline int frame_write(struct list_head *flist, char *data,
			      unsigned int data_size,
			      enum irq_reason reason,
			      unsigned long long frm_count)
{
	struct frame_list *frame = container_of(flist, struct frame_list, list);
	struct timespec64 tv;

	if (data_size <= frame->data_size) {
		ktime_get_ts64(&tv);
		frame->buf->frame_tv0.value[0] = timespec64_to_ns(&tv);
		frame->buf->reason = reason;
		memcpy(&frame->buf->data[0], data, data_size);
		frame->cunsumed = false;
		frame->frm_count = frm_count;
	} else {
		hbp_err("write out of limited size %d, limit %d\n", data_size, frame->data_size);
		return -EACCES;
	}

	return 0;
}

static inline struct list_head *frame_valid_list(struct list_head *freed,
		struct frame_queue *queue)
{
	struct frame_list *frame;
	struct list_head *valid = NULL;

	if (list_empty(freed)) {
		if (!list_empty(&queue->cunsume)) {
			frame = list_first_entry(&queue->cunsume, struct frame_list, list);
			frame->cunsumed = true;
			list_move_tail(&frame->list, freed);
			hbp_info("get frame from cunsumed list, count %lld\n", frame->frm_count);
		} else {
			return NULL;
		}
	}

	list_for_each_entry(frame, freed, list) {
		if (frame->cunsumed == true) {
			valid = &frame->list;
			break;
		}
	}

	return valid;
}

inline int frame_put(char *data,
		     unsigned int data_size,
		     enum irq_reason reason,
		     struct frame_queue *queue)
{
	struct frame_list *frame, *temp;
	struct list_head *valid;

	trace_hbp(0, "frame_put", TRACE_START);
	mutex_lock(&queue->lock);

	valid = frame_valid_list(&queue->freed, queue);
	if (!valid) {
		hbp_err("droped frame %llu\n", queue->frame_count);
		mutex_unlock(&queue->lock);
		return -EFAULT;
	}

	frame_write(valid, data, data_size, reason, queue->frame_count);

	list_for_each_entry_safe(frame, temp, &queue->freed, list) {
		if (frame->cunsumed == false) {
			list_move_tail(&frame->list, &queue->cunsume);
		}
	}

	mutex_unlock(&queue->lock);
	trace_hbp(0, "frame_put", TRACE_END);
	return 0;
}

static int frame_read(char __user *buf,
		      unsigned int buf_size,
		      struct list_head *flist)
{
	struct frame_list *frame = container_of(flist, struct frame_list, list);
	if (buf_size <= frame->buf_size) {
		frame->cunsumed = true;
		if (copy_to_user(buf, frame->buf, buf_size)) {
			hbp_err("failed to copy frame to user, count %lld\n", frame->frm_count);
			return -EFAULT;
		}
	} else {
		hbp_err("read out of limited size %d, limit %d\n", buf_size, frame->buf_size);
		return -EACCES;
	}

	return 0;
}

inline int frame_get(char     __user *buf, unsigned int size, struct frame_queue *queue)
{
	struct frame_list *frame, *temp;
	int ret = -EINVAL;

	if (list_empty(&queue->cunsume)) {
		queue->waitq_flag = QUEUE_WAIT;
		wait_event_interruptible(queue->waitq, (queue->waitq_flag == QUEUE_WAKEUP));
	}

	mutex_lock(&queue->lock);
	list_for_each_entry_safe(frame, temp, &queue->cunsume, list) {
		if (frame->cunsumed == false) {
			ret = frame_read(buf, size, &frame->list);
			list_move_tail(&frame->list, &queue->freed);
			break;
		} else {
			/*warning if consumed == true in cunsume list, why?*/
			list_move_tail(&frame->list, &queue->freed);
			hbp_warn("unexpect cunsumed frame data %lld\n", frame->frm_count);
		}
	}
	mutex_unlock(&queue->lock);

	return ret;
}

inline void frame_clear(struct frame_queue *queue)
{
	struct frame_list *frame, *temp;

	mutex_lock(&queue->lock);
	list_for_each_entry_safe(frame, temp, &queue->cunsume, list) {
		frame->cunsumed = true;
		list_move_tail(&frame->list, &queue->freed);
	}
	mutex_unlock(&queue->lock);
}

void frame_wake_up_waitq(struct frame_queue *queue)
{
	queue->waitq_flag = QUEUE_WAKEUP;
	wake_up_interruptible(&(queue->waitq));
}

