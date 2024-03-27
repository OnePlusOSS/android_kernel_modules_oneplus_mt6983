/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2018-2020 Oplus. All rights reserved.
 */

#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/version.h>
#include "../linkpower_netlink/linkpower_netlink.h"

/* Netlink */
extern int netlink_send_to_user(int msg_type, char *data, int data_len);

/* Kprobe */
#define CCCI_CCIF_RX_COLLECT_NAME "ccif_rx_collect"
#define CCCI_CCIF_RX_WORK_NAME "ccif_rx_work"
#define CCCI_CCIF_RX_COLLECT_OFFSET_RELEASE_V510 0x288
#define CCCI_CCIF_RX_COLLECT_OFFSET_RELEASE_V515 0x0
#define CCCI_CCIF_RX_COLLECT_OFFSET_RELEASE_V610 0x238
#define CCCI_CCIF_RX_COLLECT_MIPC_OFFSET_RELEASE_V510 0x328
#define CCCI_CCIF_RX_COLLECT_MIPC_OFFSET_RELEASE_V515 0x0
#define CCCI_CCIF_RX_COLLECT_MIPC_OFFSET_RELEASE_V610 0x2C4
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
#define CCCI_CHANNEL_ID_REGS_INDEX 5
#define CCCI_MIPC_MSG_ID_REGS_INDEX 6
#else
#define CCCI_CHANNEL_ID_REGS_INDEX 6
#define CCCI_MIPC_MSG_ID_REGS_INDEX 7
#endif
#define CCCI_CHANNEL_ID_INVALID_CHECK_MASK 0xFFFF0000
#define CCCI_MIPC_MSG_ID_INVALID_CHECK_MASK 0xFFFF0000
#define CCCI_MIPC0_CHANNEL_RX 214
#define CCCI_MIPC9_CHANNEL_TX 233

static int handler_ccif_rx_collect(struct kprobe *kp, struct pt_regs *regs);
static struct kprobe kp_ccif_rx_collect = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
	.symbol_name = CCCI_CCIF_RX_WORK_NAME,
#else
	.symbol_name = CCCI_CCIF_RX_COLLECT_NAME,
#endif
	.pre_handler = handler_ccif_rx_collect,
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(5, 11, 0)))
	.offset = CCCI_CCIF_RX_COLLECT_OFFSET_RELEASE_V510,
#elif ((LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(5, 16, 0)))
	.offset = CCCI_CCIF_RX_COLLECT_OFFSET_RELEASE_V515,
#elif ((LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(6, 2, 0)))
	.offset = CCCI_CCIF_RX_COLLECT_OFFSET_RELEASE_V610,
#else
	.offset = 0,
#endif
};

static int handler_ccif_rx_collect_mipc(struct kprobe *kp, struct pt_regs *regs);
static struct kprobe kp_ccif_rx_collect_mipc = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
	.symbol_name = CCCI_CCIF_RX_WORK_NAME,
#else
	.symbol_name = CCCI_CCIF_RX_COLLECT_NAME,
#endif
	.pre_handler = handler_ccif_rx_collect_mipc,
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(5, 11, 0)))
	.offset = CCCI_CCIF_RX_COLLECT_MIPC_OFFSET_RELEASE_V510,
#elif ((LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(5, 16, 0)))
	.offset = CCCI_CCIF_RX_COLLECT_MIPC_OFFSET_RELEASE_V515,
#elif ((LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(6, 2, 0)))
	.offset = CCCI_CCIF_RX_COLLECT_MIPC_OFFSET_RELEASE_V610,
#else
	.offset = 0,
#endif
};

/* Statistics(reuse qrtr hook statistics struct) */
/* Make sure CCCI_WAKEUP_ARRAY_LEN == QRTR_WAKEUP_ARRAY_LEN */
#define CCCI_WAKEUP_ARRAY_LEN 30
#define CCCI_WAKEUP_IGNORE_TIME_MS 1000
/* Make sure sizeof(ccci_wakeup_st) == sizeof(qrtr_wakeup_st) */
struct ccci_wakeup_st
{
	uint16_t channel_id;
	uint16_t msg_id;
	uint32_t count;
};
static struct ccci_wakeup_st ccci_wakeup_array[CCCI_WAKEUP_ARRAY_LEN];

/**
 * @brief      The handler of kprobe hook.
 *
 * @param      kp    The kprobe
 * @param      regs  The regs
 *
 * @return     0
 */
