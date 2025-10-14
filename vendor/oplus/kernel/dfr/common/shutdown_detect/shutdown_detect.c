// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2018-2020 Oplus. All rights reserved.
 */
/**********************************************************************************
* Description:     shutdown_detect Monitor  Kernel Driver
*
* Version   : 1.0
***********************************************************************************/
#define FILP_OPEN_FUNCTION_CLOSE 0

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/sched.h>
#include <linux/sched/rt.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/reboot.h>
#include <linux/sysrq.h>
#include <linux/kbd_kern.h>
#include <linux/proc_fs.h>
#include <linux/nmi.h>
#include <linux/quotaops.h>
#include <linux/perf_event.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/suspend.h>
#include <linux/writeback.h>
#include <linux/swap.h>
#include <linux/spinlock.h>
#include <linux/vt_kern.h>
#include <linux/workqueue.h>
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include <linux/oom.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/uaccess.h>
#include <linux/moduleparam.h>
#include <linux/jiffies.h>
#include <linux/syscalls.h>
#include <linux/of.h>
#include <linux/rcupdate.h>
#include <linux/kthread.h>

#include <asm/ptrace.h>
#include <asm/irq_regs.h>

#include <linux/sysrq.h>
#include <linux/clk.h>

#include <linux/kmsg_dump.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
#include <linux/timekeeping.h>
#endif

#include <linux/rtc.h>
#include <linux/ktime.h>
#include <linux/sched/clock.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
#include <linux/wakelock.h>
#endif
#include <linux/pm_wakeup.h>
#include <soc/oplus/system/oplus_project.h>

#if IS_ENABLED (CONFIG_OPLUS_BSP_DFR_USERSPACE_BACKTRACE)
#include <soc/oplus/dfr/oplus_bsp_dfr_ubt.h>
#endif /* CONFIG_OPLUS_BSP_DFR_USERSPACE_BACKTRACE */

#include <linux/blkdev.h>
#include <linux/notifier.h>

#define SEQ_printf(m, x...)                                                    \
	do {                                                                   \
		if (m)                                                         \
			seq_printf(m, x);                                      \
		else                                                           \
			pr_debug(x);                                           \
	} while (0)

#define OPLUS_SHUTDOWN_LOG_START_BLOCK_EMMC 10240
#define OPLUS_SHUTDOWN_LOG_START_BLOCK_UFS 1280
#define OPLUS_SHUTDOWN_KERNEL_LOG_SIZE_BYTES 1024 * 1024
#define OPLUS_SHUTDOWN_FLAG_OFFSET 0 * 1024 * 1024
#define OPLUS_SHUTDOWN_KMSG_OFFSET 61 * 1024 * 1024
#define FILE_MODE_0666 0666

#define BLOCK_SIZE_EMMC 512
#define BLOCK_SIZE_UFS 4096

#define SHUTDOWN_MAGIC "ShutDown"
#define SHUTDOWN_MAGIC_LEN 16

#define ShutDownTO 0x9B

#define TASK_INIT_COMM "init"

#define OPLUS_PARTITION_OPLUSRESERVE3_LINK "/dev/block/by-name/oplusreserve3"

#define ST_LOG_NATIVE_HELPER "/system/bin/phoenix_log_native_helper.sh"

#define SIG_SHUTDOWN (SIGRTMIN + 0x12)

#define SHUTDOWN_STAGE_KERNEL 20
#define SHUTDOWN_STAGE_INIT 30
#define SHUTDOWN_STAGE_SYSTEMSERVER 40
#define SHUTDOWN_TIMEOUNT_UMOUNT 31
#define SHUTDOWN_TIMEOUNT_VOLUME 32
#define SHUTDOWN_TIMEOUNT_SUBSYSTEM 43
#define SHUTDOWN_TIMEOUNT_RADIOS 44
#define SHUTDOWN_TIMEOUNT_PM 45
#define SHUTDOWN_TIMEOUNT_AM 46
#define SHUTDOWN_TIMEOUNT_BC 47
#define SHUTDOWN_STAGE_INIT_POFF 70
#define SHUTDOWN_STAGE_HARDWARE 100
#define SHUTDOWN_STAGE_UNKNOWN 101
#define SHUTDOWN_TRIGGER_RESTORE 102
#define SHUTDOWN_TRIGGER_STORE 103
#define SHUTDOWN_RUS_MIN 255
#define SHUTDOWN_TOTAL_TIME_MIN 60
#define SHUTDOWN_DEFAULT_NATIVE_TIME 60
#define SHUTDOWN_DEFAULT_JAVA_TIME 60
#define SHUTDOWN_DEFAULT_TOTAL_TIME 90
#define SHUTDOWN_INCREASE_TIME 5
#define SHUTDOWN_DELAY_ENABLE 192
#define SHUTDOWN_DELAY_DISABLE 128
#define KE_LOG_COLLECT_TIMEOUT msecs_to_jiffies(10000)

/* Used to convert the name of a macro definition into a string */
#define TO_STRING(x) #x

#define PATH_OPLUS_RESERVE_1 "/dev/block/by-name/oplusreserve1"
#define PARTLABEL_OPLUS_RESERVE_1 "PARTLABEL=oplusreserve1"

#define OPLUS_RESERVE1_SHUDOWN_RECORD_UFS_OFFSET (1305 * 4096)
#define OPLUS_RESERVE1_SHUDOWN_RECORD_EMMC_OFFSET (9952 * 512)

#define RETRY_COUNT_FOR_GET_DEVICE 3
#define WAITING_FOR_GET_DEVICE 1000

