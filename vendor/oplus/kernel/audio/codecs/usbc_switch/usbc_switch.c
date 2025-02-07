// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2022-2024, The Linux Foundation. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/power_supply.h>
#include <linux/mutex.h>
#include <linux/usb/typec.h>
#include <linux/usb/ucsi_glink.h>
#include <linux/iio/consumer.h>
#include <linux/cdev.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include "usbc_switch.h"

/*2022/01/08 add for 3rd protocal stack notifier*/
#if IS_ENABLED(CONFIG_TCPC_CLASS)
#include <linux/usb/tcpci.h>
#include "tcpm.h"
#endif

#ifdef OPLUS_FEATURE_MM_FEEDBACK
#include <soc/oplus/system/oplus_mm_kevent_fb.h>
#endif

#define USBC_SWITCH_DRIVER_NAME "usbc_switch"
#define MINOR_NUMBER_COUNT 1

#undef dev_dbg
#define dev_dbg dev_info

/*2022/01/08 add for 3rd protocal stack notifier*/
#if IS_ENABLED(CONFIG_TCPC_CLASS)
static int probe_retry = 0;
#endif

struct usbc_switch_dev {
	struct class *cls;
	struct device *dev;
	struct cdev cdev;
	dev_t dev_num;
};

struct usbc_switch_priv {
	struct device *dev;
	struct usbc_switch_dev *adev;
	struct power_supply *usb_psy;
	struct notifier_block nb;
	struct iio_channel *iio_ch;
	atomic_t usbc_mode;
	struct work_struct usbc_analog_work;
	struct blocking_notifier_head usbc_switch_notifier;
	struct mutex notification_lock;
	u32 use_powersupply;
	unsigned int mic_gnd_status;
	unsigned int mic_gnd_swh_pin;
	unsigned int mic_gnd_swh_pin_2;
	unsigned int lr_sel_pin;
	unsigned int hs_det_pin;
	unsigned int pmic_gpio6;
	int switch_control;
	/*2022/02/04, add for type-c headphones cclogic sgm7220 notifiy */
	int is_audio_input;
	u32 use_cclogic_det;
	/*2022/01/12, add for 3rd usb protocal support*/
	unsigned int usb_protocal;
};

static struct usbc_switch_priv *g_swh_priv = NULL;

static ssize_t usbc_switch_read(struct file *p_file,
			 char __user *puser_buf, size_t count, loff_t *p_offset)
{
	return 0;
}

static ssize_t usbc_switch_write(struct file *p_file,
			 const char __user *puser_buf,
			 size_t count, loff_t *p_offset)
{
	return 0;
}

static const struct file_operations usbc_switch_fops = {
	.owner =                THIS_MODULE,
	.read =                 usbc_switch_read,
	.write =                usbc_switch_write,
};

static int usbc_switch_usbc_event_changed_psupply(struct usbc_switch_priv *swh_priv,
					unsigned long evt, void *ptr)
{
	struct device *dev = NULL;

	if (!swh_priv)
		return -EINVAL;

	dev = swh_priv->dev;
	if (!dev)
		return -EINVAL;
	dev_dbg(dev, "%s: queueing usbc_analog_work\n",
		__func__);
	pm_stay_awake(swh_priv->dev);
	queue_work(system_freezable_wq, &swh_priv->usbc_analog_work);

	return 0;
}

