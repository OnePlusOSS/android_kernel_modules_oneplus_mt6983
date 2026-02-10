#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/kernel.h>

#include "utils/debug.h"
#include "hbp_core.h"
#include "hbp_notify.h"

extern void hbp_state_notify(struct hbp_core *hbp, int id, hbp_panel_event event);

static int select_int_para(const char *input, int idx, int *val)
{
#define SEP ","
	int i = 0;
	const char *pos = input;
	const char *next = pos;
	char result[32] = {0};
	int int_val;

	for (i = 0; i < idx; i++) {
		pos = strstr(pos, SEP);
		if (!pos) {
			break;
		}
		pos += strlen(SEP);
	}

	if (i == idx) {
		next = strstr(pos, SEP);
		if (!next) {
			next = pos + strlen(pos);
		}
		if (next - pos > 31) {
			return -1;
		} else {
			memcpy(result, pos, next-pos);
			result[next-pos] = '\0';
		}

		if (kstrtos32(result, 0, &int_val) < 0 || int_val < 0) {
			return -1;
		}

		*val = (int)int_val;
		return 0;
	}

	return -1;
}

static ssize_t trusted_enable_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	return count;
}

/*show all curent settings*/
static ssize_t touch_settings_show(struct device *dev,
				   struct device_attribute *attr,
				   char *buf)
{
	return 0;
}

static ssize_t trusted_type_show(struct device *dev,
				 struct device_attribute *attr,
				 char *buf)
{
	return 0;
}

static ssize_t trusted_event_show(struct device *dev,
				  struct device_attribute *attr,
				  char *buf)
{
	return 0;
}

static ssize_t state_store(struct device *dev,
			   struct device_attribute *attr,
			   const char *buf, size_t count)
{
	int id = -1;
	int event = HBP_PANEL_EVENT_UNKNOWN;
	struct hbp_core *hbp = dev_get_drvdata(dev);

	/*data style: id,state*/
	if (select_int_para(buf, 0, &id) != 0 || id < 0 || id >= MAX_DEVICES) {
		hbp_err("Invalid id value\n");
		return -EINVAL;
	}

	if (select_int_para(buf, 1, &event) != 0) {
		hbp_err("Invalid event value\n");
		return -EINVAL;
	}

	hbp_info("user: id %d, state %d\n", id, event);
	hbp_state_notify(hbp, id, event);
	return count;
}

static ssize_t state_show(struct device *dev,
			  struct device_attribute *attr,
			  char *buf)
{
	int cnt = 0;
	int i = 0;
	struct hbp_core *hbp = dev_get_drvdata(dev);

	cnt += scnprintf(buf + cnt, PAGE_SIZE, "early suspend:1, suspend:2, early-resume:3, resume:4\n");
	for (i = 0; i < MAX_DEVICES; i++) {
		if (!hbp->devices[i]) {
			continue;
		}
		cnt += scnprintf(buf + cnt, PAGE_SIZE,
				 "id %d, state %d, sync %d\n",
				 hbp->states[i].id,
				 hbp->states[i].state,
				 hbp->states[i].sync);
	}
	return cnt;
}

static ssize_t active_id_store(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct hbp_core *hbp = dev_get_drvdata(dev);
	int id = 0;

	if (kstrtos32(buf, 0, &id) < 0 ||
	    id < 0 ||
	    id >= MAX_DEVICES) {
		return count;
	}

	hbp->active_id = id;
	return count;
}

static ssize_t active_id_show(struct device *dev,
			      struct device_attribute *attr,
			      char *buf)
{
	struct hbp_core *hbp = dev_get_drvdata(dev);

	return scnprintf(buf, PAGE_SIZE, "active id: %d\n", hbp->active_id);
}

static ssize_t debug_level_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	int level = 0;

	if (kstrtos32(buf, 0, &level) < 0) {
		return count;
	}

	set_debug_level(level);
	return count;
}

static ssize_t debug_level_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "Debug Level: %d (6:debug 5:cache 4:info 3:warn 2:error 1:fatal)\n",
			 get_debug_level());
}

static DEVICE_ATTR(settings, 0660, touch_settings_show, NULL);
static DEVICE_ATTR(trusted_enable, 0660, NULL, trusted_enable_store);
static DEVICE_ATTR(trusted_type, 0660, trusted_type_show, NULL);
static DEVICE_ATTR(trusted_event, 0660, trusted_event_show, NULL);
static DEVICE_ATTR(state, 0660, state_show, state_store);
static DEVICE_ATTR(active_id, 0660, active_id_show, active_id_store);
static DEVICE_ATTR(debug_level, 0660, debug_level_show, debug_level_store);

static struct attribute *hbp_attrs[] = {
	&dev_attr_settings.attr,
	&dev_attr_trusted_enable.attr,
	&dev_attr_trusted_type.attr,
	&dev_attr_trusted_event.attr,
	&dev_attr_state.attr,
	&dev_attr_active_id.attr,
	&dev_attr_debug_level.attr,
	NULL,
};

static const struct attribute_group hbp_attr_grp = {
	.attrs = hbp_attrs,
};

int hbp_register_sysfs(struct hbp_core *hbp)
{
	int ret = 0;

	ret = sysfs_create_group(&hbp->dev->kobj, &hbp_attr_grp);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

