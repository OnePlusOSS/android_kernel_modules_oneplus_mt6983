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
* File Name: fhp_input.c
* Author: Focaltech Driver Team
* Created: 2020-06-30
*
* Abstract: handle input sub-system.
*
*****************************************************************************/
/*****************************************************************************
* Included header files
*****************************************************************************/
#include "fhp_core.h"

/*****************************************************************************
* Private constant and macro definitions using #define
*****************************************************************************/
#define FHP_INPUT_NAME                       "fhp,input"
#define FHP_INPUT_MISCDEV_NAME               "fhp_input"
#define FHP_MAX_PONITS                       10
#define FHP_MAX_X_DEFAULT                    1080
#define FHP_MAX_Y_DEFAULT                    1920


/*****************************************************************************
* Private enumerations, structures and unions using typedef
*****************************************************************************/
struct fhp_input_cfg {
	int x_max;
	int y_max;
};

struct fhp_input_core {
	struct fhp_core_data *fhp_data;
	struct input_dev *input_dev;
	struct miscdevice input_miscdev;
	struct mutex input_mutex;
	struct fhp_input_cfg input_cfg;
};

struct fhp_input_touch_event {
	int valid;
	int down;
	int id;
	int type;
	int x;
	int y;
	int pressure;
	int touch_major;
	int touch_minor;
};

struct fhp_input_ioctl_touch_event {
	struct fhp_input_touch_event events[FHP_MAX_PONITS];
	int touch_num;
	int event_num;
};

#define FHP_INPUT_IOCTL_DEVICE           0xC6
#define FHP_INPUT_IOCTL_REPORT           _IOWR(FHP_INPUT_IOCTL_DEVICE, 0x01, struct fhp_input_ioctl_touch_event)
#define FHP_INPUT_IOCTL_GET_INPUT_CFG    _IOWR(FHP_INPUT_IOCTL_DEVICE, 0x02, struct fhp_input_cfg)

/*****************************************************************************
* Static variabls
*****************************************************************************/
struct fhp_input_core *fhp_input;

/*****************************************************************************
* Global variable or extern global variabls/functions
*****************************************************************************/

/*****************************************************************************
* Static function prototypes
*****************************************************************************/

void fhp_input_release_all_fingers(void)
{
	int i = 0;
	struct fhp_input_core *input = fhp_input;

	FHP_INFO("call in");
	mutex_lock(&input->input_mutex);
	for (i = 0; i < FHP_MAX_PONITS; i++) {
		input_mt_slot(input->input_dev, i);
		input_mt_report_slot_state(input->input_dev, MT_TOOL_FINGER, false);
	}
	input_report_key(input->input_dev, BTN_TOUCH, 0);
	input_sync(input->input_dev);
	mutex_unlock(&input->input_mutex);
	FHP_INFO("call out");
}

void fhp_input_report_key(int key_code)
{
	struct fhp_input_core *input = fhp_input;

	FTS_DEBUG("Gesture Code=0x%02x", key_code);
	input_report_key(input->input_dev, key_code, 1);
	input_sync(input->input_dev);
	input_report_key(input->input_dev, key_code, 0);
	input_sync(input->input_dev);
}

static int input_miscdev_open_cnt = 0;
static int fhp_input_open(struct inode *inode, struct file *file)
{
	if (input_miscdev_open_cnt >= 1) {
		FHP_ERROR("open fail(cnt:%d)", input_miscdev_open_cnt);
		return -EBUSY;
	}
	FHP_INFO("open successfully(cnt:%d)", input_miscdev_open_cnt);
	input_miscdev_open_cnt++;
	return 0;
}

static int fhp_input_close(struct inode *inode, struct file *file)
{
	FHP_INFO("call.");
	input_miscdev_open_cnt--;
	return 0;
}

