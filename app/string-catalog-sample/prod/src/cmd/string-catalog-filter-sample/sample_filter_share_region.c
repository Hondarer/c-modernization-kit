/**
 *******************************************************************************
 *  @file           sample_filter_share_region.c
 *  @brief          配布に使う受け渡し用のメモリ領域と、その排他を実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/27
 *  @version        0.2.0
 *
 *  領域は cplat のメモリ マップ (`cplat_mmap`)、排他は cplat のプロセス間ロック (`cplat_interprocess_lock`) で実装します。\n
 *  インターフェイス (`sample_filter_share_region.h`) は cplat に依存しないため、cplat の結果コードは本モジュールの結果へ変換します。
 *
 *  プロセス間ロックの実体はファイル ロック (Linux の flock、Windows の LockFileEx) であり、
 *  同じハンドルを使う複数のスレッドの間では排他になりません。
 *  そこで、プロセス内のスレッドをプロセス内のロックで先に直列化し、その内側でプロセス間ロックを取ります。\n
 *  see: https://man7.org/linux/man-pages/man2/flock.2.html \n
 *  see: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-lockfileex
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#include "sample_filter_share_region.h"

#include <cplat/base/result.h>
#include <cplat/clock/clock.h>
#include <cplat/mmap/mmap.h>
#include <cplat/sync/sync.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** ロックのファイルの名前に付ける接尾辞です。 */
#define LOCK_FILE_SUFFIX ".lock"

struct sample_filter_share_region
{
    cplat_mmap *map; /**< 領域のファイルのメモリ マップ。 */
};

struct sample_filter_share_lock
{
    cplat_local_lock *local;          /**< 同じ排他を使うスレッドを直列化するプロセス内のロック。 */
    cplat_interprocess_lock *process; /**< プロセスの間を直列化するファイル ロック。 */
};

/** cplat の結果コードを、本モジュールの結果へ変換します。 */
static sample_filter_share_region_result result_of(const int cplat_result)
{
    switch (cplat_result)
    {
    case CPLAT_OK:
        return SAMPLE_FILTER_SHARE_REGION_OK;
    case CPLAT_ERR_INVALID_ARGUMENT:
        return SAMPLE_FILTER_SHARE_REGION_INVALID_ARGUMENT;
    case CPLAT_ERR_TIMEOUT:
    case CPLAT_ERR_BUSY:
        return SAMPLE_FILTER_SHARE_REGION_TIMEOUT;
    default:
        return SAMPLE_FILTER_SHARE_REGION_FAILED;
    }
}

/* ===== 受け渡し用のメモリ領域 ===== */

/* Doxygen コメントは、ヘッダーに記載 */

