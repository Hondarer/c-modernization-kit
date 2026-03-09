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
 *  追加の OSS ライブラリを必要とせず、OS 標準 API のみを使用します。
 *
 *  | OS      | 出力先                                                                  |
 *  | ------- | ----------------------------------------------------------------------- |
 *  | Linux   | syslog (常時)、ログファイル (log_file 指定時)、stderr (console 指定時) |
 *  | Windows | ログファイル (log_file 指定時)、stderr (console 指定時)                 |
 *
 *  スレッドセーフ:
 *  - Linux:   PTHREAD_MUTEX_INITIALIZER による静的初期化済みミューテックスを使用。
 *  - Windows: INIT_ONCE_STATIC_INIT + InitOnceExecuteOnce による遅延初期化。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <syslog.h>
    #include <pthread.h>
    #include <time.h>
#endif

#include <porter_const.h>
#include <porter.h>

#include "potrLog.h"

/* ── ログレベル文字列 ───────────────────────────────────────────────────── */

/** ログレベル表示文字列テーブル (POTR_LOG_TRACE〜POTR_LOG_FATAL の 6 エントリ)。 */
static const char * const s_level_str[] = {
    "TRACE", /* POTR_LOG_TRACE */
    "DEBUG", /* POTR_LOG_DEBUG */
    "INFO ", /* POTR_LOG_INFO  */
    "WARN ", /* POTR_LOG_WARN  */
    "ERROR", /* POTR_LOG_ERROR */
    "FATAL", /* POTR_LOG_FATAL */
};

/* ── グローバルロガー状態 ──────────────────────────────────────────────── */

/** 出力ファイルパスの最大長 (終端 NUL を含む)。 */
#define POTR_LOG_FILE_PATH_MAX 512

/** 出力先ミューテックスおよびロガー設定。スレッド間で共有する。 */
#ifdef _WIN32
static INIT_ONCE        g_log_init_once = INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION g_log_mutex;

/** InitOnceExecuteOnce コールバック: CRITICAL_SECTION を初期化する。 */
static BOOL CALLBACK log_init_mutex_func(PINIT_ONCE once, PVOID param, PVOID *ctx)
{
    (void)once;
    (void)param;
    (void)ctx;
    InitializeCriticalSection(&g_log_mutex);
    return TRUE;
}

static void log_lock(void)
{
    InitOnceExecuteOnce(&g_log_init_once, log_init_mutex_func, NULL, NULL);
    EnterCriticalSection(&g_log_mutex);
}

static void log_unlock(void)
{
    LeaveCriticalSection(&g_log_mutex);
}

#else /* Linux */

static pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;
static int             g_syslog_open = 0; /**< openlog() 済みフラグ (1: 済み)。 */

static void log_lock(void)
{
    pthread_mutex_lock(&g_log_mutex);
}

static void log_unlock(void)
{
    pthread_mutex_unlock(&g_log_mutex);
}

#endif /* _WIN32 */

/** 出力ログレベル (POTR_LOG_OFF = 無効)。 */
static volatile int g_log_level   = (int)POTR_LOG_OFF;

/** stderr 出力フラグ (非 0: 有効)。 */
static int g_log_console = 0;

/** ログファイルパス (空文字列 = 無効)。 */
static char g_log_file_path[POTR_LOG_FILE_PATH_MAX];

/** ログファイル FILE ポインタ (NULL = 未オープン)。 */
static FILE *g_log_fp = NULL;

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
 *  @param[out] buf     出力バッファ (最低 28 バイト)。
 *  @param[in]  buflen  バッファサイズ。
 */
static void log_timestamp(char *buf, size_t buflen)
{
#ifdef _WIN32
    SYSTEMTIME st;
    GetLocalTime(&st);
    /* snprintf は _snprintf_s でなく標準の snprintf を使用 (C99 準拠)。 */
    (void)snprintf(buf, buflen,
                   "[%04d-%02d-%02d %02d:%02d:%02d.%03d]",
                   (int)st.wYear, (int)st.wMonth,  (int)st.wDay,
                   (int)st.wHour, (int)st.wMinute, (int)st.wSecond,
                   (int)st.wMilliseconds);
#else
    struct timespec ts;
    struct tm       tm_info;

    clock_gettime(CLOCK_REALTIME, &ts);
    localtime_r(&ts.tv_sec, &tm_info);
    (void)snprintf(buf, buflen,
                   "[%04d-%02d-%02d %02d:%02d:%02d.%03d]",
                   tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
                   tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec,
                   (int)(ts.tv_nsec / 1000000L));
#endif
}

