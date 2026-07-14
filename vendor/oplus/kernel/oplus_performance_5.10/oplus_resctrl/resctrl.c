// SPDX-License-Identifier: GPL-2.0-only
/*
* Copyright (C) 2023 Oplus. All rights reserved.
*/

#include <linux/string.h>
#include "resctrl.h"
#include "linux/proc_fs.h"
#include "linux/module.h"

#define OPLUS_RESCTRL_PROC "oplus_resctrl"
#define RESCTRL_BUFSIZE_MAX 4096
#define RESCTRL_CONFIG_MAX 64

void resctrl_printk(int log_level, const char *func, const char *fmt, ...)
{
	/* todo */
	return;
}

static ssize_t iocost_ppm_read(struct file *filp, char __user *buff,
			       size_t count, loff_t *off)
{
	char page[32] = { 0 };
	int len = 0;

	len += sprintf(page, "%d\n", ioc_iop_read());

	return sched_data_to_user(buff, count, off, page, len);
}

static ssize_t iocost_ppm_write(struct file *filp, const char __user *buff,
			      size_t count, loff_t *off)
{
	char str[RESCTRL_CONFIG_MAX] = {0};
	u32 new_start;
	int ret;

	if(count > (RESCTRL_CONFIG_MAX - 1) || count == 0)
		return -EFAULT;

	if (copy_from_user(str, buff, count))
		return -EFAULT;
	ret = kstrtouint(str, 10, &new_start);
	if (ret)
		return ret;
	if (new_start == iocost_config.ppm_start)
		return count;
	if (new_start) {
		iocost_config.ppm_start = new_start;
		iocost_config.timer.expires = jiffies + 1*HZ;
		add_timer(&iocost_config.timer);
	} else {
		iocost_config.ppm_start = new_start;
		iocost_config.timer.expires = jiffies + 1*HZ;
		del_timer(&iocost_config.timer);
	}

	return count;
}

static const struct proc_ops proc_iocost_ppm_fops = {
	.proc_read = iocost_ppm_read,
	.proc_write = iocost_ppm_write,
	.proc_lseek = default_llseek,
};

static ssize_t ioc_dist_read(int rw, char __user *buff, size_t count,
			     loff_t *off)
{
	int len = 0;
	int ret_len = 0;
	int i, j;
	char *const time_str[] = { "1us",      "2us",      "4us",
				   "8us",      "16us",     "32us",
				   "64us",     "128us",    "256us",
				   "512us",    "1024us",   "2048us",
				   "4096us",   "8192us",   "16384us",
				   "32768us",  "65536us",  "131072us",
				   "262144us", "524288us", "other" };
	char *page = kzalloc(RESCTRL_BUFSIZE_MAX, GFP_KERNEL);
	if (!page)
		return -ENOMEM;

	len += sprintf(page + len,
		       "%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s"
		       "%-10s\n",
		       "latency", "1", "2", "4", "8", "16", "32", "64", "128",
		       "256", "other");

	for (i = 0; i < IOC_DIST_TIMESTAT; i++) {
		u64 request[IOC_DIST_SEGMENTS] = { 0 };

		ioc_dist_get_stat(rw, request, i);

		len += sprintf(page + len, "%-10s", time_str[i]);
		for (j = 0; j < IOC_DIST_SEGMENTS; j++)
			len += sprintf(page + len, "%-10llu", request[j]);
		len += sprintf(page + len, "\n");
	}

	ret_len = sched_data_to_user(buff, count, off, page, len);
	kfree(page);
	return ret_len;
}

static ssize_t ioc_dist_read_read(struct file *filp, char __user *buff,
				  size_t count, loff_t *off)
{
	return ioc_dist_read(RESCTRL_READ, buff, count, off);
}

static ssize_t ioc_dist_write_read(struct file *filp, char __user *buff,
				   size_t count, loff_t *off)
{
	return ioc_dist_read(RESCTRL_WRITE, buff, count, off);
}

static void  ioc_parse_config(char *key, char *value)
{
	int ret;
	if (strcmp(key, "read_lat") == 0) {
		ret = kstrtouint(value, 10, &iocost_config.read_lat);
		if (ret)
			printk("resctrl ioc_parse_config err\n");
		printk("resctrl ioc_parse_config read_lat=%d\n", iocost_config.read_lat);
	}

	if (strcmp(key, "write_lat") == 0) {
		ret = kstrtouint(value, 10, &iocost_config.write_lat);
		if (ret)
			printk("resctrl ioc_parse_config err\n");
		printk("resctrl ioc_parse_config write_lat=%d\n", iocost_config.write_lat);
	}
}

static ssize_t ioc_dist_write(struct file *filp, const char __user *buff,
			      size_t count, loff_t *off)
{
	char str[RESCTRL_CONFIG_MAX] = {0};
	char *sptr;
	char *split = NULL;
	char *key = NULL;

	if(count > (RESCTRL_CONFIG_MAX - 1) || count == 0)
		return -EFAULT;

	if (copy_from_user(str, buff, count))
		return -EFAULT;
	sptr = str;

	split = strsep(&sptr, " ");
	while (split != NULL) {
		key = strsep(&split, "=");
		if (key && split) {
			ioc_parse_config(key, split);
		}

		split = strsep(&sptr, " ");
	}

	ioc_dist_clear_stat();
	return count;
}

static const struct proc_ops proc_ioc_dist_read_fops = {
	.proc_read = ioc_dist_read_read,
	.proc_write = ioc_dist_write,
	.proc_lseek = default_llseek,
};

static const struct proc_ops proc_ioc_dist_write_fops = {
	.proc_read = ioc_dist_write_read,
	.proc_write = ioc_dist_write,
	.proc_lseek = default_llseek,
};

static int __init oplus_resctrl_init(void)
{
	int ret = 0;
	struct proc_dir_entry *pentry_dir = NULL;
	struct proc_dir_entry *pentry = NULL;

	pentry_dir = proc_mkdir(OPLUS_RESCTRL_PROC, NULL);
	if (!pentry_dir) {
		resctrl_err("create dir failed.\n");
		return -ENOENT;
	}

	pentry = proc_create("iocost_ppm", S_IRUGO | S_IWUGO, pentry_dir,
			     &proc_iocost_ppm_fops);
	if (!pentry) {
		resctrl_err("create iocost_ppm failed.\n");
		goto err;
	}
	pentry = proc_create("ioc_dist_read", S_IRUGO | S_IWUGO, pentry_dir,
			     &proc_ioc_dist_read_fops);
	if (!pentry) {
		resctrl_err("create ioc_dist_read failed.\n");
		goto err;
	}

	pentry = proc_create("ioc_dist_write", S_IRUGO | S_IWUGO, pentry_dir,
			     &proc_ioc_dist_write_fops);
	if (!pentry) {
		resctrl_err("create ioc_dist_write failed.\n");
		goto err;
	}

	timer_setup(&iocost_config.timer, ioc_timer_fn, 0);
	iocost_config.ppm_start = 1;
	iocost_config.timer.expires = jiffies + 1*HZ;
	add_timer(&iocost_config.timer);

	return ret;
err:
	remove_proc_entry(OPLUS_RESCTRL_PROC, NULL);
	return -ENOENT;
}

static void __exit oplus_resctrl_exit(void)
{
	del_timer(&iocost_config.timer);
	remove_proc_subtree(OPLUS_RESCTRL_PROC, NULL);
	printk("oplus resctrl module exit\n");
}

module_init(oplus_resctrl_init);
module_exit(oplus_resctrl_exit);

MODULE_DESCRIPTION("oplus_resctrl");
MODULE_LICENSE("GPL v2");
