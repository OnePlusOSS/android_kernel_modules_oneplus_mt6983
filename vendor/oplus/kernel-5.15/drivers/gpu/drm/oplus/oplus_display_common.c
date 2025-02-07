/***************************************************************
** Copyright (C),  2020,  OPLUS Mobile Comm Corp.,  Ltd
** File : oplus_display_dc.c
** Description : oplus dc feature
** Version : 1.0
** Date : 2020/07/1
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**  JianBin.Zhang   2020/07/01        1.0           Build this moudle
**  Xiaolei.Gao     2021/08/14        1.1           Build this moudle
***************************************************************/
#include <oplus_display_common.h>
#include "oplus_display_panel.h"

#ifdef OPLUS_FEATURE_DISPLAY_ONSCREENFINGERPRINT
#include "oplus_display_onscreenfingerprint.h"
#endif /* OPLUS_FEATURE_DISPLAY_ONSCREENFINGERPRINT */
#include "../mediatek/mediatek_v2/mtk_disp_bdg.h"

#define PANEL_SERIAL_NUM_REG 0xA1
#define PANEL_REG_READ_LEN   10
#define BOE_PANEL_SERIAL_NUM_REG 0xA3
#define PANEL_SERIAL_NUM_REG_TIANMA 0xD6
#define SILKY_MAX_NORMAL_BRIGHTNESS 8191

extern unsigned int cabc_mode;
extern unsigned int cabc_true_mode;
extern unsigned int cabc_sun_flag;
extern unsigned int cabc_back_flag;
extern void disp_aal_set_dre_en(int enable);
extern unsigned int silence_mode;
extern unsigned int oplus_display_brightness;
extern unsigned int oplus_max_normal_brightness;
extern uint64_t serial_number;
extern unsigned int esd_mode;
extern unsigned int seed_mode;
extern unsigned int m_da;
extern unsigned int m_db;
extern unsigned int m_dc;
extern bool g_dp_support;
extern bool pq_trigger;
extern unsigned int hpwm_mode;
extern unsigned int hpwm_mode_90hz;
extern unsigned int get_project(void);
extern char lcm_version[32];
extern char lcm_manufacture[32];
extern char lcm_version_ext[32];
extern char lcm_manufacture_ext[32];
extern bool oplus_hbm_max_en;
DEFINE_MUTEX(oplus_seed_lock);
DEFINE_MUTEX(g_oplus_hbm_max_lock);
DEFINE_MUTEX(g_oplus_hbm_max_switch_lock);

extern struct drm_device* get_drm_device(void);
extern int mtk_drm_setbacklight(struct drm_crtc *crtc, unsigned int level, unsigned int panel_ext_param, unsigned int cfg_flag);

extern int panel_serial_number_read(struct drm_crtc *crtc, char cmd, int num);
extern int oplus_mtk_drm_setcabc(struct drm_crtc *crtc, unsigned int hbm_mode);
extern int oplus_mtk_drm_setseed(struct drm_crtc *crtc, unsigned int seed_mode);

static unsigned int cabc_mode_last = 1;

enum {
	CABC_LEVEL_0,
	CABC_LEVEL_1,
	CABC_LEVEL_2 = 3,
	CABC_EXIT_SPECIAL = 8,
	CABC_ENTER_SPECIAL = 9,
};

unsigned int oplus_display_panel_max_brightness = 4094;
EXPORT_SYMBOL(oplus_display_panel_max_brightness);

int oplus_display_set_brightness(void *buf)
{
	struct drm_crtc *crtc;
	struct drm_device *ddev = get_drm_device();
	unsigned int *set_brightness = buf;
	unsigned int oplus_set_brightness = (*set_brightness);

	printk("%s %d\n", __func__, oplus_set_brightness);

	if (oplus_set_brightness > OPLUS_MAX_BRIGHTNESS) {
		printk(KERN_ERR "%s, brightness:%d out of scope\n", __func__, oplus_set_brightness);
		return -1;
	}

	/* this debug cmd only for crtc0 */
	crtc = list_first_entry(&(ddev)->mode_config.crtc_list,
				typeof(*crtc), head);
	if (!crtc) {
		printk(KERN_ERR "find crtc fail\n");
		return -1;
	}
	mtk_drm_setbacklight(crtc, oplus_set_brightness, 0, 0x1 << SET_BACKLIGHT_LEVEL);

	return 0;
}