#define COUNT_SHUTDOWN_STAGE 5

static bool ufs_flag = true;

static struct kmsg_dumper shutdown_kmsg_dumper;

static DECLARE_COMPLETION(shd_comp);
static DEFINE_MUTEX(shd_wf_mutex);

static unsigned int shutdown_phase;
static bool shutdown_detect_started = false;
static bool shutdown_detect_enable = true;
static bool is_shutdows = false;
static unsigned int gtimeout = 0;
static unsigned int gtotaltimeout = SHUTDOWN_DEFAULT_TOTAL_TIME;
static unsigned int gjavatimeout = SHUTDOWN_DEFAULT_JAVA_TIME;
static unsigned int gnativetimeout = SHUTDOWN_DEFAULT_NATIVE_TIME;
static struct task_struct *shutdown_task = NULL;
struct task_struct *shd_complete_monitor = NULL;

static struct timer_list shutdown_timer;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
struct wake_lock shutdown_wakelock;
#else
struct wakeup_source *shutdown_ws;
#endif

struct shd_info {
	char magic[SHUTDOWN_MAGIC_LEN];
	int shutdown_err;
	int shutdown_times;
};

#define SIZEOF_STRUCT_SHD_INFO sizeof(struct shd_info)

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
time_t shutdown_start_time = 0;
time_t shutdown_end_time = 0;
time_t shutdown_systemserver_start_time = 0;
time_t shutdown_init_start_time = 0;
time_t shutdown_kernel_start_time = 0;
#else
unsigned long long shutdown_start_time = 0;
unsigned long long shutdown_end_time = 0;
unsigned long long shutdown_systemserver_start_time = 0;
unsigned long long shutdown_init_start_time = 0;
unsigned long long shutdown_kernel_start_time = 0;
#endif

static int shutdown_kthread(void *data)
{
	kernel_power_off();
	return 0;
}

unsigned int g_shutdown_flag = 0;
EXPORT_SYMBOL(g_shutdown_flag);

/* This flag indicates that the checkpoint data from the last shutdown has been successfully restored */
static int flag_restored = 0;

static int shutdown_detect_func(void *dummy);

static void shutdown_timeout_flag_write(int timeout);
static void shutdown_dump_kernel_log(void);
static int shutdown_timeout_flag_write_now(void *args);
#if IS_ENABLED (CONFIG_OPLUS_BSP_DFR_USERSPACE_BACKTRACE)
static void shutdown_dump_ubt(void);
#endif /* CONFIG_OPLUS_BSP_DFR_USERSPACE_BACKTRACE */
static void try_restore(void);

extern int creds_change_dac(void);
extern int shutdown_kernel_log_save(void *args);
extern void shutdown_dump_android_log(void);

struct shutdown_stage_struct {
	long stage_id;
	unsigned long long uptime; /* Milliseconds since the current boot */
	unsigned long long utc; /* Seconds since 1970/01/01 */
};

struct shutdown_record_struct {
	char magic[SHUTDOWN_MAGIC_LEN];
	unsigned long action; /* Refer to `SYS_RESTART|SYS_HALT|SYS_POWER_OFF` in `include/linux/kernel.h` */
	struct shutdown_stage_struct stages[COUNT_SHUTDOWN_STAGE];
};

static struct shutdown_record_struct last_shutdown_record = {0};

static struct shutdown_record_struct current_shutdown_record = {
	.magic = SHUTDOWN_MAGIC,
	.stages = {
		{ SHUTDOWN_STAGE_SYSTEMSERVER },
		{ SHUTDOWN_STAGE_INIT }, /* Note: Used in reboot scenarios */
		{ SHUTDOWN_STAGE_INIT_POFF }, /* Note: Used in shutdown scenarios */
		{ SHUTDOWN_STAGE_KERNEL },
		{ SHUTDOWN_STAGE_HARDWARE }
	}
};

#define LENGTH_SHUTDOWN_RECORD sizeof(current_shutdown_record)

/* This function is copied from the function with the same name in oplus_phoenix/oplus_kmsg_wb.c */
static struct block_device *get_reserve_partition_bdev(void) {
	struct block_device *bdev = NULL;
	int retry_wait_for_device = RETRY_COUNT_FOR_GET_DEVICE;
	dev_t dev;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
	int ret = 0;
#endif

	while (retry_wait_for_device--) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
		ret = lookup_bdev(PATH_OPLUS_RESERVE_1, &dev);
		if (ret) {
			pr_err("failed to get bdev! ret = %d\n", ret);
			msleep_interruptible(WAITING_FOR_GET_DEVICE);
			continue;
		}
#else
		dev = name_to_dev_t(PARTLABEL_OPLUS_RESERVE_1);
#endif
		if (dev != 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
			bdev = blkdev_get_by_dev(dev, BLK_OPEN_READ | BLK_OPEN_WRITE | BLK_OPEN_EXCL, THIS_MODULE, NULL);
#else
			bdev = blkdev_get_by_dev(dev, FMODE_READ | FMODE_WRITE | FMODE_EXCL, THIS_MODULE);
#endif
			if (!IS_ERR(bdev)) {
				pr_err("success to get dev block\n");
				return bdev;
			}
		}
		pr_err("Failed to get dev block, retry %d\n", retry_wait_for_device);
		msleep_interruptible(WAITING_FOR_GET_DEVICE);
	}
	pr_err("Failed to get dev block final\n");
	return NULL;
}

