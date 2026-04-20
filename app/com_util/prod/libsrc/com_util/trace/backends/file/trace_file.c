/**
 *******************************************************************************
 *  @file           trace_file.c
 *  @brief          ファイルトレースプロバイダ実装ファイル。
 *  @author         Tetsuo Honda
 *  @date           2026/04/03
 *  @version        1.0.0
 *
 *  ファイルへのトレースログ書き込みプロバイダを提供します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/clock/clock.h>
#include <com_util/fs/file_io.h>
#include <com_util/fs/path_max.h>
#include <com_util/trace/trace_file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trace_file_internal.h"

#if defined(PLATFORM_LINUX)
    #include <fcntl.h>
    #include <limits.h>
    #include <pthread.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif /* PLATFORM_LINUX */

/* ===== 内部定数 ===== */

/** 1 行分のスタックバッファサイズ。 */
#define TRACE_FILE_LINE_BUF 1100

/** ファイル書き込みロック取得のタイムアウト (ミリ秒)。 */
#define FILE_LOCK_TIMEOUT_MS 100

/** タイムスタンプ部分の文字数 ("YYYY-MM-DD HH:MM:SS.mmm" = 23 文字)。 */
#define TRACE_FILE_TS_LEN 23

/** ローテーションパスのサフィックス最大長 (".999\0" = 5 文字)。 */
#define TRACE_FILE_SUFFIX_MAX 5

/* ===== 内部構造体 ===== */

/**
 *  @brief  ファイルトレースプロバイダハンドル構造体 (内部定義)。
 */
struct trace_file_sink
{
    /** ヒープ確保済みファイルパス文字列。 */
    char *path;
    /** ファイル 1 世代あたりの最大バイト数。 */
    size_t max_bytes;
    /** 現ファイルへの書き込み済みバイト数 (インメモリ追跡)。 */
    size_t current_bytes;
    /** 保持する旧世代数。 */
    int generations;

#if defined(PLATFORM_LINUX)
    /** ファイルディスクリプタ。-1 = 未開。 */
    int fd;
    /** スレッド安全のための mutex。 */
    pthread_mutex_t mutex;
    /** mutex が初期化済みかどうかのフラグ。 */
    int mutex_initialized;
    /** パディング (構造体サイズを 8 バイト境界に揃える)。 */
    int _pad_end;
#elif defined(PLATFORM_WINDOWS)
    /** ファイルハンドル。INVALID_HANDLE_VALUE = 未開。 */
    HANDLE fh;
    /** スレッド安全のためのクリティカルセクション。 */
    CRITICAL_SECTION cs;
    /** cs が初期化済みかどうかのフラグ。 */
    int cs_initialized;
#endif /* PLATFORM_ */
};

/* ===== 内部ヘルパー関数 ===== */

/**
 *  @brief  トレースレベル整数をレベル文字に変換する。
 */
static char level_char(int level)
{
    switch (level)
    {
    case TRACE_LEVEL_CRITICAL:
        return 'C';
    case TRACE_LEVEL_ERROR:
        return 'E';
    case TRACE_LEVEL_WARNING:
        return 'W';
    case TRACE_LEVEL_INFO:
        return 'I';
    case TRACE_LEVEL_VERBOSE:
        return 'V';
    default:
        return 'D';
    }
}

/**
 *  @brief  現在時刻を "YYYY-MM-DD HH:MM:SS.mmm" (UTC) 形式でバッファへ書き込む。
 *  @param  buf      書き込み先バッファ。
 *  @param  buf_size バッファサイズ (TRACE_FILE_TS_LEN + 1 以上を推奨)。
 */
