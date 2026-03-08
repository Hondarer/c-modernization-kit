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
 *  @brief          正常終了通知 (FIN) パケットを構築します。
 *  @param[out]     packet      構築結果を格納するパケット構造体へのポインタ。
 *  @param[in]      shdr        セッション識別ヘッダーへのポインタ。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  送信者が potrClose 時に送出する終了通知パケットです。ペイロードなし。\n
 *  受信者はこのパケットを受け取ると即座に POTR_EVENT_DISCONNECTED を発火します。
 *******************************************************************************
 */
int packet_build_fin(PotrPacket *packet, const PotrPacketSessionHdr *shdr)
{
    if (packet == NULL || shdr == NULL)
    {
        return POTR_ERROR;
    }

    memset(packet, 0, sizeof(*packet));
    fill_session_hdr(packet, shdr);
    packet->seq_num     = 0;
    packet->ack_num     = 0;
    packet->flags       = htons(POTR_FLAG_FIN);
    packet->payload_len = 0;

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          データパケット (パックコンテナ) を構築します。
 *  @param[out]     out             構築結果を格納するパケット構造体へのポインタ。
 *  @param[in]      shdr            セッション識別ヘッダーへのポインタ。
 *  @param[in]      seq_num         外側パケットの通番。再送・順序整列に使用する。
 *  @param[in]      packed_payload  送信スレッドが構築したサブパケット列。
 *  @param[in]      payload_len     packed_payload のバイト数。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  すべてのデータパケットはパックコンテナ形式で送受信します。\n
 *  サブパケットが 1 件のみの場合も同じ形式を使用します。\n
 *  再送・順序整列の単位は外側パケット (本関数が構築する UDP ペイロード) であり、
 *  通番は外側パケットの seq_num フィールドで管理します。\n
 *  サブパケットの形式は flags(2) + payload_len(2) + payload(N) です。\n
 *  受信者は POTR_FLAG_DATA を検出後 packet_unpack_next() でサブパケットを展開します。
 *******************************************************************************
 */
int packet_build_packed(PotrPacket *out, const PotrPacketSessionHdr *shdr,
                        uint32_t seq_num,
                        const void *packed_payload, size_t payload_len)
{
    if (out == NULL || shdr == NULL || packed_payload == NULL
        || payload_len == 0 || payload_len > POTR_MAX_PAYLOAD)
    {
        return POTR_ERROR;
    }

    memset(out, 0, sizeof(*out));
    fill_session_hdr(out, shdr);
    out->seq_num     = htonl(seq_num);
    out->ack_num     = 0;
    out->flags       = htons(POTR_FLAG_DATA);
    out->payload_len = htons((uint16_t)payload_len);
    memcpy(out->payload, packed_payload, payload_len);

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          データパケットから次のサブパケットを取り出します。
 *  @param[in]      container  packet_parse() 済みのデータパケット (POTR_FLAG_DATA)。
 *  @param[in,out]  offset     コンテナ payload 内の読み取り位置。呼び出し毎に更新。
 *  @param[out]     sub_out    取り出したサブパケットを格納する構造体へのポインタ。
 *  @return         サブパケットを取り出せた場合は POTR_SUCCESS、
 *                  末尾に達した場合またはエラーの場合は POTR_ERROR を返します。
 *
 *  @details
 *  サブパケットの形式は flags(2) + payload_len(2) + payload(N) です。\n
 *  通番は外側パケットで管理するためサブパケットに含まれません。\n
 *  container->payload_len はホストバイトオーダー (packet_parse() 変換済み) で参照します。\n
 *  sub_out の session 情報は container から引き継ぎます。
 *******************************************************************************
 */
int packet_unpack_next(const PotrPacket *container, size_t *offset,
                       PotrPacket *sub_out)
{
    const uint8_t *p;
    uint16_t       flags_nbo;
    uint16_t       plen_nbo;
    uint16_t       payload_len;

    if (container == NULL || offset == NULL || sub_out == NULL)
    {
        return POTR_ERROR;
    }

    /* 末尾チェック (container->payload_len はホストバイトオーダー) */
    if (*offset + POTR_PACKED_SUB_HDR_SIZE > (size_t)container->payload_len)
    {
        return POTR_ERROR;
    }

    p = container->payload + *offset;

    memcpy(&flags_nbo, p,     2);
    memcpy(&plen_nbo,  p + 2, 2);

    payload_len = ntohs(plen_nbo);

    if (*offset + POTR_PACKED_SUB_HDR_SIZE + payload_len > (size_t)container->payload_len
        || payload_len > POTR_MAX_PAYLOAD)
    {
        return POTR_ERROR;
    }

    memset(sub_out, 0, sizeof(*sub_out));
    sub_out->service_id      = container->service_id;
    sub_out->session_id      = container->session_id;
    sub_out->session_tv_sec  = container->session_tv_sec;
    sub_out->session_tv_nsec = container->session_tv_nsec;
    sub_out->ack_num         = 0;
    sub_out->flags           = ntohs(flags_nbo);
    sub_out->payload_len     = payload_len;
    memcpy(sub_out->payload, p + POTR_PACKED_SUB_HDR_SIZE, payload_len);

    *offset += POTR_PACKED_SUB_HDR_SIZE + payload_len;
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
