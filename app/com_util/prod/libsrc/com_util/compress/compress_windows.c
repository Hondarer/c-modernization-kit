/**
 *******************************************************************************
 *  @file           compress_windows.c
 *  @brief          Windows 向け圧縮・解凍モジュール (Compression API)。
 *  @author         Tetsuo Honda
 *  @date           2026/03/05
 *  @version        1.0.0
 *
 *  @details
 *  Windows 8 以降の Compression API を MSZIP | COMPRESS_RAW (Block Mode) で
 *  使用して raw DEFLATE ストリームを生成します。\n
 *  Block Mode でブロックサイズをデータサイズ以上に設定することで、
 *  データ全体を単一の raw DEFLATE ストリームとして出力します。\n
 *  これにより Linux 実装 (zlib, windowBits = -15) と同一フォーマットとなり、
 *  クロスプラットフォーム通信に対応します。
 *
 *  @note           Cabinet.lib のリンクが必要です。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>

#if defined(PLATFORM_WINDOWS)

#include <string.h>

#include <com_util/base/windows_sdk.h>
#pragma comment(lib, "ws2_32.lib")
#include <compressapi.h>
#pragma comment(lib, "Cabinet.lib")

#include <com_util/compress/compress.h>

/* doxygen コメントはヘッダに記載 */
int com_util_compress(uint8_t       *dst,
                      size_t        *dst_len,
                      const uint8_t *src,
                      size_t         src_len)
{
    COMPRESSOR_HANDLE h;
    uint32_t          orig_len_nbo;
    SIZE_T            cmp_len;
    ULONG             block_size;
    BOOL              ok;

    if (dst == NULL || dst_len == NULL || src == NULL || src_len == 0)
    {
        return -1;
    }

    if (*dst_len < COM_UTIL_COMPRESS_HEADER_SIZE + 1U)
    {
        return -1;
    }

    /* 先頭 4 バイトに元サイズ (NBO) を書く */
    orig_len_nbo = htonl((uint32_t)src_len);
    memcpy(dst, &orig_len_nbo, COM_UTIL_COMPRESS_HEADER_SIZE);

    /* MSZIP Block Mode (COMPRESS_RAW) で raw DEFLATE を生成 */
    if (!CreateCompressor(COMPRESS_ALGORITHM_MSZIP | COMPRESS_RAW, NULL, &h))
    {
        return -1;
    }

    /* ブロックサイズをデータサイズ以上に設定して単一ブロックとして処理する */
    if (src_len > 32768U)
    {
        block_size = (ULONG)src_len;
    }
    else
    {
        block_size = 32768U;
    }
    (void)SetCompressorInformation(h,
                                   COMPRESS_INFORMATION_CLASS_BLOCK_SIZE,
                                   &block_size,
                                   sizeof(block_size));

    ok = Compress(h,
                  src,
                  (SIZE_T)src_len,
                  dst + COM_UTIL_COMPRESS_HEADER_SIZE,
                  (SIZE_T)(*dst_len - COM_UTIL_COMPRESS_HEADER_SIZE),
                  &cmp_len);
    CloseCompressor(h);

    if (!ok)
    {
        return -1;
    }

    *dst_len = COM_UTIL_COMPRESS_HEADER_SIZE + (size_t)cmp_len;
    return 0;
}

/* doxygen コメントはヘッダに記載 */
int com_util_decompress(uint8_t       *dst,
                        size_t        *dst_len,
                        const uint8_t *src,
                        size_t         src_len)
{
    DECOMPRESSOR_HANDLE h;
    uint32_t            orig_len_nbo;
    uint32_t            orig_len;
    SIZE_T              out_len;
    ULONG               block_size;
    BOOL                ok;

    if (dst == NULL || dst_len == NULL || src == NULL
        || src_len <= COM_UTIL_COMPRESS_HEADER_SIZE)
    {
        return -1;
    }

    /* 先頭 4 バイトから元サイズを取得 */
    memcpy(&orig_len_nbo, src, COM_UTIL_COMPRESS_HEADER_SIZE);
    orig_len = ntohl(orig_len_nbo);

    if (*dst_len < (size_t)orig_len)
    {
        return -1;
    }

    /* MSZIP Block Mode (COMPRESS_RAW) で解凍 */
    if (!CreateDecompressor(COMPRESS_ALGORITHM_MSZIP | COMPRESS_RAW, NULL, &h))
    {
        return -1;
    }

    /* 圧縮時に設定したブロックサイズと揃える */
    if (orig_len > 32768U)
    {
        block_size = orig_len;
    }
    else
    {
        block_size = 32768U;
    }
    (void)SetDecompressorInformation(h,
                                     COMPRESS_INFORMATION_CLASS_BLOCK_SIZE,
                                     &block_size,
                                     sizeof(block_size));

    ok = Decompress(h,
                    src + COM_UTIL_COMPRESS_HEADER_SIZE,
                    (SIZE_T)(src_len - COM_UTIL_COMPRESS_HEADER_SIZE),
                    dst,
                    (SIZE_T)*dst_len,
                    &out_len);
    CloseDecompressor(h);

    if (!ok)
    {
        return -1;
    }

    *dst_len = (size_t)out_len;
    return 0;
}

#endif /* PLATFORM_WINDOWS */