static int handler_ccif_rx_collect(struct kprobe *kp, struct pt_regs *regs)
{
	int i = 0;
	bool array_overflow = true;
	uint64_t channel_id = 0;

	channel_id = regs->regs[CCCI_CHANNEL_ID_REGS_INDEX];

<<<<<<< HEAD
	if ((now - ccci_wakeup_stamp) < CCCI_WAKEUP_IGNORE_TIME_MS) {
		printk("[ccci_wakeup_hook] channel_id=%lu stamp=%lu, ignore wakeup!", channel_id, now);
		return 0;
	}
	ccci_wakeup_stamp = now;

	if ((channel_id & CCCI_CHANNEL_ID_INVALID_CHECK_MASK) == 0) {
=======
	if (((channel_id & CCCI_CHANNEL_ID_INVALID_CHECK_MASK) == 0) && (channel_id > 0)) {
		if ((channel_id >= CCCI_MIPC0_CHANNEL_RX) && (channel_id <= CCCI_MIPC9_CHANNEL_TX)) {
			return 0;
		}
>>>>>>> edd9e29361cd5a87aca0b42eeb89076f9c40938f
		for (i = 0; i < CCCI_WAKEUP_ARRAY_LEN; i++) {
			if (ccci_wakeup_array[i].channel_id == channel_id) {
				++ccci_wakeup_array[i].count;
				array_overflow = false;
				break;
			}
			if (ccci_wakeup_array[i].channel_id == 0) {
				ccci_wakeup_array[i].channel_id = channel_id;
				ccci_wakeup_array[i].msg_id = 0;
				ccci_wakeup_array[i].count = 1;
				array_overflow = false;
				break;
			}
		}
		if (array_overflow) {
<<<<<<< HEAD
			printk("[ccci_wakeup_hook] channel_id=%lu stamp=%lu, array overflow!", channel_id, now);
		} else {
			printk("[ccci_wakeup_hook] channel_id=%lu stamp=%lu", channel_id, now);
=======
			printk("[ccci_wakeup_hook] channel_id=%llu, array overflow!", channel_id);
		} else {
			printk("[ccci_wakeup_hook] channel_id=%llu", channel_id);
>>>>>>> edd9e29361cd5a87aca0b42eeb89076f9c40938f
		}
	} else {
		printk("[ccci_wakeup_hook] failed to hook ccci wakeup, invalid format [%lu]",
		       regs->regs[CCCI_CHANNEL_ID_REGS_INDEX]);
	}

	return 0;
}

/**
 * @brief      The handler of kprobe hook.
 *
 * @param      kp    The kprobe
 * @param      regs  The regs
 *
 * @return     0
 */
static int handler_ccif_rx_collect_mipc(struct kprobe *kp, struct pt_regs *regs)
{
	int i = 0;
	bool array_overflow = true;
	uint64_t channel_id = 0;
	uint64_t msg_id = 0;

<<<<<<< HEAD
	if ((now - ccci_wakeup_stamp) < CCCI_WAKEUP_IGNORE_TIME_MS) {
		printk("[ccci_wakeup_hook] channel_id=%lu stamp=%lu, ignore wakeup!", channel_id, now);
		return 0;
	}
	ccci_wakeup_stamp = now;
=======
	channel_id = regs->regs[CCCI_CHANNEL_ID_REGS_INDEX];
	msg_id = regs->regs[CCCI_MIPC_MSG_ID_REGS_INDEX];
>>>>>>> edd9e29361cd5a87aca0b42eeb89076f9c40938f

	if (((channel_id & CCCI_CHANNEL_ID_INVALID_CHECK_MASK) == 0)
	        && ((msg_id & CCCI_MIPC_MSG_ID_INVALID_CHECK_MASK) == 0)
	        && (channel_id > 0) && (msg_id > 0)) {
		if ((channel_id < CCCI_MIPC0_CHANNEL_RX) || (channel_id > CCCI_MIPC9_CHANNEL_TX)) {
			return 0;
		}
		for (i = 0; i < CCCI_WAKEUP_ARRAY_LEN; i++) {
			if ((ccci_wakeup_array[i].channel_id == channel_id) && (ccci_wakeup_array[i].msg_id == msg_id)) {
				++ccci_wakeup_array[i].count;
				array_overflow = false;
				break;
			}
			if ((ccci_wakeup_array[i].channel_id == 0) && (ccci_wakeup_array[i].msg_id == 0)) {
				ccci_wakeup_array[i].channel_id = channel_id;
				ccci_wakeup_array[i].msg_id = msg_id;
				ccci_wakeup_array[i].count = 1;
				array_overflow = false;
				break;
			}
		}
		if (array_overflow) {
			printk("[ccci_wakeup_hook] channel_id=%llu mipc_msg_id=0x%llx, array overflow!", channel_id, msg_id);
		} else {
			printk("[ccci_wakeup_hook] channel_id=%llu mipc_msg_id=0x%llx", channel_id, msg_id);
		}
<<<<<<< HEAD
	}
	if (array_overflow) {
		printk("[ccci_wakeup_hook] channel_id=%lu stamp=%lu, array overflow!", channel_id, now);
	} else {
		printk("[ccci_wakeup_hook] channel_id=%lu stamp=%lu", channel_id, now);
=======
	} else {
		printk("[ccci_wakeup_hook] failed to hook ccci mipc wakeup, invalid format [%llu 0x%llx]",
		       regs->regs[CCCI_CHANNEL_ID_REGS_INDEX], regs->regs[CCCI_MIPC_MSG_ID_REGS_INDEX]);
>>>>>>> edd9e29361cd5a87aca0b42eeb89076f9c40938f
	}

	return 0;
}

/**
 * @brief      The handler of request ccci wakeup count from user space.
 *
 * @return     0 if successful, negative otherwise.
 */
static int request_ccci_wakeup()
{
	int ret = 0;
	char msg_buf[sizeof(struct ccci_wakeup_st) * CCCI_WAKEUP_ARRAY_LEN] = { 0 };

	memcpy(msg_buf, ccci_wakeup_array, sizeof(struct ccci_wakeup_st) * CCCI_WAKEUP_ARRAY_LEN);
	memset(ccci_wakeup_array, 0x0, sizeof(struct ccci_wakeup_st) * CCCI_WAKEUP_ARRAY_LEN);

	ret = netlink_send_to_user(NETLINK_RESPONSE_CCCI_WAKEUP, (char *)msg_buf, sizeof(msg_buf));
	if (ret < 0) {
		printk("[ccci_wakeup_hook] request_ccci_wakeup failed, netlink_send_to_user ret=%d.\n", ret);
	}

	printk("[ccci_wakeup_hook] request and reset ccci wakeup!");
	return ret;
}

/**
 * @brief      The handler of qrtr hook netlink message from user space.
 *
 * @param      skb   The socket buffer
 * @param      info  The information
 *
 * @return     0 if successful, negative otherwise.
 */
int ccci_wakeup_hook_netlink_nlmsg_handle(struct sk_buff *skb, struct genl_info *info)
{
	int ret = 0;

	struct nlmsghdr *nlhdr;
	struct genlmsghdr *genlhdr;
	struct nlattr *nla;

	nlhdr = nlmsg_hdr(skb);
	genlhdr = nlmsg_data(nlhdr);
	nla = genlmsg_data(genlhdr);

	switch (nla->nla_type) {
	case NETLINK_REQUEST_CCCI_WAKEUP:
		ret = request_ccci_wakeup();
		break;
	default:
		printk("[ccci_wakeup_hook] ccci_wakeup_hook_netlink_nlmsg_handle failed, unknown nla type=%d.\n", nla->nla_type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

/**
 * @brief      Initialize ccci wakeup hook.
 *
 * @return     0 if successful, negative otherwise.
 */
int ccci_wakeup_hook_init(void)
{
	int ret = 0;

	ret = register_kprobe(&kp_ccif_rx_collect);
	if (ret < 0) {
		printk("[ccci_wakeup_hook] register ccci kprobe failed with %d", ret);
	}

	ret = register_kprobe(&kp_ccif_rx_collect_mipc);
	if (ret < 0) {
		printk("[ccci_wakeup_hook] register ccci_mipc kprobe failed with %d", ret);
	}

	memset(ccci_wakeup_array, 0x0, sizeof(struct ccci_wakeup_st) * CCCI_WAKEUP_ARRAY_LEN);

	printk("[ccci_wakeup_hook] module init successfully!");
	return 0;
}

/**
 * @brief      Uninstall ccci wakeup hook.
 */
void ccci_wakeup_hook_fini(void)
{
	unregister_kprobe(&kp_ccif_rx_collect);
	unregister_kprobe(&kp_ccif_rx_collect_mipc);
}