static int usbc_switch_usbc_event_changed_ucsi(struct usbc_switch_priv *swh_priv,
				      unsigned long evt, void *ptr)
{
	struct device *dev;
	/*2022/02/04, add for type-c headphones cclogic sgm7220 notifiy */
	enum typec_accessory acc;
#if IS_ENABLED(CONFIG_TCPC_CLASS)
	struct tcp_notify *noti = ptr;
	int old_state = TYPEC_UNATTACHED;
	int new_state = TYPEC_UNATTACHED;
#endif
	pr_err("%s enter! evt = %d \n", __func__, evt);
	if (swh_priv->use_cclogic_det) {
		acc = evt;
	/*2022/01/08 add for 3rd protocal stack notifier*/
	} else if (swh_priv->usb_protocal != 1) {
		acc = ((struct ucsi_glink_constat_info *)ptr)->acc;
	} else {
		acc = TYPEC_ACCESSORY_NONE;
	}

	if (!swh_priv)
		return -EINVAL;

	dev = swh_priv->dev;
	if (!dev)
		return -EINVAL;

	if (swh_priv->usb_protocal == 1) {
#if IS_ENABLED(CONFIG_TCPC_CLASS)
		dev_err(dev, "%s: USB change event received, new_state:%d, old_state:%d\n",
				__func__, noti->typec_state.new_state, noti->typec_state.old_state);
#endif
	} else {
		dev_err(dev, "%s: USB change event received, supply mode %d, usbc mode %ld, expected %d\n",
				__func__, acc, swh_priv->usbc_mode.counter,
				TYPEC_ACCESSORY_AUDIO);
	}

/*2022/01/08 add for 3rd protocal stack notifier*/
	if (swh_priv->usb_protocal == 1) {
#if IS_ENABLED(CONFIG_TCPC_CLASS)
		switch (evt) {
		case TCP_NOTIFY_TYPEC_STATE:
			dev_err(dev, "case TCP_NOTIFY_TYPEC_STATE\n");
			old_state = noti->typec_state.old_state;
			new_state = noti->typec_state.new_state;
			if (old_state == TYPEC_UNATTACHED &&
			    new_state == TYPEC_ATTACHED_AUDIO) {
				dev_err(dev, "Audio plug in\n");
				/* enable AudioAccessory connection */
				acc = TYPEC_ACCESSORY_AUDIO;

				if (gpio_is_valid(g_swh_priv->pmic_gpio6)) {
					dev_err(dev, "%s: pull up pmic_gpio6.\n", __func__);
					gpio_direction_output(g_swh_priv->pmic_gpio6, 1);
				}
			} else if (old_state == TYPEC_ATTACHED_AUDIO &&
				   new_state == TYPEC_UNATTACHED) {
				dev_err(dev, "Audio plug out\n");
				/* disable AudioAccessory connection */
				acc = TYPEC_ACCESSORY_NONE;

				if (gpio_is_valid(g_swh_priv->pmic_gpio6)) {
					dev_err(dev, "%s: pull down pmic_gpio6.\n", __func__);
					gpio_direction_output(g_swh_priv->pmic_gpio6, 0);
				}
			}
			break;
		default:
			return 0;
		}
#endif
	}

	switch (acc) {
	case TYPEC_ACCESSORY_AUDIO:
	case TYPEC_ACCESSORY_NONE:
		if (atomic_read(&(swh_priv->usbc_mode)) == acc)
			break; /* filter notifications received before */
		atomic_set(&(swh_priv->usbc_mode), acc);

		dev_dbg(dev, "%s: queueing usbc_analog_work\n",
			__func__);
		pm_stay_awake(swh_priv->dev);
		queue_work(system_freezable_wq, &swh_priv->usbc_analog_work);
		break;
	default:
		break;
	}

	return 0;
}

static int usbc_switch_usbc_event_changed(struct notifier_block *nb_ptr,
				      unsigned long evt, void *ptr)
{
	struct usbc_switch_priv *swh_priv =
			container_of(nb_ptr, struct usbc_switch_priv, nb);
	struct device *dev;

	/* 2022/02/04, add for type-c headphones cclogic sgm7220 notifiy */
	if (swh_priv->use_cclogic_det) {
		swh_priv->is_audio_input = evt;
		pr_err("%s enter! evt = %d use_powersupply=%d\n", __func__, evt, swh_priv->use_powersupply);
	}

	if (!swh_priv)
		return -EINVAL;

	dev = swh_priv->dev;
	if (!dev)
		return -EINVAL;

	if (swh_priv->use_powersupply)
		return usbc_switch_usbc_event_changed_psupply(swh_priv, evt, ptr);
	else
		return usbc_switch_usbc_event_changed_ucsi(swh_priv, evt, ptr);
}

