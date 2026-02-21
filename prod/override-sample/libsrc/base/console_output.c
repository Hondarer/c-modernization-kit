/**
 *******************************************************************************
 *  @file           console_output.c
 *  @brief          console_output 関数の実装ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  printf と同じ書式でコンソールに出力する関数を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <libbase.h>
#include <stdarg.h>
#include <stdio.h>

/* doxygen コメントは、ヘッダに記載 */
void WINAPI console_output(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