int oplus_display_get_brightness(void *buf)
{
	unsigned int *brightness = buf;

	(*brightness) = oplus_display_brightness;

	return 0;
}

int oplus_display_panel_get_max_brightness(void *buf)
{
	unsigned int *brightness = buf;

	if (m_new_pq_persist_property[DISP_PQ_CCORR_SILKY_BRIGHTNESS]) {
		(*brightness) = SILKY_MAX_NORMAL_BRIGHTNESS;
	} else {
		(*brightness) = oplus_max_normal_brightness;
	}

	return 0;
}

int oplus_display_panel_get_max_dbv(void *data)
{
	unsigned int *dbv = (unsigned int*)data;
	*dbv = oplus_display_panel_max_brightness;
	pr_info("%s get max dbv: %d\n", __func__, oplus_display_panel_max_brightness);
	return 0;
}

int oplus_display_panel_dbv_probe(struct device *dev)
{
	u32 config = 0;
	int rc = 0;

	if (IS_ERR_OR_NULL(dev)) {
		pr_err("%s Invalid params\n", __func__);
		return -EINVAL;
	}

	rc = of_property_read_u32(dev->of_node, "oplus,panel-max-brightness", &config);
	if (rc == 0) {
		oplus_display_panel_max_brightness = config;
	} else {
		oplus_display_panel_max_brightness = 4095;
	}
	pr_info("%s config=%d, oplus_display_panel_max_brightness=%d\n",
		__func__, config, oplus_display_panel_max_brightness);
	return 0;
}
EXPORT_SYMBOL(oplus_display_panel_dbv_probe);

int oplus_display_panel_set_hbm_max_switch(struct drm_crtc *crtc, unsigned int en)
{
	struct mtk_drm_crtc *mtk_crtc = to_mtk_crtc(crtc);
	struct mtk_ddp_comp *comp;
	struct cmdq_pkt *cmdq_handle;


	pr_info("%s, en=%d\n", __func__, en);
	mutex_lock(&g_oplus_hbm_max_switch_lock);

	if (!mtk_crtc->enabled) {
		pr_err("%s crtc not enabled\n", __func__);
		goto done;
	}

	comp = mtk_ddp_comp_request_output(mtk_crtc);
	if (!comp) {
		pr_err("%s, request output fail\n", __func__);
		goto done;
	}

	mtk_drm_send_lcm_cmd_prepare(crtc, &cmdq_handle);
	mtk_ddp_comp_io_cmd(comp, cmdq_handle, DSI_SET_HBM_MAX, &en);
	mtk_drm_send_lcm_cmd_flush(crtc, &cmdq_handle, 0);

done:
	mutex_unlock(&g_oplus_hbm_max_switch_lock);

	return 0;
}

int oplus_display_panel_set_hbm_max(void *data)
{
	struct drm_crtc *crtc;
	struct drm_device *ddev = get_drm_device();
	unsigned int enabled = *((unsigned int*)data);

	crtc = list_first_entry(&(ddev)->mode_config.crtc_list,
				typeof(*crtc), head);
	if (!crtc) {
		pr_err("find crtc fail\n");
		return -EINVAL;
	}

	pr_info("%s set hbm_max status: %d\n", __func__, enabled);
	if ((bool)enabled == oplus_hbm_max_en) {
		pr_info("%s skip setting duplicate hbm_apl status: %d\n", __func__, enabled);
		return -EINVAL;
	}

	oplus_hbm_max_en = (bool)enabled;
	oplus_display_panel_set_hbm_max_switch(crtc, enabled);

	return 0;
}
EXPORT_SYMBOL(oplus_display_panel_set_hbm_max);

int oplus_display_panel_get_hbm_max(void *data)
{
	bool *enabled = (bool*)data;

	mutex_lock(&g_oplus_hbm_max_lock);
	*enabled = oplus_hbm_max_en;
	mutex_unlock(&g_oplus_hbm_max_lock);
	pr_info("%s get hbm_apl status: %d\n", __func__, *enabled);

	return 0;
}
EXPORT_SYMBOL(oplus_display_panel_get_hbm_max);

int oplus_display_panel_get_panel_bpp(void *buf)
{
	unsigned int *panel_bpp = buf;
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

	(*panel_bpp) = mtk_crtc->panel_ext->params->panel_bpp;
	printk("%s panel_bpp : %d\n", __func__, *panel_bpp);

	return 0;
}