static int usbc_switch_setup_switches(struct usbc_switch_priv *swh_priv)
{
	int rc = 0;
	union power_supply_propval ps_mode;
	int mode;
	struct device *dev;

	if (!swh_priv)
		return -EINVAL;
	dev = swh_priv->dev;
	if (!dev)
		return -EINVAL;

	mutex_lock(&swh_priv->notification_lock);
	if (swh_priv->use_powersupply) {
		/* get latest mode again within locked context */
		rc = iio_read_channel_processed(swh_priv->iio_ch, &ps_mode.intval);
		if (rc < 0) {
			dev_err(dev, "%s: Unable to read USB TYPEC_MODE: %d\n",
				__func__, rc);
			return rc;
		}
		mode = ps_mode.intval;
	} else {
		/* get latest mode again within locked context */
		mode = atomic_read(&(swh_priv->usbc_mode));
	}
	dev_info(dev, "%s: setting GPIOs active = %d, rcvd mode %d\n",
			__func__, mode != TYPEC_ACCESSORY_NONE, mode);

	switch (mode) {
	case TYPEC_ACCESSORY_AUDIO:
		/* notify call chain on event */
		blocking_notifier_call_chain(&swh_priv->usbc_switch_notifier,
					mode, NULL);
		if (gpio_is_valid(swh_priv->hs_det_pin)) {
			dev_info(dev, "%s: set hs_det_pin to low.\n", __func__);
			gpio_direction_output(swh_priv->hs_det_pin, 0);
		}
		break;
	case TYPEC_ACCESSORY_NONE:
		if (gpio_is_valid(swh_priv->hs_det_pin)) {
			dev_info(dev, "%s: set hs_det_pin to high.\n", __func__);
			gpio_direction_output(swh_priv->hs_det_pin, 1);
		}
		/* notify call chain on event */
		blocking_notifier_call_chain(&swh_priv->usbc_switch_notifier,
				TYPEC_ACCESSORY_NONE, NULL);
		break;
	default:
		/* ignore other usb connection modes */
		break;
	}

	mutex_unlock(&swh_priv->notification_lock);
	return rc;
}

int usbc_switch_hs_event(enum usbc_switch_event event)
{
	int gpio1=100, gpio2=200;
	if (!g_swh_priv)
		return -EINVAL;

	pr_info("%s - switch headset event: %d\n", __func__, event);

	switch (event) {
	case HS_MIC_GND_SWAP:
		if (gpio_is_valid(g_swh_priv->mic_gnd_swh_pin)) {
			if (g_swh_priv->mic_gnd_status == 0) {
				gpio_direction_output(g_swh_priv->mic_gnd_swh_pin, 1);
				if (gpio_is_valid(g_swh_priv->mic_gnd_swh_pin_2)) {
					gpio_direction_output(g_swh_priv->mic_gnd_swh_pin_2, 1);
				}
				g_swh_priv->mic_gnd_status = 1;
			} else {
				gpio_direction_output(g_swh_priv->mic_gnd_swh_pin, 0);
				if (gpio_is_valid(g_swh_priv->mic_gnd_swh_pin_2)) {
					gpio_direction_output(g_swh_priv->mic_gnd_swh_pin_2, 0);
				}
				g_swh_priv->mic_gnd_status = 0;
			}
			gpio1 = gpio_get_value(g_swh_priv->mic_gnd_swh_pin);
			gpio2 = gpio_get_value(g_swh_priv->mic_gnd_swh_pin_2);
			pr_info("%s: swap mic gnd, set mic_gnd_swh_pin %d, gpio1=%d, gpio2=%d\n", \
					__func__, g_swh_priv->mic_gnd_status, gpio1, gpio2);
			//msleep(20000);
			/* wcd-mbhc-adc.c swap_gnd_mic determine swap success by return value !0 */
			return 1;
		}
		break;

	case HS_LR_CONNECT:
		if (gpio_is_valid(g_swh_priv->lr_sel_pin)) {
			pr_info("%s: connect LR.\n", __func__);
			gpio_direction_output(g_swh_priv->lr_sel_pin, 1);
		}
		break;

	case HS_LR_DISCONNECT:
		if (gpio_is_valid(g_swh_priv->lr_sel_pin)) {
			pr_info("%s: disconnect LR.\n", __func__);
			gpio_direction_output(g_swh_priv->lr_sel_pin, 0);
		}
		break;

	case HS_PLUG_OUT:
		if (gpio_is_valid(g_swh_priv->mic_gnd_swh_pin)) {
			pr_info("%s: set mic_gnd_swh_pin 0\n", __func__);
			gpio_direction_output(g_swh_priv->mic_gnd_swh_pin, 0);
			if (gpio_is_valid(g_swh_priv->mic_gnd_swh_pin_2)) {
				gpio_direction_output(g_swh_priv->mic_gnd_swh_pin_2, 0);
			}
			g_swh_priv->mic_gnd_status = 0;
		}
		if (gpio_is_valid(g_swh_priv->lr_sel_pin)) {
			pr_info("%s: disconnect LR.\n", __func__);
			gpio_direction_output(g_swh_priv->lr_sel_pin, 0);
		}
		break;
	default:
		break;
	}

	return 0;
}
EXPORT_SYMBOL(usbc_switch_hs_event);

