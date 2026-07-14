#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>
#include <linux/kernel.h>
#include <linux/sched/cputime.h>
#include <kernel/sched/sched.h>

#include "sa_common.h"
#include "sa_pipeline.h"

#define MAX_PIPELINE_TASK_NUM 5
static int pipeline_pids[MAX_PIPELINE_TASK_NUM] = {-1, -1, -1, -1, -1};
static int pipeline_cpus[MAX_PIPELINE_TASK_NUM] = {-1, -1, -1, -1, -1};
static struct task_struct *pipeline_task[MAX_PIPELINE_TASK_NUM] = {NULL};
static struct oplus_task_struct *pipeline_ots[MAX_PIPELINE_TASK_NUM] = {NULL};
static struct task_struct *prime_task = NULL;
static struct oplus_task_struct *prime_ots = NULL;
static unsigned int pipeline_task_nr = 0;
static DEFINE_RAW_SPINLOCK(pipeline_lock);

#if IS_ENABLED(CONFIG_SCHED_WALT)
static bool pipeline_prime_rearrange = false;
#define PIPELINE_SWAP_UTIL_DIFF 50
#endif

#define PIPELINE_TASK_UX_STATE (UX_PRIORITY_PIPELINE | SA_TYPE_HEAVY)

static DEFINE_MUTEX(p_mutex);

static inline bool is_valid_pipeline_task(int pipeline_cpu, int ux_state)
{
	return (pipeline_cpu > 0) && (pipeline_cpu <= nr_cpu_ids) &&
		((ux_state & PIPELINE_TASK_UX_STATE) == PIPELINE_TASK_UX_STATE);
}

bool oplus_is_pipeline_task(struct task_struct *task)
{
	struct oplus_task_struct *ots;

	ots = get_oplus_task_struct(task);
	if (IS_ERR_OR_NULL(ots))
		return false;

	return is_valid_pipeline_task(atomic_read(&ots->pipeline_cpu), ots->ux_state);
}

int oplus_get_task_pipeline_cpu(struct task_struct *task)
{
	struct oplus_task_struct *ots;
	int pipeline_cpu;

	if (unlikely(!global_sched_assist_enabled))
		return -1;

	ots = get_oplus_task_struct(task);
	if (IS_ERR_OR_NULL(ots))
		return -1;

	pipeline_cpu = atomic_read(&ots->pipeline_cpu);
	/*
	 * some tasks have PIPELINE_TASK_UX_STATE priority,
	 * but have a invalid pipeline_cpu 'nr_cpu_ids'.
	 */
	if (pipeline_cpu == nr_cpu_ids)
		return -1;
	if (is_valid_pipeline_task(pipeline_cpu, ots->ux_state))
		return pipeline_cpu;

	return -1;
}
EXPORT_SYMBOL_GPL(oplus_get_task_pipeline_cpu);

bool oplus_pipeline_low_latency_task(int pipeline_cpu)
{
	struct task_struct *task;
	struct oplus_task_struct *ots;

	if (unlikely(!global_sched_assist_enabled))
		return true;

	task = cpu_rq(pipeline_cpu)->curr;

	if (task->prio < MAX_RT_PRIO)
		return true;

	ots = get_oplus_task_struct(task);
	if (IS_ERR_OR_NULL(ots))
		return false;

	if ((ots->ux_state & SCHED_ASSIST_UX_PRIORITY_MASK) > UX_PRIORITY_PIPELINE)
		return true;

	if (is_valid_pipeline_task(atomic_read(&ots->pipeline_cpu), ots->ux_state))
		return true;

	return false;
}
EXPORT_SYMBOL_GPL(oplus_pipeline_low_latency_task);

bool oplus_is_pipeline_scene(void)
{
	return pipeline_task_nr >= 1;
}
EXPORT_SYMBOL_GPL(oplus_is_pipeline_scene);

static void systrace_c_printk(const char *msg, unsigned long val)
{
	char buf[256];

	snprintf(buf, sizeof(buf), "C|99999|%s|%lu\n", msg, val);
	tracing_mark_write(buf);
}

