#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <net/dst.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/netfilter_ipv4.h>
#include <linux/spinlock.h>
#include <linux/notifier.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include "oplus_dev_check.h"

<<<<<<< HEAD
int oplus_dev_check_init(void)
{
	return 0;
=======
#define WLAN1_NAME "wlan1"
#define WLAN1_NAME_LEN 5
#define BLACKHOLE_DEV "blackhole"
#define BLACKHOLE_DEV_LEN 9
#define SLA_WLAN1_MARK 0x200
#define SLA_MARK_MASK 0xfff
#define WLAN1_HOLD_TIME 1000000000 //1000000000nsec equal 1000ms

static unsigned long s_wlan1_event = 0;
static spinlock_t dev_event_lock;
static struct hrtimer dev_hold_timer;
static int timer_start_flag = 0;  //0:timer not started, 1:started
static struct net_device *p_wlan1_dev = 0;
static spinlock_t timer_lock;
static int oplus_dev_hold = 1;
static struct ctl_table_header *oplus_dev_table_hrd = NULL;
static int s_support_dual_sta = 0;

enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
	spin_lock_bh(&timer_lock);
	//check whether timer is start and p_wlan1_dev is null
	if ((timer_start_flag == 0) || (p_wlan1_dev == 0)) {
		printk("oplus_dev:error:timer_start_flag=%d\n", timer_start_flag);
		spin_unlock_bh(&timer_lock);
		return HRTIMER_NORESTART;
	} else {
		printk("oplus_dev:timer_out:dev->name=%s,dev_state=%lu,reg_state=%d,s_wlan1_event=%lu\n",
			p_wlan1_dev->name, p_wlan1_dev->state, p_wlan1_dev->reg_state, s_wlan1_event);
		dev_put(p_wlan1_dev);
		timer_start_flag = 0;
		p_wlan1_dev = 0;
	}
	spin_unlock_bh(&timer_lock);

	return HRTIMER_NORESTART;
}

static inline void update_wlan1_event(unsigned long event)
{
	spin_lock_bh(&dev_event_lock);
	s_wlan1_event = event;
	spin_unlock_bh(&dev_event_lock);
}

static inline void start_wlan1_hold_timer(void)
{
	ktime_t ktimer = ktime_set(0, WLAN1_HOLD_TIME);
	hrtimer_init(&dev_hold_timer, CLOCK_BOOTTIME, HRTIMER_MODE_REL);
	dev_hold_timer.function = timer_callback;
	hrtimer_start(&dev_hold_timer, ktimer, HRTIMER_MODE_REL);
}

static void check_to_hold_wlan1(unsigned long event, struct net_device *dev)
{
	if (!oplus_dev_hold) {
		return;
	}

	if (event == NETDEV_DOWN) {
		spin_lock_bh(&timer_lock);
		if ((timer_start_flag == 0) && (p_wlan1_dev == 0)) {
			p_wlan1_dev = dev;
			dev_hold(dev);
			start_wlan1_hold_timer();
			timer_start_flag = 1;
			printk("oplus_dev:dev_hold:dev->name=%s\n", p_wlan1_dev->name);
		}
		spin_unlock_bh(&timer_lock);
	}
}

static int oplus_netdev_event_handle(struct notifier_block *this, unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

	if (!s_support_dual_sta) {
		return 0;
	}

	if (strncmp(dev->name, "wlan", 4)) {
		return 0;
	}

	printk("oplus_dev:dev->name=%s,dev_state=%lu,reg_state=%d,event=%lu\n",
		dev->name, dev->state, dev->reg_state, event);

	if (!strncmp(dev->name, WLAN1_NAME, WLAN1_NAME_LEN)) {
		update_wlan1_event(event);
		check_to_hold_wlan1(event, dev);
	}

	return 0;
}

static struct notifier_block oplus_netdev_notifier = {
	.notifier_call = oplus_netdev_event_handle,
};

