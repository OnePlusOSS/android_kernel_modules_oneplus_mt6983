// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#include <linux/seq_file.h>
#if IS_ENABLED(CONFIG_PROC_FS)
#include <linux/proc_fs.h>
#endif
#include <mali_kbase.h>
#include <mali_kbase_defs.h>
#include <platform/mtk_platform_common.h>
#include <platform/mtk_platform_common/mtk_platform_memtrack.h>
#include <linux/delay.h>
#if IS_ENABLED(CONFIG_OPLUS_FEATURE_MM_OSVELTE)
#include <linux/proc_fs.h>
#include "sys-memstat.h"
#include "common.h"
#include "memstat.h"
#endif /* CONFIG_OPLUS_FEATURE_MM_OSVELTE */

extern unsigned int (*mtk_get_gpu_memory_usage_fp)(void);

#if IS_ENABLED(CONFIG_PROC_FS)
/* name of the proc entry */
#define	PROC_GPU_MEMORY "gpu_memory"

static DEFINE_MUTEX(memtrack_lock);

static int mtk_gpu_memory_show(struct seq_file *m, void *v)
{
	struct kbase_device *kbdev = (struct kbase_device *)mtk_common_get_kbdev();
	struct kbase_context *kctx;
	unsigned int trylock_count = 0;

	if (IS_ERR_OR_NULL(kbdev))
		return -1;

	lockdep_off();

	mutex_lock(&memtrack_lock);

	while (!mutex_trylock(&kbdev->kctx_list_lock)) {
		if (trylock_count > 3) {
			pr_info("[%s]lock held, bypass memory usage query", __func__);
			seq_printf(m, "<INVALID>");
			goto out_lock_held;
		}
		trylock_count ++;
		udelay(10);
	}

	/* output the total memory usage */
	seq_printf(m, "%-16s  %10u\n",
	           kbdev->devname,
	           atomic_read(&(kbdev->memdev.used_pages)));

	list_for_each_entry(kctx, &kbdev->kctx_list, kctx_list_link) {
		/* output the memory usage and cap for each kctx */
		seq_printf(m, "  %s-0x%p %10u %10u\n",
		           "kctx",
		           kctx,
		           atomic_read(&(kctx->used_pages)),
		           kctx->tgid);
	}
	mutex_unlock(&kbdev->kctx_list_lock);

out_lock_held:
	mutex_unlock(&memtrack_lock);

	lockdep_on();

	return 0;
}
DEFINE_PROC_SHOW_ATTRIBUTE(mtk_gpu_memory);
#if IS_ENABLED(CONFIG_OPLUS_FEATURE_MM_OSVELTE)
static int mali_procinfo_show(struct seq_file *s, void *unused)
{
	return mtk_gpu_memory_show(s, unused);
}
DEFINE_PROC_SHOW_ATTRIBUTE(mali_procinfo);

long read_mali_mem_usage(enum mtrack_subtype type)
{
	struct kbase_device *kbdev = (struct kbase_device *)mtk_common_get_kbdev();
	unsigned long sz = 0;

	if (IS_ERR_OR_NULL(kbdev))
		return 0;

	if (type == MTRACK_GPU_TOTAL)
		sz = atomic_read(&(kbdev->memdev.used_pages));

	return sz;
}

void dump_mali_usage_stat(bool verbose)
{
	struct kbase_device *kbdev = (struct kbase_device *)mtk_common_get_kbdev();
	struct kbase_context *kctx;

	if (IS_ERR_OR_NULL(kbdev))
		return;

	osvelte_info("======= %s\n", __func__);
	osvelte_info("%16s %5s %s\n", "comm", "pid", "size");

	mutex_lock(&memtrack_lock);
	mutex_lock(&kbdev->kctx_list_lock);
	list_for_each_entry(kctx, &kbdev->kctx_list, kctx_list_link) {
		struct task_struct *task;
		rcu_read_lock();
		task = find_task_by_vpid(kctx->tgid);
		osvelte_info("%16s %5u %u\n",
			     task ? task->comm : "[null task]",
			     kctx->tgid,
			     K(atomic_read(&(kctx->used_pages))));
		rcu_read_unlock();
	}
	mutex_unlock(&kbdev->kctx_list_lock);
	mutex_unlock(&memtrack_lock);
}

