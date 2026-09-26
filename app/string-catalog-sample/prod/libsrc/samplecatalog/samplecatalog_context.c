/**
 *******************************************************************************
 *  @file           libsrc/samplecatalog/samplecatalog_context.c
 *  @brief          トレースのコンテキスト引数として付加する値の取得関数を実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/21
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <cplat/sync/atomic.h>
#include <samplecatalog/samplecatalog_context.h>
#include <stdint.h>

/**
 *  @brief          直前に返した連番です。
 *
 *  コンテキスト引数の取得式は出力要求ごとに評価され、並行して呼び出されます。\n
 *  待機処理を含まない契約のため、ロックではなくアトミックな加算で更新します。\n
 *  値の公開ではなく計数だけが目的のため、順序の保証がない軽量な加算 (RELAXED) で足ります。
 */
static cplat_atomic_u32 s_sequence_number = CPLAT_ATOMIC_INIT(0U);

/* Doxygen コメントは、ヘッダーに記載 */

int32_t samplecatalog_next_sequence_number(void)
{
    /* 加算前の値が返るため、1 を足して加算後の値にそろえる。 */
    const uint32_t current = cplat_atomic_fetch_add_u32(&s_sequence_number, 1U, CPLAT_MEMORY_ORDER_RELAXED) + 1U;

    /*
     * 上限到達時に 1 へ戻す。カウンター自体は折り返さずに増加し続けるため、
     * 剰余によってマッピングする。並行する呼び出し間で順序が前後することはあるが、
     * 同一の値を重複して返すのは上限周期を一巡した後に限定される。
     */
    return (int32_t)((current - 1U) % (uint32_t)SAMPLECATALOG_SEQUENCE_NUMBER_MAX) + 1;
}