static long fhp_input_report(struct fhp_input_core *input, unsigned long arg)
{
	int i = 0;
	struct fhp_input_ioctl_touch_event ioctl_touch;
	struct fhp_input_touch_event *events = &ioctl_touch.events[0];

	FHP_DEBUG("in");
	if (copy_from_user(&ioctl_touch, (void *)arg, sizeof(struct fhp_input_ioctl_touch_event))) {
		FHP_ERROR("copy input touch event data from userspace fail");
		return -EFAULT;
	}

	mutex_lock(&input->input_mutex);
	for (i = 0; i < FHP_MAX_PONITS; i++) {
		if (events[i].down) {
			input_mt_slot(input->input_dev, i);
			input_mt_report_slot_state(input->input_dev, MT_TOOL_FINGER, true);
			input_report_abs(input->input_dev, ABS_MT_PRESSURE, events[i].pressure);
			input_report_abs(input->input_dev, ABS_MT_TOUCH_MAJOR, events[i].touch_major);
			input_report_abs(input->input_dev, ABS_MT_TOUCH_MINOR, events[i].touch_minor);
			input_report_abs(input->input_dev, ABS_MT_POSITION_X, events[i].x);
			input_report_abs(input->input_dev, ABS_MT_POSITION_Y, events[i].y);
		} else {
			input_mt_slot(input->input_dev, i);
			input_mt_report_slot_state(input->input_dev, MT_TOOL_FINGER, false);
		}
	}

	if (ioctl_touch.touch_num) {
		input_report_key(input->input_dev, BTN_TOUCH, 1);
	} else {
		input_report_key(input->input_dev, BTN_TOUCH, 0);
	}

	/*sync*/
	input_sync(input->input_dev);
	mutex_unlock(&input->input_mutex);
	FHP_DEBUG("out");
	return 0;
}

static long fhp_input_get_cfg(struct fhp_input_core *input, unsigned long arg)
{
	if (copy_to_user((void *)arg, (void *)&input->input_cfg, sizeof(struct fhp_input_cfg))) {
		FHP_ERROR("copy input config to userspace fail");
		return -EFAULT;
	}

	return 0;
}

static long fhp_input_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct fhp_input_core *input = container_of(filp->private_data, struct fhp_input_core, input_miscdev);

	if (!input) {
		FHP_ERROR("input data is null");
		return -ENODATA;
	}

	FHP_DEBUG("ioctl cmd:%x,arg:%lx", cmd, arg);
	switch (cmd) {
	case FHP_INPUT_IOCTL_REPORT:
		ret = fhp_input_report(input, arg);
		break;
	case FHP_INPUT_IOCTL_GET_INPUT_CFG:
		ret = fhp_input_get_cfg(input, arg);
		break;
	default:
		FHP_ERROR("unkown ioctl cmd(0x%x)", (int)cmd);
		return -EINVAL;
	}

	return ret;
}
#if 0
static unsigned int fhp_input_poll(struct file *filp, struct poll_table_struct *wait)
{
	unsigned int mask = 0;
	struct fhp_input_core *input = container_of(filp->private_data, struct fhp_input_core, input_miscdev);

	if (!input) {
		FHP_ERROR("input data is null");
		return -ENODATA;
	}
#if 0
	poll_wait(filp, &fhp_data->fhp_frame.wait_queue_head, wait);
	FHP_DEBUG("poll triggerd:%d", fhp_data->fhp_frame.triggered);
	if (fhp_data->fhp_frame.triggered) {
		mask |= POLLIN | POLLRDNORM;
	}
	fhp_data->fhp_frame.triggered = 0;
#endif
	return mask;
}
#endif
static struct file_operations fhp_input_fops = {
	.open = fhp_input_open,
	.release = fhp_input_close,
	.unlocked_ioctl = fhp_input_ioctl,
	//    .poll = fhp_input_poll,
};

static int fhp_input_miscdev_init(struct fhp_input_core *input)
{
	int ret = 0;

	input->input_miscdev.minor = MISC_DYNAMIC_MINOR;
	input->input_miscdev.name = FHP_INPUT_MISCDEV_NAME;
	input->input_miscdev.fops = &fhp_input_fops;
	ret = misc_register(&input->input_miscdev);
	if (ret) {
		FHP_ERROR("misc_register(input misc device) fail");
		return ret;
	}

	FHP_INFO("misc_register(input misc device) success");
	return 0;
}

