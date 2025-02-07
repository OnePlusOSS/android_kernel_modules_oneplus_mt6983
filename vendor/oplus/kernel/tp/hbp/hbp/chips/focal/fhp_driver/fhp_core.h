/*
 *
 * FocalTech TouchScreen driver.
 *
 * Copyright (c) 2020-2022, FocalTech Systems, Ltd., all rigfhps reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/*****************************************************************************
* File Name: fhp_core.h
* Author: Focaltech Driver Team
* Created: 2020-06-17
*
* Abstract: include file of fhp driver for FocalTech
*
*****************************************************************************/

#ifndef __LINUX_FHP_CORE_H__
#define __LINUX_FHP_CORE_H__
/*****************************************************************************
* Included header files
*****************************************************************************/
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/spi/spi.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/uaccess.h>
#include <linux/firmware.h>
#include <linux/debugfs.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>

/*****************************************************************************
* Private constant and macro definitions using #define
*****************************************************************************/
/*ID*/
#define FTS_CHIP_ID                         0x56

/*Please don't modify the value of following chip related macroes*/
#define FTS_REG_CHIP_ID                     0xA3
#define FTS_REG_FW_VER                      0xA6

#define FTS_CMD_START                       0x55
#define FTS_CMD_START_DELAY                 12
#define FTS_CMD_READ_ID                     0x90



#define MAX_SPI_BUF_LENGTH                  4096
#define FTS_MAX_POINTS_SUPPORT              10

#define FTS_GESTURE_POINTS_MAX              6
#define FTS_GESTURE_DATA_LEN               (FTS_GESTURE_POINTS_MAX * 4 + 4)

#define FTS_MAX_COMMMAND_LENGTH             16


/*****************************************************************************
*  Alternative mode (When something goes wrong, the modules may be able to solve the problem.)
*****************************************************************************/
/*
 * For commnication error in PM(deep sleep) state
 */
#define FTS_PATCH_COMERR_PM                     1
#define FTS_TIMEOUT_COMERR_PM                   700


/*****************************************************************************
* Private enumerations, structures and unions using typedef
*****************************************************************************/


struct fts_core {
	struct mutex bus_mutex;
	u8 *bus_tx_buf;
	u8 *bus_rx_buf;
	struct bus_operations *bus_ops;
};


/*****************************************************************************
* Global variable or extern global variabls/functions
*****************************************************************************/
enum gesture_id {
	GESTURE_RIGHT2LEFT_SWIP = 0x20,
	GESTURE_LEFT2RIGHT_SWIP = 0x21,
	GESTURE_DOWN2UP_SWIP = 0x22,
	GESTURE_UP2DOWN_SWIP = 0x23,
	GESTURE_DOUBLE_TAP = 0x24,
	GESTURE_DOUBLE_SWIP = 0x25,
	GESTURE_RIGHT_VEE = 0x51,
	GESTURE_LEFT_VEE = 0x52,
	GESTURE_DOWN_VEE = 0x53,
	GESTURE_UP_VEE = 0x54,
	GESTURE_O_CLOCKWISE = 0x57,
	GESTURE_O_ANTICLOCK = 0x30,
	GESTURE_W = 0x31,
	GESTURE_M = 0x32,
	GESTURE_FINGER_PRINT = 0x26,
	GESTURE_SINGLE_TAP = 0x27,
	GESTURE_HEART_ANTICLOCK = 0x55,
	GESTURE_HEART_CLOCKWISE = 0x59,
};


/* communication interface */
int fhp_read(u8 *cmd, u32 cmdlen, u8 *data, u32 datalen);
int fhp_read_reg(u8 addr, u8 *value);
int fhp_write(u8 *writebuf, u32 writelen);
int fhp_write_reg(u8 addr, u8 value);


#endif /* __LINUX_FHP_CORE_H__ */
