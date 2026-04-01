/**
 *******************************************************************************
 *  @file           potrLog.c
 *  @brief          porter ロガー実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/09
 *  @version        1.0.0
 *
 *  @details
 *  クロスプラットフォーム対応のロガーモジュールです。\n
 *  OS レベルのトレース出力 (Linux: syslog、Windows: ETW) と
 *  ファイルトレースは trace-util に委任します。\n
 *  stderr 出力 (console フラグ) は本モジュールが直接行います。
 *
 *  | OS      | 出力先                                                                                          |
 *  | ------- | ----------------------------------------------------------------------------------------------- |
 *  | Linux   | syslog (trace-util 経由)、ログファイル (trace-util 経由)、stderr (console 指定時)               |
 *  | Windows | ETW (trace-util 経由)、ログファイル (trace-util 経由)、stderr (console 指定時)                  |
 *
 *  @par            スレッド セーフティ
 *  本モジュールはスレッドセーフです。\n
 *  OS / ファイルへの出力は trace-util が内部で排他制御を行います。\n
 *  stderr への出力は本モジュールが mutex で排他制御します。\n
 *  Linux では PTHREAD_MUTEX_INITIALIZER による静的初期化済みミューテックスを使用します。\n
 *  Windows では INIT_ONCE_STATIC_INIT + InitOnceExecuteOnce による遅延初期化を使用します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifndef _WIN32
    #include <pthread.h>
    #include <time.h>
#else /* _WIN32 */
    #include <windows.h>
#endif /* _WIN32 */

#include <trace-util.h>

#include <porter_const.h>
#include <porter.h>

#include "potrLog.h"

/* ── ログレベル文字列 ───────────────────────────────────────────────────── */

/**
 *  ログレベル表示文字列テーブル (POTR_LOG_FATAL〜POTR_LOG_DEBUG の 5 エントリ)。
 *  インデックスは PotrLogLevel の数値と一致する。
 */
static const char * const s_level_str[] = {
    "FATAL", /* POTR_LOG_FATAL (0) */
    "ERROR", /* POTR_LOG_ERROR (1) */
    "WARN ", /* POTR_LOG_WARN  (2) */
    "INFO ", /* POTR_LOG_INFO  (3) */
    "DEBUG", /* POTR_LOG_DEBUG (4) */
};

/* ── グローバルロガー状態 ──────────────────────────────────────────────── */

/** トレースプロバイダハンドル。trace_init() で一度だけ初期化する。 */
static trace_provider_t *s_trace = NULL;

/** 出力ログレベル (POTR_LOG_OFF = 無効)。高速パスでロックなし読み取りするため volatile。 */
static volatile int g_log_level = (int)POTR_LOG_OFF;

/** stderr 出力フラグ (非 0: 有効)。 */
static int g_log_console = 0;

/* ── console (stderr) 専用 mutex ──────────────────────────────────────── */

#ifndef _WIN32

static pthread_mutex_t g_console_mutex = PTHREAD_MUTEX_INITIALIZER;

static void c_lock(void)
{
    pthread_mutex_lock(&g_console_mutex);
}

static void c_unlock(void)
{
    pthread_mutex_unlock(&g_console_mutex);
}

#else /* _WIN32 */

static INIT_ONCE        g_console_init_once = INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION g_console_mutex;

/** InitOnceExecuteOnce コールバック: CRITICAL_SECTION を初期化する。 */
static BOOL CALLBACK console_init_mutex_func(PINIT_ONCE once, PVOID param, PVOID *ctx)
{
    (void)once;
    (void)param;
    (void)ctx;
    InitializeCriticalSection(&g_console_mutex);
    return TRUE;
}

static void c_lock(void)
{
    InitOnceExecuteOnce(&g_console_init_once, console_init_mutex_func, NULL, NULL);
    EnterCriticalSection(&g_console_mutex);
}

static void c_unlock(void)
{
    LeaveCriticalSection(&g_console_mutex);
}

#endif /* _WIN32 */

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

/**
 *  @brief  現在時刻を "[YYYY-MM-DD HH:MM:SS.mmm]" 形式で buf に書き込む。
 *  @param[out] buf     出力バッファ (最低 96 バイト)。
 *  @param[in]  buflen  バッファサイズ。
 */
