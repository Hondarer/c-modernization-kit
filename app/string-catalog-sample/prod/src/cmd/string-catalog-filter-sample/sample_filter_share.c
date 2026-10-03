/**
 *******************************************************************************
 *  @file           sample_filter_share.c
 *  @brief          コンパイル済みの条件を共有メモリで配布する仕組みを実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/27
 *  @version        0.2.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#include "sample_filter_share.h"

#include <cplat/base/result.h>

#include <stdlib.h>

/** 排他と、cplat から取得するときの待ち時間の組です。cplat へ渡す関数の引数にします。 */
typedef struct source_lock_context
{
    sample_filter_share_lock *lock; /**< 排他。 */
    int timeout_ms;                 /**< 待ち時間 (ミリ秒)。 */
    int pad;                        /**< 明示的アラインメントです。 */
} source_lock_context;

/** 共有メモリによる配布のハンドルです。 */
struct sample_filter_share
{
    sample_filter_share_region *region; /**< 受け渡し用のメモリ領域。 */
    sample_filter_share_lock *lock;     /**< 共有メモリの排他。呼び出し側が所有する。 */
    void *source;                       /**< 共有メモリの先頭。cplat のソース領域として使う。 */
    size_t source_size;                 /**< ソース領域のバイト数。 */
    source_lock_context writer_lock;    /**< 公開で使う排他。取得できるまで待つ。 */
    source_lock_context reader_lock;    /**< 取り込みで使う排他。待ち時間に上限を設ける。 */
    uint32_t line_capacity;             /**< 行数の上限。 */
    uint32_t line_width;                /**< 行幅。 */
};

/** cplat が公開と取り込みの間に、排他を取得するために呼び出します。 */
static int acquire_source_lock(void *context)
{
    const source_lock_context *lock_context = (const source_lock_context *)context;

    return sample_filter_share_to_result(sample_filter_share_lock_acquire(lock_context->lock, lock_context->timeout_ms));
}

/** cplat が公開と取り込みの間に、排他を解放するために呼び出します。 */
static void release_source_lock(void *context)
{
    sample_filter_share_lock_release(((const source_lock_context *)context)->lock);
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_share_to_result(const sample_filter_share_region_result result)
{
    switch (result)
    {
    case SAMPLE_FILTER_SHARE_REGION_OK:
        return CPLAT_OK;
    case SAMPLE_FILTER_SHARE_REGION_INVALID_ARGUMENT:
        return CPLAT_ERR_INVALID_ARGUMENT;
    case SAMPLE_FILTER_SHARE_REGION_TOO_SMALL:
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    case SAMPLE_FILTER_SHARE_REGION_TIMEOUT:
        return CPLAT_ERR_TIMEOUT;
    case SAMPLE_FILTER_SHARE_REGION_FAILED:
    default:
        return CPLAT_ERR_UNKNOWN;
    }
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_share_open(const char *path, sample_filter_share_lock *lock, const size_t line_capacity,
                             const size_t line_width, sample_filter_share **share_out)
{
    sample_filter_share *share;
    int ret;

    if ((path == NULL) || (lock == NULL) || (share_out == NULL) || (line_capacity == 0U) ||
        (line_capacity > CPLAT_STRING_CATALOG_FILTER_LINE_MAX) ||
        (line_width < CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MIN) ||
        (line_width > CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MAX))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *share_out = NULL;

    share = (sample_filter_share *)calloc(1U, sizeof(*share));
    if (share == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    share->lock = lock;
    share->writer_lock.lock = lock;
    share->writer_lock.timeout_ms = SAMPLE_FILTER_SHARE_WAIT_FOREVER;
    share->reader_lock.lock = lock;
    share->reader_lock.timeout_ms = SAMPLE_FILTER_SHARE_READER_TIMEOUT_MS;
    share->line_capacity = (uint32_t)line_capacity;
    share->line_width = (uint32_t)line_width;
    share->source_size = CPLAT_STRING_CATALOG_FILTER_SOURCE_SIZE(line_capacity, line_width);

    /* 複数のプロセスが同時に作成しないよう、排他の下で開く。新しく作成した領域は 0 で埋まり、
       cplat は未公開のソース領域として扱う */
    ret = sample_filter_share_to_result(sample_filter_share_lock_acquire(lock, SAMPLE_FILTER_SHARE_WAIT_FOREVER));
    if (ret == CPLAT_OK)
    {
        ret = sample_filter_share_to_result(sample_filter_share_region_open(path, share->source_size, &share->region));
        sample_filter_share_lock_release(lock);
    }
    if (ret != CPLAT_OK)
    {
        sample_filter_share_close(&share);
        return ret;
    }

    /* 領域の先頭は 8 バイト境界のため、ソース領域の公開時刻をアトミックに読み書きできる */
    share->source = sample_filter_share_region_get_address(share->region);

    *share_out = share;
    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

void sample_filter_share_close(sample_filter_share **share)
{
    if ((share == NULL) || (*share == NULL))
    {
        return;
    }

    sample_filter_share_region_close(&(*share)->region);
    /* 排他は呼び出し側が所有するため、破棄しない */
    free(*share);
    *share = NULL;
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_share_publish(sample_filter_share *share, const void *image, const size_t image_size,
                                uint64_t *timestamp_out)
{
    cplat_string_catalog_filter_source_lock lock;
    cplat_string_catalog_filter_info info;
    int ret;

    if ((share == NULL) || (image == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    /* 読み取り側のスロットが受け付ける形だけを公開する。異なる形を公開すると、どの読み取り側も取り込めないため */
    ret = cplat_string_catalog_filter_get_info(image, image_size, &info);
    if ((ret != CPLAT_OK) || (info.line_capacity != share->line_capacity) || (info.line_width != share->line_width))
    {
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }

    /* 排他は cplat が書き込みの間だけ取る。書き込み側どうしと、排他を結び付けた読み取り側の複製を直列化する */
    lock.lock = acquire_source_lock;
    lock.unlock = release_source_lock;
    lock.context = &share->writer_lock;
    return cplat_string_catalog_filter_source_publish(share->source, share->source_size, image, image_size, &lock,
                                                      timestamp_out);
}

/* Doxygen コメントは、ヘッダーに記載 */

const void *sample_filter_share_get_source(const sample_filter_share *share, size_t *size_out)
{
    if ((share == NULL) || (size_out == NULL))
    {
        return NULL;
    }
    *size_out = share->source_size;
    return share->source;
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_share_get_source_lock(sample_filter_share *share,
                                        cplat_string_catalog_filter_source_lock *lock_out)
{
    if ((share == NULL) || (lock_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    lock_out->lock = acquire_source_lock;
    lock_out->unlock = release_source_lock;
    lock_out->context = &share->reader_lock;
    return CPLAT_OK;
}