static void format_timestamp(char *buf, int buf_size)
{
    struct tm utc_tm;
    int32_t tv_nsec;

    clock_get_realtime_utc(&utc_tm, &tv_nsec);

    /* -Wformat-truncation の抑制: clock_get_realtime_utc() が返す tm 構造体の各フィールドは
     * UTC 分解済みの正規化値であり (tm_mon: 0-11, tm_mday: 1-31 等)、出力は常に
     * 23 文字以内に収まる。GCC は int 型の理論上の最大範囲 [-2147483648, 2147483647]
     * を使って静的検証するため false positive が発生する。pragma はその誤報を局所的に
     * 抑制する。 */
    #if defined(COMPILER_GCC)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat-truncation"
    #endif /* COMPILER_GCC */
    snprintf(buf, (size_t)buf_size, "%04d-%02d-%02d %02d:%02d:%02d.%03d", utc_tm.tm_year + 1900, utc_tm.tm_mon + 1,
             utc_tm.tm_mday, utc_tm.tm_hour, utc_tm.tm_min, utc_tm.tm_sec, (int)(tv_nsec / 1000000));
    #if defined(COMPILER_GCC)
        #pragma GCC diagnostic pop
    #endif /* COMPILER_GCC */
}

/**
 *  @brief  ファイルを追記モードで開き current_bytes を初期サイズで初期化する。
 *  @return 成功 0 / 失敗 -1。
 */
static int open_file(trace_file_sink_t *p)
{
#if defined(PLATFORM_LINUX)
    struct stat st;

    p->fd = open(p->path, O_WRONLY | O_APPEND | O_CREAT | O_DSYNC, 0644);

    if (p->fd == -1)
    {
        p->current_bytes = 0;
        return -1;
    }

    /* 既存ファイルサイズを取得してインメモリカウンタを初期化する */
    if (fstat(p->fd, &st) == 0)
    {
        p->current_bytes = (size_t)st.st_size;
    }
    else
    {
        p->current_bytes = 0;
    }

    return 0;
#elif defined(PLATFORM_WINDOWS)
    LARGE_INTEGER pos;
    LARGE_INTEGER size;

    p->fh = CreateFileA(p->path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);

    if (p->fh == INVALID_HANDLE_VALUE)
    {
        p->current_bytes = 0;
        return -1;
    }

    /* 末尾へ移動して追記モードにする */
    pos.QuadPart = 0;
    SetFilePointerEx(p->fh, pos, NULL, FILE_END);

    /* 既存ファイルサイズを取得してインメモリカウンタを初期化する */
    if (GetFileSizeEx(p->fh, &size))
    {
        p->current_bytes = (size_t)size.QuadPart;
    }
    else
    {
        p->current_bytes = 0;
    }

    return 0;
#endif /* PLATFORM_ */
}

/**
 *  @brief  ローテーション後の新規ファイルを空で作成して開く。
 *          current_bytes は必ず 0 に設定される。
 *  @return 成功 0 / 失敗 -1。
 */
static int open_file_truncate(trace_file_sink_t *p)
{
    p->current_bytes = 0;

#if defined(PLATFORM_LINUX)
    p->fd = open(p->path, O_WRONLY | O_APPEND | O_CREAT | O_TRUNC | O_DSYNC, 0644);

    return (p->fd != -1) ? 0 : -1;
#elif defined(PLATFORM_WINDOWS)
    p->fh = CreateFileA(p->path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);

    return (p->fh != INVALID_HANDLE_VALUE) ? 0 : -1;
#endif /* PLATFORM_ */
}

/**
 *  @brief  開いているファイルを閉じる。未開の場合は何もしない (冪等)。
 */
static void close_file(trace_file_sink_t *p)
{
#if defined(PLATFORM_LINUX)
    if (p->fd != -1)
    {
        close(p->fd);
        p->fd = -1;
    }
#elif defined(PLATFORM_WINDOWS)
    if (p->fh != INVALID_HANDLE_VALUE)
    {
        CloseHandle(p->fh);
        p->fh = INVALID_HANDLE_VALUE;
    }
#endif /* PLATFORM_ */
}

/**
 *  @brief  トレースファイルをローテーションする。
 *  @details ロック保持中から呼ばれる。\n
 *           リネームに失敗した場合はその世代でカスケードを打ち切り、
 *           呼び出し元をブロックせずに続行する (ベストエフォート)。
 */
