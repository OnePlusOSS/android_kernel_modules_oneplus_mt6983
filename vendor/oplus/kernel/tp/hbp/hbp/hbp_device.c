#include <linux/irqreturn.h>
#include <linux/delay.h>
#include <linux/of_gpio.h>
#include <linux/kthread.h>
#include <linux/version.h>
#include <linux/reboot.h>
#include <linux/sched.h>
#include <linux/sched/clock.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/input/mt.h>

#include "hbp_core.h"
#include "hbp_tui.h"
#include "hbp_frame.h"
#include "hbp_spi.h"

#define CREATE_TRACE_POINTS
#include "hbp_trace.h"

#define HBP_IOCTRL_GROUP 0xC5
#define HBP_IOCTRL_START		 		_IO(HBP_IOCTRL_GROUP, 0x03)
#define HBP_IOCTRL_SET_PARAM	 		_IO(HBP_IOCTRL_GROUP, 0x04)
#define HBP_IOCTRL_NOTIFY				_IO(HBP_IOCTRL_GROUP, 0x08)
#define HBP_IOCTRL_SYNC_KERNEL			_IO(HBP_IOCTRL_GROUP, 0x09)
#define HBP_IOCTRL_IRQ_ENABLE			_IO(HBP_IOCTRL_GROUP, 0x0A)
#define HBP_IOCTRL_RESET				_IO(HBP_IOCTRL_GROUP, 0x0B)
#define HBP_IOCTRL_POWER				_IO(HBP_IOCTRL_GROUP, 0x0C)
#define HBP_IOCTRL_WRITE				_IO(HBP_IOCTRL_GROUP, 0x0D)
#define HBP_IOCTRL_READ					_IO(HBP_IOCTRL_GROUP, 0x0E)
#define HBP_IOCTRL_SPI_SYNC				_IO(HBP_IOCTRL_GROUP, 0x0F)
#define HBP_IOCTRL_GET_FRAME			_IO(HBP_IOCTRL_GROUP, 0x10)
#define HBP_IOCTRL_SET_FRAMESIZE		_IO(HBP_IOCTRL_GROUP, 0x11)
#define HBP_IOCTRL_TRIGGER_IFP			_IO(HBP_IOCTRL_GROUP, 0x12)
#define HBP_IOCTRL_CLEAR_FIFO			_IO(HBP_IOCTRL_GROUP, 0x13)
#define HBP_IOCTRL_SET_DEBUG_LEVEL		_IO(HBP_IOCTRL_GROUP, 0x14)

extern void hbp_state_notify(struct hbp_core *hbp, int id, hbp_panel_event event);
extern void hbp_register_notify_cb(struct hbp_device *hbp_dev, struct device *dev);
extern void hbp_power_ctrl(struct hbp_device *hbp_dev, struct power_sequeue sq[]);
static int hbp_register_irq_func(struct hbp_device *hbp_dev);
extern int hbp_init_pinctrl(struct device *dev, struct hbp_device *hbp_dev);
extern int hbp_init_power(struct device *dev, struct hbp_device *hbp_dev);
extern void hbp_core_set_gesture_coord(struct gesture_info *gesture);
extern bool hbp_tui_mem_map(struct hbp_core *hbp, struct hbp_device *hbp_dev, const char *vm_env);

extern struct hbp_core *g_hbp;
static struct file_operations hbp_ctrl_fops;

static void hbp_assert_ops(struct dev_operations *ops)
{
	assert(!IS_ERR_OR_NULL(ops->get_frame));
}

static void hbp_start_flow(struct hbp_core *hbp)
{
	//TODO: set daemon is ready.
}

