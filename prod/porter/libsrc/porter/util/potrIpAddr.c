/**
 *******************************************************************************
 *  @file           potrIpAddr.c
 *  @brief          IPv4 アドレス変換ユーティリティ (内部用)。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/05
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stddef.h>

#include <porter_const.h>

#include "potrIpAddr.h"

/* IPv4 文字列をネットワークバイトオーダーへ変換する。 */
int parse_ipv4_addr(const char *ip_str, struct in_addr *out_addr)
{
    if (ip_str == NULL || out_addr == NULL)
    {
        return POTR_ERROR;
    }

    return (inet_pton(AF_INET, ip_str, out_addr) == 1) ? POTR_SUCCESS : POTR_ERROR;
}