static void rotate_file(trace_file_sink_t *p)
{
    /* パス構築用スタックバッファ */
    char old_path[PLATFORM_PATH_MAX];
    char new_path[PLATFORM_PATH_MAX];
    int gen;

    close_file(p);

    /* 最老世代のファイルを削除する (失敗は無視) */
    snprintf(new_path, sizeof(new_path), "%s.%d", p->path, p->generations);
#if defined(PLATFORM_LINUX)
    unlink(new_path);
#elif defined(PLATFORM_WINDOWS)
    DeleteFileA(new_path);
#endif /* PLATFORM_ */

    /* path.(gen-1) → path.gen のカスケードリネーム */
    for (gen = p->generations; gen >= 1; gen--)
    {
        /* 移動先: path.gen */
        snprintf(new_path, sizeof(new_path), "%s.%d", p->path, gen);

        /* 移動元: gen==1 のときは path そのもの */
        if (gen == 1)
        {
            /* old_path に path をコピー */
            snprintf(old_path, sizeof(old_path), "%s", p->path);
        }
        else
        {
            snprintf(old_path, sizeof(old_path), "%s.%d", p->path, gen - 1);
        }

        if (com_util_rename(old_path, new_path) != 0)
        {
            /* リネーム失敗: カスケードをここで打ち切る */
            break;
        }
    }

    /* 新規ファイルを作成して開く (失敗しても fh=INVALID/fd=-1 のまま続行) */
    open_file_truncate(p);
}

/* ===== 公開 API ===== */

/* doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT trace_file_sink_t *COM_UTIL_API trace_file_sink_create(const char *path, size_t max_bytes,
                                                                           int generations)
{
    trace_file_sink_t *handle;
    size_t path_len;

    if (path == NULL)
    {
        return NULL;
    }

    path_len = strlen(path);

    /* パスが長すぎてローテーションサフィックスを付加できない場合は拒否する */
    if (path_len + TRACE_FILE_SUFFIX_MAX >= (size_t)PLATFORM_PATH_MAX)
    {
        return NULL;
    }

    handle = (trace_file_sink_t *)malloc(sizeof(trace_file_sink_t));
    if (handle == NULL)
    {
        return NULL;
    }

    /* パス文字列をヒープへ複製する */
    handle->path = (char *)malloc(path_len + 1);
    if (handle->path == NULL)
    {
        free(handle);
        return NULL;
    }
    memcpy(handle->path, path, path_len + 1);

    handle->max_bytes = (max_bytes > 0) ? max_bytes : TRACE_FILE_SINK_DEFAULT_MAX_BYTES;
    handle->generations = (generations > 0) ? generations : TRACE_FILE_SINK_DEFAULT_GENERATIONS;
    handle->current_bytes = 0;

    /* プラットフォームごとの同期プリミティブを初期化する */
#if defined(PLATFORM_LINUX)
    handle->fd = -1;
    handle->mutex_initialized = 0;
    if (pthread_mutex_init(&handle->mutex, NULL) != 0)
    {
        free(handle->path);
        free(handle);
        return NULL;
    }
    handle->mutex_initialized = 1;
#elif defined(PLATFORM_WINDOWS)
    handle->fh = INVALID_HANDLE_VALUE;
    handle->cs_initialized = 0;
    InitializeCriticalSectionAndSpinCount(&handle->cs, 1000);
    handle->cs_initialized = 1;
#endif /* PLATFORM_ */

    /* ファイルを開く; 失敗したらリソースを解放して NULL を返す */
    if (open_file(handle) != 0)
    {
#if defined(PLATFORM_LINUX)
        if (handle->mutex_initialized)
        {
            pthread_mutex_destroy(&handle->mutex);
        }
#elif defined(PLATFORM_WINDOWS)
        if (handle->cs_initialized)
        {
            DeleteCriticalSection(&handle->cs);
        }
#endif /* PLATFORM_ */
        free(handle->path);
        free(handle);
        return NULL;
    }

    return handle;
}

/* doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API trace_file_sink_write(trace_file_sink_t *handle, int level, const char *message)
{
    char ts[TRACE_FILE_TS_LEN + 1];
    char buf[TRACE_FILE_LINE_BUF];
    int len;
    int ret;

    if (handle == NULL || message == NULL)
    {
        return 0;
    }

    /* タイムスタンプはロック外で取得する (共有状態へのアクセスなし) */
    format_timestamp(ts, (int)sizeof(ts));

    /* 1 行全体をスタックバッファへフォーマットする (syscall 回数を最小化) */
    len = snprintf(buf, sizeof(buf), "%s %c %s\n", ts, level_char(level), message);
    if (len <= 0)
    {
        return -1;
    }
    if (len >= (int)sizeof(buf))
    {
        /* 切り詰め: バッファ末尾を必ず改行で終端する */
        len = (int)sizeof(buf) - 1;
        buf[len - 1] = '\n';
    }

    /* ロック取得 (タイムアウト付き) */