static int init_input_device(struct hbp_device *hbp_dev, int id)
{
	int ret = 0;

	hbp_dev->i_dev = input_allocate_device();
	if (!hbp_dev->i_dev) {
		hbp_err("Failed to allocate input device %d\n", id);
		return -ENODEV;
	}

	if (id == 0) {
		hbp_dev->i_dev->name = TOUCH_NAME;
	} else if (id == 1) {
		hbp_dev->i_dev->name = TOUCH_NAME"1";
	} else {
		hbp_dev->i_dev->name = TOUCH_NAME;
	}

	set_bit(EV_SYN, hbp_dev->i_dev->evbit);
	set_bit(EV_ABS, hbp_dev->i_dev->evbit);
	set_bit(EV_KEY, hbp_dev->i_dev->evbit);
	set_bit(ABS_MT_TOUCH_MAJOR, hbp_dev->i_dev->absbit);
	set_bit(ABS_MT_WIDTH_MAJOR, hbp_dev->i_dev->absbit);
	set_bit(ABS_MT_POSITION_X, hbp_dev->i_dev->absbit);
	set_bit(ABS_MT_POSITION_Y, hbp_dev->i_dev->absbit);
	set_bit(ABS_MT_PRESSURE, hbp_dev->i_dev->absbit);
	set_bit(ABS_MT_TOOL_TYPE, hbp_dev->i_dev->absbit);
	set_bit(INPUT_PROP_DIRECT, hbp_dev->i_dev->propbit);
	set_bit(BTN_TOUCH, hbp_dev->i_dev->keybit);
	set_bit(BTN_TOOL_FINGER, hbp_dev->i_dev->keybit);
	set_bit(KEY_F4, hbp_dev->i_dev->keybit);		/*for black gesture*/
	set_bit(KEY_POWER, hbp_dev->i_dev->keybit);		/*for apk test*/
	set_bit(KEY_SLEEP, hbp_dev->i_dev->keybit);

	input_mt_init_slots(hbp_dev->i_dev, TOUCH_MAX_FINGERS, INPUT_MT_DIRECT);
	input_set_abs_params(hbp_dev->i_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(hbp_dev->i_dev, ABS_MT_POSITION_X, 0,
			     hbp_dev->hw.resolution.x - 1, 0, 0);
	input_set_abs_params(hbp_dev->i_dev, ABS_MT_POSITION_Y, 0,
			     hbp_dev->hw.resolution.y - 1, 0, 0);
	input_set_drvdata(hbp_dev->i_dev, hbp_dev);

	ret = input_register_device(hbp_dev->i_dev);
	if (ret < 0) {
		input_free_device(hbp_dev->i_dev);
		return -ENODEV;
	}

	return 0;
}

static int hbp_device_dt_parse(struct hbp_core *hbp, struct hbp_device *hbp_dev)
{
	int ret = 0;
	struct device *dev = hbp_dev->dev;
	struct device_node *np = dev->of_node;
	const char *vm_label;

	/*for power control*/
	ret = hbp_init_power(dev, hbp_dev);
	if (ret < 0) {
		hbp_err("failed to init power\n");
		return ret;
	}

	ret = hbp_init_pinctrl(dev, hbp_dev);
	if (ret < 0) {
		hbp_err("failed to init pin ctrl\n");
		return ret;
	}

	ret = of_property_read_u32_array(np, "device,resolution", (uint32_t *)&hbp_dev->hw.resolution, 2);
	if (ret < 0) {
		hbp_err("failed to get resolution\n");
		return ret;
	}

	/*for interrupts*/
	hbp_dev->hw.irq_gpio = of_get_named_gpio_flags(np, "irq-gpio", 0, &hbp_dev->irq_flags);
	if (!hbp_dev->hw.irq_gpio) {
		hbp_err("Failed to get irq gpio\n");
		return -ENODEV;
	}

	if (!gpio_is_valid(hbp_dev->hw.irq_gpio)) {
		hbp_err("irq gpio is not valid\n");
		return -ENODEV;
	}
	hbp_dev->irq = gpio_to_irq(hbp_dev->hw.irq_gpio);
	hbp_debug("irq gpio:%d, irq:%d\n", hbp_dev->hw.irq_gpio, hbp_dev->irq);

	ret = of_property_read_string(np, "vm,label", &vm_label);
	if (!ret) {
		if (hbp_tui_mem_map(hbp, hbp_dev, vm_label)) {
#if IS_ENABLED(CONFIG_GH_ARM64_DRV)
			hbp_info("tui mem matched, env:%d %s\n",
				 hbp_dev->env,
				 hbp_dev->vm_info->mem->env_type);
#endif
		}
	}

	return 0;
}

static int hbp_match_bus(struct device *dev, struct bus_operations **bus_ops)
{
	struct device_node *np = NULL;
	struct device *bus_dev = NULL;
	struct spi_bus *bus;

	np = of_parse_phandle(dev->of_node, "device,attached_bus", 0);
	if (!np) {
		hbp_err("Failed to find attached bus\n");
		return -ENODEV;
	}

	bus_dev = bus_find_device_by_name(&platform_bus_type, NULL, np->full_name);
	if (!bus_dev) {
		hbp_err("Failed to match bus: %s\n", np->full_name);
		return -ENODEV;
	}

	hbp_debug("matched bus:%s\n", np->full_name);
	bus = (struct spi_bus *)bus_dev->platform_data;
	if (bus) {
		*bus_ops = &bus->spi_ops;
	}

	return 0;
}

static int hbp_create_dev(struct hbp_core *hbp, struct hbp_device *hbp_dev, int id)
{
	struct device *dev;
	int major;

	major = register_chrdev(0, HBP_CORE, &hbp_ctrl_fops);
	if (major < 0) {
		hbp_fatal("Failed to register char device \n");
		goto err;
	}

	dev = device_create(hbp->cls, NULL, MKDEV(major, id),
			    hbp_dev, HBP_CTRL"%d", id);
	if (IS_ERR(dev)) {
		hbp_fatal("Failed to create device\n");
		goto err_class;
	}

	return 0;

err_class:
	unregister_chrdev(major, HBP_CORE);
err:
	return -EINVAL;
}

static void hbp_queue_init(struct frame_queue *queue)
{
	mutex_init(&queue->lock);
	init_waitqueue_head(&queue->waitq);
	INIT_LIST_HEAD(&queue->freed);
	INIT_LIST_HEAD(&queue->cunsume);

	queue->data = NULL;
	queue->data_size = 0;
	queue->waitq_flag = QUEUE_WAIT;
}

static void hbp_panel_notify_callback(hbp_panel_event event, struct hbp_device *hbp_dev)
{
	if (event != HBP_PANEL_EVENT_UNKNOWN) {
		hbp_state_notify(g_hbp, hbp_dev->id, event);
	}
}

struct hbp_device *hbp_device_create(void *priv,
				     struct hbp_core *hbp,
				     struct device *dev,
				     struct dev_operations *dev_ops,
				     struct bus_operations **bus_ops,
				     int id)
{
	struct hbp_device *hbp_dev;
	int ret = -EINVAL;

	hbp_assert_ops(dev_ops);

	hbp_dev = kzalloc(sizeof(struct hbp_device), GFP_KERNEL);
	if (!hbp_dev) {
		hbp_err("Failed to alloc memory\n");
		goto exit;
	}

	hbp_dev->state = HBP_PANEL_EVENT_RESUME;
	hbp_dev->priv = priv;
	hbp_dev->dev_ops = dev_ops;
	hbp_dev->dev = dev;
	hbp_dev->id = id;
	hbp_dev->panel_cb = hbp_panel_notify_callback;

	ret = hbp_create_dev(hbp, hbp_dev, id);
	if (ret < 0) {
		hbp_err("failed to create control device\n");
		goto exit;
	}

	init_waitqueue_head(&hbp_dev->poll_wait);

	mutex_init(&hbp_dev->mifp);
	mutex_init(&hbp_dev->mlock);
	init_waitqueue_head(&hbp_dev->sts_event);
	init_waitqueue_head(&hbp_dev->drv_event);

	hbp_queue_init(&hbp_dev->frame_queue);

	ret = hbp_device_dt_parse(hbp, hbp_dev);
	if (ret < 0) {
		hbp_err("Failed to parse device dt\n");
		goto exit;
	}

	/*register input device*/
	ret = init_input_device(hbp_dev, id);
	if (ret < 0) {
		hbp_err("Failed to init input device %d\n", id);
		goto exit;
	}

	/*match bus, spi or i2c*/
	ret = hbp_match_bus(dev, bus_ops);
	if (ret < 0) {
		hbp_err("Failed to match bus\n");
		goto exit;
	}

	hbp_dev->drv_ack = DRV_NOREADY;
	hbp_dev->irq_freed = true;
	ret = hbp_register_irq_func(hbp_dev);
	if (ret < 0) {
		hbp_err("Failed to register irq\n");
		goto exit;
	}

	hbp_dev->screenoff_ifp = false;
	hbp_register_notify_cb(hbp_dev, dev);

	return hbp_dev;

exit:
	if (hbp_dev) {
		kfree(hbp_dev);
		hbp_dev = NULL;
		hbp_err("hbp_dev is free\n");
	}
	return NULL;
}

#if 0
static void hbp_pm_awake(struct hbp_core *hbp, bool hold)
{
	static DEFINE_SPINLOCK(ws_lock);
	static int count = 0;

	if (!hbp || !hbp->ws) {
		return;
	}

	spin_lock(&ws_lock);
	if (hold) {
		if (!count) {
			__pm_stay_awake(hbp->ws);
		}
		count++;
	} else {
		count--;
		if (!count) {
			__pm_relax(hbp->ws);
		}
	}
	spin_unlock(&ws_lock);
}
#endif

static void hbp_pm_wakeup_timeout(struct hbp_core *hbp, const unsigned long j)
{
	if (!hbp || !hbp->ws) {
		return;
	}

	__pm_wakeup_event(hbp->ws, jiffies_to_msecs(j));

}

static void driver_notify_kernel(struct hbp_device *hbp_dev, int drv_ack)
{
	hbp_dev->drv_ack = drv_ack;
	wake_up_all(&hbp_dev->drv_event);
}

static int sync_with_driver(struct hbp_device *hbp_dev)
{
	int ret = 0;

	ret = wait_event_timeout(hbp_dev->drv_event,
				 (hbp_dev->drv_ack == DRV_ACK),
				 msecs_to_jiffies(DRIVER_SYNC_TIMEOUT));
	if (!ret) {
		hbp_err("failed to wait drv ack %d\n", hbp_dev->drv_ack);
		return -1;
	}

	return ret;
}

static void hbp_fingerprint_report(struct hbp_device *hbp_dev, struct gesture_info *gesture, int source)
{
	struct fp_event fp_ev;
	static int pre_state = -1;

	mutex_lock(&hbp_dev->mifp);
	memset(&fp_ev, 0, sizeof(fp_ev));
	fp_ev.id = hbp_dev->id;
	fp_ev.x = gesture->Point_start.x;
	fp_ev.y = gesture->Point_start.y;
	fp_ev.touch_state = gesture->type == FingerprintDown?1:0;

	/*screen off source*/
	if (!source) {
		hbp_dev->screenoff_ifp = gesture->type == FingerprintDown?true:false;
	}

	hbp_info("screen %s fingerprint %s:(%d, %d)(screenoff_ifp = %d)\n",
		 source?"on":"off",
		 fp_ev.touch_state?"down":"up",
		 fp_ev.x,
		 fp_ev.y,
		 hbp_dev->screenoff_ifp);

	if (pre_state != fp_ev.touch_state) {
		hbp_event_call_notifier(EVENT_ACTION_FOR_FINGPRINT, (void *)&fp_ev);
	}
	pre_state = fp_ev.touch_state;

	mutex_unlock(&hbp_dev->mifp);
}

static void hbp_gesture_report(struct hbp_device *hbp_dev, struct gesture_info *gesture)
{

	if (gesture->type == FingerprintDown ||
	    gesture->type == FingerprintUp) {
		hbp_fingerprint_report(hbp_dev, gesture, 0);
	} else {
		hbp_info("device-%d detect %s gesture\n",
			hbp_dev->id,
			gesture->type == DoubleTap? "double tap" :
			gesture->type == UpVee? "up vee" :
			gesture->type == DownVee? "down vee" :
			gesture->type == LeftVee? "(>)" :
			gesture->type == RightVee? "(<)" :
			gesture->type == Circle? "circle" :
			gesture->type == DoubleSwip? "(||)" :
			gesture->type == Left2RightSwip? "(-->)" :
			gesture->type == Right2LeftSwip? "(<--)" :
			gesture->type == Up2DownSwip? "up to down |" :
			gesture->type == Down2UpSwip? "down to up |" :
			gesture->type == Mgestrue? "(M)" :
			gesture->type == Wgestrue? "(W)" :
			gesture->type == FingerprintDown? "(fingerprintdown)" :
			gesture->type == FingerprintUp? "(fingerprintup)" :
			gesture->type == SingleTap? "single tap" :
			gesture->type == Heart? "heart" :
			gesture->type == PenDetect? "(pen detect)" :
			gesture->type == SGesture? "(S)" : "unknown");

		if (gesture->type != UnknownGesture) {
			gesture->id = hbp_dev->id;
			hbp_core_set_gesture_coord(gesture);

			//back up gesture info
			input_report_key(hbp_dev->i_dev, KEY_F4, 1);
			input_sync(hbp_dev->i_dev);
			input_report_key(hbp_dev->i_dev, KEY_F4, 0);
			input_sync(hbp_dev->i_dev);
		} else {
			hbp_err("detect unkown gesture\n");
		}
	}
}

static inline void tp_touch_down(struct hbp_device *hbp_dev, struct point_info points, int touch_report_num, int id)
{
	if (hbp_dev->i_dev == NULL) {
		return;
	}

	input_report_key(hbp_dev->i_dev, BTN_TOUCH, 1);
	input_report_key(hbp_dev->i_dev, BTN_TOOL_FINGER, 1);

	if (touch_report_num == 1) {
		input_report_abs(hbp_dev->i_dev, ABS_MT_WIDTH_MAJOR, points.width_major);
		hbp_dev->last_width_major = points.width_major;
		hbp_dev->last_touch_major = points.touch_major;
	} else if (!(touch_report_num & 0x7f) || touch_report_num == 30) {
		/*if touch_report_num == 127, every 127 points, change width_major*/
		/*down and keep long time, auto repeat per 5 seconds, for weixing*/
		/*report move event after down event, for weixing voice delay problem, 30 -> 300ms in order to avoid the intercept by shortcut*/
		if (hbp_dev->last_width_major == points.width_major) {
			hbp_dev->last_width_major = points.width_major + 1;
		} else {
			hbp_dev->last_width_major = points.width_major;
		}

		input_report_abs(hbp_dev->i_dev, ABS_MT_WIDTH_MAJOR, hbp_dev->last_width_major);
	}

	hbp_dev->last_touch_major = points.touch_major;

	/*smart_gesture_support*/
	if (hbp_dev->last_touch_major > SMART_GESTURE_THRESHOLD) {
		input_report_abs(hbp_dev->i_dev, ABS_MT_TOUCH_MAJOR, hbp_dev->last_touch_major);
	} else {
		input_report_abs(hbp_dev->i_dev, ABS_MT_TOUCH_MAJOR, SMART_GESTURE_LOW_VALUE);
	}

	/*pressure_report_support*/
	input_report_abs(hbp_dev->i_dev, ABS_MT_PRESSURE,
				hbp_dev->last_touch_major);   /*add for fixing gripview tap no function issue*/

	input_report_abs(hbp_dev->i_dev, ABS_MT_POSITION_X, points.x);
	input_report_abs(hbp_dev->i_dev, ABS_MT_POSITION_Y, points.y);

	if (!CHK_BIT(hbp_dev->irq_slot, (1 << id))) {
		hbp_info("first touch point id %d [%4d %4d %4d %4d %4d %4d %4d]\n", id, points.x, points.y, points.z,
					points.rx_press, points.tx_press, points.rx_er, points.tx_er);
	}

	hbp_debug("Touchpanel id %d :Down[%4d %4d %4d %4d %4d %4d %4d]\n", id, points.x, points.y, points.z,
						points.rx_press, points.tx_press, points.rx_er, points.tx_er);
}

static inline void tp_touch_up(struct hbp_device *hbp_dev)
{
	if (hbp_dev->i_dev == NULL) {
		return;
	}

	input_report_key(hbp_dev->i_dev, BTN_TOUCH, 0);
	input_report_key(hbp_dev->i_dev, BTN_TOOL_FINGER, 0);
}

static void hbp_touch_points_report(struct hbp_device *hbp_dev, struct point_info *points, int obj_attention)
{
	int i = 0;
	uint8_t finger_num = 0;
	if ((obj_attention & TOUCH_BIT_CHECK) != 0) {
		hbp_dev->up_status = false;

		for (i = 0; i < MAX_TOUCH_POINTS; i++) {
			if (((obj_attention & TOUCH_BIT_CHECK) >> i) & 0x01
			    && (points[i].status == 0)) { /* buf[0] == 0 is wrong point, no process*/
				continue;
			}

			if (((obj_attention & TOUCH_BIT_CHECK) >> i) & 0x01
			    && (points[i].status != 0)) {
				input_mt_slot(hbp_dev->i_dev, i);
				input_mt_report_slot_state(hbp_dev->i_dev, MT_TOOL_FINGER, 1);
				hbp_dev->touch_report_num++;
				tp_touch_down(hbp_dev, points[i], hbp_dev->touch_report_num, i);
				SET_BIT(hbp_dev->irq_slot, (1 << i));
				finger_num++;

				/*strore  the last point data*/
				/*retval = tp_memcpy(&ts->last_point, sizeof(ts->last_point), \
					  &points[i], sizeof(struct point_info), \
					  sizeof(struct point_info));
				if (retval < 0) {
					hbp_err(ts->tp_index, "tp_memcpy failed.\n");
				}*/
			} else {
				input_mt_slot(hbp_dev->i_dev, i);
				input_mt_report_slot_state(hbp_dev->i_dev, MT_TOOL_FINGER, 0);
			}
		}
	} else {
		if (hbp_dev->up_status) {
			tp_touch_up(hbp_dev);
			return;
		}

		for (i = 0; i < MAX_TOUCH_POINTS; i++) {
			input_mt_slot(hbp_dev->i_dev, i);
			input_mt_report_slot_state(hbp_dev->i_dev, MT_TOOL_FINGER, 0);
		}

		tp_touch_up(hbp_dev);
		hbp_dev->irq_slot = 0;
		hbp_dev->up_status = true;
		hbp_info("all touch up, finger_num=%d\n", finger_num);
	}

	input_sync(hbp_dev->i_dev);
}

static irqreturn_t hbp_irq_handler(int irq, void *dev_id)
{
	struct hbp_device *hbp_dev = (struct hbp_device *)dev_id;

	if (hbp_dev->i_dev) {
		input_set_timestamp(hbp_dev->i_dev, ktime_get());
	}

	return IRQ_WAKE_THREAD;
}

static irqreturn_t hbp_irq_threaded_fn(int irq, void *dev_id)
{
	struct hbp_device *hbp_dev = (struct hbp_device *)dev_id;
	struct frame_queue *queue = &hbp_dev->frame_queue;
	struct gesture_info gesture;
	struct point_info points[MAX_TOUCH_POINTS];
	enum irq_reason reason = IRQ_REASON_NORMAL;
	int ret = 0;
	int obj_attention = 0;

	//hbp_pm_awake(g_hbp, true);
	hbp_pm_wakeup_timeout(g_hbp, HZ);

	ret = hbp_dev->dev_ops->get_irq_reason(hbp_dev->priv, &reason);
	if (ret < 0) {
		hbp_err("failed to get irq type\n");
	} else {
		if (reason == IRQ_REASON_RESET_WDT
				|| reason == IRQ_REASON_RESET_PWR
				|| reason == IRQ_REASON_RESET_FWUPDATE) {
			goto report_frame;
		}
	}

	//black gesture or fingerprint flow
	if (unlikely(hbp_dev->state == HBP_PANEL_EVENT_SUSPEND ||
		(hbp_dev->state == HBP_PANEL_EVENT_EARLY_SUSPEND))) {
		memset(&gesture, 0, sizeof(gesture));
		ret = hbp_dev->dev_ops->get_gesture(hbp_dev->priv, &gesture);
		if (ret < 0) {
			hbp_err("failed to get gesture type\n");
		} else {
			hbp_gesture_report(hbp_dev, &gesture);
			//for debug gesture, record frame data
			if (hbp_dev->debug.report_gesture_frm) {
				goto report_frame;
			}
		}
		goto exit;
	}

	if (!g_hbp->in_hbp_mode) {
		memset(points, 0, sizeof(struct point_info) * MAX_TOUCH_POINTS);
		obj_attention = hbp_dev->dev_ops->get_touch_points(hbp_dev->priv, points);
		if (obj_attention < 0) {
			hbp_err("failed to get touch points\n");
		} else {
			hbp_touch_points_report(hbp_dev, points, obj_attention);
		}
		goto exit;
	}
report_frame:
	ret = hbp_dev->dev_ops->get_frame(hbp_dev->priv,
					  queue->data,
					  queue->data_size);
	if (ret >= 0) {
		hbp_dev->frame_queue.frame_count++;
		frame_put(queue->data, queue->data_size, reason, queue);
		frame_wake_up_waitq(queue);
		goto exit;
	}

	if (hbp_dev->drv_ack == DRV_NOREADY) {
		goto exit;
	}

	hbp_dev->drv_ack = DRV_INT;
	wake_up_interruptible(&hbp_dev->poll_wait);
	/*sync with driver*/
	if (sync_with_driver(hbp_dev) < 0) {
		hbp_err("failed to sync with user driver\n");
	}

exit:
	//hbp_pm_awake(g_hbp, false);
	return IRQ_HANDLED;
}

int hbp_unregister_irq(struct hbp_device *hbp_dev)
{
	if (hbp_dev->irq_freed) {
		return -EBUSY;
	}

	disable_irq(hbp_dev->irq);
	free_irq(hbp_dev->irq, hbp_dev);
	hbp_dev->irq_freed = true;

	return 0;
}

static int hbp_register_irq_func(struct hbp_device *hbp_dev)
{
	int ret = 0;
	const char *irq_name = NULL;

	if (hbp_dev->id == 0) {
		irq_name = "touch-00";
	} else if (hbp_dev->id == 1) {
		irq_name = "touch-01";
	} else {
		irq_name = "touch-02";
	}

	if (hbp_dev->irq_freed) {
		ret = request_threaded_irq(hbp_dev->irq,
					   hbp_irq_handler,
					   hbp_irq_threaded_fn,
					   hbp_dev->irq_flags | IRQF_ONESHOT | IRQF_NO_SUSPEND,
					   irq_name,
					   hbp_dev);
		if (ret < 0) {
			hbp_err("Failed to register irq func\n");
			return ret;
		}

		disable_irq(hbp_dev->irq);
		hbp_dev->irq_freed = false;
		hbp_dev->irq_enabled = false;
	}

	return 0;
}

static int hbp_dev_spi_sync(struct hbp_device *hbp_dev, int cmd, union usr_data *usr)
{
	int ret = 0;
	static char *_wr = NULL;
	static char *_rd = NULL;

	trace_hbp(hbp_dev->id, "spi-sync", TRACE_START);

	if (!_wr) {
		_wr = kzalloc(PAGE_SIZE, GFP_KERNEL);
		if (!_wr) {
			hbp_err("failed to alloc spi write buffer\n");
			goto alloc_err;
		}
	}

	if (!_rd) {
		_rd = kzalloc(PAGE_SIZE, GFP_KERNEL);
		if (!_rd) {
			hbp_err("failed to alloc spi read buffer\n");
			goto alloc_err;
		}
	}

	if (usr->rw.len > PAGE_SIZE) {
		hbp_warn("max write or read max length \n");
		goto alloc_err;
	}

	if (usr->rw.tx) {
		if (copy_from_user(_wr, usr->rw.tx, usr->rw.len)) {
			hbp_info("failed to copy tx from user, abort\n");
			goto copy_err;
		}
	}

	hbp_dev->dev_ops->spi_sync(hbp_dev->priv, _wr, _rd, usr->rw.len);

	if (usr->rw.rx) {
		if (copy_to_user(usr->rw.rx, _rd, usr->rw.len)) {
			hbp_info("faile to copy rx to user\n");
			goto copy_err;
		}
	}

	trace_hbp(hbp_dev->id, "spi-sync", TRACE_END);
	return 0;

alloc_err:
	ret = -ENOMEM;
copy_err:
	ret = -EINVAL;
	trace_hbp(hbp_dev->id, "spi-sync", TRACE_END);
	return ret;
}

static int hbp_queue_config(unsigned int buf_size, struct frame_queue *queue)
{
	int ret = 0;
	int data_size = 0;

	data_size = buf_size - sizeof(struct frame_buf) + 1;
	hbp_info("queue config size buf:%d data:%d\n", buf_size, data_size);

	if (data_size > 0 && (data_size > queue->data_size)) {
		queue->data_size = data_size;
		if (queue->data) {
			kfree(queue->data);
			queue->data = NULL;
		}
		queue->data = kzalloc(data_size, GFP_KERNEL);
		if (!queue->data) {
			return -ENOMEM;
		}
		ret = frame_init(queue, buf_size);
		if (ret < 0) {
			hbp_err("failed frame init\n");
			return ret;
		}
	}

	return 0;
}

static void hbp_set_irq_status(struct hbp_device *hbp_dev, bool en)
{
	if (hbp_dev->irq_enabled != (!!en)) {
		en ? enable_irq(hbp_dev->irq): disable_irq(hbp_dev->irq);
		hbp_dev->irq_enabled = (!!en);
	}
}

static void hbp_queue_clear(struct frame_queue *queue)
{
	frame_clear(queue);
}

static long hbp_ctrl_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct hbp_device *hbp_dev = (struct hbp_device *)filp->private_data;
	union usr_data usr;
	struct gesture_info gesture;

	if (copy_from_user(&usr, (void *)arg, sizeof(union usr_data))) {
		hbp_err("failed to copy data from user space\n");
		return -EFAULT;
	}

	hbp_debug("unlocked ioctl cmd: 0x%x\n", cmd);

	switch (cmd) {
	case HBP_IOCTRL_POWER:
		hbp_power_ctrl(hbp_dev, usr.sq);
		break;
	case HBP_IOCTRL_SYNC_KERNEL:
		driver_notify_kernel(hbp_dev, usr.val);
		break;
	case HBP_IOCTRL_NOTIFY:
		hbp_dev->drv_ack = usr.val;
		break;
	case HBP_IOCTRL_IRQ_ENABLE:
		hbp_set_irq_status(hbp_dev, usr.val);
		break;
	case HBP_IOCTRL_START:
		hbp_start_flow(NULL);
		break;
	case HBP_IOCTRL_WRITE:
	case HBP_IOCTRL_READ:
	case HBP_IOCTRL_SPI_SYNC:
		ret = hbp_dev_spi_sync(hbp_dev, cmd, &usr);
		if (ret < 0) {
			hbp_err("failed to write ");
			return ret;
		}
		break;
	case HBP_IOCTRL_GET_FRAME:
		ret = frame_get(usr.frame.data, usr.frame.size, &hbp_dev->frame_queue);
		if (ret < 0) {
			return -EFAULT;
		}
		break;
	case HBP_IOCTRL_SET_FRAMESIZE:
		ret = hbp_queue_config(usr.val, &hbp_dev->frame_queue);
		break;
	case HBP_IOCTRL_TRIGGER_IFP:
		if (usr.ifp.state == 1) {
			gesture.type = FingerprintDown;
			gesture.Point_start.x = usr.ifp.x;
			gesture.Point_start.y = usr.ifp.y;
		} else {
			gesture.type = FingerprintUp;
			gesture.Point_start.x = 0;
			gesture.Point_start.y = 0;
		}
		hbp_fingerprint_report(hbp_dev, &gesture, 1);
		break;
	case HBP_IOCTRL_CLEAR_FIFO:
		hbp_queue_clear(&hbp_dev->frame_queue);
		break;
	case HBP_IOCTRL_SET_DEBUG_LEVEL:
		set_debug_level(usr.val);
		hbp_dev->debug.report_gesture_frm = (LOG_LEVEL_DEBUG == usr.val) ? true : false;
		break;
	default:
		hbp_err("invalid cmd\n");
		break;
	}

	return ret;
}