sample_filter_share_region_result sample_filter_share_region_open(const char *path, const size_t size,
                                                                  sample_filter_share_region **region_out)
{
    sample_filter_share_region *region;
    int ret;

    if ((path == NULL) || (size == 0U) || (region_out == NULL))
    {
        return SAMPLE_FILTER_SHARE_REGION_INVALID_ARGUMENT;
    }
    *region_out = NULL;

    region = (sample_filter_share_region *)calloc(1U, sizeof(*region));
    if (region == NULL)
    {
        return SAMPLE_FILTER_SHARE_REGION_FAILED;
    }

    /* ファイルがなければ size バイトで作成する。作成したファイルは 0 で埋まる */
    ret = cplat_mmap_attach(path, CPLAT_MMAP_ACCESS_READ_WRITE, size, &region->map, NULL);
    if (ret != CPLAT_OK)
    {
        free(region);
        return result_of(ret);
    }

    /* 既存のファイルは作り直さない。大きい場合は先頭の size バイトだけを使い、足りない場合は拒否する */
    if (cplat_mmap_get_size(region->map) < size)
    {
        sample_filter_share_region_close(&region);
        return SAMPLE_FILTER_SHARE_REGION_TOO_SMALL;
    }

    *region_out = region;
    return SAMPLE_FILTER_SHARE_REGION_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

void sample_filter_share_region_close(sample_filter_share_region **region)
{
    if ((region == NULL) || (*region == NULL))
    {
        return;
    }

    if ((*region)->map != NULL)
    {
        (void)cplat_mmap_detach((*region)->map, NULL);
    }
    free(*region);
    *region = NULL;
}

/* Doxygen コメントは、ヘッダーに記載 */

void *sample_filter_share_region_get_address(const sample_filter_share_region *region)
{
    if (region == NULL)
    {
        return NULL;
    }
    /* マップの先頭はページ境界のため、8 バイト境界に揃っている */
    return cplat_mmap_get_address(region->map);
}

/* ===== 受け渡しの排他 ===== */

/* Doxygen コメントは、ヘッダーに記載 */

sample_filter_share_region_result sample_filter_share_lock_create(const char *path, sample_filter_share_lock **lock_out)
{
    sample_filter_share_lock *lock;
    char *lock_path;
    size_t lock_path_size;
    int ret;

    if ((path == NULL) || (lock_out == NULL))
    {
        return SAMPLE_FILTER_SHARE_REGION_INVALID_ARGUMENT;
    }
    *lock_out = NULL;

    lock = (sample_filter_share_lock *)calloc(1U, sizeof(*lock));
    lock_path_size = strlen(path) + sizeof(LOCK_FILE_SUFFIX);
    lock_path = (char *)malloc(lock_path_size);
    if ((lock == NULL) || (lock_path == NULL))
    {
        free(lock_path);
        free(lock);
        return SAMPLE_FILTER_SHARE_REGION_FAILED;
    }
    (void)snprintf(lock_path, lock_path_size, "%s%s", path, LOCK_FILE_SUFFIX);

    ret = cplat_local_lock_create(&lock->local);
    if (ret == CPLAT_OK)
    {
        ret = cplat_interprocess_lock_open(lock_path, &lock->process);
    }
    free(lock_path);
    if (ret != CPLAT_OK)
    {
        sample_filter_share_lock_dispose(&lock);
        return result_of(ret);
    }

    *lock_out = lock;
    return SAMPLE_FILTER_SHARE_REGION_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

void sample_filter_share_lock_dispose(sample_filter_share_lock **lock)
{
    if ((lock == NULL) || (*lock == NULL))
    {
        return;
    }

    if ((*lock)->process != NULL)
    {
        cplat_interprocess_lock_dispose((*lock)->process);
    }
    if ((*lock)->local != NULL)
    {
        cplat_local_lock_dispose((*lock)->local);
    }
    free(*lock);
    *lock = NULL;
}

/* Doxygen コメントは、ヘッダーに記載 */

sample_filter_share_region_result sample_filter_share_lock_acquire(sample_filter_share_lock *lock, const int timeout_ms)
{
    uint64_t deadline_ms = 0U;
    int remaining_ms = CPLAT_SYNC_WAIT_FOREVER;
    int ret;

    if ((lock == NULL) || ((timeout_ms < 0) && (timeout_ms != SAMPLE_FILTER_SHARE_WAIT_FOREVER)))
    {
        return SAMPLE_FILTER_SHARE_REGION_INVALID_ARGUMENT;
    }
    if (timeout_ms != SAMPLE_FILTER_SHARE_WAIT_FOREVER)
    {
        remaining_ms = timeout_ms;
        deadline_ms = cplat_clock_get_monotonic_ms() + (uint64_t)timeout_ms;
    }

    /* 同じハンドルを使うスレッドの間はファイル ロックが排他にならないため、プロセス内のロックを先に取る */
    ret = cplat_local_lock_lock(lock->local, remaining_ms);
    if (ret != CPLAT_OK)
    {
        return result_of(ret);
    }

    /* プロセス間ロックは、プロセス内のロックで使った時間を差し引いた残りだけ待つ */
    if (timeout_ms != SAMPLE_FILTER_SHARE_WAIT_FOREVER)
    {
        const uint64_t now_ms = cplat_clock_get_monotonic_ms();

        remaining_ms = (now_ms >= deadline_ms) ? 0 : (int)(deadline_ms - now_ms);
    }
    ret = cplat_interprocess_lock_lock(lock->process, remaining_ms);
    if (ret != CPLAT_OK)
    {
        (void)cplat_local_lock_unlock(lock->local);
        return result_of(ret);
    }
    return SAMPLE_FILTER_SHARE_REGION_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

void sample_filter_share_lock_release(sample_filter_share_lock *lock)
{
    if (lock == NULL)
    {
        return;
    }
    (void)cplat_interprocess_lock_unlock(lock->process);
    (void)cplat_local_lock_unlock(lock->local);
}
