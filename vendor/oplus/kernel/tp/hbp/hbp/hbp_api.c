#include <linux/delay.h>
#include <linux/power_supply.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/of_gpio.h>
#include <linux/kthread.h>
#include <linux/version.h>
#include <linux/reboot.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/syscalls.h>
#include <linux/vmalloc.h>
#include <uapi/linux/sched/types.h>
#include <linux/regulator/consumer.h>
#include "hbp_api.h"
#include "hbp_core.h"
#include "utils/debug.h"

static bool hbp_feature_supported(struct hbp_feature *features, touch_feature mode)
{
	if (features->supports & (1 << mode)) {
		return true;
	}

	hbp_info("feature:%d not support\n", mode);
	return false;
}

bool hbp_update_feature_param(struct hbp_feature *features,
			      touch_feature mode,
			      struct param_value *param)
{
	mutex_lock(&features->mfeature);

	if (hbp_feature_supported(features, mode) && param) {
		memcpy(&features->params[mode], param, sizeof(struct param_value));
		mutex_unlock(&features->mfeature);
		return true;
	}

	mutex_unlock(&features->mfeature);
	return false;
}

bool hbp_get_feature_param(struct hbp_feature *features,
			   touch_feature mode,
			   struct param_value *param)
{
	mutex_lock(&features->mfeature);

	if (features->supports & (1 << mode)) {
		memcpy(param, &features->params[mode], sizeof(struct param_value));
		mutex_unlock(&features->mfeature);
		return true;
	}

	mutex_unlock(&features->mfeature);
	return false;
}
EXPORT_SYMBOL(hbp_get_feature_param);

