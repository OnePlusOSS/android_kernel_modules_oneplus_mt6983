#ifndef __HBP_FRAME_H_
#define __HBP_FRAME_H_

#include <linux/list.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/vmalloc.h>
#include <uapi/linux/sched/types.h>
#include <linux/syscalls.h>
#include <uapi/asm-generic/errno.h>
#include <linux/time.h>
#include "utils/debug.h"

#define MAX_FRAME_COUNT  (100)

enum {
	QUEUE_WAIT,
	QUEUE_WAKEUP,
};

enum irq_reason {
	IRQ_REASON_NORMAL,
	IRQ_REASON_RESET = 0xA0,
	IRQ_REASON_RESET_FWUPDATE,
	IRQ_REASON_RESET_WDT,
	IRQ_REASON_RESET_EXTERNAL,
	IRQ_REASON_RESET_PWR,
};

union touch_time {
	int64_t value[2];
	struct timespec64 tv;
};

struct frame_buf {
	union touch_time frame_tv0;
	union touch_time frame_tv;
	enum irq_reason reason;
	char data[1];
};

struct frame_list {
	struct list_head list;
	unsigned long long frm_count;
	bool cunsumed;  /*true:cunsumed false:ready to consume*/
	unsigned int buf_size;  /*size of buf*/
	unsigned int data_size; /*size of data[0]*/
	struct frame_buf *buf;
};

struct frame_queue {
	struct mutex lock;

	wait_queue_head_t waitq;
	int8_t waitq_flag;

	unsigned int data_size;
	char *data;

	unsigned long long frame_count;

	struct list_head freed;
	struct list_head cunsume;
};

inline int frame_put(char *buf, unsigned int size, enum irq_reason reason, struct frame_queue *queue);
inline int frame_get(char __user *buf, unsigned int size, struct frame_queue *queue);
inline void frame_clear(struct frame_queue *queue);
void frame_wake_up_waitq(struct frame_queue *queue);
void frame_destory(struct frame_queue *queue);
int frame_init(struct frame_queue *queue, int buf_size);

#endif
