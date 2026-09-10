/**
 *******************************************************************************
 *  @file           libsrc/message_catalog/message_catalog_argument.c
 *  @brief          引数スキーマに従って可変長引数を値の配列へ取り出します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  可変長引数は順番にしか安全に取り出せず、`va_arg` の型指定が実引数の型 (既定引数拡張後)
 *  と適合しない場合は未定義動作になります。\n
 *  そのため、引数種別ごとの取り出し型をこのファイルへ閉じ込め、
 *  ほかの実装は値の配列だけを扱います。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "format_engine.h"

#include <message_catalog/message_catalog_const.h>
#include <stdarg.h>
#include <stdint.h>

/* Doxygen コメントは、ヘッダーに記載 */

int format_engine_collect_arguments(const message_catalog_entry *entry, va_list args,
                                    format_engine_argument_value *values)
{
    int index;

    for (index = 0; index < entry->argument_count; index++)
    {
        const message_catalog_argument_kind kind = entry->arguments[index];

        values[index].kind = kind;
        values[index].pad = 0U;

        switch (kind)
        {
        case MESSAGE_CATALOG_ARGUMENT_KIND_STRING:
            values[index].value.string_value = va_arg(args, const char *);
            break;

        case MESSAGE_CATALOG_ARGUMENT_KIND_INT32:
            values[index].value.int32_value = va_arg(args, int32_t);
            break;

        case MESSAGE_CATALOG_ARGUMENT_KIND_UINT32:
        case MESSAGE_CATALOG_ARGUMENT_KIND_HEX32:
            values[index].value.uint32_value = va_arg(args, uint32_t);
            break;

        case MESSAGE_CATALOG_ARGUMENT_KIND_INT64:
            values[index].value.int64_value = va_arg(args, int64_t);
            break;

        case MESSAGE_CATALOG_ARGUMENT_KIND_UINT64:
        case MESSAGE_CATALOG_ARGUMENT_KIND_HEX64:
            values[index].value.uint64_value = va_arg(args, uint64_t);
            break;

        case MESSAGE_CATALOG_ARGUMENT_KIND_SIZE:
            values[index].value.size_value = va_arg(args, size_t);
            break;

        case MESSAGE_CATALOG_ARGUMENT_KIND_POINTER:
            values[index].value.pointer_value = va_arg(args, const void *);
            break;

        case MESSAGE_CATALOG_ARGUMENT_KIND_DOUBLE:
            values[index].value.double_value = va_arg(args, double);
            break;

        case MESSAGE_CATALOG_ARGUMENT_KIND_ERROR_CODE:
            values[index].value.error_code_value = va_arg(args, int);
            break;

        default:
            /* 未知の種別では取り出す型を決められないため、以降の引数も読み進めない */
            return MESSAGE_CATALOG_ERR_INVALID_DEFINITION;
        }
    }

    return MESSAGE_CATALOG_OK;
}
