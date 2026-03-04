/**
 *******************************************************************************
 *  @file           packet.c
 *  @brief          パケット構築・解析モジュール。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif

#include <simplecomm_const.h>
#include <simplecomm_type.h>

#include "packet.h"

/**
 *******************************************************************************
 *  @brief          データパケットを構築します。
 *  @param[out]     packet      構築結果を格納するパケット構造体へのポインタ。
 *  @param[in]      seq_num     通番。
 *  @param[in]      data        ペイロードデータへのポインタ。
 *  @param[in]      len         ペイロードのバイト数。
 *  @return         成功時は COMM_SUCCESS、失敗時は COMM_ERROR を返します。
 *
 *  @details
 *  各フィールドをネットワークバイトオーダーに変換してパケットを構築します。
 *******************************************************************************
 */
int packet_build_data(CommPacket *packet, uint32_t seq_num,
                      const void *data, size_t len)
{
    if (packet == NULL || data == NULL || len == 0 || len > COMM_MAX_PAYLOAD)
    {
        return COMM_ERROR;
    }

    memset(packet, 0, sizeof(*packet));
    packet->seq_num     = htonl(seq_num);
    packet->ack_num     = 0;
    packet->flags       = htons(COMM_FLAG_DATA);
    packet->payload_len = htons((uint16_t)len);
    memcpy(packet->payload, data, len);

    return COMM_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          ACK パケットを構築します。
 *  @param[out]     packet      構築結果を格納するパケット構造体へのポインタ。
 *  @param[in]      ack_num     確認応答番号。
 *  @return         成功時は COMM_SUCCESS、失敗時は COMM_ERROR を返します。
 *******************************************************************************
 */
int packet_build_ack(CommPacket *packet, uint32_t ack_num)
{
    if (packet == NULL)
    {
        return COMM_ERROR;
    }

    memset(packet, 0, sizeof(*packet));
    packet->seq_num     = 0;
    packet->ack_num     = htonl(ack_num);
    packet->flags       = htons(COMM_FLAG_ACK);
    packet->payload_len = 0;

    return COMM_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          NACK パケットを構築します。
 *  @param[out]     packet      構築結果を格納するパケット構造体へのポインタ。
 *  @param[in]      nack_num    再送要求する通番。
 *  @return         成功時は COMM_SUCCESS、失敗時は COMM_ERROR を返します。
 *******************************************************************************
 */
int packet_build_nack(CommPacket *packet, uint32_t nack_num)
{
    if (packet == NULL)
    {
        return COMM_ERROR;
    }

    memset(packet, 0, sizeof(*packet));
    packet->seq_num     = 0;
    packet->ack_num     = htonl(nack_num);
    packet->flags       = htons(COMM_FLAG_NACK);
    packet->payload_len = 0;

    return COMM_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          受信バイト列をパケット構造体に解析します。
 *  @param[out]     packet      解析結果を格納するパケット構造体へのポインタ。
 *  @param[in]      buf         受信バイト列へのポインタ。
 *  @param[in]      buf_len     受信バイト列の長さ。
 *  @return         成功時は COMM_SUCCESS、失敗時は COMM_ERROR を返します。
 *
 *  @details
 *  各フィールドをホストバイトオーダーに変換して構造体に格納します。
 *******************************************************************************
 */
int packet_parse(CommPacket *packet, const void *buf, size_t buf_len)
{
    const size_t header_size = sizeof(uint32_t) * 2 + sizeof(uint16_t) * 2;

    if (packet == NULL || buf == NULL || buf_len < header_size)
    {
        return COMM_ERROR;
    }

    memcpy(packet, buf, buf_len < sizeof(*packet) ? buf_len : sizeof(*packet));

    packet->seq_num     = ntohl(packet->seq_num);
    packet->ack_num     = ntohl(packet->ack_num);
    packet->flags       = ntohs(packet->flags);
    packet->payload_len = ntohs(packet->payload_len);

    if (packet->payload_len > COMM_MAX_PAYLOAD)
    {
        return COMM_ERROR;
    }

    return COMM_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          パケットのヘッダー + ペイロードの合計バイト数を返します。
 *  @param[in]      packet  対象のパケット構造体へのポインタ。
 *  @return         パケットの送信サイズ (バイト)。packet が NULL の場合は 0。
 *
 *  @details
 *  UDP 送信時に sendto() へ渡すバイト数を求めるために使用します。
 *******************************************************************************
 */
size_t packet_wire_size(const CommPacket *packet)
{
    const size_t header_size = sizeof(uint32_t) * 2 + sizeof(uint16_t) * 2;

    if (packet == NULL)
    {
        return 0;
    }

    return header_size + ntohs(packet->payload_len);
}
