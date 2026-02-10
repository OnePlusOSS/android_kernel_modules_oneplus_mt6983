/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2025 Oplus. All rights reserved.
*/
#ifndef _OPLUS_QOS_UX_ADAPT_H
#define _OPLUS_QOS_UX_ADAPT_H

#include "qos_sched_lut.h"
#include "qos_sched.h"

typedef enum __qos_ux_adapt_type {
	QOS_UX_ADAPT_THREAD = 0,
	QOS_UX_ADAPT_PROCESS,
}qos_ux_adapt_type;

void qos_sched_set_ux(struct qos_sched_ioctl_data *data, qos_ux_adapt_type type);

#endif /* _OPLUS_QOS_UX_ADAPT_H */
