/* SPDX-License-Identifier: GPL-2.0-only  */
/*
 * Copyright (C) 2018-2020 Oplus. All rights reserved.
 */
#include "ambiente_proxhub.h"
#include "sensor_cmd.h"
#include "virtual_sensor.h"
#include <linux/notifier.h>
#include <linux/pm_wakeup.h>
#include <linux/version.h>

#define AMBIENTE_PROX_TAG					"[ambiente_proxhub] "
#define AMBIENTE_PROX_FUN(f)				pr_err(AMBIENTE_PROX_TAG"%s\n", __func__)
#define AMBIENTE_PROX_PR_ERR(fmt, args...)	pr_err(AMBIENTE_PROX_TAG"%s %d : "fmt, __func__, __LINE__, ##args)
#define AMBIENTE_PROX_LOG(fmt, args...)		pr_err(AMBIENTE_PROX_TAG fmt, ##args)

static struct virtual_sensor_init_info ambiente_proxhub_init_info;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0))
static struct wakeup_source ambiente_wake_lock;
#else
static struct wakeup_source *ambiente_wake_lock = NULL;
#endif
static int ambiente_prox_open_report_data(int open)
{
	return 0;
}

static int ambiente_prox_enable_nodata(int en)
{
	AMBIENTE_PROX_LOG("ambiente_prox enable nodata, en = %d\n", en);

	return oplus_enable_to_hub(ID_AMBIENTE_PROX, en);
}

static int ambiente_prox_set_delay(u64 delay)
{
#if defined CONFIG_MTK_SCP_SENSORHUB_V1
	unsigned int delayms = 0;

	delayms = delay / 1000 / 1000;
	return oplus_set_delay_to_hub(ID_AMBIENTE_PROX, delayms);
#elif defined CONFIG_NANOHUB
	return 0;
#else
	return 0;
#endif
}

static int ambiente_prox_batch(int flag, int64_t samplingPeriodNs, int64_t maxBatchReportLatencyNs)
{
#if defined CONFIG_MTK_SCP_SENSORHUB_V1
	ambiente_prox_set_delay(samplingPeriodNs);
#endif

	AMBIENTE_PROX_LOG("ambiente_prox: samplingPeriodNs:%lld, maxBatchReportLatencyNs: %lld\n",
		samplingPeriodNs, maxBatchReportLatencyNs);

	return oplus_batch_to_hub(ID_AMBIENTE_PROX, flag, samplingPeriodNs, maxBatchReportLatencyNs);
}

static int ambiente_prox_flush(void)
{
	return oplus_flush_to_hub(ID_AMBIENTE_PROX);
}

static int ambiente_prox_data_report(struct data_unit_t *input_event)
{
	struct oplus_sensor_event event;

	memset(&event, 0, sizeof(struct oplus_sensor_event));

	event.handle = ID_AMBIENTE_PROX;
	event.flush_action = DATA_ACTION;
	event.time_stamp = (int64_t)input_event->time_stamp;
	event.word[0] = input_event->oplus_data_t.ambiente_prox_event.value;
	event.word[1] = input_event->oplus_data_t.ambiente_prox_event.report_count;
	return virtual_sensor_data_report(&event);
}

static int	ambiente_prox_flush_report()
{
	return virtual_sensor_flush_report(ID_AMBIENTE_PROX);
}

static int ambiente_prox_recv_data(struct data_unit_t *event, void *reserved)
{
	int err = 0;

	AMBIENTE_PROX_LOG("ambiente_prox recv data, flush_action = %d, value = %d, report_count = %d, timestamp = %lld\n",
		event->flush_action,
		event->oplus_data_t.ambiente_prox_event.value,
		event->oplus_data_t.ambiente_prox_event.report_count,
		(int64_t)event->time_stamp);

	if (event->flush_action == DATA_ACTION) {
		/*hold 100 ms timeout wakelock*/
		#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0))
		__pm_wakeup_event(&ambiente_wake_lock, msecs_to_jiffies(100));
		#else
		__pm_wakeup_event(ambiente_wake_lock, msecs_to_jiffies(100));
	   #endif
		ambiente_prox_data_report(event);
	} else if (event->flush_action == FLUSH_ACTION) {
		err = ambiente_prox_flush_report();
	}

	return err;
}

static int ambiente_proxhub_local_init(void)
{
	struct virtual_sensor_control_path ctl = {0};
	int err = 0;

	ctl.open_report_data = ambiente_prox_open_report_data;
	ctl.enable_nodata = ambiente_prox_enable_nodata;
	ctl.set_delay = ambiente_prox_set_delay;
	ctl.batch = ambiente_prox_batch;
	ctl.flush = ambiente_prox_flush;
	ctl.report_data = ambiente_prox_recv_data;

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

	err = virtual_sensor_register_control_path(&ctl, ID_AMBIENTE_PROX);
	if (err) {
		AMBIENTE_PROX_PR_ERR("register ambiente_prox control path err\n");
		goto exit;
	}
#ifdef _OPLUS_SENSOR_HUB_VI

	err = scp_sensorHub_data_registration(ID_AMBIENTE_PROX, ambiente_prox_recv_data);
	if (err < 0) {
		AMBIENTE_PROX_PR_ERR("SCP_sensorHub_data_registration failed\n");
		goto exit;
	}
#endif
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0))
	wakeup_source_init(&ambiente_wake_lock, "ambiente_wake_lock");
	#else
	ambiente_wake_lock = wakeup_source_register(NULL, "ambiente_wake_lock");
	#endif
	return 0;
exit:
	return -1;
}

static int ambiente_proxhub_local_uninit(void)
{
	return 0;
}

static struct virtual_sensor_init_info ambiente_proxhub_init_info = {
	.name = "ambiente_prox_hub",
	.init = ambiente_proxhub_local_init,
	.uninit = ambiente_proxhub_local_uninit,
};

static int __init ambiente_proxhub_init(void)
{
	virtual_sensor_driver_add(&ambiente_proxhub_init_info, ID_AMBIENTE_PROX);
	return 0;
}

static void __exit ambiente_proxhub_exit(void)
{
	AMBIENTE_PROX_FUN();
}

module_init(ambiente_proxhub_init);
module_exit(ambiente_proxhub_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ACTIVITYHUB driver");