#if IS_ENABLED(CONFIG_SCHED_WALT)
static inline unsigned int pipeline_task_avg_util(struct task_struct *p, unsigned int divisor)
{
	unsigned int avg_util;

	struct walt_task_struct *wts = (struct walt_task_struct *) p->android_vendor_data1;
	unsigned int coloc_demand = wts->coloc_demand;

	if (divisor > 0)
		do_div(coloc_demand, divisor);

	avg_util = coloc_demand;

	if (unlikely(global_debug_enabled & DEBUG_PIPELINE)) {
		char buf[64];

		snprintf(buf, sizeof(buf), "%s_avg_util\n", p->comm);
		systrace_c_printk(buf, avg_util);
		snprintf(buf, sizeof(buf), "%s_util\n", p->comm);
		systrace_c_printk(buf, wts->demand_scaled);
	}

	return avg_util;
}

static inline unsigned int pipeline_task_util(struct task_struct *p)
{
	struct walt_task_struct *wts = (struct walt_task_struct *) p->android_vendor_data1;

	return wts->demand_scaled;
}

void qcom_rearrange_pipeline_preferred_cpus(unsigned int divisor)
{
	int i;
	struct task_struct *task;
	struct oplus_task_struct *ots;
	struct task_struct *max_util_task = NULL;
	struct oplus_task_struct *max_util_ots = NULL;
	unsigned int task_util = 0;
	unsigned int max_util = 0;
	unsigned int prime_util = 0;
	bool avg_prime_swap = false;
	bool prime_swap = false;
	unsigned long flags;

	if (unlikely(!global_sched_assist_enabled))
		return;

	if (!pipeline_prime_rearrange)
		return;

	if ((pipeline_task_nr <= 1) || (prime_task == NULL))
		return;

	if (raw_spin_trylock_irqsave(&pipeline_lock, flags)) {
		if ((pipeline_task_nr <= 1) || (prime_task == NULL))
			goto unlock;

		if (prime_task->nr_cpus_allowed < 2)
			goto unlock;

		for (i = 0; i < MAX_PIPELINE_TASK_NUM; i++) {
			if (pipeline_task[i]) {
				task = pipeline_task[i];
				if (!cpumask_test_cpu(nr_cpu_ids - 1, task->cpus_ptr))
					continue;
				ots = pipeline_ots[i];
				if (atomic_read(&ots->pipeline_cpu) == nr_cpu_ids)
					continue;

				task_util = pipeline_task_avg_util(task, divisor);

				if (max_util_task == NULL || max_util < task_util) {
					max_util_task = task;
					max_util_ots = ots;
					max_util = task_util;
				}

				if (task == prime_task)
					prime_util = task_util;
			}
		}

		if ((max_util_task != NULL) &&
				(max_util_task != prime_task) &&
				(max_util - prime_util > PIPELINE_SWAP_UTIL_DIFF)) {
			avg_prime_swap = true;
			goto swap;
		}

		max_util_task = NULL;
		max_util_ots = NULL;
		max_util = 0;
		prime_util = 0;
		for (i = 0; i < MAX_PIPELINE_TASK_NUM; i++) {
			if (pipeline_task[i]) {
				task = pipeline_task[i];
				if (!cpumask_test_cpu(nr_cpu_ids - 1, task->cpus_ptr))
					continue;
				ots = pipeline_ots[i];
				if (atomic_read(&ots->pipeline_cpu) == nr_cpu_ids)
					continue;

				task_util = pipeline_task_util(task);

				if (max_util_task == NULL || max_util < task_util) {
					max_util_task = task;
					max_util_ots = ots;
					max_util = task_util;
				}

				if (task == prime_task)
					prime_util = task_util;
			}
		}

		if ((max_util_task != NULL) &&
				(max_util_task != prime_task) &&
				(max_util - prime_util > (PIPELINE_SWAP_UTIL_DIFF << 1))) {
			prime_swap = true;
		}

swap:
		if (avg_prime_swap || prime_swap) {
			atomic_set(&prime_ots->pipeline_cpu, atomic_read(&max_util_ots->pipeline_cpu));
			atomic_set(&max_util_ots->pipeline_cpu, nr_cpu_ids - 1);
			prime_task = max_util_task;
			prime_ots = max_util_ots;

			if (unlikely(global_debug_enabled & DEBUG_PIPELINE)) {
				systrace_c_printk("swap_prime_task", avg_prime_swap? 1 : 2);
				systrace_c_printk("swap_prime_task", 0);
			}
		}

unlock:
		raw_spin_unlock_irqrestore(&pipeline_lock, flags);
	}
}
EXPORT_SYMBOL_GPL(qcom_rearrange_pipeline_preferred_cpus);
#endif

