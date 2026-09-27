/**
 *******************************************************************************
 *  @file           sample_filter_share_region.c
 *  @brief          配布に使う受け渡し用のメモリ領域と、その排他を実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/27
 *  @version        0.1.0
 *
 *  PoC の実装です。共有メモリは calloc で確保したプロセス内の領域で、排他はプロセス内のミューテックスで模擬します。
 *  同じパスで確保した領域は、参照カウントで 1 つの実体を共有します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#include "sample_filter_share_region.h"

#include <cplat/base/result.h>
#include <cplat/sync/sync.h>

#include <stdlib.h>
#include <string.h>

struct sample_filter_share_region
{
    sample_filter_share_region *next; /**< 確保済み領域の一覧の次の要素。 */
    char *path;                       /**< 領域を識別するパス。 */
    void *address;                    /**< 共有メモリを模擬する領域。calloc で 0 に初期化する。 */
    size_t size;                      /**< 領域のバイト数。 */
    size_t reference_count;           /**< 領域を確保している利用者の数。 */
};

struct sample_filter_share_lock
{
    cplat_local_lock *lock; /**< プロセスをまたぐ排他を模擬するミューテックス。 */
};

/** 確保済みの領域の一覧です。同じパスの確保で同じ実体を返すために使います。 */
static sample_filter_share_region *s_regions = NULL;

/* ===== 受け渡し用のメモリ領域 ===== */

/** パスが一致する確保済みの領域を探します。見つからない場合は NULL を返します。 */
static sample_filter_share_region *find_region(const char *path)
{
    sample_filter_share_region *region;

    for (region = s_regions; region != NULL; region = region->next)
    {
        if (strcmp(region->path, path) == 0)
        {
            return region;
        }
    }
    return NULL;
}

/** 領域を解放します。一覧からは外しません。 */
static void free_region(sample_filter_share_region *region)
{
    free(region->address);
    free(region->path);
    free(region);
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_share_region_open(const char *path, const size_t size, sample_filter_share_region **region_out)
{
    sample_filter_share_region *region;
    size_t path_size;

    if ((path == NULL) || (size == 0U) || (region_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *region_out = NULL;

    region = find_region(path);
    if (region != NULL)
    {
        /* 別の大きさで確保済みの領域は使わない */
        if (region->size < size)
        {
            return CPLAT_ERR_CORRUPT_DESCRIPTOR;
        }
        region->reference_count++;
        *region_out = region;
        return CPLAT_OK;
    }

    region = (sample_filter_share_region *)calloc(1U, sizeof(*region));
    if (region == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    path_size = strlen(path) + 1U;
    region->path = (char *)malloc(path_size);
    /* calloc の戻り値は、あらゆる基本型に揃っているため 8 バイト境界の契約を満たす */
    region->address = calloc(1U, size);
    if ((region->path == NULL) || (region->address == NULL))
    {
        free_region(region);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    memcpy(region->path, path, path_size);
    region->size = size;
    region->reference_count = 1U;

    region->next = s_regions;
    s_regions = region;
    *region_out = region;
    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

void sample_filter_share_region_close(sample_filter_share_region **region)
{
    sample_filter_share_region **link;

    if ((region == NULL) || (*region == NULL))
    {
        return;
    }

    (*region)->reference_count--;
    if ((*region)->reference_count == 0U)
    {
        for (link = &s_regions; *link != NULL; link = &(*link)->next)
        {
            if (*link == *region)
            {
                *link = (*region)->next;
                break;
            }
        }
        free_region(*region);
    }
    *region = NULL;
}

/* Doxygen コメントは、ヘッダーに記載 */

void *sample_filter_share_region_get_address(const sample_filter_share_region *region)
{
    if (region == NULL)
    {
        return NULL;
    }

    return region->address;
}

/* ===== 受け渡しの排他 ===== */

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_share_lock_create(const char *path, sample_filter_share_lock **lock_out)
{
    sample_filter_share_lock *lock;
    int ret;

    if ((path == NULL) || (lock_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *lock_out = NULL;

    lock = (sample_filter_share_lock *)calloc(1U, sizeof(*lock));
    if (lock == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    /* PoC ではプロセス内のミューテックスで模擬するため、path は使わない */
    ret = cplat_local_lock_create(&lock->lock);
    if (ret != CPLAT_OK)
    {
        free(lock);
        return ret;
    }

    *lock_out = lock;
    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

void sample_filter_share_lock_dispose(sample_filter_share_lock **lock)
{
    if ((lock == NULL) || (*lock == NULL))
    {
        return;
    }

    cplat_local_lock_dispose((*lock)->lock);
    free(*lock);
    *lock = NULL;
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_share_lock_acquire(sample_filter_share_lock *lock)
{
    if (lock == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    return cplat_local_lock_lock(lock->lock, CPLAT_SYNC_WAIT_FOREVER);
}

/* Doxygen コメントは、ヘッダーに記載 */

void sample_filter_share_lock_release(sample_filter_share_lock *lock)
{
    if (lock == NULL)
    {
        return;
    }

    (void)cplat_local_lock_unlock(lock->lock);
}