/*
 * This function is copied from the function `read_header` in oplus_phoenix/oplus_kmsg_wb.c.
 * The return value of 0 indicates success, while any other value indicates failure.
 */
static int read_flash(struct block_device *bdev, loff_t ki_pos, void *iov_base, size_t iov_len) {
	struct file dev_map_file;
	struct kiocb kiocb;
	struct iov_iter iter;
	struct kvec iov;
	int read_size = 0;

	memset(&dev_map_file, 0, sizeof(struct file));

	dev_map_file.f_mapping = bdev->bd_inode->i_mapping;
	dev_map_file.f_flags = O_DSYNC | __O_SYNC | O_NOATIME;
	dev_map_file.f_inode = bdev->bd_inode;

	init_sync_kiocb(&kiocb, &dev_map_file);
	kiocb.ki_pos = ki_pos;
	iov.iov_base = iov_base;
	iov.iov_len = iov_len;
	iov_iter_kvec(&iter, READ, &iov, 1, iov_len);

	read_size = generic_file_read_iter(&kiocb, &iter);
	if (read_size <= 0) {
		pr_err("generic_file_read_iter failed, read_size=%d\n", read_size);
		return read_size;
	}

	return 0;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(5, 15, 0)
static int blkdev_fsync(struct file *filp, loff_t start, loff_t end,
						int datasync) {
	struct inode *bd_inode = filp->f_mapping->host;
	struct block_device *bdev = I_BDEV(bd_inode);
	int error;

	error = file_write_and_wait_range(filp, start, end);
	if (error)
		return error;

	/*
	 * There is no need to serialise calls to blkdev_issue_flush with
	 * i_mutex and doing so causes performance issues with concurrent
	 * O_SYNC writers to a block device.
	 */
	error = blkdev_issue_flush(bdev);
	if (error == -EOPNOTSUPP)
		error = 0;

	return error;
}
#endif

/*
 * This function is copied from the function `write_header` in oplus_phoenix/oplus_kmsg_wb.c.
 * The return value of 0 indicates success, while any other value indicates failure.
 */
static int write_flash(struct block_device *bdev, loff_t ki_pos, void *iov_base, size_t iov_len) {
	struct kiocb kiocb;
	struct iov_iter iter;
	struct kvec iov;
	const struct file_operations f_op = {.fsync = blkdev_fsync};
	struct file dev_map_file;
	int ret = 0;

	memset(&dev_map_file, 0, sizeof(struct file));

	dev_map_file.f_mapping = bdev->bd_inode->i_mapping;
	dev_map_file.f_flags = O_DSYNC | __O_SYNC | O_NOATIME;
	dev_map_file.f_inode = bdev->bd_inode;
#if LINUX_VERSION_CODE > KERNEL_VERSION(6, 1, 0)
	dev_map_file.f_iocb_flags = IOCB_DSYNC;
#endif

	init_sync_kiocb(&kiocb, &dev_map_file);
	kiocb.ki_pos = ki_pos;
	iov.iov_base = iov_base;
	iov.iov_len = iov_len;
	iov_iter_kvec(&iter, WRITE, &iov, 1, iov_len);

	ret = generic_write_checks(&kiocb, &iter);
	if (ret <= 0) {
		pr_err("generic_write_checks failed, ret=%d\n", ret);
		return ret;
	}

#if LINUX_VERSION_CODE > KERNEL_VERSION(6, 1, 0)
	ret = generic_perform_write(&kiocb, &iter);
#else
	ret = generic_perform_write(&dev_map_file, &iter, kiocb.ki_pos);
#endif
	if (ret <= 0) {
		pr_err("generic_perform_write failed, ret=%d\n", ret);
		return ret;
	}

	dev_map_file.f_op = &f_op;
	kiocb.ki_pos += ret;

	ret = generic_write_sync(&kiocb, ret);
	if (ret < 0) {
		pr_err("generic_write_sync failed, ret=%d\n", ret);
		return ret;
	}

	return 0;
}

/* The return value of 0 indicates success, while any other value indicates failure. */
static int clear_last_shutdown_record(struct block_device *bdev) {
	char buffer[LENGTH_SHUTDOWN_RECORD] = {0};
	loff_t ki_pos;

	if (ufs_flag) {
		ki_pos = OPLUS_RESERVE1_SHUDOWN_RECORD_UFS_OFFSET;
	} else {
		ki_pos = OPLUS_RESERVE1_SHUDOWN_RECORD_EMMC_OFFSET;
	}

	return write_flash(bdev, ki_pos, buffer, LENGTH_SHUTDOWN_RECORD);
}

/* The return value of 0 indicates success, while any other value indicates failure. */
static int restore_last_shutdown_record(struct block_device *bdev) {
	loff_t ki_pos;
	int ret;

	if (ufs_flag) {
		ki_pos = OPLUS_RESERVE1_SHUDOWN_RECORD_UFS_OFFSET;
	} else {
		ki_pos = OPLUS_RESERVE1_SHUDOWN_RECORD_EMMC_OFFSET;
	}

	ret = read_flash(bdev, ki_pos, &last_shutdown_record, LENGTH_SHUTDOWN_RECORD);
	if (ret) {
		pr_err("read_flash failed, ret=%d\n", ret);
	} else if (strncmp(last_shutdown_record.magic, SHUTDOWN_MAGIC, sizeof(SHUTDOWN_MAGIC) - 1)) {
		last_shutdown_record.magic[sizeof(SHUTDOWN_MAGIC) - 1] = '\0'; /* Avoid continuous printing */
		pr_err("magic invalid: %s\n", last_shutdown_record.magic);
	} else {
		pr_info("magic valid\n");
	}

	return clear_last_shutdown_record(bdev);
}

/* To get the millisecond value from the start of the kernel boot. */
static unsigned long long get_uptime_ms(void) {
	struct timespec64 ts = {0};
	ktime_get_boottime_ts64(&ts);
	return (unsigned long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static void record_shutdown_stage(long stage_id) {
	size_t i;
	struct timespec64 ts;

	for (i = 0; i < COUNT_SHUTDOWN_STAGE; i++) {
		if (current_shutdown_record.stages[i].stage_id == stage_id) {
			current_shutdown_record.stages[i].uptime = get_uptime_ms();

			ktime_get_real_ts64(&ts);
			current_shutdown_record.stages[i].utc = ts.tv_sec;

			break;
		}
	}
}

/*
 * This function is used in the reboot notifier callback.
 * Note: To ensure other callbacks are not affected, it always returns NOTIFY_DONE
 */
static int store_current_shutdown_record(struct notifier_block *nb, unsigned long action, void *data) {
	loff_t ki_pos;
	struct block_device *bdev;

	record_shutdown_stage(SHUTDOWN_STAGE_HARDWARE);

	current_shutdown_record.action = action;

	bdev = get_reserve_partition_bdev();
	if (!bdev) {
		pr_err("get_reserve_partition_bdev fail, err=%ld\n", PTR_ERR(bdev));

		return NOTIFY_DONE;
	}

	if (ufs_flag) {
		ki_pos = OPLUS_RESERVE1_SHUDOWN_RECORD_UFS_OFFSET;
	} else {
		ki_pos = OPLUS_RESERVE1_SHUDOWN_RECORD_EMMC_OFFSET;
	}

	write_flash(bdev, ki_pos, &current_shutdown_record, LENGTH_SHUTDOWN_RECORD);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
	blkdev_put(bdev, NULL);
#else
	blkdev_put(bdev, FMODE_READ | FMODE_WRITE | FMODE_EXCL);
#endif

	return NOTIFY_DONE;
}

struct notifier_block reboot_nb = {
	.notifier_call = store_current_shutdown_record,
	.priority = INT_MIN, /* The lowest priority, to be called last */
};

static char * get_shutdown_stage_name(long stage_id) {
	switch (stage_id) {
	case SHUTDOWN_STAGE_KERNEL:
		return TO_STRING(SHUTDOWN_STAGE_KERNEL);
	case SHUTDOWN_STAGE_INIT:
		return TO_STRING(SHUTDOWN_STAGE_INIT);
	case SHUTDOWN_STAGE_SYSTEMSERVER:
		return TO_STRING(SHUTDOWN_STAGE_SYSTEMSERVER);
	case SHUTDOWN_STAGE_INIT_POFF:
		return TO_STRING(SHUTDOWN_STAGE_INIT_POFF);
	case SHUTDOWN_STAGE_HARDWARE:
		return TO_STRING(SHUTDOWN_STAGE_HARDWARE);
	default:
		return TO_STRING(SHUTDOWN_STAGE_UNKNOWN);
	}
}

static char * get_action_name(long action) {
	switch (action) {
	case SYS_RESTART:
		return TO_STRING(SYS_RESTART);
	case SYS_HALT:
		return TO_STRING(SYS_HALT);
	case SYS_POWER_OFF:
		return TO_STRING(SYS_POWER_OFF);
	default:
		return TO_STRING(SHUTDOWN_STAGE_UNKNOWN);
	}
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
static struct timespec current_kernel_time(void)
{
	struct timespec64 ts64;

	ktime_get_real_ts64(&ts64);

	return timespec64_to_timespec(ts64);
}
#else
static unsigned long long current_kernel_time(void)
{
        unsigned long long local_time_s;
        local_time_s = local_clock() / 1000000000;
        pr_debug("local_time_s:%llu\n", local_time_s);
        return local_time_s;
}
#endif

static ssize_t shutdown_detect_trigger(struct file *filp, const char *ubuf,
				       size_t cnt, loff_t *data)
{
	char buf[64];
	long val = 0;
	int ret = 0;
	unsigned int shutdown_timeout = 0;

#ifdef OPLUS_BUG_STABILITY
	struct task_struct *tsk = NULL;
#endif
	unsigned int temp = SHUTDOWN_DEFAULT_TOTAL_TIME;

	if (shutdown_detect_enable == false) {
		return -EPERM;
	}

	if (cnt >= sizeof(buf)) {
		return -EINVAL;
	}

	if (copy_from_user(&buf, ubuf, cnt)) {
		return -EFAULT;
	}

	buf[cnt] = 0;

	ret = kstrtoul(buf, 0, (unsigned long *)&val);

	if (ret < 0) {
		return ret;
	}

	record_shutdown_stage(val);

	if (val == SHUTDOWN_STAGE_INIT_POFF) {
		is_shutdows = true;
		val = SHUTDOWN_STAGE_INIT;
	}

	if (OEM_RELEASE != get_eng_version()) {
		gnativetimeout += SHUTDOWN_INCREASE_TIME;
		gjavatimeout += SHUTDOWN_INCREASE_TIME;
	}
#ifdef OPLUS_BUG_STABILITY
	tsk = current->group_leader;
	pr_info("%s:%d shutdown_detect, GroupLeader is %s:%d\n", current->comm,
		task_pid_nr(current), tsk->comm, task_pid_nr(tsk));
#endif /*OPLUS_BUG_STABILITY*/
	/* val: 0x gtotaltimeout|gjavatimeout|gnativetimeout , gnativetimeout < F, gjavatimeout < F */
	if (val > SHUTDOWN_RUS_MIN) {
		gnativetimeout = val % 16;
		gjavatimeout = ((val - gnativetimeout) % 256) / 16;
		temp = val / 256;
		/* for safe */
		gtotaltimeout = (temp < SHUTDOWN_TOTAL_TIME_MIN) ?
					SHUTDOWN_TOTAL_TIME_MIN :
					temp;
		pr_info("shutdown_detect_trigger rus val %ld %d %d %d\n", val,
			gnativetimeout, gjavatimeout, gtotaltimeout);
		return cnt;
	}

	/* pr_err("shutdown_detect_trigger final val %ld %d %d %d\n", val, gnativetimeout, gjavatimeout, gtotaltimeout); */

	/*val:0x shutdown_delay_enable | shutdown_timeout */
	if (val < SHUTDOWN_RUS_MIN && val >= SHUTDOWN_DELAY_ENABLE) {
		shutdown_timeout = (val % (SHUTDOWN_DELAY_ENABLE)) * 5;
		if (!shutdown_task) {
			shutdown_task = kthread_create(shutdown_kthread, NULL,
                                               "shutdown_kthread");
			if (IS_ERR(shutdown_task)) {
				pr_err("create shutdown thread fail, will BUG()\n");
				msleep(60 * 1000);
				BUG();
			}
		}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
		wake_lock(&shutdown_wakelock);
#else
		__pm_stay_awake(shutdown_ws);
#endif
		mod_timer(&shutdown_timer, jiffies + HZ * shutdown_timeout);
		pr_err("shutdown_delay enable , shutdown timeout : %d\n", shutdown_timeout);
	} else if (val == SHUTDOWN_DELAY_DISABLE) {
		del_timer_sync(&shutdown_timer);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
		wake_unlock(&shutdown_wakelock);
#else
		__pm_relax(shutdown_ws);
#endif
		pr_err("shutdown_delay disable \n");
	}

	switch (val) {
	case 0:
		if (shutdown_detect_started) {
			shutdown_detect_started = false;
			shutdown_phase = 0;
		}
		shutdown_detect_enable = false;
		pr_err("shutdown_detect: abort shutdown detect\n");
		break;
	case SHUTDOWN_STAGE_KERNEL:
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
		shutdown_kernel_start_time = current_kernel_time().tv_sec;

		pr_info("shutdown_kernel_start_time %ld\n",
			shutdown_kernel_start_time);
#else
		shutdown_kernel_start_time = current_kernel_time();

		pr_info("shutdown_kernel_start_time %lld\n",
			shutdown_kernel_start_time);
#endif
		if ((shutdown_kernel_start_time - shutdown_init_start_time) >
		    gnativetimeout) {
			pr_err("shutdown_detect_timeout: timeout happened in reboot.cpp\n");
			shutdown_dump_kernel_log();
			shutdown_timeout_flag_write(1);
		} else {
			if (((shutdown_init_start_time -
			      shutdown_systemserver_start_time) >
			     gjavatimeout) &&
			    shutdown_systemserver_start_time) {
				/* timeout happend in system_server stage */
				shutdown_timeout_flag_write(1);
			}
		}
		shutdown_phase = val;
		pr_err("shutdown_detect_phase: shutdown  current phase systemcall\n");
		break;
	case SHUTDOWN_STAGE_INIT:
		if (!shutdown_detect_started) {
			shutdown_detect_started = true;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
			shutdown_init_start_time = current_kernel_time().tv_sec;
#else
			shutdown_init_start_time = current_kernel_time();
#endif
			shutdown_start_time = shutdown_init_start_time;
			shd_complete_monitor =
				kthread_run(shutdown_detect_func, NULL,
					    "shutdown_detect_thread");
			if (IS_ERR(shd_complete_monitor)) {
				ret = PTR_ERR(shd_complete_monitor);
				pr_err("shutdown_detect: cannot start thread: %d\n",
				       ret);
			}

		} else {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
			shutdown_init_start_time = current_kernel_time().tv_sec;
#else
			shutdown_init_start_time = current_kernel_time();
#endif

			if ((shutdown_init_start_time -
			     shutdown_systemserver_start_time) > gjavatimeout) {
				pr_err("shutdown_detect_timeout: timeout happened in system_server stage\n");
			}
		}
		/* pr_err("shutdown_init_start_time %ld\n", shutdown_init_start_time); */
		shutdown_phase = val;
		del_timer_sync(&shutdown_timer);
		pr_err("shutdown_detect_phase: shutdown  current phase init\n");
		break;
	case SHUTDOWN_TIMEOUNT_UMOUNT:
		pr_err("shutdown_detect_timeout: umount timeout\n");
		break;
	case SHUTDOWN_TIMEOUNT_VOLUME:
		pr_err("shutdown_detect_timeout: volume shutdown timeout\n");
		break;
	case SHUTDOWN_STAGE_SYSTEMSERVER:
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
		shutdown_systemserver_start_time = current_kernel_time().tv_sec;
#else
		shutdown_systemserver_start_time = current_kernel_time();
#endif

		/* pr_err("shutdown_systemserver_start_time %ld\n", shutdown_systemserver_start_time); */
		if (!shutdown_detect_started) {
			shutdown_detect_started = true;
			shutdown_start_time = shutdown_systemserver_start_time;
			shd_complete_monitor =
				kthread_run(shutdown_detect_func, NULL,
					    "shutdown_detect_thread");
		}
		shutdown_phase = val;
		g_shutdown_flag = 1;
		pr_err("shutdown_detect_phase: shutdown  current phase systemserver\n");
		break;
	case SHUTDOWN_TIMEOUNT_SUBSYSTEM:
		pr_err("shutdown_detect_timeout: ShutdownSubSystem timeout\n");
		break;
	case SHUTDOWN_TIMEOUNT_RADIOS:
		pr_err("shutdown_detect_timeout: ShutdownRadios timeout\n");
		break;
	case SHUTDOWN_TIMEOUNT_PM:
		pr_err("shutdown_detect_timeout: ShutdownPackageManager timeout\n");
		break;
	case SHUTDOWN_TIMEOUNT_AM:
		pr_err("shutdown_detect_timeout: ShutdownActivityManager timeout\n");
		break;
	case SHUTDOWN_TIMEOUNT_BC:
		pr_err("shutdown_detect_timeout: SendShutdownBroadcast timeout\n");
		break;
	case SHUTDOWN_TRIGGER_RESTORE:/* 102 */
		try_restore();
		break;
	case SHUTDOWN_TRIGGER_STORE:/* 103 */
		store_current_shutdown_record(NULL, 0, NULL);
		break;
	default:
		break;
	}
	if (!shutdown_task && is_shutdows) {
		shutdown_task = kthread_create(shutdown_kthread, NULL,
					       "shutdown_kthread");
		if (IS_ERR(shutdown_task)) {
			pr_err("create shutdown thread fail, will BUG()\n");
			msleep(60 * 1000);
			BUG();
		}
	}
	return cnt;
}

static int shutdown_detect_show(struct seq_file *m, void *v)
{
	size_t i;
	struct rtc_time tm;

	SEQ_printf(m, "=== Last shutdown record ===\n");
	if (!strncmp(last_shutdown_record.magic, SHUTDOWN_MAGIC, SHUTDOWN_MAGIC_LEN)) {
		SEQ_printf(m, "SHUTDOWN_ACTION | %ld:%s\n", last_shutdown_record.action, get_action_name(last_shutdown_record.action));

		for (i = 0; i < COUNT_SHUTDOWN_STAGE; i++) {
			if (last_shutdown_record.stages[i].uptime != 0) {
				/* Need to add 8 hours because China Standard Time (CST) is 8 hours ahead of UTC */
				rtc_time64_to_tm(last_shutdown_record.stages[i].utc + 8 * 3600, &tm);

				SEQ_printf(m, "%s | %llu | %04d/%02d/%02d %02d:%02d:%02d\n",
						get_shutdown_stage_name(last_shutdown_record.stages[i].stage_id),
						last_shutdown_record.stages[i].uptime,
						tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			}
		}
	} else {
		SEQ_printf(m, "SHUTDOWN_MAGIC_INVALID\n");
	}
	SEQ_printf(m, "=== Last shutdown record ===\n\n");

	SEQ_printf(m, "=== shutdown_detect controller ===\n");
	SEQ_printf(m, "0:   shutdown_detect abort\n");
	SEQ_printf(m, "20:   shutdown_detect systemcall reboot phase\n");
	SEQ_printf(m, "30:   shutdown_detect init reboot phase\n");
	SEQ_printf(m, "40:   shutdown_detect system server reboot phase\n");
	SEQ_printf(m, "=== shutdown_detect controller ===\n\n");
	SEQ_printf(m, "shutdown_detect: shutdown phase: %u\n", shutdown_phase);
	return 0;
}

static int shutdown_detect_open(struct inode *inode, struct file *file)
{
	return single_open(file, shutdown_detect_show, inode->i_private);
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
static const struct file_operations shutdown_detect_fops = {
	.open = shutdown_detect_open,
	.write = shutdown_detect_trigger,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#else
static const struct proc_ops shutdown_detect_fops = {
	.proc_open = shutdown_detect_open,
	.proc_write = shutdown_detect_trigger,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
#endif

static int dump_kmsg(const char *filepath, size_t offset_of_start,
		     struct kmsg_dumper *kmsg_dumper)
{
#if FILP_OPEN_FUNCTION_CLOSE
	struct file *opfile;
	loff_t offset;
	char line[1024] = { 0 };
	size_t len = 0;
	int result = -1;
	size_t bytes_writed = 0;

	opfile = filp_open(filepath, O_CREAT | O_WRONLY | O_TRUNC,
			   FILE_MODE_0666);
	if (IS_ERR(opfile)) {
		pr_err("filp_open %s failed, error: %ld\n", filepath,
		       PTR_ERR(opfile));
		return -1;
	}
	offset = offset_of_start;

	kmsg_dumper->active = true;
	while (kmsg_dump_get_line(kmsg_dumper, true, line, sizeof(line),
				  &len)) {
		line[len] = '\0';
		mutex_lock(&shd_wf_mutex);

		bytes_writed = kernel_write(opfile, line, len, &offset);

		if (len != bytes_writed) {
			pr_err("kernel_write %s failed, len: %lu bytes_writed: %lu\n",
			       filepath, len, bytes_writed);
			mutex_unlock(&shd_wf_mutex);
			result = -1;
			goto shd_fail;
		}
		mutex_unlock(&shd_wf_mutex);
	}
	result = 0;

shd_fail:
	vfs_fsync(opfile, 0);
	filp_close(opfile, NULL);
	return result;
#endif
	return 0; /* FILP_OPEN_FUNCTION_CLOSE */
}

int shutdown_kernel_log_save(void *args)
{
	if (0 != dump_kmsg(OPLUS_PARTITION_OPLUSRESERVE3_LINK,
			   OPLUS_SHUTDOWN_KMSG_OFFSET, &shutdown_kmsg_dumper)) {
		pr_err("dump kmsg to OPLUS_PARTITION_OPLUSRESERVE3_LINK failed\n");
		complete(&shd_comp);
		return -1;
	}
	complete(&shd_comp);
	return 1;
}

static int shutdown_timeout_flag_write_now(void *args)
{
#if FILP_OPEN_FUNCTION_CLOSE
	struct file *opfile;
	ssize_t size;
	loff_t offsize;
	char data_info[SIZEOF_STRUCT_SHD_INFO] = { '\0' };
	int rc;
	struct shd_info shutdown_flag;

	opfile = filp_open(OPLUS_PARTITION_OPLUSRESERVE3_LINK, O_RDWR, 0600);
	if (IS_ERR(opfile)) {
		pr_err("open OPLUS_PARTITION_OPLUSRESERVE3_LINK error: %ld\n",
		       PTR_ERR(opfile));
		complete(&shd_comp);
		return -1;
	}

	offsize = OPLUS_SHUTDOWN_FLAG_OFFSET;

	strncpy(shutdown_flag.magic, SHUTDOWN_MAGIC, SHUTDOWN_MAGIC_LEN);
	if (gtimeout) {
		shutdown_flag.shutdown_err = ShutDownTO;
	} else {
		shutdown_flag.shutdown_err = 0;
	}

	shutdown_flag.shutdown_times =
		(int)(shutdown_end_time - shutdown_start_time);

	memcpy(data_info, &shutdown_flag, SIZEOF_STRUCT_SHD_INFO);

	size = kernel_write(opfile, data_info, SIZEOF_STRUCT_SHD_INFO,
			    &offsize);
	if (size < 0) {
		pr_err("kernel_write data_info %s size %ld \n", data_info,
		       size);
		filp_close(opfile, NULL);
		complete(&shd_comp);
		return -1;
	}

	rc = vfs_fsync(opfile, 1);
	if (rc) {
		pr_err("sync returns %d\n", rc);
	}

	filp_close(opfile, NULL);
	pr_info("shutdown_timeout_flag_write_now done \n");
	complete(&shd_comp);
#endif
	return 0;
}

static void task_comm_to_struct(const char *pcomm,
				struct task_struct **t_result)
{
	struct task_struct *g, *t;
	rcu_read_lock();
	for_each_process_thread(g, t) {
		if (!strcmp(t->comm, pcomm)) {
			*t_result = t;
			rcu_read_unlock();
			return;
		}
	}
	t_result = NULL;
	rcu_read_unlock();
}

#if IS_MODULE(CONFIG_OPLUS_FEATURE_SHUTDOWN_DETECT)
#define __si_special(priv) ((priv) ? SEND_SIG_PRIV : SEND_SIG_NOINFO)
#endif
void shutdown_dump_android_log(void)
{
	struct task_struct *sd_init;
	sd_init = NULL;
	task_comm_to_struct(TASK_INIT_COMM, &sd_init);
	if (NULL != sd_init) {
		pr_err("send shutdown_dump_android_log signal %d",
		       SIG_SHUTDOWN);
#if IS_MODULE(CONFIG_OPLUS_FEATURE_SHUTDOWN_DETECT)
		send_sig_info(SIG_SHUTDOWN, __si_special(0), sd_init);
#else
		send_sig(SIG_SHUTDOWN, sd_init, 0);
#endif
		pr_err("after send shutdown_dump_android_log signal %d",
		       SIG_SHUTDOWN);
		/* wait to collect shutdown log finished */
		schedule_timeout_interruptible(20 * HZ);
	}
}

static void shutdown_dump_kernel_log(void)
{
	struct task_struct *tsk;
	tsk = kthread_run(shutdown_kernel_log_save, NULL, "shd_collect_dmesg");
	if (IS_ERR(tsk)) {
		pr_err("create kernel thread shd_collect_dmesg failed\n");
		return;
	}
	/* wait max 10s to collect shutdown log finished */
	if (!wait_for_completion_timeout(&shd_comp, KE_LOG_COLLECT_TIMEOUT)) {
		pr_err("collect kernel log timeout\n");
	}
}


#if IS_ENABLED (CONFIG_OPLUS_BSP_DFR_USERSPACE_BACKTRACE)
static const char *shutdown_key_processes[] = {
	"surfaceflinger",
	"Binder:vold",
	"init",
};

static void shutdown_dump_ubt(void)
{
	struct task_struct *p = NULL;
	struct task_struct *target_task = NULL;
	int key_size = ARRAY_SIZE(shutdown_key_processes);
	int idx = 0;

	while(idx < key_size) {
		rcu_read_lock();
		for_each_process(p) {
			if (!strncmp(p->comm, shutdown_key_processes[idx], TASK_COMM_LEN)) {
				target_task = p;
				break;
			}
		}
		rcu_read_unlock();
		if (target_task) {
			/* user space backtrace */
			dump_userspace_bt(target_task);
		}
		target_task = NULL;
		idx++;
	}
}
#endif /* CONFIG_OPLUS_BSP_DFR_USERSPACE_BACKTRACE */


static void shutdown_timeout_flag_write(int timeout)
{
	struct task_struct *tsk;

	gtimeout = timeout;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
	shutdown_end_time = current_kernel_time().tv_sec;
#else
	shutdown_end_time = current_kernel_time();
#endif

	tsk = kthread_run(shutdown_timeout_flag_write_now, NULL, "shd_to_flag");
	if (IS_ERR(tsk)) {
		pr_err("create kernel thread shd_to_flag failed\n");
		return;
	}
	/* wait max 10s to collect shutdown log finished */
	if (!wait_for_completion_timeout(&shd_comp, KE_LOG_COLLECT_TIMEOUT)) {
		pr_err("shutdown_timeout_flag_write timeout\n");
	}
}

static int shutdown_detect_func(void *dummy)
{
	/* schedule_timeout_uninterruptible(gtotaltimeout * HZ); */
	msleep(gtotaltimeout * 1000);

	pr_err("shutdown_detect:%s call sysrq show block and cpu thread. BUG\n",
	       __func__);
	handle_sysrq('w');
	handle_sysrq('l');
	pr_err("shutdown_detect:%s shutdown_detect status:%u. \n", __func__,
	       shutdown_phase);

	if (shutdown_phase >= SHUTDOWN_STAGE_INIT) {
		shutdown_dump_android_log();
	}

#if IS_ENABLED (CONFIG_OPLUS_BSP_DFR_USERSPACE_BACKTRACE)
	/* add init stage userspace backtrace dump */
	if (shutdown_phase == SHUTDOWN_STAGE_INIT ||
		shutdown_phase == SHUTDOWN_TIMEOUNT_UMOUNT ||
		shutdown_phase == SHUTDOWN_TIMEOUNT_VOLUME) {
		shutdown_dump_ubt();
	}
#endif /* CONFIG_OPLUS_BSP_DFR_USERSPACE_BACKTRACE */

	shutdown_dump_kernel_log();
	/* timeout happened */
	shutdown_timeout_flag_write(1);

	if (get_eng_version() == OEM_RELEASE || get_eng_version() == AGING) {
		if (is_shutdows) {
			pr_err("shutdown_detect: shutdown or reboot? shutdown\n");
			if (shutdown_task) {
				wake_up_process(shutdown_task);
			}
		} else {
			pr_err("shutdown_detect: shutdown or reboot? reboot\n");
			BUG();
		}
	} else {
		pr_err("shutdown_detect_error, keep origin follow in !release build, but you can still get log in oplusreserve3\n");
	}
	return 0;
}

static void shutdown_timer_func(struct timer_list *t)
{
	if (shutdown_task) {
		wake_up_process(shutdown_task);
	}

	return;
}

static int restore_thread(void *arg)
{
	struct block_device *bdev = NULL;
	int ret;

	/* Get the last shutdown record from oplusreserve1 */
	bdev = get_reserve_partition_bdev();
	if (!bdev) {
		pr_err("get_reserve_partition_bdev fail, err=%ld\n", PTR_ERR(bdev));
	} else {
		ret = restore_last_shutdown_record(bdev);
		if (ret) {
			pr_err("restore_last_shutdown_record fail, ret=%d\n", ret);
		} else {
			pr_info("Last shutdown checkpoint data has been successfully restored");
			flag_restored = 1;
		}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
		blkdev_put(bdev, NULL);
#else
		blkdev_put(bdev, FMODE_READ | FMODE_WRITE | FMODE_EXCL);
#endif
	}

	return 0;
}

static void try_restore(void) {
	struct task_struct *tsk;

	if (flag_restored) {
	pr_err("The last shutdown checkpoint data has been restored.");
		return;
	}

	tsk = kthread_run(restore_thread, NULL, "restore_thread");
	if (!tsk) {
		pr_err("kthread init failed\n");
	}
}

static int __init init_shutdown_detect_ctrl(void)
{
	struct proc_dir_entry *pe;

	pr_err("shutdown_detect:register shutdown_detect interface\n");
	pe = proc_create("shutdown_detect", 0664, NULL, &shutdown_detect_fops);
	if (!pe) {
		pr_err("shutdown_detect:Failed to register shutdown_detect interface\n");
		goto EXIT;
	}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
	wake_lock_init(&shutdown_wakelock, WAKE_LOCK_SUSPEND, "shutdown_wakelock");
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 102) && LINUX_VERSION_CODE > KERNEL_VERSION(4, 14, 999))
	shutdown_ws = wakeup_source_register("shutdown_wakelock");
#else
	shutdown_ws = wakeup_source_register(NULL, "shutdown_wakelock");
#endif
	/* init shutdown timer */
	timer_setup(&shutdown_timer, shutdown_timer_func,
  		TIMER_DEFERRABLE);
#if IS_ENABLED (CONFIG_OPLUS_BSP_DFR_USERSPACE_BACKTRACE)
	dump_userspace_init("ubt,shutdown");
#endif /* CONFIG_OPLUS_BSP_DFR_USERSPACE_BACKTRACE */

	/* For recording the current shutdown record */
	register_reboot_notifier(&reboot_nb);

EXIT:
	try_restore();

	return 0;
}

device_initcall(init_shutdown_detect_ctrl);

static void __exit exit_shutdown_detect_ctrl(void)
{
	pr_err("shutdown_detect:unregister shutdown_detect interface\n");
}

module_exit(exit_shutdown_detect_ctrl);

#if IS_MODULE(CONFIG_OPLUS_FEATURE_SHUTDOWN_DETECT)
MODULE_IMPORT_NS(VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver);
#endif
MODULE_LICENSE("GPL v2");
