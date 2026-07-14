// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Oplus. All rights reserved.
 */

#include <uapi/linux/sched/types.h>
#include <linux/atomic.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <trace/hooks/sched.h>

#include "game_ctrl.h"

#define SKIP_GAMESELF_SCHED_SETAFFINITY (1 << 0)
#define DEBUG_SCHED_SETAFFINITY_INFO (1 << 1)

int skip_gameself_setaffinity = 0;
static DEFINE_MUTEX(d_mutex);

static void sched_setaffinity_early_hook(void *unused, struct task_struct *p,
	const struct cpumask *in_mask, int *skip)
{
	if (p->tgid == game_pid) {
		if ((skip_gameself_setaffinity & SKIP_GAMESELF_SCHED_SETAFFINITY) && (current->tgid == p->tgid)) {
			*skip = 1;
		}

		if (skip_gameself_setaffinity & DEBUG_SCHED_SETAFFINITY_INFO) {
			pr_info("gameopt, %s: c_comm=%s, c_pid=%d, c_tgid=%d, comm=%s, pid=%d, tgid=%d, in_mask=%*pbl, cpus_ptr=%*pbl, skip=%d\n",
				__func__, current->comm, current->pid, current->tgid, p->comm, p->pid, p->tgid,
				cpumask_pr_args(in_mask), cpumask_pr_args(p->cpus_ptr), *skip);
		}
	}
}

static ssize_t skip_gameself_setaffinity_proc_write(struct file *file,
	const char __user *buf, size_t count, loff_t *ppos)
{
	char page[32] = {0};
	int ret, value;
	static bool register_trace = false;

	ret = simple_write_to_buffer(page, sizeof(page) - 1, ppos, buf, count);
	if (ret <= 0)
		return ret;

	ret = sscanf(page, "%d", &value);
	if (ret != 1)
		return -EINVAL;

	mutex_lock(&d_mutex);
	skip_gameself_setaffinity = value;
	if (skip_gameself_setaffinity > 0) {
		if (!register_trace) {
			register_trace_android_vh_sched_setaffinity_early(sched_setaffinity_early_hook, NULL);
			register_trace = true;
		}
	} else {
		if (register_trace) {
			unregister_trace_android_vh_sched_setaffinity_early(sched_setaffinity_early_hook, NULL);
			register_trace = false;
		}
	}
	mutex_unlock(&d_mutex);

	return count;
}

static ssize_t skip_gameself_setaffinity_proc_read(struct file *file,
	char __user *buf, size_t count, loff_t *ppos)
{
	char page[32] = {0};
	int len;

	len = sprintf(page, "%d\n", skip_gameself_setaffinity);

	return simple_read_from_buffer(buf, count, ppos, page, len);
}

static const struct proc_ops skip_gameself_setaffinity_proc_ops = {
	.proc_write		= skip_gameself_setaffinity_proc_write,
	.proc_read		= skip_gameself_setaffinity_proc_read,
	.proc_lseek		= default_llseek,
};

int debug_init(void)
{
	proc_create_data("skip_gameself_setaffinity", 0664, game_opt_dir, &skip_gameself_setaffinity_proc_ops, NULL);

	return 0;
}
