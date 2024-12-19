/***************************************************************
** Copyright (C),  2023,  OPLUS Mobile Comm Corp.,  Ltd
** File : oplus_display_high_frequency_pwm.c
** Description : oplus high frequency PWM
** Version : 1.0
** Date : 2023/05/23
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**  Li.Ping      2023/05/23        1.0           Build this moudle
******************************************************************/
#include "oplus_display_high_frequency_pwm.h"
#include <linux/thermal.h>
#include <linux/delay.h>
#include <../drm/drm_device.h>
#include <../drm/drm_crtc.h>
#include "mtk_panel_ext.h"
#include "mtk_drm_crtc.h"
#include "mtk_drm_drv.h"
#include "mtk_debug.h"
#include "mtk_dsi.h"
#include "oplus_display_trace.h"
#include "mtk_drm_mmp.h"
#include "mtk_log.h"
#ifdef OPLUS_FEATURE_DISPLAY_TEMP_COMPENSATION
#include "oplus_display_temp_compensation.h"
#endif /* OPLUS_FEATURE_DISPLAY_TEMP_COMPENSATION */

/* -------------------- macro ---------------------------------- */
/* config bit setting */
#define REGFLAG_CMD									0xFFFA

/* -------------------- parameters ----------------------------- */
static DEFINE_MUTEX(g_pwm_turbo_lock);

unsigned int hpwm_bl = 0;
EXPORT_SYMBOL(hpwm_bl);
unsigned int hpwm_mode = 0;
EXPORT_SYMBOL(hpwm_mode);
unsigned int hpwm_fps_mode;
EXPORT_SYMBOL(hpwm_fps_mode);
/* log level config */
unsigned int oplus_pwm_turbo_log = 0;
EXPORT_SYMBOL(oplus_pwm_turbo_log);

unsigned int hpwm_90nit_set_temp;
EXPORT_SYMBOL(hpwm_90nit_set_temp);
unsigned int hpwm_mode_90hz = 0;
EXPORT_SYMBOL(hpwm_mode_90hz);

extern unsigned int oplus_display_brightness;
unsigned int last_backlight = 0;
EXPORT_SYMBOL(last_backlight);
bool pwm_power_on = false;
EXPORT_SYMBOL(pwm_power_on);

bool hpwm_onepulse_update_temp = false;
EXPORT_SYMBOL(hpwm_onepulse_update_temp);

struct LCM_setting_table {
	unsigned int cmd;
	unsigned int count;
	unsigned char para_list[256];
};

struct oplus_demura_setting_table demura_setting = {
	.oplus_bl_demura_dbv_support = false,
	.bl_demura_mode = OPLUS_DEMURA_DBV_MODE_MAX,
	.demura_switch_dvb1 = 0,
	.demura_switch_dvb2 = 0
};
EXPORT_SYMBOL(demura_setting);

/* -------------------- extern ---------------------------------- */
/* extern params */
extern unsigned int lcm_id1;
extern unsigned int lcm_id2;
extern unsigned int hpwm_mode_90hz;
extern unsigned int oplus_display_brightness;

/* extern functions */
extern struct drm_device *get_drm_device(void);
extern inline bool oplus_ofp_is_support(void);
extern bool oplus_ofp_backlight_filter(int bl_level);
extern void ddic_dsi_send_cmd(unsigned int cmd_num, char val[20]);
extern void mtk_read_ddic_v2(u8 ddic_reg, int ret_num, char ret_val[10]);
extern void mtk_crtc_cmdq_timeout_cb(struct cmdq_cb_data data);
extern void mtk_drm_send_lcm_cmd_prepare_wait_for_vsync(struct drm_crtc *crtc, struct cmdq_pkt **cmdq_handle);


/* -------------------- function implementation -------------------- */
inline bool pwm_turbo_support(void)
{
	struct drm_crtc *crtc;
	struct mtk_drm_crtc *mtk_crtc;
	struct drm_device *ddev = get_drm_device();

	/* this debug cmd only for crtc0 */
	crtc = list_first_entry(&(ddev)->mode_config.crtc_list,
				typeof(*crtc), head);
	if (!crtc) {
		printk(KERN_ERR "find crtc fail\n");
		return -1;
	}

	mtk_crtc = to_mtk_crtc(crtc);
	if (!mtk_crtc || !mtk_crtc->panel_ext || !mtk_crtc->panel_ext->params) {
		pr_err("falied to get lcd proc info\n");
		return -EINVAL;
	}

	pr_err("pwm_turbo support %s\n", mtk_crtc->panel_ext->params->vendor);

	if (!strcmp(mtk_crtc->panel_ext->params->vendor, "Tianma_NT37705")) {
		printk(KERN_ERR "support pwm turbo\n");
		return true;
	}

	return false;
}
EXPORT_SYMBOL(pwm_turbo_support);

