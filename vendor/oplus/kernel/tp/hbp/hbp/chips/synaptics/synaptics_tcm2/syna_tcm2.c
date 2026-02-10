#include <linux/irqreturn.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/sched.h>
#include "../../../hbp_core.h"
#include "../../../hbp_bus.h"
#include "../../../utils/debug.h"

#define PLATFORM_DRIVER_NAME "syna_tcm2"

struct syna_tcm_hcd {
	struct platform_device *pdev;
	struct hbp_bus_io *bus_io;
};

static int syna_dev_write(void *priv, void *data, int32_t len)
{
	struct syna_tcm_hcd *tcm_hcd = (struct syna_tcm_hcd *)priv;

	return tcm_hcd->bus_io->write_block(tcm_hcd->bus_io, data, len);

}

static int syna_dev_read(void *priv, char *data, int32_t len)
{
	struct syna_tcm_hcd *tcm_hcd = (struct syna_tcm_hcd *)priv;
	return tcm_hcd->bus_io->read_block(tcm_hcd->bus_io, data, len);
}

struct dev_operations syna_ops = {
	.spi_sync = 
};

static int syna_dev_probe(struct platform_device *pdev)
{
	struct syna_tcm_hcd *tcm_hcd;
	struct chip_info info;
	int ret = 0;

	//TODO:need get from dts
	if (!match_from_cmdline(&pdev->dev, &info)) {
		return 0;
	}

	tcm_hcd = kzalloc(sizeof(*tcm_hcd), GFP_KERNEL);
	if (!tcm_hcd) {
		hbp_err("Failed to allocate memory for tcm_hcd\n");
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, tcm_hcd);
	tcm_hcd->pdev = pdev;

	ret = hbp_register_devices(tcm_hcd,
								&pdev->dev,
								&syna_ops,
								&info,
								&tcm_hcd->bus_io);
	if (ret < 0) {
		hbp_info("failed to register device\n");
		goto err_exit;
	}

	hbp_info("probe end\n");
	return 0;

err_exit:
	return ret;
}

static int syna_dev_remove(struct platform_device *pdev)
{
	return 0;
}

static struct of_device_id syna_tcm2_of_match[] = {
	{ .compatible = "synaptics-tcm" },
	{},
};

static struct platform_driver syna_dev_driver = {
	.driver = {
		.name = PLATFORM_DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = syna_tcm2_of_match,
	},
	.probe = syna_dev_probe,
	.remove = syna_dev_remove,
};

module_platform_driver(syna_dev_driver);

MODULE_AUTHOR("Synaptics, Inc.");
MODULE_DESCRIPTION("Synaptics TCM Touch Driver");
MODULE_LICENSE("GPL v2");