#ifndef _WIN32
/**
 *  @brief  PotrLogLevel を syslog の priority に変換する (Linux のみ)。
 */
static int level_to_syslog_priority(PotrLogLevel level)
{
    switch (level)
    {
        case POTR_LOG_TRACE: return LOG_DEBUG;
        case POTR_LOG_DEBUG: return LOG_DEBUG;
        case POTR_LOG_INFO:  return LOG_INFO;
        case POTR_LOG_WARN:  return LOG_WARNING;
        case POTR_LOG_ERROR: return LOG_ERR;
        case POTR_LOG_FATAL: return LOG_CRIT;
        default:             return LOG_DEBUG;
    }
}
#endif /* !_WIN32 */

/* ── 公開 API ─────────────────────────────────────────────────────────── */

/* doxygen コメントは porter.h に記載 */
POTR_API int POTRAPI potrLogConfig(PotrLogLevel  level,
                                   const char   *log_file,
                                   int           console)
{
    FILE *new_fp = NULL;

    /* log_file を事前にオープンしておく (ミューテックス外で実施し、ロック時間を短縮)。 */
    if (log_file != NULL && log_file[0] != '\0')
    {
        new_fp = fopen(log_file, "a");
        if (new_fp == NULL)
        {
            return POTR_ERROR;
        }
    }

    log_lock();

    /* 既存ファイルをクローズ。 */
    if (g_log_fp != NULL)
    {
        fclose(g_log_fp);
        g_log_fp = NULL;
    }

#ifndef _WIN32
    /* 既存 syslog をクローズして再度 openlog する (ident は "porter" 固定)。 */
    if (g_syslog_open)
    {
        closelog();
        g_syslog_open = 0;
    }
#endif

    /* 設定を更新する。 */
    g_log_level   = (int)level;
    g_log_console = console;
    g_log_fp      = new_fp;

    if (log_file != NULL && log_file[0] != '\0')
    {
        size_t len = strlen(log_file);
        if (len >= POTR_LOG_FILE_PATH_MAX)
        {
            len = POTR_LOG_FILE_PATH_MAX - 1;
        }
        memcpy(g_log_file_path, log_file, len);
        g_log_file_path[len] = '\0';
    }
    else
    {
        g_log_file_path[0] = '\0';
    }

#ifndef _WIN32
    /* level が POTR_LOG_OFF でない場合のみ syslog を開く。 */
    if (level != POTR_LOG_OFF)
    {
        openlog("porter", LOG_PID | LOG_NDELAY, LOG_USER);
        g_syslog_open = 1;
    }
#endif

    log_unlock();

    return POTR_SUCCESS;
}

/* doxygen コメントは potrLog.h に記載 */
void potr_log_write(PotrLogLevel level, const char *file, int line,
                    const char *fmt, ...)
{
    char    msg[512];
    char    ts[32];
    va_list ap;
    int     cur_level;

    /* クイックチェック (ロックなし)。ほとんどの場合ここで早期リターンする。 */
    cur_level = g_log_level;
    if ((int)level < cur_level || cur_level >= (int)POTR_LOG_OFF)
    {
        return;
    }

    /* メッセージを先にフォーマット (ロック外でスループット向上)。 */
    va_start(ap, fmt);
    (void)vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    log_lock();

    /* ロック後に再確認 (設定変更の競合を回避)。 */
    cur_level = g_log_level;
    if ((int)level < cur_level || cur_level >= (int)POTR_LOG_OFF)
    {
        log_unlock();
        return;
    }

    {
        const char *lstr = ((int)level >= 0 && level <= POTR_LOG_FATAL)
                               ? s_level_str[(int)level]
                               : "?????";

#ifndef _WIN32
        /* ── Linux: syslog ─────────────────────────────────────────────── */
        if (g_syslog_open)
        {
            syslog(level_to_syslog_priority(level),
                   "[%s] [%s:%d] %s",
                   lstr, log_basename(file), line, msg);
        }
#endif

        /* ── ファイル / stderr: タイムスタンプ付きフォーマット ─────────── */
        if (g_log_fp != NULL || g_log_console)
        {
            log_timestamp(ts, sizeof(ts));

            if (g_log_fp != NULL)
            {
                (void)fprintf(g_log_fp, "%s [%s] [%s:%d] %s\n",
                              ts, lstr, log_basename(file), line, msg);
                (void)fflush(g_log_fp);
            }

            if (g_log_console)
            {
                (void)fprintf(stderr, "%s [%s] [%s:%d] %s\n",
                              ts, lstr, log_basename(file), line, msg);
            }
        }
    }

    log_unlock();
}
