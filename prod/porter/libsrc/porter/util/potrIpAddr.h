/**
 *******************************************************************************
 *  @file           potrIpAddr.h
 *  @brief          IPv4 アドレス変換ユーティリティ (内部用)。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/05
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef POTR_IP_ADDR_H
#define POTR_IP_ADDR_H

#ifndef _WIN32
    #include <arpa/inet.h>
    #include <netinet/in.h>
#else /* _WIN32 */
    #include <winsock2.h>
    #include <ws2tcpip.h>
#endif /* _WIN32 */

int parse_ipv4_addr(const char *ip_str, struct in_addr *out_addr);

/**
 *  @brief  ホスト名または IPv4 アドレス文字列を struct in_addr に解決する。
 *  @details
 *  getaddrinfo() を使用して AF_INET で名前解決する。
 *  複数のアドレスが返された場合は先頭のアドレスを採用する。
 */
int resolve_ipv4_addr(const char *host, struct in_addr *out_addr);

#endif /* POTR_IP_ADDR_H */