int usbc_switch_reg_notifier(struct notifier_block *nb,
			 struct device_node *node)
{
	int rc = 0;

	pr_info("%s: !g_swh_priv=%d.\n", __func__, !g_swh_priv);

	if (!g_swh_priv)
		return -EINVAL;

	rc = blocking_notifier_chain_register
				(&g_swh_priv->usbc_switch_notifier, nb);
	if (rc)
		return rc;
	/*
	 * as part of the init sequence check if there is a connected
	 * USB C analog adapter
	 */
	dev_dbg(g_swh_priv->dev, "%s: verify if USB adapter is already inserted\n",
		__func__);
	rc = usbc_switch_setup_switches(g_swh_priv);

	return rc;
}
EXPORT_SYMBOL(usbc_switch_reg_notifier);

int usbc_switch_unreg_notifier(struct notifier_block *nb,
			     struct device_node *node)
{
	int rc = 0;

	if (!g_swh_priv)
		return -EINVAL;

	mutex_lock(&g_swh_priv->notification_lock);

	rc = blocking_notifier_chain_unregister
				(&g_swh_priv->usbc_switch_notifier, nb);
	mutex_unlock(&g_swh_priv->notification_lock);

	return rc;
}
EXPORT_SYMBOL(usbc_switch_unreg_notifier);

static void usbc_switch_work_fn(struct work_struct *work)
{
	struct usbc_switch_priv *swh_priv =
		container_of(work, struct usbc_switch_priv, usbc_analog_work);

	if (!swh_priv) {
		pr_err("%s: audio switch container invalid\n", __func__);
		return;
	}
	usbc_switch_setup_switches(swh_priv);
	pm_relax(swh_priv->dev);
}

static int usbc_switch_parse_dt(struct usbc_switch_priv *swh_priv,
	struct device *dev)
{
	struct device_node *dNode = dev->of_node;
	u32 use_powersupply;
	u32 use_cclogic_det = 0;
	int ret = 0;
	pr_info("%s: enter\n", __func__);

	if (dNode == NULL) {
		return -ENODEV;
	}

	if (!swh_priv) {
		pr_err("%s: swh_priv is NULL\n", __func__);
		return -ENOMEM;
	}

	ret = of_property_read_u32(dNode,
				"oplus,use-cclogic-det", &use_cclogic_det);
	if (ret || use_cclogic_det == 0) {
		swh_priv->use_cclogic_det = 0;
	} else {
		swh_priv->use_cclogic_det = 1;
	}
	dev_err(swh_priv->dev, "%s: use_cclogic_det = %d\n",
			__func__, swh_priv->use_cclogic_det);

