/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
 */
#ifndef AUDIO_SWITCH_H
#define AUDIO_SWITCH_H

#include <linux/of.h>
#include <linux/notifier.h>

enum usbc_switch_event {
	HS_MIC_GND_SWAP = 0,
	HS_LR_CONNECT,
	HS_LR_DISCONNECT,
	HS_PLUG_OUT,
	HS_SWITCH_EVENT_MAX
};


#if IS_ENABLED(CONFIG_OPLUS_SEPARATE_USBC_SWITCH)
int usbc_switch_hs_event(enum usbc_switch_event event);
int usbc_switch_reg_notifier(struct notifier_block *nb,
			struct device_node *node);
int usbc_switch_unreg_notifier(struct notifier_block *nb,
			     struct device_node *node);
#else /* CONFIG_OPLUS_SEPARATE_USBC_SWITCH */

static inline int usbc_switch_hs_event(enum fsa_function event)
{
	return 0;
}

static inline int usbc_switch_reg_notifier(struct notifier_block *nb,
			struct device_node *node)
{
	return 0;
}

#endif /* CONFIG_OPLUS_SEPARATE_USBC_SWITCH */

#endif /* AUDIO_SWITCH_H */

