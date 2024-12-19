#include <linux/vmalloc.h>
#include <uapi/linux/sched/types.h>
#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>

#include "hbp_core.h"
#include "utils/debug.h"

#define TOUCH_PARAM_LEN (256)
#define VENDOR_LEN (32)

static char touch_primary[TOUCH_PARAM_LEN];
static char touch_secondary[TOUCH_PARAM_LEN];

static char *supported_vendors[] = {
	"BOE", "DSJM", "TIANMA", "TRULY", "DJN", "OLIM", "G2Y", "JDI",
	"TPK", "SAMSUNG", "INNOLUX", "DPT", "AUO", "DEPUTE", "HUAXING",
	"HLT", "VXN",
	NULL
};

static bool check_supported_vendor(const char *vendor)
{
	int i = 0;
	int cmp_len = 0;

	for(i = 0; supported_vendors[i]; i++) {
		cmp_len = strlen(supported_vendors[i]);
		if (strncasecmp(vendor, supported_vendors[i], cmp_len) == 0) {
			return true;
		}
	}

	return false;
}

void split_touch_cmdline(void)
{
	struct device_node * of_chosen = NULL;
	char *bootargs = NULL;
	char *start, *end;

	of_chosen = of_find_node_by_path("/chosen");
	bootargs = (char *)of_get_property(of_chosen, "bootargs", NULL);
	if (!bootargs) {
		hbp_err("failed to get bootargs\n");
		return;
	}

	hbp_info("boot args %s\n", bootargs);
	start = bootargs;

	//for touch panel 0
	start = strstr(bootargs, "touch_panel0=");
	if (start) {
		end = strstr(start, " ");
		if (end - start >= TOUCH_PARAM_LEN) {
			hbp_err("cmdline touch panel 0 out of size\n");
		} else {
			strncpy(touch_primary, start, end - start);
		}
	}

	//for touch panel 1
	start = strstr(bootargs, "touch_panel1=");
	if (start) {
		end = strstr(start, " ");
		if (end - start >= TOUCH_PARAM_LEN) {
			hbp_err("cmdline touch panel 1 out of size\n");
		} else {
			strncpy(touch_secondary, start, end - start);
		}
	}

	hbp_info("parsed touch args primary:%s secondary:%s\n", touch_primary, touch_secondary);
}


/*
** command line: id:chip:panel[-panelid]
**
*/
bool match_from_cmdline(struct device *dev, struct chip_info *info)
{
	char *cmdline, *panel, *offset = NULL;
	int id = -EINVAL;
	int ret = 0;

	hbp_info("%s start.\n", dev->of_node->name);

	ret = of_property_read_u32(dev->of_node, "device,id", &id);
	if (ret < 0) {
		hbp_err("tp device id not found\n");
		return false;
	}

	ret = of_property_read_string(dev->of_node, "device,chip_name", &info->ic_name);
	if (ret < 0) {
		hbp_err("failed to find chip name\n");
		return false;
	}

	panel = kzalloc(VENDOR_LEN, GFP_KERNEL);
	if (!panel) {
		hbp_err("failed to alloc memory\n");
		return false;
	}

	if (id == 0) {
		cmdline = touch_primary;
	} else {
		cmdline = touch_secondary;
	}

	hbp_info("TP cmdline:%s chip name:%s\n", cmdline, info->ic_name);

	offset = strstr(cmdline, info->ic_name);
	/*if not found ic name from cmdline, return false*/
	if (!offset) {
		hbp_err("failed to find ic name %s from %s\n", info->ic_name, cmdline);
		goto end;;
	}

	/*vendor name seperated from ':' */
	offset += strlen(info->ic_name);
	offset = strstr(offset, ":");
	if (!offset) {
		hbp_err("failed to find ic name %s\n", cmdline);
		goto end;
	}

	offset += 1;
	memcpy(panel, offset, strlen(offset) > (VENDOR_LEN - 1)?(VENDOR_LEN - 1):strlen(offset));
	info->vendor = strim(panel);

	if (!check_supported_vendor(info->vendor)) {
		hbp_err("vendor name not in supported vendor list %s\n", info->vendor);
		goto end;
	}

	hbp_info("match TP ic name:%s vendor name:%s\n", info->ic_name, info->vendor);

	return true;

end:
	kfree(panel);
	return false;
}
EXPORT_SYMBOL(match_from_cmdline);


