/**
 *******************************************************************************
 *  @file           arena.c
 *  @brief          実行時に組み立てる記述子の記憶域をまとめて確保、解放します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/catalog/arena.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** 塊 1 個の既定バイト数です。これを超える要求には専用の塊を割り当てます。 */
#define ARENA_CHUNK_BYTES 4096U

/**
 *  @brief          アリーナが連ねる記憶域の塊です。
 */
typedef struct arena_chunk
{
    struct arena_chunk *next; /**< 次の塊です。 */
    size_t capacity;          /**< @c bytes のバイト数です。 */
    size_t used;              /**< 使用済みのバイト数です。 */
    unsigned char *bytes;     /**< 記憶域の先頭です。 */
} arena_chunk;

struct struct_meta_internal_arena
{
    arena_chunk *chunks; /**< 塊の連結リストです。先頭が最も新しい塊です。 */
};

/* Doxygen コメントは、ヘッダーに記載 */

struct_meta_internal_arena *struct_meta_internal_arena_create(void)
{
    return (struct_meta_internal_arena *)calloc(1, sizeof(struct_meta_internal_arena));
}

/**
 *  @brief          指定した容量の塊を確保し、アリーナの先頭へつなぎます。
 *  @param[in,out]  arena     確保元。
 *  @param[in]      capacity  塊のバイト数。
 *  @return         つないだ塊です。確保できない場合は NULL を返します。
 */
static arena_chunk *add_chunk(struct_meta_internal_arena *arena, size_t capacity)
{
    arena_chunk *chunk = (arena_chunk *)calloc(1, sizeof(*chunk));
    if (chunk == NULL)
    {
        return NULL;
    }
    chunk->bytes = (unsigned char *)calloc(1, capacity);
    if (chunk->bytes == NULL)
    {
        free(chunk);
        return NULL;
    }
    chunk->capacity = capacity;
    chunk->used = 0U;
    chunk->next = arena->chunks;
    arena->chunks = chunk;
    return chunk;
}

/**
 *  @brief          塊の中で、アラインメントを満たす次の位置を求めます。
 *  @param[in]      chunk      対象の塊。
 *  @param[in]      alignment  必要なアラインメント。
 *  @return         塊の先頭からの位置を返します。
 */
static size_t aligned_offset(const arena_chunk *chunk, size_t alignment)
{
    const uintptr_t base = (uintptr_t)chunk->bytes + (uintptr_t)chunk->used;
    const uintptr_t misaligned = base % (uintptr_t)alignment;
    size_t padding;
    if (misaligned == 0U)
    {
        padding = 0U;
    }
    else
    {
        padding = (size_t)((uintptr_t)alignment - misaligned);
    }
    return chunk->used + padding;
}

void *struct_meta_internal_arena_allocate(struct_meta_internal_arena *arena, size_t size, size_t alignment)
{
    if ((arena == NULL) || (size == 0U) || (alignment == 0U) || ((alignment & (alignment - 1U)) != 0U))
    {
        return NULL;
    }

    if (arena->chunks != NULL)
    {
        const size_t offset = aligned_offset(arena->chunks, alignment);
        if ((offset <= arena->chunks->capacity) && (size <= (arena->chunks->capacity - offset)))
        {
            unsigned char *result = arena->chunks->bytes + offset;
            arena->chunks->used = offset + size;
            return result;
        }
    }

    /* 既定の塊に収まらない要求には、その要求だけを入れる塊を割り当てる。 */
    size_t capacity = ARENA_CHUNK_BYTES;
    if (size > (SIZE_MAX - alignment))
    {
        return NULL;
    }
    if ((size + alignment) > capacity)
    {
        capacity = size + alignment;
    }
    arena_chunk *chunk = add_chunk(arena, capacity);
    if (chunk == NULL)
    {
        return NULL;
    }

    const size_t offset = aligned_offset(chunk, alignment);
    if ((offset > chunk->capacity) || (size > (chunk->capacity - offset)))
    {
        return NULL;
    }
    unsigned char *result = chunk->bytes + offset;
    chunk->used = offset + size;
    return result;
}

char *struct_meta_internal_arena_copy_string(struct_meta_internal_arena *arena, const char *text)
{
    if (text == NULL)
    {
        return NULL;
    }
    const size_t length = strlen(text) + 1U;
    char *copy = (char *)struct_meta_internal_arena_allocate(arena, length, 1U);
    if (copy == NULL)
    {
        return NULL;
    }
    memcpy(copy, text, length);
    return copy;
}

void struct_meta_internal_arena_destroy(struct_meta_internal_arena *arena)
{
    if (arena == NULL)
    {
        return;
    }
    arena_chunk *current = arena->chunks;
    while (current != NULL)
    {
        arena_chunk *next = current->next;
        free(current->bytes);
        free(current);
        current = next;
    }
    free(arena);
}
