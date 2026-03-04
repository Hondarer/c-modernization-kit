/**
 *******************************************************************************
 *  @file           packet.h
 *  @brief          パケット構築・解析モジュールの内部ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef PACKET_H
#define PACKET_H

#include <stddef.h>
#include <stdint.h>

#include <porter_type.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern int    packet_build_data(PotrPacket *packet, uint32_t seq_num,
                                    const void *data, size_t len);
    extern int    packet_build_ack(PotrPacket *packet, uint32_t ack_num);
    extern int    packet_build_nack(PotrPacket *packet, uint32_t nack_num);
    extern int    packet_parse(PotrPacket *packet, const void *buf, size_t buf_len);
    extern size_t packet_wire_size(const PotrPacket *packet);

#ifdef __cplusplus
}
#endif

#endif /* PACKET_H */
