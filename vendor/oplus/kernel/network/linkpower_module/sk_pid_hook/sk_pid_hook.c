/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2018-2020 Oplus. All rights reserved.
 */

#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/socket.h>
#include <net/inet_sock.h>
#include "../linkpower_netlink/linkpower_netlink.h"

/* Netlink */
extern int netlink_send_to_user(int msg_type, char *data, int data_len);

/* Kprobe */
#define TCP_CONNECT_NAME "tcp_connect"
#define SYSTEM_UID 1000

static int handler_tcp_connect(struct kprobe *kp, struct pt_regs *regs);

static struct kprobe kp_tcp_connect = {
	.symbol_name = TCP_CONNECT_NAME,
	.pre_handler = handler_tcp_connect,
};

<<<<<<< HEAD
/* Statistics */
#define SK_PID_ARRAY_LEN 50
struct sk_pid_st
=======
static bool boot_monitor_push_sk = false;
static bool boot_monitor_sk_connect = false;
static uint64_t push_sk_transport_stamp = 0;
static netlink_ul_sport_pid_struct sprot_pid_array[SPORT_PID_ARRAY_LEN];
static sk_info_struct destroy_sk_array[DESTROY_SK_ARRAY_LEN];
static monitor_push_sk_struct push_sk_array[PUSH_SK_ARRAY_LEN];
static netlink_ul_sk_connect_info_struct sk_connect_array[SK_CONNECT_ARRAY_LEN];
static uint64_t sk_connect_deadline_array[SK_CONNECT_ARRAY_LEN];

/**
 * @brief      Determine whether the chain is empty.
 *
 * @param[in]  i     The index
 *
 * @return     True if empty, false otherwise.
 */
static bool empty_bucket(int i)
{
	return hlist_nulls_empty(&tcp_hashinfo.ehash[i].chain);
}

/**
 * @brief      Customized current kernel time function.
 *
 * @return     The current kernel time.
 */
static uint64_t current_kernel_time(void)
{
	struct timespec64 ts64;

	ktime_get_real_ts64(&ts64);
	return ts64.tv_sec * 1000 + ts64.tv_nsec / 1000000;
}

/**
 * @brief      Get the uid from sock.
 *
 * @param[in]  sk    The sock
 *
 * @return     The uid from sock.
 */
static uint32_t get_uid_from_sock(const struct sock *sk)
{
	uint32_t sk_uid = 0;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
	const struct file *filp = NULL;
#endif
	if (NULL == sk || !sk_fullsock(sk) || NULL == sk->sk_socket) {
		return 0;
	}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
	filp = sk->sk_socket->file;
	if (NULL == filp) {
		return 0;
	}
	sk_uid = __kuid_val(filp->f_cred->fsuid);
#else
	sk_uid = __kuid_val(sk->sk_uid);
#endif
	return sk_uid;
}

/**
 * @brief      Get the pid from sock.
 *
 * @param[in]  sk    The sock
 *
 * @return     The pid from sock.
 */
static uint32_t get_pid_from_sock(const struct sock *sk)
{
	uint32_t sk_pid = 0;
	if (NULL == sk || !sk_fullsock(sk) || NULL == sk->sk_socket) {
		return 0;
	}
#ifdef CONFIG_ANDROID_KABI_RESERVE
	sk_pid = sk->android_kabi_reserved7;
#endif
	return sk_pid;
}

/**
 * @brief      Free inet_twsk.
 *
 * @param      tw    The timewait sock
 */
static void my_inet_twsk_free(struct inet_timewait_sock *tw)
{
	struct module *owner = tw->tw_prot->owner;
	twsk_destructor((struct sock *)tw);
	kmem_cache_free(tw->tw_prot->twsk_prot->twsk_slab, tw);
	module_put(owner);
}

/**
 * @brief      Sock gen put.
 *
 * @param      sk    The socket
 */
static void my_sock_gen_put(struct sock *sk)
{
	if (!refcount_dec_and_test(&sk->sk_refcnt))
		return;

	if (sk->sk_state == TCP_TIME_WAIT)
		my_inet_twsk_free(inet_twsk(sk));
	else if (sk->sk_state == TCP_NEW_SYN_RECV)
		reqsk_free(inet_reqsk(sk));
	else
		sk_free(sk);
}