static void log_timestamp(char *buf, size_t buflen)
{
#ifndef _WIN32
    struct timespec ts;
    struct tm       tm_info;

    clock_gettime(CLOCK_REALTIME, &ts);
    localtime_r(&ts.tv_sec, &tm_info);
    (void)snprintf(buf, buflen,
                   "[%04d-%02d-%02d %02d:%02d:%02d.%03d]",
                   tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
                   tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec,
                   (int)(ts.tv_nsec / 1000000L));
#else /* _WIN32 */
    SYSTEMTIME st;
    GetLocalTime(&st);
    /* snprintf は _snprintf_s でなく標準の snprintf を使用 (C99 準拠)。 */
    (void)snprintf(buf, buflen,
                   "[%04d-%02d-%02d %02d:%02d:%02d.%03d]",
                   (int)st.wYear, (int)st.wMonth,  (int)st.wDay,
                   (int)st.wHour, (int)st.wMinute, (int)st.wSecond,
                   (int)st.wMilliseconds);
#endif /* _WIN32 */
}

/* ── 公開 API ─────────────────────────────────────────────────────────── */

/* doxygen コメントは porter.h に記載 */
POTR_EXPORT int POTR_API potrLogConfig(PotrLogLevel  level,
                                   const char   *log_file,
                                   int           console)
{
    enum trace_level trc_level;

    /* level の範囲チェック。PotrLogLevel と enum trace_level は値が一致するため直接キャストする。 */
    if ((int)level < 0 || (int)level > (int)POTR_LOG_OFF)
    {
        return POTR_ERROR;
    }
    trc_level = (enum trace_level)(int)level;

    /* 初回呼び出し: トレースプロバイダを初期化する。 */
    if (s_trace == NULL)
    {
        s_trace = trace_init();
        if (s_trace == NULL)
        {
            return POTR_ERROR;
        }
        trace_modify_name(s_trace, "porter", 0);
    }
    else
    {
        /* stopped 状態に遷移させる (冪等)。 */
        trace_stop(s_trace);
    }

    /* OS トレース (syslog / ETW) のしきい値レベルを設定する。 */
    trace_modify_ostrc(s_trace, trc_level);

    /* ファイルトレースを設定する。
     * log_file が NULL または空文字列の場合は path=NULL を渡してファイルトレースを無効化する。
     * max_bytes=0, generations=0 で既定値 (10 MB / 5 世代) を使用する。 */
    if (trace_modify_filetrc(s_trace,
                             (log_file != NULL && log_file[0] != '\0') ? log_file : NULL,
                             trc_level, 0, 0) != 0)
    {
        return POTR_ERROR;
    }

    /* console フラグとログレベルを更新する。 */
    c_lock();
    g_log_console = console;
    g_log_level   = (int)level;
    c_unlock();

    /* POTR_LOG_OFF でない場合のみ出力を開始する。 */
    if (level != POTR_LOG_OFF)
    {
        trace_start(s_trace);
    }

    return POTR_SUCCESS;
}

/* doxygen コメントは potrLog.h に記載 */
void potr_log_write(PotrLogLevel level, const char *file, int line,
                    const char *fmt, ...)
{
    char    msg[512];
    va_list ap;
    int     cur_level;

    /* クイックチェック (ロックなし)。ほとんどの場合ここで早期リターンする。
     * level > cur_level: メッセージの重大度がしきい値より低い (数値が大きい) → 出力しない。 */
    cur_level = g_log_level;
    if ((int)level > cur_level || cur_level >= (int)POTR_LOG_OFF)
    {
        return;
    }
    if (s_trace == NULL)
    {
        return;
    }

    /* メッセージを先にフォーマット (ロック外でスループット向上)。 */
    va_start(ap, fmt);
    (void)vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    {
        const char *lstr = ((int)level >= 0 && (int)level < (int)POTR_LOG_OFF)
                               ? s_level_str[(int)level]
                               : "?????";

        /* ── trace-util 経由で OS (syslog / ETW) + ファイルに出力 ────────
         * trace_writef は内部でスレッドセーフのため、ロック不要。
         * PotrLogLevel と enum trace_level は値が一致するため直接キャストする。
         * メッセージに [LEVEL] [file:line] を含めてソース位置を記録する。 */
        (void)trace_writef(s_trace, (enum trace_level)(int)level,
                           "[%s] [%s:%d] %s",
                           lstr, log_basename(file), line, msg);

        /* ── stderr 出力: タイムスタンプ付きフォーマット ─────────────── */
        if (g_log_console)
        {
            char ts[96];

            c_lock();
            /* ロック後に再確認 (設定変更の競合を回避)。 */
            if (g_log_console)
            {
                log_timestamp(ts, sizeof(ts));
                (void)fprintf(stderr, "%s [%s] [%s:%d] %s\n",
                              ts, lstr, log_basename(file), line, msg);
            }
            c_unlock();
        }
    }
}
