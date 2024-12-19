// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2020-2022 Oplus. All rights reserved.
 */

#define pr_fmt(fmt) "mapped_protect: " fmt

#include <linux/module.h>
#include <linux/types.h>
#include <trace/hooks/vmscan.h>
#include <trace/hooks/mm.h>
#include <linux/swap.h>
#include <linux/proc_fs.h>
#include <linux/gfp.h>
#include <linux/printk.h>
#include <linux/mmzone.h>
#include <linux/mm.h>
#include <linux/mm_inline.h>
#include <linux/pagemap.h>
#include <linux/page-flags.h>
#include <linux/debugfs.h>
#include <linux/memcontrol.h>

#ifdef CONFIG_MAPPED_PROTECT_ALL
#define MAPCOUNT_PROTECT_THRESHOLD (20)
#define MAX_BUF_LEN 10
/*
 * [0]: nr_anon_mapped_multiple
 * [1]: nr_file_mapped_multiple
 *
 * */
static atomic_long_t nr_mapped_multiple[2] = {
	ATOMIC_INIT(0)
};
static atomic_long_t nr_mapped_multiple_debug[2] = {
	ATOMIC_INIT(0)
};

unsigned long  max_nr_mapped_multiple[2] = { 0 };
#ifdef CONFIG_OPLUS_FG_PROTECT
unsigned long fg_mapped_file = 0;

static atomic_long_t fg_mapcount_debug_1[21] = {
	ATOMIC_INIT(0)
};

static atomic_t fg_mapcount = ATOMIC_INIT(0);
static atomic_long_t fg_protect_count = ATOMIC_INIT(0);
static atomic_t fg_mapcount_enable = ATOMIC_INIT(0);
#endif

unsigned long  mapcount_protected_high[2] = { 0 };

unsigned long  memavail_noprotected = 0;
static bool mapcount_protect_setup = false;
extern struct lruvec* page_to_lruvec(struct page *page, pg_data_t *pgdat);
extern void do_traversal_all_lruvec(void);
extern int __page_mapcount(struct page *page);
#ifdef CONFIG_OPLUS_FG_PROTECT
extern s64 get_mem_cgroup_app_uid(struct mem_cgroup *memcg);
extern bool is_fg(int uid);
extern struct mem_cgroup *get_next_memcg(struct mem_cgroup *prev);
extern void get_next_memcg_break(struct mem_cgroup *memcg);
#endif

static inline bool page_evictable(struct page *page)
{
	bool ret;

	/* Prevent address_space of inode and swap cache from being freed */
	rcu_read_lock();
	ret = !mapping_unevictable(page_mapping(page)) && !PageMlocked(page);
	rcu_read_unlock();
	return ret;
}

static bool mapped_protected_is_full(int file)
{
	if (atomic_long_read(&nr_mapped_multiple[file])
			> mapcount_protected_high[file])
		return true;
	else
		return false;
}

static bool mem_available_is_low(void)
{
	long available = si_mem_available();

	if (available < memavail_noprotected)
		return true;

	return false;
}

static void mark_page_accessed_handler(void *data, struct page* page)
{
	struct pglist_data *pgdat = page_pgdat(page);
	struct lruvec *lruvec;
	int file;

	if (likely(page_mapcount(page) < MAPCOUNT_PROTECT_THRESHOLD))
		return;

	if (!PageLRU(page) || PageUnevictable(page))
		return;

	if (!PageActive(page) && !PageUnevictable(page) &&
			(PageReferenced(page) || page_mapcount(page) > 10))
		return;

	file = page_is_file_lru(page);
	if (unlikely(mapped_protected_is_full(file)))
		return;

	if (spin_trylock_irq(&pgdat->lru_lock)) {
		int lru;

		lruvec = page_to_lruvec(page, pgdat);
		lru = page_lru(page);
		if (PageLRU(page) && !PageUnevictable(page))
			list_move(&page->lru, &lruvec->lists[lru]);
		spin_unlock_irq(&pgdat->lru_lock);
	}
}