unsigned int oplus_network_skb_dev_check(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	int ret = NF_ACCEPT;
	struct net_device *dev = skb_dst(skb)->dev;

	if (!s_support_dual_sta) {
		return ret;
	}

	/* dev is null, drop the skb */
	if (!dev) {
		printk("oplus_dev:NF_DROP:dev is null!\n");
		return NF_DROP;
	}

	/* blackhole_dev, drop the skb */
	if (!strncmp(dev->name, BLACKHOLE_DEV, BLACKHOLE_DEV_LEN)) {
		printk("oplus_dev:NF_DROP:BLACKHOLE_DEV,s_wlan1_event=%lu\n", s_wlan1_event);
		return NF_DROP;
	}

	spin_lock_bh(&dev_event_lock);
	if (s_wlan1_event == NETDEV_DOWN
		|| s_wlan1_event == NETDEV_UNREGISTER
		|| s_wlan1_event == NETDEV_RELEASE) {
		dev = skb_dst(skb)->dev;
		if (((skb->mark & SLA_MARK_MASK) == SLA_WLAN1_MARK)
			|| !strncmp(dev->name, BLACKHOLE_DEV, BLACKHOLE_DEV_LEN)
			|| !strncmp(dev->name, WLAN1_NAME, WLAN1_NAME_LEN)) {
			printk("oplus_dev:NF_DROP:dev->name=%s,dev_state=%lu,reg_state=%d,s_wlan1_event=%lu\n",
			dev->name, dev->state, dev->reg_state, s_wlan1_event);
			ret = NF_DROP;
		}
	}
	spin_unlock_bh(&dev_event_lock);

	return ret;
}

 void oplus_tuning_dual_sta_support(struct nlattr *nla)
 {
	s_support_dual_sta = 1;
	printk("oplus_dev:s_support_dual_sta=%d\n", s_support_dual_sta);
	return;
}

struct nf_hook_ops oplus_dev_check_netfilter_ops[] __read_mostly =
{
	{
		.hook = oplus_network_skb_dev_check,
		.pf = NFPROTO_IPV4,
		.hooknum = NF_INET_POST_ROUTING,
		.priority = NF_IP_PRI_RAW - 1,
	},
	{
		.hook = oplus_network_skb_dev_check,
		.pf = NFPROTO_IPV6,
		.hooknum = NF_INET_POST_ROUTING,
		.priority = NF_IP_PRI_RAW - 1,
	},
};

static struct ctl_table oplus_dev_sysctl_table[] =
{
	{
		.procname	= "oplus_dev_hold",
		.data		= &oplus_dev_hold,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},
	{}
};

int oplus_dev_check_init(void)
{
	int ret;
	spin_lock_init(&dev_event_lock);
	spin_lock_init(&timer_lock);
	ret = nf_register_net_hooks(&init_net, oplus_dev_check_netfilter_ops, ARRAY_SIZE(oplus_dev_check_netfilter_ops));
	if (ret) {
		printk("register oplus_dev_check_netfilter_ops fail:ret=%d\n", ret);
		return ret;
	}

	ret = register_netdevice_notifier(&oplus_netdev_notifier);
	if (ret) {
		nf_unregister_net_hooks(&init_net, oplus_dev_check_netfilter_ops, ARRAY_SIZE(oplus_dev_check_netfilter_ops));
		printk("register oplus_netdev_notifier fail:ret=%d\n", ret);
	}

	oplus_dev_table_hrd = register_net_sysctl(&init_net, "net/oplus_dev", oplus_dev_sysctl_table);
	if (oplus_dev_table_hrd == NULL) {
  		printk("register oplus_dev_table_hrd fail!");
	}

	return ret;
>>>>>>> edd9e29361cd5a87aca0b42eeb89076f9c40938f
}

void oplus_dev_check_fini(void)
{
<<<<<<< HEAD
=======
	nf_unregister_net_hooks(&init_net, oplus_dev_check_netfilter_ops, ARRAY_SIZE(oplus_dev_check_netfilter_ops));
	unregister_netdevice_notifier(&oplus_netdev_notifier);
	if (oplus_dev_table_hrd) {
 		unregister_net_sysctl_table(oplus_dev_table_hrd);
 		oplus_dev_table_hrd = 0;
	}
>>>>>>> edd9e29361cd5a87aca0b42eeb89076f9c40938f
}
