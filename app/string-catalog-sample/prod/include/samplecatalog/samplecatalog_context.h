/**
 *******************************************************************************
 *  @file           samplecatalog_context.h
 *  @brief          トレースのコンテキスト引数として付加する値の取得関数を宣言します。
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

    /** 巡回連番の上限値です。この値を超えると 1 へ戻ります。 */
#define SAMPLECATALOG_SEQUENCE_NUMBER_MAX 999

    /**
     *  @brief          トレースへ付与する連番を 1 つ進めて返します。
     *  @return         1 から @ref SAMPLECATALOG_SEQUENCE_NUMBER_MAX までの値です。
     *
     *  カタログ設定 `catalog_settings.jsonc` の `context` が、本関数をコンテキスト引数の取得式として
     *  参照します。トレースの出力要求ごとに 1 回評価され、位置指定 `{46}` の値になります。\n
     *  上限に達した次の呼び出しで 1 へ戻るため、番号は巡回します。
     *
     *  本関数はコンテキスト引数の動作実証を目的とした検証用の値です。\n
     *  出力の連続性を目視で確認するためのものであり、欠落の検出には使用できません。
     *
     *  コンテキスト引数の取得式は、生成ヘッダーの @c static @c inline 関数内で展開されます。\n
     *  利用側の翻訳単位から呼び出されるため、本関数はライブラリの外部へ公開します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。\n
     *  複数のスレッドから同時に呼び出しても、同一の値を重複して返しません。
     */
    SAMPLECATALOG_EXPORT int32_t SAMPLECATALOG_API samplecatalog_next_sequence_number(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* SAMPLECATALOG_CONTEXT_H */