#ifdef CONFIG_OPLUS_FG_PROTECT
static void update_fg_mapcount(long fg_mapped_file) {
	unsigned long fg_mapped_file_mb = fg_mapped_file >> 8;
	if (fg_mapped_file_mb  < 100)
		atomic_set(&fg_mapcount, 0);
	else if (fg_mapped_file_mb  < 150)
		atomic_set(&fg_mapcount, 1);
	else if (fg_mapped_file_mb  < 200)
		atomic_set(&fg_mapcount, 2);
	else if (fg_mapped_file_mb  < 250)
		atomic_set(&fg_mapcount, 3);
	else if (fg_mapped_file_mb  < 300)
		atomic_set(&fg_mapcount, 4);
	else if (fg_mapped_file_mb  < 350)
		atomic_set(&fg_mapcount, 5);
	else if (fg_mapped_file_mb  < 400)
		atomic_set(&fg_mapcount, 6);
	else if (fg_mapped_file_mb  < 450)
		atomic_set(&fg_mapcount, 7);
	else if (fg_mapped_file_mb  < 500)
		atomic_set(&fg_mapcount, 8);
	else if (fg_mapped_file_mb  < 550)
		atomic_set(&fg_mapcount, 9);
	else if (fg_mapped_file_mb  < 600)
		atomic_set(&fg_mapcount, 10);
	else
		atomic_set(&fg_mapcount, 20);
}
#endif

static void page_should_be_protect(void *data, struct page* page,
				bool *should_protect)
{
	int file;
#ifdef CONFIG_OPLUS_FG_PROTECT
	struct mem_cgroup *memcg = NULL;
	int uid;
	long fg_mapped_file = 0;
#endif

	file = page_is_file_lru(page);

	if (unlikely(!mapcount_protect_setup)) {
		*should_protect = false;
		return;
	}

#ifndef CONFIG_OPLUS_FG_PROTECT
	if (likely(page_mapcount(page) < MAPCOUNT_PROTECT_THRESHOLD)) {
		*should_protect = false;
		return;
	}
#endif

	if (unlikely(!page_evictable(page) || PageUnevictable(page))) {
		*should_protect = false;
		return;
	}

#ifdef CONFIG_OPLUS_FG_PROTECT
	if (unlikely(mem_available_is_low())) {
		*should_protect = false;
		return;
	}

	if (atomic_read(&fg_mapcount_enable) && file && page_memcg(page)) {
		memcg = page_memcg(page);
		uid = (int)get_mem_cgroup_app_uid(memcg);
		if (is_fg(uid)) {
			fg_mapped_file = atomic_long_read(&memcg->vmstats[NR_FILE_MAPPED]);
			update_fg_mapcount(fg_mapped_file);
			if (page_mapcount(page) > atomic_read(&fg_mapcount)) {
				*should_protect = true;
				atomic_long_add(1, &fg_protect_count);
				return;
			}
		}
	}

	if (likely(page_mapcount(page) < MAPCOUNT_PROTECT_THRESHOLD)) {
		*should_protect = false;
		return;
	}
#endif

	if (unlikely(mapped_protected_is_full(file))) {
		*should_protect = false;
		return;
	}

#ifndef CONFIG_OPLUS_FG_PROTECT
	if (unlikely(mem_available_is_low())) {
		*should_protect = false;
		return;
	}
#endif

	*should_protect = true;
}