/**
 * @brief      Lookup sk by 5-tuple information in sk_info.
 *
 * @param      sk_info  The socket information
 *
 * @return     struct sock* if successful, NULL otherwise.
 */
struct sock *my_lookup_sk(sk_info_struct *sk_info)
{
	struct sock *sk = NULL;

	if (sk_info == NULL) {
		printk("[sk_pid_hook] my_lookup_sk failed, sk_info is null!\n");
		return NULL;
	}

	if (!sk_info->is_ipv6) {
		sk = __inet_lookup_established(&init_net, &tcp_hashinfo, sk_info->v6_daddr32[0],
		                               sk_info->dport, sk_info->v6_saddr32[0],
		                               htons(sk_info->sport), 0, 0);
		if (NULL == sk || !sk_fullsock(sk) || NULL == sk->sk_socket) {
			sk = NULL;
		}
		return sk;
	}
	else {
#if defined(CONFIG_IPV6)
		const struct in6_addr saddr6 =
			{	{	.u6_addr32 = {sk_info->v6_saddr32[0], sk_info->v6_saddr32[1],
					sk_info->v6_saddr32[2], sk_info->v6_saddr32[3]
				}
			}
		};
		const struct in6_addr daddr6 =
			{	{	.u6_addr32 = {sk_info->v6_daddr32[0], sk_info->v6_daddr32[1],
					sk_info->v6_daddr32[2], sk_info->v6_daddr32[3]
				}
			}
		};
		sk = __inet6_lookup_established(&init_net, &tcp_hashinfo, &daddr6,
		                                sk_info->dport, &saddr6,
		                                htons(sk_info->sport), 0, 0);
		if (NULL == sk || !sk_fullsock(sk) || NULL == sk->sk_socket) {
			sk = NULL;
		}
		return sk;
#endif
	}

	return NULL;
}

/**
 * @brief      Monitor Tcp Push Package.
 *
 * @param      sk         The socket
 * @param      skb        The socket buffer
 * @param[in]  is_output  Indicates if output
 * @param[in]  is_ipv6    Indicates if IPv6
 */
static void monitor_push_sk(struct sock *sk, struct sk_buff *skb, bool is_output, bool is_ipv6)
{
	int i = 0;
	int tcp_len = 0;
	int header_len = 0;
	bool match = false;
	uint64_t now = 0;
	uint32_t sk_uid = 0;
	uint8_t *tcp_data = NULL;
	uint8_t *buffer = NULL;
	struct iphdr *iph = NULL;
	struct tcphdr *tcph = NULL;

	if (!boot_monitor_push_sk) {
		return;
	}

	sk_uid = get_uid_from_sock(sk);
	if (sk_uid == 0) {
		return;
	}

	for (i = 0; i < PUSH_SK_ARRAY_LEN; ++i) {
		if (push_sk_array[i].monitor.uid == sk_uid) {
			match = true;
			break;
		}
	}
	if (!match || i >= PUSH_SK_ARRAY_LEN) {
		return;
	}
	tcph = tcp_hdr(skb);
	if (is_ipv6) {
		header_len = sizeof(struct ipv6hdr) + tcph->doff * 4;
	} else {
		iph = ip_hdr(skb);
		header_len = iph->ihl * 4 + tcph->doff * 4;
	}
	tcp_len = skb->len - header_len;
	if (tcp_len <= 0) {
		return;
	}

	if (is_output && tcp_len == push_sk_array[i].monitor.beat_msg_len
	        && tcp_len >= push_sk_array[i].monitor.beat_feature_len) {
		if (push_sk_array[i].sk == sk) {
			push_sk_array[i].beat_count++;
		} else {
			if (push_sk_array[i].monitor.beat_feature_len != 0) {
				buffer = kmalloc(tcp_len, GFP_ATOMIC);
				if (!buffer) {
					return;
				}
				memset(buffer, 0x0, tcp_len);
				tcp_data = (uint8_t *)skb_header_pointer(skb, header_len, tcp_len, buffer);
				if (tcp_data && (sizeof(push_sk_array[i].monitor.beat_feature)
				                 >= push_sk_array[i].monitor.beat_feature_len)
				        && (memcmp(tcp_data, push_sk_array[i].monitor.beat_feature,
				                   push_sk_array[i].monitor.beat_feature_len) == 0)) {
					push_sk_array[i].sk = sk;
					push_sk_array[i].type = UNSOL_PUSH_SOCK_TYPE_DETECTED;
					push_sk_array[i].pid = get_pid_from_sock(sk);
					push_sk_array[i].beat_count++;
					schedule_work(&push_sk_detected_work);
				}
				kfree(buffer);
			} else {
				push_sk_array[i].sk = sk;
				push_sk_array[i].type = UNSOL_PUSH_SOCK_TYPE_DETECTED;
				push_sk_array[i].pid = get_pid_from_sock(sk);
				push_sk_array[i].beat_count++;
				schedule_work(&push_sk_detected_work);
			}
		}
	}

	if (!is_output && tcp_len >= push_sk_array[i].monitor.push_feature_len) {
		if (push_sk_array[i].monitor.push_feature_len == 0) {
			if (push_sk_array[i].sk == sk) {
				now = current_kernel_time();
				if ((now - push_sk_transport_stamp) > PUSH_SK_TRANSPORT_IGNORE_TIME) {
					push_sk_array[i].push_count++;
					push_sk_transport_stamp = now;
				}
			}
		} else {
			buffer = kmalloc(tcp_len, GFP_ATOMIC);
			if (!buffer) {
				return;
			}
			memset(buffer, 0x0, tcp_len);
			tcp_data = (uint8_t *)skb_header_pointer(skb, header_len, tcp_len, buffer);
			if (tcp_data && push_sk_array[i].sk == sk
			        && (sizeof(push_sk_array[i].monitor.push_feature) >= push_sk_array[i].monitor.push_feature_len)
			        && (memcmp(tcp_data, push_sk_array[i].monitor.push_feature,
			                   push_sk_array[i].monitor.push_feature_len) == 0)) {
				now = current_kernel_time();
				if ((now - push_sk_transport_stamp) > PUSH_SK_TRANSPORT_IGNORE_TIME) {
					push_sk_array[i].push_count++;
					push_sk_transport_stamp = now;
				}
			}
			kfree(buffer);
		}
	}
}

