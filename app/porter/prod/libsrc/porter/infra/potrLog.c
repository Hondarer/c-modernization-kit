/**
 *******************************************************************************
 *  @file           potrLog.c
 *  @brief          porter ロガー実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/09
 *  @version        1.0.0
 *
 *  @details
 *  trace-com_util のプロキシモジュールです。\n
 *  OS レベルのトレース出力 (Linux: syslog、Windows: ETW)、
 *  ファイルトレース、stderr 出力のすべてを trace-com_util に委任します。
 *
 *  | OS      | 出力先                                                                                          |
 *  | ------- | ----------------------------------------------------------------------------------------------- |
 *  | Linux   | syslog (trace-com_util 経由)、ログファイル (trace-com_util 経由)、stderr (trace-com_util 経由、console 指定時) |
 *  | Windows | ETW (trace-com_util 経由)、ログファイル (trace-com_util 経由)、stderr (trace-com_util 経由、console 指定時)    |
 *
 *  @par            スレッド セーフティ
 *  本モジュールはスレッドセーフです。\n
 *  すべての出力制御は trace-com_util が内部で排他制御を行います。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <com_util/trace/trace.h>

#include <porter_const.h>
#include <porter.h>

#include "potrLog.h"

/* ── グローバルロガー状態 ──────────────────────────────────────────────── */

/** トレースプロバイダハンドル。trace_logger_create() で一度だけ初期化する。 */
static trace_logger_t *s_trace = NULL;

/* ── ユーティリティ ────────────────────────────────────────────────────── */

/**
 *  @brief  パスの末尾ファイル名部分を返す。
 *
 *  @details
 *  '/' または '\\' の最後の出現以降の文字列を返します。\n
 *  区切り文字が存在しない場合は path をそのまま返します。
 */
static const char *log_basename(const char *path)
{
    const char *p    = path;
    const char *last = path;

    while (*p != '\0')
    {
        if (*p == '/' || *p == '\\')
        {
            last = p + 1;
        }
        p++;
    }
    return last;
}

/* ── 公開 API ─────────────────────────────────────────────────────────── */

/* doxygen コメントは porter.h に記載 */
POTR_EXPORT int POTR_API potrLogConfig(PotrLogLevel  level,
                                   const char   *log_file,
                                   int           console)
{
    trace_level_t trc_level;

    /* level の範囲チェック。PotrLogLevel と trace_level_t は値が一致するため直接キャストする。 */
    if ((int)level < 0 || (int)level > (int)POTR_TRACE_NONE)
    {
        return POTR_ERROR;
    }
    trc_level = (trace_level_t)(int)level;

    /* 初回呼び出し: トレースプロバイダを初期化する。 */
    if (s_trace == NULL)
    {
        s_trace = trace_logger_create();
        if (s_trace == NULL)
        {
            return POTR_ERROR;
        }
        trace_logger_set_name(s_trace, "porter", 0);
    }
    else
    {
        /* stopped 状態に遷移させる (冪等)。 */
        trace_logger_stop(s_trace);
    }

    /* OS トレース (syslog / ETW) のしきい値レベルを設定する。 */
    trace_logger_set_os_level(s_trace, trc_level);

    /* ファイルトレースを設定する。
     * log_file が NULL または空文字列の場合は path=NULL を渡してファイルトレースを無効化する。
     * max_bytes=0, generations=0 で既定値 (10 MB / 5 世代) を使用する。 */
    if (trace_logger_set_file_level(s_trace,
                             (log_file != NULL && log_file[0] != '\0') ? log_file : NULL,
                             trc_level, 0, 0) != 0)
    {
        return POTR_ERROR;
    }

    /* stderr: console フラグに応じて trace-com_util のスレッショルドを設定する。
     * console != 0 の場合は trc_level 以上を出力、0 の場合は完全に抑止する。 */
    trace_logger_set_stderr_level(s_trace, console ? trc_level : TRACE_LEVEL_NONE);

    /* POTR_TRACE_NONE でない場合のみ出力を開始する。 */
    if (level != POTR_TRACE_NONE)
    {
        trace_logger_start(s_trace);
    }

    return POTR_SUCCESS;
}

/* doxygen コメントは potrLog.h に記載 */
void potr_log_write(PotrLogLevel level, const char *file, int line,
                    const char *fmt, ...)
{
    char             msg[512];
    va_list          ap;
    trace_level_t min_lv;
    trace_level_t lv;

    if (s_trace == NULL)
    {
        return;
    }

    /* 3 出力先のうち最も緩いスレッショルドより重大度が低ければ早期リターンする。
     * (数値が大きい = 重大度が低い)
     * trace_logger_writef 内部でも同等のチェックが行われるが、vsnprintf の実行を
     * 省くためにここで確認する。 */
    min_lv = trace_logger_get_os_level(s_trace);
    lv     = trace_logger_get_file_level(s_trace);
    if ((int)lv < (int)min_lv) { min_lv = lv; }
    lv     = trace_logger_get_stderr_level(s_trace);
    if ((int)lv < (int)min_lv) { min_lv = lv; }
    if ((int)level > (int)min_lv || (int)min_lv >= (int)TRACE_LEVEL_NONE)
    {
        return;
    }

    /* メッセージをフォーマット */
    va_start(ap, fmt);
    (void)vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    /* trace-com_util 経由で OS (syslog / ETW)、ファイル、stderr に出力する。
     * trace_logger_writef は内部でスレッドセーフのため、ロック不要。
     * PotrLogLevel と trace_level_t は値が一致するため直接キャストする。
     * メッセージに [file:line] を含めてソース位置を記録する。 */
    (void)trace_logger_writef(s_trace, (trace_level_t)(int)level,
                       "[%s:%d] %s",
                       log_basename(file), line, msg);
}
