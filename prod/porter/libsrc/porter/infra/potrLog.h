/**
 *******************************************************************************
 *  @file           potrLog.h
 *  @brief          porter 内部ログマクロ定義ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/09
 *  @version        1.0.0
 *
 *  @details
 *  porter ライブラリ内部でのみ使用するログ出力マクロを定義します。\n
 *  ライブラリ外部には公開しません。\n
 *  公開 API は porter.h の potrLogConfig() を参照してください。
 *
 *  使用方法:
 *  @code{.c}
 *  POTR_LOG(POTR_TRACE_INFO,  "service_id=%" PRId64 " opened", service_id);
 *  POTR_LOG(POTR_TRACE_ERROR, "socket bind failed: port=%u", port);
 *  POTR_LOG(POTR_TRACE_VERBOSE, "PING sent: seq=%u", seq);
 *  @endcode
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef POTR_LOG_H
#define POTR_LOG_H

#include <porter_type.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 *******************************************************************************
 *  @brief          ログメッセージを書き込みます (内部関数)。
 *  @param[in]      level   ログレベル。
 *  @param[in]      file    ソースファイル名 (__FILE__)。
 *  @param[in]      line    行番号 (__LINE__)。
 *  @param[in]      fmt     printf 形式のフォーマット文字列。
 *  @param[in]      ...     フォーマット引数。
 *
 *  @details
 *  g_log_level より低いレベルのメッセージは無視されます (高速パス)。\n
 *  本関数を直接呼び出さず、POTR_LOG マクロを使用してください。
 *******************************************************************************
 */
void potr_log_write(PotrLogLevel level, const char *file, int line,
                    const char *fmt, ...);

/**
 *  @def            POTR_LOG(level, ...)
 *  @brief          porter 内部ログ出力マクロ。
 *
 *  @details
 *  __FILE__ と __LINE__ を自動付加して potr_log_write() を呼び出します。\n
 *  設定レベルより低いメッセージは potr_log_write() 冒頭で早期リターンします。
 *
 *  @par            例
 *  @code{.c}
 *  POTR_LOG(POTR_TRACE_INFO,  "potrOpenService: service_id=%" PRId64 "", service_id);
 *  POTR_LOG(POTR_TRACE_WARNING,  "NACK received: seq=%u", seq);
 *  POTR_LOG(POTR_TRACE_ERROR, "socket() failed");
 *  @endcode
 */
#define POTR_LOG(level, ...) \
    potr_log_write((level), __FILE__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POTR_LOG_H */
