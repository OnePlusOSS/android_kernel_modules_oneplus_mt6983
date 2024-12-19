// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2022, Qualcomm Innovation Center, Inc. All rights reserved. */

#include <linux/err.h>
#include <linux/idr.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/nvmem-consumer.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/string.h>
#include <linux/io.h>
#include <linux/of_address.h>

#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>

#include "oplus_pmic_info.h"

//#define CURRENT_OCP_SID_MODE(ocp_dev)     ((ocp_dev)->base)
//#define CURRENT_OCP_PID(ocp_dev)          ((ocp_dev)->base + 0x1)
//#define LAST_OCP_SID_MODE(ocp_dev)        ((ocp_dev)->base + 0x2)
//#define LAST_OCP_PID(ocp_dev)             ((ocp_dev)->base + 0x3)
//#define LLAST_OCP_SID_MODE(ocp_dev)       ((ocp_dev)->base + 0x5)
//#define LLAST_OCP_PID(ocp_dev)            ((ocp_dev)->base + 0x6)
#define OCP_CLEAR_TRIGGER_ADDR(ocp_dev)     ((ocp_dev)->base + 0xe5)

#define OCP_CLEAR_MASK                    (0xFF)
#define OCP_CLEAR_VALUE                   (0x00)
#define OCP_DEBUG_VALUE                   (0xFF)
#define OCP_CLEAR_TRIGGER_VALUE           (0x1)

extern struct ocp_device_dev *oplus_ocp_dev;
static void *ocp_info = NULL;
struct PMICOcplogRecoedStruct ocp_state;

#define OCP_LOG_ENTRY_SIZE	2

struct ocp_log_entry {
	u16	ppid;
	u8	mode_at_ocp;
};

struct ocp_log_dev {
	struct device		*dev;
	struct nvmem_cell	*nvmem_cell;
	int			ocp_log_entry_count;
};

void *get_ocp_state_from_nvmem(void) {
	return &ocp_state;
}

void *get_ocp_state(void)
{
	if (ocp_info == NULL) {
		ocp_info = get_ocp_state_from_nvmem();
	}
  	return ocp_info;
}

static int ocp_reg_clear(struct ocp_device_dev *ocp_dev)
{
	int rc = 0;
	//write 1 to 0x82e5 to clear ocp event
	rc = ocp_dev_masked_write(ocp_dev, OCP_CLEAR_TRIGGER_ADDR(ocp_dev), OCP_CLEAR_MASK, OCP_CLEAR_TRIGGER_VALUE);
	if (rc) {
                pr_err("Unable to write to addr=%x, rc(%d)\n", OCP_CLEAR_TRIGGER_ADDR(ocp_dev), rc);
        }
	return rc;
}


static int pmic_ocp_log_event(struct ocp_log_dev *ocp_dev,
				struct ocp_log_entry *entry, const char *label)
{
	pr_err("%s ppid=0x%03X, mode=%u\n",
                        label, entry->ppid, entry->mode_at_ocp);
	return 0;
}

static int pmic_ocp_log_read_entry(struct ocp_log_dev *ocp_dev, u32 index,
				   struct ocp_log_entry *entry)
{
	size_t len = 0;
	u8 *buf;
	int ret, i;

	if (index >= ocp_dev->ocp_log_entry_count)
		return -EINVAL;

	buf = nvmem_cell_read(ocp_dev->nvmem_cell, &len);
	if (IS_ERR(buf)) {
		ret = PTR_ERR(buf);
		dev_err(ocp_dev->dev, "failed to read nvmem cell, ret=%d\n", ret);
		return ret;
	}

	i = index * OCP_LOG_ENTRY_SIZE;
	if (i + 1 >= len) {
		dev_err(ocp_dev->dev, "invalid OCP log index=%i\n", i);
		kfree(buf);
		return -EINVAL;
	}

	/*
	 * OCP log entry layout:
	 * Byte 0:	[7:4] - SID
	 *		[2:0] - mode at OCP
	 * Byte 1:	[7:0] - PID
	 */
	entry->ppid = (((u16)buf[i] << 4) & 0xF00) | buf[i + 1];
	entry->mode_at_ocp = buf[i] & 0x7;

	kfree(buf);

	return 0;
}