	swh_priv->mic_gnd_swh_pin = of_get_named_gpio(dNode,
			"mic-gnd-swh-gpio", 0); //LR_SEL1
	if (!gpio_is_valid(swh_priv->mic_gnd_swh_pin)) {
		pr_err("%s: mic-gnd-swh-gpio in dt node is missing\n", __func__);
		return -ENODEV;
	} else {
		ret = gpio_request(swh_priv->mic_gnd_swh_pin, "audio_mic_gnd_swh");
		if (ret) {
			pr_err("%s: mic-gnd-swh-gpio request fail\n", __func__);
			return ret;
		}
		pr_err("%s: mic-gnd-swh-gpio init as low\n", __func__);
		gpio_direction_output(swh_priv->mic_gnd_swh_pin, 0);
	}

	swh_priv->mic_gnd_swh_pin_2 = of_get_named_gpio(dNode,
			"mic-gnd-swh-gpio-2", 0); //LR_SEL2
	if (!gpio_is_valid(swh_priv->mic_gnd_swh_pin_2)) {
		pr_err("%s: mic-gnd-swh-gpio-2 node is missing or is not support\n", __func__);
		return -ENODEV;
	} else {
		ret = gpio_request(swh_priv->mic_gnd_swh_pin_2, "audio_mic_gnd_swh_2");
		if (ret) {
			pr_err("%s: mic-gnd-swh-gpio-2 request fail\n", __func__);
			return ret;
		}
		pr_err("%s: mic-gnd-swh-gpio_2 init as low\n", __func__);
		gpio_direction_output(swh_priv->mic_gnd_swh_pin_2, 0);
	}

	swh_priv->mic_gnd_status = 0;

	swh_priv->lr_sel_pin = of_get_named_gpio(dNode,
			"lr-sel-gpio", 0);//PM6375 GPIO 7
	if (!gpio_is_valid(swh_priv->lr_sel_pin)) {
		pr_err("%s: lr-sel-gpio in dt node is missing\n", __func__);
		return -ENODEV;
	} else {
		ret = gpio_request(swh_priv->lr_sel_pin, "audio_lr_select");
		if (ret) {
			pr_err("%s: lr-sel-gpio request fail\n", __func__);
			return ret;
		}
		pr_err("%s: lr-sel-gpio request init as low\n", __func__);
		gpio_direction_output(swh_priv->lr_sel_pin, 0);
	}

	swh_priv->hs_det_pin = of_get_named_gpio(dNode,
			"hs-det-gpio", 0); //GOIO 154
	if (!gpio_is_valid(swh_priv->hs_det_pin)) {
		pr_err("%s: hs-det-gpio in dt node is missing\n", __func__);
		return -ENODEV;
	} else {
		ret = gpio_request(swh_priv->hs_det_pin, "audio_hs_det");
		if (ret) {
			pr_err("%s: hs-det-gpio request fail\n", __func__);
			return ret;
		}
		pr_err("%s: hs-det-gpio request init as high\n", __func__);
		gpio_direction_output(swh_priv->hs_det_pin, 1);
	}

	swh_priv->pmic_gpio6 = of_get_named_gpio(dNode,
			"pm6375_gpio6", 0);
	if (!gpio_is_valid(swh_priv->pmic_gpio6)) {
		pr_err("%s: pm6375_gpio6 in dt node is missing\n", __func__);
	} else {
		ret = gpio_request(swh_priv->pmic_gpio6, "pmic_gpio6");
		if (ret) {
			pr_err("%s: pmic_gpio6 request fail\n", __func__);
		} else {
			pr_err("%s: pmic_gpio6 request init as low\n", __func__);
			gpio_direction_output(swh_priv->pmic_gpio6, 0);
		}
	}

	pr_err("%s: mic_gnd_swh_pin:%d, lr_sel_pin:%d, hs_det_pin:%d\n", __func__, \
		swh_priv->mic_gnd_swh_pin, swh_priv->lr_sel_pin, swh_priv->hs_det_pin);

	/* 2022/02/04, add for type-c headphones cclogic sgm7220 notifiy */
	ret = of_property_read_u32(dNode, "qcom,use-power-supply", &use_powersupply);
	if (ret || use_powersupply == 0) {
		swh_priv->use_powersupply = 0;
	} else {
		swh_priv->use_powersupply = 1;
	}
	pr_info("%s: use_powersupply = %d\n", __func__, swh_priv->use_powersupply);