static void update_page_mapcount(void *data, struct page *page, bool inc_size, bool compound, bool *ret, bool *success)
{
	unsigned long nr_mapped_multi_pages;
	int file, mapcount;

	*success = true;
	if (inc_size) {
		mapcount = atomic_inc_return(&page->_mapcount);
		if (ret)
			*ret = ((mapcount == 0) ? true : false);
	} else {
		mapcount = atomic_add_return(-1, &page->_mapcount);
		if (ret)
			*ret = ((mapcount < 0) ? true : false);
	}
	/* we update multi-mapped counts when page_mapcount(page) changing:
	 * - 19->20
	 * - 20->19
	 * Because we judge mapcount by page_mapcount(page) which return
	 * page->_mapcount + 1, variable "mapcount" + 1 = page_mapcount(page).
	 * If inc_size is equal to true, "mapcount" + 1 = MAPCOUNT_PROTECT_THRESHOLD
	 * which means 19->20.
	 * If inc_size is equal to false, "mapcount" + 2 = MAPCOUNT_PROTECT_THRESHOLD
	 * which means 20->19.
	 * */
	if (likely((mapcount + (inc_size ? 1 : 2)) !=
					MAPCOUNT_PROTECT_THRESHOLD))
		return;

	if (unlikely(!mapcount_protect_setup))
		return;

	if (!PageLRU(page) || PageUnevictable(page))
		return;

	file = page_is_file_lru(page);

	if (inc_size) {
		atomic_long_add(1, &nr_mapped_multiple[file]);
		nr_mapped_multi_pages = atomic_long_read(&nr_mapped_multiple[file]);
		if (max_nr_mapped_multiple[file] < nr_mapped_multi_pages)
			max_nr_mapped_multiple[file] = nr_mapped_multi_pages;
	} else {
		atomic_long_sub(1, &nr_mapped_multiple[file]);
	}

	return;
}

static void add_page_to_lrulist(void *data, struct page *page, bool compound, enum lru_list lru)
{
	unsigned long nr_mapped_multi_pages;
	int file;

	if (unlikely(!mapcount_protect_setup))
		return;

	if (likely(page_mapcount(page) < MAPCOUNT_PROTECT_THRESHOLD))
		return;

	if (lru == LRU_UNEVICTABLE)
		return;

	file = page_is_file_lru(page);

	atomic_long_add(1, &nr_mapped_multiple[file]);
	nr_mapped_multi_pages = atomic_long_read(&nr_mapped_multiple[file]);
	if (max_nr_mapped_multiple[file] < nr_mapped_multi_pages)
		max_nr_mapped_multiple[file] = nr_mapped_multi_pages;
}

static void del_page_from_lrulist(void *data, struct page *page, bool compound, enum lru_list lru)
{
	int file;

	if (unlikely(!mapcount_protect_setup))
		return;

	if (likely(page_mapcount(page) < MAPCOUNT_PROTECT_THRESHOLD))
		return;

	if (lru == LRU_UNEVICTABLE)
		return;

	file = page_is_file_lru(page);

	atomic_long_sub(1, &nr_mapped_multiple[file]);
}

#ifdef CONFIG_OPLUS_FG_PROTECT
static void do_traversal_fg_memcg()
{
	struct page *page;
	struct lruvec* lruvec;
	int lru, i;
	struct mem_cgroup *memcg = NULL;
	pg_data_t *pgdat = NODE_DATA(0);

	while ((memcg = get_next_memcg(memcg))) {
		if (is_fg(get_mem_cgroup_app_uid(memcg))) {
			lruvec = mem_cgroup_lruvec(memcg, pgdat);
			fg_mapped_file = atomic_long_read(&memcg->vmstats[NR_FILE_MAPPED]);
			for_each_evictable_lru(lru) {
				int file = is_file_lru(lru);
				if (file) {
					spin_lock_irq(&pgdat->lru_lock);
					list_for_each_entry(page, &lruvec->lists[lru], lru) {
						if (!page)
							continue;
						if (PageHead(page)) {
							for (i = 0; i < compound_nr(page); i++) {
								if (page_mapcount(page) < 20)
									atomic_long_add(1, &fg_mapcount_debug_1[page_mapcount(page)]);
								else
									atomic_long_add(1, &fg_mapcount_debug_1[20]);
							}
							continue;
						}
						if (page_mapcount(page) < 20)
							atomic_long_add(1, &fg_mapcount_debug_1[page_mapcount(page)]);
						else
							atomic_long_add(1, &fg_mapcount_debug_1[20]);
					}
					spin_unlock_irq(&pgdat->lru_lock);
				}
			}
			get_next_memcg_break(memcg);
			break;
		}
	}
}
#endif