int oplus_display_panel_get_serial_number(void *buf)
{
	struct panel_serial_number *p_snumber = buf;
	int ret = 0;
	struct drm_crtc *crtc;
	struct drm_device *ddev = get_drm_device();
	/* this debug cmd only for crtc0 */
	crtc = list_first_entry(&(ddev)->mode_config.crtc_list,
		typeof(*crtc), head);
	if (!crtc) {
		DDPPR_ERR("get_panel_serial_number find crtc fail\n");
		return 0;
	}

	if (oplus_display_brightness == 0) {
		pr_info("backlight is 0, skip get serial number!\n");
		return 0;
	}

	/* LCD panel need not show serial_number in *#808# */
	if (is_bdg_supported() == true) {
		return 0;
	}

	if (serial_number == 0) {
		panel_serial_number_read(crtc, PANEL_SERIAL_NUM_REG, PANEL_REG_READ_LEN);
		pr_info("%s read, serial_number: %llx, da=0x%llx, db=0x%llx, dc=0x%llx\n", __func__, serial_number, m_da, m_db, m_dc);
	}

	printk("%s read serial number 0x%x\n", __func__, serial_number);
	ret = scnprintf(p_snumber->serial_number, sizeof(p_snumber->serial_number)+1, "Get panel serial number: %llx\n", serial_number);
	return ret;
}

int oplus_display_panel_get_cabc(void *buf)
{
	unsigned int *c_mode = buf;

	printk("%s CABC_mode=%d\n", __func__, cabc_true_mode);
	*c_mode = cabc_true_mode;

	return 0;
}
# if 0
int oplus_display_panel_set_cabc(void *buf)
{
	struct drm_crtc *crtc;
	struct drm_device *ddev = get_drm_device();
	unsigned int *c_mode = buf;
	cabc_mode = (unsigned int)(*c_mode);

	cabc_true_mode = cabc_mode;
	printk("%s,cabc mode is %d, cabc_back_flag is %d\n", __func__, cabc_mode, cabc_back_flag);
	if (cabc_mode < 4) {
		cabc_back_flag = cabc_mode;
	}

	if (cabc_mode == CABC_ENTER_SPECIAL) {
		cabc_sun_flag = 1;
		cabc_true_mode = 0;
	} else if (cabc_mode == CABC_EXIT_SPECIAL) {
		cabc_sun_flag = 0;
		cabc_true_mode = cabc_back_flag;
	} else if (cabc_sun_flag == 1) {
		if (cabc_back_flag == CABC_LEVEL_0) {
			disp_aal_set_dre_en(1);
			printk("%s sun enable dre\n", __func__);
		} else {
			disp_aal_set_dre_en(0);
			printk("%s sun disable dre\n", __func__);
		}
		return 0;
	}

	printk("%s,cabc mode is %d\n", __func__, cabc_true_mode);

	/* this debug cmd only for crtc0 */
	crtc = list_first_entry(&(ddev)->mode_config.crtc_list,
				typeof(*crtc), head);
	if (!crtc) {
		printk(KERN_ERR "find crtc fail\n");
		return -1;
	}
	if (cabc_true_mode == CABC_LEVEL_0 && cabc_back_flag == CABC_LEVEL_0) {
		disp_aal_set_dre_en(1);
		printk("%s enable dre\n", __func__);
	} else {
		disp_aal_set_dre_en(0);
		printk("%s disable dre\n", __func__);
	}
	/*oplus_mtk_drm_setcabc(crtc, cabc_true_mode);*/
	if (cabc_true_mode != cabc_back_flag) {
		cabc_true_mode = cabc_back_flag;
	}

	return 0;
}
#endif
int oplus_display_panel_set_cabc(void *buf)
{
	struct drm_crtc *crtc;
	struct drm_device *ddev = get_drm_device();
	struct mtk_drm_crtc *mtk_crtc;
	uint32_t *cabc_mode_temp = buf;
	cabc_mode = *cabc_mode_temp;
	cabc_true_mode = cabc_mode;

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

	printk("%s,cabc mode is %d, cabc_back_flag is %d, oplus_display_global_dre = %d\n", __func__,
		cabc_mode, cabc_back_flag, mtk_crtc->panel_ext->params->oplus_display_global_dre);
	cabc_mode_last = cabc_back_flag;
	if (cabc_mode < 4) {
		cabc_back_flag = cabc_mode;
	}

	if (cabc_mode == CABC_ENTER_SPECIAL) {
		cabc_sun_flag = 1;
		cabc_true_mode = 0;
	} else if (cabc_mode == CABC_EXIT_SPECIAL) {
		cabc_sun_flag = 0;
		cabc_true_mode = cabc_back_flag;
	} else if (cabc_sun_flag == 1) {
		if (cabc_back_flag == CABC_LEVEL_0 || mtk_crtc->panel_ext->params->oplus_display_global_dre) {
			disp_aal_set_dre_en(1);
			printk("%s sun enable dre\n", __func__);
		} else {
			disp_aal_set_dre_en(0);
			printk("%s sun disable dre\n", __func__);
		}
		return 0;
	}

	printk("%s,cabc mode is %d\n", __func__, cabc_true_mode);

	if (!crtc) {
		printk(KERN_ERR "find crtc fail\n");
		return -1;
	}
	if ((cabc_true_mode == CABC_LEVEL_0 && cabc_back_flag == CABC_LEVEL_0) || mtk_crtc->panel_ext->params->oplus_display_global_dre) {
		disp_aal_set_dre_en(1);
		printk("%s enable dre\n", __func__);
	} else {
		disp_aal_set_dre_en(0);
		printk("%s disable dre\n", __func__);
	}

	/* Fix the flicker when cabc three to zero */
	if (cabc_true_mode == 0 && cabc_mode_last == 3 && mtk_crtc->panel_ext->params->cabc_three_to_zero) {
		oplus_mtk_drm_setcabc(crtc, 2);
		oplus_mtk_drm_setcabc(crtc, 1);
		oplus_mtk_drm_setcabc(crtc, 0);
	} else {
		oplus_mtk_drm_setcabc(crtc, cabc_true_mode);
	}

	if (cabc_true_mode != cabc_back_flag) {
		cabc_true_mode = cabc_back_flag;
	}
	return 0;
}