/**
 * @brief      Monitor Tcp Close Package.
 *
 * @param      sk         The socket
 * @param      skb        The socket buffer
 * @param[in]  is_output  Indicates if output
 * @param[in]  is_ipv6    Indicates if IPv6
 */
static void monitor_sk_close(struct sock *sk, struct sk_buff *skb, bool is_output, bool is_ipv6)
{
	int i = 0;
	bool match = false;
	uint32_t sk_uid = 0;
	struct tcphdr *tcph = NULL;

	sk_uid = get_uid_from_sock(sk);
	if (sk_uid == 0) {
		return;
	}

	if (boot_monitor_push_sk) {
		for (i = 0; i < PUSH_SK_ARRAY_LEN; ++i) {
			if (push_sk_array[i].monitor.uid == sk_uid && push_sk_array[i].sk == sk) {
				match = true;
				break;
			}
		}
		if (match && i < PUSH_SK_ARRAY_LEN) {
			push_sk_array[i].type = UNSOL_PUSH_SOCK_TYPE_DESTROY;
			schedule_work(&push_sk_destroyed_work);
		}
	}

	tcph = tcp_hdr(skb);
	if (boot_monitor_sk_connect && sk->sk_state == TCP_SYN_SENT && tcph->rst) {
		match = false;
		for (i = 0; i < SK_CONNECT_ARRAY_LEN; ++i) {
			if (sk_connect_array[i].proc.uid == sk_uid) {
				if (sk_connect_array[i].proc.pid == 0) {
					match = true;
				} else if (sk_connect_array[i].proc.pid == get_pid_from_sock(sk)) {
					match = true;
				}
				break;
			}
		}
		if (match && i < SK_CONNECT_ARRAY_LEN && (current_kernel_time() <= sk_connect_deadline_array[i])) {
			if (is_output)
				sk_connect_array[i].syn_snd_rst_count++;
			else
				sk_connect_array[i].syn_rcv_rst_count++;
		}
	}
}

/**
 * @brief      Monitor Tcp Syn Package.
 *
 * @param      sk         The socket
 * @param      skb        The socket buffer
 * @param[in]  is_output  Indicates if output
 * @param[in]  is_ipv6    Indicates if IPv6
 */