static void do_traversal_lruvec(void *data, struct lruvec *lruvec)
{
	struct page *page;
	int lru, i;
	static bool first_get_nr_mapped = true;

	for_each_evictable_lru(lru) {
		int file = is_file_lru(lru);
		list_for_each_entry(page, &lruvec->lists[lru], lru) {
			if (!page)
				continue;
			if (PageHead(page)) {
				for (i = 0; i < compound_nr(page); i++) {
					if (page_mapcount(&page[i]) >= MAPCOUNT_PROTECT_THRESHOLD) {
						atomic_long_add(1, &nr_mapped_multiple_debug[file]);
						if (first_get_nr_mapped) {
							atomic_long_add(1, &nr_mapped_multiple[file]);
						}
					}
				}
				continue;
			}
			if (page_mapcount(page) >= MAPCOUNT_PROTECT_THRESHOLD) {
				if (first_get_nr_mapped)
					atomic_long_add(thp_nr_pages(page), &nr_mapped_multiple[file]);
				atomic_long_add(thp_nr_pages(page), &nr_mapped_multiple_debug[file]);
			}
		}
	}
	first_get_nr_mapped = false;
}

static void show_mapcount_pages(void *data, void *unused)
{
	printk(" multi_mapped0:%ld multi_mapped1:%ld max0:%ld max1:%ld\n",
		atomic_long_read(&nr_mapped_multiple[0]),
		atomic_long_read(&nr_mapped_multiple[1]),
		max_nr_mapped_multiple[0],
		max_nr_mapped_multiple[1]);
}
#endif

static void should_skip_page_referenced(void *data, struct page *page, unsigned long nr_to_scan, int lru, bool *bypass)
{
#ifdef CONFIG_MAPPED_PROTECT_ALL
	if (page_mapcount(page) >= 20)
#else
	if ((atomic_read(&page->_mapcount) + 1) >= 20)
#endif
		*bypass = true;
}

static int register_mapped_protect_vendor_hooks(void)
{
	int ret = 0;

	ret = register_trace_android_vh_page_referenced_check_bypass(should_skip_page_referenced, NULL);
	if (ret != 0) {
		pr_err("register_trace_android_vh_skip_page_referenced failed! ret=%d\n", ret);
		goto out;
	}

#ifdef CONFIG_MAPPED_PROTECT_ALL
	ret = register_trace_android_vh_update_page_mapcount(update_page_mapcount, NULL);
	if (ret != 0) {
		pr_err("register update_mapped_mul vendor_hooks failed! ret=%d\n", ret);
		goto out;
	}
	ret = register_trace_android_vh_add_page_to_lrulist(add_page_to_lrulist, NULL);
	if (ret != 0) {
		pr_err("register add_mapped_mul_op_lrulist vendor_hooks failed! ret=%d\n", ret);
		goto out1;
	}
	ret = register_trace_android_vh_del_page_from_lrulist(del_page_from_lrulist, NULL);
	if (ret != 0) {
		pr_err("register dec_mapped_mul_op_lrulist vendor_hooks failed! ret=%d\n", ret);
		goto out2;
	}
	ret = register_trace_android_vh_page_should_be_protected(page_should_be_protect, NULL);
	if (ret != 0) {
		pr_err("register page_should_be_protect vendor_hooks failed! ret=%d\n", ret);
		goto out3;
	}
	ret = register_trace_android_vh_mark_page_accessed(mark_page_accessed_handler, NULL);
	if (ret != 0) {
		pr_err("register mark_page_accessed vendor_hooks failed! ret=%d\n", ret);
		goto out4;
	}
	ret = register_trace_android_vh_do_traversal_lruvec(do_traversal_lruvec, NULL);
	if (ret != 0) {
		pr_err("register do_traversal_lruvec vendor_hooks failed! ret=%d\n", ret);
		goto out5;
	}
	ret = register_trace_android_vh_show_mapcount_pages(show_mapcount_pages, NULL);
	if (ret != 0) {
		pr_err("register do_traversal_lruvec vendor_hooks failed! ret=%d\n", ret);
		goto out6;
	}
	return ret;

out6:
	unregister_trace_android_vh_do_traversal_lruvec(do_traversal_lruvec, NULL);
out5:
	unregister_trace_android_vh_mark_page_accessed(mark_page_accessed_handler, NULL);
out4:
	unregister_trace_android_vh_page_should_be_protected(page_should_be_protect, NULL);
out3:
	unregister_trace_android_vh_del_page_from_lrulist(del_page_from_lrulist, NULL);
out2:
	unregister_trace_android_vh_add_page_to_lrulist(add_page_to_lrulist, NULL);
out1:
	unregister_trace_android_vh_update_page_mapcount(update_page_mapcount, NULL);
#endif
out:
	return ret;
}