#if defined(PLATFORM_LINUX)
    {
        struct timespec abs_timeout;
        clock_get_realtime_deadline_ms(FILE_LOCK_TIMEOUT_MS, &abs_timeout);
        if (pthread_mutex_timedlock(&handle->mutex, &abs_timeout) != 0)
        {
            return -1;
        }
    }
#elif defined(PLATFORM_WINDOWS)
    {
        uint64_t deadline = clock_get_monotonic_ms() + (uint64_t)FILE_LOCK_TIMEOUT_MS;
        while (!TryEnterCriticalSection(&handle->cs))
        {
            if (clock_get_monotonic_ms() >= deadline)
            {
                return -1;
            }
            SwitchToThread();
        }
    }
#endif /* PLATFORM_ */

    /* ファイルが未開の場合は書き込みをスキップする */
#if defined(PLATFORM_LINUX)
    if (handle->fd == -1)
    {
        pthread_mutex_unlock(&handle->mutex);
        return -1;
    }
#elif defined(PLATFORM_WINDOWS)
    if (handle->fh == INVALID_HANDLE_VALUE)
    {
        LeaveCriticalSection(&handle->cs);
        return -1;
    }
#endif /* PLATFORM_ */

    /* ファイルへ書き込む (FILE_FLAG_WRITE_THROUGH / O_DSYNC により自動フラッシュ) */
    ret = -1;
#if defined(PLATFORM_LINUX)
    {
        ssize_t written = write(handle->fd, buf, (size_t)len);
        if (written == (ssize_t)len)
        {
            ret = 0;
        }
    }
#elif defined(PLATFORM_WINDOWS)
    {
        DWORD written = 0;
        if (WriteFile(handle->fh, buf, (DWORD)len, &written, NULL) && (DWORD)written == (DWORD)len)
        {
            ret = 0;
        }
    }
#endif /* PLATFORM_ */

    /* 書き込み成功時: サイズを追跡しローテーション閾値を確認する */
    if (ret == 0)
    {
        /* size_t のオーバーフローを防ぐ (実用上は発生しないが防御的に扱う) */
        if (handle->current_bytes <= (size_t)-1 - (size_t)len)
        {
            handle->current_bytes += (size_t)len;
        }

        if (handle->current_bytes >= handle->max_bytes)
        {
            rotate_file(handle);
        }
    }

    /* ロック解放 */
#if defined(PLATFORM_LINUX)
    pthread_mutex_unlock(&handle->mutex);
#elif defined(PLATFORM_WINDOWS)
    LeaveCriticalSection(&handle->cs);
#endif /* PLATFORM_ */

    return ret;
}

/* doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT void COM_UTIL_API trace_file_sink_destroy(trace_file_sink_t *handle)
{
    if (handle == NULL)
    {
        return;
    }

    close_file(handle);

#if defined(PLATFORM_LINUX)
    if (handle->mutex_initialized)
    {
        pthread_mutex_destroy(&handle->mutex);
        handle->mutex_initialized = 0;
    }
#elif defined(PLATFORM_WINDOWS)
    if (handle->cs_initialized)
    {
        DeleteCriticalSection(&handle->cs);
        handle->cs_initialized = 0;
    }
#endif /* PLATFORM_ */

    free(handle->path);
    free(handle);
}

/* doxygen コメントは、ヘッダに記載 */
void trace_file_sink_destroy_on_unload(trace_file_sink_t *handle)
{
    if (handle == NULL)
    {
        return;
    }

    close_file(handle);

#if defined(PLATFORM_LINUX)
    if (handle->mutex_initialized)
    {
        pthread_mutex_destroy(&handle->mutex);
    }
#elif defined(PLATFORM_WINDOWS)
    if (handle->cs_initialized)
    {
        DeleteCriticalSection(&handle->cs);
    }
#endif /* PLATFORM_ */

    free(handle->path);
    free(handle);
}
