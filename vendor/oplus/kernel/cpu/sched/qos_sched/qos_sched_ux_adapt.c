// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025 Oplus. All rights reserved.
 */

#include "qos_sched.h"
#include "qos_sched_lut.h"
#include "../sched_assist/sa_common.h"
#include "../sched_assist/sa_sysfs.h"
#include "trace_qos_sched.h"

#define UX_VALUE_MASK                           (0xFFFFFFFF)
#define GET_UX_VALUE_BY_LATENCY(qos_latency)    (qos_latency & UX_VALUE_MASK)

/*
void qos_sched_set_ux(struct qos_sched_ioctl_data *data, qos_ux_adapt_type type)
{
	u64 qos_latency = 0;
	u32 ux_state = 0;
	int pid;

	qos_latency = qos_sched_get_latecny_by_qos_level(data->level);
	ux_state = GET_UX_VALUE_BY_LATENCY(qos_latency);
	pid = data->info.pid;
	ux_task_set(ux_state, pid);
}
*/
