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

struct ocp_device_dev *oplus_ocp_dev;
EXPORT_SYMBOL(oplus_ocp_dev);

bool oplus_ocp_device = false;
EXPORT_SYMBOL(oplus_ocp_device);

static int pmic_ocp_dev_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ocp_device_dev *ocp_dev;
	const __be32 *addr;

	pr_err("it only debug,mean the ocp device driver have probe\n");
	ocp_dev = devm_kzalloc(dev, sizeof(*ocp_dev), GFP_KERNEL);
	if (!ocp_dev)
		return -ENOMEM;

	ocp_dev->dev = &pdev->dev;

	ocp_dev->regmap = dev_get_regmap(dev->parent, NULL);
	if (!ocp_dev->regmap) {
		dev_err(dev, "ocp device Parent regmap missing\n");
		return -ENODEV;
	}

	addr = of_get_address(dev->of_node, 0, NULL, NULL);
	if (!addr) {
		dev_err(dev, "ocp device reg property missing\n");
		return -EINVAL;
	}
	ocp_dev->base = be32_to_cpu(*addr);

	addr = of_get_address(dev->of_node, 1, NULL, NULL);
	if (addr)
		ocp_dev->dev_base = be32_to_cpu(*addr);

	platform_set_drvdata(pdev, ocp_dev);

	if (of_property_read_bool(dev->of_node, "oplus-ocp-device")) {
		oplus_ocp_device = true;
	}

	oplus_ocp_dev = ocp_dev;

	return 0;
}

static int pmic_ocp_dev_remove(struct platform_device *pdev)
{
	dev_err(&pdev->dev, "PMIC ocp dev remove\n");
	return 0;
}

static const struct of_device_id pmic_ocp_dev_of_match[] = {
	{ .compatible = "oplus,pmic-ocp-device" },
	{}
};
MODULE_DEVICE_TABLE(of, pmic_ocp_dev_of_match);

static struct platform_driver pmic_ocp_dev_driver = {
	.driver = {
		.name = "oplusi-ocp-dev",
		.of_match_table = of_match_ptr(pmic_ocp_dev_of_match),
	},
	.probe = pmic_ocp_dev_probe,
	.remove = pmic_ocp_dev_remove,
};

int __init pmic_ocp_dev_driver_init(void)
{
	pr_err("debug init pmic ocp dev driver\n");
        return platform_driver_register(&pmic_ocp_dev_driver);
}

void __exit pmic_ocp_dev_driver_exit(void)
{
        platform_driver_unregister(&pmic_ocp_dev_driver);
}


MODULE_DESCRIPTION("OPLUS  OCP dev Driver");
MODULE_LICENSE("GPL v2");
