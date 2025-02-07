/***************************************************************
** Copyright (C),  2023,  OPLUS Mobile Comm Corp.,  Ltd
** File : oplus_display_high_frequency_pwm.h
** Description : oplus high frequency PWM
** Version : 1.0
** Date : 2023/05/23
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**  Li.Ping      2023/05/23        1.0           Build this moudle
******************************************************************/
#ifndef _OPLUS_DISPLAY_HIGH_FREQUENCY_PWM_H_
#define _OPLUS_DISPLAY_HIGH_FREQUENCY_PWM_H_

/* please just only include linux common head file  */
#include <linux/kobject.h>
#include <linux/iio/consumer.h>

enum oplus_pwm_trubo_type {
	OPLUS_PWM_TRUBO_CLOSE = 0,
	OPLUS_PWM_TRUBO_SWITCH = 1,
	OPLUS_PWM_TRUBO_GLOBAL_OPEN_NO_SWITCH = 2,
};

enum oplus_pwm_turbo_log_level {
	OPLUS_PWM_TURBO_LOG_LEVEL_ERR = 0,
	OPLUS_PWM_TURBO_LOG_LEVEL_WARN = 1,
	OPLUS_PWM_TURBO_LOG_LEVEL_INFO = 2,
	OPLUS_PWM_TURBO_LOG_LEVEL_DEBUG = 3,
};

enum oplus_demura_mode {
	OPLUS_DEMURA_DBV_MODE0 = 0,
	OPLUS_DEMURA_DBV_MODE1 = 1,
	OPLUS_DEMURA_DBV_MODE2 = 2,
	OPLUS_DEMURA_DBV_MODE_MAX,
};

struct oplus_demura_setting_table {
	bool oplus_bl_demura_dbv_support;
	int bl_demura_mode;
	int demura_switch_dvb1;
	int demura_switch_dvb2;
};

/* -------------------- extern ---------------------------------- */
extern unsigned int oplus_pwm_turbo_log;

/* -------------------- pwm turbo debug log-------------------------------------------  */
#define PWM_TURBO_ERR(fmt, arg...)	\
	do {	\
		if (oplus_pwm_turbo_log >= OPLUS_PWM_TURBO_LOG_LEVEL_ERR)	\
			pr_err("[PWM_TURBO][ERR][%s:%d]"pr_fmt(fmt), __func__, __LINE__, ##arg);	\
	} while (0)

#define PWM_TURBO_WARN(fmt, arg...)	\
	do {	\
		if (oplus_pwm_turbo_log >= OPLUS_PWM_TURBO_LOG_LEVEL_WARN)	\
			pr_warn("[PWM_TURBO][WARN][%s:%d]"pr_fmt(fmt), __func__, __LINE__, ##arg);	\
	} while (0)

#define PWM_TURBO_INFO(fmt, arg...)	\
	do {	\
		if (oplus_pwm_turbo_log >= OPLUS_PWM_TURBO_LOG_LEVEL_INFO)	\
			pr_info("[PWM_TURBO][INFO][%s:%d]"pr_fmt(fmt), __func__, __LINE__, ##arg);	\
	} while (0)

#define PWM_TURBO_DEBUG(fmt, arg...)	\
	do {	\
		if (oplus_pwm_turbo_log >= OPLUS_PWM_TURBO_LOG_LEVEL_DEBUG)	\
			pr_info("[PWM_TURBO][DEBUG][%s:%d]"pr_fmt(fmt), __func__, __LINE__, ##arg);	\
	} while (0)

/* -------------------- function implementation ---------------------------------------- */
inline bool pwm_turbo_support(void);
int oplus_display_panel_set_pwm_status(void *data);
int oplus_display_panel_get_pwm_status(void *buf);
int oplus_display_panel_get_pwm_status_for_90hz(void *buf);
/* -------------------- oplus api nodes ----------------------------------------------- */
ssize_t oplus_display_get_high_pwm(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf);
ssize_t oplus_display_set_high_pwm(struct kobject *kobj,
                struct kobj_attribute *attr, const char *buf, size_t count);

/* config */

#endif /* _OPLUS_DISPLAY_HIGH_FREQUENCY_PWM_H_ */
