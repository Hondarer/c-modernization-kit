/**
 *******************************************************************************
 *  @file           sample_filter_share.c
 *  @brief          コンパイル済みの条件を共有メモリで配布する仕組みを実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/27
 *  @version        0.1.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#include "sample_filter_share.h"

#include <cplat/base/platform.h>
#include <cplat/base/result.h>
#include <cplat/clock/clock.h>
#include <cplat/mmap/mmap.h>
#include <cplat/runtime/process.h>
#include <cplat/sync/atomic.h>
#include <cplat/sync/sync.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(sizeof(sample_filter_share_header) == SAMPLE_FILTER_SHARE_HEADER_SIZE, "share header size");

struct sample_filter_share
{
    cplat_mmap *map;                    /**< 共有メモリ。 */
    sample_filter_share_header *header; /**< 共有メモリの先頭の配布ヘッダー。 */
    unsigned char *image_area;          /**< 共有メモリ上のフィルター オブジェクト。 */
    unsigned char *copy;                /**< 取り込みで複製する手元の領域。 */
    cplat_local_lock *lock;             /**< 書き込みと取り込みを排他するミューテックス。呼び出し側が所有する。 */
    size_t image_size;                  /**< フィルター オブジェクトのバイト数。 */
    cplat_atomic_u64 taken_generation;  /**< 取り込み済みの世代番号。ロックなしでアトミックに読まれる。 */
    size_t last_take_invalid_count;     /**< 直近の取り込みで無効にした行の数。lock の下で読み書きする。 */
    uint32_t line_capacity;             /**< 行数の上限。 */
    uint32_t line_width;                /**< 行幅。 */
    int last_take_result;               /**< 直近の取り込みの適用結果。lock の下で読み書きする。 */
    unsigned int pad;                   /**< 明示的アラインメントです。 */
};

/* ===== ロック ===== */

/**
 *  @brief          書き込みと取り込みを排他するミューテックスを取ります。
 *
 *  PoC では、呼び出し側が用意した単純なミューテックスでプロセスをまたぐ排他を模擬します。
 *  実際に複数のプロセスで動かす段階では、プロセス間で共有できる排他へ置き換える箇所です。
 */
static int lock_share(sample_filter_share *share)
{
    return cplat_local_lock_lock(share->lock, CPLAT_SYNC_WAIT_FOREVER);
}

/** ミューテックスを解放します。 */
static void unlock_share(sample_filter_share *share)
{
    (void)cplat_local_lock_unlock(share->lock);
}

/**
 *  @brief          配布ヘッダーがこのハンドルと同じ形式のフィルター オブジェクトを持つかを確かめます。
 *
 *  ミューテックスの下で呼び出します。
 */
static bool is_header_compatible(const sample_filter_share *share)
{
    const sample_filter_share_header *header = share->header;

    return (header->signature == SAMPLE_FILTER_SHARE_SIGNATURE) &&
           (header->format_version == SAMPLE_FILTER_SHARE_FORMAT_VERSION) &&
           (header->header_size == SAMPLE_FILTER_SHARE_HEADER_SIZE) &&
           (header->line_capacity == share->line_capacity) && (header->line_width == share->line_width) &&
           (header->image_size == (uint64_t)share->image_size);
}

