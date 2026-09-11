/**
 *******************************************************************************
 *  @file           sample_trace_level.h
 *  @brief          この app がカタログの分類値として使うトレース レベルを定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/12
 *  @version        1.0.0
 *
 *  本ヘッダーは `prod/src/cmd/string-catalog-sample/` のモジュール私有ヘッダーです。\n
 *  同ディレクトリの実装ファイルからだけ `#include "sample_trace_level.h"` で取り込みます。
 *
 *  分類値の意味付けは利用者の取り決めであり、カタログ定義から生成されるものではありません。\n
 *  そのため生成物 `sample_messages.h` から分離し、手書きで保守します。\n
 *  生成器は、カタログ定義に書かれた分類値の定数名をそのまま出力します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef SAMPLE_TRACE_LEVEL_H
#define SAMPLE_TRACE_LEVEL_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          文字列の重大度を表すトレース レベルです。
     *
     *  カタログの分類値 (@ref string_catalog_entry::category) として使用します。\n
     *  ライブラリは分類値を解釈しないため、この意味付けは利用者側の取り決めです。
     *
     *  値は `app/c-platform` の `cplat_trace_level` と同一です。\n
     *  この app は標準 C だけで完結するサンプルであり、cplat に依存しないため、
     *  同じ値をここで再実装しています。\n
     *  cplat を利用する app へ移植する場合は、値が同じであるため変換表なしで置き換えられます。
     *
     *  | sample_trace_level          | ETW Level         | syslog severity |
     *  | --------------------------- | ----------------- | --------------- |
     *  | SAMPLE_TRACE_LEVEL_CRITICAL | Critical (1)      | LOG_CRIT (2)    |
     *  | SAMPLE_TRACE_LEVEL_ERROR    | Error (2)         | LOG_ERR (3)     |
     *  | SAMPLE_TRACE_LEVEL_WARNING  | Warning (3)       | LOG_WARNING (4) |
     *  | SAMPLE_TRACE_LEVEL_INFO     | Informational (4) | LOG_INFO (6)    |
     *  | SAMPLE_TRACE_LEVEL_VERBOSE  | Verbose (5)       | LOG_DEBUG (7)   |
     *  | SAMPLE_TRACE_LEVEL_DEBUG    | Verbose (5)       | LOG_DEBUG (7)   |
     *
     *  @ref SAMPLE_TRACE_LEVEL_CRITICAL は 0 であり、分類なしと同じ値です。\n
     *  分類値を取得しただけでは、登録されていない文字列と区別できません。
     *
     *  本 app はトレースの出力機構を持ちません。\n
     *  レベルは、利用側が出力先や絞り込みを決めるための情報として保持します。
     */
    typedef enum sample_trace_level
    {
        SAMPLE_TRACE_LEVEL_CRITICAL = 0, /**< 致命的エラー。 */
        SAMPLE_TRACE_LEVEL_ERROR = 1,    /**< エラー。 */
        SAMPLE_TRACE_LEVEL_WARNING = 2,  /**< 警告。 */
        SAMPLE_TRACE_LEVEL_INFO = 3,     /**< 情報。 */
        SAMPLE_TRACE_LEVEL_VERBOSE = 4,  /**< 詳細な診断情報。 */
        SAMPLE_TRACE_LEVEL_DEBUG = 5,    /**< 最も詳細な診断情報。 */
        SAMPLE_TRACE_LEVEL_NONE = 6      /**< 出力しない。 */
    } sample_trace_level;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAMPLE_TRACE_LEVEL_H */