static int pmic_ocp_state_probe(struct platform_device *pdev)
{
	struct ocp_log_dev *ocp_dev;
	struct ocp_device_dev *ocp_device;
	struct ocp_log_entry entry = {0};
	size_t len = 0;
	int ret, i;
	u8 *buf;

	ocp_dev = devm_kzalloc(&pdev->dev, sizeof(*ocp_dev), GFP_KERNEL);
	if (!ocp_dev)
		return -ENOMEM;

	ocp_dev->dev = &pdev->dev;
	ocp_dev->nvmem_cell = devm_nvmem_cell_get(&pdev->dev, "ocp_log");
	if (IS_ERR(ocp_dev->nvmem_cell)) {
		ret = PTR_ERR(ocp_dev->nvmem_cell);
		if (ret != -EPROBE_DEFER)
			dev_err(&pdev->dev, "failed to get nvmem cell, ret=%d\n",
				ret);
		return ret;
	}

	platform_set_drvdata(pdev, ocp_dev);
	
	buf = nvmem_cell_read(ocp_dev->nvmem_cell, &len);
	if (IS_ERR(buf)) {
		ret = PTR_ERR(buf);
		dev_err(&pdev->dev, "failed to read nvmem cell, ret=%d\n", ret);
		return ret;
	}

	ocp_dev->ocp_log_entry_count = len / OCP_LOG_ENTRY_SIZE;
	dev_err(&pdev->dev, "ocp_dev get ocp_log_entry_count =%d\n", ocp_dev->ocp_log_entry_count);
	kfree(buf);

	for (i = 0; i < ocp_dev->ocp_log_entry_count; i++) {
		dev_err(&pdev->dev, "pmic_monitor ocp log count=%d\n", i);
		ret = pmic_ocp_log_read_entry(ocp_dev, i, &entry);
		if (ret)
			return ret;
		ocp_state.ocp_record[i].ppid = entry.ppid;
		ocp_state.ocp_record[i].mode_at_ocp = entry.mode_at_ocp;

		ret = pmic_ocp_log_event(ocp_dev, &entry,
				"Regulator OCP event in pmic_monitor:");
		if (ret) {
			if (ret != -EPROBE_DEFER)
				dev_err(&pdev->dev, "failed to log entry %d, ret=%d\n",
					i, ret);
			return ret;
		}
	}
	
	if (!oplus_ocp_dev) {
		dev_err(&pdev->dev, "oplus ocp dev is null");
		return -EPROBE_DEFER;
	}
	//not retrun after read cell info,we should clear reg info
	//if ( ocp_reason ) {
	//	__raw_writel(0xffffffff, ocp_reason);
	//}
	//cell = ocp_dev->nvmem_cell;
	//nvmem = cell->nvmem;
        //u8 w_buf = 0x11;
	/*
	if (nvmem) {
		dev_err(&pdev->dev, "liuzhiwen debug ocp  nvmem read_only=%d,root_only=%d,flags=%d,cell_bytes=%d,\n", nvmem->read_only,
													            nvmem->root_only,
														    nvmem->flags,
														    cell->bytes);
        } else {
	        dev_err(&pdev->dev, "liuzhiwen debug ocp nvmem invalid\n");
        }
	*/
	ocp_device = oplus_ocp_dev;
        ret = ocp_reg_clear(ocp_device);
	if (ret) {
		dev_err(&pdev->dev, "update ocp log bits failed\n");
	}
	return ret;
}

static int pmic_ocp_state_remove(struct platform_device *pdev)
{
	dev_err(&pdev->dev, "PMIC ocp state remove\n");
	return 0;
}

static const struct of_device_id pmic_ocp_state_of_match[] = {
	{ .compatible = "oplus,pmic-ocp-log" },
	{}
};
MODULE_DEVICE_TABLE(of, pmic_ocp_state_of_match);

static struct platform_driver pmic_ocp_state_driver = {
	.driver = {
		.name = "oplusi-ocp-state",
		.of_match_table = of_match_ptr(pmic_ocp_state_of_match),
	},
	.probe = pmic_ocp_state_probe,
	.remove = pmic_ocp_state_remove,
};

int __init pmic_ocp_state_driver_init(void)
{
        return platform_driver_register(&pmic_ocp_state_driver);
}

void __exit pmic_ocp_state_driver_exit(void)
{
        platform_driver_unregister(&pmic_ocp_state_driver);
}


MODULE_DESCRIPTION("OPLUS  OCP State Driver");
MODULE_LICENSE("GPL v2");
