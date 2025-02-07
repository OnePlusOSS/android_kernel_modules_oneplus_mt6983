#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>


#ifdef assert
#undef assert
#endif
#define assert(expression) { \
	if (!expression) { \
		(void)panic("assertion %s line:%d\n", __FILE__, __LINE__); \
	} \
}
extern void set_debug_level(int new_level);
extern int get_debug_level(void);
extern int debug_level;

enum {
	LOG_LEVEL_FATAL = 1,
	LOG_LEVEL_ERR = 2,
	LOG_LEVEL_WARN = 3,
	LOG_LEVEL_INFO = 4,
	LOG_LEVEL_CACHE = 5,
	LOG_LEVEL_DEBUG = 6,
	LOG_LEVEL_MAX
};

#define __hbp_printk(ftm, LEVEL, ...) \
		printk("[T%d][touch-krn]["#LEVEL"][%s]" ftm, current->pid, __FUNCTION__,##__VA_ARGS__)

#define hbp_err(ftm, ...) \
	do { \
		if (debug_level >= LOG_LEVEL_ERR) { \
			__hbp_printk(ftm, ERROR, ##__VA_ARGS__); \
		}\
	} while(0)

/*TODO: for user version, we may not reach it*/
#if 0
#define hbp_fatal(ftm, ...) \
	do { \
		__hbp_printk(ftm, FATAL, ##__VA_ARGS__); \
		WARN_ON(1); \
	} while(0)

#else
#define hbp_fatal hbp_err
#endif

#define hbp_info(ftm, ...) \
	do { \
		if (debug_level >= LOG_LEVEL_INFO) { \
			__hbp_printk(ftm, INFO, ##__VA_ARGS__); \
		} \
	} while(0)

#define hbp_warn(ftm, ...) \
	do { \
		if (debug_level >= LOG_LEVEL_WARN) { \
			__hbp_printk(ftm, WARN, ##__VA_ARGS__); \
		} \
	} while(0)

#define hbp_debug(ftm, ...) \
	do { \
		if (debug_level >= LOG_LEVEL_DEBUG) { \
			__hbp_printk(ftm, DEBUG, ##__VA_ARGS__); \
		} \
	} while(0)

#endif
