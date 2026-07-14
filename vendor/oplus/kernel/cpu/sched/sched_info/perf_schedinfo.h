#ifndef __PERF_SCHEDINFO_H
#define __PERF_SCHEDINFO_H



enum cpu_state {
	CPU_OFFLINE,
	CPU_PAUSED,
	CPU_NORMAL,
};

#define SCHED_MAX_CPU_STATE 10ULL
#define SCHED_MAX_CFS_R 1000ULL
#define SCHED_MAX_RT_R 10ULL
#define SCHED_MAX_UX_R 10ULL
#define SCHED_MAX_CPU_CAPACITY 10000ULL
#define SCHED_MAX_CPU_UTIL_PCT 100ULL
#define CFS_R_MULT_UNIT (SCHED_MAX_CPU_STATE)
#define RT_R_MULT_UNIT (CFS_R_MULT_UNIT * SCHED_MAX_CFS_R)
#define UX_R_MULT_UNIT (RT_R_MULT_UNIT * SCHED_MAX_RT_R)
#define CPU_CAP_MULT_UNIT (UX_R_MULT_UNIT * SCHED_MAX_UX_R)
#define CPU_UTIL_PCT_MULT_UNIT (CPU_CAP_MULT_UNIT * SCHED_MAX_CPU_CAPACITY)

#define MAX_MSG_SIZE 1024

static inline unsigned long curr_capacity_of(int cpu)
{
	return cpu_rq(cpu)->cpu_capacity;
}

struct proc_dir_entry *perf_schedinfo_init(struct proc_dir_entry *pde);
void perf_schedinfo_exit(struct proc_dir_entry *pde);

#endif