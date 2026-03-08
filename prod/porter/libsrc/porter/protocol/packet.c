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

#include <stddef.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif

#include <porter_const.h>
#include <porter_type.h>

#include "packet.h"

/* ヘッダー固定長: payload フィールドの開始オフセット */
#define PACKET_HEADER_SIZE (offsetof(PotrPacket, payload))

/* 64 ビット値をホストバイトオーダーからネットワークバイトオーダーへ変換する */
static uint64_t hton64(uint64_t v)
{
    uint32_t hi = htonl((uint32_t)(v >> 32));
    uint32_t lo = htonl((uint32_t)(v & 0xFFFFFFFFUL));
    return ((uint64_t)lo << 32) | (uint64_t)hi;
}

/* 64 ビット値をネットワークバイトオーダーからホストバイトオーダーへ変換する */
static uint64_t ntoh64(uint64_t v)
{
    return hton64(v); /* 対称変換 */
}

/* セッションヘッダーフィールドをパケットに NBO で書き込む */
static void fill_session_hdr(PotrPacket *packet, const PotrPacketSessionHdr *shdr)
{
    packet->service_id      = htonl((uint32_t)shdr->service_id);
    packet->session_id      = htonl(shdr->session_id);
    packet->session_tv_sec  = (int64_t)hton64((uint64_t)shdr->session_tv_sec);
    packet->session_tv_nsec = htonl((uint32_t)shdr->session_tv_nsec);
}

/**
 *******************************************************************************
 *  @brief          データパケットを構築します。
 *  @param[out]     packet      構築結果を格納するパケット構造体へのポインタ。
 *  @param[in]      shdr        セッション識別ヘッダーへのポインタ。
 *  @param[in]      seq_num     通番。
 *  @param[in]      data        ペイロードデータへのポインタ。
 *  @param[in]      len         ペイロードのバイト数。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  各フィールドをネットワークバイトオーダーに変換してパケットを構築します。
 *******************************************************************************
 */
int packet_build_data(PotrPacket *packet, const PotrPacketSessionHdr *shdr,
                      uint32_t seq_num, const void *data, size_t len)
{
    if (packet == NULL || shdr == NULL || data == NULL
        || len == 0 || len > POTR_MAX_PAYLOAD)
    {
        return POTR_ERROR;
    }

    memset(packet, 0, sizeof(*packet));
    fill_session_hdr(packet, shdr);
    packet->seq_num     = htonl(seq_num);
    packet->ack_num     = 0;
    packet->flags       = htons(POTR_FLAG_DATA);
    packet->payload_len = htons((uint16_t)len);
    memcpy(packet->payload, data, len);

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          NACK パケットを構築します。
 *  @param[out]     packet      構築結果を格納するパケット構造体へのポインタ。
 *  @param[in]      shdr        セッション識別ヘッダーへのポインタ。
 *  @param[in]      nack_num    再送要求する通番。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *******************************************************************************
 */
int packet_build_nack(PotrPacket *packet, const PotrPacketSessionHdr *shdr,
                      uint32_t nack_num)
{
    if (packet == NULL || shdr == NULL)
    {
        return POTR_ERROR;
    }

    memset(packet, 0, sizeof(*packet));
    fill_session_hdr(packet, shdr);
    packet->seq_num     = 0;
    packet->ack_num     = htonl(nack_num);
    packet->flags       = htons(POTR_FLAG_NACK);
    packet->payload_len = 0;

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          PING パケットを構築します。
 *  @param[out]     packet      構築結果を格納するパケット構造体へのポインタ。
 *  @param[in]      shdr        セッション識別ヘッダーへのポインタ。
 *  @param[in]      seq_num     通番 (ウィンドウ管理に使用)。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  ヘルスチェック要求パケットです。ペイロードなし (payload_len=0)。\n
 *  通番はデータパケットと同一の送信ウィンドウで管理されます。
 *******************************************************************************
 */
int packet_build_ping(PotrPacket *packet, const PotrPacketSessionHdr *shdr,
                      uint32_t seq_num)
{
    if (packet == NULL || shdr == NULL)
    {
        return POTR_ERROR;
    }

    memset(packet, 0, sizeof(*packet));
    fill_session_hdr(packet, shdr);
    packet->seq_num     = htonl(seq_num);
    packet->ack_num     = 0;
    packet->flags       = htons(POTR_FLAG_PING);
    packet->payload_len = 0;

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          再送不能通知 (REJECT) パケットを構築します。
 *  @param[out]     packet      構築結果を格納するパケット構造体へのポインタ。
 *  @param[in]      shdr        セッション識別ヘッダーへのポインタ。
 *  @param[in]      seq_num     再送不能な通番。ack_num フィールドに格納します。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  受信者から NACK を受け取ったが、送信ウィンドウに該当パケットが存在しない場合に
 *  送信者が返すパケットです。受信者はこのパケットを受け取ると即時 DISCONNECTED を
 *  発火し、欠落通番をスキップして後続パケットの配信を継続します。
 *******************************************************************************
 */
int packet_build_reject(PotrPacket *packet, const PotrPacketSessionHdr *shdr,
                        uint32_t seq_num)
{
    if (packet == NULL || shdr == NULL)
    {
        return POTR_ERROR;
    }

    memset(packet, 0, sizeof(*packet));
    fill_session_hdr(packet, shdr);
    packet->seq_num     = 0;
    packet->ack_num     = htonl(seq_num);
    packet->flags       = htons(POTR_FLAG_REJECT);
    packet->payload_len = 0;

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          受信バイト列をパケット構造体に解析します。
 *  @param[out]     packet      解析結果を格納するパケット構造体へのポインタ。
 *  @param[in]      buf         受信バイト列へのポインタ。
 *  @param[in]      buf_len     受信バイト列の長さ。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  各フィールドをホストバイトオーダーに変換して構造体に格納します。
 *******************************************************************************
 */
int packet_parse(PotrPacket *packet, const void *buf, size_t buf_len)
{
    if (packet == NULL || buf == NULL || buf_len < PACKET_HEADER_SIZE)
    {
        return POTR_ERROR;
    }

    memcpy(packet, buf, buf_len < sizeof(*packet) ? buf_len : sizeof(*packet));

    packet->service_id      = (int32_t)ntohl((uint32_t)packet->service_id);
    packet->session_id      = ntohl(packet->session_id);
    packet->session_tv_sec  = (int64_t)ntoh64((uint64_t)packet->session_tv_sec);
    packet->session_tv_nsec = (int32_t)ntohl((uint32_t)packet->session_tv_nsec);
    packet->seq_num         = ntohl(packet->seq_num);
    packet->ack_num         = ntohl(packet->ack_num);
    packet->flags           = ntohs(packet->flags);
    packet->payload_len     = ntohs(packet->payload_len);

    if (packet->payload_len > POTR_MAX_PAYLOAD)
    {
        return POTR_ERROR;
    }

    return POTR_SUCCESS;
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
size_t packet_wire_size(const PotrPacket *packet)
{
    if (packet == NULL)
    {
        return 0;
    }

    return PACKET_HEADER_SIZE + ntohs(packet->payload_len);
}