static void unregister_mapped_protect_vendor_hooks(void)
{
#ifdef CONFIG_MAPPED_PROTECT_ALL
	unregister_trace_android_vh_update_page_mapcount(update_page_mapcount, NULL);
	unregister_trace_android_vh_add_page_to_lrulist(add_page_to_lrulist, NULL);
	unregister_trace_android_vh_del_page_from_lrulist(del_page_from_lrulist, NULL);
	unregister_trace_android_vh_page_should_be_protected(page_should_be_protect, NULL);
	unregister_trace_android_vh_mark_page_accessed(mark_page_accessed_handler, NULL);
	unregister_trace_android_vh_do_traversal_lruvec(do_traversal_lruvec, NULL);
	unregister_trace_android_vh_show_mapcount_pages(show_mapcount_pages, NULL);
#endif
	unregister_trace_android_vh_page_referenced_check_bypass(should_skip_page_referenced, NULL);
	return;
}

#ifdef CONFIG_MAPPED_PROTECT_ALL
static int mapcount_protect_show(struct seq_file *m, void *arg)
{
	atomic_long_set(&nr_mapped_multiple_debug[0], 0);
	atomic_long_set(&nr_mapped_multiple_debug[1], 0);
	do_traversal_all_lruvec();

	seq_printf(m,
		   "now_anon_nr_mapped:     %ld\n"
		   "now_file_nr_mapped:     %ld\n"
		   "nr_anon_mapped_multiple:     %ld\n"
		   "nr_file_mapped_multiple:     %ld\n"
		   "max_nr_anon_mapped_multiple:     %ld\n"
		   "max_nr_file_mapped_multiple:     %ld\n"
		   "nr_anon_mapped_high:     %lu\n"
		   "nr_file_mapped_high:     %lu\n"
		   "memavail_noprotected:     %lu\n",
		   nr_mapped_multiple_debug[0],
		   nr_mapped_multiple_debug[1],
		   nr_mapped_multiple[0],
		   nr_mapped_multiple[1],
		   max_nr_mapped_multiple[0],
		   max_nr_mapped_multiple[1],
		   mapcount_protected_high[0],
		   mapcount_protected_high[1],
		   memavail_noprotected);
	seq_putc(m, '\n');

	return 0;
}
#endif

#ifdef CONFIG_OPLUS_FG_PROTECT
static int fg_protect_show(struct seq_file *m, void *arg)
{
	int i = 0;
	memset(fg_mapcount_debug_1, 0, sizeof(fg_mapcount_debug_1));
	do_traversal_fg_memcg();

	seq_printf(m,
		   "fg_mapcount:     %d\n",
		   fg_mapcount);
	seq_printf(m,
		   "fg_mapped_file:     %lu\n",
		   fg_mapped_file);

	for (i = 0; i < 21; i++) {
		seq_printf(m,
			   "fg_mapcount_debug_%d:     %lu\n",
			   i, fg_mapcount_debug_1[i]);
	}

	seq_putc(m, '\n');

	return 0;
}

static ssize_t fg_mapcount_enable_ops_write(struct file *file,
			const char __user *buff, size_t len, loff_t *ppos)
{
	int ret;
	char kbuf[MAX_BUF_LEN] = {'\0'};
	char *str;
	int val;

	if (len > MAX_BUF_LEN - 1) {
		return -EINVAL;
	}

	if (copy_from_user(&kbuf, buff, len))
		return -EFAULT;
	kbuf[len] = '\0';

	str = strstrip(kbuf);
	if (!str) {
		return -EINVAL;
	}

	ret = kstrtoint(str, 10, &val);
	if (ret) {
		return -EINVAL;
	}

	if (val < 0 || val > INT_MAX) {
		return -EINVAL;
	}

	printk("fg_mapcount_ops_write is %d\n", val);
	atomic_set(&fg_mapcount_enable, val);

	return len;
}


