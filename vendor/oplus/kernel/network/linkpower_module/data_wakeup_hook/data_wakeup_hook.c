/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2018-2020 Oplus. All rights reserved.
 */

#include <linux/kprobes.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/atomic.h>
#include <linux/socket.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <net/tcp.h>
#include "../linkpower_netlink/linkpower_netlink.h"

/* Netlink */
extern int netlink_send_to_user(int msg_type, char *data, int data_len);

static unsigned int my_input_hook_v4(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
static unsigned int my_input_hook_v6(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
static unsigned int my_output_hook_v4(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
static unsigned int my_output_hook_v6(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
static struct nf_hook_ops my_nf_hook_ops[] __read_mostly = {
	{
		.hook = my_input_hook_v4,
		.pf = NFPROTO_IPV4,
		.hooknum = NF_INET_LOCAL_IN,
		.priority = NF_IP_PRI_FIRST,
	},
	{
		.hook = my_input_hook_v6,
		.pf = NFPROTO_IPV6,
		.hooknum = NF_INET_LOCAL_IN,
		.priority = NF_IP_PRI_FIRST,
	},
	{
		.hook = my_output_hook_v4,
		.pf = NFPROTO_IPV4,
		.hooknum = NF_INET_LOCAL_OUT,
		.priority = NF_IP_PRI_LAST,
	},
	{
		.hook = my_output_hook_v6,
		.pf = NFPROTO_IPV6,
		.hooknum = NF_INET_LOCAL_OUT,
		.priority = NF_IP_PRI_LAST,
	},
};

static int handler_pm_system_irq_wakeup(struct kprobe *kp, struct pt_regs *regs);
static struct kprobe kp_pm_system_irq_wakeup = {
	.symbol_name = "pm_system_irq_wakeup",
	.pre_handler = handler_pm_system_irq_wakeup,
};

static atomic_t my_pm_wakeup = ATOMIC_INIT(0);
static DEFINE_SPINLOCK(pm_wakeup_lock);

struct irq_data_wakeup_st
{
	uint32_t udp_in_count;
	uint32_t udp_out_count;
	uint32_t udp_closed_count;
	uint32_t tcp_in_count;
	uint32_t tcp_out_count;
	uint32_t tcp_keepalive_count;
	uint32_t tcp_closed_rst_count;
	uint32_t tcp_closed_fin_count;
	uint32_t tcp_closed_other_count;
};
static struct irq_data_wakeup_st s_irq_data_wakeup;

static uint64_t current_kernel_time(void)
{
	struct timespec64 ts64;

	ktime_get_real_ts64(&ts64);
	return ts64.tv_sec * 1000 + ts64.tv_nsec / 1000000;
}

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

static int handler_pm_system_irq_wakeup(struct kprobe *kp, struct pt_regs *regs)
{
	atomic_set(&my_pm_wakeup, 1);
	return 0;
}

static bool is_tcp_skb(struct sk_buff *skb, bool is_ipv6)
{
	struct iphdr *iph = NULL;
	struct ipv6hdr *ipv6h = NULL;
	struct tcphdr *tcph = NULL;

	if (is_ipv6) {
		ipv6h = ipv6_hdr(skb);
		tcph = tcp_hdr(skb);
		if (skb->protocol != htons(ETH_P_IPV6) || (!ipv6h))
			return false;
		if (ipv6h->nexthdr != NEXTHDR_TCP || !tcph)
			return false;
		if (ipv6h->saddr.s6_addr32[3] == ntohl(1))
			return false;
		if (ipv6h->daddr.s6_addr32[3] == ntohl(1))
			return false;
	} else {
		iph = ip_hdr(skb);
		tcph = tcp_hdr(skb);
		if (skb->protocol != htons(ETH_P_IP) || (!iph))
			return false;
		if (iph->protocol != IPPROTO_TCP || !tcph)
			return false;
		if (iph->saddr == 0x100007F)
			return false;
		if (iph->daddr == 0x100007F)
			return false;
	}
	return true;
}

static bool is_udp_skb(struct sk_buff *skb, bool is_ipv6)
{
	struct iphdr *iph = NULL;
	struct ipv6hdr *ipv6h = NULL;
	struct udphdr *udph = NULL;

	if (is_ipv6) {
		ipv6h = ipv6_hdr(skb);
		udph = udp_hdr(skb);
		if (skb->protocol != htons(ETH_P_IPV6) || (!ipv6h))
			return false;
		if (ipv6h->nexthdr != NEXTHDR_UDP || !udph)
			return false;
		if (ipv6h->saddr.s6_addr32[3] == ntohl(1))
			return false;
		if (ipv6h->daddr.s6_addr32[3] == ntohl(1))
			return false;
	} else {
		iph = ip_hdr(skb);
		udph = udp_hdr(skb);
		if (skb->protocol != htons(ETH_P_IP) || (!iph))
			return false;
		if (iph->protocol != IPPROTO_UDP || !udph)
			return false;
		if (iph->saddr == 0x100007F)
			return false;
		if (iph->daddr == 0x100007F)
			return false;
	}
	return true;
}

static void my_wakeup_hook(struct sk_buff *skb, bool is_ipv6, bool is_output)
{
	int sk_uid = 0, sk_pid = 0;
	uint16_t sport = 0, dport = 0;
	struct sock *sk = NULL;
	struct tcphdr *tcph = NULL;
	struct udphdr *udph = NULL;
	struct tcp_sock *tp = NULL;
	bool is_keepalive = false;
	bool is_tcp = is_tcp_skb(skb, is_ipv6);
	bool is_udp = is_udp_skb(skb, is_ipv6);

	if (!is_udp && !is_tcp)
		return;

	spin_lock_bh(&pm_wakeup_lock);
	if (atomic_read(&my_pm_wakeup) == 0) {
		spin_unlock_bh(&pm_wakeup_lock);
		return;
	}
	atomic_set(&my_pm_wakeup, 0);
	spin_unlock_bh(&pm_wakeup_lock);

	sk = skb_to_full_sk(skb);
	if (!sk) {
		sk_uid = -1;
		sk_pid = -1;
	} else {
		sk_uid = get_uid_from_sock(sk);
		sk_pid = get_pid_from_sock(sk);
	}

	if (is_tcp) {
		tcph = tcp_hdr(skb);
		sport = ntohs(tcph->source);
		dport = ntohs(tcph->dest);
		if (is_output) {
			++s_irq_data_wakeup.tcp_out_count;
		} else {
			++s_irq_data_wakeup.tcp_in_count;
			if (sk) {
				tp = tcp_sk(sk);
				if ((sk->sk_state == TCP_ESTABLISHED) && (ntohl(tcph->seq) == (tp->rcv_nxt - 1))) {
					is_keepalive = true;
					++s_irq_data_wakeup.tcp_keepalive_count;
				}
			} else {
				if (tcph->rst) {
					++s_irq_data_wakeup.tcp_closed_rst_count;
				} else if (tcph->fin) {
					++s_irq_data_wakeup.tcp_closed_fin_count;
				} else {
					++s_irq_data_wakeup.tcp_closed_other_count;
				}
			}
		}
	} else {
		udph = udp_hdr(skb);
		sport = ntohs(udph->source);
		dport = ntohs(udph->dest);
		if (is_output) {
			++s_irq_data_wakeup.udp_out_count;
		} else {
			++s_irq_data_wakeup.udp_in_count;
			if (!sk) {
				++s_irq_data_wakeup.udp_closed_count;
			}
		}
	}

	printk("[data_wakeup_hook] stamp:%llu in:%u protocol:%u port:[%u:%u] uid:[%d:%d] flag:[%u:%u:%u]\n",
	       current_kernel_time(), is_output ? 0 : 1,
	       is_tcp ? IPPROTO_TCP : IPPROTO_UDP,
	       sport, dport, sk_uid, sk_pid,
	       is_tcp ? tcph->fin : 0,
	       is_tcp ? tcph->rst : 0,
	       is_keepalive ? 1 : 0);
}

static unsigned int my_input_hook_v4(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	my_wakeup_hook(skb, false, false);
	return NF_ACCEPT;
}

static unsigned int my_input_hook_v6(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	my_wakeup_hook(skb, true, false);
	return NF_ACCEPT;
}

static unsigned int my_output_hook_v4(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	my_wakeup_hook(skb, false, true);
	return NF_ACCEPT;
}

static unsigned int my_output_hook_v6(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	my_wakeup_hook(skb, true, true);
	return NF_ACCEPT;
}

static int request_irq_data_wakeup(void)
{
	int ret = 0;
	char msg_buf[sizeof(struct irq_data_wakeup_st)] = { 0 };

	memcpy(msg_buf, &s_irq_data_wakeup, sizeof(struct irq_data_wakeup_st));
	memset(&s_irq_data_wakeup, 0x0, sizeof(struct irq_data_wakeup_st));

	ret = netlink_send_to_user(NETLINK_RESPONSE_IRQ_DATA_WAKEUP, (char *)msg_buf, sizeof(msg_buf));
	if (ret < 0) {
		printk("[data_wakeup_hook] request_irq_data_wakeup failed, netlink_send_to_user ret=%d.\n", ret);
	}

	printk("[data_wakeup_hook] request_irq_data_wakeup successfully!");
	return ret;
}

int data_wakeup_hook_netlink_nlmsg_handle(struct sk_buff *skb, struct genl_info *info)
{
	int ret = 0;

	struct nlmsghdr *nlhdr;
	struct genlmsghdr *genlhdr;
	struct nlattr *nla;

	nlhdr = nlmsg_hdr(skb);
	genlhdr = nlmsg_data(nlhdr);
	nla = genlmsg_data(genlhdr);

	switch (nla->nla_type) {
	case NETLINK_REQUEST_IRQ_DATA_WAKEUP:
		ret = request_irq_data_wakeup();
		break;
	default:
		printk("[data_wakeup_hook] data_wakeup_hook_netlink_nlmsg_handle failed, unknown nla type=%d.\n", nla->nla_type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

int data_wakeup_hook_init(void)
{
	int ret = 0;

	ret = register_kprobe(&kp_pm_system_irq_wakeup);
	if (ret < 0) {
		printk("[data_wakeup_hook] register pm_system_irq_wakeup kprobe failed with %d\n", ret);
		return ret;
	}

	ret = nf_register_net_hooks(&init_net, my_nf_hook_ops, ARRAY_SIZE(my_nf_hook_ops));
	if (ret < 0) {
		printk("[data_wakeup_hook] module failed to init netfilter!\n");
		unregister_kprobe(&kp_pm_system_irq_wakeup);
		return ret;
	}

	printk("[data_wakeup_hook] module init successfully!");
	return 0;
}

void data_wakeup_hook_fini(void)
{
	unregister_kprobe(&kp_pm_system_irq_wakeup);
	nf_unregister_net_hooks(&init_net, my_nf_hook_ops, ARRAY_SIZE(my_nf_hook_ops));
}
