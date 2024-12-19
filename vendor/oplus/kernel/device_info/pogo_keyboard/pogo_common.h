#ifndef __POGO_COMMON_H__
#define __POGO_COMMON_H__
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/serial_core.h>

struct pogo_keyboard_operations {
    char name[32];
    int (*init)(void *port, int value);
    int (*write)(void *param, int value);
    int (*recv)(char *buf, int len);
    int (*resume)(struct platform_device *device);
    int (*suspend)(struct platform_device *device);
    int (*remove)(struct platform_device *device);
    bool (*check)(struct uart_port *port);
};

#endif

