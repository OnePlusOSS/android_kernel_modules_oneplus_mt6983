// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025-2026 Oplus. All rights reserved.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/async.h>
#include <trace/events/filemap.h>

#define TASK_COMM_LEN 16
#define MAX_PROCESSES 5
#define PROC_NAME "io_file_record"
#define TASK_COMM_LEN 16

#define FOR_EACH_INTEREST(i) \
	for (i = 0; i < sizeof(interests) / sizeof(struct tracepoints_table); \
	i++)

#define TRACING_MARK_BUF_SIZE 256

#define tracing_mark(fmt, args...) \
do { \
	char buf[TRACING_MARK_BUF_SIZE]; \
	snprintf(buf, TRACING_MARK_BUF_SIZE, "B|" fmt, ##args); \
	tracing_mark_write(buf); \
	snprintf(buf, TRACING_MARK_BUF_SIZE, "E|" fmt, ##args); \
	tracing_mark_write(buf); \
} while (0)

static char *process_names[MAX_PROCESSES];
static int process_count = 0;
static spinlock_t node_lock;

struct tracepoints_table {
	const char *name;
	void *func;
	struct tracepoint *tp;
	bool init;
};

static noinline int tracing_mark_write(const char *buf)
{
	trace_printk(buf);
	return 0;
}

static char *__dentry_name(struct dentry *dentry, char *name)
{
	char *p = dentry_path_raw(dentry, name, PATH_MAX);
	char *root;
	size_t len;

	root = dentry->d_sb->s_fs_info;
	len = strlen(root);
	if (IS_ERR(p)) {
		__putname(name);
		return NULL;
	}

	/*
	 * This function relies on the fact that dentry_path_raw() will place
	 * the path name at the end of the provided buffer.
	 */
	WARN_ON(p + strlen(p) + 1 != name + PATH_MAX);

	strlcpy(name, root, PATH_MAX);
	if (len > p - name) {
		__putname(name);
		return NULL;
	}

	if (p > name + len)
		strcpy(name + len, p);

	return name;
}

static char *dentry_name(struct dentry *dentry)
{
	char *name = kmem_cache_alloc(names_cachep, GFP_ATOMIC);

	if (!name) {
		return NULL;
	}

	return __dentry_name(dentry, name);
}

static void put_dentry(void *data, async_cookie_t cookie)
{
	struct dentry *dentry = data;
	dput(dentry);
}

static char *inode_name(struct inode *ino)
{
	struct dentry *dentry;
	char *name;

	dentry = d_find_alias(ino);
	if (!dentry)
		return NULL;

	name = dentry_name(dentry);
	async_schedule(put_dentry, dentry);

	return name;
}

static bool match_group_leader(void)
{
	int i;
	struct task_struct *leader = current->group_leader;
	unsigned long flags;
	bool result = false;

	spin_lock_irqsave(&node_lock, flags);
	if (!leader) {
		spin_unlock_irqrestore(&node_lock, flags);
		return false;
	}

	for (i = 0; i < process_count; i++) {
		if (process_names[i] && strcmp(process_names[i], leader->comm) == 0) {
			result = true;
			break;
		}
	}
	spin_unlock_irqrestore(&node_lock, flags);

	return result;
}

static void file_map_track_handler(void *ignore, struct page *page)
{
	struct address_space *mapping = page->mapping;
	char *name;
	dev_t s_dev;

	if (!match_group_leader())
		return;

	if (mapping->host->i_sb) {
		name = inode_name(mapping->host);
		if (name) {
			tracing_mark("%d|file_record %s ofs=%lu\n", current->tgid, name,
				page->index << PAGE_SHIFT);
			__putname(name);
		} else {
			s_dev = mapping->host->i_sb->s_dev;
			tracing_mark("%d|file_record s_dev=%d ino=%lu ofs=%lu\n",
				current->tgid, s_dev, mapping->host->i_ino,
				 page->index << PAGE_SHIFT);
		}
	}
}

static struct tracepoints_table interests[] = {
	{
		.name = "mm_filemap_add_to_page_cache",
		.func = file_map_track_handler
	},
};

/*
 * Find the struct tracepoint* associated with a given tracepoint
 * name.
 */
static void lookup_tracepoints(struct tracepoint *tp, void *ignore)
{
	int i;

	FOR_EACH_INTEREST(i) {
		if (strcmp(interests[i].name, tp->name) == 0)
			interests[i].tp = tp;
	}
}

static void uninstall_tracepoints(void)
{
	int i;

	FOR_EACH_INTEREST(i) {
		if (interests[i].init) {
			tracepoint_probe_unregister(interests[i].tp,
						    interests[i].func,
						    NULL);
		}
	}
}

static bool install_tracepoints(void)
{
	int i;

	for_each_kernel_tracepoint(lookup_tracepoints, NULL);
	FOR_EACH_INTEREST(i) {
		if (interests[i].tp == NULL) {
			pr_err("%s : tracepoint %s not found\n",
				THIS_MODULE->name, interests[i].name);
			uninstall_tracepoints();
			return false;
		}

		tracepoint_probe_register(interests[i].tp,
					  interests[i].func,
					  NULL);
		interests[i].init = true;
	}

	return true;
}

static ssize_t file_record_proc_read(struct file *file, char __user *buf,
					size_t count, loff_t *ppos)
{
	char *buffer;
	int len = 0;
	int i;
	unsigned long flags;

	for (i = 0; i < process_count; i++) {
		if (process_names[i]) {
			len += strlen(process_names[i]) + 1;
		}
	}

	buffer = kmalloc(len + 1, GFP_KERNEL);
	if (!buffer) {
		return -ENOMEM;
	}

	len = 0;
	spin_lock_irqsave(&node_lock, flags);
	for (i = 0; i < process_count; i++) {
		if (process_names[i]) {
			len += snprintf(buffer + len, strlen(process_names[i]) + 2,
				"%s\n", process_names[i]);
		}
	}

	spin_unlock_irqrestore(&node_lock, flags);
	if (*ppos >= len) {
		kfree(buffer);
		return 0;
	}

	if (count > len - *ppos) {
		count = len - *ppos;
	}

	if (copy_to_user(buf, buffer + *ppos, count)) {
		kfree(buffer);
		return -EFAULT;
	}

	*ppos += count;
	kfree(buffer);
	return count;
}

static ssize_t file_record_proc_write(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	char *buffer, *orig;
	char *cmd;
	char *name;
	unsigned long flags;

	buffer = kmalloc(count + 1, GFP_KERNEL);
	if (!buffer) {
		return -ENOMEM;
	}

	if (copy_from_user(buffer, buf, count)) {
		kfree(buffer);
		return -EFAULT;
	}

	buffer[count] = '\0';
	orig = buffer;

	if (count > 0 && buffer[count-1] == '\n')
		buffer[count-1] = '\0';

	cmd = strsep(&buffer, " ");
	name = strsep(&buffer, " ");
	spin_lock_irqsave(&node_lock, flags);
	if (!strcmp(cmd, "-add") && name) {
		if (process_count < MAX_PROCESSES) {
			process_names[process_count] = kmalloc(TASK_COMM_LEN, GFP_ATOMIC);
			if (!process_names[process_count]) {
				spin_unlock_irqrestore(&node_lock, flags);
				kfree(orig);
				return -ENOMEM;
			}
			strncpy(process_names[process_count], name, TASK_COMM_LEN - 1);
			process_names[process_count][TASK_COMM_LEN - 1] = '\0';
			process_count++;
		} else {
			spin_unlock_irqrestore(&node_lock, flags);
			kfree(orig);
			return -ENOSPC;
		}
	} else if (!strcmp(cmd, "-del") && name) {
		int i;
		for (i = 0; i < process_count; i++) {
			if (process_names[i] && strcmp(process_names[i], name) == 0) {
				kfree(process_names[i]);
				process_names[i] = NULL;
				for (; i < process_count - 1; i++) {
					process_names[i] = process_names[i + 1];
				}
				process_count--;
				break;
			}
		}
	} else if (!strncmp(cmd, "-clear", sizeof("-clear") - 1)) {
		int i;
		for (i = 0; i < process_count; i++) {
			if (process_names[i]) {
				kfree(process_names[i]);
				process_names[i] = NULL;
			}
		}
		process_count = 0;
	} else {
		spin_unlock_irqrestore(&node_lock, flags);
		kfree(orig);
		return -EINVAL;
	}
	spin_unlock_irqrestore(&node_lock, flags);
	kfree(orig);
	return count;
}

static const struct proc_ops file_record_proc_fops = {
	.proc_write = file_record_proc_write,
	.proc_read = file_record_proc_read,
};

static void create_proc_node(void)
{
	struct proc_dir_entry *pentry;

	pentry = proc_create(PROC_NAME,
		(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH), NULL,
			&file_record_proc_fops);

	if (!pentry)
		pr_err("%s: %s fail, name=%s", THIS_MODULE->name, __func__, PROC_NAME);
}

static int __init oplus_file_record_init(void)
{
	spin_lock_init(&node_lock);

	if (install_tracepoints())
		create_proc_node();

	return 0;
}

static void __exit oplus_file_record_exit(void)
{
	int i;
	unsigned long flags;

	spin_lock_irqsave(&node_lock, flags);

	for (i = 0; i < process_count; i++) {
		kfree(process_names[i]);
		process_names[i] = NULL;
	}

	spin_unlock_irqrestore(&node_lock, flags);
	remove_proc_entry(PROC_NAME, NULL);
	uninstall_tracepoints();
}

module_init(oplus_file_record_init);
module_exit(oplus_file_record_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jiayingrui");
MODULE_DESCRIPTION("Used to record IO file paths to trace buf");
