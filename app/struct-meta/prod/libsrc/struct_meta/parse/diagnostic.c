/**
 *******************************************************************************
 *  @file           diagnostic.c
 *  @brief          解析と記述子構築が使う診断の書き込みを実装します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/parse/diagnostic.h>

#include <stdarg.h>
#include <stdio.h>

/* Doxygen コメントは、ヘッダーに記載 */

void struct_meta_internal_diagnostic_clear(struct_meta_diagnostic *diagnostic)
{
    if (diagnostic == NULL)
    {
        return;
    }

    diagnostic->line = 0;
    diagnostic->message[0] = '\0';
}

void struct_meta_internal_diagnose(struct_meta_diagnostic *diagnostic, int line, const char *format, ...)
{
    if ((diagnostic == NULL) || (format == NULL))
    {
        return;
    }

    /* 最初の 1 件だけを残す。後続は最初の診断の波及であることが多い。 */
    if (diagnostic->message[0] != '\0')
    {
        return;
    }

    va_list args;
    va_start(args, format);
    (void)vsnprintf(diagnostic->message, sizeof(diagnostic->message), format, args);
    va_end(args);

    diagnostic->line = line;
}
