/**
 *******************************************************************************
 *  @file           layout.c
 *  @brief          対応する型の表と、x86_64 の構造体レイアウト計算を実装します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/layout/layout.h>

#include <cplat/base/result.h>

#include <stdint.h>
#include <string.h>

/**
 *  対応する型の表です。
 *
 *  幅が LP64 と LLP64 で一致する型だけを載せます。`long` と `unsigned long` は
 *  幅が異なるため載せず、構文解析側が診断付きで拒否します。
 *
 *  x86_64 では、ここに載せたすべての型でアラインメントが大きさと一致します。
 *  System V AMD64 ABI と Windows x64 でこの点は同じです。`double` を 4 バイト境界へ
 *  置く 32 ビット x86 Linux は契約の対象外です。
 *  see: app/struct-meta/docs/architecture.md
 */
static const struct_meta_internal_layout_type g_layout_types[] = {
    {"char", "char", STRUCT_META_FIELD_SIGNED_INTEGER, 0, 1, 1},
    {"signed char", "signed char", STRUCT_META_FIELD_SIGNED_INTEGER, 0, 1, 1},
    {"unsigned char", "unsigned char", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 1, 1},
    {"short", "short", STRUCT_META_FIELD_SIGNED_INTEGER, 0, 2, 2},
    {"unsigned short", "unsigned short", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 2, 2},
    {"int", "int", STRUCT_META_FIELD_SIGNED_INTEGER, 0, 4, 4},
    {"unsigned", "unsigned int", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 4, 4},
    {"long long", "long long", STRUCT_META_FIELD_SIGNED_INTEGER, 0, 8, 8},
    {"unsigned long long", "unsigned long long", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 8, 8},
    {"int8_t", "int8_t", STRUCT_META_FIELD_SIGNED_INTEGER, 0, 1, 1},
    {"int16_t", "int16_t", STRUCT_META_FIELD_SIGNED_INTEGER, 0, 2, 2},
    {"int32_t", "int32_t", STRUCT_META_FIELD_SIGNED_INTEGER, 0, 4, 4},
    {"int64_t", "int64_t", STRUCT_META_FIELD_SIGNED_INTEGER, 0, 8, 8},
    {"uint8_t", "uint8_t", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 1, 1},
    {"uint16_t", "uint16_t", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 2, 2},
    {"uint32_t", "uint32_t", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 4, 4},
    {"uint64_t", "uint64_t", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 8, 8},
    {"float", "float", STRUCT_META_FIELD_FLOAT, 0, 4, 4},
    {"double", "double", STRUCT_META_FIELD_DOUBLE, 0, 8, 8},
};

/* Doxygen コメントは、ヘッダーに記載 */

const struct_meta_internal_layout_type *struct_meta_internal_layout_find_type(const char *spelling)
{
    if (spelling == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < (sizeof(g_layout_types) / sizeof(g_layout_types[0])); i++)
    {
        if (strcmp(g_layout_types[i].spelling, spelling) == 0)
        {
            return &g_layout_types[i];
        }
    }
    return NULL;
}

/**
 *  @brief          2 の冪かどうかを返します。
 *  @param[in]      value  判定する値。
 *  @return         2 の冪なら 0 以外、そうでなければ 0 を返します。
 *
 *  0 は 2 の冪ではないため 0 を返します。
 */
static int is_power_of_two(size_t value)
{
    if ((value != 0U) && ((value & (value - 1U)) == 0U))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void struct_meta_internal_layout_begin(struct_meta_internal_layout_builder *builder)
{
    if (builder == NULL)
    {
        return;
    }
    builder->offset = 0U;
    builder->alignment = 1U;
}

int struct_meta_internal_layout_add(struct_meta_internal_layout_builder *builder, size_t element_size,
                                    size_t element_count, size_t alignment, size_t *offset_out)
{
    if ((builder == NULL) || (offset_out == NULL) || (element_size == 0U) || (element_count == 0U) ||
        (is_power_of_two(alignment) == 0))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    /* オフセットをアラインメントへ切り上げる。加算で桁あふれしないことを先に確かめる。 */
    size_t padding;
    if (builder->offset % alignment == 0U)
    {
        padding = 0U;
    }
    else
    {
        padding = (alignment - (builder->offset % alignment));
    }
    if (padding > (SIZE_MAX - builder->offset))
    {
        return CPLAT_ERR_OUT_OF_RANGE;
    }
    const size_t offset = builder->offset + padding;

    /* 要素数を掛けた総バイト数を求める。 */
    if (element_count > (SIZE_MAX / element_size))
    {
        return CPLAT_ERR_OUT_OF_RANGE;
    }
    const size_t total = element_size * element_count;
    if (total > (SIZE_MAX - offset))
    {
        return CPLAT_ERR_OUT_OF_RANGE;
    }

    builder->offset = offset + total;
    if (alignment > builder->alignment)
    {
        builder->alignment = alignment;
    }
    *offset_out = offset;
    return CPLAT_OK;
}

int struct_meta_internal_layout_end(const struct_meta_internal_layout_builder *builder, size_t *size_out,
                                    size_t *alignment_out)
{
    if ((builder == NULL) || (size_out == NULL) || (alignment_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    /* C ではメンバーの無い構造体を宣言できないため、1 個も追加していない状態を拒否する。 */
    if (builder->offset == 0U)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if (is_power_of_two(builder->alignment) == 0)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    const size_t remainder = builder->offset % builder->alignment;
    size_t padding;
    if (remainder == 0U)
    {
        padding = 0U;
    }
    else
    {
        padding = (builder->alignment - remainder);
    }
    if (padding > (SIZE_MAX - builder->offset))
    {
        return CPLAT_ERR_OUT_OF_RANGE;
    }

    *size_out = builder->offset + padding;
    *alignment_out = builder->alignment;
    return CPLAT_OK;
}
