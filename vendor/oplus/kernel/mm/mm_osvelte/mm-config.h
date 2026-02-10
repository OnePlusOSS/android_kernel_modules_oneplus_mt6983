/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2018-2024 Oplus. All rights reserved.
 * this referenced by android cutil/trace.h
 */
#ifndef _OSVELTE_MM_CONFIG_H
#define _OSVELTE_MM_CONFIG_H

static const char *module_name_uxmem_opt = "oplus_bsp_uxmem_opt";
struct config_oplus_bsp_uxmem_opt {
	bool enable;
};

static const char *module_name_boost_pool = "oplus_boost_pool";
struct config_oplus_boost_pool {
	bool enable;
};

extern int mm_config_init(struct proc_dir_entry *root);
extern int mm_config_exit(void);
extern void *oplus_read_mm_config(const char *module_name);
#endif /* _OSVELTE_MM_CONFIG_H */