int oplus_display_panel_get_closebl_flag(void *buf)
{
	unsigned int *closebl_flag = buf;

	printk("%s silence_mode=%d\n", __func__, silence_mode);
	(*closebl_flag) = silence_mode;

	return 0;
}

int oplus_display_panel_set_closebl_flag(void *buf)
{
	unsigned int *closebl_flag = buf;

	msleep(1000);
	silence_mode = (*closebl_flag);
	printk("%s silence_mode=%d\n", __func__, silence_mode);

	return 0;
}

int oplus_display_panel_get_esd(void *buf)
{
	unsigned int *p_esd = buf;

	printk("%s esd=%d\n", __func__, esd_mode);
	(*p_esd) = esd_mode;

	return 0;
}

int oplus_display_panel_set_esd(void *buf)
{
	unsigned int *p_esd = buf;

	esd_mode = (*p_esd);
	printk("%s,esd mode is %d\n", __func__, esd_mode);

	return 0;
}

int oplus_display_panel_get_vendor(void *buf)
{
	struct drm_crtc *crtc;
	struct mtk_drm_crtc *mtk_crtc;
	struct panel_info *p_info = buf;
	struct drm_device *ddev = get_drm_device();
	int panel_id = p_info->version[0];
	/* this debug cmd only for crtc0 */
	crtc = list_first_entry(&(ddev)->mode_config.crtc_list,
				typeof(*crtc), head);
	if (!crtc) {
		printk(KERN_ERR "find crtc fail,p_info=%p\n", p_info);
		return -1;
	}

	mtk_crtc = to_mtk_crtc(crtc);
	if (!mtk_crtc || !mtk_crtc->panel_ext || !mtk_crtc->panel_ext->params) {
		pr_err("falied to get lcd proc info\n");
		return -EINVAL;
	}
	if (1 == panel_id) {
		pr_info("get lcd_ext proc info: lcm_version_ext[%s] lcm_manufacture_ext[%s]\n", lcm_version_ext, lcm_manufacture_ext);
		if (!strcmp(lcm_version_ext, "")) {
			memcpy(p_info->version, mtk_crtc->panel_ext->params->vendor,
				   sizeof(mtk_crtc->panel_ext->params->vendor) >= 31?31:(sizeof(mtk_crtc->panel_ext->params->vendor)+1));
		} else {
			memcpy(p_info->version, lcm_version_ext,
				sizeof(lcm_version_ext) >= 31?31:(sizeof(lcm_version_ext)+1));
		}

		if (!strcmp(lcm_manufacture_ext, "")) {
			memcpy(p_info->manufacture, mtk_crtc->panel_ext->params->manufacture,
				   sizeof(mtk_crtc->panel_ext->params->manufacture) >= 31?31:(sizeof(mtk_crtc->panel_ext->params->manufacture)+1));
		} else {
			memcpy(p_info->manufacture, lcm_manufacture_ext,
				   sizeof(lcm_manufacture_ext) >= 31?31:(sizeof(lcm_manufacture_ext)+1));
		}
	} else {
		pr_info("get lcd proc info: vendor[%s] lcm_version[%s] lcm_manufacture[%s]\n", mtk_crtc->panel_ext->params->vendor,
				lcm_version, lcm_manufacture);
		if (!strcmp(lcm_version, "")) {
			memcpy(p_info->version, mtk_crtc->panel_ext->params->vendor,
				   sizeof(mtk_crtc->panel_ext->params->vendor) >= 31?31:(sizeof(mtk_crtc->panel_ext->params->vendor)+1));
		} else {
			memcpy(p_info->version, lcm_version,
				sizeof(lcm_version) >= 31?31:(sizeof(lcm_version)+1));
		}

		if (!strcmp(lcm_manufacture, "")) {
			memcpy(p_info->manufacture, mtk_crtc->panel_ext->params->manufacture,
				   sizeof(mtk_crtc->panel_ext->params->manufacture) >= 31?31:(sizeof(mtk_crtc->panel_ext->params->manufacture)+1));
		} else {
			memcpy(p_info->manufacture, lcm_manufacture,
				   sizeof(lcm_manufacture) >= 31?31:(sizeof(lcm_manufacture)+1));
		}
	}

	return 0;
}
int oplus_display_get_softiris_color_status(void *buf)
{
	struct drm_crtc *crtc;
	struct mtk_drm_crtc *mtk_crtc;
	struct drm_device *ddev = get_drm_device();
	struct softiris_color *iris_color_status = buf;

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

	iris_color_status->color_vivid_status = mtk_crtc->panel_ext->params->color_vivid_status;
	iris_color_status->color_srgb_status = mtk_crtc->panel_ext->params->color_srgb_status;
	iris_color_status->color_softiris_status = mtk_crtc->panel_ext->params->color_softiris_status;
	iris_color_status->color_dual_panel_status = mtk_crtc->panel_ext->params->color_dual_panel_status;
	iris_color_status->color_dual_brightness_status = mtk_crtc->panel_ext->params->color_dual_brightness_status;
	iris_color_status->color_oplus_calibrate_status = mtk_crtc->panel_ext->params->color_oplus_calibrate_status;
	iris_color_status->color_samsung_status = mtk_crtc->panel_ext->params->color_samsung_status;
	iris_color_status->color_loading_status = mtk_crtc->panel_ext->params->color_loading_status;
	iris_color_status->color_2nit_status = mtk_crtc->panel_ext->params->color_2nit_status;
	pr_err("oplus_color_vivid_status: %s", iris_color_status->color_vivid_status ? "true" : "false");
	pr_err("oplus_color_srgb_status: %s", iris_color_status->color_srgb_status ? "true" : "false");
	pr_err("oplus_color_softiris_status: %s", iris_color_status->color_softiris_status ? "true" : "false");
	pr_err("color_dual_panel_status: %s", iris_color_status->color_dual_panel_status ? "true" : "false");
	pr_err("color_dual_brightness_status: %s", iris_color_status->color_dual_brightness_status ? "true" : "false");
	pr_err("color_samsung_status: %s", iris_color_status->color_samsung_status ? "true" : "false");
	pr_err("color_loading_status: %s", iris_color_status->color_loading_status ? "true" : "false");
	pr_err("color_2nit_status: %s", iris_color_status->color_2nit_status ? "true" : "false");
	return 0;
}

