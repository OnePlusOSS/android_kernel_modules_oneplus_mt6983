#ifndef __HBP_BUS_H__
#define __HBP_BUS_H__

#include <linux/delay.h>
#include <linux/power_supply.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/of_gpio.h>
#include <linux/kthread.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/syscalls.h>
#include <linux/vmalloc.h>
#include <uapi/linux/sched/types.h>
#include <linux/device.h>
#include <linux/spi/spi.h>

#define HBP_SPI_MODULE_NAME  "hbp_spi_bus"

struct bus_operations {
	int (*read_block)(void *ops, uint8_t *data, size_t len);
	int (*write_block)(void *ops, uint8_t *data, size_t len);
	int(*spi_sync)(void *ops, uint8_t *tx, uint8_t *rx, size_t len);
	void (*shutdown)(void *ops);
	int (*spi_setup)(void *ops, uint8_t mode, uint8_t bits_per_word, int speed);
};

struct spi_param {
	uint16_t byte_delay_us;
	uint16_t block_delay_us;
	int mode;
};

struct spi_bus {
	struct bus_operations spi_ops;
	struct mutex mtx;
	const char *name;
	struct spi_device *spi_dev;
	struct spi_param param;
	bool bus_ready; /*spi or i2c resume status*/
};

extern int hw_interface_init(void);

#endif