	ret = of_property_read_u32(dNode, "use-3rd-usb-protocal", &swh_priv->usb_protocal);
	if (ret) {
		pr_info("%s: use-3rd-usb-protocal in dt node is missing\n", __func__);
		swh_priv->usb_protocal = 0;
	}

	/* 2022/02/04, add for type-c headphones cclogic sgm7220 notifiy */
	ret = of_property_read_u32(dNode, "oplus,use-cclogic-det", &use_cclogic_det);
	if (ret || use_cclogic_det == 0) {
		swh_priv->use_cclogic_det = 0;
	} else {
		swh_priv->use_cclogic_det = 1;
	}
	pr_info("%s: use_cclogic_det = %d\n",
			__func__, swh_priv->use_cclogic_det);


	pr_info("%s: exit ret=%d\n", __func__, ret);

	return 0;
}

#if 0
/* 2022/02/04, add for type-c headphones cclogic sgm7220 notifiy */
extern int cc_audio_register_notify(struct notifier_block *nb);

extern int cc_audio_unregister_notify(struct notifier_block *nb);
#endif

static int usbc_switch_probe(struct platform_device *pdev)
{
	int rc;
	struct usbc_switch_dev *adev;
	/* 2022/01/08 add for 3rd protocal stack notifer */
#if IS_ENABLED(CONFIG_TCPC_CLASS)
	struct tcpc_device *tcpc;
#endif
	struct usbc_switch_priv *swh_priv = NULL;

	pr_info("%s: enter\n", __func__);

	adev = devm_kzalloc(&pdev->dev, sizeof(struct usbc_switch_dev), GFP_KERNEL);
	if (!adev) {
		return-ENOMEM;
	}

	rc = alloc_chrdev_region(&adev->dev_num, 0, MINOR_NUMBER_COUNT,
				  USBC_SWITCH_DRIVER_NAME);
	if (rc < 0) {
		dev_err(&pdev->dev, "%s: Failed to alloc char dev, err = %d\n",
			__func__, rc);
		goto err_chrdev;
	}

	adev->cls = class_create(THIS_MODULE, USBC_SWITCH_DRIVER_NAME);
	if (IS_ERR(adev->cls)) {
		rc = PTR_ERR(adev->cls);
		dev_err(&pdev->dev, "%s: Failed to create class, err = %d\n",
			__func__, rc);
		goto err_class;
	}

	adev->dev = device_create(adev->cls, NULL, adev->dev_num,
				  NULL, USBC_SWITCH_DRIVER_NAME);
	if (IS_ERR(adev->dev)) {
		rc = PTR_ERR(adev->dev);
		dev_err(&pdev->dev, "%s: Failed to create device, err = %d\n",
			__func__, rc);
		goto err_dev_create;
	}

	cdev_init(&adev->cdev, &usbc_switch_fops);
	rc = cdev_add(&adev->cdev, adev->dev_num, MINOR_NUMBER_COUNT);
	if (rc < 0) {
		dev_err(&pdev->dev, "%s: Failed to register char dev, err = %d\n",
			__func__, rc);
		goto err_cdev_add;
	}

	swh_priv = devm_kzalloc(&pdev->dev,
			sizeof(struct usbc_switch_priv), GFP_KERNEL);
	if (!swh_priv) {
		dev_err(&pdev->dev, "%s: devm_kzalloc failed\n", __func__);
		goto err_cdev_add;
	}

	swh_priv->adev = adev;
	swh_priv->dev = adev->dev;
	rc = usbc_switch_parse_dt(swh_priv, &pdev->dev);
	if (rc < 0) {
		dev_err(&pdev->dev, "%s: Failed to parse dt, err = %d\n",
			__func__, rc);
		goto err_parse_dt;
	}

	swh_priv->nb.notifier_call = usbc_switch_usbc_event_changed;
	swh_priv->nb.priority = 0;
	if (swh_priv->use_powersupply == 0) {
		/* 2022/02/04, add for type-c headphones cclogic sgm7220 notifiy */
		/*
		if (swh_priv->use_cclogic_det)
		{
			rc = cc_audio_register_notify(&swh_priv->nb);
			if (rc < 0) {
				pr_err("blocking_notifier_chain_register error");
				goto err_parse_dt;
			}
		} else */
		if (swh_priv->usb_protocal != 1) {
			rc = register_ucsi_glink_notifier(&swh_priv->nb);
			if (rc) {
				dev_err(&pdev->dev,
				"%s: ucsi glink notifier registration failed: %d\n",
				__func__, rc);
				goto err_parse_dt;
			}
		} else {
#if IS_ENABLED(CONFIG_TCPC_CLASS)
			dev_err(&pdev->dev, "%s: start register 3rd protocal stack notifier\n", __func__);
			tcpc = tcpc_dev_get_by_name("type_c_port0");
			if (!tcpc) {
				if (probe_retry > 30) {
					dev_err(&pdev->dev, "%s: get tcpc failed, jump tcp register\n", __func__);
					rc = 0;
					goto tcp_register_finish;
				} else {
					probe_retry++;
					dev_err(&pdev->dev, "%s: get tcpc failed, retry:%d \n", __func__, probe_retry);
					usleep_range(1*1000, 1*1005);
					rc = -EPROBE_DEFER;
					goto err_parse_dt;
				}
			}
			rc = register_tcp_dev_notifier(tcpc, &swh_priv->nb, TCP_NOTIFY_TYPE_USB);
			if (rc) {
				dev_err(&pdev->dev, "%s: ucsi glink notifier registration failed: %d\n",
					__func__, rc);
				goto err_parse_dt;
			}
#endif
		}
	} else {
		swh_priv->use_powersupply = 1;
		swh_priv->usb_psy = power_supply_get_by_name("usb");
		if (!swh_priv->usb_psy) {
			rc = -EPROBE_DEFER;
			dev_dbg(&pdev->dev,
				"%s: could not get USB psy info: %d\n",
				__func__, rc);
			goto err_parse_dt;
		}

		/* 2022/02/04, add for type-c headphones cclogic sgm7220 notifiy */
		if (!swh_priv->use_cclogic_det) {
			swh_priv->iio_ch = iio_channel_get(swh_priv->dev, "typec_mode");
			if (!swh_priv->iio_ch) {
				dev_err(&pdev->dev,
					"%s: iio_channel_get failed for typec_mode\n",
					__func__);
				goto err_supply;
			}
			rc = power_supply_reg_notifier(&swh_priv->nb);
			if (rc) {
				dev_err(&pdev->dev,
					"%s: power supply reg failed: %d\n",
				__func__, rc);
				goto err_supply;
			}
		}
	}

#if IS_ENABLED(CONFIG_TCPC_CLASS)
tcp_register_finish:
#endif
	mutex_init(&swh_priv->notification_lock);

	INIT_WORK(&swh_priv->usbc_analog_work,
		usbc_switch_work_fn);

	swh_priv->usbc_switch_notifier.rwsem =
		(struct rw_semaphore)__RWSEM_INITIALIZER
		((swh_priv->usbc_switch_notifier).rwsem);
	swh_priv->usbc_switch_notifier.head = NULL;

	platform_set_drvdata(pdev, swh_priv);
	g_swh_priv = swh_priv;
	pr_info("%s: exit, rc=%d\n", __func__, rc);
	return 0;

err_supply:
	power_supply_put(swh_priv->usb_psy);
err_parse_dt:
	if (swh_priv && gpio_is_valid(swh_priv->hs_det_pin)) {
		gpio_free(swh_priv->hs_det_pin);
	}
	if (swh_priv && gpio_is_valid(swh_priv->lr_sel_pin)) {
		gpio_free(swh_priv->lr_sel_pin);
	}
	if (swh_priv && gpio_is_valid(swh_priv->mic_gnd_swh_pin)) {
		gpio_free(swh_priv->mic_gnd_swh_pin);
	}
	if (gpio_is_valid(swh_priv->mic_gnd_swh_pin_2)) {
		gpio_free(swh_priv->mic_gnd_swh_pin_2);
	}
	if (gpio_is_valid(swh_priv->pmic_gpio6)) {
		gpio_free(swh_priv->pmic_gpio6);
	}
	if (swh_priv) {
		pr_info("%s: devm_kfree swh_priv\n", __func__);
		devm_kfree(&pdev->dev, swh_priv);
	}
err_cdev_add:
	device_destroy(adev->cls, adev->dev_num);
err_dev_create:
	class_destroy(adev->cls);
err_class:
	unregister_chrdev_region(0, MINOR_NUMBER_COUNT);
err_chrdev:
	if (adev) {
		pr_info("%s: devm_kfree adev\n", __func__);
		devm_kfree(&pdev->dev, adev);
	}
	pr_info("%s: exit, rc=%d\n", __func__, rc);
	return rc;
}