int oplus_display_panel_get_seed(void *buf)
{
	unsigned int *seed = buf;

	mutex_lock(&oplus_seed_lock);
	printk("%s seed_mode=%d\n", __func__, seed_mode);
	(*seed) = seed_mode;
	mutex_unlock(&oplus_seed_lock);

	return 0;
}

int oplus_display_panel_set_seed(void *buf)
{
	struct drm_crtc *crtc;
	struct drm_device *ddev = get_drm_device();
	unsigned int *seed_mode_tmp = buf;

	printk("%s, %d to be %d\n", __func__, seed_mode, *seed_mode_tmp);

	/* this debug cmd only for crtc0 */
	crtc = list_first_entry(&(ddev)->mode_config.crtc_list,
				typeof(*crtc), head);
	if (!crtc) {
		printk(KERN_ERR "find crtc fail\n");
		return -1;
	}
	mutex_lock(&oplus_seed_lock);
	oplus_mtk_drm_setseed(crtc, *seed_mode_tmp);
	seed_mode = (*seed_mode_tmp);
	mutex_unlock(&oplus_seed_lock);

	return 0;
}

int oplus_display_panel_get_id(void *buf)
{
	struct panel_id *panel_rid = buf;

	pr_err("%s: 0xDA= 0x%x, 0xDB=0x%x, 0xDC=0x%x\n", __func__, m_da, m_db, m_dc);

	panel_rid->DA = (uint32_t)m_da;
	panel_rid->DB = (uint32_t)m_db;
	panel_rid->DC = (uint32_t)m_dc;

	return 0;
}

