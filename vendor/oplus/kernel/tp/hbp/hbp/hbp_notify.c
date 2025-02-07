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
#include <drm/drm_panel.h>
#include "hbp_notify.h"
#include "hbp_core.h"
#include "utils/debug.h"

#ifdef QCOM_PLATFORM
#if IS_ENABLED(CONFIG_DRM_PANEL_NOTIFY)
#include <linux/soc/qcom/panel_event_notifier.h>
static void qcom_panel_event_callback(enum panel_event_notifier_tag tag,
				      struct panel_event_notification *notify,
				      void *pvt_data)
{
	struct hbp_device *hbp_dev = (struct hbp_device *)pvt_data;
	hbp_panel_event event = HBP_PANEL_EVENT_UNKNOWN;

	hbp_debug("tag %d, notify type %d, early_trigger %d\n", tag,
		  notify->notif_type,
		  notify->notif_data.early_trigger);

	if (notify->notif_type == DRM_PANEL_EVENT_UNBLANK) {
		if (notify->notif_data.early_trigger) {
			event = HBP_PANEL_EVENT_EARLY_RESUME;
		} else {
			event = HBP_PANEL_EVENT_RESUME;
		}
	}

	if (notify->notif_type == DRM_PANEL_EVENT_BLANK) {
		if (notify->notif_data.early_trigger) {
			event = HBP_PANEL_EVENT_EARLY_SUSPEND;
		} else {
			event = HBP_PANEL_EVENT_SUSPEND;
		}
	}

	hbp_dev->panel_cb(event, hbp_dev);
}
#else
#include <linux/msm_drm_notify.h>
#include <drm/drm_panel.h>
static int qcom_drm_panel_notifier_callback(struct notifier_block *nb, unsigned long event, void *data)
{
	struct hbp_device *hbp_dev = container_of(nb, struct hbp_device, fb_notif);
	hbp_panel_event touch_event = HBP_PANEL_EVENT_UNKNOWN;
	struct msm_drm_notifier *evdata = data;
	int blank =  *(int *)(evdata->data);

	hbp_debug("blank %d, event %d\n", blank, event);

	/*power on*/
	if (blank == MSM_DRM_BLANK_UNBLANK) {
		if (event == MSM_DRM_EARLY_EVENT_BLANK) {
			touch_event = HBP_PANEL_EVENT_EARLY_RESUME;
		} else {
			touch_event = HBP_PANEL_EVENT_RESUME;
		}
	} else if (blank == MSM_DRM_BLANK_POWERDOWN) { /*poweroff*/
		if (event ==  MSM_DRM_EARLY_EVENT_BLANK) {
			touch_event = HBP_PANEL_EVENT_EARLY_SUSPEND;
		} else {
			touch_event = HBP_PANEL_EVENT_SUSPEND;
		}
	} else {
		hbp_err("unknow blank %d event %d\n", blank, event);
	}

	hbp_dev->panel_cb(touch_event, hbp_dev);

	return 0;
}
#endif /*end of CONFIG_DRM_PANEL_NOTIFY*/

static void hbp_register_notify_cb_qcom(struct hbp_device *hbp_dev, struct drm_panel *drm)
{
	int ret = 0;

#if IS_ENABLED(CONFIG_DRM_PANEL_NOTIFY)
	void *entry;
	enum panel_event_notifier_tag tag = (hbp_dev->id == 0)?
					    PANEL_EVENT_NOTIFICATION_PRIMARY:
					    PANEL_EVENT_NOTIFICATION_SECONDARY;
	enum panel_event_notifier_client client = (hbp_dev->id == 0)?
			PANEL_EVENT_NOTIFIER_CLIENT_PRIMARY_TOUCH:
			PANEL_EVENT_NOTIFIER_CLIENT_SECONDARY_TOUCH;


	entry = panel_event_notifier_register(tag, client, drm, qcom_panel_event_callback, hbp_dev);
	if (!entry) {
		hbp_err("Failed to register panel notify\n");
		return;
	}
#else
	hbp_dev->fb_notif.notifier_call = qcom_drm_panel_notifier_callback;
	if (drm) {
		ret = drm_panel_notifier_register(drm, &hbp_dev->fb_notif);
		if (ret < 0) {
			hbp_err("failed to register notifier\n");
		}
	}
#endif
}