/* ===== 公開関数 ===== */

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_share_open(const char *path, cplat_local_lock *lock, const size_t line_capacity,
                             const size_t line_width, sample_filter_share **share_out)
{
    sample_filter_share *share;
    size_t share_size;
    int ret;

    if ((path == NULL) || (lock == NULL) || (share_out == NULL) || (line_capacity == 0U) ||
        (line_capacity > SAMPLE_FILTER_LINE_MAX) || (line_width < SAMPLE_FILTER_LINE_WIDTH_MIN) ||
        (line_width > SAMPLE_FILTER_LINE_WIDTH_MAX))
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
    share->line_capacity = (uint32_t)line_capacity;
    share->line_width = (uint32_t)line_width;
    share->image_size = SAMPLE_FILTER_IMAGE_SIZE(line_capacity, line_width);
    share->last_take_result = CPLAT_OK;
    share_size = SAMPLE_FILTER_SHARE_SIZE(line_capacity, line_width);

    share->copy = (unsigned char *)malloc(share->image_size);
    if (share->copy == NULL)
    {
        sample_filter_share_close(&share);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    /* 新しく作成したファイルは 0 で埋まる。配布ヘッダーは最初の公開で初期化する */
    ret = cplat_mmap_attach(path, CPLAT_MMAP_ACCESS_READ_WRITE, share_size, &share->map, NULL);
    if (ret != CPLAT_OK)
    {
        sample_filter_share_close(&share);
        return ret;
    }

    /* 別の大きさで作成済みのファイルは使わない */
    if (cplat_mmap_get_size(share->map) < share_size)
    {
        sample_filter_share_close(&share);
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }

    /* マップの先頭はページ境界のため、配布ヘッダーの世代番号は 8 バイト境界に置かれる */
    share->header = (sample_filter_share_header *)cplat_mmap_get_address(share->map);
    share->image_area = (unsigned char *)share->header + SAMPLE_FILTER_SHARE_HEADER_SIZE;

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

    if ((*share)->map != NULL)
    {
        (void)cplat_mmap_detach((*share)->map, NULL);
    }
    /* ミューテックスは呼び出し側が所有するため、破棄しない */
    free((*share)->copy);
    free(*share);
    *share = NULL;
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_share_publish(sample_filter_share *share, const void *image, const size_t image_size,
                                uint64_t *generation_out)
{
    sample_filter_share_header *header;
    sample_filter_info info;
    cplat_timespec now;
    uint64_t generation;
    int ret;

    if ((share == NULL) || (image == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    /* 共有メモリに触れる前に検証し、壊れた内容を配布しない */
    if ((sample_filter_get_info(image, image_size, &info) != CPLAT_OK) ||
        (info.line_capacity != share->line_capacity) || (info.line_width != share->line_width))
    {
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }

    ret = lock_share(share);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    header = share->header;
    if ((header->signature == 0U) && (cplat_atomic_load_u64(&header->generation, CPLAT_MEMORY_ORDER_RELAXED) ==
                                      SAMPLE_FILTER_SHARE_GENERATION_NONE))
    {
        /* 未初期化の共有メモリ。最初の書き込み側が配布ヘッダーを初期化する */
        header->format_version = SAMPLE_FILTER_SHARE_FORMAT_VERSION;
        header->header_size = SAMPLE_FILTER_SHARE_HEADER_SIZE;
        header->line_capacity = share->line_capacity;
        header->line_width = share->line_width;
        header->image_size = (uint64_t)share->image_size;
        memset(header->reserved, 0, sizeof(header->reserved));
        header->signature = SAMPLE_FILTER_SHARE_SIGNATURE;
    }
    else if (!is_header_compatible(share))
    {
        unlock_share(share);
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }

    memcpy(share->image_area, image, share->image_size);
    cplat_get_realtime(&now);
    header->published_seconds = (int64_t)now.tv_sec;
    header->published_nanoseconds = now.tv_nsec;
    header->publisher_process_id = cplat_process_get_pid();

    /* 内容を書き終えてから世代を進める。緩いチェックで新しい世代を見た読み取り側は、ロックを取ってから複製する。
       世代は一致しないことだけで変化と判定されるため、一巡しても検知できる。0 は未公開のため飛ばす。
       内容の受け渡しはミューテックスが順序付けるため、世代の読み書き自体に強い順序は要らない。
       ただし緩いチェックで新しい世代を見た時点で内容も書き終えていると言えるよう、書き込みはリリース順序とする */
    generation = cplat_atomic_load_u64(&header->generation, CPLAT_MEMORY_ORDER_RELAXED) + 1U;
    if (generation == SAMPLE_FILTER_SHARE_GENERATION_NONE)
    {
        generation = 1U;
    }
    cplat_atomic_store_u64(&header->generation, generation, CPLAT_MEMORY_ORDER_RELEASE);

    unlock_share(share);

    if (generation_out != NULL)
    {
        *generation_out = generation;
    }
    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_share_refresh(sample_filter_share *share, sample_filter_slot *slot, int *is_taken_out)
{
    uint64_t published;
    size_t invalid_count = 0U;
    int ret;

    if ((share == NULL) || (slot == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if (is_taken_out != NULL)
    {
        *is_taken_out = 0;
    }

    /* 緩いチェック。ロックを取らずに世代を読み、取り込み済みと一致すれば何もしない。
       不一致なら必ずロックを取ってから内容を読むため、ここは順序の保証がない軽量な読み取りで足りる */
    published = cplat_atomic_load_u64(&share->header->generation, CPLAT_MEMORY_ORDER_RELAXED);
    if (published == cplat_atomic_load_u64(&share->taken_generation, CPLAT_MEMORY_ORDER_RELAXED))
    {
        return CPLAT_OK;
    }

    ret = lock_share(share);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    /* 最終チェック。ほかのスレッドが先に取り込んでいれば何もしない */
    published = cplat_atomic_load_u64(&share->header->generation, CPLAT_MEMORY_ORDER_RELAXED);
    if (published == cplat_atomic_load_u64(&share->taken_generation, CPLAT_MEMORY_ORDER_RELAXED))
    {
        unlock_share(share);
        return CPLAT_OK;
    }

    if (published == SAMPLE_FILTER_SHARE_GENERATION_NONE)
    {
        /* 共有メモリが作り直された。取り込む内容がないため、取り込み済みの世代だけを合わせる */
        cplat_atomic_store_u64(&share->taken_generation, published, CPLAT_MEMORY_ORDER_RELEASE);
        unlock_share(share);
        return CPLAT_OK;
    }

    if (!is_header_compatible(share))
    {
        ret = CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }
    else
    {
        memcpy(share->copy, share->image_area, share->image_size);
    }

    /* 適用は手元の複製に対して行う。取り込みを直列に行うため、適用を終えるまでミューテックスを保持する */
    if (ret == CPLAT_OK)
    {
        ret = sample_filter_slot_apply(slot, share->copy, share->image_size, NULL, 0U, &invalid_count);
    }

    /* 適用に失敗しても世代を記録し、同じ世代の取り込みを繰り返さない */
    share->last_take_result = ret;
    share->last_take_invalid_count = invalid_count;
    cplat_atomic_store_u64(&share->taken_generation, published, CPLAT_MEMORY_ORDER_RELEASE);

    unlock_share(share);

    if (is_taken_out != NULL)
    {
        *is_taken_out = 1;
    }
    return ret;
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_share_get_status(sample_filter_share *share, sample_filter_share_status *status_out)
{
    const sample_filter_share_header *header;
    int ret;

    if ((share == NULL) || (status_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    ret = lock_share(share);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    header = share->header;
    memset(status_out, 0, sizeof(*status_out));
    status_out->published_generation = cplat_atomic_load_u64(&share->header->generation, CPLAT_MEMORY_ORDER_RELAXED);
    status_out->taken_generation = cplat_atomic_load_u64(&share->taken_generation, CPLAT_MEMORY_ORDER_RELAXED);
    if (status_out->published_generation != SAMPLE_FILTER_SHARE_GENERATION_NONE)
    {
        status_out->published_seconds = header->published_seconds;
        status_out->published_nanoseconds = header->published_nanoseconds;
        status_out->publisher_process_id = header->publisher_process_id;
    }
    status_out->last_take_result = share->last_take_result;
    status_out->last_take_invalid_count = share->last_take_invalid_count;

    unlock_share(share);
    return CPLAT_OK;
}
