/**
 *******************************************************************************
 *  @file           compress.h
 *  @brief          データ圧縮・解凍モジュールの内部ヘッダー。
 *  @author         Tetsuo Honda
 *  @date           2026/03/05
 *  @version        1.0.0
 *
 *  @details
 *  プラットフォームごとに異なる圧縮 API を共通インターフェースで抽象化します。\n
 *
 *  | OS      | 使用ライブラリ                                   | フォーマット        |
 *  | ------- | ------------------------------------------------ | ------------------- |
 *  | Linux   | zlib (deflate/inflate, windowBits = -15)         | raw DEFLATE         |
 *  | Windows | Compression API (MSZIP \| COMPRESS_RAW)          | raw DEFLATE         |
 *
 *  両 OS とも raw DEFLATE (RFC 1951) を出力するため、クロスプラットフォーム互換
 *  通信が可能です。
 *
 *  圧縮ペイロードのフォーマット:
 *  @code
    [元サイズ: uint32_t (ネットワークバイトオーダー)] [raw DEFLATE データ]
 *  @endcode
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef COMPRESS_H
#define COMPRESS_H

#include <stddef.h>
#include <stdint.h>

#include <porter_const.h>

/** 圧縮ペイロード先頭に付加する元サイズフィールドのバイト数。 */
#define POTR_COMPRESS_HEADER_SIZE 4U

/** 圧縮後の最大バッファサイズ (元データ + ヘッダー + DEFLATE オーバーヘッド)。 */
#define POTR_COMPRESS_BUF_SIZE (POTR_COMPRESS_HEADER_SIZE + POTR_MAX_MESSAGE_SIZE + 64U)

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 *******************************************************************************
 *  @brief          データを圧縮します。
 *  @param[out]     dst      圧縮後データを格納するバッファ。
 *                           先頭 4 バイトに元サイズ (NBO) が書き込まれます。
 *  @param[in,out]  dst_len  入力: dst のバッファサイズ。
 *                           出力: 書き込んだバイト数。
 *  @param[in]      src      圧縮前データへのポインタ。
 *  @param[in]      src_len  圧縮前データのバイト数。
 *  @return         成功時は 0、失敗時は -1 を返します。
 *******************************************************************************
 */
extern int potr_compress(uint8_t       *dst,
                         size_t        *dst_len,
                         const uint8_t *src,
                         size_t         src_len);

/**
 *******************************************************************************
 *  @brief          圧縮データを解凍します。
 *  @param[out]     dst      解凍後データを格納するバッファ。
 *  @param[in,out]  dst_len  入力: dst のバッファサイズ。
 *                           出力: 書き込んだバイト数。
 *  @param[in]      src      圧縮後データへのポインタ (先頭 4 バイトは元サイズ)。
 *  @param[in]      src_len  圧縮後データのバイト数 (ヘッダーを含む)。
 *  @return         成功時は 0、失敗時は -1 を返します。
 *******************************************************************************
 */
extern int potr_decompress(uint8_t       *dst,
                           size_t        *dst_len,
                           const uint8_t *src,
                           size_t         src_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COMPRESS_H */