#else /*QCOM_PLATFORM*/

#if 1//IS_ENABLED(CONFIG_DRM_MEDIATEK)
#include <linux/mtk_disp_notify.h>

static int mtk_drm_panel_notifier_callback(struct notifier_block *nb,
		unsigned long event, void *data)
{
	struct hbp_device *hbp_dev = container_of(nb, struct hbp_device, fb_notif);
	hbp_panel_event touch_event = HBP_PANEL_EVENT_UNKNOWN;
	int blank =  *(int *)(data);

	hbp_debug("blank %d, event %ld\n", blank, event);

	if (blank == MTK_DISP_BLANK_UNBLANK) {
		if (event == MTK_DISP_EARLY_EVENT_BLANK) {
			touch_event = HBP_PANEL_EVENT_EARLY_RESUME;
		} else if (event == MTK_DISP_EVENT_BLANK) {
			touch_event = HBP_PANEL_EVENT_RESUME;
		} else {
			hbp_warn("unkown blank %d, event %ld\n", blank, event);
		}
	} else if (blank == MTK_DISP_BLANK_POWERDOWN) {
		if (event == MTK_DISP_EARLY_EVENT_BLANK) {
			touch_event = HBP_PANEL_EVENT_EARLY_SUSPEND;
		} else if (event == MTK_DISP_EVENT_BLANK) {
			touch_event = HBP_PANEL_EVENT_SUSPEND;
		} else {
			hbp_warn("unkown blank %d, event %ld\n", blank, event);
			return 0;
		}
	} else {
		hbp_warn("unkown blank %d\n", blank);
		return 0;
	}

	hbp_dev->panel_cb(touch_event, hbp_dev);
	return 0;
}

static void hbp_register_notify_cb_mtk(struct hbp_device *hbp_dev, struct drm_panel *drm)
{
	hbp_dev->fb_notif.notifier_call = mtk_drm_panel_notifier_callback;
	if (hbp_dev->id == 0) {
		if (mtk_disp_notifier_register("oplus_touch", &hbp_dev->fb_notif)) {
			hbp_err("failed to register main display notifer\n");
		}
	} else {
		if (mtk_disp_sub_notifier_register("oplus_touch_sub", &hbp_dev->fb_notif)) {
			hbp_err("failed to register main display notifer\n");
		}
	}
}
#endif

#endif /*end of QCOM_PLATFORM*/

void hbp_register_notify_cb(struct hbp_device *hbp_dev, struct device *dev)
{
#ifdef QCOM_PLATFORM
	struct drm_panel *drm;
	struct device_node *dsi_np, *panel_np;
	const char *dsi_name;
	int i = 0, count = 0;

	dsi_np = of_find_node_by_name(NULL, "oplus,dsi-display-dev");
	if (!dsi_np) {
		dsi_name = "dsi_panel";
		dsi_np = dev->of_node;
	} else {
		if (hbp_dev->id == 0) {
			dsi_name = "oplus,dsi-panel-primary";
		} else {
			dsi_name = "oplus,dsi-panel-secondary";
		}
	}

	count = of_count_phandle_with_args(dsi_np, dsi_name, NULL);
	for (i = 0; i < count; i++) {
		panel_np = of_parse_phandle(dsi_np, dsi_name, i);
		if (panel_np) {
			hbp_info("panel list name:%s\n", panel_np->name);
			drm = of_drm_find_panel(panel_np);
			of_node_put(panel_np);
			if (!IS_ERR_OR_NULL(drm)) {
				break;
			} else {
				hbp_err("drm err %ld",  PTR_ERR(drm));
			}
		} else {
			hbp_err("failed to find dsi name %s\n", dsi_name);
		}
	}

	if (count < 0 || i == count) {
		hbp_err("Failed to find attached panel, count = %d\n", count);
		return;
	}

	hbp_register_notify_cb_qcom(hbp_dev, drm);
#else
	hbp_register_notify_cb_mtk(hbp_dev, NULL);
#endif
}

void hbp_event_call_notifier(unsigned long action, void *data)
{
	touchpanel_event_call_notifier(action, data);
}

/*
int (*tp_gesture_enable_notifier)(unsigned int tp_index) = NULL;
EXPORT_SYMBOL(tp_gesture_enable_notifier);
*/