static ssize_t fg_mapcount_enable_ops_read(struct file *file,
			char __user *buffer, size_t count, loff_t *off)
{
	char kbuf[MAX_BUF_LEN] = {'\0'};
	int len;

	len = snprintf(kbuf, MAX_BUF_LEN, "%d\n", fg_mapcount_enable);

	if (len > *off)
		len -= *off;
	else
		len = 0;

	if (copy_to_user(buffer, kbuf + *off, (len < count ? len : count)))
		return -EFAULT;

	*off += (len < count ? len : count);
	return (len < count ? len : count);
}

static ssize_t fg_protect_count_ops_write(struct file *file,
			const char __user *buff, size_t len, loff_t *ppos)
{
	int ret;
	char kbuf[MAX_BUF_LEN] = {'\0'};
	char *str;
	int val;

	if (len > MAX_BUF_LEN - 1) {
		return -EINVAL;
	}

	if (copy_from_user(&kbuf, buff, len))
		return -EFAULT;
	kbuf[len] = '\0';

	str = strstrip(kbuf);
	if (!str) {
		return -EINVAL;
	}

	ret = kstrtoint(str, 10, &val);
	if (ret) {
		return -EINVAL;
	}

	if (val < 0 || val > INT_MAX) {
		return -EINVAL;
	}

	printk("fg_mapcount_ops_write is %lu\n", val);
	atomic_long_set(&fg_protect_count, val);

	return len;
}

static ssize_t fg_protect_count_ops_read(struct file *file,
			char __user *buffer, size_t count, loff_t *off)
{
	char kbuf[MAX_BUF_LEN] = {'\0'};
	int len;

	len = snprintf(kbuf, MAX_BUF_LEN, "%lu\n", fg_protect_count);

	if (len > *off)
		len -= *off;
	else
		len = 0;

	if (copy_to_user(buffer, kbuf + *off, (len < count ? len : count)))
		return -EFAULT;

	*off += (len < count ? len : count);
	return (len < count ? len : count);
}

static const struct proc_ops fg_mapcount_enable_ops = {
	.proc_write = fg_mapcount_enable_ops_write,
	.proc_read = fg_mapcount_enable_ops_read,
};

static const struct proc_ops fg_protect_count_ops = {
	.proc_write = fg_protect_count_ops_write,
	.proc_read = fg_protect_count_ops_read,
};
#endif

static int __init mapped_protect_init(void)
{
#ifdef CONFIG_OPLUS_FG_PROTECT
	static struct proc_dir_entry *enable_entry;
	static struct proc_dir_entry *protect_count_entry;
#endif
	int ret = 0;

	ret = register_mapped_protect_vendor_hooks();
	if (ret != 0)
		return ret;

#ifdef CONFIG_MAPPED_PROTECT_ALL
	memavail_noprotected = totalram_pages() / 10;
	mapcount_protected_high[0] = memavail_noprotected >> 1;
	mapcount_protected_high[1] = memavail_noprotected >> 1;

	do_traversal_all_lruvec();

	mapcount_protect_setup = true;
	proc_create_single("mapped_protect_show", 0, NULL, mapcount_protect_show);
#endif

#ifdef CONFIG_OPLUS_FG_PROTECT
	proc_create_single("fg_protect_show", 0, NULL, fg_protect_show);
	enable_entry = proc_create("fg_protect_enable", 0666, NULL, &fg_mapcount_enable_ops);
	protect_count_entry = proc_create("fg_protect_count", 0666, NULL, &fg_protect_count_ops);
#endif

	pr_info("mapped_protect_init succeed!\n");
	return 0;
}

static void __exit mapped_protect_exit(void)
{
	unregister_mapped_protect_vendor_hooks();
	remove_proc_entry("mapped_protect_show", NULL);
	pr_info("mapped_protect_exit exit succeed!\n");

	return;
}

module_init(mapped_protect_init);
module_exit(mapped_protect_exit);

MODULE_LICENSE("GPL v2");