static void monitor_sk_syn(struct sock *sk, struct sk_buff *skb, bool is_output, bool is_ipv6)
>>>>>>> edd9e29361cd5a87aca0b42eeb89076f9c40938f
{
	uint16_t sport;
	uint16_t pid;
};
static struct sk_pid_st sk_pid_array[SK_PID_ARRAY_LEN];

/**
 * @brief      The handler of kprobe hook.
 *
 * @param      kp    The kprobe
 * @param      regs  The regs
 *
 * @return     0
 */
static int handler_tcp_connect(struct kprobe *kp, struct pt_regs *regs)
{
	int i = 0;
	bool array_overflow = true;
	int uid = 0, sk_pid = 0;
	int inet_sport = 0;
	struct sock *sk = NULL;
	struct inet_sock *inet = NULL;

	uid = current_uid().val;
	if (uid != SYSTEM_UID) {
		return 0;
	}

	sk_pid = current->tgid;
	sk = (struct sock *) regs->regs[0];
	inet = inet_sk(sk);
	inet_sport = ntohs(inet->inet_sport);

	if ((inet_sport == 0) || (sk_pid == 0)) {
		return 0;
	}
	for (i = 0; i < SK_PID_ARRAY_LEN; i++) {
		if ((sk_pid_array[i].sport == 0) && (sk_pid_array[i].pid == 0)) {
			sk_pid_array[i].sport = (uint16_t) inet_sport;
			sk_pid_array[i].pid = (uint16_t) sk_pid;
			array_overflow = false;
			break;
		}
	}
	if (array_overflow) {
		printk("[sk_pid_hook] sk_pid=%d, sport=%d, array overflow!", sk_pid, inet_sport);
	} else {
		printk("[sk_pid_hook] sk_pid=%d, sport=%d", sk_pid, inet_sport);
	}

	return 0;
}

/**
 * @brief      The handler of request sport and pid info from user space.
 *
 * @return     0 if successful, negative otherwise.
 */
static int request_sk_port_and_pid(void)
{
	int ret = 0;
	char msg_buf[sizeof(struct sk_pid_st) * SK_PID_ARRAY_LEN] = { 0 };

	memcpy(msg_buf, sk_pid_array, sizeof(struct sk_pid_st) * SK_PID_ARRAY_LEN);
	memset(sk_pid_array, 0x0, sizeof(struct sk_pid_st) * SK_PID_ARRAY_LEN);

	ret = netlink_send_to_user(NETLINK_RESPONSE_SK_PORT_AND_PID, (char *)msg_buf, sizeof(msg_buf));
	if (ret < 0) {
		printk("[sk_pid_hook] request_sk_port_and_pid failed, netlink_send_to_user ret=%d.\n", ret);
	}

	printk("[sk_pid_hook] request and reset sk port and pid!");
	return ret;
}

/**
 * @brief      The handler of sk pid hook netlink message from user space.
 *
 * @param      skb   The socket buffer
 * @param      info  The information
 *
 * @return     0 if successful, negative otherwise.
 */
int sk_pid_hook_netlink_nlmsg_handle(struct sk_buff *skb, struct genl_info *info)
{
	int ret = 0;

	struct nlmsghdr *nlhdr;
	struct genlmsghdr *genlhdr;
	struct nlattr *nla;

	nlhdr = nlmsg_hdr(skb);
	genlhdr = nlmsg_data(nlhdr);
	nla = genlmsg_data(genlhdr);

	switch (nla->nla_type) {
	case NETLINK_REQUEST_SK_PORT_AND_PID:
		ret = request_sk_port_and_pid();
		break;
	default:
		printk("[sk_pid_hook] sk_pid_hook_netlink_nlmsg_handle failed, unknown nla type=%d.\n", nla->nla_type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

/**
 * @brief      Initialize sk pid hook.
 *
 * @return     0 if successful, negative otherwise.
 */
int sk_pid_hook_init(void)
{
	int ret = 0;

	ret = register_kprobe(&kp_tcp_connect);
	if (ret < 0) {
		printk("[sk_pid_hook] register tcp connect kprobe failed with %d", ret);
		return ret;
	}

	memset(sk_pid_array, 0x0, sizeof(struct sk_pid_st) * SK_PID_ARRAY_LEN);

	printk("[sk_pid_hook] module init successfully!");
	return 0;
}

/**
 * @brief      Uninstall sk pid hook.
 */
void sk_pid_hook_fini(void)
{
	unregister_kprobe(&kp_tcp_connect);
}