int oplus_display_panel_set_pwm_turbo_switch(struct drm_crtc *crtc, unsigned int en)
{
	struct mtk_drm_crtc *mtk_crtc = to_mtk_crtc(crtc);
	struct mtk_ddp_comp *comp;
	struct mtk_panel_ext *ext = mtk_crtc->panel_ext;
	struct cmdq_pkt *cmdq_handle;
	struct mtk_crtc_state *state =
	    to_mtk_crtc_state(mtk_crtc->base.state);
	unsigned int src_mode =
	    state->prop_val[CRTC_PROP_DISP_MODE_IDX];
	unsigned int fps = 0;

	if (ext == NULL) {
		DDPINFO("%s %d mtk_crtc->panel_ext is NULL\n", __func__, __LINE__);
		return -EINVAL;
	}

	if (hpwm_mode == en) {
        DDPPR_ERR("hpwm_mode no changer\n");
		return 0;
	}

	pr_info("%s, en=%d, src_mode=%d, ext->params->dyn_fps.vact_timing_fps=%d\n",
		__func__, en, src_mode, ext->params->dyn_fps.vact_timing_fps);
	mutex_lock(&g_pwm_turbo_lock);
	DDP_MUTEX_LOCK(&mtk_crtc->lock, __func__, __LINE__);

	if (!mtk_crtc->enabled)
		goto done;

	comp = mtk_ddp_comp_request_output(mtk_crtc);
	if (!comp) {
		DDPPR_ERR("request output fail\n");
		DDP_MUTEX_UNLOCK(&mtk_crtc->lock, __func__, __LINE__);
		mutex_unlock(&g_pwm_turbo_lock);
		return -EINVAL;
	}

	/* because mode = 2 (90fps), not support hpwm */
	if (ext->params->dyn_fps.vact_timing_fps == 90) {
		hpwm_mode_90hz = 255;
		pr_info("%s, if 90fps goto done\n", __func__);
		goto done;
	} else {
		hpwm_mode_90hz = 1;
	}

	mtk_drm_send_lcm_cmd_prepare_wait_for_vsync(crtc, &cmdq_handle);

	/** wait one TE **/
	cmdq_pkt_clear_event(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
	if (mtk_drm_lcm_is_connect(mtk_crtc))
		cmdq_pkt_wfe(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);

	/* because only mode = 3 (60fps), we need changed to 120fps */
	pr_info("pwm_turbo %s, src_mode=%d\n", __func__, src_mode);
	if (ext->params->dyn_fps.vact_timing_fps == 60) {
		fps = 120;
		if (!en)
			hpwm_fps_mode = !en;
		pr_info("pwm_turbo %s, src_mode=%d,hpwm_fps_mode=%d\n", __func__, src_mode, hpwm_fps_mode);
		if (comp && comp->funcs && comp->funcs->io_cmd)
			comp->funcs->io_cmd(comp, cmdq_handle, DSI_SET_HPWM_FPS, &fps);
		/** wait one TE **/
		cmdq_pkt_clear_event(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
		if (mtk_drm_lcm_is_connect(mtk_crtc))
			cmdq_pkt_wfe(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
        /** wait one TE **/
		cmdq_pkt_clear_event(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
		if (mtk_drm_lcm_is_connect(mtk_crtc))
			cmdq_pkt_wfe(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
		hpwm_fps_mode = en;
	}

	if (comp && comp->funcs && comp->funcs->io_cmd)
		comp->funcs->io_cmd(comp, cmdq_handle, DSI_SET_HPWM, &en);

	/** wait one TE **/
	cmdq_pkt_clear_event(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
	if (mtk_drm_lcm_is_connect(mtk_crtc))
		cmdq_pkt_wfe(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);

	if (comp && comp->funcs && comp->funcs->io_cmd)
		comp->funcs->io_cmd(comp, cmdq_handle, DSI_SET_HPWM_ELVSS, &en);

	/* because only mode = 3 (60fps), we need recovery 60fps */
	if (ext->params->dyn_fps.vact_timing_fps == 60) {
		fps = 60;
		/** wait one TE **/
		cmdq_pkt_clear_event(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
		if (mtk_drm_lcm_is_connect(mtk_crtc))
			cmdq_pkt_wfe(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
		pr_info("pwm_turbo %s, 60fps src_mode=%d, hpwm_fps_mode=%d\n", __func__, src_mode, hpwm_fps_mode);
		if (comp && comp->funcs && comp->funcs->io_cmd)
			comp->funcs->io_cmd(comp, cmdq_handle, DSI_SET_HPWM_FPS, &fps);
	}

	/* because only mode = 0 (120fps), we need set fps close pwm */
	if (ext->params->dyn_fps.vact_timing_fps == 120) {
		fps = 120;
		hpwm_fps_mode = en;
		pr_info("pwm_turbo %s, 120fps src_mode=%d, hpwm_fps_mode=%d\n", __func__, src_mode, hpwm_fps_mode);
		/** wait one TE **/
		cmdq_pkt_clear_event(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
		if (mtk_drm_lcm_is_connect(mtk_crtc))
			cmdq_pkt_wfe(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
		if (comp && comp->funcs && comp->funcs->io_cmd)
			comp->funcs->io_cmd(comp, cmdq_handle, DSI_SET_HPWM_FPS, &fps);
	}

	/* 1pulse swtich, update temp compensation */
	if (pwm_turbo_support() == true) {
		hpwm_mode = en;
		hpwm_onepulse_update_temp = true;
		oplus_temp_compensation_io_cmd_set(comp, cmdq_handle, OPLUS_TEMP_COMPENSATION_TEMPERATURE_SETTING);
		hpwm_onepulse_update_temp = false;
	}

	mtk_drm_send_lcm_cmd_flush(crtc, &cmdq_handle, 0);

	if (en == 0) {
		hpwm_mode_90hz = 0;
	}

done:
	DDP_MUTEX_UNLOCK(&mtk_crtc->lock, __func__, __LINE__);
	mutex_unlock(&g_pwm_turbo_lock);

	return 0;
}

/* -------------------- oplus hidl nodes --------------------------------------- */
int oplus_display_panel_set_pwm_status(void *data)
{
	int rc = 0;
	struct drm_crtc *crtc;
	struct drm_device *ddev = get_drm_device();
	unsigned int *pwm_status = data;
	int prj_id = get_project();

	if (!data) {
		pr_err("%s: set pwm status data is null\n", __func__);
		return -EINVAL;
	}

	printk(KERN_INFO "oplus high pwm mode = %d\n", *pwm_status);

	crtc = list_first_entry(&(ddev)->mode_config.crtc_list,
				typeof(*crtc), head);
	if (!crtc) {
		printk(KERN_ERR "find crtc fail\n");
		return 0;
	}
	if (((prj_id == 22023) || (prj_id == 22223)) && ((hpwm_bl == 0x63F) || (hpwm_bl == 0x58D))) {
		hpwm_mode = 1;
	} else {
		/* 1pusle switch, need update temp parameter */
		if (pwm_turbo_support() == true) {
			oplus_temp_compensation_data_replace(*pwm_status);
			rc = oplus_display_panel_set_pwm_turbo_switch(crtc, *pwm_status);
		} else {
			rc = oplus_display_panel_set_pwm_turbo_switch(crtc, *pwm_status);
			hpwm_mode = (long)*pwm_status;
		}
	}

	return rc;
}

int oplus_display_panel_get_pwm_status(void *buf)
{
	struct drm_crtc *crtc;
	struct mtk_drm_crtc *mtk_crtc;
	struct drm_device *ddev = get_drm_device();
	unsigned int *pwm_status = buf;


	/* this debug cmd only for crtc0 */
	crtc = list_first_entry(&(ddev)->mode_config.crtc_list,
				typeof(*crtc), head);
	if (!crtc) {
		printk(KERN_ERR "find crtc fail\n");
		return -1;
	}
	mtk_crtc = to_mtk_crtc(crtc);
	if (!mtk_crtc || !mtk_crtc->panel_ext || !mtk_crtc->panel_ext->params) {
		pr_err("falied to get lcd proc info\n");
		return -EINVAL;
	}

	pr_info("%s: %s\n", __func__, mtk_crtc->panel_ext->params->vendor);

	mutex_lock(&g_pwm_turbo_lock);
	if (!strcmp(mtk_crtc->panel_ext->params->vendor, "Tianma_NT37705")) {
		*pwm_status = hpwm_mode;
		pr_info("%s: high pwm mode = %d\n", __func__, hpwm_mode);
	} else {
		*pwm_status = 0;
	}
	mutex_unlock(&g_pwm_turbo_lock);

	return 0;
}

int oplus_display_panel_get_pwm_status_for_90hz(void *buf)
{
	struct drm_crtc *crtc;
	struct mtk_drm_crtc *mtk_crtc;
	struct drm_device *ddev = get_drm_device();
	unsigned int *pwm_status = buf;

	/* this debug cmd only for crtc0 */
	crtc = list_first_entry(&(ddev)->mode_config.crtc_list,
				typeof(*crtc), head);
	if (!crtc) {
		printk(KERN_ERR "find crtc fail\n");
		return -1;
	}
	mtk_crtc = to_mtk_crtc(crtc);
	if (!mtk_crtc || !mtk_crtc->panel_ext || !mtk_crtc->panel_ext->params) {
		pr_err("falied to get lcd proc info\n");
		return -EINVAL;
	}

	if (!strcmp(mtk_crtc->panel_ext->params->vendor, "22823_Tianma_NT37705")
		|| !strcmp(mtk_crtc->panel_ext->params->vendor, "Tianma_NT37705")) {
		*pwm_status = hpwm_mode_90hz;
		pr_info("%s: high hpwm_mode_90hz = %d\n", __func__, hpwm_mode_90hz);
	} else {
		*pwm_status = 10;
	}

	return 0;
}

/* -------------------- oplus api nodes ----------------------------------------------- */
ssize_t oplus_display_get_high_pwm(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	printk(KERN_INFO "high pwm mode = %d\n", hpwm_mode);

	return sprintf(buf, "%d\n", hpwm_mode);
}

ssize_t oplus_display_set_high_pwm(struct kobject *kobj,
                struct kobj_attribute *attr, const char *buf, size_t count)
{
	struct drm_crtc *crtc;
	unsigned int temp_save = 0;
	int ret = 0;
	struct drm_device *ddev = get_drm_device();
	int prj_id = get_project();

	ret = kstrtouint(buf, 10, &temp_save);
	printk(KERN_INFO "pwm_turbo mode = %d\n", temp_save);

	crtc = list_first_entry(&(ddev)->mode_config.crtc_list,
				typeof(*crtc), head);
	if (!crtc) {
		printk(KERN_ERR "find crtc fail\n");
		return 0;
	}
	if (((prj_id == 22023) || (prj_id == 22223)) && ((hpwm_bl == 0x63F) || (hpwm_bl == 0x58D))) {
		hpwm_mode = 1;
	} else {
		/* 1pusle switch, need update temp parameter */
		if (pwm_turbo_support() == true) {
			oplus_temp_compensation_data_replace(temp_save);
			oplus_display_panel_set_pwm_turbo_switch(crtc, temp_save);
		} else {
			oplus_display_panel_set_pwm_turbo_switch(crtc, temp_save);
			hpwm_mode = temp_save;
		}
	}

	return count;
}

bool oplus_display_get_demura_support(void)
{
	DDPINFO("get oplus_bl_demura_dbv_support = %d\n", demura_setting.oplus_bl_demura_dbv_support);

	return demura_setting.oplus_bl_demura_dbv_support;
}
EXPORT_SYMBOL(oplus_display_get_demura_support);

bool oplus_display_set_demura_support(bool mode)
{
	pr_info("set oplus_bl_demura_dbv_support = %d\n", mode);
	demura_setting.oplus_bl_demura_dbv_support = mode;
	return demura_setting.oplus_bl_demura_dbv_support;
}
EXPORT_SYMBOL(oplus_display_set_demura_support);

void oplus_display_get_demura_cfg(struct device *dev)
{
	int rc = 0;
	u32 demura_switch_dvb1 = 0;
	u32 demura_switch_dvb2 = 0;

	rc = of_property_read_u32(dev->of_node, "demura_dbv_cfg1", &demura_switch_dvb1);
	if (rc == 0) {
		demura_setting.demura_switch_dvb1 = demura_switch_dvb1;
		pr_info("%s:demura_switch_dvb1=%d\n", __func__, demura_setting.demura_switch_dvb1);
	} else {
		demura_setting.demura_switch_dvb1 = 0;
		pr_info("%s:default demura_switch_dvb1=%d\n", __func__, demura_setting.demura_switch_dvb1);
	}

	rc = of_property_read_u32(dev->of_node, "demura_dbv_cfg2", &demura_switch_dvb2);
	if (rc == 0) {
		demura_setting.demura_switch_dvb2 = demura_switch_dvb2;
		pr_info("%s:demura_switch_dvb2=%d\n", __func__, demura_setting.demura_switch_dvb2);
	} else {
		demura_setting.demura_switch_dvb2 = 0;
		pr_info("%s:default demura_switch_dvb2=%d\n", __func__, demura_setting.demura_switch_dvb2);
	}
	return;
}
EXPORT_SYMBOL(oplus_display_get_demura_cfg);

static void hpwm_cmdq_cb(struct cmdq_cb_data data)
{
	struct mtk_cmdq_cb_data *cb_data = data.data;
	oplus_disp_trace_begin("hpwm_cmdq_cb");
	cmdq_pkt_destroy(cb_data->cmdq_handle);
	kfree(cb_data);
	oplus_disp_trace_end("hpwm_cmdq_cb");
}

int oplus_drm_sethpwm_temp(struct drm_crtc *crtc)
{
	struct mtk_drm_crtc *mtk_crtc = to_mtk_crtc(crtc);
	struct mtk_ddp_comp *comp = mtk_ddp_comp_request_output(mtk_crtc);
	struct mtk_cmdq_cb_data *cb_data;
	struct cmdq_pkt *cmdq_handle;
	struct cmdq_client *client;
	bool is_frame_mode;

	if (!(comp && comp->funcs && comp->funcs->io_cmd))
		return -EINVAL;

	if (!(mtk_crtc->enabled)) {
		DDPINFO("%s: skip, slept\n", __func__);
		return -EINVAL;
	}

	mtk_drm_idlemgr_kick(__func__, crtc, 0);

	cb_data = kmalloc(sizeof(*cb_data), GFP_KERNEL);
	if (!cb_data) {
		DDPPR_ERR("hpwm_temp cb data creation failed\n");
		return -EINVAL;
	}

	DDPINFO("%s: start\n", __func__);

	is_frame_mode = mtk_crtc_is_frame_trigger_mode(&mtk_crtc->base);

	/* send temp cmd would use VM CMD in  DSI VDO mode only */
	client = (is_frame_mode) ? mtk_crtc->gce_obj.client[CLIENT_CFG] :
				mtk_crtc->gce_obj.client[CLIENT_DSI_CFG];
	#ifndef OPLUS_FEATURE_DISPLAY
	cmdq_handle =
		cmdq_pkt_create(client);
	#else
	mtk_crtc_pkt_create(&cmdq_handle, crtc, client);
	#endif

	if (!cmdq_handle) {
		DDPPR_ERR("%s:%d NULL cmdq handle\n", __func__, __LINE__);
		return -EINVAL;
	}

	/** wait one TE **/
	cmdq_pkt_clear_event(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
	if (mtk_drm_lcm_is_connect(mtk_crtc))
		cmdq_pkt_wait_no_clear(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);

	mtk_crtc_wait_frame_done(mtk_crtc, cmdq_handle, DDP_FIRST_PATH, 0);

	if (is_frame_mode) {
		cmdq_pkt_clear_event(cmdq_handle,
				mtk_crtc->gce_obj.event[EVENT_STREAM_BLOCK]);
		cmdq_pkt_wfe(cmdq_handle,
				mtk_crtc->gce_obj.event[EVENT_CABC_EOF]);
	}
	if(mtk_crtc->panel_ext->params->dyn_fps.vact_timing_fps == 90) {
		usleep_range(3000, 3100);
	} else {
		usleep_range(1100, 1200);
	}
	DDPPR_ERR("%s:%d 90nit send temp cmd\n", __func__, __LINE__);
#ifdef OPLUS_FEATURE_DISPLAY_TEMP_COMPENSATION
	if (oplus_temp_compensation_is_supported()) {
		if(mtk_crtc->panel_ext->params->dyn_fps.vact_timing_fps == 60) {
			oplus_temp_compensation_io_cmd_set(comp, cmdq_handle, OPLUS_TEMP_COMPENSATION_FIRST_HALF_FRAME_SETTING);
		} else {
			oplus_temp_compensation_io_cmd_set(comp, cmdq_handle, OPLUS_TEMP_COMPENSATION_BACKLIGHT_SETTING);
		}
	}
#endif /* OPLUS_FEATURE_DISPLAY_TEMP_COMPENSATION */

	if (is_frame_mode) {
		cmdq_pkt_set_event(cmdq_handle,
				mtk_crtc->gce_obj.event[EVENT_CABC_EOF]);
		cmdq_pkt_set_event(cmdq_handle,
				mtk_crtc->gce_obj.event[EVENT_STREAM_BLOCK]);
	}

	cb_data->crtc = crtc;
	cb_data->cmdq_handle = cmdq_handle;

	if (cmdq_pkt_flush_threaded(cmdq_handle, hpwm_cmdq_cb, cb_data) < 0) {
		DDPPR_ERR("failed to flush hpwm_cmdq_cb\n");
		return -EINVAL;
	}

	DDPINFO("%s: end\n", __func__);

	return 0;
}

void oplus_panel_backlight_demura_dbv_switch(struct drm_crtc *crtc, struct cmdq_pkt *cmdq_handle, unsigned int level)
{
	struct mtk_drm_crtc *mtk_crtc = to_mtk_crtc(crtc);
	struct mtk_ddp_comp *comp = mtk_ddp_comp_request_output(mtk_crtc);
	int bl_demura_last_mode = demura_setting.bl_demura_mode;
	int bl_demura_mode = OPLUS_DEMURA_DBV_MODE_MAX;

	if (!comp) {
		DDPPR_ERR("%s:%d NULL comp\n", __func__, __LINE__);
		return;
	}

	if (comp->id == DDP_COMPONENT_DSI0) {
		if ((level != 1) && (level <= demura_setting.demura_switch_dvb1)) {
			demura_setting.bl_demura_mode = OPLUS_DEMURA_DBV_MODE0;
		} else if ((level > demura_setting.demura_switch_dvb1) && (level <= demura_setting.demura_switch_dvb2)) {
			demura_setting.bl_demura_mode = OPLUS_DEMURA_DBV_MODE1;
		} else if (level > demura_setting.demura_switch_dvb2) {
			demura_setting.bl_demura_mode = OPLUS_DEMURA_DBV_MODE2;
		}
		bl_demura_mode = demura_setting.bl_demura_mode;
		DDPINFO("level:%d,hpwm_mode:%d,bl_demura_mode:%d,pwm_power_on:%d,bl_demura_last_mode:%d\n", level, hpwm_mode, bl_demura_mode,
                        pwm_power_on, bl_demura_last_mode);
		if ((pwm_power_on == true) || (bl_demura_mode != bl_demura_last_mode)) {
			if (comp->funcs && comp->funcs->io_cmd)
				comp->funcs->io_cmd(comp, cmdq_handle, DSI_SET_DEMURA_BL, &bl_demura_mode);
			if (!hpwm_mode)
				pwm_power_on = false;
		}
	}

	return;
}
EXPORT_SYMBOL(oplus_panel_backlight_demura_dbv_switch);

int oplus_drm_sethpwm_bl(struct drm_crtc *crtc, struct cmdq_pkt *cmdq_handle, unsigned int level, bool is_sync)
{
	struct mtk_drm_crtc *mtk_crtc = to_mtk_crtc(crtc);
	struct mtk_ddp_comp *comp = mtk_ddp_comp_request_output(mtk_crtc);

	hpwm_90nit_set_temp = 0;
	oplus_display_brightness = level;
	DDPINFO("oplus_display_brightness:%u\n", oplus_display_brightness);

#ifdef OPLUS_FEATURE_DISPLAY_ONSCREENFINGERPRINT
	if (oplus_ofp_is_support()) {
		if (oplus_ofp_backlight_filter(level)) {
			goto end;
		}
	}
#endif /* OPLUS_FEATURE_DISPLAY_ONSCREENFINGERPRINT */

	if (oplus_display_get_demura_support()) {
		oplus_panel_backlight_demura_dbv_switch(crtc, cmdq_handle, level);
	}

	if (hpwm_mode) {
		DDPINFO("pwm_turbo: hpwm_90nit_set_temp:%d,vact_timing_fps:%d,pwm_turbo:%d,pwm_power_on:%d,is_sync:%d last_backlight=%d\n", hpwm_90nit_set_temp,
			mtk_crtc->panel_ext->params->dyn_fps.vact_timing_fps, hpwm_mode, pwm_power_on, is_sync, last_backlight);
		if (mtk_crtc->panel_ext->params->oplus_hpwm_config) {
			if ((level != 1) &&  (level != 0) && (((level <= hpwm_bl) && (last_backlight > hpwm_bl)) || (pwm_power_on == true && level <= hpwm_bl))) {
				if ((mtk_crtc->panel_ext->params->dyn_fps.vact_timing_fps == 60) &&  (is_sync == false)) {
					/** wait one TE **/
					cmdq_pkt_clear_event(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
					if (mtk_drm_lcm_is_connect(mtk_crtc))
						cmdq_pkt_wfe(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
				}
				DDPINFO("%s DSI_SET_BL sleep 0ms.<hpwm_bl\n", __func__);
				if (comp->funcs && comp->funcs->io_cmd)
					comp->funcs->io_cmd(comp, cmdq_handle, DSI_SET_HPWM_PLUSS_BL, &level);
				hpwm_90nit_set_temp = 1;
				pwm_power_on = false;
				last_backlight = level;
			} else if (((level > hpwm_bl) && (last_backlight <= hpwm_bl)) || (pwm_power_on == true && level >  hpwm_bl)) {
				if ((mtk_crtc->panel_ext->params->dyn_fps.vact_timing_fps == 60) &&  (is_sync == false)) {
					/** wait one TE **/
					cmdq_pkt_clear_event(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
					if (mtk_drm_lcm_is_connect(mtk_crtc))
						cmdq_pkt_wfe(cmdq_handle, mtk_crtc->gce_obj.event[EVENT_TE]);
				}
				DDPINFO("%s DSI_SET_BL sleep 0ms.>hpwm_bl\n", __func__);
				if (comp->funcs && comp->funcs->io_cmd)
					comp->funcs->io_cmd(comp, cmdq_handle, DSI_SET_HPWM_PLUSS_BL, &level);
				hpwm_90nit_set_temp = 2;
				pwm_power_on = false;
				last_backlight =  level;
			}

			if (hpwm_90nit_set_temp == 0) {
				if (comp->funcs && comp->funcs->io_cmd)
					comp->funcs->io_cmd(comp, cmdq_handle, DSI_SET_BL, &level);
			}
			if ((hpwm_90nit_set_temp > 0)  &&  (is_sync == false)) {
				if (mtk_crtc->panel_ext->params->dyn_fps.vact_timing_fps == 60) {
					cmdq_pkt_sleep(cmdq_handle, CMDQ_US_TO_TICK(2500), mtk_get_gpr(comp, cmdq_handle));
					DDPINFO("%s bl not sync mipi cmdq sleep 2.5ms \n", __func__);
#ifdef OPLUS_FEATURE_DISPLAY_TEMP_COMPENSATION
					if (oplus_temp_compensation_is_supported()) {
						oplus_temp_compensation_io_cmd_set(comp, cmdq_handle, OPLUS_TEMP_COMPENSATION_FIRST_HALF_FRAME_SETTING);
					}
#endif /* OPLUS_FEATURE_DISPLAY_TEMP_COMPENSATION */
				}
				hpwm_90nit_set_temp = 0;
			}
		} else {
			DDPINFO("%s DSI_SET_BL not hpwm_supported panel\n", __func__);
			if (comp->funcs && comp->funcs->io_cmd)
				comp->funcs->io_cmd(comp, cmdq_handle, DSI_SET_BL, &level);
		}
	} else {
		if (comp->funcs && comp->funcs->io_cmd)
				comp->funcs->io_cmd(comp, cmdq_handle, DSI_SET_BL, &level);
		last_backlight =  level;
	}

end:
#ifdef OPLUS_FEATURE_DISPLAY_TEMP_COMPENSATION
	if (oplus_temp_compensation_is_supported()) {
		if (!((hpwm_90nit_set_temp > 0) && (is_sync == true))) {
			oplus_temp_compensation_io_cmd_set(comp, cmdq_handle, OPLUS_TEMP_COMPENSATION_BACKLIGHT_SETTING);
		}
	}
#endif /* OPLUS_FEATURE_DISPLAY_TEMP_COMPENSATION */

	return 0;
}
