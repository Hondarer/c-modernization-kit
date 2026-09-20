/**
 *******************************************************************************
 *  @file           samplecatalog_context.h
 *  @brief          トレースの文脈引数として付け加える値の取得を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/21
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SAMPLECATALOG_CONTEXT_H
#define SAMPLECATALOG_CONTEXT_H

#include <samplecatalog/samplecatalog_export.h>
#include <stdint.h>

/**
 *  @addtogroup     SAMPLECATALOG
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** 巡回する連番の上限です。この値の次は 1 へ戻ります。 */
#define SAMPLECATALOG_SEQUENCE_NUMBER_MAX 999

    /**
     *  @brief          トレースへ付ける連番を 1 つ進めて返します。
     *  @return         1 から @ref SAMPLECATALOG_SEQUENCE_NUMBER_MAX までの値です。
     *
     *  カタログ設定 `catalog_settings.jsonc` の `context` が、本関数を文脈引数の取得式として
     *  参照します。トレースの出力要求ごとに 1 回評価され、位置指定 `{46}` の値になります。\n
     *  上限に達した次の呼び出しで 1 へ戻るため、番号は巡回します。
     *
     *  本関数は文脈引数の実証を目的とした試験用の値です。\n
     *  出力の連続性を目視で確かめるためのものであり、欠落の検出には使用できません。
     *
     *  文脈引数の取得式は、生成ヘッダーの @c static @c inline の中で展開されます。\n
     *  利用側のコンパイル単位から呼ばれるため、本関数はライブラリの外部へ公開します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。\n
     *  複数のスレッドから同時に呼び出しても、同じ値を 2 回返しません。
     */
    SAMPLECATALOG_EXPORT int32_t SAMPLECATALOG_API samplecatalog_next_sequence_number(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* SAMPLECATALOG_CONTEXT_H */