static void fhp_input_config_default(struct fhp_input_core *input)
{
	input->input_cfg.x_max = FHP_MAX_X_DEFAULT;
	input->input_cfg.y_max = FHP_MAX_Y_DEFAULT;
}

static void fhp_dt_parse_input(struct fhp_input_core *input, struct device_node *node)
{
	int ret = 0;
	int value = 0;

	ret = of_property_read_u32(node, "focaltech,x_max", &value);
	if (ret < 0) {
		FHP_ERROR("parse <focaltech,x_max> fail,ret:%d,val:%d", ret, value);
	}
	input->input_cfg.x_max = value;

	ret = of_property_read_u32(node, "focaltech,y_max", &value);
	if (ret < 0) {
		FHP_ERROR("parse <focaltech,y_max> fail,ret:%d,val:%d", ret, value);
	}
	input->input_cfg.y_max = value;

	FHP_INFO("x_max:%d,y_max:%d", input->input_cfg.x_max, input->input_cfg.y_max);
}

static int fhp_input_register(struct fhp_input_core *input)
{
	int ret = 0;
	struct input_dev *input_dev;

	FHP_INFO("input_dev register");
	input_dev = input_allocate_device();
	if (!input_dev) {
		FHP_ERROR("Failed to allocate memory for input device");
		return -ENOMEM;
	}

	/*register input*/
	input_dev->name = FHP_INPUT_NAME;
	set_bit(EV_SYN, input_dev->evbit);
	set_bit(EV_ABS, input_dev->evbit);
	set_bit(EV_KEY, input_dev->evbit);
	set_bit(BTN_TOUCH, input_dev->keybit);
	set_bit(INPUT_PROP_DIRECT, input_dev->propbit);
	input_mt_init_slots(input_dev, FHP_MAX_PONITS, INPUT_MT_DIRECT);
	input_set_abs_params(input_dev, ABS_MT_POSITION_X, 0, input->input_cfg.x_max, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y, 0, input->input_cfg.y_max, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, 0xFF, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_PRESSURE, 0, 0xFF, 0, 0);

	/*For Gesture*/
	input_set_capability(input_dev, EV_KEY, KEY_U);
	input_set_capability(input_dev, EV_KEY, KEY_T);

	ret = input_register_device(input_dev);
	if (ret) {
		FHP_ERROR("input device registration fail");
		input_free_device(input_dev);
		input_dev = NULL;
		return ret;
	}

	input->input_dev = input_dev;
	return 0;
}

int fhp_input_init(struct fhp_core_data *fhp_data)
{
	int ret = 0;
	struct fhp_input_core *input;

	FHP_INFO("input sub-system init");
	input = kzalloc(sizeof(struct fhp_input_core), GFP_KERNEL);
	if (!input) {
		FHP_ERROR("kzalloc for fhp_input fail");
		return -ENOMEM;
	}

	/*get input config*/
	fhp_input_config_default(input);
	fhp_dt_parse_input(input, fhp_data->spi->dev.of_node);

	/*register input device*/
	ret = fhp_input_register(input);
	if (ret) {
		FHP_ERROR("input register fail");
		kfree(input);
		return ret;
	}

	/*register input misc device*/
	ret = fhp_input_miscdev_init(input);
	if (ret) {
		FHP_ERROR("input misc device register fail");
		input_unregister_device(input->input_dev);
		kfree(input);
		return ret;
	}

	mutex_init(&input->input_mutex);
	input->fhp_data = fhp_data;
	fhp_input = input;
	return 0;
}

int fhp_input_exit(void)
{
	if (fhp_input) {
		misc_deregister(&fhp_input->input_miscdev);
		input_unregister_device(fhp_input->input_dev);
		kfree(fhp_input);
		fhp_input = NULL;
	}
	return 0;
}
