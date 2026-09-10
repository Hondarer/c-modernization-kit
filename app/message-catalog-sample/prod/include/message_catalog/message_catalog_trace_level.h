/**
 *******************************************************************************
 *  @file           message_catalog_trace_level.h
 *  @brief          メッセージの重大度を表すトレース レベルを定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  OS 非依存のトレース レベルを定義します。重大度は上から下へ低下します。
 *
 *  値は `app/c-platform` の `cplat_trace_level` と同一です。\n
 *  この app は標準 C だけで完結するサンプルであり、cplat に依存しないため、
 *  同じ値をここで再実装しています。\n
 *  cplat を利用する app へメッセージ カタログを移植する場合は、この列挙を
 *  `cplat_trace_level` へ置き換えられます。値が同じであるため、変換表は不要です。
 *
 *  | message_catalog_trace_level          | ETW Level         | syslog severity |
 *  | ------------------------------------ | ----------------- | --------------- |
 *  | MESSAGE_CATALOG_TRACE_LEVEL_CRITICAL | Critical (1)      | LOG_CRIT (2)    |
 *  | MESSAGE_CATALOG_TRACE_LEVEL_ERROR    | Error (2)         | LOG_ERR (3)     |
 *  | MESSAGE_CATALOG_TRACE_LEVEL_WARNING  | Warning (3)       | LOG_WARNING (4) |
 *  | MESSAGE_CATALOG_TRACE_LEVEL_INFO     | Informational (4) | LOG_INFO (6)    |
 *  | MESSAGE_CATALOG_TRACE_LEVEL_VERBOSE  | Verbose (5)       | LOG_DEBUG (7)   |
 *  | MESSAGE_CATALOG_TRACE_LEVEL_DEBUG    | Verbose (5)       | LOG_DEBUG (7)   |
 *
 *  本 app はトレースの出力機構を持ちません。\n
 *  レベルは、利用側が出力先や絞り込みを決めるための情報として保持します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef MESSAGE_CATALOG_MESSAGE_CATALOG_TRACE_LEVEL_H
#define MESSAGE_CATALOG_MESSAGE_CATALOG_TRACE_LEVEL_H

/**
 *  @ingroup        MESSAGE_CATALOG_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          メッセージの重大度を表すトレース レベルです。
     *
     *  値は `cplat_trace_level` と同一です。\n
     *  @ref MESSAGE_CATALOG_TRACE_LEVEL_NONE は重大度ではなく、出力しないことを表します。\n
     *  カタログに存在しないメッセージ ID を指定した場合の戻り値としても使用します。
     */
    typedef enum message_catalog_trace_level
    {
        MESSAGE_CATALOG_TRACE_LEVEL_CRITICAL = 0, /**< 致命的エラー。 */
        MESSAGE_CATALOG_TRACE_LEVEL_ERROR = 1,    /**< エラー。 */
        MESSAGE_CATALOG_TRACE_LEVEL_WARNING = 2,  /**< 警告。 */
        MESSAGE_CATALOG_TRACE_LEVEL_INFO = 3,     /**< 情報。 */
        MESSAGE_CATALOG_TRACE_LEVEL_VERBOSE = 4,  /**< 詳細な診断情報。 */
        MESSAGE_CATALOG_TRACE_LEVEL_DEBUG = 5,    /**< 最も詳細な診断情報。 */
        MESSAGE_CATALOG_TRACE_LEVEL_NONE = 6      /**< 出力しない。 */
    } message_catalog_trace_level;

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* MESSAGE_CATALOG_MESSAGE_CATALOG_TRACE_LEVEL_H */