#if IS_ENABLED(CONFIG_MTK_FPSGO_V3)
static DEFINE_RAW_SPINLOCK(mtk_rearrange_lock);
#define REARRANGE_INTERVAL_NS 5000000 /* 5ms */
static u64 last_rearrange_begin_time = 0;
static pid_t rearrange_thread_pid = 0;
static bool demote_prime_task = false;
static struct task_struct *promote_task = NULL;
static bool rearrange_done = false;

static inline void pipeline_swap_prime_task(void)
{
	int i;
	struct oplus_task_struct *promote_ots;
	unsigned long flags;

	if (raw_spin_trylock_irqsave(&pipeline_lock, flags)) {
		if ((pipeline_task_nr <= 1) || (prime_task == NULL))
			goto unlock;

		if (prime_task->nr_cpus_allowed < 2)
			goto unlock;

		if (unlikely(promote_task == prime_task))
			goto unlock;

		for (i = 0; i < MAX_PIPELINE_TASK_NUM; i++) {
			if (pipeline_task[i] &&
				(pipeline_task[i] == promote_task) &&
				(pipeline_task[i]->pid == promote_task->pid)) {
				promote_ots = pipeline_ots[i];
				if (unlikely(atomic_read(&promote_ots->pipeline_cpu) == nr_cpu_ids))
					break;

				atomic_set(&prime_ots->pipeline_cpu, atomic_read(&promote_ots->pipeline_cpu));
				atomic_set(&promote_ots->pipeline_cpu, nr_cpu_ids - 1);
				prime_task = promote_task;
				prime_ots = promote_ots;

				if (unlikely(global_debug_enabled & DEBUG_PIPELINE)) {
					systrace_c_printk("swap_prime_task", 1);
					systrace_c_printk("swap_prime_task", 0);
				}

				break;
			}
		}

unlock:
		raw_spin_unlock_irqrestore(&pipeline_lock, flags);
	}
}

void mtk_rearrange_pipeline_preferred_cpus(struct task_struct *p, const struct cpumask *in_mask)
{
	u64 now_ns;
	unsigned long flags;

	if ((pipeline_task_nr <= 1) || (prime_task == NULL))
		return;

	if (oplus_get_task_pipeline_cpu(p) == -1)
		return;

	if (unlikely(global_debug_enabled & DEBUG_PIPELINE)) {
		printk("%s: c_comm=%s, c_pid=%d, c_tgid=%d, comm=%s, pid=%d, tgid=%d, in_mask=%*pbl\n",
			__func__, current->comm, current->pid, current->tgid, p->comm, p->pid, p->tgid, cpumask_pr_args(in_mask));
	}

	if (raw_spin_trylock_irqsave(&mtk_rearrange_lock, flags)) {
		now_ns = ktime_get_raw();
		if ((now_ns - last_rearrange_begin_time > REARRANGE_INTERVAL_NS) ||
			(rearrange_thread_pid != current->pid)) {
			last_rearrange_begin_time = now_ns;
			rearrange_thread_pid = current->pid;
			demote_prime_task = false;
			promote_task = NULL;
			rearrange_done = false;
		}

		if (rearrange_done)
			goto unlock;

		if (p == prime_task) {
			if (cpumask_test_cpu(nr_cpu_ids - 1, in_mask)) {
				rearrange_done = true;
			} else {
				demote_prime_task = true;
				if (promote_task) {
					pipeline_swap_prime_task();
					rearrange_done = true;
				}
			}
		} else {
			if (!cpumask_test_cpu(0, in_mask) &&
				cpumask_test_cpu(nr_cpu_ids - 1, in_mask)) {
				promote_task = p;
				if (demote_prime_task) {
					pipeline_swap_prime_task();
					rearrange_done = true;
				}
			}
		}

unlock:
		raw_spin_unlock_irqrestore(&mtk_rearrange_lock, flags);
	}
}
EXPORT_SYMBOL_GPL(mtk_rearrange_pipeline_preferred_cpus);
#endif

