/* SPDX-License-Identifier: GPL-2.0-only  */
/*
 * Copyright (C) 2018-2020 Oplus. All rights reserved.
 */
#include "phone_proxhub.h"
#include "sensor_cmd.h"
#include "virtual_sensor.h"
#include <linux/notifier.h>
#include <linux/pm_wakeup.h>
#include <linux/version.h>

#define PHONE_PROX_TAG					"[phone_proxhub] "
#define PHONE_PROX_FUN(f)				pr_err(PHONE_PROX_TAG"%s\n", __func__)
#define PHONE_PROX_PR_ERR(fmt, args...)	pr_err(PHONE_PROX_TAG"%s %d : "fmt, __func__, __LINE__, ##args)
#define PHONE_PROX_LOG(fmt, args...)		pr_err(PHONE_PROX_TAG fmt, ##args)

static struct virtual_sensor_init_info phone_proxhub_init_info;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0))
static struct wakeup_source phone_wake_lock;
#else
static struct wakeup_source *phone_wake_lock = NULL;
#endif
static int phone_prox_open_report_data(int open)
{
	return 0;
}

static int phone_prox_enable_nodata(int en)
{
	PHONE_PROX_LOG("phone_prox enable nodata, en = %d\n", en);

	return oplus_enable_to_hub(ID_PHONE_PROX, en);
}

static int phone_prox_set_delay(u64 delay)
{
#if defined CONFIG_MTK_SCP_SENSORHUB_V1
	unsigned int delayms = 0;

	delayms = delay / 1000 / 1000;
	return oplus_set_delay_to_hub(ID_PHONE_PROX, delayms);
#elif defined CONFIG_NANOHUB
	return 0;
#else
	return 0;
#endif
}

static int phone_prox_batch(int flag, int64_t samplingPeriodNs, int64_t maxBatchReportLatencyNs)
{
#if defined CONFIG_MTK_SCP_SENSORHUB_V1
	phone_prox_set_delay(samplingPeriodNs);
#endif

	PHONE_PROX_LOG("phone_prox: samplingPeriodNs:%lld, maxBatchReportLatencyNs: %lld\n",
		samplingPeriodNs, maxBatchReportLatencyNs);

	return oplus_batch_to_hub(ID_PHONE_PROX, flag, samplingPeriodNs, maxBatchReportLatencyNs);
}

static int phone_prox_flush(void)
{
	return oplus_flush_to_hub(ID_PHONE_PROX);
}

static int phone_prox_data_report(struct data_unit_t *input_event)
{
	struct oplus_sensor_event event;

	memset(&event, 0, sizeof(struct oplus_sensor_event));

	event.handle = ID_PHONE_PROX;
	event.flush_action = DATA_ACTION;
	event.time_stamp = (int64_t)input_event->time_stamp;
	event.word[0] = input_event->oplus_data_t.phone_prox_event.value;
	event.word[1] = input_event->oplus_data_t.phone_prox_event.report_count;
	return virtual_sensor_data_report(&event);
}

static int	phone_prox_flush_report()
{
	return virtual_sensor_flush_report(ID_PHONE_PROX);
}

static int phone_prox_recv_data(struct data_unit_t *event, void *reserved)
{
	int err = 0;

	PHONE_PROX_LOG("phone_prox recv data, flush_action = %d, value = %d, report_count = %d, timestamp = %lld\n",
		event->flush_action,
		event->oplus_data_t.phone_prox_event.value,
		event->oplus_data_t.phone_prox_event.report_count,
		(int64_t)event->time_stamp);

	if (event->flush_action == DATA_ACTION) {
		/*hold 100 ms timeout wakelock*/
		#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0))
		__pm_wakeup_event(&phone_wake_lock, msecs_to_jiffies(100));
		#else
		__pm_wakeup_event(phone_wake_lock, msecs_to_jiffies(100));
	   #endif
		phone_prox_data_report(event);
	} else if (event->flush_action == FLUSH_ACTION) {
		err = phone_prox_flush_report();
	}

	return err;
}

static int phone_proxhub_local_init(void)
{
	struct virtual_sensor_control_path ctl = {0};
	int err = 0;

	ctl.open_report_data = phone_prox_open_report_data;
	ctl.enable_nodata = phone_prox_enable_nodata;
	ctl.set_delay = phone_prox_set_delay;
	ctl.batch = phone_prox_batch;
	ctl.flush = phone_prox_flush;
	ctl.report_data = phone_prox_recv_data;

#if defined CONFIG_MTK_SCP_SENSORHUB_V1
	ctl.is_report_input_direct = true;
	ctl.is_support_batch = false;
#ifdef OPLUS_FEATURE_SENSOR_ALGORITHM
	ctl.is_support_wake_lock = true;
#endif
#elif defined CONFIG_NANOHUB
	ctl.is_report_input_direct = true;
	ctl.is_support_batch = false;
#ifdef OPLUS_FEATURE_SENSOR_ALGORITHM
	ctl.is_support_wake_lock = true;
#endif
#else
#endif

	err = virtual_sensor_register_control_path(&ctl, ID_PHONE_PROX);
	if (err) {
		PHONE_PROX_PR_ERR("register phone_prox control path err\n");
		goto exit;
	}
#ifdef _OPLUS_SENSOR_HUB_VI

	err = scp_sensorHub_data_registration(ID_PHONE_PROX, phone_prox_recv_data);
	if (err < 0) {
		PHONE_PROX_PR_ERR("SCP_sensorHub_data_registration failed\n");
		goto exit;
	}
#endif
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0))
	wakeup_source_init(&phone_wake_lock, "phone_wake_lock");
	#else
	phone_wake_lock = wakeup_source_register(NULL, "phone_wake_lock");
	#endif
	return 0;
exit:
	return -1;
}

static int phone_proxhub_local_uninit(void)
{
	return 0;
}

static struct virtual_sensor_init_info phone_proxhub_init_info = {
	.name = "phone_prox_hub",
	.init = phone_proxhub_local_init,
	.uninit = phone_proxhub_local_uninit,
};

static int __init phone_proxhub_init(void)
{
	virtual_sensor_driver_add(&phone_proxhub_init_info, ID_PHONE_PROX);
	return 0;
}

static void __exit phone_proxhub_exit(void)
{
	PHONE_PROX_FUN();
}

module_init(phone_proxhub_init);
module_exit(phone_proxhub_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ACTIVITYHUB driver");
