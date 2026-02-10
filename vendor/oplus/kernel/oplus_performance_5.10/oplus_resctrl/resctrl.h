// SPDX-License-Identifier: GPL-2.0-only
/*
* Copyright (C) 2023 Oplus. All rights reserved.
*/
#ifndef _RESCTRL_H_
#define _RESCTRL_H_
#include <linux/compiler_types.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/blkdev.h>
#include <linux/version.h>

#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a, b, c)                                                \
	(((a) << 16) + ((b) << 8) + ((c) > 255 ? 255 : (c)))
#endif

#ifndef LINUX_VERSION_CODE
#error "Need LINUX_VERSION_CODE"
#endif

#define LINUX_KERNEL_601                                                       \
	((LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)) &&                   \
	 (LINUX_VERSION_CODE < KERNEL_VERSION(6, 1, 256)))
#define LINUX_KERNEL_515                                                       \
	((LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)) &&                   \
	 (LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 256)))
#define LINUX_KERNEL_510                                                       \
	((LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)) &&                   \
	 (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 256)))
#define LINUX_KERNEL_504                                                       \
	((LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)) &&                    \
	 (LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 256)))
#define LINUX_KERNEL_419                                                       \
	((LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)) &&                   \
	 (LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 256)))

#if (!LINUX_KERNEL_601 && !LINUX_KERNEL_515 && !LINUX_KERNEL_510 && !LINUX_KERNEL_419 &&            \
	 !LINUX_KERNEL_504)
#error "Kernel version not supported"
#endif

enum {
	RESCTRL_LOG_LEVEL_ERROR,
	RESCTRL_LOG_LEVEL_WARN,
	RESCTRL_LOG_LEVEL_NOTICE,
	RESCTRL_LOG_LEVEL_INFO,
	RESCTRL_LOG_LEVEL_DEBUG,
	RESCTRL_LOG_LEVEL_TEMP,
};

/*
 * we know min is 4K and max is 512K
 * but we still keep the 1 and other col
 */
#define IOC_DIST_TIMESTAT 21
#define IOC_DIST_SEGMENTS 10

#define RESCTRL_READ 0
#define RESCTRL_WRITE 1

struct ioc_dist_rw {
	u64 dist[IOC_DIST_SEGMENTS][IOC_DIST_TIMESTAT];
	u64 high;
	u64 last_sum;
	u64 last_high;
};

struct ioc_dist_stat {
	struct ioc_dist_rw rw[2];
};

struct iocost_config {
	struct timer_list timer;
	u32 read_lat;
	u32 write_lat;
	u32 ppm_start;
};

extern struct iocost_config iocost_config;

__printf(3, 4) void resctrl_printk(int log_level, const char *func,
				   const char *fmt, ...);

#define resctrl_err(fmt, ...)                                                  \
	resctrl_printk(RESCTRL_LOG_LEVEL_ERROR, __func__, KERN_ERR fmt,        \
		       ##__VA_ARGS__)
#define resctrl_warn(fmt, ...)                                                 \
	resctrl_printk(RESCTRL_LOG_LEVEL_WARN, __func__, KERN_WARNING fmt,     \
		       ##__VA_ARGS__)
#ifdef CONFIG_RESCTRL_DEBUG
#define resctrl_notice(fmt, ...)                                               \
	resctrl_printk(RESCTRL_LOG_LEVEL_NOTICE, __func__, KERN_NOTICE fmt,    \
		       ##__VA_ARGS__)
#define resctrl_info(fmt, ...)                                                 \
	resctrl_printk(RESCTRL_LOG_LEVEL_INFO, __func__, KERN_INFO fmt,        \
		       ##__VA_ARGS__)
#define resctrl_debug(fmt, ...)                                                \
	resctrl_printk(RESCTRL_LOG_LEVEL_DEBUG, __func__, KERN_DEBUG fmt,      \
		       ##__VA_ARGS__)
#define resctrl_temp(fmt, ...)                                                 \
	resctrl_printk(RESCTRL_LOG_LEVEL_TEMP, __func__, KERN_DEBUG fmt,       \
		       ##__VA_ARGS__)
#else
#define resctrl_notice(sbi, fmt, ...)                                          \
	do {                                                                   \
	} while (0)
#define resctrl_info(sbi, fmt, ...)                                            \
	do {                                                                   \
	} while (0)
#define resctrl_debug(sbi, fmt, ...)                                           \
	do {                                                                   \
	} while (0)
#define resctrl_temp(sbi, fmt, ...)                                            \
	do {                                                                   \
	} while (0)
#endif

#define REGISTER_TRACE_VH(vender_hook, handler)                                \
{                                                                      \
	ret = register_trace_##vender_hook(handler, NULL);             \
	if (ret) {                                                     \
		pr_info("failed to register_" #vender_hook             \
			", ret=%d\n",                                  \
			ret);                                          \
		return ret;                                            \
	}                                                              \
}

#define UNREGISTER_TRACE_VH(vender_hook, handler)                              \
{                                                                      \
	ret = unregister_trace_##vender_hook(handler, NULL);           \
	pr_info("%s handler register success", #handler);              \
}

static inline ssize_t sched_data_to_user(char __user *buff, size_t count,
					 loff_t *off, char *format_str, int len)
{
	if (len > *off)
		len -= *off;
	else
		len = 0;
	if (copy_to_user(buff, format_str, (len < count ? len : count)))
		return -EFAULT;
	*off += len < count ? len : count;

	return (len < count ? len : count);
}

void android_vh_blk_account_io_done_handler(void *unused, struct request *rq);

int  ioc_iop_read(void);
void ioc_dist_clear_stat(void);
void ioc_dist_get_stat(int rw, u64 *request, int time_dist);
void ioc_timer_fn(struct timer_list *timer);

static inline bool ioc_high_read(u32 value) {
	if (iocost_config.read_lat == 0)
		return false;

	if (value > iocost_config.read_lat)
		return true;
	return false;
}

static inline bool ioc_high_write(u32 value) {
	if (iocost_config.write_lat == 0)
		return false;

	if (value > iocost_config.write_lat)
		return true;
	return false;
}

#endif /* _RESCTRL_H_ */
