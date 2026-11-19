/**
 *******************************************************************************
 *  @file           console_output.c
 *  @brief          printf と同じ書式でコンソールに出力する関数を提供します。
 *  @author         c-modernization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <base/base_spec.h>
#include <stdarg.h>
#include <stdio.h>

/* Doxygen コメントは、ヘッダーに記載 */

void base_console_output(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    (void)vprintf(format, args); /* 出力の失敗を通知しない契約 (base_spec.h) のため、戻り値は明示的に破棄する */
    va_end(args);
}
