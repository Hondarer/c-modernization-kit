/**
 *******************************************************************************
 *  @file           libsrc/samplecatalog/samplecatalog_context.c
 *  @brief          トレースの文脈引数として付け加える値の取得を実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/21
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <cplat/base/platform.h>
#include <samplecatalog/samplecatalog_context.h>
#include <stdint.h>
#if defined(PLATFORM_WINDOWS)
    #include <cplat/win32/win32.h>
#endif /* PLATFORM_WINDOWS */

/**
 *  @brief          直前に返した連番です。
 *
 *  文脈引数の取得式は出力の要求ごとに評価され、並行して呼ばれます。\n
 *  待ち合わせを含めない契約のため、ロックではなく不可分な加算で更新します。
 */
static volatile int32_t s_sequence_number = 0;

/* Doxygen コメントは、ヘッダーに記載 */

int32_t samplecatalog_next_sequence_number(void)
{
    int32_t current;

#if defined(PLATFORM_LINUX)
    /* 加算後の値を受け取る。cplat の sym_loader が使用する組み込み関数と同じ系統。
       see: https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html */
    current = __atomic_add_fetch(&s_sequence_number, 1, __ATOMIC_RELAXED);
#elif defined(PLATFORM_WINDOWS)
    /* InterlockedIncrement も加算後の値を返す。
       see: https://learn.microsoft.com/windows/win32/api/winnt/nf-winnt-interlockedincrement */
    current = (int32_t)InterlockedIncrement((volatile LONG *)&s_sequence_number);
#else
    current = ++s_sequence_number;
#endif /* PLATFORM_ */

    /*
     * 上限で 1 へ戻す。カウンターそのものは折り返さずに増え続けるため、
     * 剰余で写像する。並行する呼び出しどうしで値が入れ替わることはあるが、
     * 同じ値を 2 回返すのは上限の周期を一巡したあとに限られる。
     */
    return (int32_t)(((uint32_t)current - 1U) % (uint32_t)SAMPLECATALOG_SEQUENCE_NUMBER_MAX) + 1;
}