bool oplus_pipeline_task_skip_ux_change(struct oplus_task_struct *ots, int *ux_state)
{
	/* no pipeline task, simply return */
	if (!is_valid_pipeline_task(atomic_read(&ots->pipeline_cpu), ots->ux_state))
		return false;

	/* pipeline task, not allowed to set ux_state 0 */
	if (*ux_state == 0)
		return true;

	/* pipeline task, not allowed to change ux_state, unless add SA_TYPE_INHERIT */
	*ux_state &= ~SCHED_ASSIST_UX_MASK;
	*ux_state &= ~SCHED_ASSIST_UX_PRIORITY_MASK;
	*ux_state |= PIPELINE_TASK_UX_STATE;

	return false;
}

/*
 * handle the problem of two or more pipeline tasks grabbing prime cpu.
 */
bool oplus_pipeline_task_skip_cpu(struct task_struct *task, unsigned int dst_cpu)
{
	struct task_struct *dst_task;
	struct oplus_task_struct *ots;
	int pipeline_cpu;
	bool skip = true;

	if (unlikely(!global_sched_assist_enabled))
		return false;

	if ((prime_ots == NULL) || (dst_cpu != nr_cpu_ids - 1))
		return false;

	/* rt task */
	if (task->prio < MAX_RT_PRIO)
		return false;

	dst_task = cpu_rq(dst_cpu)->curr;
	if (task == dst_task)
		return false;

	ots = get_oplus_task_struct(task);
	if (IS_ERR_OR_NULL(ots))
		return false;

	pipeline_cpu = atomic_read(&ots->pipeline_cpu);
	if (!is_valid_pipeline_task(pipeline_cpu, ots->ux_state))
		return false;

#if IS_ENABLED(CONFIG_SCHED_WALT)
	if (pipeline_prime_rearrange) {
		if ((pipeline_cpu == dst_cpu) && !oplus_is_pipeline_task(dst_task))
			skip = false;
	} else {
		if (available_idle_cpu(dst_cpu) &&
			(pipeline_task_util(task) - pipeline_task_util(dst_task) >
			(PIPELINE_SWAP_UTIL_DIFF << 1)))
			skip = false;
	}
#else
	if ((pipeline_cpu == dst_cpu) && !oplus_is_pipeline_task(dst_task))
		skip = false;
#endif

	if (unlikely(global_debug_enabled & DEBUG_PIPELINE)) {
		char buf[64];

		snprintf(buf, sizeof(buf), "%s_RunOnCpu%d_%s\n",
			task->comm, dst_cpu, skip ? "refused" : "allowed");
		systrace_c_printk(buf, 1);
		systrace_c_printk(buf, 0);
	}

	return skip;
}
EXPORT_SYMBOL_GPL(oplus_pipeline_task_skip_cpu);

core_ctl_set_boost_t oplus_core_ctl_set_boost = NULL;
EXPORT_SYMBOL_GPL(oplus_core_ctl_set_boost);
core_ctl_set_cluster_boost_t oplus_core_ctl_set_cluster_boost = NULL;
EXPORT_SYMBOL_GPL(oplus_core_ctl_set_cluster_boost);

