  /******************************************************************************
  ** Copyright (C), 2019-2029, Oplus Mobile Comm Corp., Ltd
  ** File: - oplus_prase_package.c
  ** Description: prase wake up package type
  **
  ** Version: 1.0
  ** Date : 2023/06/01
  ** CONNECTIVITY.WIFI.HARDWARE.26106
  ** TAG: OPLUS_FEATURE_WIFI_HARDWARE_POWER
  ** ------------------------------- Revision History: ----------------------------
  ** <author>  wuguotian                    <data>        <version>       <desc>
  ** ------------------------------------------------------------------------------
  **CONNECTIVITY.WIFI.HARDWARE.POWER.26106 2023/06/01     1.0      OPLUS_FEATURE_WIFI_HARDWARE_POWER
  *******************************************************************************/

#ifndef _OPLUS_PARSE_PACKAGE_H
#define _OPLUS_PARSE_PACKAGE_H

#include "precomp.h"

uint16_t ParseRxPackage(struct SW_RFB *prSwRfb);

enum enum_oplus_wifi_wkup_reason {
    OPLUS_WIFI_RX_EVENT_Uni = 0,    /* wakeup by RX UNI FW EVT */
    OPLUS_WIFI_RX_EVENT,            /* wakeup by RX FW EVT */
    OPLUS_WIFI_RX_MGMT,             /* wakeup by RX Management frame */
    OPLUS_WIFI_OUT_OF_DEF,          /* wakeup by RX NO define */
    OPLUS_WIFI_RX_DATA,             /* wakeup by RX data */
    OPLUS_WIFI_RX_FRAME,            /* wakeup by RX Control/Data frame */
    OPLUS_WIFI_RX_OTHERS,           /* wakeup by RX unknown */
};

uint16_t oplusStatsParseIPV4Info (struct sk_buff *skb, uint8_t *pucEthBody);
#endif /* _OPLUS_PARSE_PACKAGE_H */
