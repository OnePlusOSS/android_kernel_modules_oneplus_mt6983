/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __WF_PSE_TOP_REGS_H__
#define __WF_PSE_TOP_REGS_H__

#ifdef __cplusplus
extern "C" {
#endif


/* ************************************************************************** */
/* */
/* WF_PSE_TOP CR Definitions */
/* */
/* ************************************************************************** */

#define WF_PSE_TOP_BASE                                        0x820C8000

#define WF_PSE_TOP_GC_ADDR \
	(WF_PSE_TOP_BASE + 0x00) /* 8000 */
#define WF_PSE_TOP_PBUF_CTRL_ADDR \
	(WF_PSE_TOP_BASE + 0x04) /* 8004 */
#define WF_PSE_TOP_INT_N9_EN_MASK_ADDR \
	(WF_PSE_TOP_BASE + 0x08) /* 8008 */
#define WF_PSE_TOP_INT_N9_ERR_MASK_ADDR \
	(WF_PSE_TOP_BASE + 0x24) /* 8024 */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR \
	(WF_PSE_TOP_BASE + 0x28) /* 8028 */
#define WF_PSE_TOP_INT_N9_STS_ADDR \
	(WF_PSE_TOP_BASE + 0x30) /* 8030 */
#define WF_PSE_TOP_INT_N9_ERR_STS_ADDR \
	(WF_PSE_TOP_BASE + 0x34) /* 8034 */
#define WF_PSE_TOP_INT_N9_ERR1_STS_ADDR \
	(WF_PSE_TOP_BASE + 0x38) /* 8038 */
#define WF_PSE_TOP_QUEUE_EMPTY_ADDR \
	(WF_PSE_TOP_BASE + 0xB0) /* 80B0 */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR \
	(WF_PSE_TOP_BASE + 0xB4) /* 80B4 */
#define WF_PSE_TOP_PSE_LP_CTRL_ADDR \
	(WF_PSE_TOP_BASE + 0xB8) /* 80B8 */
#define WF_PSE_TOP_TO_N9_INT_ADDR \
	(WF_PSE_TOP_BASE + 0xF0) /* 80F0 */
#define WF_PSE_TOP_GROUP_REFILL_CTRL_ADDR \
	(WF_PSE_TOP_BASE + 0x108) /* 8108 */
#define WF_PSE_TOP_FREEPG_START_END_ADDR \
	(WF_PSE_TOP_BASE + 0x10C) /* 810C */
#define WF_PSE_TOP_PG_HIF0_GROUP_ADDR \
	(WF_PSE_TOP_BASE + 0x110) /* 8110 */
#define WF_PSE_TOP_PG_HIF1_GROUP_ADDR \
	(WF_PSE_TOP_BASE + 0x114) /* 8114 */
#define WF_PSE_TOP_PG_CPU_GROUP_ADDR \
	(WF_PSE_TOP_BASE + 0x118) /* 8118 */
#define WF_PSE_TOP_PG_PLE_GROUP_ADDR \
	(WF_PSE_TOP_BASE + 0x11C) /* 811C */
#define WF_PSE_TOP_PG_LMAC0_GROUP_ADDR \
	(WF_PSE_TOP_BASE + 0x124) /* 8124 */
#define WF_PSE_TOP_PG_LMAC1_GROUP_ADDR \
	(WF_PSE_TOP_BASE + 0x128) /* 8128 */
#define WF_PSE_TOP_PG_LMAC2_GROUP_ADDR \
	(WF_PSE_TOP_BASE + 0x12C) /* 812C */
#define WF_PSE_TOP_PG_MDP_GROUP_ADDR \
	(WF_PSE_TOP_BASE + 0x134) /* 8134 */
#define WF_PSE_TOP_PG_MDP1_GROUP_ADDR \
	(WF_PSE_TOP_BASE + 0x138) /* 8138 */
#define WF_PSE_TOP_PG_HIF2_GROUP_ADDR \
	(WF_PSE_TOP_BASE + 0x140) /* 8140 */
#define WF_PSE_TOP_HIF0_PG_INFO_ADDR \
	(WF_PSE_TOP_BASE + 0x150) /* 8150 */
#define WF_PSE_TOP_HIF1_PG_INFO_ADDR \
	(WF_PSE_TOP_BASE + 0x154) /* 8154 */
#define WF_PSE_TOP_CPU_PG_INFO_ADDR \
	(WF_PSE_TOP_BASE + 0x158) /* 8158 */
#define WF_PSE_TOP_PLE_PG_INFO_ADDR \
	(WF_PSE_TOP_BASE + 0x15C) /* 815C */
#define WF_PSE_TOP_LMAC0_PG_INFO_ADDR \
	(WF_PSE_TOP_BASE + 0x164) /* 8164 */
#define WF_PSE_TOP_LMAC1_PG_INFO_ADDR \
	(WF_PSE_TOP_BASE + 0x168) /* 8168 */
#define WF_PSE_TOP_LMAC2_PG_INFO_ADDR \
	(WF_PSE_TOP_BASE + 0x16C) /* 816C */
#define WF_PSE_TOP_MDP_PG_INFO_ADDR \
	(WF_PSE_TOP_BASE + 0x174) /* 8174 */
#define WF_PSE_TOP_MDP1_PG_INFO_ADDR \
	(WF_PSE_TOP_BASE + 0x178) /* 8178 */
#define WF_PSE_TOP_HIF2_PG_INFO_ADDR \
	(WF_PSE_TOP_BASE + 0x180) /* 8180 */
#define WF_PSE_TOP_FL_QUE_CTRL_0_ADDR \
	(WF_PSE_TOP_BASE + 0x1B0) /* 81B0 */
#define WF_PSE_TOP_FL_QUE_CTRL_1_ADDR \
	(WF_PSE_TOP_BASE + 0x1B4) /* 81B4 */
#define WF_PSE_TOP_FL_QUE_CTRL_2_ADDR \
	(WF_PSE_TOP_BASE + 0x1B8) /* 81B8 */
#define WF_PSE_TOP_FL_QUE_CTRL_3_ADDR \
	(WF_PSE_TOP_BASE + 0x1BC) /* 81BC */
#define WF_PSE_TOP_PL_QUE_CTRL_0_ADDR \
	(WF_PSE_TOP_BASE + 0x1C0) /* 81C0 */
#define WF_PSE_TOP_PLE_ENQ_PKT_NUM_ADDR \
	(WF_PSE_TOP_BASE + 0x1F0) /* 81F0 */
#define WF_PSE_TOP_CPU_ENQ_PKT_NUM_ADDR \
	(WF_PSE_TOP_BASE + 0x1F4) /* 81F4 */
#define WF_PSE_TOP_LMAC_ENQ_PKT_NUM_ADDR \
	(WF_PSE_TOP_BASE + 0x1F8) /* 81F8 */
#define WF_PSE_TOP_HIF_ENQ_PKT_NUM_ADDR \
	(WF_PSE_TOP_BASE + 0x1FC) /* 81FC */
#define WF_PSE_TOP_MDP_ENQ_PKT_NUM_ADDR \
	(WF_PSE_TOP_BASE + 0x200) /* 8200 */
#define WF_PSE_TOP_RL_BUF_CTRL_0_ADDR \
	(WF_PSE_TOP_BASE + 0x210) /* 8210 */
#define WF_PSE_TOP_RL_BUF_CTRL_1_ADDR \
	(WF_PSE_TOP_BASE + 0x214) /* 8214 */
#define WF_PSE_TOP_TIMEOUT_CTRL_ADDR \
	(WF_PSE_TOP_BASE + 0x244) /* 8244 */
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_ADDR \
	(WF_PSE_TOP_BASE + 0x248) /* 8248 */
#define WF_PSE_TOP_FSM_IDLE_WD_EN_ADDR \
	(WF_PSE_TOP_BASE + 0x24C) /* 824C */
#define WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_ADDR \
	(WF_PSE_TOP_BASE + 0x250) /* 8250 */
#define WF_PSE_TOP_PSE_MISC_FUNC_CTRL_ADDR \
	(WF_PSE_TOP_BASE + 0x254) /* 8254 */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR \
	(WF_PSE_TOP_BASE + 0x258) /* 8258 */
#define WF_PSE_TOP_PSE_INTER_ERR_FLAG_ADDR \
	(WF_PSE_TOP_BASE + 0x280) /* 8280 */
#define WF_PSE_TOP_PSE_SER_CTRL_ADDR \
	(WF_PSE_TOP_BASE + 0x2A0) /* 82A0 */
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_ADDR \
	(WF_PSE_TOP_BASE + 0x2B0) /* 82B0 */
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_1_ADDR \
	(WF_PSE_TOP_BASE + 0x2B4) /* 82B4 */
#define WF_PSE_TOP_PSE_MBIST_BSEL_ADDR \
	(WF_PSE_TOP_BASE + 0x2B8) /* 82B8 */
#define WF_PSE_TOP_SRAM_MBIST_BACKGROUND_ADDR \
	(WF_PSE_TOP_BASE + 0x2C0) /* 82C0 */
#define WF_PSE_TOP_SRAM_MBIST_DONE_ADDR \
	(WF_PSE_TOP_BASE + 0x2C4) /* 82C4 */
#define WF_PSE_TOP_SRAM_MBIST_FAIL_ADDR \
	(WF_PSE_TOP_BASE + 0x2C8) /* 82C8 */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR \
	(WF_PSE_TOP_BASE + 0x2D0) /* 82D0 */
#define WF_PSE_TOP_SRAM_MBIST_DELSEL_ADDR \
	(WF_PSE_TOP_BASE + 0x2D4) /* 82D4 */
#define WF_PSE_TOP_SRAM_MBIST_DELSEL_1_ADDR \
	(WF_PSE_TOP_BASE + 0x2D8) /* 82D8 */
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_ADDR \
	(WF_PSE_TOP_BASE + 0x2DC) /* 82DC */
#define WF_PSE_TOP_C_GET_FID_0_ADDR \
	(WF_PSE_TOP_BASE + 0x310) /* 8310 */
#define WF_PSE_TOP_C_GET_FID_1_ADDR \
	(WF_PSE_TOP_BASE + 0x314) /* 8314 */
#define WF_PSE_TOP_C_EN_QUEUE_0_ADDR \
	(WF_PSE_TOP_BASE + 0x320) /* 8320 */
#define WF_PSE_TOP_C_EN_QUEUE_1_ADDR \
	(WF_PSE_TOP_BASE + 0x324) /* 8324 */
#define WF_PSE_TOP_C_EN_QUEUE_2_ADDR \
	(WF_PSE_TOP_BASE + 0x328) /* 8328 */
#define WF_PSE_TOP_C_DE_QUEUE_0_ADDR \
	(WF_PSE_TOP_BASE + 0x330) /* 8330 */
#define WF_PSE_TOP_C_DE_QUEUE_1_ADDR \
	(WF_PSE_TOP_BASE + 0x334) /* 8334 */
#define WF_PSE_TOP_C_DE_QUEUE_2_ADDR \
	(WF_PSE_TOP_BASE + 0x338) /* 8338 */
#define WF_PSE_TOP_C_DE_QUEUE_3_ADDR \
	(WF_PSE_TOP_BASE + 0x33C) /* 833C */
#define WF_PSE_TOP_C_DE_QUEUE_4_ADDR \
	(WF_PSE_TOP_BASE + 0x340) /* 8340 */
#define WF_PSE_TOP_ALLOCATE_0_ADDR \
	(WF_PSE_TOP_BASE + 0x350) /* 8350 */
#define WF_PSE_TOP_ALLOCATE_1_ADDR \
	(WF_PSE_TOP_BASE + 0x354) /* 8354 */
#define WF_PSE_TOP_FREEPG_CNT_ADDR \
	(WF_PSE_TOP_BASE + 0x380) /* 8380 */
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_ADDR \
	(WF_PSE_TOP_BASE + 0x384) /* 8384 */
#define WF_PSE_TOP_PSE_SEEK_CR_00_ADDR \
	(WF_PSE_TOP_BASE + 0x3D0) /* 83D0 */
#define WF_PSE_TOP_PSE_SEEK_CR_01_ADDR \
	(WF_PSE_TOP_BASE + 0x3D4) /* 83D4 */
#define WF_PSE_TOP_PSE_SEEK_CR_02_ADDR \
	(WF_PSE_TOP_BASE + 0x3D8) /* 83D8 */
#define WF_PSE_TOP_PSE_SEEK_CR_03_ADDR \
	(WF_PSE_TOP_BASE + 0x3DC) /* 83DC */
#define WF_PSE_TOP_PSE_SEEK_CR_04_ADDR \
	(WF_PSE_TOP_BASE + 0x3E0) /* 83E0 */
#define WF_PSE_TOP_PSE_SEEK_CR_05_ADDR \
	(WF_PSE_TOP_BASE + 0x3E4) /* 83E4 */
#define WF_PSE_TOP_PSE_SEEK_CR_06_ADDR \
	(WF_PSE_TOP_BASE + 0x3E8) /* 83E8 */
#define WF_PSE_TOP_PSE_SEEK_CR_07_ADDR \
	(WF_PSE_TOP_BASE + 0x3EC) /* 83EC */
#define WF_PSE_TOP_PSE_SEEK_CR_08_ADDR \
	(WF_PSE_TOP_BASE + 0x3F0) /* 83F0 */
#define WF_PSE_TOP_PSE_SEEK_CR_09_ADDR \
	(WF_PSE_TOP_BASE + 0x3F4) /* 83F4 */




/*
* ---GC (0x820C8000 + 0x00)---
* ALL_RESET[0] - (RW) Resets PSE logic and register
* LOGIC_RESET[1] - (RW) Resets PSE logic circuit
* INIT_DONE[2] - (RO) PSE control SRAM initialization indicator
* RESERVED3[15..3] - (RO) Reserved bits
* SRAM_MBIST_RESET[16] - (RW) Reset control of SRAM MBIST (low active)
* RESERVED17[17] - (RO) Reserved bits
* DIS_PSE_DYN_CKG[18] - (RW) Disable control of wf_pse_top dynamic clock gating
function
* RESERVED19[31..19] - (RO) Reserved bits
*/
#define WF_PSE_TOP_GC_DIS_PSE_DYN_CKG_ADDR \
	WF_PSE_TOP_GC_ADDR
#define WF_PSE_TOP_GC_DIS_PSE_DYN_CKG_MASK \
	0x00040000 /* DIS_PSE_DYN_CKG[18] */
#define WF_PSE_TOP_GC_DIS_PSE_DYN_CKG_SHFT                     18
#define WF_PSE_TOP_GC_SRAM_MBIST_RESET_ADDR \
	WF_PSE_TOP_GC_ADDR
#define WF_PSE_TOP_GC_SRAM_MBIST_RESET_MASK \
	0x00010000 /* SRAM_MBIST_RESET[16] */
#define WF_PSE_TOP_GC_SRAM_MBIST_RESET_SHFT                    16
#define WF_PSE_TOP_GC_INIT_DONE_ADDR \
	WF_PSE_TOP_GC_ADDR
#define WF_PSE_TOP_GC_INIT_DONE_MASK \
	0x00000004 /* INIT_DONE[2] */
#define WF_PSE_TOP_GC_INIT_DONE_SHFT                           2
#define WF_PSE_TOP_GC_LOGIC_RESET_ADDR \
	WF_PSE_TOP_GC_ADDR
#define WF_PSE_TOP_GC_LOGIC_RESET_MASK \
	0x00000002 /* LOGIC_RESET[1] */
#define WF_PSE_TOP_GC_LOGIC_RESET_SHFT                         1
#define WF_PSE_TOP_GC_ALL_RESET_ADDR \
	WF_PSE_TOP_GC_ADDR
#define WF_PSE_TOP_GC_ALL_RESET_MASK \
	0x00000001 /* ALL_RESET[0] */
#define WF_PSE_TOP_GC_ALL_RESET_SHFT                           0

/*
* ---PBUF_CTRL (0x820C8000 + 0x04)---
* TOTAL_PAGE_NUM[11..0] - (RW) Total page numbers
* Set the total page before release PSE logic reset, and must not be changed
* after release logic reset.
* RESERVED12[16..12] - (RO) Reserved bits
* PBUF_OFFSET[25..17] - (RW) Packet buffer offset.
* Set the buffer offset before release PSE logic reset, and must not be changed
* after release logic reset.
* RESERVED26[30..26] - (RO) Reserved bits
* PAGE_SIZE_CFG[31] - (RW) Configures page size
* Set up the page size before releasing PSE logic reset; it should not be
* changed after logic reset is released.
*/
#define WF_PSE_TOP_PBUF_CTRL_PAGE_SIZE_CFG_ADDR \
	WF_PSE_TOP_PBUF_CTRL_ADDR
#define WF_PSE_TOP_PBUF_CTRL_PAGE_SIZE_CFG_MASK \
	0x80000000 /* PAGE_SIZE_CFG[31] */
#define WF_PSE_TOP_PBUF_CTRL_PAGE_SIZE_CFG_SHFT                31
#define WF_PSE_TOP_PBUF_CTRL_PBUF_OFFSET_ADDR \
	WF_PSE_TOP_PBUF_CTRL_ADDR
#define WF_PSE_TOP_PBUF_CTRL_PBUF_OFFSET_MASK \
	0x03FE0000 /* PBUF_OFFSET[25..17] */
#define WF_PSE_TOP_PBUF_CTRL_PBUF_OFFSET_SHFT                  17
#define WF_PSE_TOP_PBUF_CTRL_TOTAL_PAGE_NUM_ADDR \
	WF_PSE_TOP_PBUF_CTRL_ADDR
#define WF_PSE_TOP_PBUF_CTRL_TOTAL_PAGE_NUM_MASK \
	0x00000FFF /* TOTAL_PAGE_NUM[11..0] */
#define WF_PSE_TOP_PBUF_CTRL_TOTAL_PAGE_NUM_SHFT               0

/*
* ---INT_N9_EN_MASK (0x820C8000 + 0x08)---
* EN_CPU_Q0_NE[0] - (RW) CPU queue 0 not empty interrupt
* EN_CPU_Q1_NE[1] - (RW) CPU queue 1 not empty interrupt
* EN_CPU_Q2_NE[2] - (RW) CPU queue 2 not empty interrupt
* EN_CPU_Q3_NE[3] - (RW) CPU queue 3 not empty interrupt
* EN_CPU_Q4_NE[4] - (RW) CPU queue 4 not empty interrupt
* RESERVED5[11..5] - (RO) Reserved bits
* EN_LMAC_ENQ[12] - (RW) Enables LMAC enq interrupt
* EN_ERROR_INT[13] - (RW) Error condition interrupt status
* RESERVED14[15..14] - (RO) Reserved bits
* EN_TOGGLE_INT[16] - (RW) Data toggle of CR4 toggle register (0xe0)
* EN_LMAC_EMPTY_FALL[17] - (RW) LMAC Buffer empty fall edge detect
* EN_LMAC_EMPTY_RAISE[18] - (RW) LMAC Buffer empty raise edge detect
* EN_HIF_Q0_NE[19] - (RW) HIF queue 0 not empty interrupt
* EN_HIF_Q1_NE[20] - (RW) HIF queue 1 not empty interrupt
* EN_HIF_Q2_NE[21] - (RW) HIF queue 2 not empty interrupt
* EN_HIF_Q3_NE[22] - (RW) HIF queue 3 not empty interrupt
* EN_HIF_Q4_NE[23] - (RW) HIF queue 4 not empty interrupt
* EN_HIF_Q5_NE[24] - (RW) HIF queue 5 not empty interrupt
* EN_HIF_Q6_NE[25] - (RW) HIF queue 6 not empty interrupt
* EN_HIF_Q7_NE[26] - (RW) HIF queue 7 not empty interrupt
* EN_HIF_Q8_NE[27] - (RW) HIF queue 8 not empty interrupt
* EN_HIF_Q9_NE[28] - (RW) HIF queue 9 not empty interrupt
* EN_HIF_Q10_NE[29] - (RW) HIF queue 10 not empty interrupt
* EN_HIF_Q11_NE[30] - (RW) HIF queue 11 not empty interrupt
* RESERVED31[31] - (RO) Reserved bits
*/
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q11_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q11_NE_MASK \
	0x40000000 /* EN_HIF_Q11_NE[30] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q11_NE_SHFT           30
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q10_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q10_NE_MASK \
	0x20000000 /* EN_HIF_Q10_NE[29] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q10_NE_SHFT           29
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q9_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q9_NE_MASK \
	0x10000000 /* EN_HIF_Q9_NE[28] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q9_NE_SHFT            28
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q8_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q8_NE_MASK \
	0x08000000 /* EN_HIF_Q8_NE[27] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q8_NE_SHFT            27
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q7_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q7_NE_MASK \
	0x04000000 /* EN_HIF_Q7_NE[26] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q7_NE_SHFT            26
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q6_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q6_NE_MASK \
	0x02000000 /* EN_HIF_Q6_NE[25] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q6_NE_SHFT            25
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q5_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q5_NE_MASK \
	0x01000000 /* EN_HIF_Q5_NE[24] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q5_NE_SHFT            24
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q4_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q4_NE_MASK \
	0x00800000 /* EN_HIF_Q4_NE[23] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q4_NE_SHFT            23
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q3_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q3_NE_MASK \
	0x00400000 /* EN_HIF_Q3_NE[22] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q3_NE_SHFT            22
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q2_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q2_NE_MASK \
	0x00200000 /* EN_HIF_Q2_NE[21] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q2_NE_SHFT            21
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q1_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q1_NE_MASK \
	0x00100000 /* EN_HIF_Q1_NE[20] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q1_NE_SHFT            20
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q0_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q0_NE_MASK \
	0x00080000 /* EN_HIF_Q0_NE[19] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_HIF_Q0_NE_SHFT            19
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_LMAC_EMPTY_RAISE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_LMAC_EMPTY_RAISE_MASK \
	0x00040000 /* EN_LMAC_EMPTY_RAISE[18] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_LMAC_EMPTY_RAISE_SHFT     18
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_LMAC_EMPTY_FALL_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_LMAC_EMPTY_FALL_MASK \
	0x00020000 /* EN_LMAC_EMPTY_FALL[17] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_LMAC_EMPTY_FALL_SHFT      17
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_TOGGLE_INT_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_TOGGLE_INT_MASK \
	0x00010000 /* EN_TOGGLE_INT[16] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_TOGGLE_INT_SHFT           16
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_ERROR_INT_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_ERROR_INT_MASK \
	0x00002000 /* EN_ERROR_INT[13] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_ERROR_INT_SHFT            13
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_LMAC_ENQ_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_LMAC_ENQ_MASK \
	0x00001000 /* EN_LMAC_ENQ[12] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_LMAC_ENQ_SHFT             12
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q4_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q4_NE_MASK \
	0x00000010 /* EN_CPU_Q4_NE[4] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q4_NE_SHFT            4
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q3_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q3_NE_MASK \
	0x00000008 /* EN_CPU_Q3_NE[3] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q3_NE_SHFT            3
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q2_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q2_NE_MASK \
	0x00000004 /* EN_CPU_Q2_NE[2] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q2_NE_SHFT            2
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q1_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q1_NE_MASK \
	0x00000002 /* EN_CPU_Q1_NE[1] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q1_NE_SHFT            1
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q0_NE_ADDR \
	WF_PSE_TOP_INT_N9_EN_MASK_ADDR
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q0_NE_MASK \
	0x00000001 /* EN_CPU_Q0_NE[0] */
#define WF_PSE_TOP_INT_N9_EN_MASK_EN_CPU_Q0_NE_SHFT            0

/*
* ---INT_N9_ERR_MASK (0x820C8000 + 0x24)---
* EN_Q_CMD_ERR_P0[0] - (RW) Enable Queue command error interrupt status of PP
* port. Avoid unclear error flag, please clear flag when logic reset.
* EN_Q_CMD_ERR_P1[1] - (RW) Enable Queue command error interrupt status of CPU
* port. Avoid unclear error flag, please clear flag when logic reset.
* EN_Q_CMD_ERR_P2[2] - (RW) Enable Queue command error interrupt status of LMAC
* DMA port. Avoid unclear error flag, please clear flag when logic reset.
* EN_Q_CMD_ERR_P3[3] - (RW) Enable Queue command error interrupt status of MDP
* port. Avoid unclear error flag, please clear flag when logic reset.
* EN_Q_CMD_ERR_P4[4] - (RW) Enable Queue command error interrupt status of SEC
* port. Avoid unclear error flag, please clear flag when logic reset.
* EN_Q_CMD_ERR_P5[5] - (RW) Enable Queue command error interrupt status of PLE
* port. Avoid unclear error flag, please clear flag when logic reset.
* EN_Q_CMD_ERR_P6[6] - (RW) Enable Queue command error interrupt status of AMSDU
* port. Avoid unclear error flag, please clear flag when logic reset.
* RESERVED7[7] - (RO) Reserved bits
* EN_PAGE_UDF_P0[8] - (RW) Enable Page underflow interrupt status of PP port.
* Avoid unclear error flag, please clear flag when logic reset.
* EN_PAGE_UDF_P1[9] - (RW) Enable Page underflow interrupt status of CPU port.
* Avoid unclear error flag, please clear flag when logic reset.
* EN_PAGE_UDF_P2[10] - (RW) Enable Page underflow interrupt status of LMAC DMA
* port. Avoid unclear error flag, please clear flag when logic reset.
* EN_PAGE_UDF_P3[11] - (RW) Enable Page underflow interrupt status of MDP port.
* Avoid unclear error flag, please clear flag when logic reset.
* EN_PAGE_UDF_P4[12] - (RW) Enable Page underflow interrupt status of SEC port.
* Avoid unclear error flag, please clear flag when logic reset.
* EN_PAGE_UDF_P5[13] - (RW) Enable Page underflow interrupt status of PLE port.
* Avoid unclear error flag, please clear flag when logic reset.
* EN_PAGE_UDF_P6[14] - (RW) Enable Page underflow interrupt status of AMSDU
* port. Avoid unclear error flag, please clear flag when logic reset.
* RESERVED15[15] - (RO) Reserved bits
* EN_QUEUE_OPER_ERR_P0[16] - (RW) Enable Queue operation error of PP port. Avoid
* unclear error flag, please clear flag when logic reset.
* EN_QUEUE_OPER_ERR_P1[17] - (RW) Enable Queue operation error of CPU port.
* Avoid unclear error flag, please clear flag when logic reset.
* EN_QUEUE_OPER_ERR_P2[18] - (RW) Enable Queue operation error of LMAC DMA port.
* Avoid unclear error flag, please clear flag when logic reset.
* EN_QUEUE_OPER_ERR_P3[19] - (RW) Enable Queue operation error of MDP port.
* Avoid unclear error flag, please clear flag when logic reset.
* EN_QUEUE_OPER_ERR_P4[20] - (RW) Enable Queue operation error of SEC port.
* Avoid unclear error flag, please clear flag when logic reset.
* EN_QUEUE_OPER_ERR_P5[21] - (RW) Enable Queue operation error of PLE port.
* Avoid unclear error flag, please clear flag when logic reset.
* EN_QUEUE_OPER_ERR_P6[22] - (RW) Enable Queue operation error of AMSDU port.
* Avoid unclear error flag, please clear flag when logic reset.
* RESERVED23[23] - (RO) Reserved bits
* EN_DATA_OPER_ERR_P0[24] - (RW) Enable Data operation error of PP port. Avoid
* unclear error flag, please clear flag when logic reset.
* EN_DATA_OPER_ERR_P1[25] - (RW) Enable Data operation error of CPU port. Avoid
* unclear error flag, please clear flag when logic reset.
* EN_DATA_OPER_ERR_P2[26] - (RW) Enable Data operation error of LMAC DMA port.
* Avoid unclear error flag, please clear flag when logic reset.
* EN_DATA_OPER_ERR_P3[27] - (RW) Enable Data operation error of MDP port. Avoid
* unclear error flag, please clear flag when logic reset.
* EN_DATA_OPER_ERR_P4[28] - (RW) Enable Data operation error of SEC port. Avoid
* unclear error flag, please clear flag when logic reset.
* EN_DATA_OPER_ERR_P5[29] - (RW) Enable Data operation error of PLE port. Avoid
* unclear error flag, please clear flag when logic reset.
* EN_DATA_OPER_EN_ERR_P6[30] - (RW) Enable Data operation error of AMSDU port.
* Avoid unclear error flag, please clear flag when logic reset.
* RESERVED31[31] - (RO) Reserved bits
*/
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_EN_ERR_P6_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_EN_ERR_P6_MASK \
	0x40000000 /* EN_DATA_OPER_EN_ERR_P6[30] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_EN_ERR_P6_SHFT 30
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P5_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P5_MASK \
	0x20000000 /* EN_DATA_OPER_ERR_P5[29] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P5_SHFT    29
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P4_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P4_MASK \
	0x10000000 /* EN_DATA_OPER_ERR_P4[28] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P4_SHFT    28
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P3_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P3_MASK \
	0x08000000 /* EN_DATA_OPER_ERR_P3[27] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P3_SHFT    27
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P2_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P2_MASK \
	0x04000000 /* EN_DATA_OPER_ERR_P2[26] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P2_SHFT    26
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P1_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P1_MASK \
	0x02000000 /* EN_DATA_OPER_ERR_P1[25] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P1_SHFT    25
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P0_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P0_MASK \
	0x01000000 /* EN_DATA_OPER_ERR_P0[24] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_DATA_OPER_ERR_P0_SHFT    24
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P6_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P6_MASK \
	0x00400000 /* EN_QUEUE_OPER_ERR_P6[22] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P6_SHFT   22
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P5_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P5_MASK \
	0x00200000 /* EN_QUEUE_OPER_ERR_P5[21] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P5_SHFT   21
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P4_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P4_MASK \
	0x00100000 /* EN_QUEUE_OPER_ERR_P4[20] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P4_SHFT   20
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P3_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P3_MASK \
	0x00080000 /* EN_QUEUE_OPER_ERR_P3[19] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P3_SHFT   19
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P2_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P2_MASK \
	0x00040000 /* EN_QUEUE_OPER_ERR_P2[18] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P2_SHFT   18
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P1_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P1_MASK \
	0x00020000 /* EN_QUEUE_OPER_ERR_P1[17] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P1_SHFT   17
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P0_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P0_MASK \
	0x00010000 /* EN_QUEUE_OPER_ERR_P0[16] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_QUEUE_OPER_ERR_P0_SHFT   16
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P6_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P6_MASK \
	0x00004000 /* EN_PAGE_UDF_P6[14] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P6_SHFT         14
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P5_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P5_MASK \
	0x00002000 /* EN_PAGE_UDF_P5[13] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P5_SHFT         13
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P4_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P4_MASK \
	0x00001000 /* EN_PAGE_UDF_P4[12] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P4_SHFT         12
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P3_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P3_MASK \
	0x00000800 /* EN_PAGE_UDF_P3[11] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P3_SHFT         11
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P2_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P2_MASK \
	0x00000400 /* EN_PAGE_UDF_P2[10] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P2_SHFT         10
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P1_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P1_MASK \
	0x00000200 /* EN_PAGE_UDF_P1[9] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P1_SHFT         9
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P0_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P0_MASK \
	0x00000100 /* EN_PAGE_UDF_P0[8] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_PAGE_UDF_P0_SHFT         8
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P6_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P6_MASK \
	0x00000040 /* EN_Q_CMD_ERR_P6[6] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P6_SHFT        6
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P5_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P5_MASK \
	0x00000020 /* EN_Q_CMD_ERR_P5[5] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P5_SHFT        5
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P4_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P4_MASK \
	0x00000010 /* EN_Q_CMD_ERR_P4[4] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P4_SHFT        4
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P3_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P3_MASK \
	0x00000008 /* EN_Q_CMD_ERR_P3[3] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P3_SHFT        3
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P2_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P2_MASK \
	0x00000004 /* EN_Q_CMD_ERR_P2[2] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P2_SHFT        2
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P1_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P1_MASK \
	0x00000002 /* EN_Q_CMD_ERR_P1[1] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P1_SHFT        1
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P0_ADDR \
	WF_PSE_TOP_INT_N9_ERR_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P0_MASK \
	0x00000001 /* EN_Q_CMD_ERR_P0[0] */
#define WF_PSE_TOP_INT_N9_ERR_MASK_EN_Q_CMD_ERR_P0_SHFT        0

/*
* ---INT_N9_ERR1_MASK (0x820C8000 + 0x28)---
* EN_WDT_ERR_P0[0] - (RW) Enable Port state watch dog timeout error interrupt
* status of PP port. Avoid unclear error flag, please clear flag when logic
reset.
* EN_WDT_ERR_P1[1] - (RW) Enable Port state watch dog timeout error interrupt
* status of CPU port. Avoid unclear error flag, please clear flag when logic
reset.
* EN_WDT_ERR_P2[2] - (RW) Enable Port state watch dog timeout error interrupt
* status of LMAC DMA port. Avoid unclear error flag, please clear flag when
* logic reset.
* EN_WDT_ERR_P3[3] - (RW) Enable Port state watch dog timeout error interrupt
* status of MDP port. Avoid unclear error flag, please clear flag when logic
reset.
* EN_WDT_ERR_P4[4] - (RW) Enable Port state watch dog timeout error interrupt
* status of SEC port. Avoid unclear error flag, please clear flag when logic
reset.
* EN_WDT_ERR_P5[5] - (RW) Enable Port state watch dog timeout error interrupt
* status of PLE port. Avoid unclear error flag, please clear flag when logic
reset.
* EN_WDT_ERR_P6[6] - (RW) Enable Port state watch dog timeout error interrupt
* status of AMSDU port. Avoid unclear error flag, please clear flag when logic
reset.
* RESERVED7[7] - (RO) Reserved bits
* EN_FL_HANG_ERR[8] - (RW) Enable Frame link FSM hang error interrupt. Avoid
* unclear error flag, please clear flag when logic reset.
* PL_HANGEN__ERR[9] - (RW) Enable Page link FSM hang error interrupt. Avoid
* unclear error flag, please clear flag when logic reset.
* EN_DOUBLE_RLS_ERR[10] - (RW) Enable Double release error interrupt. Avoid
* unclear error flag, please clear flag when logic reset.
* EN_FREE_HEAD_TAIL_ERR[11] - (RW) Enable Free head/tail error interrupt. Avoid
* unclear error flag, please clear flag when logic reset.
* EN_FL_QSTRTUT_ERR[12] - (RW) Enable Frame Link queue NULL error interrupt.
* Avoid unclear error flag, please clear flag when logic reset.
* RESERVED13[31..13] - (RO) Reserved bits
*/
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_FL_QSTRTUT_ERR_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_FL_QSTRTUT_ERR_MASK \
	0x00001000 /* EN_FL_QSTRTUT_ERR[12] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_FL_QSTRTUT_ERR_SHFT     12
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_FREE_HEAD_TAIL_ERR_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_FREE_HEAD_TAIL_ERR_MASK \
	0x00000800 /* EN_FREE_HEAD_TAIL_ERR[11] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_FREE_HEAD_TAIL_ERR_SHFT 11
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_DOUBLE_RLS_ERR_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_DOUBLE_RLS_ERR_MASK \
	0x00000400 /* EN_DOUBLE_RLS_ERR[10] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_DOUBLE_RLS_ERR_SHFT     10
#define WF_PSE_TOP_INT_N9_ERR1_MASK_PL_HANGEN__ERR_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_PL_HANGEN__ERR_MASK \
	0x00000200 /* PL_HANGEN__ERR[9] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_PL_HANGEN__ERR_SHFT        9
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_FL_HANG_ERR_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_FL_HANG_ERR_MASK \
	0x00000100 /* EN_FL_HANG_ERR[8] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_FL_HANG_ERR_SHFT        8
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P6_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P6_MASK \
	0x00000040 /* EN_WDT_ERR_P6[6] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P6_SHFT         6
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P5_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P5_MASK \
	0x00000020 /* EN_WDT_ERR_P5[5] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P5_SHFT         5
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P4_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P4_MASK \
	0x00000010 /* EN_WDT_ERR_P4[4] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P4_SHFT         4
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P3_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P3_MASK \
	0x00000008 /* EN_WDT_ERR_P3[3] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P3_SHFT         3
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P2_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P2_MASK \
	0x00000004 /* EN_WDT_ERR_P2[2] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P2_SHFT         2
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P1_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P1_MASK \
	0x00000002 /* EN_WDT_ERR_P1[1] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P1_SHFT         1
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P0_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P0_MASK \
	0x00000001 /* EN_WDT_ERR_P0[0] */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_EN_WDT_ERR_P0_SHFT         0

/*
* ---INT_N9_STS (0x820C8000 + 0x30)---
* CPU_Q0_NE[0] - (W1C) CPU queue 0 not empty interrupt status
* CPU_Q1_NE[1] - (W1C) CPU queue 1 not empty interrupt status
* CPU_Q2_NE[2] - (W1C) CPU queue 2 not empty interrupt status
* CPU_Q3_NE[3] - (W1C) CPU queue 3 not empty interrupt status
* CPU_Q4_NE[4] - (W1C) CPU queue 4 not empty interrupt status
* RESERVED5[11..5] - (RO) Reserved bits
* LMAC_ENQ[12] - (W1C) LMAC enq interrupt status
* ERROR_INT[13] - (RO) Error condition interrupt status
* ERROR_1_INT[14] - (RO) Error condition interrupt status
* RESERVED15[15] - (RO) Reserved bits
* DATA_TOGGLE[16] - (W1C) Data toggle of CR4 toggle register (0xe0)
* LMAC_EMPTY_FALL[17] - (W1C) LMAC Buffer empty fall edge detect
* LMAC_EMPTY_RAISE[18] - (W1C) LMAC Buffer empty raise edge detect
* HIF_Q0_NE[19] - (W1C) HIF queue 0 not empty interrupt status
* HIF_Q1_NE[20] - (W1C) HIF queue 1 not empty interrupt status
* HIF_Q2_NE[21] - (W1C) HIF queue 2 not empty interrupt status
* HIF_Q3_NE[22] - (W1C) HIF queue 3 not empty interrupt status
* HIF_Q4_NE[23] - (W1C) HIF queue 4 not empty interrupt status
* HIF_Q5_NE[24] - (W1C) HIF queue 5 not empty interrupt status
* HIF_Q6_NE[25] - (W1C) HIF queue 6 not empty interrupt status
* HIF_Q7_NE[26] - (W1C) HIF queue 7 not empty interrupt status
* HIF_Q8_NE[27] - (W1C) HIF queue 8 not empty interrupt status
* HIF_Q9_NE[28] - (W1C) HIF queue 9 not empty interrupt status
* HIF_Q10_NE[29] - (W1C) HIF queue 10 not empty interrupt status
* HIF_Q11_NE[30] - (W1C) HIF queue 11 not empty interrupt status
* RESERVED31[31] - (RO) Reserved bits
*/
#define WF_PSE_TOP_INT_N9_STS_HIF_Q11_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q11_NE_MASK \
	0x40000000 /* HIF_Q11_NE[30] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q11_NE_SHFT                  30
#define WF_PSE_TOP_INT_N9_STS_HIF_Q10_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q10_NE_MASK \
	0x20000000 /* HIF_Q10_NE[29] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q10_NE_SHFT                  29
#define WF_PSE_TOP_INT_N9_STS_HIF_Q9_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q9_NE_MASK \
	0x10000000 /* HIF_Q9_NE[28] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q9_NE_SHFT                   28
#define WF_PSE_TOP_INT_N9_STS_HIF_Q8_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q8_NE_MASK \
	0x08000000 /* HIF_Q8_NE[27] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q8_NE_SHFT                   27
#define WF_PSE_TOP_INT_N9_STS_HIF_Q7_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q7_NE_MASK \
	0x04000000 /* HIF_Q7_NE[26] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q7_NE_SHFT                   26
#define WF_PSE_TOP_INT_N9_STS_HIF_Q6_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q6_NE_MASK \
	0x02000000 /* HIF_Q6_NE[25] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q6_NE_SHFT                   25
#define WF_PSE_TOP_INT_N9_STS_HIF_Q5_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q5_NE_MASK \
	0x01000000 /* HIF_Q5_NE[24] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q5_NE_SHFT                   24
#define WF_PSE_TOP_INT_N9_STS_HIF_Q4_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q4_NE_MASK \
	0x00800000 /* HIF_Q4_NE[23] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q4_NE_SHFT                   23
#define WF_PSE_TOP_INT_N9_STS_HIF_Q3_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q3_NE_MASK \
	0x00400000 /* HIF_Q3_NE[22] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q3_NE_SHFT                   22
#define WF_PSE_TOP_INT_N9_STS_HIF_Q2_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q2_NE_MASK \
	0x00200000 /* HIF_Q2_NE[21] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q2_NE_SHFT                   21
#define WF_PSE_TOP_INT_N9_STS_HIF_Q1_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q1_NE_MASK \
	0x00100000 /* HIF_Q1_NE[20] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q1_NE_SHFT                   20
#define WF_PSE_TOP_INT_N9_STS_HIF_Q0_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_HIF_Q0_NE_MASK \
	0x00080000 /* HIF_Q0_NE[19] */
#define WF_PSE_TOP_INT_N9_STS_HIF_Q0_NE_SHFT                   19
#define WF_PSE_TOP_INT_N9_STS_LMAC_EMPTY_RAISE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_LMAC_EMPTY_RAISE_MASK \
	0x00040000 /* LMAC_EMPTY_RAISE[18] */
#define WF_PSE_TOP_INT_N9_STS_LMAC_EMPTY_RAISE_SHFT            18
#define WF_PSE_TOP_INT_N9_STS_LMAC_EMPTY_FALL_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_LMAC_EMPTY_FALL_MASK \
	0x00020000 /* LMAC_EMPTY_FALL[17] */
#define WF_PSE_TOP_INT_N9_STS_LMAC_EMPTY_FALL_SHFT             17
#define WF_PSE_TOP_INT_N9_STS_DATA_TOGGLE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_DATA_TOGGLE_MASK \
	0x00010000 /* DATA_TOGGLE[16] */
#define WF_PSE_TOP_INT_N9_STS_DATA_TOGGLE_SHFT                 16
#define WF_PSE_TOP_INT_N9_STS_ERROR_1_INT_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_ERROR_1_INT_MASK \
	0x00004000 /* ERROR_1_INT[14] */
#define WF_PSE_TOP_INT_N9_STS_ERROR_1_INT_SHFT                 14
#define WF_PSE_TOP_INT_N9_STS_ERROR_INT_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_ERROR_INT_MASK \
	0x00002000 /* ERROR_INT[13] */
#define WF_PSE_TOP_INT_N9_STS_ERROR_INT_SHFT                   13
#define WF_PSE_TOP_INT_N9_STS_LMAC_ENQ_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_LMAC_ENQ_MASK \
	0x00001000 /* LMAC_ENQ[12] */
#define WF_PSE_TOP_INT_N9_STS_LMAC_ENQ_SHFT                    12
#define WF_PSE_TOP_INT_N9_STS_CPU_Q4_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_CPU_Q4_NE_MASK \
	0x00000010 /* CPU_Q4_NE[4] */
#define WF_PSE_TOP_INT_N9_STS_CPU_Q4_NE_SHFT                   4
#define WF_PSE_TOP_INT_N9_STS_CPU_Q3_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_CPU_Q3_NE_MASK \
	0x00000008 /* CPU_Q3_NE[3] */
#define WF_PSE_TOP_INT_N9_STS_CPU_Q3_NE_SHFT                   3
#define WF_PSE_TOP_INT_N9_STS_CPU_Q2_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_CPU_Q2_NE_MASK \
	0x00000004 /* CPU_Q2_NE[2] */
#define WF_PSE_TOP_INT_N9_STS_CPU_Q2_NE_SHFT                   2
#define WF_PSE_TOP_INT_N9_STS_CPU_Q1_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_CPU_Q1_NE_MASK \
	0x00000002 /* CPU_Q1_NE[1] */
#define WF_PSE_TOP_INT_N9_STS_CPU_Q1_NE_SHFT                   1
#define WF_PSE_TOP_INT_N9_STS_CPU_Q0_NE_ADDR \
	WF_PSE_TOP_INT_N9_STS_ADDR
#define WF_PSE_TOP_INT_N9_STS_CPU_Q0_NE_MASK \
	0x00000001 /* CPU_Q0_NE[0] */
#define WF_PSE_TOP_INT_N9_STS_CPU_Q0_NE_SHFT                   0

/*
* ---INT_N9_ERR_STS (0x820C8000 + 0x34)---
* Q_CMD_ERR_P0[0] - (W1C) Queue command error interrupt status of PP port. Avoid
* unclear error flag, please clear flag when logic reset.
* Q_CMD_ERR_P1[1] - (W1C) Queue command error interrupt status of CPU port.
* Avoid unclear error flag, please clear flag when logic reset.
* Q_CMD_ERR_P2[2] - (W1C) Queue command error interrupt status of LMAC DMA port.
* Avoid unclear error flag, please clear flag when logic reset.
* Q_CMD_ERR_P3[3] - (W1C) Queue command error interrupt status of MDP port.
* Avoid unclear error flag, please clear flag when logic reset.
* Q_CMD_ERR_P4[4] - (W1C) Queue command error interrupt status of SEC port.
* Avoid unclear error flag, please clear flag when logic reset.
* Q_CMD_ERR_P5[5] - (W1C) Queue command error interrupt status of PLE port.
* Avoid unclear error flag, please clear flag when logic reset.
* Q_CMD_ERR_P6[6] - (W1C) Queue command error interrupt status of AMSDU port.
* Avoid unclear error flag, please clear flag when logic reset.
* RESERVED7[7] - (RO) Reserved bits
* PAGE_UDF_P0[8] - (W1C) Page underflow interrupt status of PP port. Avoid
* unclear error flag, please clear flag when logic reset.
* PAGE_UDF_P1[9] - (W1C) Page underflow interrupt status of CPU port. Avoid
* unclear error flag, please clear flag when logic reset.
* PAGE_UDF_P2[10] - (W1C) Page underflow interrupt status of LMAC DMA port.
* Avoid unclear error flag, please clear flag when logic reset.
* PAGE_UDF_P3[11] - (W1C) Page underflow interrupt status of MDP port. Avoid
* unclear error flag, please clear flag when logic reset.
* PAGE_UDF_P4[12] - (W1C) Page underflow interrupt status of SEC port. Avoid
* unclear error flag, please clear flag when logic reset.
* PAGE_UDF_P5[13] - (W1C) Page underflow interrupt status of PLE port. Avoid
* unclear error flag, please clear flag when logic reset.
* PAGE_UDF_P6[14] - (W1C) Page underflow interrupt status of AMSDU port. Avoid
* unclear error flag, please clear flag when logic reset.
* RESERVED15[15] - (RO) Reserved bits
* QUEUE_OPER_ERR_P0[16] - (W1C) Queue operation error of PP port. Avoid unclear
* error flag, please clear flag when logic reset.
* QUEUE_OPER_ERR_P1[17] - (W1C) Queue operation error of CPU port. Avoid unclear
* error flag, please clear flag when logic reset.
* QUEUE_OPER_ERR_P2[18] - (W1C) Queue operation error of LMAC DMA port. Avoid
* unclear error flag, please clear flag when logic reset.
* QUEUE_OPER_ERR_P3[19] - (W1C) Queue operation error of MDP port. Avoid unclear
* error flag, please clear flag when logic reset.
* QUEUE_OPER_ERR_P4[20] - (W1C) Queue operation error of SEC port. Avoid unclear
* error flag, please clear flag when logic reset.
* QUEUE_OPER_ERR_P5[21] - (W1C) Queue operation error of PLE port. Avoid unclear
* error flag, please clear flag when logic reset.
* QUEUE_OPER_ERR_P6[22] - (W1C) Queue operation error of AMSDU port. Avoid
* unclear error flag, please clear flag when logic reset.
* RESERVED23[23] - (RO) Reserved bits
* DATA_OPER_ERR_P0[24] - (W1C) Data operation error of PP port. Avoid unclear
* error flag, please clear flag when logic reset.
* DATA_OPER_ERR_P1[25] - (W1C) Data operation error of CPU port. Avoid unclear
* error flag, please clear flag when logic reset.
* DATA_OPER_ERR_P2[26] - (W1C) Data operation error of LMAC DMA port. Avoid
* unclear error flag, please clear flag when logic reset.
* DATA_OPER_ERR_P3[27] - (W1C) Data operation error of MDP port. Avoid unclear
* error flag, please clear flag when logic reset.
* DATA_OPER_ERR_P4[28] - (W1C) Data operation error of SEC port. Avoid unclear
* error flag, please clear flag when logic reset.
* DATA_OPER_ERR_P5[29] - (W1C) Data operation error of PLE port. Avoid unclear
* error flag, please clear flag when logic reset.
* DATA_OPER_ERR_P6[30] - (W1C) Data operation error of AMSDU port. Avoid unclear
* error flag, please clear flag when logic reset.
* RESERVED31[31] - (RO) Reserved bits
*/
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P6_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P6_MASK \
	0x40000000 /* DATA_OPER_ERR_P6[30] */
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P6_SHFT        30
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P5_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P5_MASK \
	0x20000000 /* DATA_OPER_ERR_P5[29] */
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P5_SHFT        29
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P4_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P4_MASK \
	0x10000000 /* DATA_OPER_ERR_P4[28] */
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P4_SHFT        28
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P3_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P3_MASK \
	0x08000000 /* DATA_OPER_ERR_P3[27] */
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P3_SHFT        27
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P2_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P2_MASK \
	0x04000000 /* DATA_OPER_ERR_P2[26] */
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P2_SHFT        26
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P1_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P1_MASK \
	0x02000000 /* DATA_OPER_ERR_P1[25] */
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P1_SHFT        25
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P0_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P0_MASK \
	0x01000000 /* DATA_OPER_ERR_P0[24] */
#define WF_PSE_TOP_INT_N9_ERR_STS_DATA_OPER_ERR_P0_SHFT        24
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P6_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P6_MASK \
	0x00400000 /* QUEUE_OPER_ERR_P6[22] */
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P6_SHFT       22
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P5_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P5_MASK \
	0x00200000 /* QUEUE_OPER_ERR_P5[21] */
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P5_SHFT       21
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P4_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P4_MASK \
	0x00100000 /* QUEUE_OPER_ERR_P4[20] */
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P4_SHFT       20
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P3_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P3_MASK \
	0x00080000 /* QUEUE_OPER_ERR_P3[19] */
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P3_SHFT       19
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P2_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P2_MASK \
	0x00040000 /* QUEUE_OPER_ERR_P2[18] */
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P2_SHFT       18
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P1_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P1_MASK \
	0x00020000 /* QUEUE_OPER_ERR_P1[17] */
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P1_SHFT       17
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P0_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P0_MASK \
	0x00010000 /* QUEUE_OPER_ERR_P0[16] */
#define WF_PSE_TOP_INT_N9_ERR_STS_QUEUE_OPER_ERR_P0_SHFT       16
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P6_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P6_MASK \
	0x00004000 /* PAGE_UDF_P6[14] */
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P6_SHFT             14
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P5_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P5_MASK \
	0x00002000 /* PAGE_UDF_P5[13] */
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P5_SHFT             13
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P4_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P4_MASK \
	0x00001000 /* PAGE_UDF_P4[12] */
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P4_SHFT             12
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P3_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P3_MASK \
	0x00000800 /* PAGE_UDF_P3[11] */
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P3_SHFT             11
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P2_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P2_MASK \
	0x00000400 /* PAGE_UDF_P2[10] */
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P2_SHFT             10
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P1_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P1_MASK \
	0x00000200 /* PAGE_UDF_P1[9] */
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P1_SHFT             9
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P0_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P0_MASK \
	0x00000100 /* PAGE_UDF_P0[8] */
#define WF_PSE_TOP_INT_N9_ERR_STS_PAGE_UDF_P0_SHFT             8
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P6_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P6_MASK \
	0x00000040 /* Q_CMD_ERR_P6[6] */
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P6_SHFT            6
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P5_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P5_MASK \
	0x00000020 /* Q_CMD_ERR_P5[5] */
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P5_SHFT            5
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P4_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P4_MASK \
	0x00000010 /* Q_CMD_ERR_P4[4] */
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P4_SHFT            4
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P3_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P3_MASK \
	0x00000008 /* Q_CMD_ERR_P3[3] */
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P3_SHFT            3
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P2_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P2_MASK \
	0x00000004 /* Q_CMD_ERR_P2[2] */
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P2_SHFT            2
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P1_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P1_MASK \
	0x00000002 /* Q_CMD_ERR_P1[1] */
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P1_SHFT            1
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P0_ADDR \
	WF_PSE_TOP_INT_N9_ERR_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P0_MASK \
	0x00000001 /* Q_CMD_ERR_P0[0] */
#define WF_PSE_TOP_INT_N9_ERR_STS_Q_CMD_ERR_P0_SHFT            0

/*
* ---INT_N9_ERR1_STS (0x820C8000 + 0x38)---
* WDT_ERR_P0[0] - (W1C) Port state watch dog timeout error interrupt status of
* PP port. Avoid unclear error flag, please clear flag when logic reset.
* WDT_ERR_P1[1] - (W1C) Port state watch dog timeout error interrupt status of
* CPU port. Avoid unclear error flag, please clear flag when logic reset.
* WDT_ERR_P2[2] - (W1C) Port state watch dog timeout error interrupt status of
* LMAC DMA port. Avoid unclear error flag, please clear flag when logic reset.
* WDT_ERR_P3[3] - (W1C) Port state watch dog timeout error interrupt status of
* MDP port. Avoid unclear error flag, please clear flag when logic reset.
* WDT_ERR_P4[4] - (W1C) Port state watch dog timeout error interrupt status of
* SEC port. Avoid unclear error flag, please clear flag when logic reset.
* WDT_ERR_P5[5] - (W1C) Port state watch dog timeout error interrupt status of
* PLE port. Avoid unclear error flag, please clear flag when logic reset.
* WDT_ERR_P6[6] - (W1C) Port state watch dog timeout error interrupt status of
* AMSDU port. Avoid unclear error flag, please clear flag when logic reset.
* RESERVED7[7] - (RO) Reserved bits
* FL_HANG_ERR[8] - (W1C) Frame link FSM hang error interrupt. Avoid unclear
* error flag, please clear flag when logic reset.
* PL_HANG_ERR[9] - (W1C) Page link FSM hang error interrupt. Avoid unclear error
* flag, please clear flag when logic reset.
* DOUBLE_RLS_ERR[10] - (W1C) Double release error interrupt. Avoid unclear error
* flag, please clear flag when logic reset.
* FREE_HEAD_TAIL_ERR[11] - (W1C) Free head/tail error interrupt. Avoid unclear
* error flag, please clear flag when logic reset.
* FL_QSTRUCT_ERR[12] - (W1C) Frame Link queue NULL error interrupt. Avoid
* unclear error flag, please clear flag when logic reset.
* RESERVED13[31..13] - (RO) Reserved bits
*/
#define WF_PSE_TOP_INT_N9_ERR1_STS_FL_QSTRUCT_ERR_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_FL_QSTRUCT_ERR_MASK \
	0x00001000 /* FL_QSTRUCT_ERR[12] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_FL_QSTRUCT_ERR_SHFT         12
#define WF_PSE_TOP_INT_N9_ERR1_STS_FREE_HEAD_TAIL_ERR_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_FREE_HEAD_TAIL_ERR_MASK \
	0x00000800 /* FREE_HEAD_TAIL_ERR[11] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_FREE_HEAD_TAIL_ERR_SHFT     11
#define WF_PSE_TOP_INT_N9_ERR1_STS_DOUBLE_RLS_ERR_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_DOUBLE_RLS_ERR_MASK \
	0x00000400 /* DOUBLE_RLS_ERR[10] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_DOUBLE_RLS_ERR_SHFT         10
#define WF_PSE_TOP_INT_N9_ERR1_STS_PL_HANG_ERR_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_PL_HANG_ERR_MASK \
	0x00000200 /* PL_HANG_ERR[9] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_PL_HANG_ERR_SHFT            9
#define WF_PSE_TOP_INT_N9_ERR1_STS_FL_HANG_ERR_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_FL_HANG_ERR_MASK \
	0x00000100 /* FL_HANG_ERR[8] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_FL_HANG_ERR_SHFT            8
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P6_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P6_MASK \
	0x00000040 /* WDT_ERR_P6[6] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P6_SHFT             6
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P5_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P5_MASK \
	0x00000020 /* WDT_ERR_P5[5] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P5_SHFT             5
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P4_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P4_MASK \
	0x00000010 /* WDT_ERR_P4[4] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P4_SHFT             4
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P3_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P3_MASK \
	0x00000008 /* WDT_ERR_P3[3] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P3_SHFT             3
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P2_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P2_MASK \
	0x00000004 /* WDT_ERR_P2[2] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P2_SHFT             2
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P1_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P1_MASK \
	0x00000002 /* WDT_ERR_P1[1] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P1_SHFT             1
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P0_ADDR \
	WF_PSE_TOP_INT_N9_ERR1_STS_ADDR
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P0_MASK \
	0x00000001 /* WDT_ERR_P0[0] */
#define WF_PSE_TOP_INT_N9_ERR1_STS_WDT_ERR_P0_SHFT             0

/*
* ---QUEUE_EMPTY (0x820C8000 + 0xB0)---
* CPU_Q0_EMPTY[0] - (RO) CPU queue 0 empty status
* CPU_Q1_EMPTY[1] - (RO) CPU queue 1 empty status
* CPU_Q2_EMPTY[2] - (RO) CPU queue 2 empty status
* CPU_Q3_EMPTY[3] - (RO) CPU queue 3 empty status
* RESERVED4[7..4] - (RO) Reserved bits
* HIF_0_EMPTY[8] - (RO) HIF queue 0 empty status
* HIF_1_EMPTY[9] - (RO) HIF queue 1 empty status
* HIF_2_EMPTY[10] - (RO) HIF queue 2 empty status
* HIF_3_EMPTY[11] - (RO) HIF queue 3 empty status
* HIF_4_EMPTY[12] - (RO) HIF queue 4 empty status
* HIF_5_EMPTY[13] - (RO) HIF queue 5 empty status
* RESERVED14[15..14] - (RO) Reserved bits
* LMAC_TX_QUEUE_EMPTY[16] - (RO) LMAC TX queue empty status
* MDP_TX_QUEUE_EMPTY[17] - (RO) MDP TX queue empty status
* MDP_RX_QUEUE_EMPTY[18] - (RO) MDP RX queue empty status
* SEC_TX_QUEUE_EMPTY[19] - (RO) SEC TX queue empty status
* SEC_RX_QUEUE_EMPTY[20] - (RO) SEC RX queue empty status
* SFD_PARK_QUEUE_EMPTY[21] - (RO) SFD PARK queue empty status
* MDP_TXIOC_QUEUE_EMPTY[22] - (RO) MDP TXIOC queue empty status
* MDP_RXIOC_QUEUE_EMPTY[23] - (RO) MDP RXIOC queue empty status
* MDP_TX1_QUEUE_EMPTY[24] - (RO) MDP TX queue empty status for Band 1
* SEC_TX1_QUEUE_EMPTY[25] - (RO) SEC TX queue empty status for Band 1
* MDP_TXIOC1_QUEUE_EMPTY[26] - (RO) MDP TXIOC queue empty status for Band 1
* MDP_RXIOC1_QUEUE_EMPTY[27] - (RO) MDP RXIOC queue empty status for Band 1
* CPU_Q4_EMPTY[28] - (RO) CPU queue 4 empty status
* RESERVED29[30..29] - (RO) Reserved bits
* RLS_Q_EMTPY[31] - (RO) Release queue empty status
*/
#define WF_PSE_TOP_QUEUE_EMPTY_RLS_Q_EMTPY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_RLS_Q_EMTPY_MASK \
	0x80000000 /* RLS_Q_EMTPY[31] */
#define WF_PSE_TOP_QUEUE_EMPTY_RLS_Q_EMTPY_SHFT                31
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q4_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q4_EMPTY_MASK \
	0x10000000 /* CPU_Q4_EMPTY[28] */
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q4_EMPTY_SHFT               28
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC1_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC1_QUEUE_EMPTY_MASK \
	0x08000000 /* MDP_RXIOC1_QUEUE_EMPTY[27] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC1_QUEUE_EMPTY_SHFT     27
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC1_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC1_QUEUE_EMPTY_MASK \
	0x04000000 /* MDP_TXIOC1_QUEUE_EMPTY[26] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC1_QUEUE_EMPTY_SHFT     26
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX1_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX1_QUEUE_EMPTY_MASK \
	0x02000000 /* SEC_TX1_QUEUE_EMPTY[25] */
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX1_QUEUE_EMPTY_SHFT        25
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX1_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX1_QUEUE_EMPTY_MASK \
	0x01000000 /* MDP_TX1_QUEUE_EMPTY[24] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX1_QUEUE_EMPTY_SHFT        24
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC_QUEUE_EMPTY_MASK \
	0x00800000 /* MDP_RXIOC_QUEUE_EMPTY[23] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC_QUEUE_EMPTY_SHFT      23
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC_QUEUE_EMPTY_MASK \
	0x00400000 /* MDP_TXIOC_QUEUE_EMPTY[22] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC_QUEUE_EMPTY_SHFT      22
#define WF_PSE_TOP_QUEUE_EMPTY_SFD_PARK_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_SFD_PARK_QUEUE_EMPTY_MASK \
	0x00200000 /* SFD_PARK_QUEUE_EMPTY[21] */
#define WF_PSE_TOP_QUEUE_EMPTY_SFD_PARK_QUEUE_EMPTY_SHFT       21
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_RX_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_RX_QUEUE_EMPTY_MASK \
	0x00100000 /* SEC_RX_QUEUE_EMPTY[20] */
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_RX_QUEUE_EMPTY_SHFT         20
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX_QUEUE_EMPTY_MASK \
	0x00080000 /* SEC_TX_QUEUE_EMPTY[19] */
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX_QUEUE_EMPTY_SHFT         19
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RX_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RX_QUEUE_EMPTY_MASK \
	0x00040000 /* MDP_RX_QUEUE_EMPTY[18] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RX_QUEUE_EMPTY_SHFT         18
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX_QUEUE_EMPTY_MASK \
	0x00020000 /* MDP_TX_QUEUE_EMPTY[17] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX_QUEUE_EMPTY_SHFT         17
#define WF_PSE_TOP_QUEUE_EMPTY_LMAC_TX_QUEUE_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_LMAC_TX_QUEUE_EMPTY_MASK \
	0x00010000 /* LMAC_TX_QUEUE_EMPTY[16] */
#define WF_PSE_TOP_QUEUE_EMPTY_LMAC_TX_QUEUE_EMPTY_SHFT        16
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_5_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_5_EMPTY_MASK \
	0x00002000 /* HIF_5_EMPTY[13] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_5_EMPTY_SHFT                13
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_4_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_4_EMPTY_MASK \
	0x00001000 /* HIF_4_EMPTY[12] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_4_EMPTY_SHFT                12
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_3_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_3_EMPTY_MASK \
	0x00000800 /* HIF_3_EMPTY[11] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_3_EMPTY_SHFT                11
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_2_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_2_EMPTY_MASK \
	0x00000400 /* HIF_2_EMPTY[10] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_2_EMPTY_SHFT                10
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_1_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_1_EMPTY_MASK \
	0x00000200 /* HIF_1_EMPTY[9] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_1_EMPTY_SHFT                9
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_0_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_0_EMPTY_MASK \
	0x00000100 /* HIF_0_EMPTY[8] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_0_EMPTY_SHFT                8
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q3_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q3_EMPTY_MASK \
	0x00000008 /* CPU_Q3_EMPTY[3] */
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q3_EMPTY_SHFT               3
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q2_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q2_EMPTY_MASK \
	0x00000004 /* CPU_Q2_EMPTY[2] */
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q2_EMPTY_SHFT               2
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q1_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q1_EMPTY_MASK \
	0x00000002 /* CPU_Q1_EMPTY[1] */
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q1_EMPTY_SHFT               1
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q0_EMPTY_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q0_EMPTY_MASK \
	0x00000001 /* CPU_Q0_EMPTY[0] */
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q0_EMPTY_SHFT               0

/*
* ---QUEUE_EMPTY_MASK (0x820C8000 + 0xB4)---
* RESERVED0[7..0] - (RO) Reserved bits
* HIF_0_EMPTY_MASK[8] - (RW) Mask control of HIF queue 0 empty status
* HIF_1_EMPTY_MASK[9] - (RW) Mask control of HIF queue 1 empty status
* HIF_2_EMPTY_MASK[10] - (RW) Mask control of HIF queue 2 empty status
* HIF_3_EMPTY_MASK[11] - (RW) Mask control of HIF queue 3 empty status
* HIF_4_EMPTY_MASK[12] - (RW) Mask control of HIF queue 4 empty status
* HIF_5_EMPTY_MASK[13] - (RW) Mask control of HIF queue 5 empty status
* HIF_6_EMPTY_MASK[14] - (RW) Mask control of HIF queue 6 empty status
* HIF_7_EMPTY_MASK[15] - (RW) Mask control of HIF queue 7 empty status
* HIF_8_EMPTY_MASK[16] - (RW) Mask control of HIF queue 8 empty status
* HIF_9_EMPTY_MASK[17] - (RW) Mask control of HIF queue 9 empty status
* HIF_10_EMPTY_MASK[18] - (RW) Mask control of HIF queue 10 empty status
* HIF_11_EMPTY_MASK[19] - (RW) Mask control of HIF queue 11 empty status
* RESERVED20[31..20] - (RO) Reserved bits
*/
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_11_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_11_EMPTY_MASK_MASK \
	0x00080000 /* HIF_11_EMPTY_MASK[19] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_11_EMPTY_MASK_SHFT     19
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_10_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_10_EMPTY_MASK_MASK \
	0x00040000 /* HIF_10_EMPTY_MASK[18] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_10_EMPTY_MASK_SHFT     18
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_9_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_9_EMPTY_MASK_MASK \
	0x00020000 /* HIF_9_EMPTY_MASK[17] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_9_EMPTY_MASK_SHFT      17
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_8_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_8_EMPTY_MASK_MASK \
	0x00010000 /* HIF_8_EMPTY_MASK[16] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_8_EMPTY_MASK_SHFT      16
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_7_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_7_EMPTY_MASK_MASK \
	0x00008000 /* HIF_7_EMPTY_MASK[15] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_7_EMPTY_MASK_SHFT      15
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_6_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_6_EMPTY_MASK_MASK \
	0x00004000 /* HIF_6_EMPTY_MASK[14] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_6_EMPTY_MASK_SHFT      14
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_5_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_5_EMPTY_MASK_MASK \
	0x00002000 /* HIF_5_EMPTY_MASK[13] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_5_EMPTY_MASK_SHFT      13
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_4_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_4_EMPTY_MASK_MASK \
	0x00001000 /* HIF_4_EMPTY_MASK[12] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_4_EMPTY_MASK_SHFT      12
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_3_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_3_EMPTY_MASK_MASK \
	0x00000800 /* HIF_3_EMPTY_MASK[11] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_3_EMPTY_MASK_SHFT      11
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_2_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_2_EMPTY_MASK_MASK \
	0x00000400 /* HIF_2_EMPTY_MASK[10] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_2_EMPTY_MASK_SHFT      10
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_1_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_1_EMPTY_MASK_MASK \
	0x00000200 /* HIF_1_EMPTY_MASK[9] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_1_EMPTY_MASK_SHFT      9
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_0_EMPTY_MASK_ADDR \
	WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_0_EMPTY_MASK_MASK \
	0x00000100 /* HIF_0_EMPTY_MASK[8] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_0_EMPTY_MASK_SHFT      8

/*
* ---PSE_LP_CTRL (0x820C8000 + 0xB8)---
* RESERVED0[6..0] - (RO) Reserved bits
* PSE_LP_WAKEUP[7] - (RW) PSE Low Power Wakeup control
* PSE2HIF_PSSS_EN[8] - (RW) PSE Access HIF port control
* MCU_COALEASCE[9] - (RW) Coalease CPU ENQ and ENQ_DLY for HIF port
* RESERVED10[31..10] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_LP_CTRL_MCU_COALEASCE_ADDR \
	WF_PSE_TOP_PSE_LP_CTRL_ADDR
#define WF_PSE_TOP_PSE_LP_CTRL_MCU_COALEASCE_MASK \
	0x00000200 /* MCU_COALEASCE[9] */
#define WF_PSE_TOP_PSE_LP_CTRL_MCU_COALEASCE_SHFT              9
#define WF_PSE_TOP_PSE_LP_CTRL_PSE2HIF_PSSS_EN_ADDR \
	WF_PSE_TOP_PSE_LP_CTRL_ADDR
#define WF_PSE_TOP_PSE_LP_CTRL_PSE2HIF_PSSS_EN_MASK \
	0x00000100 /* PSE2HIF_PSSS_EN[8] */
#define WF_PSE_TOP_PSE_LP_CTRL_PSE2HIF_PSSS_EN_SHFT            8
#define WF_PSE_TOP_PSE_LP_CTRL_PSE_LP_WAKEUP_ADDR \
	WF_PSE_TOP_PSE_LP_CTRL_ADDR
#define WF_PSE_TOP_PSE_LP_CTRL_PSE_LP_WAKEUP_MASK \
	0x00000080 /* PSE_LP_WAKEUP[7] */
#define WF_PSE_TOP_PSE_LP_CTRL_PSE_LP_WAKEUP_SHFT              7

/*
* ---TO_N9_INT (0x820C8000 + 0xF0)---
* CR4_CMD[30..0] - (RW) Command for N9
* TOGGLE[31] - (RW) When this bit is toggled, HW will send interrupt to N9.
*/
#define WF_PSE_TOP_TO_N9_INT_TOGGLE_ADDR \
	WF_PSE_TOP_TO_N9_INT_ADDR
#define WF_PSE_TOP_TO_N9_INT_TOGGLE_MASK \
	0x80000000 /* TOGGLE[31] */
#define WF_PSE_TOP_TO_N9_INT_TOGGLE_SHFT                       31
#define WF_PSE_TOP_TO_N9_INT_CR4_CMD_ADDR \
	WF_PSE_TOP_TO_N9_INT_ADDR
#define WF_PSE_TOP_TO_N9_INT_CR4_CMD_MASK \
	0x7FFFFFFF /* CR4_CMD[30..0] */
#define WF_PSE_TOP_TO_N9_INT_CR4_CMD_SHFT                      0

/*
* ---GROUP_REFILL_CTRL (0x820C8000 + 0x108)---
* GROUP_REFILL_PRI[12..0] - (RW) group refill priority control
* RESERVED13[15..13] - (RO) Reserved bits
* DIS_GROUP_REFILL[28..16] - (RW) group quota refill enable control
* RESERVED29[31..29] - (RO) Reserved bits
*/
#define WF_PSE_TOP_GROUP_REFILL_CTRL_DIS_GROUP_REFILL_ADDR \
	WF_PSE_TOP_GROUP_REFILL_CTRL_ADDR
#define WF_PSE_TOP_GROUP_REFILL_CTRL_DIS_GROUP_REFILL_MASK \
	0x1FFF0000 /* DIS_GROUP_REFILL[28..16] */
#define WF_PSE_TOP_GROUP_REFILL_CTRL_DIS_GROUP_REFILL_SHFT     16
#define WF_PSE_TOP_GROUP_REFILL_CTRL_GROUP_REFILL_PRI_ADDR \
	WF_PSE_TOP_GROUP_REFILL_CTRL_ADDR
#define WF_PSE_TOP_GROUP_REFILL_CTRL_GROUP_REFILL_PRI_MASK \
	0x00001FFF /* GROUP_REFILL_PRI[12..0] */
#define WF_PSE_TOP_GROUP_REFILL_CTRL_GROUP_REFILL_PRI_SHFT     0

/*
* ---FREEPG_START_END (0x820C8000 + 0x10C)---
* FREEPG_START[11..0] - (RW) Start page setting of free pages
* RESERVED12[15..12] - (RO) Reserved bits
* FREEPG_END[27..16] - (RW) End page setting of free page
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_FREEPG_START_END_FREEPG_END_ADDR \
	WF_PSE_TOP_FREEPG_START_END_ADDR
#define WF_PSE_TOP_FREEPG_START_END_FREEPG_END_MASK \
	0x0FFF0000 /* FREEPG_END[27..16] */
#define WF_PSE_TOP_FREEPG_START_END_FREEPG_END_SHFT            16
#define WF_PSE_TOP_FREEPG_START_END_FREEPG_START_ADDR \
	WF_PSE_TOP_FREEPG_START_END_ADDR
#define WF_PSE_TOP_FREEPG_START_END_FREEPG_START_MASK \
	0x00000FFF /* FREEPG_START[11..0] */
#define WF_PSE_TOP_FREEPG_START_END_FREEPG_START_SHFT          0

/*
* ---PG_HIF0_GROUP (0x820C8000 + 0x110)---
* HIF0_MIN_QUOTA[11..0] - (RW) Min. quota of HIF 0 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED12[15..12] - (RO) Reserved bits
* HIF0_MAX_QUOTA[27..16] - (RW) Max. quota of HIF 0 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MAX_QUOTA_ADDR \
	WF_PSE_TOP_PG_HIF0_GROUP_ADDR
#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MAX_QUOTA_MASK \
	0x0FFF0000 /* HIF0_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MAX_QUOTA_SHFT           16
#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MIN_QUOTA_ADDR \
	WF_PSE_TOP_PG_HIF0_GROUP_ADDR
#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MIN_QUOTA_MASK \
	0x00000FFF /* HIF0_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MIN_QUOTA_SHFT           0

/*
* ---PG_HIF1_GROUP (0x820C8000 + 0x114)---
* HIF1_MIN_QUOTA[11..0] - (RW) Min. quota of HIF 1 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED12[15..12] - (RO) Reserved bits
* HIF1_MAX_QUOTA[27..16] - (RW) Max. quota of HIF 1 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MAX_QUOTA_ADDR \
	WF_PSE_TOP_PG_HIF1_GROUP_ADDR
#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MAX_QUOTA_MASK \
	0x0FFF0000 /* HIF1_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MAX_QUOTA_SHFT           16
#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MIN_QUOTA_ADDR \
	WF_PSE_TOP_PG_HIF1_GROUP_ADDR
#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MIN_QUOTA_MASK \
	0x00000FFF /* HIF1_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MIN_QUOTA_SHFT           0

/*
* ---PG_CPU_GROUP (0x820C8000 + 0x118)---
* CPU_MIN_QUOTA[11..0] - (RW) Min. quota of CPU group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED12[15..12] - (RO) Reserved bits
* CPU_MAX_QUOTA[27..16] - (RW) Max. quota of CPU group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MAX_QUOTA_ADDR \
	WF_PSE_TOP_PG_CPU_GROUP_ADDR
#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MAX_QUOTA_MASK \
	0x0FFF0000 /* CPU_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MAX_QUOTA_SHFT             16
#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MIN_QUOTA_ADDR \
	WF_PSE_TOP_PG_CPU_GROUP_ADDR
#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MIN_QUOTA_MASK \
	0x00000FFF /* CPU_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MIN_QUOTA_SHFT             0

/*
* ---PG_PLE_GROUP (0x820C8000 + 0x11C)---
* PLE_MIN_QUOTA[11..0] - (RW) Min. quota of PLE group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED12[15..12] - (RO) Reserved bits
* PLE_MAX_QUOTA[27..16] - (RW) Max. quota of PLE group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MAX_QUOTA_ADDR \
	WF_PSE_TOP_PG_PLE_GROUP_ADDR
#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MAX_QUOTA_MASK \
	0x0FFF0000 /* PLE_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MAX_QUOTA_SHFT             16
#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MIN_QUOTA_ADDR \
	WF_PSE_TOP_PG_PLE_GROUP_ADDR
#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MIN_QUOTA_MASK \
	0x00000FFF /* PLE_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MIN_QUOTA_SHFT             0

/*
* ---PG_LMAC0_GROUP (0x820C8000 + 0x124)---
* LMAC0_MIN_QUOTA[11..0] - (RW) Min. quota of LMAC 0 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED12[15..12] - (RO) Reserved bits
* LMAC0_MAX_QUOTA[27..16] - (RW) Max. quota of LMAC 0 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MAX_QUOTA_ADDR \
	WF_PSE_TOP_PG_LMAC0_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MAX_QUOTA_MASK \
	0x0FFF0000 /* LMAC0_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MAX_QUOTA_SHFT         16
#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MIN_QUOTA_ADDR \
	WF_PSE_TOP_PG_LMAC0_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MIN_QUOTA_MASK \
	0x00000FFF /* LMAC0_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MIN_QUOTA_SHFT         0

/*
* ---PG_LMAC1_GROUP (0x820C8000 + 0x128)---
* LMAC1_MIN_QUOTA[11..0] - (RW) Min. quota of LMAC 1 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED12[15..12] - (RO) Reserved bits
* LMAC1_MAX_QUOTA[27..16] - (RW) Max. quota of LMAC 1 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MAX_QUOTA_ADDR \
	WF_PSE_TOP_PG_LMAC1_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MAX_QUOTA_MASK \
	0x0FFF0000 /* LMAC1_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MAX_QUOTA_SHFT         16
#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MIN_QUOTA_ADDR \
	WF_PSE_TOP_PG_LMAC1_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MIN_QUOTA_MASK \
	0x00000FFF /* LMAC1_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MIN_QUOTA_SHFT         0

/*
* ---PG_LMAC2_GROUP (0x820C8000 + 0x12C)---
* LMAC2_MIN_QUOTA[11..0] - (RW) Min. quota of LMAC 2 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED12[15..12] - (RO) Reserved bits
* LMAC2_MAX_QUOTA[27..16] - (RW) Max. quota of LMAC 2 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MAX_QUOTA_ADDR \
	WF_PSE_TOP_PG_LMAC2_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MAX_QUOTA_MASK \
	0x0FFF0000 /* LMAC2_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MAX_QUOTA_SHFT         16
#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MIN_QUOTA_ADDR \
	WF_PSE_TOP_PG_LMAC2_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MIN_QUOTA_MASK \
	0x00000FFF /* LMAC2_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MIN_QUOTA_SHFT         0

/*
* ---PG_MDP_GROUP (0x820C8000 + 0x134)---
* MDP_MIN_QUOTA[11..0] - (RW) Min. quota of MDP group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED12[15..12] - (RO) Reserved bits
* MDP_MAX_QUOTA[27..16] - (RW) Max. quota of MDP group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MAX_QUOTA_ADDR \
	WF_PSE_TOP_PG_MDP_GROUP_ADDR
#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MAX_QUOTA_MASK \
	0x0FFF0000 /* MDP_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MAX_QUOTA_SHFT             16
#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MIN_QUOTA_ADDR \
	WF_PSE_TOP_PG_MDP_GROUP_ADDR
#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MIN_QUOTA_MASK \
	0x00000FFF /* MDP_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MIN_QUOTA_SHFT             0

/*
* ---PG_MDP1_GROUP (0x820C8000 + 0x138)---
* MDP1_MIN_QUOTA[11..0] - (RW) Min. quota of MDP1 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED12[15..12] - (RO) Reserved bits
* MDP1_MAX_QUOTA[27..16] - (RW) Max. quota of MDP1 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PG_MDP1_GROUP_MDP1_MAX_QUOTA_ADDR \
	WF_PSE_TOP_PG_MDP1_GROUP_ADDR
#define WF_PSE_TOP_PG_MDP1_GROUP_MDP1_MAX_QUOTA_MASK \
	0x0FFF0000 /* MDP1_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_MDP1_GROUP_MDP1_MAX_QUOTA_SHFT           16
#define WF_PSE_TOP_PG_MDP1_GROUP_MDP1_MIN_QUOTA_ADDR \
	WF_PSE_TOP_PG_MDP1_GROUP_ADDR
#define WF_PSE_TOP_PG_MDP1_GROUP_MDP1_MIN_QUOTA_MASK \
	0x00000FFF /* MDP1_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_MDP1_GROUP_MDP1_MIN_QUOTA_SHFT           0

/*
* ---PG_HIF2_GROUP (0x820C8000 + 0x140)---
* HIF2_MIN_QUOTA[11..0] - (RW) Min. quota of HIF 2 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED12[15..12] - (RO) Reserved bits
* HIF2_MAX_QUOTA[27..16] - (RW) Max. quota of HIF 2 group
* Set up the quota before releasing PSE logic reset; it should not be changed
* after logic reset is released.
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PG_HIF2_GROUP_HIF2_MAX_QUOTA_ADDR \
	WF_PSE_TOP_PG_HIF2_GROUP_ADDR
#define WF_PSE_TOP_PG_HIF2_GROUP_HIF2_MAX_QUOTA_MASK \
	0x0FFF0000 /* HIF2_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_HIF2_GROUP_HIF2_MAX_QUOTA_SHFT           16
#define WF_PSE_TOP_PG_HIF2_GROUP_HIF2_MIN_QUOTA_ADDR \
	WF_PSE_TOP_PG_HIF2_GROUP_ADDR
#define WF_PSE_TOP_PG_HIF2_GROUP_HIF2_MIN_QUOTA_MASK \
	0x00000FFF /* HIF2_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_HIF2_GROUP_HIF2_MIN_QUOTA_SHFT           0

/*
* ---HIF0_PG_INFO (0x820C8000 + 0x150)---
* HIF0_RSV_CNT[11..0] - (RO) Reserved pages of HIF 0 group
* RESERVED12[15..12] - (RO) Reserved bits
* HIF0_SRC_CNT[27..16] - (RO) Used pages of HIF 0 group
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_SRC_CNT_ADDR \
	WF_PSE_TOP_HIF0_PG_INFO_ADDR
#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_SRC_CNT_MASK \
	0x0FFF0000 /* HIF0_SRC_CNT[27..16] */
#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_SRC_CNT_SHFT              16
#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_RSV_CNT_ADDR \
	WF_PSE_TOP_HIF0_PG_INFO_ADDR
#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_RSV_CNT_MASK \
	0x00000FFF /* HIF0_RSV_CNT[11..0] */
#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_RSV_CNT_SHFT              0

/*
* ---HIF1_PG_INFO (0x820C8000 + 0x154)---
* HIF1_RSV_CNT[11..0] - (RO) Reserved pages of HIF 1 group
* RESERVED12[15..12] - (RO) Reserved bits
* HIF1_SRC_CNT[27..16] - (RO) Used pages of HIF 1 group
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_SRC_CNT_ADDR \
	WF_PSE_TOP_HIF1_PG_INFO_ADDR
#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_SRC_CNT_MASK \
	0x0FFF0000 /* HIF1_SRC_CNT[27..16] */
#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_SRC_CNT_SHFT              16
#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_RSV_CNT_ADDR \
	WF_PSE_TOP_HIF1_PG_INFO_ADDR
#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_RSV_CNT_MASK \
	0x00000FFF /* HIF1_RSV_CNT[11..0] */
#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_RSV_CNT_SHFT              0

/*
* ---CPU_PG_INFO (0x820C8000 + 0x158)---
* CPU_RSV_CNT[11..0] - (RO) Reserved pages of CPU group
* RESERVED12[15..12] - (RO) Reserved bits
* CPU_SRC_CNT[27..16] - (RO) Used pages of CPU group
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_CPU_PG_INFO_CPU_SRC_CNT_ADDR \
	WF_PSE_TOP_CPU_PG_INFO_ADDR
#define WF_PSE_TOP_CPU_PG_INFO_CPU_SRC_CNT_MASK \
	0x0FFF0000 /* CPU_SRC_CNT[27..16] */
#define WF_PSE_TOP_CPU_PG_INFO_CPU_SRC_CNT_SHFT                16
#define WF_PSE_TOP_CPU_PG_INFO_CPU_RSV_CNT_ADDR \
	WF_PSE_TOP_CPU_PG_INFO_ADDR
#define WF_PSE_TOP_CPU_PG_INFO_CPU_RSV_CNT_MASK \
	0x00000FFF /* CPU_RSV_CNT[11..0] */
#define WF_PSE_TOP_CPU_PG_INFO_CPU_RSV_CNT_SHFT                0

/*
* ---PLE_PG_INFO (0x820C8000 + 0x15C)---
* PLE_RSV_CNT[11..0] - (RO) Reserved pages of PLE group
* RESERVED12[15..12] - (RO) Reserved bits
* PLE_SRC_CNT[27..16] - (RO) Used pages of PLE group
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PLE_PG_INFO_PLE_SRC_CNT_ADDR \
	WF_PSE_TOP_PLE_PG_INFO_ADDR
#define WF_PSE_TOP_PLE_PG_INFO_PLE_SRC_CNT_MASK \
	0x0FFF0000 /* PLE_SRC_CNT[27..16] */
#define WF_PSE_TOP_PLE_PG_INFO_PLE_SRC_CNT_SHFT                16
#define WF_PSE_TOP_PLE_PG_INFO_PLE_RSV_CNT_ADDR \
	WF_PSE_TOP_PLE_PG_INFO_ADDR
#define WF_PSE_TOP_PLE_PG_INFO_PLE_RSV_CNT_MASK \
	0x00000FFF /* PLE_RSV_CNT[11..0] */
#define WF_PSE_TOP_PLE_PG_INFO_PLE_RSV_CNT_SHFT                0

/*
* ---LMAC0_PG_INFO (0x820C8000 + 0x164)---
* LMAC0_RSV_CNT[11..0] - (RO) Reserved pages of LMAC 0 group
* RESERVED12[15..12] - (RO) Reserved bits
* LMAC0_SRC_CNT[27..16] - (RO) Used pages of LMAC 0 group
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_SRC_CNT_ADDR \
	WF_PSE_TOP_LMAC0_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_SRC_CNT_MASK \
	0x0FFF0000 /* LMAC0_SRC_CNT[27..16] */
#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_SRC_CNT_SHFT            16
#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_RSV_CNT_ADDR \
	WF_PSE_TOP_LMAC0_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_RSV_CNT_MASK \
	0x00000FFF /* LMAC0_RSV_CNT[11..0] */
#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_RSV_CNT_SHFT            0

/*
* ---LMAC1_PG_INFO (0x820C8000 + 0x168)---
* LMAC1_RSV_CNT[11..0] - (RO) Reserved pages of LMAC 1 group
* RESERVED12[15..12] - (RO) Reserved bits
* LMAC1_SRC_CNT[27..16] - (RO) Used pages of LMAC 1 group
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_SRC_CNT_ADDR \
	WF_PSE_TOP_LMAC1_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_SRC_CNT_MASK \
	0x0FFF0000 /* LMAC1_SRC_CNT[27..16] */
#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_SRC_CNT_SHFT            16
#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_RSV_CNT_ADDR \
	WF_PSE_TOP_LMAC1_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_RSV_CNT_MASK \
	0x00000FFF /* LMAC1_RSV_CNT[11..0] */
#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_RSV_CNT_SHFT            0

/*
* ---LMAC2_PG_INFO (0x820C8000 + 0x16C)---
* LMAC2_RSV_CNT[11..0] - (RO) Reserved pages of LMAC 2 group
* RESERVED12[15..12] - (RO) Reserved bits
* LMAC2_SRC_CNT[27..16] - (RO) Used pages of LMAC 2 group
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_SRC_CNT_ADDR \
	WF_PSE_TOP_LMAC2_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_SRC_CNT_MASK \
	0x0FFF0000 /* LMAC2_SRC_CNT[27..16] */
#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_SRC_CNT_SHFT            16
#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_RSV_CNT_ADDR \
	WF_PSE_TOP_LMAC2_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_RSV_CNT_MASK \
	0x00000FFF /* LMAC2_RSV_CNT[11..0] */
#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_RSV_CNT_SHFT            0

/*
* ---MDP_PG_INFO (0x820C8000 + 0x174)---
* MDP_RSV_CNT[11..0] - (RO) Reserved pages of MDP group
* RESERVED12[15..12] - (RO) Reserved bits
* MDP_SRC_CNT[27..16] - (RO) Used pages of MDP group
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_MDP_PG_INFO_MDP_SRC_CNT_ADDR \
	WF_PSE_TOP_MDP_PG_INFO_ADDR
#define WF_PSE_TOP_MDP_PG_INFO_MDP_SRC_CNT_MASK \
	0x0FFF0000 /* MDP_SRC_CNT[27..16] */
#define WF_PSE_TOP_MDP_PG_INFO_MDP_SRC_CNT_SHFT                16
#define WF_PSE_TOP_MDP_PG_INFO_MDP_RSV_CNT_ADDR \
	WF_PSE_TOP_MDP_PG_INFO_ADDR
#define WF_PSE_TOP_MDP_PG_INFO_MDP_RSV_CNT_MASK \
	0x00000FFF /* MDP_RSV_CNT[11..0] */
#define WF_PSE_TOP_MDP_PG_INFO_MDP_RSV_CNT_SHFT                0

/*
* ---MDP1_PG_INFO (0x820C8000 + 0x178)---
* MDP1_RSV_CNT[11..0] - (RO) Reserved pages of MDP 1 group
* RESERVED12[15..12] - (RO) Reserved bits
* MDP1_SRC_CNT[27..16] - (RO) Used pages of MDP 1 group
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_MDP1_PG_INFO_MDP1_SRC_CNT_ADDR \
	WF_PSE_TOP_MDP1_PG_INFO_ADDR
#define WF_PSE_TOP_MDP1_PG_INFO_MDP1_SRC_CNT_MASK \
	0x0FFF0000 /* MDP1_SRC_CNT[27..16] */
#define WF_PSE_TOP_MDP1_PG_INFO_MDP1_SRC_CNT_SHFT              16
#define WF_PSE_TOP_MDP1_PG_INFO_MDP1_RSV_CNT_ADDR \
	WF_PSE_TOP_MDP1_PG_INFO_ADDR
#define WF_PSE_TOP_MDP1_PG_INFO_MDP1_RSV_CNT_MASK \
	0x00000FFF /* MDP1_RSV_CNT[11..0] */
#define WF_PSE_TOP_MDP1_PG_INFO_MDP1_RSV_CNT_SHFT              0

/*
* ---HIF2_PG_INFO (0x820C8000 + 0x180)---
* HIF2_RSV_CNT[11..0] - (RO) Reserved pages of HIF 2 group
* RESERVED12[15..12] - (RO) Reserved bits
* HIF2_SRC_CNT[27..16] - (RO) Used pages of HIF 2 group
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_HIF2_PG_INFO_HIF2_SRC_CNT_ADDR \
	WF_PSE_TOP_HIF2_PG_INFO_ADDR
#define WF_PSE_TOP_HIF2_PG_INFO_HIF2_SRC_CNT_MASK \
	0x0FFF0000 /* HIF2_SRC_CNT[27..16] */
#define WF_PSE_TOP_HIF2_PG_INFO_HIF2_SRC_CNT_SHFT              16
#define WF_PSE_TOP_HIF2_PG_INFO_HIF2_RSV_CNT_ADDR \
	WF_PSE_TOP_HIF2_PG_INFO_ADDR
#define WF_PSE_TOP_HIF2_PG_INFO_HIF2_RSV_CNT_MASK \
	0x00000FFF /* HIF2_RSV_CNT[11..0] */
#define WF_PSE_TOP_HIF2_PG_INFO_HIF2_RSV_CNT_SHFT              0

/*
* ---FL_QUE_CTRL_0 (0x820C8000 + 0x1B0)---
* Q_BUF_WLANID[9..0] - (RW) Address of queue structure buffer WLANID.
* Q_BUF_PID[11..10] - (RW) Address of queue structure buffer PID
* FL_BUFFER_ADDR[23..12] - (RW) Frame address of read previous frame/next frame
* Q_BUF_QID[30..24] - (RW) Address of queue structure buffer QID
* EXECUTE[31] - (A0) Executes frame link and queue structure buffer read command
*/
#define WF_PSE_TOP_FL_QUE_CTRL_0_EXECUTE_ADDR \
	WF_PSE_TOP_FL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_0_EXECUTE_MASK \
	0x80000000 /* EXECUTE[31] */
#define WF_PSE_TOP_FL_QUE_CTRL_0_EXECUTE_SHFT                  31
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_QID_ADDR \
	WF_PSE_TOP_FL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_QID_MASK \
	0x7F000000 /* Q_BUF_QID[30..24] */
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_QID_SHFT                24
#define WF_PSE_TOP_FL_QUE_CTRL_0_FL_BUFFER_ADDR_ADDR \
	WF_PSE_TOP_FL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_0_FL_BUFFER_ADDR_MASK \
	0x00FFF000 /* FL_BUFFER_ADDR[23..12] */
#define WF_PSE_TOP_FL_QUE_CTRL_0_FL_BUFFER_ADDR_SHFT           12
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_PID_ADDR \
	WF_PSE_TOP_FL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_PID_MASK \
	0x00000C00 /* Q_BUF_PID[11..10] */
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_PID_SHFT                10
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_WLANID_ADDR \
	WF_PSE_TOP_FL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_WLANID_MASK \
	0x000003FF /* Q_BUF_WLANID[9..0] */
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_WLANID_SHFT             0

/*
* ---FL_QUE_CTRL_1 (0x820C8000 + 0x1B4)---
* NEXT_FID[11..0] - (RO) Next frame ID of FL_BUFFER_ADDR
* RESERVED12[15..12] - (RO) Reserved bits
* PREV_FID[27..16] - (RO) Previous frame ID of FL_BUFFER_ADDR
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_FL_QUE_CTRL_1_PREV_FID_ADDR \
	WF_PSE_TOP_FL_QUE_CTRL_1_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_1_PREV_FID_MASK \
	0x0FFF0000 /* PREV_FID[27..16] */
#define WF_PSE_TOP_FL_QUE_CTRL_1_PREV_FID_SHFT                 16
#define WF_PSE_TOP_FL_QUE_CTRL_1_NEXT_FID_ADDR \
	WF_PSE_TOP_FL_QUE_CTRL_1_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_1_NEXT_FID_MASK \
	0x00000FFF /* NEXT_FID[11..0] */
#define WF_PSE_TOP_FL_QUE_CTRL_1_NEXT_FID_SHFT                 0

/*
* ---FL_QUE_CTRL_2 (0x820C8000 + 0x1B8)---
* QUEUE_HEAD_FID[11..0] - (RO) Head frame ID of quest queue setting in
0x01b0[15:0]
* RESERVED12[15..12] - (RO) Reserved bits
* QUEUE_TAIL_FID[27..16] - (RO) Tail frame ID of quest queue setting in
0x01b0[15:0]
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_TAIL_FID_ADDR \
	WF_PSE_TOP_FL_QUE_CTRL_2_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_TAIL_FID_MASK \
	0x0FFF0000 /* QUEUE_TAIL_FID[27..16] */
#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_TAIL_FID_SHFT           16
#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_HEAD_FID_ADDR \
	WF_PSE_TOP_FL_QUE_CTRL_2_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_HEAD_FID_MASK \
	0x00000FFF /* QUEUE_HEAD_FID[11..0] */
#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_HEAD_FID_SHFT           0

/*
* ---FL_QUE_CTRL_3 (0x820C8000 + 0x1BC)---
* QUEUE_PKT_NUM[11..0] - (RO) Total packet number of queue setting in
0x1b0[15:0]
* QUEUE_PAGE_NUM[23..12] - (RO) Total page number of queue setting in
0x1b0[15:0]
* RESERVED24[31..24] - (RO) Reserved bits
*/
#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PAGE_NUM_ADDR \
	WF_PSE_TOP_FL_QUE_CTRL_3_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PAGE_NUM_MASK \
	0x00FFF000 /* QUEUE_PAGE_NUM[23..12] */
#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PAGE_NUM_SHFT           12
#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PKT_NUM_ADDR \
	WF_PSE_TOP_FL_QUE_CTRL_3_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PKT_NUM_MASK \
	0x00000FFF /* QUEUE_PKT_NUM[11..0] */
#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PKT_NUM_SHFT            0

/*
* ---PL_QUE_CTRL_0 (0x820C8000 + 0x1C0)---
* NEXT_PAGE[11..0] - (RO) Next page of the PL_BUFFER_ADDR
* RESERVED12[15..12] - (RO) Reserved bits
* PL_BUFFER_ADDR[27..16] - (RW) Page address of read next page
* RESERVED28[30..28] - (RO) Reserved bits
* EXECUTE[31] - (A0) Executes page link buffer read command
*/
#define WF_PSE_TOP_PL_QUE_CTRL_0_EXECUTE_ADDR \
	WF_PSE_TOP_PL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_PL_QUE_CTRL_0_EXECUTE_MASK \
	0x80000000 /* EXECUTE[31] */
#define WF_PSE_TOP_PL_QUE_CTRL_0_EXECUTE_SHFT                  31
#define WF_PSE_TOP_PL_QUE_CTRL_0_PL_BUFFER_ADDR_ADDR \
	WF_PSE_TOP_PL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_PL_QUE_CTRL_0_PL_BUFFER_ADDR_MASK \
	0x0FFF0000 /* PL_BUFFER_ADDR[27..16] */
#define WF_PSE_TOP_PL_QUE_CTRL_0_PL_BUFFER_ADDR_SHFT           16
#define WF_PSE_TOP_PL_QUE_CTRL_0_NEXT_PAGE_ADDR \
	WF_PSE_TOP_PL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_PL_QUE_CTRL_0_NEXT_PAGE_MASK \
	0x00000FFF /* NEXT_PAGE[11..0] */
#define WF_PSE_TOP_PL_QUE_CTRL_0_NEXT_PAGE_SHFT                0

/*
* ---PLE_ENQ_PKT_NUM (0x820C8000 + 0x1F0)---
* PLE_ENQ_HIF_Q_PKT_NUM[15..0] - (RO) The packets numbers of PLE port enqueue to
* HIF queue.
* PLE_ENQ_HIF_Q_SEL[19..16] - (RW) Select HIF queue which is PLE port enqueue
to.
* RESERVED20[31..20] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PLE_ENQ_PKT_NUM_PLE_ENQ_HIF_Q_SEL_ADDR \
	WF_PSE_TOP_PLE_ENQ_PKT_NUM_ADDR
#define WF_PSE_TOP_PLE_ENQ_PKT_NUM_PLE_ENQ_HIF_Q_SEL_MASK \
	0x000F0000 /* PLE_ENQ_HIF_Q_SEL[19..16] */
#define WF_PSE_TOP_PLE_ENQ_PKT_NUM_PLE_ENQ_HIF_Q_SEL_SHFT      16
#define WF_PSE_TOP_PLE_ENQ_PKT_NUM_PLE_ENQ_HIF_Q_PKT_NUM_ADDR \
	WF_PSE_TOP_PLE_ENQ_PKT_NUM_ADDR
#define WF_PSE_TOP_PLE_ENQ_PKT_NUM_PLE_ENQ_HIF_Q_PKT_NUM_MASK \
	0x0000FFFF /* PLE_ENQ_HIF_Q_PKT_NUM[15..0] */
#define WF_PSE_TOP_PLE_ENQ_PKT_NUM_PLE_ENQ_HIF_Q_PKT_NUM_SHFT  0

/*
* ---CPU_ENQ_PKT_NUM (0x820C8000 + 0x1F4)---
* CPU_ENQ_HIF_Q_PKT_NUM[15..0] - (RO) The packets numbers of CPU port enqueue to
* HIF queue.
* CPU_ENQ_HIF_Q_SEL[19..16] - (RW) Select HIF queue which is CPU port enqueue
to.
* RESERVED20[31..20] - (RO) Reserved bits
*/
#define WF_PSE_TOP_CPU_ENQ_PKT_NUM_CPU_ENQ_HIF_Q_SEL_ADDR \
	WF_PSE_TOP_CPU_ENQ_PKT_NUM_ADDR
#define WF_PSE_TOP_CPU_ENQ_PKT_NUM_CPU_ENQ_HIF_Q_SEL_MASK \
	0x000F0000 /* CPU_ENQ_HIF_Q_SEL[19..16] */
#define WF_PSE_TOP_CPU_ENQ_PKT_NUM_CPU_ENQ_HIF_Q_SEL_SHFT      16
#define WF_PSE_TOP_CPU_ENQ_PKT_NUM_CPU_ENQ_HIF_Q_PKT_NUM_ADDR \
	WF_PSE_TOP_CPU_ENQ_PKT_NUM_ADDR
#define WF_PSE_TOP_CPU_ENQ_PKT_NUM_CPU_ENQ_HIF_Q_PKT_NUM_MASK \
	0x0000FFFF /* CPU_ENQ_HIF_Q_PKT_NUM[15..0] */
#define WF_PSE_TOP_CPU_ENQ_PKT_NUM_CPU_ENQ_HIF_Q_PKT_NUM_SHFT  0

/*
* ---LMAC_ENQ_PKT_NUM (0x820C8000 + 0x1F8)---
* LMAC_ENQ_HIF_Q_PKT_NUM[15..0] - (RO) The packets numbers of LMAC port enqueue
* to HIF queue.
* LMAC_ENQ_HIF_Q_SEL[19..16] - (RW) Select HIF queue which is LMAC port enqueue
to.
* RESERVED20[31..20] - (RO) Reserved bits
*/
#define WF_PSE_TOP_LMAC_ENQ_PKT_NUM_LMAC_ENQ_HIF_Q_SEL_ADDR \
	WF_PSE_TOP_LMAC_ENQ_PKT_NUM_ADDR
#define WF_PSE_TOP_LMAC_ENQ_PKT_NUM_LMAC_ENQ_HIF_Q_SEL_MASK \
	0x000F0000 /* LMAC_ENQ_HIF_Q_SEL[19..16] */
#define WF_PSE_TOP_LMAC_ENQ_PKT_NUM_LMAC_ENQ_HIF_Q_SEL_SHFT    16
#define WF_PSE_TOP_LMAC_ENQ_PKT_NUM_LMAC_ENQ_HIF_Q_PKT_NUM_ADDR \
	WF_PSE_TOP_LMAC_ENQ_PKT_NUM_ADDR
#define WF_PSE_TOP_LMAC_ENQ_PKT_NUM_LMAC_ENQ_HIF_Q_PKT_NUM_MASK \
	0x0000FFFF /* LMAC_ENQ_HIF_Q_PKT_NUM[15..0] */
#define WF_PSE_TOP_LMAC_ENQ_PKT_NUM_LMAC_ENQ_HIF_Q_PKT_NUM_SHFT 0

/*
* ---HIF_ENQ_PKT_NUM (0x820C8000 + 0x1FC)---
* HIF_ENQ_MDP[15..0] - (RO) The packets numbers of HIF port enqueue to MDP TX
queue.
* HIF_ENQ_CPU[31..16] - (RO) The packets numbers of HIF port enqueue to CPU
queue
*/
#define WF_PSE_TOP_HIF_ENQ_PKT_NUM_HIF_ENQ_CPU_ADDR \
	WF_PSE_TOP_HIF_ENQ_PKT_NUM_ADDR
#define WF_PSE_TOP_HIF_ENQ_PKT_NUM_HIF_ENQ_CPU_MASK \
	0xFFFF0000 /* HIF_ENQ_CPU[31..16] */
#define WF_PSE_TOP_HIF_ENQ_PKT_NUM_HIF_ENQ_CPU_SHFT            16
#define WF_PSE_TOP_HIF_ENQ_PKT_NUM_HIF_ENQ_MDP_ADDR \
	WF_PSE_TOP_HIF_ENQ_PKT_NUM_ADDR
#define WF_PSE_TOP_HIF_ENQ_PKT_NUM_HIF_ENQ_MDP_MASK \
	0x0000FFFF /* HIF_ENQ_MDP[15..0] */
#define WF_PSE_TOP_HIF_ENQ_PKT_NUM_HIF_ENQ_MDP_SHFT            0

/*
* ---MDP_ENQ_PKT_NUM (0x820C8000 + 0x200)---
* MDP_ENQ_HIF_Q_PKT_NUM[15..0] - (RO) The packets numbers of MDP port enqueue to
* HIF queue.
* MDP_ENQ_HIF_Q_SEL[31..16] - (RW) Select HIF queue which is MDP port enqueue
to.
*/
#define WF_PSE_TOP_MDP_ENQ_PKT_NUM_MDP_ENQ_HIF_Q_SEL_ADDR \
	WF_PSE_TOP_MDP_ENQ_PKT_NUM_ADDR
#define WF_PSE_TOP_MDP_ENQ_PKT_NUM_MDP_ENQ_HIF_Q_SEL_MASK \
	0xFFFF0000 /* MDP_ENQ_HIF_Q_SEL[31..16] */
#define WF_PSE_TOP_MDP_ENQ_PKT_NUM_MDP_ENQ_HIF_Q_SEL_SHFT      16
#define WF_PSE_TOP_MDP_ENQ_PKT_NUM_MDP_ENQ_HIF_Q_PKT_NUM_ADDR \
	WF_PSE_TOP_MDP_ENQ_PKT_NUM_ADDR
#define WF_PSE_TOP_MDP_ENQ_PKT_NUM_MDP_ENQ_HIF_Q_PKT_NUM_MASK \
	0x0000FFFF /* MDP_ENQ_HIF_Q_PKT_NUM[15..0] */
#define WF_PSE_TOP_MDP_ENQ_PKT_NUM_MDP_ENQ_HIF_Q_PKT_NUM_SHFT  0

/*
* ---RL_BUF_CTRL_0 (0x820C8000 + 0x210)---
* RELAY_BUF_ADDR[11..0] - (RW) Read address of relay buffer
* RESERVED12[30..12] - (RO) Reserved bits
* EXECUTE[31] - (A0) Executes relay buffer read command
*/
#define WF_PSE_TOP_RL_BUF_CTRL_0_EXECUTE_ADDR \
	WF_PSE_TOP_RL_BUF_CTRL_0_ADDR
#define WF_PSE_TOP_RL_BUF_CTRL_0_EXECUTE_MASK \
	0x80000000 /* EXECUTE[31] */
#define WF_PSE_TOP_RL_BUF_CTRL_0_EXECUTE_SHFT                  31
#define WF_PSE_TOP_RL_BUF_CTRL_0_RELAY_BUF_ADDR_ADDR \
	WF_PSE_TOP_RL_BUF_CTRL_0_ADDR
#define WF_PSE_TOP_RL_BUF_CTRL_0_RELAY_BUF_ADDR_MASK \
	0x00000FFF /* RELAY_BUF_ADDR[11..0] */
#define WF_PSE_TOP_RL_BUF_CTRL_0_RELAY_BUF_ADDR_SHFT           0

/*
* ---RL_BUF_CTRL_1 (0x820C8000 + 0x214)---
* PAGE_NUM[8..0] - (RO) Page number of packet
* RESERVED9[13..9] - (RO) Reserved bits
* PKT_TAIL_PAGE[25..14] - (RO) Tail page of the packet with head page being the
* relay buffer address
* RESERVED26[26] - (RO) Reserved bits
* RESV_GRP_ID[30..27] - (RO) Group ID of reserved page used by FID
* RESERVED31[31] - (RO) Reserved bits
*/
#define WF_PSE_TOP_RL_BUF_CTRL_1_RESV_GRP_ID_ADDR \
	WF_PSE_TOP_RL_BUF_CTRL_1_ADDR
#define WF_PSE_TOP_RL_BUF_CTRL_1_RESV_GRP_ID_MASK \
	0x78000000 /* RESV_GRP_ID[30..27] */
#define WF_PSE_TOP_RL_BUF_CTRL_1_RESV_GRP_ID_SHFT              27
#define WF_PSE_TOP_RL_BUF_CTRL_1_PKT_TAIL_PAGE_ADDR \
	WF_PSE_TOP_RL_BUF_CTRL_1_ADDR
#define WF_PSE_TOP_RL_BUF_CTRL_1_PKT_TAIL_PAGE_MASK \
	0x03FFC000 /* PKT_TAIL_PAGE[25..14] */
#define WF_PSE_TOP_RL_BUF_CTRL_1_PKT_TAIL_PAGE_SHFT            14
#define WF_PSE_TOP_RL_BUF_CTRL_1_PAGE_NUM_ADDR \
	WF_PSE_TOP_RL_BUF_CTRL_1_ADDR
#define WF_PSE_TOP_RL_BUF_CTRL_1_PAGE_NUM_MASK \
	0x000001FF /* PAGE_NUM[8..0] */
#define WF_PSE_TOP_RL_BUF_CTRL_1_PAGE_NUM_SHFT                 0

/*
* ---TIMEOUT_CTRL (0x820C8000 + 0x244)---
* FL_WD_TO_CTRL[7..0] - (RW) FL watch dog timeput
* RESERVED8[15..8] - (RO) Reserved bits
* APB_WD_TO_CTRL[23..16] - (RW) APB pready watch dog timeout control register.
* RESERVED24[31..24] - (RO) Reserved bits
*/
#define WF_PSE_TOP_TIMEOUT_CTRL_APB_WD_TO_CTRL_ADDR \
	WF_PSE_TOP_TIMEOUT_CTRL_ADDR
#define WF_PSE_TOP_TIMEOUT_CTRL_APB_WD_TO_CTRL_MASK \
	0x00FF0000 /* APB_WD_TO_CTRL[23..16] */
#define WF_PSE_TOP_TIMEOUT_CTRL_APB_WD_TO_CTRL_SHFT            16
#define WF_PSE_TOP_TIMEOUT_CTRL_FL_WD_TO_CTRL_ADDR \
	WF_PSE_TOP_TIMEOUT_CTRL_ADDR
#define WF_PSE_TOP_TIMEOUT_CTRL_FL_WD_TO_CTRL_MASK \
	0x000000FF /* FL_WD_TO_CTRL[7..0] */
#define WF_PSE_TOP_TIMEOUT_CTRL_FL_WD_TO_CTRL_SHFT             0

/*
* ---FSM_IDLE_WD_CTRL (0x820C8000 + 0x248)---
* FL_IDLE_WD_TO_TH[7..0] - (RW) Watchdog timeout threshold for frame link FSM
* not returning to IDLE
* PL_IDLE_WD_TO_TH[15..8] - (RW) Watchdog timeout threshold for page link FSM
* not returning to IDLE
* PORT_IDLE_WD_TO_TH[23..16] - (RW) Watchdog timeout threshold for page link
* Port Link not returning to IDLE
* (Including HIF/CPU/LMAC port)
* MACTX_IDLE_WD_TO_TH[31..24] - (RW) Watchdog timeout threshold for port oper
* MACTX not returning to IDLE
*/
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_MACTX_IDLE_WD_TO_TH_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_CTRL_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_MACTX_IDLE_WD_TO_TH_MASK \
	0xFF000000 /* MACTX_IDLE_WD_TO_TH[31..24] */
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_MACTX_IDLE_WD_TO_TH_SHFT   24
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_PORT_IDLE_WD_TO_TH_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_CTRL_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_PORT_IDLE_WD_TO_TH_MASK \
	0x00FF0000 /* PORT_IDLE_WD_TO_TH[23..16] */
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_PORT_IDLE_WD_TO_TH_SHFT    16
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_PL_IDLE_WD_TO_TH_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_CTRL_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_PL_IDLE_WD_TO_TH_MASK \
	0x0000FF00 /* PL_IDLE_WD_TO_TH[15..8] */
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_PL_IDLE_WD_TO_TH_SHFT      8
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_FL_IDLE_WD_TO_TH_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_CTRL_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_FL_IDLE_WD_TO_TH_MASK \
	0x000000FF /* FL_IDLE_WD_TO_TH[7..0] */
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_FL_IDLE_WD_TO_TH_SHFT      0

/*
* ---FSM_IDLE_WD_EN (0x820C8000 + 0x24C)---
* EN_HIF_PORT_IDLE_WD_TO[0] - (RW) Enables watchdog for HIF port oper FSM not
* returning to IDLE
* EN_CPU_PORT_IDLE_WD_TO[1] - (RW) Enables watchdog for CPU port oper FSM not
* returning to IDLE
* EN_LMAC_PORT_IDLE_WD_TO[2] - (RW) Enables watchdog for LMAC port oper FSM not
* returning to IDLE
* EN_MDP_IDLE_WD_TO[3] - (RW) Enables watchdog for MDP port oper FSM not
* returning to IDLE
* EN_SEC_IDLE_WD_TO[4] - (RW) Enables watchdog for SEC port oper FSM not
* returning to IDLE
* EN_PLE_IDLE_WD_TO[5] - (RW) Enables watchdog for PLE port oper FSM not
* returning to IDLE
* EN_AMSDU_IDLE_WD_TO[6] - (RW) Enables watchdog for AMSDU port oper FSM not
* returning to IDLE
* RESERVED7[7] - (RO) Reserved bits
* EN_FL_IDLE_WD_TO[8] - (RW) Enables watchdog for frame link FSM not returning
* to IDLE
* EN_PL_IDLE_WD_TO[9] - (RW) Enables watchdog for page link FSM not returning to
IDLE
* RESERVED10[31..10] - (RO) Reserved bits
*/
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_PL_IDLE_WD_TO_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_EN_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_PL_IDLE_WD_TO_MASK \
	0x00000200 /* EN_PL_IDLE_WD_TO[9] */
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_PL_IDLE_WD_TO_SHFT        9
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_FL_IDLE_WD_TO_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_EN_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_FL_IDLE_WD_TO_MASK \
	0x00000100 /* EN_FL_IDLE_WD_TO[8] */
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_FL_IDLE_WD_TO_SHFT        8
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_AMSDU_IDLE_WD_TO_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_EN_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_AMSDU_IDLE_WD_TO_MASK \
	0x00000040 /* EN_AMSDU_IDLE_WD_TO[6] */
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_AMSDU_IDLE_WD_TO_SHFT     6
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_PLE_IDLE_WD_TO_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_EN_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_PLE_IDLE_WD_TO_MASK \
	0x00000020 /* EN_PLE_IDLE_WD_TO[5] */
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_PLE_IDLE_WD_TO_SHFT       5
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_SEC_IDLE_WD_TO_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_EN_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_SEC_IDLE_WD_TO_MASK \
	0x00000010 /* EN_SEC_IDLE_WD_TO[4] */
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_SEC_IDLE_WD_TO_SHFT       4
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_MDP_IDLE_WD_TO_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_EN_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_MDP_IDLE_WD_TO_MASK \
	0x00000008 /* EN_MDP_IDLE_WD_TO[3] */
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_MDP_IDLE_WD_TO_SHFT       3
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_LMAC_PORT_IDLE_WD_TO_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_EN_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_LMAC_PORT_IDLE_WD_TO_MASK \
	0x00000004 /* EN_LMAC_PORT_IDLE_WD_TO[2] */
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_LMAC_PORT_IDLE_WD_TO_SHFT 2
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_CPU_PORT_IDLE_WD_TO_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_EN_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_CPU_PORT_IDLE_WD_TO_MASK \
	0x00000002 /* EN_CPU_PORT_IDLE_WD_TO[1] */
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_CPU_PORT_IDLE_WD_TO_SHFT  1
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_HIF_PORT_IDLE_WD_TO_ADDR \
	WF_PSE_TOP_FSM_IDLE_WD_EN_ADDR
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_HIF_PORT_IDLE_WD_TO_MASK \
	0x00000001 /* EN_HIF_PORT_IDLE_WD_TO[0] */
#define WF_PSE_TOP_FSM_IDLE_WD_EN_EN_HIF_PORT_IDLE_WD_TO_SHFT  0

/*
* ---PSE_WFDMA_BUF_CTRL (0x820C8000 + 0x250)---
* WFDMA_TXS_BUF_VLD_TH[7..0] - (RW) TXS valid reserved page count threshold
* WFDMA_TXCMD_BUF_VLD_TH[15..8] - (RW) TXCMD valid reserved page count threshold
* WFDMA_RX_PCIE_ACT_TH[27..16] - (RW) RX PCIE act threshold
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_WFDMA_RX_PCIE_ACT_TH_ADDR \
	WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_ADDR
#define WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_WFDMA_RX_PCIE_ACT_TH_MASK \
	0x0FFF0000 /* WFDMA_RX_PCIE_ACT_TH[27..16] */
#define WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_WFDMA_RX_PCIE_ACT_TH_SHFT 16
#define WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_WFDMA_TXCMD_BUF_VLD_TH_ADDR \
	WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_ADDR
#define WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_WFDMA_TXCMD_BUF_VLD_TH_MASK \
	0x0000FF00 /* WFDMA_TXCMD_BUF_VLD_TH[15..8] */
#define WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_WFDMA_TXCMD_BUF_VLD_TH_SHFT 8
#define WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_WFDMA_TXS_BUF_VLD_TH_ADDR \
	WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_ADDR
#define WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_WFDMA_TXS_BUF_VLD_TH_MASK \
	0x000000FF /* WFDMA_TXS_BUF_VLD_TH[7..0] */
#define WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_WFDMA_TXS_BUF_VLD_TH_SHFT 0

/*
* ---PSE_MISC_FUNC_CTRL (0x820C8000 + 0x254)---
* RESERVED0[7..0] - (RO) Reserved bits
* PSE_QUEUE_ACK_MODE[8] - (RW) Queue ACK mode control
* RESERVED9[29..9] - (RO) Reserved bits
* DIS_BYPASS_INVALID_FID[30] - (RW) Bypass invalid FID control
* DIS_BLOCK_MODE[31] - (RW) Block Mode Control
*/
#define WF_PSE_TOP_PSE_MISC_FUNC_CTRL_DIS_BLOCK_MODE_ADDR \
	WF_PSE_TOP_PSE_MISC_FUNC_CTRL_ADDR
#define WF_PSE_TOP_PSE_MISC_FUNC_CTRL_DIS_BLOCK_MODE_MASK \
	0x80000000 /* DIS_BLOCK_MODE[31] */
#define WF_PSE_TOP_PSE_MISC_FUNC_CTRL_DIS_BLOCK_MODE_SHFT      31
#define WF_PSE_TOP_PSE_MISC_FUNC_CTRL_DIS_BYPASS_INVALID_FID_ADDR \
	WF_PSE_TOP_PSE_MISC_FUNC_CTRL_ADDR
#define WF_PSE_TOP_PSE_MISC_FUNC_CTRL_DIS_BYPASS_INVALID_FID_MASK \
	0x40000000 /* DIS_BYPASS_INVALID_FID[30] */
#define WF_PSE_TOP_PSE_MISC_FUNC_CTRL_DIS_BYPASS_INVALID_FID_SHFT 30
#define WF_PSE_TOP_PSE_MISC_FUNC_CTRL_PSE_QUEUE_ACK_MODE_ADDR \
	WF_PSE_TOP_PSE_MISC_FUNC_CTRL_ADDR
#define WF_PSE_TOP_PSE_MISC_FUNC_CTRL_PSE_QUEUE_ACK_MODE_MASK \
	0x00000100 /* PSE_QUEUE_ACK_MODE[8] */
#define WF_PSE_TOP_PSE_MISC_FUNC_CTRL_PSE_QUEUE_ACK_MODE_SHFT  8

/*
* ---PSE_MODULE_CKG_DIS (0x820C8000 + 0x258)---
* DIS_FL_DYN_CKG[0] - (RW) Disable control of PSE frame link module dynamic
* clock gating function
* DIS_PL_DYN_CKG[1] - (RW) Disable control of PSE page link module dynamic clock
* gating function
* DIS_CPU_PORT_DYN_CKG[2] - (RW) Disable control of PSE CPU port module dynamic
* clock gating function
* DIS_HIF_PORT_DYN_CKG[3] - (RW) Disable control of PSE HIF port module dynamic
* clock gating function
* DIS_WF_PLE_PORT_DYN_CKG[4] - (RW) Disable control of PSE LMAC and PLE port
* module dynamic clock gating function
* DIS_RLS_DYN_CKG[5] - (RW) Disable control of PSE release module dynamic clock
* gating function
* DIS_RL_DYN_CKG[6] - (RW) Disable control of PSE relay information module
* dynamic clock gating function
* RESERVED7[8..7] - (RO) Reserved bits
* DIS_CSR_DYN_CKG[9] - (RW) Disable control of PSE CR module dynamic clock
* gating function
* DIS_CPU_WRAP_DYN_CKG[10] - (RW) Disable control of PSE CPU_WRAP module dynamic
* clock gating function
* DIS_DBG_DYN_CKG[11] - (RW) Disable control of PSE debug module dynamic clock
* gating function
* DIS_MDP_DYN_CKG[12] - (RW) Disable control of PSE MDP module dynamic clock
* gating function
* DIS_SEC_DYN_CKG[13] - (RW) Disable control of PSE SEC module dynamic clock
* gating function
* RESERVED14[30..14] - (RO) Reserved bits
* EN_DCM_SRC_CNT_NONEMPTY[31] - (RW) Enable control of PSE DCM when used pages
* is not empty
*/
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_EN_DCM_SRC_CNT_NONEMPTY_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_EN_DCM_SRC_CNT_NONEMPTY_MASK \
	0x80000000 /* EN_DCM_SRC_CNT_NONEMPTY[31] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_EN_DCM_SRC_CNT_NONEMPTY_SHFT 31
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_SEC_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_SEC_DYN_CKG_MASK \
	0x00002000 /* DIS_SEC_DYN_CKG[13] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_SEC_DYN_CKG_SHFT     13
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_MDP_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_MDP_DYN_CKG_MASK \
	0x00001000 /* DIS_MDP_DYN_CKG[12] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_MDP_DYN_CKG_SHFT     12
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_DBG_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_DBG_DYN_CKG_MASK \
	0x00000800 /* DIS_DBG_DYN_CKG[11] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_DBG_DYN_CKG_SHFT     11
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_CPU_WRAP_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_CPU_WRAP_DYN_CKG_MASK \
	0x00000400 /* DIS_CPU_WRAP_DYN_CKG[10] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_CPU_WRAP_DYN_CKG_SHFT 10
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_CSR_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_CSR_DYN_CKG_MASK \
	0x00000200 /* DIS_CSR_DYN_CKG[9] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_CSR_DYN_CKG_SHFT     9
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_RL_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_RL_DYN_CKG_MASK \
	0x00000040 /* DIS_RL_DYN_CKG[6] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_RL_DYN_CKG_SHFT      6
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_RLS_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_RLS_DYN_CKG_MASK \
	0x00000020 /* DIS_RLS_DYN_CKG[5] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_RLS_DYN_CKG_SHFT     5
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_WF_PLE_PORT_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_WF_PLE_PORT_DYN_CKG_MASK \
	0x00000010 /* DIS_WF_PLE_PORT_DYN_CKG[4] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_WF_PLE_PORT_DYN_CKG_SHFT 4
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_HIF_PORT_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_HIF_PORT_DYN_CKG_MASK \
	0x00000008 /* DIS_HIF_PORT_DYN_CKG[3] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_HIF_PORT_DYN_CKG_SHFT 3
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_CPU_PORT_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_CPU_PORT_DYN_CKG_MASK \
	0x00000004 /* DIS_CPU_PORT_DYN_CKG[2] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_CPU_PORT_DYN_CKG_SHFT 2
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_PL_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_PL_DYN_CKG_MASK \
	0x00000002 /* DIS_PL_DYN_CKG[1] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_PL_DYN_CKG_SHFT      1
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_FL_DYN_CKG_ADDR \
	WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_FL_DYN_CKG_MASK \
	0x00000001 /* DIS_FL_DYN_CKG[0] */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_DIS_FL_DYN_CKG_SHFT      0

/*
* ---PSE_INTER_ERR_FLAG (0x820C8000 + 0x280)---
* DEQ_EMPTY_QUEUE[0] - (RO) DEQueue Empty Error Flag
* APB_WD_TO[1] - (RO) APB Write Data Timeout Error Flag
* FL_CTRL_WD_TO[2] - (RO) FL Write Data Timeout Error Flag
* RESERVED3[31..3] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_INTER_ERR_FLAG_FL_CTRL_WD_TO_ADDR \
	WF_PSE_TOP_PSE_INTER_ERR_FLAG_ADDR
#define WF_PSE_TOP_PSE_INTER_ERR_FLAG_FL_CTRL_WD_TO_MASK \
	0x00000004 /* FL_CTRL_WD_TO[2] */
#define WF_PSE_TOP_PSE_INTER_ERR_FLAG_FL_CTRL_WD_TO_SHFT       2
#define WF_PSE_TOP_PSE_INTER_ERR_FLAG_APB_WD_TO_ADDR \
	WF_PSE_TOP_PSE_INTER_ERR_FLAG_ADDR
#define WF_PSE_TOP_PSE_INTER_ERR_FLAG_APB_WD_TO_MASK \
	0x00000002 /* APB_WD_TO[1] */
#define WF_PSE_TOP_PSE_INTER_ERR_FLAG_APB_WD_TO_SHFT           1
#define WF_PSE_TOP_PSE_INTER_ERR_FLAG_DEQ_EMPTY_QUEUE_ADDR \
	WF_PSE_TOP_PSE_INTER_ERR_FLAG_ADDR
#define WF_PSE_TOP_PSE_INTER_ERR_FLAG_DEQ_EMPTY_QUEUE_MASK \
	0x00000001 /* DEQ_EMPTY_QUEUE[0] */
#define WF_PSE_TOP_PSE_INTER_ERR_FLAG_DEQ_EMPTY_QUEUE_SHFT     0

/*
* ---PSE_SER_CTRL (0x820C8000 + 0x2A0)---
* HIF_SER_PAUSE_ALLOCATE[0] - (RW) System Error Recover function for Pause
* HIFBuffer Allocation
* CPU_SER_PAUSE_ALLOCATE[1] - (RW) System Error Recover function for Pause CPU
* Buffer Allocation
* WF_SER_PAUSE_ALLOCATE[2] - (RW) System Error Recover function for Pause WF
* Buffer Allocation
* MDP_SER_PAUSE_ALLOCATE[3] - (RW) System Error Recover function for Pause MDP
* Buffer Allocation
* SEC_SER_PAUSE_ALLOCATE[4] - (RW) System Error Recover function for Pause SEC
* Buffer Allocation
* PLE_SER_PAUSE_ALLOCATE[5] - (RW) System Error Recover function for Pause PLE
* Buffer Allocation
* RESERVED6[7..6] - (RO) Reserved bits
* HIF_SER_PAUSE_DATA[8] - (RW) System Error Recover function for Pause HIF DATA
opeartion.
* CPU_SER_PAUSE_DATA[9] - (RW) System Error Recover function for Pause CPU DATA
opeartion.
* WF_SER_PAUSE_DATA[10] - (RW) System Error Recover function for Pause WF DATA
opeartion.
* MDP_SER_PAUSE_DATA[11] - (RW) System Error Recover function for Pause MDP DATA
opeartion.
* SEC_SER_PAUSE_DATA[12] - (RW) System Error Recover function for Pause SEC DATA
opeartion.
* PLE_SER_PAUSE_DATA[13] - (RW) System Error Recover function for Pause PLE DATA
opeartion.
* RESERVED14[15..14] - (RO) Reserved bits
* HIF_SER_PAUSE_QUEUE[16] - (RW) System Error Recover function for Pause HIF
* Queue opeartion.
* CPU_SER_PAUSE_QUEUE[17] - (RW) System Error Recover function for Pause CPU
* Queue opeartion.
* WF_SER_PAUSE_QUEUE[18] - (RW) System Error Recover function for Pause WF Queue
opeartion.
* MDP_SER_PAUSE_QUEUE[19] - (RW) System Error Recover function for Pause MDP
* Queue opeartion.
* SEC_SER_PAUSE_QUEUE[20] - (RW) System Error Recover function for Pause SEC
* Queue opeartion.
* PLE_SER_PAUSE_QUEUE[21] - (RW) System Error Recover function for Pause PLE
* Queue opeartion.
* RESERVED22[31..22] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_SER_CTRL_PLE_SER_PAUSE_QUEUE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_PLE_SER_PAUSE_QUEUE_MASK \
	0x00200000 /* PLE_SER_PAUSE_QUEUE[21] */
#define WF_PSE_TOP_PSE_SER_CTRL_PLE_SER_PAUSE_QUEUE_SHFT       21
#define WF_PSE_TOP_PSE_SER_CTRL_SEC_SER_PAUSE_QUEUE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_SEC_SER_PAUSE_QUEUE_MASK \
	0x00100000 /* SEC_SER_PAUSE_QUEUE[20] */
#define WF_PSE_TOP_PSE_SER_CTRL_SEC_SER_PAUSE_QUEUE_SHFT       20
#define WF_PSE_TOP_PSE_SER_CTRL_MDP_SER_PAUSE_QUEUE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_MDP_SER_PAUSE_QUEUE_MASK \
	0x00080000 /* MDP_SER_PAUSE_QUEUE[19] */
#define WF_PSE_TOP_PSE_SER_CTRL_MDP_SER_PAUSE_QUEUE_SHFT       19
#define WF_PSE_TOP_PSE_SER_CTRL_WF_SER_PAUSE_QUEUE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_WF_SER_PAUSE_QUEUE_MASK \
	0x00040000 /* WF_SER_PAUSE_QUEUE[18] */
#define WF_PSE_TOP_PSE_SER_CTRL_WF_SER_PAUSE_QUEUE_SHFT        18
#define WF_PSE_TOP_PSE_SER_CTRL_CPU_SER_PAUSE_QUEUE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_CPU_SER_PAUSE_QUEUE_MASK \
	0x00020000 /* CPU_SER_PAUSE_QUEUE[17] */
#define WF_PSE_TOP_PSE_SER_CTRL_CPU_SER_PAUSE_QUEUE_SHFT       17
#define WF_PSE_TOP_PSE_SER_CTRL_HIF_SER_PAUSE_QUEUE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_HIF_SER_PAUSE_QUEUE_MASK \
	0x00010000 /* HIF_SER_PAUSE_QUEUE[16] */
#define WF_PSE_TOP_PSE_SER_CTRL_HIF_SER_PAUSE_QUEUE_SHFT       16
#define WF_PSE_TOP_PSE_SER_CTRL_PLE_SER_PAUSE_DATA_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_PLE_SER_PAUSE_DATA_MASK \
	0x00002000 /* PLE_SER_PAUSE_DATA[13] */
#define WF_PSE_TOP_PSE_SER_CTRL_PLE_SER_PAUSE_DATA_SHFT        13
#define WF_PSE_TOP_PSE_SER_CTRL_SEC_SER_PAUSE_DATA_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_SEC_SER_PAUSE_DATA_MASK \
	0x00001000 /* SEC_SER_PAUSE_DATA[12] */
#define WF_PSE_TOP_PSE_SER_CTRL_SEC_SER_PAUSE_DATA_SHFT        12
#define WF_PSE_TOP_PSE_SER_CTRL_MDP_SER_PAUSE_DATA_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_MDP_SER_PAUSE_DATA_MASK \
	0x00000800 /* MDP_SER_PAUSE_DATA[11] */
#define WF_PSE_TOP_PSE_SER_CTRL_MDP_SER_PAUSE_DATA_SHFT        11
#define WF_PSE_TOP_PSE_SER_CTRL_WF_SER_PAUSE_DATA_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_WF_SER_PAUSE_DATA_MASK \
	0x00000400 /* WF_SER_PAUSE_DATA[10] */
#define WF_PSE_TOP_PSE_SER_CTRL_WF_SER_PAUSE_DATA_SHFT         10
#define WF_PSE_TOP_PSE_SER_CTRL_CPU_SER_PAUSE_DATA_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_CPU_SER_PAUSE_DATA_MASK \
	0x00000200 /* CPU_SER_PAUSE_DATA[9] */
#define WF_PSE_TOP_PSE_SER_CTRL_CPU_SER_PAUSE_DATA_SHFT        9
#define WF_PSE_TOP_PSE_SER_CTRL_HIF_SER_PAUSE_DATA_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_HIF_SER_PAUSE_DATA_MASK \
	0x00000100 /* HIF_SER_PAUSE_DATA[8] */
#define WF_PSE_TOP_PSE_SER_CTRL_HIF_SER_PAUSE_DATA_SHFT        8
#define WF_PSE_TOP_PSE_SER_CTRL_PLE_SER_PAUSE_ALLOCATE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_PLE_SER_PAUSE_ALLOCATE_MASK \
	0x00000020 /* PLE_SER_PAUSE_ALLOCATE[5] */
#define WF_PSE_TOP_PSE_SER_CTRL_PLE_SER_PAUSE_ALLOCATE_SHFT    5
#define WF_PSE_TOP_PSE_SER_CTRL_SEC_SER_PAUSE_ALLOCATE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_SEC_SER_PAUSE_ALLOCATE_MASK \
	0x00000010 /* SEC_SER_PAUSE_ALLOCATE[4] */
#define WF_PSE_TOP_PSE_SER_CTRL_SEC_SER_PAUSE_ALLOCATE_SHFT    4
#define WF_PSE_TOP_PSE_SER_CTRL_MDP_SER_PAUSE_ALLOCATE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_MDP_SER_PAUSE_ALLOCATE_MASK \
	0x00000008 /* MDP_SER_PAUSE_ALLOCATE[3] */
#define WF_PSE_TOP_PSE_SER_CTRL_MDP_SER_PAUSE_ALLOCATE_SHFT    3
#define WF_PSE_TOP_PSE_SER_CTRL_WF_SER_PAUSE_ALLOCATE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_WF_SER_PAUSE_ALLOCATE_MASK \
	0x00000004 /* WF_SER_PAUSE_ALLOCATE[2] */
#define WF_PSE_TOP_PSE_SER_CTRL_WF_SER_PAUSE_ALLOCATE_SHFT     2
#define WF_PSE_TOP_PSE_SER_CTRL_CPU_SER_PAUSE_ALLOCATE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_CPU_SER_PAUSE_ALLOCATE_MASK \
	0x00000002 /* CPU_SER_PAUSE_ALLOCATE[1] */
#define WF_PSE_TOP_PSE_SER_CTRL_CPU_SER_PAUSE_ALLOCATE_SHFT    1
#define WF_PSE_TOP_PSE_SER_CTRL_HIF_SER_PAUSE_ALLOCATE_ADDR \
	WF_PSE_TOP_PSE_SER_CTRL_ADDR
#define WF_PSE_TOP_PSE_SER_CTRL_HIF_SER_PAUSE_ALLOCATE_MASK \
	0x00000001 /* HIF_SER_PAUSE_ALLOCATE[0] */
#define WF_PSE_TOP_PSE_SER_CTRL_HIF_SER_PAUSE_ALLOCATE_SHFT    0

/*
* ---PSE_MBIST_RP_FUSE (0x820C8000 + 0x2B0)---
* WF_PSE_MBIST_RP_FAIL[3..0] - (RO) MBIST Repair FAIL report
* RESERVED4[7..4] - (RO) Reserved bits
* WF_PSE_MBIST_RP_OK[11..8] - (RO) MBIST Repair OK report
* RESERVED12[14..12] - (RO) Reserved bits
* REG_FUSE_SEL[15] - (RW) MBIST FUSE setting
* RESERVED16[31..16] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_REG_FUSE_SEL_ADDR \
	WF_PSE_TOP_PSE_MBIST_RP_FUSE_ADDR
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_REG_FUSE_SEL_MASK \
	0x00008000 /* REG_FUSE_SEL[15] */
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_REG_FUSE_SEL_SHFT         15
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_WF_PSE_MBIST_RP_OK_ADDR \
	WF_PSE_TOP_PSE_MBIST_RP_FUSE_ADDR
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_WF_PSE_MBIST_RP_OK_MASK \
	0x00000F00 /* WF_PSE_MBIST_RP_OK[11..8] */
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_WF_PSE_MBIST_RP_OK_SHFT   8
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_WF_PSE_MBIST_RP_FAIL_ADDR \
	WF_PSE_TOP_PSE_MBIST_RP_FUSE_ADDR
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_WF_PSE_MBIST_RP_FAIL_MASK \
	0x0000000F /* WF_PSE_MBIST_RP_FAIL[3..0] */
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_WF_PSE_MBIST_RP_FAIL_SHFT 0

/*
* ---PSE_MBIST_RP_FUSE_1 (0x820C8000 + 0x2B4)---
* WF_UMAC_TOP_MBIST_PREFUSE_0[15..0] - (RW) MBIST PRE-FUSE setting
* WF_UMAC_TOP_MBIST_PREFUSE_D_0[31..16] - (RO) MBIST PRE-FUSE result
*/
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_1_WF_UMAC_TOP_MBIST_PREFUSE_D_0_ADDR \
	WF_PSE_TOP_PSE_MBIST_RP_FUSE_1_ADDR
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_1_WF_UMAC_TOP_MBIST_PREFUSE_D_0_MASK \
	0xFFFF0000 /* WF_UMAC_TOP_MBIST_PREFUSE_D_0[31..16] */
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_1_WF_UMAC_TOP_MBIST_PREFUSE_D_0_SHFT 16
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_1_WF_UMAC_TOP_MBIST_PREFUSE_0_ADDR \
	WF_PSE_TOP_PSE_MBIST_RP_FUSE_1_ADDR
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_1_WF_UMAC_TOP_MBIST_PREFUSE_0_MASK \
	0x0000FFFF /* WF_UMAC_TOP_MBIST_PREFUSE_0[15..0] */
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_1_WF_UMAC_TOP_MBIST_PREFUSE_0_SHFT 0

/*
* ---PSE_MBIST_BSEL (0x820C8000 + 0x2B8)---
* WF_PSE_MBIST_BSEL[15..0] - (RW) Memory MBIST BSEL control
* RESERVED16[31..16] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_MBIST_BSEL_WF_PSE_MBIST_BSEL_ADDR \
	WF_PSE_TOP_PSE_MBIST_BSEL_ADDR
#define WF_PSE_TOP_PSE_MBIST_BSEL_WF_PSE_MBIST_BSEL_MASK \
	0x0000FFFF /* WF_PSE_MBIST_BSEL[15..0] */
#define WF_PSE_TOP_PSE_MBIST_BSEL_WF_PSE_MBIST_BSEL_SHFT       0

/*
* ---SRAM_MBIST_BACKGROUND (0x820C8000 + 0x2C0)---
* MBIST_BACKGROUND[15..0] - (RW) Background setting for PSE SRAM MBIST circuit
* RESERVED16[31..16] - (RO) Reserved bits
*/
#define WF_PSE_TOP_SRAM_MBIST_BACKGROUND_MBIST_BACKGROUND_ADDR \
	WF_PSE_TOP_SRAM_MBIST_BACKGROUND_ADDR
#define WF_PSE_TOP_SRAM_MBIST_BACKGROUND_MBIST_BACKGROUND_MASK \
	0x0000FFFF /* MBIST_BACKGROUND[15..0] */
#define WF_PSE_TOP_SRAM_MBIST_BACKGROUND_MBIST_BACKGROUND_SHFT 0

/*
* ---SRAM_MBIST_DONE (0x820C8000 + 0x2C4)---
* MBIST_DONE[0] - (RO) Working status of PSE SRAM MBIST circuit
* RESERVED1[31..1] - (RO) Reserved bits
*/
#define WF_PSE_TOP_SRAM_MBIST_DONE_MBIST_DONE_ADDR \
	WF_PSE_TOP_SRAM_MBIST_DONE_ADDR
#define WF_PSE_TOP_SRAM_MBIST_DONE_MBIST_DONE_MASK \
	0x00000001 /* MBIST_DONE[0] */
#define WF_PSE_TOP_SRAM_MBIST_DONE_MBIST_DONE_SHFT             0

/*
* ---SRAM_MBIST_FAIL (0x820C8000 + 0x2C8)---
* FORDLINK_SRAM_MBIST_FAIL[0] - (RO) MBIST check result of forward link SRAM
* BACKLINK_SRAM_MBIST_FAIL[1] - (RO) MBIST check result of backward link SRAM
* PAGELINK_SRAM_MBIST_FAIL[2] - (RO) MBIST check result of PAGELINK SRAM
* RLINFO_SRAM_MBIST_FAIL[3] - (RO) MBIST check result of RLINFO SRAM
* RESERVED4[31..4] - (RO) Reserved bits
*/
#define WF_PSE_TOP_SRAM_MBIST_FAIL_RLINFO_SRAM_MBIST_FAIL_ADDR \
	WF_PSE_TOP_SRAM_MBIST_FAIL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_FAIL_RLINFO_SRAM_MBIST_FAIL_MASK \
	0x00000008 /* RLINFO_SRAM_MBIST_FAIL[3] */
#define WF_PSE_TOP_SRAM_MBIST_FAIL_RLINFO_SRAM_MBIST_FAIL_SHFT 3
#define WF_PSE_TOP_SRAM_MBIST_FAIL_PAGELINK_SRAM_MBIST_FAIL_ADDR \
	WF_PSE_TOP_SRAM_MBIST_FAIL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_FAIL_PAGELINK_SRAM_MBIST_FAIL_MASK \
	0x00000004 /* PAGELINK_SRAM_MBIST_FAIL[2] */
#define WF_PSE_TOP_SRAM_MBIST_FAIL_PAGELINK_SRAM_MBIST_FAIL_SHFT 2
#define WF_PSE_TOP_SRAM_MBIST_FAIL_BACKLINK_SRAM_MBIST_FAIL_ADDR \
	WF_PSE_TOP_SRAM_MBIST_FAIL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_FAIL_BACKLINK_SRAM_MBIST_FAIL_MASK \
	0x00000002 /* BACKLINK_SRAM_MBIST_FAIL[1] */
#define WF_PSE_TOP_SRAM_MBIST_FAIL_BACKLINK_SRAM_MBIST_FAIL_SHFT 1
#define WF_PSE_TOP_SRAM_MBIST_FAIL_FORDLINK_SRAM_MBIST_FAIL_ADDR \
	WF_PSE_TOP_SRAM_MBIST_FAIL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_FAIL_FORDLINK_SRAM_MBIST_FAIL_MASK \
	0x00000001 /* FORDLINK_SRAM_MBIST_FAIL[0] */
#define WF_PSE_TOP_SRAM_MBIST_FAIL_FORDLINK_SRAM_MBIST_FAIL_SHFT 0

/*
* ---SRAM_MBIST_CTRL (0x820C8000 + 0x2D0)---
* MBIST_MODE[0] - (RW) Control register for mbist_mode of MBIST
* RESERVED1[3..1] - (RO) Reserved bits
* MBIST_HOLDB[4] - (RW) Control register for mbist_holdb of MBIST
* RESERVED5[7..5] - (RO) Reserved bits
* MBIST_DEBUG[8] - (RW) Control register for mbist_debug of MBIST
* MBIST_RPRST_B[9] - (RW) MBIST Repair function Enable
* MBIST_USE_DEFAULT_DELSEL[10] - (RW) MBIST delay select control
* RESERVED11[15..11] - (RO) Reserved bits
* MBIST_DIAG_SEL[19..16] - (RW) Selection register for mbist_diag_scan_out
* RESERVED20[23..20] - (RO) Reserved bits
* MBIST_SLEEP_TEST[24] - (RW) Control register for sleep_test of MBIST
* MBIST_SLEEP_INV[25] - (RW) Control register for sleep_inv of MBIST
* MBIST_SLEEP_W[26] - (RW) Control register for sleep_w of MBIST
* MBIST_SLEEP_R[27] - (RW) Control register for sleep_r of MBIST
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_R_ADDR \
	WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_R_MASK \
	0x08000000 /* MBIST_SLEEP_R[27] */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_R_SHFT          27
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_W_ADDR \
	WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_W_MASK \
	0x04000000 /* MBIST_SLEEP_W[26] */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_W_SHFT          26
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_INV_ADDR \
	WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_INV_MASK \
	0x02000000 /* MBIST_SLEEP_INV[25] */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_INV_SHFT        25
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_TEST_ADDR \
	WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_TEST_MASK \
	0x01000000 /* MBIST_SLEEP_TEST[24] */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_SLEEP_TEST_SHFT       24
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_DIAG_SEL_ADDR \
	WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_DIAG_SEL_MASK \
	0x000F0000 /* MBIST_DIAG_SEL[19..16] */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_DIAG_SEL_SHFT         16
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_USE_DEFAULT_DELSEL_ADDR \
	WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_USE_DEFAULT_DELSEL_MASK \
	0x00000400 /* MBIST_USE_DEFAULT_DELSEL[10] */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_USE_DEFAULT_DELSEL_SHFT 10
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_RPRST_B_ADDR \
	WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_RPRST_B_MASK \
	0x00000200 /* MBIST_RPRST_B[9] */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_RPRST_B_SHFT          9
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_DEBUG_ADDR \
	WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_DEBUG_MASK \
	0x00000100 /* MBIST_DEBUG[8] */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_DEBUG_SHFT            8
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_HOLDB_ADDR \
	WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_HOLDB_MASK \
	0x00000010 /* MBIST_HOLDB[4] */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_HOLDB_SHFT            4
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_MODE_ADDR \
	WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_MODE_MASK \
	0x00000001 /* MBIST_MODE[0] */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_MBIST_MODE_SHFT             0

/*
* ---SRAM_MBIST_DELSEL (0x820C8000 + 0x2D4)---
* MBIST_DELSEL0_2[31..0] - (RW) Control register of delsel for PL SRAM
*/
#define WF_PSE_TOP_SRAM_MBIST_DELSEL_MBIST_DELSEL0_2_ADDR \
	WF_PSE_TOP_SRAM_MBIST_DELSEL_ADDR
#define WF_PSE_TOP_SRAM_MBIST_DELSEL_MBIST_DELSEL0_2_MASK \
	0xFFFFFFFF /* MBIST_DELSEL0_2[31..0] */
#define WF_PSE_TOP_SRAM_MBIST_DELSEL_MBIST_DELSEL0_2_SHFT      0

/*
* ---SRAM_MBIST_DELSEL_1 (0x820C8000 + 0x2D8)---
* MBIST_DELSEL3[31..0] - (RW) Control register of delsel for RLINFO SRAM
*/
#define WF_PSE_TOP_SRAM_MBIST_DELSEL_1_MBIST_DELSEL3_ADDR \
	WF_PSE_TOP_SRAM_MBIST_DELSEL_1_ADDR
#define WF_PSE_TOP_SRAM_MBIST_DELSEL_1_MBIST_DELSEL3_MASK \
	0xFFFFFFFF /* MBIST_DELSEL3[31..0] */
#define WF_PSE_TOP_SRAM_MBIST_DELSEL_1_MBIST_DELSEL3_SHFT      0

/*
* ---SRAM_AWT_HDEN_CTRL (0x820C8000 + 0x2DC)---
* FL_TBL_AWT1[0] - (RW) Memory AWT Control for FL_TBL1
* FL_TBL_AWT2[1] - (RW) Memory AWT Control for FL_TBL2
* PL_TBL_AWT[2] - (RW) Memory AWT Control for PL
* RLINFO_MEM_AWT[3] - (RW) Memory AWT Control for RL
* RESERVED4[15..4] - (RO) Reserved bits
* FL_TBL_HDEN1[16] - (RW) Memory HDEN Control for FL_TBL1
* FL_TBL_HDEN2[17] - (RW) Memory HDEN Control for FL_TBL2
* PL_TBL_HDEN[18] - (RW) Memory HDEN Control for PL
* RLINFO_MEM_HDEN[19] - (RW) Memory HDEN Control for RL
* RESERVED20[31..20] - (RO) Reserved bits
*/
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_RLINFO_MEM_HDEN_ADDR \
	WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_ADDR
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_RLINFO_MEM_HDEN_MASK \
	0x00080000 /* RLINFO_MEM_HDEN[19] */
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_RLINFO_MEM_HDEN_SHFT     19
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_PL_TBL_HDEN_ADDR \
	WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_ADDR
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_PL_TBL_HDEN_MASK \
	0x00040000 /* PL_TBL_HDEN[18] */
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_PL_TBL_HDEN_SHFT         18
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_HDEN2_ADDR \
	WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_ADDR
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_HDEN2_MASK \
	0x00020000 /* FL_TBL_HDEN2[17] */
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_HDEN2_SHFT        17
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_HDEN1_ADDR \
	WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_ADDR
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_HDEN1_MASK \
	0x00010000 /* FL_TBL_HDEN1[16] */
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_HDEN1_SHFT        16
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_RLINFO_MEM_AWT_ADDR \
	WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_ADDR
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_RLINFO_MEM_AWT_MASK \
	0x00000008 /* RLINFO_MEM_AWT[3] */
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_RLINFO_MEM_AWT_SHFT      3
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_PL_TBL_AWT_ADDR \
	WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_ADDR
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_PL_TBL_AWT_MASK \
	0x00000004 /* PL_TBL_AWT[2] */
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_PL_TBL_AWT_SHFT          2
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_AWT2_ADDR \
	WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_ADDR
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_AWT2_MASK \
	0x00000002 /* FL_TBL_AWT2[1] */
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_AWT2_SHFT         1
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_AWT1_ADDR \
	WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_ADDR
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_AWT1_MASK \
	0x00000001 /* FL_TBL_AWT1[0] */
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_FL_TBL_AWT1_SHFT         0

/*
* ---C_GET_FID_0 (0x820C8000 + 0x310)---
* QUEUE_FRAME_ID[15..0] - (RW) Frame ID for get command
* At GET_FRAME_TYPE = 2'h0/2'h1:
* QUEUE_FRAME_ID[15:14] = PID
* 2'h0: HIF port
* 2'h1: CPU port
* 2'h2: LMAC port
* QUEUE_FRAME_ID[9:0] = WLANID
* At GET_FRAME_TYPE = 2'h2/2'h3:
* QUEUE_FRAME_ID[11:0] = reference FID
* GET_FRAME_TYPE[19..16] - (RW) GET_SUB_TYPE
* RESERVED20[23..20] - (RO) Reserved bits
* GET_FRAME_QID[30..24] - (RW) Queue ID vlaue
* EXECUTE[31] - (A0) Executes command
*/
#define WF_PSE_TOP_C_GET_FID_0_EXECUTE_ADDR \
	WF_PSE_TOP_C_GET_FID_0_ADDR
#define WF_PSE_TOP_C_GET_FID_0_EXECUTE_MASK \
	0x80000000 /* EXECUTE[31] */
#define WF_PSE_TOP_C_GET_FID_0_EXECUTE_SHFT                    31
#define WF_PSE_TOP_C_GET_FID_0_GET_FRAME_QID_ADDR \
	WF_PSE_TOP_C_GET_FID_0_ADDR
#define WF_PSE_TOP_C_GET_FID_0_GET_FRAME_QID_MASK \
	0x7F000000 /* GET_FRAME_QID[30..24] */
#define WF_PSE_TOP_C_GET_FID_0_GET_FRAME_QID_SHFT              24
#define WF_PSE_TOP_C_GET_FID_0_GET_FRAME_TYPE_ADDR \
	WF_PSE_TOP_C_GET_FID_0_ADDR
#define WF_PSE_TOP_C_GET_FID_0_GET_FRAME_TYPE_MASK \
	0x000F0000 /* GET_FRAME_TYPE[19..16] */
#define WF_PSE_TOP_C_GET_FID_0_GET_FRAME_TYPE_SHFT             16
#define WF_PSE_TOP_C_GET_FID_0_QUEUE_FRAME_ID_ADDR \
	WF_PSE_TOP_C_GET_FID_0_ADDR
#define WF_PSE_TOP_C_GET_FID_0_QUEUE_FRAME_ID_MASK \
	0x0000FFFF /* QUEUE_FRAME_ID[15..0] */
#define WF_PSE_TOP_C_GET_FID_0_QUEUE_FRAME_ID_SHFT             0

/*
* ---C_GET_FID_1 (0x820C8000 + 0x314)---
* GET_RETURN_FID[11..0] - (RO) Return frame ID
* RESERVED12[14..12] - (RO) Reserved bits
* END[15] - (RO) Return frame ID is end FID
* RESERVED16[31..16] - (RO) Reserved bits
*/
#define WF_PSE_TOP_C_GET_FID_1_END_ADDR \
	WF_PSE_TOP_C_GET_FID_1_ADDR
#define WF_PSE_TOP_C_GET_FID_1_END_MASK \
	0x00008000 /* END[15] */
#define WF_PSE_TOP_C_GET_FID_1_END_SHFT                        15
#define WF_PSE_TOP_C_GET_FID_1_GET_RETURN_FID_ADDR \
	WF_PSE_TOP_C_GET_FID_1_ADDR
#define WF_PSE_TOP_C_GET_FID_1_GET_RETURN_FID_MASK \
	0x00000FFF /* GET_RETURN_FID[11..0] */
#define WF_PSE_TOP_C_GET_FID_1_GET_RETURN_FID_SHFT             0

/*
* ---C_EN_QUEUE_0 (0x820C8000 + 0x320)---
* DST_WLANID[9..0] - (RW) Destination WLANID for enqueue
* RESERVED10[13..10] - (RO) Reserved bits
* DST_PID[15..14] - (RW) Destination port ID for enqueue
* SUB_TYPE[19..16] - (RW) Sub-type of enqueue command
* RESERVED20[22..20] - (RO) Reserved bits
* DELAY_ENQ[23] - (RW) Delays enqueue
* DST_QID[30..24] - (RW) Destination queue ID for enqueue
* EXECUTE[31] - (A0) Executes command
*/
#define WF_PSE_TOP_C_EN_QUEUE_0_EXECUTE_ADDR \
	WF_PSE_TOP_C_EN_QUEUE_0_ADDR
#define WF_PSE_TOP_C_EN_QUEUE_0_EXECUTE_MASK \
	0x80000000 /* EXECUTE[31] */
#define WF_PSE_TOP_C_EN_QUEUE_0_EXECUTE_SHFT                   31
#define WF_PSE_TOP_C_EN_QUEUE_0_DST_QID_ADDR \
	WF_PSE_TOP_C_EN_QUEUE_0_ADDR
#define WF_PSE_TOP_C_EN_QUEUE_0_DST_QID_MASK \
	0x7F000000 /* DST_QID[30..24] */
#define WF_PSE_TOP_C_EN_QUEUE_0_DST_QID_SHFT                   24
#define WF_PSE_TOP_C_EN_QUEUE_0_DELAY_ENQ_ADDR \
	WF_PSE_TOP_C_EN_QUEUE_0_ADDR
#define WF_PSE_TOP_C_EN_QUEUE_0_DELAY_ENQ_MASK \
	0x00800000 /* DELAY_ENQ[23] */
#define WF_PSE_TOP_C_EN_QUEUE_0_DELAY_ENQ_SHFT                 23
#define WF_PSE_TOP_C_EN_QUEUE_0_SUB_TYPE_ADDR \
	WF_PSE_TOP_C_EN_QUEUE_0_ADDR
#define WF_PSE_TOP_C_EN_QUEUE_0_SUB_TYPE_MASK \
	0x000F0000 /* SUB_TYPE[19..16] */
#define WF_PSE_TOP_C_EN_QUEUE_0_SUB_TYPE_SHFT                  16
#define WF_PSE_TOP_C_EN_QUEUE_0_DST_PID_ADDR \
	WF_PSE_TOP_C_EN_QUEUE_0_ADDR
#define WF_PSE_TOP_C_EN_QUEUE_0_DST_PID_MASK \
	0x0000C000 /* DST_PID[15..14] */
#define WF_PSE_TOP_C_EN_QUEUE_0_DST_PID_SHFT                   14
#define WF_PSE_TOP_C_EN_QUEUE_0_DST_WLANID_ADDR \
	WF_PSE_TOP_C_EN_QUEUE_0_ADDR
#define WF_PSE_TOP_C_EN_QUEUE_0_DST_WLANID_MASK \
	0x000003FF /* DST_WLANID[9..0] */
#define WF_PSE_TOP_C_EN_QUEUE_0_DST_WLANID_SHFT                0

/*
* ---C_EN_QUEUE_1 (0x820C8000 + 0x324)---
* CUR_LIST_FID_START[11..0] - (RW) Start frame ID of enqueue operation list,
* enqueue FID of enqueue operation
* RESERVED12[15..12] - (RO) Reserved bits
* CUR_LIST_FID_END[27..16] - (RW) End frame ID of enqueue operation list
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_C_EN_QUEUE_1_CUR_LIST_FID_END_ADDR \
	WF_PSE_TOP_C_EN_QUEUE_1_ADDR
#define WF_PSE_TOP_C_EN_QUEUE_1_CUR_LIST_FID_END_MASK \
	0x0FFF0000 /* CUR_LIST_FID_END[27..16] */
#define WF_PSE_TOP_C_EN_QUEUE_1_CUR_LIST_FID_END_SHFT          16
#define WF_PSE_TOP_C_EN_QUEUE_1_CUR_LIST_FID_START_ADDR \
	WF_PSE_TOP_C_EN_QUEUE_1_ADDR
#define WF_PSE_TOP_C_EN_QUEUE_1_CUR_LIST_FID_START_MASK \
	0x00000FFF /* CUR_LIST_FID_START[11..0] */
#define WF_PSE_TOP_C_EN_QUEUE_1_CUR_LIST_FID_START_SHFT        0

/*
* ---C_EN_QUEUE_2 (0x820C8000 + 0x328)---
* TARGET_FID[11..0] - (RW) Target reference FID for enqueue operation
* RESERVED12[31..12] - (RO) Reserved bits
*/
#define WF_PSE_TOP_C_EN_QUEUE_2_TARGET_FID_ADDR \
	WF_PSE_TOP_C_EN_QUEUE_2_ADDR
#define WF_PSE_TOP_C_EN_QUEUE_2_TARGET_FID_MASK \
	0x00000FFF /* TARGET_FID[11..0] */
#define WF_PSE_TOP_C_EN_QUEUE_2_TARGET_FID_SHFT                0

/*
* ---C_DE_QUEUE_0 (0x820C8000 + 0x330)---
* SRC_WLANID[9..0] - (RW) Source WLAN ID for dequeue command
* RESERVED10[13..10] - (RO) Reserved bits
* SRC_PID[15..14] - (RW) Source port ID for dequeue command
* DEQ_SUB_TYPE[19..16] - (RW) Dequeue subtype of dequeue command
* ENQ_SUB_TYPE[22..20] - (RW) Enqueue subtype of enqueue command
* Only valid in Deq&Enq type.
* ENQ_VLD[23] - (RW) Deq&Enq command valid
* SRC_QID[30..24] - (RW) Source queue ID for dequeue command
* EXECUTE[31] - (A0) Executes dequeue command
*/
#define WF_PSE_TOP_C_DE_QUEUE_0_EXECUTE_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_0_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_0_EXECUTE_MASK \
	0x80000000 /* EXECUTE[31] */
#define WF_PSE_TOP_C_DE_QUEUE_0_EXECUTE_SHFT                   31
#define WF_PSE_TOP_C_DE_QUEUE_0_SRC_QID_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_0_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_0_SRC_QID_MASK \
	0x7F000000 /* SRC_QID[30..24] */
#define WF_PSE_TOP_C_DE_QUEUE_0_SRC_QID_SHFT                   24
#define WF_PSE_TOP_C_DE_QUEUE_0_ENQ_VLD_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_0_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_0_ENQ_VLD_MASK \
	0x00800000 /* ENQ_VLD[23] */
#define WF_PSE_TOP_C_DE_QUEUE_0_ENQ_VLD_SHFT                   23
#define WF_PSE_TOP_C_DE_QUEUE_0_ENQ_SUB_TYPE_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_0_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_0_ENQ_SUB_TYPE_MASK \
	0x00700000 /* ENQ_SUB_TYPE[22..20] */
#define WF_PSE_TOP_C_DE_QUEUE_0_ENQ_SUB_TYPE_SHFT              20
#define WF_PSE_TOP_C_DE_QUEUE_0_DEQ_SUB_TYPE_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_0_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_0_DEQ_SUB_TYPE_MASK \
	0x000F0000 /* DEQ_SUB_TYPE[19..16] */
#define WF_PSE_TOP_C_DE_QUEUE_0_DEQ_SUB_TYPE_SHFT              16
#define WF_PSE_TOP_C_DE_QUEUE_0_SRC_PID_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_0_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_0_SRC_PID_MASK \
	0x0000C000 /* SRC_PID[15..14] */
#define WF_PSE_TOP_C_DE_QUEUE_0_SRC_PID_SHFT                   14
#define WF_PSE_TOP_C_DE_QUEUE_0_SRC_WLANID_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_0_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_0_SRC_WLANID_MASK \
	0x000003FF /* SRC_WLANID[9..0] */
#define WF_PSE_TOP_C_DE_QUEUE_0_SRC_WLANID_SHFT                0

/*
* ---C_DE_QUEUE_1 (0x820C8000 + 0x334)---
* CUR_LIST_FID_START[11..0] - (RW) Start frame ID of dequeue operation list,
* enqueue start FID of enqueue operation
* Only valid in Deq&Enq type.
* RESERVED12[15..12] - (RO) Reserved bits
* CUR_LIST_FID_END[27..16] - (RW) End framd ID of dequeue operation list,
* enqueue end FID of enqueue operation
* Only valid in Deq&Enq type.
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_C_DE_QUEUE_1_CUR_LIST_FID_END_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_1_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_1_CUR_LIST_FID_END_MASK \
	0x0FFF0000 /* CUR_LIST_FID_END[27..16] */
#define WF_PSE_TOP_C_DE_QUEUE_1_CUR_LIST_FID_END_SHFT          16
#define WF_PSE_TOP_C_DE_QUEUE_1_CUR_LIST_FID_START_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_1_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_1_CUR_LIST_FID_START_MASK \
	0x00000FFF /* CUR_LIST_FID_START[11..0] */
#define WF_PSE_TOP_C_DE_QUEUE_1_CUR_LIST_FID_START_SHFT        0

/*
* ---C_DE_QUEUE_2 (0x820C8000 + 0x338)---
* DEQ_ENQ_DST_WLANID[9..0] - (RW) Destination WLAN ID for enqueue command
* Only valid in Deq&Enq type.
* RESERVED10[13..10] - (RO) Reserved bits
* DEQ_ENQ_DST_PID[15..14] - (RW) Destination port ID for dequeue command
* RESERVED16[23..16] - (RO) Reserved bits
* DEQ_ENQ_DST_QID[30..24] - (RW) Destination queue ID for enqueue command
* Only valid in Deq&Enq type.
* RESERVED31[31] - (RO) Reserved bits
*/
#define WF_PSE_TOP_C_DE_QUEUE_2_DEQ_ENQ_DST_QID_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_2_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_2_DEQ_ENQ_DST_QID_MASK \
	0x7F000000 /* DEQ_ENQ_DST_QID[30..24] */
#define WF_PSE_TOP_C_DE_QUEUE_2_DEQ_ENQ_DST_QID_SHFT           24
#define WF_PSE_TOP_C_DE_QUEUE_2_DEQ_ENQ_DST_PID_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_2_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_2_DEQ_ENQ_DST_PID_MASK \
	0x0000C000 /* DEQ_ENQ_DST_PID[15..14] */
#define WF_PSE_TOP_C_DE_QUEUE_2_DEQ_ENQ_DST_PID_SHFT           14
#define WF_PSE_TOP_C_DE_QUEUE_2_DEQ_ENQ_DST_WLANID_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_2_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_2_DEQ_ENQ_DST_WLANID_MASK \
	0x000003FF /* DEQ_ENQ_DST_WLANID[9..0] */
#define WF_PSE_TOP_C_DE_QUEUE_2_DEQ_ENQ_DST_WLANID_SHFT        0

/*
* ---C_DE_QUEUE_3 (0x820C8000 + 0x33C)---
* DEQ_HEAD_FDI[11..0] - (RO) Head FID got from dequeue command
* RESERVED12[14..12] - (RO) Reserved bits
* DEQ_EMPTY[15] - (RO) Queue empty after dequeue command is executed
* DEQ_TAIL_FID[27..16] - (RO) Last FID got from dequeue command
* RESERVED28[30..28] - (RO) Reserved bits
* BUSY[31] - (RO) Dequeue execute busy
*/
#define WF_PSE_TOP_C_DE_QUEUE_3_BUSY_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_3_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_3_BUSY_MASK \
	0x80000000 /* BUSY[31] */
#define WF_PSE_TOP_C_DE_QUEUE_3_BUSY_SHFT                      31
#define WF_PSE_TOP_C_DE_QUEUE_3_DEQ_TAIL_FID_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_3_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_3_DEQ_TAIL_FID_MASK \
	0x0FFF0000 /* DEQ_TAIL_FID[27..16] */
#define WF_PSE_TOP_C_DE_QUEUE_3_DEQ_TAIL_FID_SHFT              16
#define WF_PSE_TOP_C_DE_QUEUE_3_DEQ_EMPTY_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_3_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_3_DEQ_EMPTY_MASK \
	0x00008000 /* DEQ_EMPTY[15] */
#define WF_PSE_TOP_C_DE_QUEUE_3_DEQ_EMPTY_SHFT                 15
#define WF_PSE_TOP_C_DE_QUEUE_3_DEQ_HEAD_FDI_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_3_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_3_DEQ_HEAD_FDI_MASK \
	0x00000FFF /* DEQ_HEAD_FDI[11..0] */
#define WF_PSE_TOP_C_DE_QUEUE_3_DEQ_HEAD_FDI_SHFT              0

/*
* ---C_DE_QUEUE_4 (0x820C8000 + 0x340)---
* DEQ_ENQ_REF_FID[11..0] - (RW) Reference frame ID for enqueue command
* Only valid in Deq&Enq type.
* RESERVED12[31..12] - (RO) Reserved bits
*/
#define WF_PSE_TOP_C_DE_QUEUE_4_DEQ_ENQ_REF_FID_ADDR \
	WF_PSE_TOP_C_DE_QUEUE_4_ADDR
#define WF_PSE_TOP_C_DE_QUEUE_4_DEQ_ENQ_REF_FID_MASK \
	0x00000FFF /* DEQ_ENQ_REF_FID[11..0] */
#define WF_PSE_TOP_C_DE_QUEUE_4_DEQ_ENQ_REF_FID_SHFT           0

/*
* ---ALLOCATE_0 (0x820C8000 + 0x350)---
* ALLOCATE_FRAME_LENGTH[13..0] - (RW) Allocate frame length
* Unit: DW (4 bytes)
* RESERVED14[15..14] - (RO) Reserved bits
* ALLOCATE_QID[20..16] - (RW) QID used for allocate buffer
* RESERVED21[30..21] - (RO) Reserved bits
* EXECUTE[31] - (A0) Executes allocate buffer command
*/
#define WF_PSE_TOP_ALLOCATE_0_EXECUTE_ADDR \
	WF_PSE_TOP_ALLOCATE_0_ADDR
#define WF_PSE_TOP_ALLOCATE_0_EXECUTE_MASK \
	0x80000000 /* EXECUTE[31] */
#define WF_PSE_TOP_ALLOCATE_0_EXECUTE_SHFT                     31
#define WF_PSE_TOP_ALLOCATE_0_ALLOCATE_QID_ADDR \
	WF_PSE_TOP_ALLOCATE_0_ADDR
#define WF_PSE_TOP_ALLOCATE_0_ALLOCATE_QID_MASK \
	0x001F0000 /* ALLOCATE_QID[20..16] */
#define WF_PSE_TOP_ALLOCATE_0_ALLOCATE_QID_SHFT                16
#define WF_PSE_TOP_ALLOCATE_0_ALLOCATE_FRAME_LENGTH_ADDR \
	WF_PSE_TOP_ALLOCATE_0_ADDR
#define WF_PSE_TOP_ALLOCATE_0_ALLOCATE_FRAME_LENGTH_MASK \
	0x00003FFF /* ALLOCATE_FRAME_LENGTH[13..0] */
#define WF_PSE_TOP_ALLOCATE_0_ALLOCATE_FRAME_LENGTH_SHFT       0

/*
* ---ALLOCATE_1 (0x820C8000 + 0x354)---
* ALLOCATE_FID[11..0] - (RO) Return frame ID for allocate buffer command
* RESERVED12[30..12] - (RO) Reserved bits
* EXECUTE[31] - (RO) Execute status of allocate buffer command
*/
#define WF_PSE_TOP_ALLOCATE_1_EXECUTE_ADDR \
	WF_PSE_TOP_ALLOCATE_1_ADDR
#define WF_PSE_TOP_ALLOCATE_1_EXECUTE_MASK \
	0x80000000 /* EXECUTE[31] */
#define WF_PSE_TOP_ALLOCATE_1_EXECUTE_SHFT                     31
#define WF_PSE_TOP_ALLOCATE_1_ALLOCATE_FID_ADDR \
	WF_PSE_TOP_ALLOCATE_1_ADDR
#define WF_PSE_TOP_ALLOCATE_1_ALLOCATE_FID_MASK \
	0x00000FFF /* ALLOCATE_FID[11..0] */
#define WF_PSE_TOP_ALLOCATE_1_ALLOCATE_FID_SHFT                0

/*
* ---FREEPG_CNT (0x820C8000 + 0x380)---
* FREEPG_CNT[11..0] - (RO) Total page number of free
* RESERVED12[15..12] - (RO) Reserved bits
* FFA_CNT[27..16] - (RO) Free page numbers of free for all
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_FREEPG_CNT_FFA_CNT_ADDR \
	WF_PSE_TOP_FREEPG_CNT_ADDR
#define WF_PSE_TOP_FREEPG_CNT_FFA_CNT_MASK \
	0x0FFF0000 /* FFA_CNT[27..16] */
#define WF_PSE_TOP_FREEPG_CNT_FFA_CNT_SHFT                     16
#define WF_PSE_TOP_FREEPG_CNT_FREEPG_CNT_ADDR \
	WF_PSE_TOP_FREEPG_CNT_ADDR
#define WF_PSE_TOP_FREEPG_CNT_FREEPG_CNT_MASK \
	0x00000FFF /* FREEPG_CNT[11..0] */
#define WF_PSE_TOP_FREEPG_CNT_FREEPG_CNT_SHFT                  0

/*
* ---FREEPG_HEAD_TAIL (0x820C8000 + 0x384)---
* FREEPG_HEAD[11..0] - (RO) Head page of free page list
* RESERVED12[15..12] - (RO) Reserved bits
* FREEPG_TAIL[27..16] - (RO) Tail page of free page list
* RESERVED28[31..28] - (RO) Reserved bits
*/
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_TAIL_ADDR \
	WF_PSE_TOP_FREEPG_HEAD_TAIL_ADDR
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_TAIL_MASK \
	0x0FFF0000 /* FREEPG_TAIL[27..16] */
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_TAIL_SHFT           16
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_HEAD_ADDR \
	WF_PSE_TOP_FREEPG_HEAD_TAIL_ADDR
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_HEAD_MASK \
	0x00000FFF /* FREEPG_HEAD[11..0] */
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_HEAD_SHFT           0

/*
* ---PSE_SEEK_CR_00 (0x820C8000 + 0x3D0)---
* PSE_FL_ARB_CS[2..0] - (RO) PSE debug state for FL_ARB_CS
* RESERVED3[3] - (RO) Reserved bits
* PSE_FL_CS[7..4] - (RO) PSE debug state for FL_CS
* PSE_ENQ_FL_CS[11..8] - (RO) PSE debug state for ENQ_FL_CS
* PSE_DEQ_FL_CS[15..12] - (RO) PSE debug state for DEQ_FL_CS
* PSE_ACC_LST_CS[18..16] - (RO) PSE debug state for ACC_LST_CS
* RESERVED19[31..19] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_ACC_LST_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_00_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_ACC_LST_CS_MASK \
	0x00070000 /* PSE_ACC_LST_CS[18..16] */
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_ACC_LST_CS_SHFT          16
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_DEQ_FL_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_00_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_DEQ_FL_CS_MASK \
	0x0000F000 /* PSE_DEQ_FL_CS[15..12] */
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_DEQ_FL_CS_SHFT           12
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_ENQ_FL_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_00_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_ENQ_FL_CS_MASK \
	0x00000F00 /* PSE_ENQ_FL_CS[11..8] */
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_ENQ_FL_CS_SHFT           8
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_FL_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_00_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_FL_CS_MASK \
	0x000000F0 /* PSE_FL_CS[7..4] */
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_FL_CS_SHFT               4
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_FL_ARB_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_00_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_FL_ARB_CS_MASK \
	0x00000007 /* PSE_FL_ARB_CS[2..0] */
#define WF_PSE_TOP_PSE_SEEK_CR_00_PSE_FL_ARB_CS_SHFT           0

/*
* ---PSE_SEEK_CR_01 (0x820C8000 + 0x3D4)---
* PSE_PL_ARB_CS[2..0] - (RO) PSE debug state for PL_ARB_CS
* RESERVED3[3] - (RO) Reserved bits
* PSE_PL_INIT_CS[5..4] - (RO) PSE debug state for PL_INIT_CS
* RESERVED6[7..6] - (RO) Reserved bits
* PSE_PL_GNEXT_CS[8] - (RO) PSE debug state for PL_GNEXT_CS
* RESERVED9[11..9] - (RO) Reserved bits
* PSE_PL_GBUF_CS[14..12] - (RO) PSE debug state for PL_GBUF_CS
* RESERVED15[15] - (RO) Reserved bits
* PSE_PL_RLS_CS[17..16] - (RO) PSE debug state for PL_RLS_CS
* RESERVED18[19..18] - (RO) Reserved bits
* PSE_PL_REFILL_CS[22..20] - (RO) PSE debug state for PL_REFILL_CS
* RESERVED23[23] - (RO) Reserved bits
* PSE_RLS_CS[25..24] - (RO) PSE debug state for RLS_CS
* RESERVED26[31..26] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_RLS_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_01_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_RLS_CS_MASK \
	0x03000000 /* PSE_RLS_CS[25..24] */
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_RLS_CS_SHFT              24
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_REFILL_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_01_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_REFILL_CS_MASK \
	0x00700000 /* PSE_PL_REFILL_CS[22..20] */
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_REFILL_CS_SHFT        20
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_RLS_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_01_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_RLS_CS_MASK \
	0x00030000 /* PSE_PL_RLS_CS[17..16] */
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_RLS_CS_SHFT           16
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_GBUF_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_01_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_GBUF_CS_MASK \
	0x00007000 /* PSE_PL_GBUF_CS[14..12] */
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_GBUF_CS_SHFT          12
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_GNEXT_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_01_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_GNEXT_CS_MASK \
	0x00000100 /* PSE_PL_GNEXT_CS[8] */
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_GNEXT_CS_SHFT         8
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_INIT_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_01_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_INIT_CS_MASK \
	0x00000030 /* PSE_PL_INIT_CS[5..4] */
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_INIT_CS_SHFT          4
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_ARB_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_01_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_ARB_CS_MASK \
	0x00000007 /* PSE_PL_ARB_CS[2..0] */
#define WF_PSE_TOP_PSE_SEEK_CR_01_PSE_PL_ARB_CS_SHFT           0

/*
* ---PSE_SEEK_CR_02 (0x820C8000 + 0x3D8)---
* PSE_HIF_DOP_PBUF_CS[2..0] - (RO) PSE debug state for HIF_DOP_PBUF_CS
* RESERVED3[3] - (RO) Reserved bits
* PSE_HIF_DOP_CACHE_CS[5..4] - (RO) PSE debug state for HIF_DOP_CACHE_CS
* RESERVED6[7..6] - (RO) Reserved bits
* PSE_HIF_QOP_Q_OPER_CS[11..8] - (RO) PSE debug state for HIF_QOP_Q_OPER_CS
* PSE_HIF_QOP_RL_OCP_CS[13..12] - (RO) PSE debug state for HIF_QOP_RL_OCP_CS
* RESERVED14[15..14] - (RO) Reserved bits
* PSE_HIF_QOP_PL_OCP_CS[17..16] - (RO) PSE debug state for HIF_QOP_PL_OCP_CS
* RESERVED18[19..18] - (RO) Reserved bits
* PSE_HIF_QOP_ALLOCATE_CS[22..20] - (RO) PSE debug state for HIF_QOP_ALLOCATE_CS
* RESERVED23[23] - (RO) Reserved bits
* PSE_HIF_RX_DOP_PBUF_CS[26..24] - (RO) PSE debug state for HIF_RX_DOP_PBUF_CS
* RESERVED27[27] - (RO) Reserved bits
* PSE_HIF_RX_DOP_CACHE_CS[29..28] - (RO) PSE debug state for HIF_RX_DOP_CACHE_CS
* RESERVED30[31..30] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_RX_DOP_CACHE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_02_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_RX_DOP_CACHE_CS_MASK \
	0x30000000 /* PSE_HIF_RX_DOP_CACHE_CS[29..28] */
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_RX_DOP_CACHE_CS_SHFT 28
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_RX_DOP_PBUF_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_02_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_RX_DOP_PBUF_CS_MASK \
	0x07000000 /* PSE_HIF_RX_DOP_PBUF_CS[26..24] */
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_RX_DOP_PBUF_CS_SHFT  24
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_ALLOCATE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_02_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_ALLOCATE_CS_MASK \
	0x00700000 /* PSE_HIF_QOP_ALLOCATE_CS[22..20] */
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_ALLOCATE_CS_SHFT 20
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_PL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_02_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_PL_OCP_CS_MASK \
	0x00030000 /* PSE_HIF_QOP_PL_OCP_CS[17..16] */
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_PL_OCP_CS_SHFT   16
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_RL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_02_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_RL_OCP_CS_MASK \
	0x00003000 /* PSE_HIF_QOP_RL_OCP_CS[13..12] */
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_RL_OCP_CS_SHFT   12
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_Q_OPER_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_02_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_Q_OPER_CS_MASK \
	0x00000F00 /* PSE_HIF_QOP_Q_OPER_CS[11..8] */
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_QOP_Q_OPER_CS_SHFT   8
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_DOP_CACHE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_02_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_DOP_CACHE_CS_MASK \
	0x00000030 /* PSE_HIF_DOP_CACHE_CS[5..4] */
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_DOP_CACHE_CS_SHFT    4
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_DOP_PBUF_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_02_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_DOP_PBUF_CS_MASK \
	0x00000007 /* PSE_HIF_DOP_PBUF_CS[2..0] */
#define WF_PSE_TOP_PSE_SEEK_CR_02_PSE_HIF_DOP_PBUF_CS_SHFT     0

/*
* ---PSE_SEEK_CR_03 (0x820C8000 + 0x3DC)---
* PSE_CPU_DOP_PBUF_CS[2..0] - (RO) PSE debug state for CPU_DOP_PBUF_CS
* RESERVED3[3] - (RO) Reserved bits
* PSE_CPU_DOP_CACHE_CS[5..4] - (RO) PSE debug state for CPU_DOP_CACHE_CS
* RESERVED6[7..6] - (RO) Reserved bits
* PSE_CPU_QOP_Q_OPER_CS[11..8] - (RO) PSE debug state for CPU_QOP_Q_OPER_CS
* PSE_CPU_QOP_RL_OCP_CS[13..12] - (RO) PSE debug state for CPU_QOP_RL_OCP_CS
* RESERVED14[15..14] - (RO) Reserved bits
* PSE_CPU_QOP_PL_OCP_CS[17..16] - (RO) PSE debug state for CPU_QOP_PL_OCP_CS
* RESERVED18[19..18] - (RO) Reserved bits
* PSE_CPU_QOP_ALLOCATE_CS[22..20] - (RO) PSE debug state for CPU_QOP_ALLOCATE_CS
* RESERVED23[31..23] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_ALLOCATE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_03_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_ALLOCATE_CS_MASK \
	0x00700000 /* PSE_CPU_QOP_ALLOCATE_CS[22..20] */
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_ALLOCATE_CS_SHFT 20
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_PL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_03_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_PL_OCP_CS_MASK \
	0x00030000 /* PSE_CPU_QOP_PL_OCP_CS[17..16] */
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_PL_OCP_CS_SHFT   16
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_RL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_03_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_RL_OCP_CS_MASK \
	0x00003000 /* PSE_CPU_QOP_RL_OCP_CS[13..12] */
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_RL_OCP_CS_SHFT   12
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_Q_OPER_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_03_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_Q_OPER_CS_MASK \
	0x00000F00 /* PSE_CPU_QOP_Q_OPER_CS[11..8] */
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_QOP_Q_OPER_CS_SHFT   8
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_DOP_CACHE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_03_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_DOP_CACHE_CS_MASK \
	0x00000030 /* PSE_CPU_DOP_CACHE_CS[5..4] */
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_DOP_CACHE_CS_SHFT    4
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_DOP_PBUF_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_03_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_DOP_PBUF_CS_MASK \
	0x00000007 /* PSE_CPU_DOP_PBUF_CS[2..0] */
#define WF_PSE_TOP_PSE_SEEK_CR_03_PSE_CPU_DOP_PBUF_CS_SHFT     0

/*
* ---PSE_SEEK_CR_04 (0x820C8000 + 0x3E0)---
* PSE_WF_DOP_PBUF_CS[2..0] - (RO) PSE debug state for WF_DOP_PBUF_CS
* RESERVED3[3] - (RO) Reserved bits
* PSE_WF_DOP_CACHE_CS[5..4] - (RO) PSE debug state for WF_DOP_CACHE_CS
* RESERVED6[7..6] - (RO) Reserved bits
* PSE_WF_QOP_Q_OPER_CS[11..8] - (RO) PSE debug state for WF_QOP_Q_OPER_CS
* PSE_WF_QOP_RL_OCP_CS[13..12] - (RO) PSE debug state for WF_QOP_RL_OCP_CS
* RESERVED14[15..14] - (RO) Reserved bits
* PSE_WF_QOP_PL_OCP_CS[17..16] - (RO) PSE debug state for WF_QOP_PL_OCP_CS
* RESERVED18[19..18] - (RO) Reserved bits
* PSE_WF_QOP_ALLOCATE_CS[22..20] - (RO) PSE debug state for WF_QOP_ALLOCATE_CS
* RESERVED23[31..23] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_ALLOCATE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_04_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_ALLOCATE_CS_MASK \
	0x00700000 /* PSE_WF_QOP_ALLOCATE_CS[22..20] */
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_ALLOCATE_CS_SHFT  20
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_PL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_04_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_PL_OCP_CS_MASK \
	0x00030000 /* PSE_WF_QOP_PL_OCP_CS[17..16] */
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_PL_OCP_CS_SHFT    16
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_RL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_04_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_RL_OCP_CS_MASK \
	0x00003000 /* PSE_WF_QOP_RL_OCP_CS[13..12] */
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_RL_OCP_CS_SHFT    12
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_Q_OPER_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_04_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_Q_OPER_CS_MASK \
	0x00000F00 /* PSE_WF_QOP_Q_OPER_CS[11..8] */
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_QOP_Q_OPER_CS_SHFT    8
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_DOP_CACHE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_04_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_DOP_CACHE_CS_MASK \
	0x00000030 /* PSE_WF_DOP_CACHE_CS[5..4] */
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_DOP_CACHE_CS_SHFT     4
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_DOP_PBUF_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_04_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_DOP_PBUF_CS_MASK \
	0x00000007 /* PSE_WF_DOP_PBUF_CS[2..0] */
#define WF_PSE_TOP_PSE_SEEK_CR_04_PSE_WF_DOP_PBUF_CS_SHFT      0

/*
* ---PSE_SEEK_CR_05 (0x820C8000 + 0x3E4)---
* PSE_MDP_DOP_PBUF_CS[2..0] - (RO) PSE debug state for MDP_DOP_PBUF_CS
* RESERVED3[3] - (RO) Reserved bits
* PSE_MDP_DOP_CACHE_CS[5..4] - (RO) PSE debug state for MDP_DOP_CACHE_CS
* RESERVED6[7..6] - (RO) Reserved bits
* PSE_MDP_QOP_Q_OPER_CS[11..8] - (RO) PSE debug state for MDP_QOP_Q_OPER_CS
* PSE_MDP_QOP_RL_OCP_CS[13..12] - (RO) PSE debug state for MDP_QOP_RL_OCP_CS
* RESERVED14[15..14] - (RO) Reserved bits
* PSE_MDP_QOP_PL_OCP_CS[17..16] - (RO) PSE debug state for MDP_QOP_PL_OCP_CS
* RESERVED18[19..18] - (RO) Reserved bits
* PSE_MDP_QOP_ALLOCATE_CS[22..20] - (RO) PSE debug state for MDP_QOP_ALLOCATE_CS
* RESERVED23[31..23] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_ALLOCATE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_05_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_ALLOCATE_CS_MASK \
	0x00700000 /* PSE_MDP_QOP_ALLOCATE_CS[22..20] */
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_ALLOCATE_CS_SHFT 20
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_PL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_05_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_PL_OCP_CS_MASK \
	0x00030000 /* PSE_MDP_QOP_PL_OCP_CS[17..16] */
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_PL_OCP_CS_SHFT   16
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_RL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_05_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_RL_OCP_CS_MASK \
	0x00003000 /* PSE_MDP_QOP_RL_OCP_CS[13..12] */
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_RL_OCP_CS_SHFT   12
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_Q_OPER_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_05_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_Q_OPER_CS_MASK \
	0x00000F00 /* PSE_MDP_QOP_Q_OPER_CS[11..8] */
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_QOP_Q_OPER_CS_SHFT   8
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_DOP_CACHE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_05_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_DOP_CACHE_CS_MASK \
	0x00000030 /* PSE_MDP_DOP_CACHE_CS[5..4] */
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_DOP_CACHE_CS_SHFT    4
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_DOP_PBUF_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_05_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_DOP_PBUF_CS_MASK \
	0x00000007 /* PSE_MDP_DOP_PBUF_CS[2..0] */
#define WF_PSE_TOP_PSE_SEEK_CR_05_PSE_MDP_DOP_PBUF_CS_SHFT     0

/*
* ---PSE_SEEK_CR_06 (0x820C8000 + 0x3E8)---
* PSE_SEC_DOP_PBUF_CS[2..0] - (RO) PSE debug state for SEC_DOP_PBUF_CS
* RESERVED3[3] - (RO) Reserved bits
* PSE_SEC_DOP_CACHE_CS[5..4] - (RO) PSE debug state for SEC_DOP_CACHE_CS
* RESERVED6[7..6] - (RO) Reserved bits
* PSE_SEC_QOP_Q_OPER_CS[11..8] - (RO) PSE debug state for SEC_QOP_Q_OPER_CS
* PSE_SEC_QOP_RL_OCP_CS[13..12] - (RO) PSE debug state for SEC_QOP_RL_OCP_CS
* RESERVED14[15..14] - (RO) Reserved bits
* PSE_SEC_QOP_PL_OCP_CS[17..16] - (RO) PSE debug state for SEC_QOP_PL_OCP_CS
* RESERVED18[19..18] - (RO) Reserved bits
* PSE_SEC_QOP_ALLOCATE_CS[22..20] - (RO) PSE debug state for SEC_QOP_ALLOCATE_CS
* RESERVED23[31..23] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_ALLOCATE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_06_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_ALLOCATE_CS_MASK \
	0x00700000 /* PSE_SEC_QOP_ALLOCATE_CS[22..20] */
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_ALLOCATE_CS_SHFT 20
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_PL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_06_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_PL_OCP_CS_MASK \
	0x00030000 /* PSE_SEC_QOP_PL_OCP_CS[17..16] */
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_PL_OCP_CS_SHFT   16
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_RL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_06_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_RL_OCP_CS_MASK \
	0x00003000 /* PSE_SEC_QOP_RL_OCP_CS[13..12] */
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_RL_OCP_CS_SHFT   12
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_Q_OPER_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_06_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_Q_OPER_CS_MASK \
	0x00000F00 /* PSE_SEC_QOP_Q_OPER_CS[11..8] */
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_QOP_Q_OPER_CS_SHFT   8
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_DOP_CACHE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_06_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_DOP_CACHE_CS_MASK \
	0x00000030 /* PSE_SEC_DOP_CACHE_CS[5..4] */
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_DOP_CACHE_CS_SHFT    4
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_DOP_PBUF_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_06_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_DOP_PBUF_CS_MASK \
	0x00000007 /* PSE_SEC_DOP_PBUF_CS[2..0] */
#define WF_PSE_TOP_PSE_SEEK_CR_06_PSE_SEC_DOP_PBUF_CS_SHFT     0

/*
* ---PSE_SEEK_CR_07 (0x820C8000 + 0x3EC)---
* PSE_PLE_DOP_PBUF_CS[2..0] - (RO) PSE debug state for PLE_DOP_PBUF_CS
* RESERVED3[3] - (RO) Reserved bits
* PSE_PLE_DOP_CACHE_CS[5..4] - (RO) PSE debug state for PLE_DOP_CACHE_CS
* RESERVED6[7..6] - (RO) Reserved bits
* PSE_PLE_QOP_Q_OPER_CS[11..8] - (RO) PSE debug state for PLE_QOP_Q_OPER_CS
* PSE_PLE_QOP_RL_OCP_CS[13..12] - (RO) PSE debug state for PLE_QOP_RL_OCP_CS
* RESERVED14[15..14] - (RO) Reserved bits
* PSE_PLE_QOP_PL_OCP_CS[17..16] - (RO) PSE debug state for PLE_QOP_PL_OCP_CS
* RESERVED18[19..18] - (RO) Reserved bits
* PSE_PLE_QOP_ALLOCATE_CS[22..20] - (RO) PSE debug state for PLE_QOP_ALLOCATE_CS
* RESERVED23[31..23] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_ALLOCATE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_07_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_ALLOCATE_CS_MASK \
	0x00700000 /* PSE_PLE_QOP_ALLOCATE_CS[22..20] */
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_ALLOCATE_CS_SHFT 20
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_PL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_07_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_PL_OCP_CS_MASK \
	0x00030000 /* PSE_PLE_QOP_PL_OCP_CS[17..16] */
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_PL_OCP_CS_SHFT   16
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_RL_OCP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_07_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_RL_OCP_CS_MASK \
	0x00003000 /* PSE_PLE_QOP_RL_OCP_CS[13..12] */
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_RL_OCP_CS_SHFT   12
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_Q_OPER_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_07_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_Q_OPER_CS_MASK \
	0x00000F00 /* PSE_PLE_QOP_Q_OPER_CS[11..8] */
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_QOP_Q_OPER_CS_SHFT   8
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_DOP_CACHE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_07_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_DOP_CACHE_CS_MASK \
	0x00000030 /* PSE_PLE_DOP_CACHE_CS[5..4] */
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_DOP_CACHE_CS_SHFT    4
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_DOP_PBUF_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_07_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_DOP_PBUF_CS_MASK \
	0x00000007 /* PSE_PLE_DOP_PBUF_CS[2..0] */
#define WF_PSE_TOP_PSE_SEEK_CR_07_PSE_PLE_DOP_PBUF_CS_SHFT     0

/*
* ---PSE_SEEK_CR_08 (0x820C8000 + 0x3F0)---
* PSE_AMSDU_DOP_PBUF_CS[2..0] - (RO) PSE debug state for AMSDU_DOP_PBUF_CS
* RESERVED3[3] - (RO) Reserved bits
* PSE_AMSDU_DOP_CACHE_CS[5..4] - (RO) PSE debug state for AMSDU_DOP_CACHE_CS
* RESERVED6[7..6] - (RO) Reserved bits
* CPU_Q_OP_CS[11..8] - (RO) PSE debug state for CPI_Q_OP_CS
* ARB_REQ_CS[14..12] - (RO) PSE debug state for ARB_REQ_CS
* RESERVED15[15] - (RO) Reserved bits
* APB_REQ_CS[18..16] - (RO) PSE debug state for APB_REQ_CS
* RESERVED19[31..19] - (RO) Reserved bits
*/
#define WF_PSE_TOP_PSE_SEEK_CR_08_APB_REQ_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_08_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_08_APB_REQ_CS_MASK \
	0x00070000 /* APB_REQ_CS[18..16] */
#define WF_PSE_TOP_PSE_SEEK_CR_08_APB_REQ_CS_SHFT              16
#define WF_PSE_TOP_PSE_SEEK_CR_08_ARB_REQ_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_08_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_08_ARB_REQ_CS_MASK \
	0x00007000 /* ARB_REQ_CS[14..12] */
#define WF_PSE_TOP_PSE_SEEK_CR_08_ARB_REQ_CS_SHFT              12
#define WF_PSE_TOP_PSE_SEEK_CR_08_CPU_Q_OP_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_08_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_08_CPU_Q_OP_CS_MASK \
	0x00000F00 /* CPU_Q_OP_CS[11..8] */
#define WF_PSE_TOP_PSE_SEEK_CR_08_CPU_Q_OP_CS_SHFT             8
#define WF_PSE_TOP_PSE_SEEK_CR_08_PSE_AMSDU_DOP_CACHE_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_08_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_08_PSE_AMSDU_DOP_CACHE_CS_MASK \
	0x00000030 /* PSE_AMSDU_DOP_CACHE_CS[5..4] */
#define WF_PSE_TOP_PSE_SEEK_CR_08_PSE_AMSDU_DOP_CACHE_CS_SHFT  4
#define WF_PSE_TOP_PSE_SEEK_CR_08_PSE_AMSDU_DOP_PBUF_CS_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_08_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_08_PSE_AMSDU_DOP_PBUF_CS_MASK \
	0x00000007 /* PSE_AMSDU_DOP_PBUF_CS[2..0] */
#define WF_PSE_TOP_PSE_SEEK_CR_08_PSE_AMSDU_DOP_PBUF_CS_SHFT   0

/*
* ---PSE_SEEK_CR_09 (0x820C8000 + 0x3F4)---
* DOUBLE_RLS_GET_BUF_GID[3..0] - (RO) PSE double RLS FID created by
* 0: Host DMA TX
* 1: CMD
* 2: CPU
* 3: LMAC RX data
* 4: LMAC RX vector
* 5: LMAC TXS
* 6: PLE TX free done event
* 7: MDP Rx
* 8: LMAC TXCMD/RX report
* 9: SPL
* 10: MDP TX 0
* 11: MDP TX 1
* 12: MCU DMA Tx
* RESERVED4[15..4] - (RO) Reserved bits
* DOUBLE_RLS_FID[27..16] - (RO) PSE double RLS FID PAGE
* RESERVED28[30..28] - (RO) Reserved bits
* DOUBLE_RLS_ERROR[31] - (RO) PSE double RLS error flag
*/
#define WF_PSE_TOP_PSE_SEEK_CR_09_DOUBLE_RLS_ERROR_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_09_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_09_DOUBLE_RLS_ERROR_MASK \
	0x80000000 /* DOUBLE_RLS_ERROR[31] */
#define WF_PSE_TOP_PSE_SEEK_CR_09_DOUBLE_RLS_ERROR_SHFT        31
#define WF_PSE_TOP_PSE_SEEK_CR_09_DOUBLE_RLS_FID_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_09_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_09_DOUBLE_RLS_FID_MASK \
	0x0FFF0000 /* DOUBLE_RLS_FID[27..16] */
#define WF_PSE_TOP_PSE_SEEK_CR_09_DOUBLE_RLS_FID_SHFT          16
#define WF_PSE_TOP_PSE_SEEK_CR_09_DOUBLE_RLS_GET_BUF_GID_ADDR \
	WF_PSE_TOP_PSE_SEEK_CR_09_ADDR
#define WF_PSE_TOP_PSE_SEEK_CR_09_DOUBLE_RLS_GET_BUF_GID_MASK \
	0x0000000F /* DOUBLE_RLS_GET_BUF_GID[3..0] */
#define WF_PSE_TOP_PSE_SEEK_CR_09_DOUBLE_RLS_GET_BUF_GID_SHFT  0

#ifdef __cplusplus
}
#endif

#endif /* __WF_PSE_TOP_REGS_H__ */