static cpumask_t cpus_for_pipeline = { CPU_BITS_NONE };
static inline void pipeline_set_boost(bool boost)
{
	int i, j, cpu;
	struct cpumask pipeline_cpu_mask;
	struct cpufreq_policy policy;
	struct cpu_topology *cpu_topo;

	if (oplus_core_ctl_set_boost != NULL) {
		oplus_core_ctl_set_boost(boost);
		printk("oplus_core_ctl_set_boost, boost=%d\n", boost);
		return;
	}

	cpumask_copy(&pipeline_cpu_mask, &cpus_for_pipeline);

	if (oplus_core_ctl_set_cluster_boost != NULL) {
		for_each_cpu(i, &pipeline_cpu_mask) {
			if (cpufreq_get_policy(&policy, i))
				continue;

			for_each_cpu(j, policy.related_cpus)
				cpumask_clear_cpu(j, &pipeline_cpu_mask);

			cpu = policy.cpu;

			cpu_topo = &cpu_topology[cpu];
			if (cpu_topo->package_id == -1)
				continue;

			oplus_core_ctl_set_cluster_boost(cpu_topo->package_id, boost);
			printk("oplus_core_ctl_set_cluster_boost, cluster_id=%d, boost=%d\n",
				cpu_topo->package_id, boost);
		}
	}
}

static ssize_t pipeline_pids_proc_write(struct file *file,
			const char __user *buf, size_t count, loff_t *ppos)
{
	char buffer[256] = {0};
	int pids[MAX_PIPELINE_TASK_NUM] = {-1, -1, -1, -1, -1};
	int cpus[MAX_PIPELINE_TASK_NUM] = {-1, -1, -1, -1, -1};
	int ret;
	int i;
	struct task_struct *task;
	struct oplus_task_struct *ots;
	unsigned long flags;

	ret = simple_write_to_buffer(buffer, sizeof(buffer) - 1, ppos, buf, count);
	if (ret <= 0)
		return ret;

	ret = sscanf(buffer, "%d %d %d %d %d %d %d %d %d %d", &pids[0], &cpus[0],
			&pids[1], &cpus[1], &pids[2], &cpus[2],
			&pids[3], &cpus[3], &pids[4], &cpus[4]);

	if (ret < 0)
		return -EINVAL;

	for (i = 0; i < MAX_PIPELINE_TASK_NUM; i++) {
		if (!((cpus[i] == -1) || ((cpus[i] > 0) && (cpus[i] < nr_cpu_ids))))
			return -EINVAL;
	}

	mutex_lock(&p_mutex);

	raw_spin_lock_irqsave(&pipeline_lock, flags);
	if (pipeline_task_nr > 0) {
		for (i = 0; i < MAX_PIPELINE_TASK_NUM; i++) {
			if (pipeline_task[i]) {
				task = pipeline_task[i];
				ots = pipeline_ots[i];
				atomic_set(&ots->pipeline_cpu, -1);
				oplus_set_ux_state_lock(task, 0, true);

				/* get_pid_task have called get_task_struct, now call put_task_struct */
				put_task_struct(task);
			}

			pipeline_pids[i] = -1;
			pipeline_cpus[i] = -1;
			pipeline_task[i] = NULL;
			pipeline_ots[i] = NULL;
		}

		prime_task = NULL;
		prime_ots = NULL;
		pipeline_task_nr = 0;
	}
	raw_spin_unlock_irqrestore(&pipeline_lock, flags);

	if (!cpumask_empty(&cpus_for_pipeline)) {
		pipeline_set_boost(false);
		cpumask_clear(&cpus_for_pipeline);
	}

	if (pids[0] != -1) {
		raw_spin_lock_irqsave(&pipeline_lock, flags);
		for (i = 0; i < MAX_PIPELINE_TASK_NUM; i++) {
			if (pids[i] == -1)
				continue;

			/* get_pid_task have called get_task_struct */
			task = get_pid_task(find_vpid(pids[i]), PIDTYPE_PID);
			if (task) {
				ots = get_oplus_task_struct(task);
				if (!IS_ERR_OR_NULL(ots)) {
					oplus_set_ux_state_lock(task, PIPELINE_TASK_UX_STATE, true);
					if (cpus[i] != -1)
						atomic_set(&ots->pipeline_cpu, cpus[i]);
					else
						atomic_set(&ots->pipeline_cpu, nr_cpu_ids);
					pipeline_pids[i] = pids[i];
					pipeline_cpus[i] = cpus[i];
					pipeline_task[i] = task;
					pipeline_ots[i] = ots;

					/* assumes just one prime */
					if (cpus[i] == nr_cpu_ids - 1) {
						prime_task = task;
						prime_ots = ots;
					}

					pipeline_task_nr++;

					if (cpus[i] != -1)
						cpumask_set_cpu(cpus[i], &cpus_for_pipeline);
				} else {
					/* get_pid_task have called get_task_struct */
					put_task_struct(task);
				}
			}
		}
		raw_spin_unlock_irqrestore(&pipeline_lock, flags);

		if (!cpumask_empty(&cpus_for_pipeline))
			pipeline_set_boost(true);
	}

	mutex_unlock(&p_mutex);

	return count;
}