static int usbc_switch_remove(struct platform_device *pdev)
{
	struct usbc_switch_priv *swh_priv = platform_get_drvdata(pdev);

	pr_info("%s: enter\n", __func__);

	if (!swh_priv)
		return -EINVAL;

	if (swh_priv->use_powersupply) {
		/* deregister from PMI */
		power_supply_unreg_notifier(&swh_priv->nb);
		power_supply_put(swh_priv->usb_psy);
	} else {
	#if 0
		/* 2022/02/04, add for type-c headphones cclogic sgm7220 notifiy */
		if (swh_priv->use_cclogic_det)
			cc_audio_unregister_notify(&swh_priv->nb);
		else
	#endif
		unregister_ucsi_glink_notifier(&swh_priv->nb);
	}
	pr_info("%s: test 1\n", __func__);

	cancel_work_sync(&swh_priv->usbc_analog_work);
	pm_relax(swh_priv->dev);
	mutex_destroy(&swh_priv->notification_lock);
	pr_info("%s: test 2\n", __func__);

	if (gpio_is_valid(swh_priv->hs_det_pin)) {
		gpio_free(swh_priv->hs_det_pin);
	}
	if (gpio_is_valid(swh_priv->lr_sel_pin)) {
		gpio_free(swh_priv->lr_sel_pin);
	}
	if (gpio_is_valid(swh_priv->mic_gnd_swh_pin)) {
		gpio_free(swh_priv->mic_gnd_swh_pin);
	}
	if (gpio_is_valid(swh_priv->mic_gnd_swh_pin_2)) {
		gpio_free(swh_priv->mic_gnd_swh_pin_2);
	}
	if (gpio_is_valid(swh_priv->pmic_gpio6)) {
		gpio_free(swh_priv->pmic_gpio6);
	}

	if (swh_priv->adev) {
		pr_info("%s: test 3\n", __func__);
		cdev_del(&swh_priv->adev->cdev);
		device_destroy(swh_priv->adev->cls, swh_priv->adev->dev_num);
		class_destroy(swh_priv->adev->cls);
		unregister_chrdev_region(0, MINOR_NUMBER_COUNT);
		devm_kfree(&pdev->dev, swh_priv->adev);
	}
	devm_kfree(&pdev->dev, swh_priv);
	g_swh_priv = NULL;
	pr_info("%s: exit\n", __func__);

	return 0;
}


static const struct of_device_id usbc_switch_of_match[] = {
	{.compatible = "oplus,usbc_switch"},
	{ }
};
MODULE_DEVICE_TABLE(of, usbc_switch_of_match);

static struct platform_driver usbc_switch_driver = {
	.probe          = usbc_switch_probe,
	.remove         = usbc_switch_remove,
	.driver         = {
		.name   = USBC_SWITCH_DRIVER_NAME,
		.owner  = THIS_MODULE,
		.of_match_table = usbc_switch_of_match,
	},
};

static int __init usbc_switch_init(void)
{
	pr_info("%s: enter\n", __func__);
	return platform_driver_register(&usbc_switch_driver);
}
module_init(usbc_switch_init);

static void __exit usbc_switch_exit(void)
{
	pr_info("%s: enter\n", __func__);
	platform_driver_unregister(&usbc_switch_driver);
}
module_exit(usbc_switch_exit);

MODULE_DESCRIPTION("audio switch driver");
MODULE_LICENSE("GPL v2");
