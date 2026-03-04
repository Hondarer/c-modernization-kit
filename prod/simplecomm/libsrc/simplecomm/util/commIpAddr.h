/**
 *******************************************************************************
 *  @file           commIpAddr.h
 *  @brief          IPv4 アドレス変換ユーティリティ (内部用)。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/05
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef COMM_IP_ADDR_H
#define COMM_IP_ADDR_H

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
#endif

int parse_ipv4_addr(const char *ip_str, struct in_addr *out_addr);

#endif /* COMM_IP_ADDR_H */
