// SPDX-License-Identifier: GPL-2.0-only
/*
* Copyright (C) 2023 Oplus. All rights reserved.
*/
#include "resctrl.h"

#if LINUX_KERNEL_601
#include <linux/blk-mq.h>
#endif

#define IOC_DEF_MIN_IOPS 640
#define IOC_DEF_PPM_HIGH 80
#define IOC_DEF_PPM_LOW (IOC_DEF_PPM_HIGH >> 2)

static int ioc_ppm_high = 0;
struct iocost_config iocost_config = {0};

DEFINE_PER_CPU(struct ioc_dist_stat, ioc_dist_stat) = { { 0 } };
EXPORT_PER_CPU_SYMBOL(ioc_dist_stat);

static void ioc_dist_update_stat(int rw, u64 on_q_us, u64 rq_wait_us,
				 int segments)
{
	int i = 0;
	int j = 0;
	while (segments > 0) {
		if (i >= (IOC_DIST_SEGMENTS - 1))
			break;
		i++;
		segments = segments >> 1;
	}

	if (rw == RESCTRL_READ) {
		if (ioc_high_read(on_q_us))
			this_cpu_inc(ioc_dist_stat.rw[rw].high);
	}
	else {
		if (ioc_high_write(on_q_us))
			this_cpu_inc(ioc_dist_stat.rw[rw].high);
	}

	while (on_q_us > 0) {
		if (j >= (IOC_DIST_TIMESTAT - 1))
			break;
		j++;
		on_q_us = on_q_us >> 1;
	}

	this_cpu_inc(ioc_dist_stat.rw[rw].dist[i][j]);
}

void ioc_dist_clear_stat(void)
{
	int cpu;

	for_each_online_cpu(cpu) {
		struct ioc_dist_stat *this = &per_cpu(ioc_dist_stat, cpu);
		memset(this, 0, sizeof(struct ioc_dist_stat));
	}
}

void ioc_dist_get_stat(int rw, u64 *request, int time_dist)
{
	int cpu;
	int i;

	for_each_online_cpu(cpu) {
		struct ioc_dist_stat *this = &per_cpu(ioc_dist_stat, cpu);

		for (i = 0; i < IOC_DIST_SEGMENTS; i++)
			request[i] += this->rw[rw].dist[i][time_dist];
	}
}

void android_vh_blk_account_io_done_handler(void *unused, struct request *rq)
{
	unsigned int segments;
	int rw = RESCTRL_READ;
	u64 on_q_ns = ktime_get_ns() - rq->start_time_ns;
	u64 rq_wait_ns = 0;
	segments = blk_rq_stats_sectors(rq) >> 3;

	switch (req_op(rq) & REQ_OP_MASK) {
	case REQ_OP_READ:
		rw = RESCTRL_READ;
		break;
	case REQ_OP_WRITE:
		rw = RESCTRL_WRITE;
		break;
	default:
		resctrl_debug("resctrl android_vh_blk_account_io_done_handler op=%d, error no support!",
					req_op(rq));
		return;
	}

	trace_printk("resctrl rq_qos_done rw=%d req_op=0x%x on_q=%lluus rq_wait=%lluus segments=%u, %d\n",
				rw, req_op(rq), on_q_ns / 1000, rq_wait_ns / 1000, segments,
				blk_rq_sectors(rq));
	/* update the stat */
	ioc_dist_update_stat(rw, on_q_ns / 1000, rq_wait_ns / 1000, segments);
	return;
}

static void ioc_lat_calc(u64 *nr_high, u64 *nr_all)
{
	int cpu, i, j, rw;

	for_each_online_cpu(cpu) {
		struct ioc_dist_stat *this = &per_cpu(ioc_dist_stat, cpu);

		for (rw = RESCTRL_READ; rw <= RESCTRL_WRITE; rw++) {
			u64 this_all = 0;
			u64 this_high = this->rw[rw].high;

			for (i = 0; i < IOC_DIST_SEGMENTS; i++) {
				for (j = 0; j < IOC_DIST_TIMESTAT; j++) {
					this_all += this->rw[rw].dist[i][j];
				}
			}

			nr_all[rw] += this_all - this->rw[rw].last_sum;
			nr_high[rw] += this_high - this->rw[rw].last_high;

			this->rw[rw].last_sum = this_all;
			this->rw[rw].last_high = this_high;
		}
	}
}

void ioc_timer_fn(struct timer_list *timer)
{
	u64 nr_high[2] = {0};
	u64 nr_all[2] = {0};

	ioc_lat_calc(nr_high, nr_all);
	trace_printk("### nr_high=%llu, nr_all=%llu ppm=%d\n", nr_high[0], nr_all[0], ioc_ppm_high);
	if (nr_all[0] >= IOC_DEF_MIN_IOPS) {
		if ((nr_high[0]*100/nr_all[0])  >= IOC_DEF_PPM_HIGH) {
			if (ioc_ppm_high == 0) {
				printk("resctrl set high ppm\n");
				ioc_ppm_high = 1;
			}
		} else if ((nr_high[0] * 100 /nr_all[0])  < IOC_DEF_PPM_LOW) {
			if (ioc_ppm_high != 0) {
				printk("resctrl set low ppm\n");
				ioc_ppm_high = 0;
			}
		}
	} else {
		if (ioc_ppm_high != 0) {
			printk("resctrl clear high ppm\n");
			ioc_ppm_high = 0;
		}
	}

	if (iocost_config.ppm_start) {
		iocost_config.timer.expires = jiffies + 1*HZ;
		add_timer(&iocost_config.timer);
	}
}

int ioc_iop_read(void)
{
	return ioc_ppm_high;
}