int oplus_display_get_dp_support(void *buf)
{
	uint32_t *dp_support = buf;

	pr_info("%s: dp_support = %s\n", __func__, g_dp_support ? "true" : "false");

	*dp_support = g_dp_support;

	return 0;
}
int oplus_display_panel_get_pq_trigger(void *buf)
{
        unsigned int *pq_trigger_flag = buf;

        printk("%s pq_trigger=%d\n", __func__, pq_trigger);
        (*pq_trigger_flag) = pq_trigger;

        return 0;
}

int oplus_display_panel_set_pq_trigger(void *buf)
{
        unsigned int *pq_trigger_flag = buf;

        pq_trigger = (*pq_trigger_flag);
        printk("%s pq_trigger=%d\n", __func__, pq_trigger);

        return 0;
}

int g_need_read_reg = 0;
void oplus_te_check(struct mtk_drm_crtc *mtk_crtc, unsigned long long te_time_diff)
{
	struct drm_crtc *crtc = NULL;
	static int refresh_rate;
	static int vsync_period;
	static int last_refresh_rate = 0;
	static int count = 0;
	static int entry_count = 0;
	static int need_te_check = 0;

	if (!mtk_crtc) {
		DDPPR_ERR("oplus_te_check mtk_crtc is null");
		return;
	}

	crtc = &mtk_crtc->base;

	if (oplus_display_brightness < 1) {
		return;
	}

	if (mtk_crtc->panel_ext && mtk_crtc->panel_ext->params
		&& mtk_crtc->panel_ext->params->dyn_fps.vact_timing_fps != 0)
		refresh_rate = mtk_crtc->panel_ext->params->dyn_fps.vact_timing_fps;

	if (refresh_rate != last_refresh_rate &&  crtc->state->mode.hskew != 2) {
		/* pr_err("need_te_check, refresh_rate = %d, last_refresh_rate = %d\n", refresh_rate, last_refresh_rate); */
		need_te_check = 1;
		count = 0;
		vsync_period = 1000000 / refresh_rate;
	}
	last_refresh_rate = refresh_rate;

	if (need_te_check == 0) {
		return;
	}

	/* pr_err("refresh_rate = %d, vsync_period = %d, te_time_diff = %lu\n", refresh_rate, vsync_period, te_time_diff); */
	if (abs(te_time_diff / 1000 - vsync_period) > vsync_period / 5) { /* The error is more than 20% */
		count++;
		if (count > 10) { /* TE error for 10 consecutive frames */
			DDPPR_ERR("oplus_te_check failed, refresh_rate=%d, te_time_diff=%llu\n", refresh_rate, te_time_diff);
			g_need_read_reg = 1;
			need_te_check = 0;
			count = 0;
		}
	} else {
		entry_count++;
		if (entry_count > 100) { /* 100 consecutive frames is no problem, clear the count */
			entry_count = 0;
			count = 0;
			need_te_check = 0;
		}
	}
}

int oplus_display_panel_get_panel_type(void *buf)
{
	struct drm_crtc *crtc;
	struct mtk_drm_crtc *mtk_crtc;
	struct drm_device *ddev = get_drm_device();
	unsigned int *panel_type = buf;

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

	*panel_type = mtk_crtc->panel_ext->params->panel_type;
	pr_info("%s: panel_type = %d\n", __func__, *panel_type);

	return 0;
}

MODULE_AUTHOR("Xiaolei Gao");
MODULE_DESCRIPTION("OPLUS common device");
MODULE_LICENSE("GPL v2");
