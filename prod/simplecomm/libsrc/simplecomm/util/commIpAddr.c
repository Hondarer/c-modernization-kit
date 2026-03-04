/**
 *******************************************************************************
 *  @file           commIpAddr.c
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

#include <simplecomm_const.h>

#include "commIpAddr.h"

/* IPv4 文字列をネットワークバイトオーダーへ変換する。 */
int parse_ipv4_addr(const char *ip_str, struct in_addr *out_addr)
{
    if (ip_str == NULL || out_addr == NULL)
    {
        return COMM_ERROR;
    }

    return (inet_pton(AF_INET, ip_str, out_addr) == 1) ? COMM_SUCCESS : COMM_ERROR;
}