static ssize_t pipeline_pids_proc_read(struct file *file,
			char __user *buf, size_t count, loff_t *ppos)
{
	char buffer[256] = {0};
	int len;

	mutex_lock(&p_mutex);
	len = snprintf(buffer, sizeof(buffer), "%d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\n",
		pipeline_pids[0], pipeline_cpus[0], pipeline_pids[1], pipeline_cpus[1],
		pipeline_pids[2], pipeline_cpus[2], pipeline_pids[3], pipeline_cpus[3],
		pipeline_pids[4], pipeline_cpus[4]);
	mutex_unlock(&p_mutex);

	return simple_read_from_buffer(buf, count, ppos, buffer, len);
}

static const struct proc_ops pipeline_pids_proc_ops = {
	.proc_write		= pipeline_pids_proc_write,
	.proc_read		= pipeline_pids_proc_read,
	.proc_lseek		= default_llseek,
};

#if IS_ENABLED(CONFIG_SCHED_WALT)
static ssize_t pipeline_prime_proc_write(struct file *file,
			const char __user *buf, size_t count, loff_t *ppos)
{
	char buffer[32] = {0};
	int ret;
	int rearrange;

	ret = simple_write_to_buffer(buffer, sizeof(buffer) - 1, ppos, buf, count);
	if (ret <= 0)
		return ret;

	ret = sscanf(buffer, "%d", &rearrange);
	if (ret != 1)
		return -EINVAL;

	mutex_lock(&p_mutex);
	pipeline_prime_rearrange = !!rearrange;
	mutex_unlock(&p_mutex);

	return count;
}

static ssize_t pipeline_prime_proc_read(struct file *file,
			char __user *buf, size_t count, loff_t *ppos)
{
	char buffer[32] = {0};
	int len;

	mutex_lock(&p_mutex);
	len = sprintf(buffer, "%d\n", pipeline_prime_rearrange? 1 : 0);
	mutex_unlock(&p_mutex);

	return simple_read_from_buffer(buf, count, ppos, buffer, len);
}

static const struct proc_ops pipeline_prime_proc_ops = {
	.proc_write		= pipeline_prime_proc_write,
	.proc_read		= pipeline_prime_proc_read,
	.proc_lseek		= default_llseek,
};
#endif

void oplus_pipeline_init(struct proc_dir_entry *pde)
{
	proc_create("pipeline_pids_cpus", (S_IRUGO|S_IWUSR|S_IWGRP), pde, &pipeline_pids_proc_ops);
#if IS_ENABLED(CONFIG_SCHED_WALT)
	proc_create("pipeline_prime_rearrange", (S_IRUGO|S_IWUSR|S_IWGRP), pde, &pipeline_prime_proc_ops);
#endif
}
