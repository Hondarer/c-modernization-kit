/**
 *******************************************************************************
 *  @file           console_output.c
 *  @brief          printf と同じ書式でコンソールに出力する関数を提供します。
 *  @author         c-modenization-kit sample team
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

void console_output(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
