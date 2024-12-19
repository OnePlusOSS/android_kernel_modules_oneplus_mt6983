/***************************************************************
 ** Copyright (C),  2023,  OPLUS Mobile Comm Corp.,  Ltd
 ** File : oplus_display_trace.h
 ** Description : Oplus display trace header
 ** Version : 1.0
 ** Date : 2023/01/10
 **
 ** ------------------------------- Revision History: -----------
 **  <author>        <data>        <version >        <desc>
 **  liuyang        2023/01/10        1.0         Build this moudle
 ******************************************************************/
#ifndef _OPLUS_DISPLAY_TRACE_H_
#define _OPLUS_DISPLAY_TRACE_H_

#include "mtk_drm_ddp_comp.h"

extern bool g_trace_log;
extern bool g_trace_log_lv2;
#define oplus_disp_trace_begin(fmt, args...) do { \
	if (g_trace_log) { \
		mtk_drm_print_trace("B|%d|"fmt"\n", current->tgid, ##args); \
	} \
} while (0)

#define oplus_disp_trace_end(fmt, args...) do { \
	if (g_trace_log) { \
		mtk_drm_print_trace("E|%d|"fmt"\n", current->tgid, ##args); \
	} \
} while (0)

#define oplus_disp_trace_c(fmt, args...) do { \
	if (g_trace_log) { \
		mtk_drm_print_trace("C|"fmt"\n", ##args); \
	} \
} while (0)

extern void mtk_drm_print_trace(char *fmt, ...);
#endif /* _OPLUS_DISPLAY_TRACE_H_ */

