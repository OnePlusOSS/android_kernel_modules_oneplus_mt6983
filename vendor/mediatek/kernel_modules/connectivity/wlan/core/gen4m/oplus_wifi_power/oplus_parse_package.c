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
**CONNECTIVITY.WIFI.HARDWARE.POWER.26106 2023/06/01        1.0    OPLUS_FEATURE_WIFI_HARDWARE_POWER
*******************************************************************************/

#include "oplus_parse_package.h"
#include "precomp.h"

uint16_t oplusStatsParseUDPInfo(struct sk_buff *skb, uint8_t *pucEthBody)
{
    uint8_t *pucUdp = &pucEthBody[20];
    uint16_t u2UdpDstPort;
    uint16_t u2UdpSrcPort;
    uint16_t packageType = IP_PRO_UDP;

    u2UdpDstPort = (pucUdp[2] << 8) | pucUdp[3];
    u2UdpSrcPort = (pucUdp[0] << 8) | pucUdp[1];
    if (u2UdpDstPort == UDP_PORT_DHCPS || u2UdpDstPort == UDP_PORT_DHCPC) {
        packageType = UDP_PORT_DHCPS;
    } else if (u2UdpSrcPort == UDP_PORT_DNS ||
        u2UdpDstPort == UDP_PORT_DNS) {
        packageType = UDP_PORT_DNS;
    }
    return packageType;
}

uint16_t oplusStatsParseIPV4Info(struct sk_buff *skb, uint8_t *pucEthBody)
{
    /* IP header without options */
    uint8_t ucIpProto = pucEthBody[9];
    uint8_t ucIpVersion =
        (pucEthBody[0] & IPVH_VERSION_MASK)
            >> IPVH_VERSION_OFFSET;
    uint16_t u2IpId = pucEthBody[4] << 8 | pucEthBody[5];
    uint16_t packageType = ETH_P_IPV4;

    if (ucIpVersion != IPVERSION) {
        return packageType;
    }

    GLUE_SET_PKT_IP_ID(skb, u2IpId);
    switch (ucIpProto) {
        case IP_PRO_ICMP: {
            packageType = IP_PRO_ICMP;
            break;
        }
        case IP_PRO_UDP: {
            packageType = oplusStatsParseUDPInfo(skb, pucEthBody);
            break;
        }
        default:
            break;
    }
    return packageType;
}

uint16_t oplusStatsParsePktInfo(uint8_t *pucPkt, struct sk_buff *skb)
{
    /* get ethernet protocol */
    uint16_t u2EtherType =
        (pucPkt[ETH_TYPE_LEN_OFFSET] << 8)
            | (pucPkt[ETH_TYPE_LEN_OFFSET + 1]);
    uint8_t *pucEthBody = &pucPkt[ETH_HLEN];
    uint16_t packageType = 0;

    switch (u2EtherType) {
        case ETH_P_ARP: {
            packageType = ETH_P_ARP;
            break;
        }
        case ETH_P_IPV4: {
            packageType = oplusStatsParseIPV4Info(skb, pucEthBody);
            break;
        }
        case ETH_P_IPV6: {
            packageType = ETH_P_IPV6;
            break;
        }
        case ETH_P_1X: {
            packageType = ETH_P_1X;
            break;
        }
#if CFG_SUPPORT_WAPI
        case ETH_WPI_1X: {
            packageType = ETH_WPI_1X;
            break;
        }
#endif
        case ETH_PRO_TDLS: {
            packageType = ETH_PRO_TDLS;
            break;
        }
        default:
            break;
    }
    return packageType;
}

uint16_t ParseRxPackage(struct SW_RFB *prSwRfb)
{
    uint8_t *pPkt = NULL;
    struct sk_buff *skb = NULL;
    uint16_t packageType = 0;

    if (prSwRfb->u2PacketLen <= ETHER_HEADER_LEN) {
        return packageType;
    }

    pPkt = prSwRfb->pvHeader;
    if (!pPkt) {
        return packageType;
    }

    skb = (struct sk_buff *)(prSwRfb->pvPacket);
    if (!skb) {
        return packageType;
    }

    packageType = oplusStatsParsePktInfo(pPkt, skb);

    return packageType;
}