long read_pid_mali_mem_usage(enum mtrack_subtype mtype, pid_t pid)
{
	struct kbase_device *kbdev = (struct kbase_device *)mtk_common_get_kbdev();
	struct kbase_context *kctx;
	unsigned long sz = 0;

	if (IS_ERR_OR_NULL(kbdev))
		return 0;

	/* maybe we do not need this lock, but for stability add this. */
	mutex_lock(&memtrack_lock);
	mutex_lock(&kbdev->kctx_list_lock);
	list_for_each_entry(kctx, &kbdev->kctx_list, kctx_list_link) {
		if (kctx->tgid == pid) {
			sz = atomic_read(&(kctx->used_pages));
			break;
		}
	}
	mutex_unlock(&kbdev->kctx_list_lock);
	mutex_unlock(&memtrack_lock);
	return sz;
}

static struct mtrack_debugger mali_mtrack_debugger = {
	.mem_usage = read_mali_mem_usage,
	.pid_mem_usage = read_pid_mali_mem_usage,
	.dump_usage_stat = dump_mali_usage_stat,
};
#endif /* CONFIG_OPLUS_FEATURE_MM_OSVELTE */

int mtk_memtrack_procfs_init(struct kbase_device *kbdev, struct proc_dir_entry *parent)
{
	if (IS_ERR_OR_NULL(kbdev))
		return -1;

	proc_create(PROC_GPU_MEMORY, 0444, parent, &mtk_gpu_memory_proc_ops);

#if IS_ENABLED(CONFIG_OPLUS_FEATURE_MM_OSVELTE)
	register_mtrack_debugger(MTRACK_GPU, &mali_mtrack_debugger);
	register_mtrack_procfs(MTRACK_GPU, "procinfo", 0444,
			       &mali_procinfo_proc_ops, NULL);
#endif /* CONFIG_OPLUS_FEATURE_MM_OSVELTE */

	return 0;
}

int mtk_memtrack_procfs_term(struct kbase_device *kbdev, struct proc_dir_entry *parent)
{
	if (IS_ERR_OR_NULL(kbdev))
		return -1;

	remove_proc_entry(PROC_GPU_MEMORY, parent);

#if IS_ENABLED(CONFIG_OPLUS_FEATURE_MM_OSVELTE)
	unregister_mtrack_debugger(MTRACK_GPU, &mali_mtrack_debugger);
	unregister_mtrack_procfs(MTRACK_GPU, "procinfo");
#endif /* CONFIG_OPLUS_FEATURE_MM_OSVELTE */

	return 0;
}
#else
int mtk_memtrack_procfs_init(struct kbase_device *kbdev, struct proc_dir_entry *parent)
{
	return 0;
}

int mtk_memtrack_procfs_term(struct kbase_device *kbdev, struct proc_dir_entry *parent)
{
	return 0;
}
#endif /* CONFIG_PROC_FS */

/* Return the total memory usage */
static unsigned int mtk_memtrack_gpu_memory_total(void)
{
	struct kbase_device *kbdev = (struct kbase_device *)mtk_common_get_kbdev();
	unsigned int used_pages;

	if (IS_ERR_OR_NULL(kbdev))
		return 0;

	used_pages = atomic_read(&(kbdev->memdev.used_pages));

	return used_pages * 4096;
}

int mtk_memtrack_init(struct kbase_device *kbdev)
{
	if (IS_ERR_OR_NULL(kbdev))
		return -1;

	mtk_get_gpu_memory_usage_fp = mtk_memtrack_gpu_memory_total;

	return 0;
}

int mtk_memtrack_term(struct kbase_device *kbdev)
{
	if (IS_ERR_OR_NULL(kbdev))
		return -1;

	mtk_get_gpu_memory_usage_fp = NULL;

	return 0;
}
