/**
 *******************************************************************************
 *  @file           compress.h
 *  @brief          データ圧縮・解凍モジュールのヘッダー。
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

#ifdef DOXYGEN

    /**
     *  @def            COMPRESS_EXPORT
     *  @brief          DLL エクスポート/インポート制御マクロ。
     */
    #define COMPRESS_EXPORT

    /**
     *  @def            COMPRESS_API
     *  @brief          呼び出し規約マクロ。
     */
    #define COMPRESS_API

#else /* !DOXYGEN */

    #ifndef COMPRESS_STATIC
        #define COMPRESS_STATIC 0
    #endif /* COMPRESS_STATIC */
    #ifndef COMPRESS_EXPORTS
        #define COMPRESS_EXPORTS 0
    #endif /* COMPRESS_EXPORTS */
    #include <com_util/base/dll_exports.h>
    #define COMPRESS_EXPORT COM_UTIL_DLL_EXPORT(COMPRESS)
    #define COMPRESS_API    COM_UTIL_DLL_API(COMPRESS)

#endif /* DOXYGEN */

/** 圧縮ペイロード先頭に付加する元サイズフィールドのバイト数。 */
#define COM_UTIL_COMPRESS_HEADER_SIZE 4U

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
COMPRESS_EXPORT int COMPRESS_API com_util_compress(uint8_t       *dst,
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
COMPRESS_EXPORT int COMPRESS_API com_util_decompress(uint8_t       *dst,
                                                     size_t        *dst_len,
                                                     const uint8_t *src,
                                                     size_t         src_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COMPRESS_H */