static __poll_t hbp_virq_poll(struct file *file, poll_table *wait)
{
	__poll_t mask = 0;
	struct hbp_device *hbp_dev = (struct hbp_device *)file->private_data;

	poll_wait(file, &hbp_dev->poll_wait, wait);

	if (hbp_dev->drv_ack == DRV_INT) {
		mask = EPOLLIN | EPOLLRDNORM;
	}

	return mask;
}

static int hbp_ctrl_release(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	file->private_data = NULL;
	return 0;
}

static int hbp_ctrl_open(struct inode *inode, struct file *file)
{
	if (!try_module_get(THIS_MODULE)) {
		return -ENODEV;
	}

	if (iminor(inode) >= MAX_DEVICES) {
		hbp_err("invalid iminor(inode): %d\n", iminor(inode));
		return -EINVAL;
	}

	file->private_data = (void *)g_hbp->devices[iminor(inode)];
	return 0;
}

static struct file_operations hbp_ctrl_fops = {
	.owner = THIS_MODULE,
	.open = hbp_ctrl_open,
	.unlocked_ioctl = hbp_ctrl_unlocked_ioctl,
	.poll = hbp_virq_poll,
	.release = hbp_ctrl_release,
};

void hbp_set_irq_wake(struct hbp_device *hbp_dev, bool wake)
{
	if (wake) {
		enable_irq_wake(hbp_dev->irq);
	} else {
		disable_irq_wake(hbp_dev->irq);
	}
	hbp_info("%s irq wake\n", wake?"enable":"disable");
}
