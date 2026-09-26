/**
 *******************************************************************************
 *  @file           sample_worker_context.c
 *  @brief          トレースのコンテキスト引数として付加する値の取得関数を実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/26
 *  @version        0.1.0
 *
 *  `app/string-catalog-sample/prod/libsrc/samplecatalog/samplecatalog_context.c` の巡回連番と同じ方式です。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#include "sample_worker_context.h"

#include <cplat/base/platform.h>

#include <stdint.h>
#if defined(PLATFORM_WINDOWS)
    #include <cplat/win32/win32.h>
#endif /* PLATFORM_WINDOWS */

/** 呼び出しごとに増加するカウンターです。剰余で巡回させるため、カウンター自体は折り返しません。 */
static volatile int32_t s_sequence_number = 0;

/* Doxygen コメントは、ヘッダーに記載 */

int32_t sample_worker_next_sequence_number(void)
{
    int32_t current;

#if defined(PLATFORM_LINUX)
    /* 加算後の値を受け取る。
       see: https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html */
    current = __atomic_add_fetch(&s_sequence_number, 1, __ATOMIC_RELAXED);
#elif defined(PLATFORM_WINDOWS)
    /* InterlockedIncrement も加算後の値を返す。
       see: https://learn.microsoft.com/windows/win32/api/winnt/nf-winnt-interlockedincrement */
    current = (int32_t)InterlockedIncrement((volatile LONG *)&s_sequence_number);
#else
    current = ++s_sequence_number;
#endif /* PLATFORM_ */

    return (int32_t)(((uint32_t)current - 1U) % (uint32_t)SAMPLE_WORKER_SEQUENCE_NUMBER_MAX) + 1;
}
