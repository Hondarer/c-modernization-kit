#include <log-util.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/**
 *  @brief  ログプロバイダハンドル構造体 (内部定義)。
 */
struct log_provider
{
#ifdef _WIN32
    /** ETW プロバイダハンドル (Windows)。 */
    etw_provider_t *etw;
#else /* !_WIN32 */
    /** syslog プロバイダハンドル (Linux)。 */
    syslog_provider_t *syslog_handle;
#endif /* _WIN32 */
};

#ifdef _WIN32

/**
 *  @brief  enum log_level を ETW Level (1-5) に変換する。
 */
static int to_etw_level(enum log_level lv)
{
    switch (lv)
    {
    case LOG_LV_CRITICAL: return 1;
    case LOG_LV_ERROR:    return 2;
    case LOG_LV_WARNING:  return 3;
    case LOG_LV_INFO:     return 4;
    default:              return 5;
    }
}

#else /* !_WIN32 */

#include <syslog.h>

/**
 *  @brief  enum log_level を syslog severity に変換する。
 */
static int to_syslog_level(enum log_level lv)
{
    switch (lv)
    {
    case LOG_LV_CRITICAL: return LOG_CRIT;
    case LOG_LV_ERROR:    return LOG_ERR;
    case LOG_LV_WARNING:  return LOG_WARNING;
    case LOG_LV_INFO:     return LOG_INFO;
    default:              return LOG_DEBUG;
    }
}

#endif /* _WIN32 */

#ifndef _WIN32
#include <unistd.h>

/** プロセス名取得失敗時のフォールバック名。 */
#define FALLBACK_NAME "unknown"

/**
 *  @brief  自プロセスの実行ファイル名 (ベースネーム) を取得する。
 *  @param[out]  buf       パス格納バッファ。
 *  @param[in]   buf_size  バッファサイズ。
 *  @return      ベースネームへのポインタ (buf 内または FALLBACK_NAME)。
 */
static const char *get_process_basename(char *buf, size_t buf_size)
{
    ssize_t len;
    const char *slash;

    len = readlink("/proc/self/exe", buf, buf_size - 1);
    if (len <= 0)
    {
        return FALLBACK_NAME;
    }
    buf[len] = '\0';

    slash = strrchr(buf, '/');
    return slash ? slash + 1 : buf;
}
#endif /* !_WIN32 */

#ifndef _WIN32
log_provider_t *LOG_UTIL_API
    log_init_linux(const char *name)
{
    log_provider_t *handle;
    syslog_provider_t *sp;
    char path_buf[256];
    const char *effective_name;

    if (name != NULL)
    {
        effective_name = name;
    }
    else
    {
        effective_name = get_process_basename(path_buf, sizeof(path_buf));
    }

    sp = syslog_provider_init(effective_name, LOG_USER);
    if (sp == NULL)
    {
        return NULL;
    }

    handle = (log_provider_t *)malloc(sizeof(log_provider_t));
    if (handle == NULL)
    {
        syslog_provider_dispose(sp);
        return NULL;
    }

    handle->syslog_handle = sp;
    return handle;
}
#else /* _WIN32 */
log_provider_t *LOG_UTIL_API
    log_init_windows(etw_provider_ref_t provider_ref)
{
    log_provider_t *handle;
    etw_provider_t *ep;

    ep = etw_provider_init(provider_ref);
    if (ep == NULL)
    {
        return NULL;
    }

    handle = (log_provider_t *)malloc(sizeof(log_provider_t));
    if (handle == NULL)
    {
        etw_provider_dispose(ep);
        return NULL;
    }

    handle->etw = ep;
    return handle;
}
#endif /* _WIN32 */

/** 切り詰め後の本文最大バイト数 (null 終端を除く)。 */
#define MAX_BODY (LOG_MESSAGE_MAX_BYTES - 1)

/**
 *  @brief  UTF-8 安全な切り詰め位置を返す。
 *  @details  pos バイト目以前で、UTF-8 マルチバイトシーケンスの途中を
 *            避けた安全な切断位置を返す。
 */
static size_t utf8_safe_truncate(const char *s, size_t pos)
{
    /* pos が文字列長以下なら調整不要 */
    while (pos > 0 && ((unsigned char)s[pos] & 0xC0) == 0x80)
    {
        /* 継続バイト (10xxxxxx) を跨がないよう先頭バイトまで戻る */
        pos--;
    }
    return pos;
}

int LOG_UTIL_API
    log_write(log_provider_t *handle, enum log_level level, const char *message)
{
    const char *msg;
    char buf[LOG_MESSAGE_MAX_BYTES];
    size_t len;

    if (handle == NULL || message == NULL)
    {
        return 0;
    }

    msg = message;
    len = strlen(message);

    if (len > MAX_BODY)
    {
        size_t safe_len = utf8_safe_truncate(message, MAX_BODY);
        memcpy(buf, message, safe_len);
        buf[safe_len] = '\0';
        msg = buf;
    }

#ifdef _WIN32
    return etw_provider_write(handle->etw, to_etw_level(level), msg);
#else /* !_WIN32 */
    return syslog_provider_write(handle->syslog_handle, to_syslog_level(level), msg);
#endif /* _WIN32 */
}

/**
 *  @brief  下層プロバイダに文字列を書き込む (内部ヘルパー)。
 */
static int write_to_provider(log_provider_t *handle, enum log_level level, const char *msg)
{
#ifdef _WIN32
    return etw_provider_write(handle->etw, to_etw_level(level), msg);
#else /* !_WIN32 */
    return syslog_provider_write(handle->syslog_handle, to_syslog_level(level), msg);
#endif /* _WIN32 */
}

int LOG_UTIL_API
    log_writef(log_provider_t *handle, enum log_level level, const char *format, ...)
{
    va_list args;
    char buf[LOG_MESSAGE_MAX_BYTES];

    if (handle == NULL || format == NULL)
    {
        return 0;
    }

    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    return write_to_provider(handle, level, buf);
}

/** HEX 変換用テーブル。 */
static const char hex_chars[] = "0123456789ABCDEF";

/** "..." サフィックスの長さ。 */
#define ELLIPSIS_LEN 3

/**
 *  @brief  HEX ダンプ出力の内部実装。
 *  @details  ラベルは呼び出し元で事前にフォーマット済みの文字列を受け取る。
 *            データが収まらない場合は切り詰めて "..." を付与する。
 */
static int hex_write_impl(log_provider_t *handle, enum log_level level,
                           const void *data, size_t size, const char *label)
{
    char buf[LOG_MESSAGE_MAX_BYTES];
    const unsigned char *bytes = (const unsigned char *)data;
    size_t pos = 0;
    size_t remaining;
    size_t max_data_bytes;
    int truncated = 0;
    size_t i;

    if (handle == NULL || data == NULL || size == 0)
    {
        return 0;
    }

    /* ラベル付与 */
    if (label != NULL && label[0] != '\0')
    {
        size_t lbl_len = strlen(label);
        /* ラベル + ": " がバッファに収まるか検査 */
        if (lbl_len + 2 >= MAX_BODY)
        {
            /* ラベルだけで上限超過 — ラベルのみ出力 */
            size_t copy_len = lbl_len < MAX_BODY ? lbl_len : MAX_BODY;
            memcpy(buf, label, copy_len);
            buf[copy_len] = '\0';
            return write_to_provider(handle, level, buf);
        }
        memcpy(buf, label, lbl_len);
        buf[lbl_len] = ':';
        buf[lbl_len + 1] = ' ';
        pos = lbl_len + 2;
    }

    /* HEX データ出力可能な残りバイト数 */
    remaining = MAX_BODY - pos;

    /* 全データが収まるか判定:
       N バイト → 3*N - 1 文字 (最終バイトはスペースなし) */
    max_data_bytes = (remaining + 1) / 3;

    if (size > max_data_bytes)
    {
        /* 切り詰めが必要: "..." (3文字) の分を確保
           各バイト "XX " (3文字) + "..." (3文字) → M*3 + 3 ≤ remaining
           M ≤ (remaining - 3) / 3 */
        truncated = 1;
        if (remaining < ELLIPSIS_LEN)
        {
            /* "..." すら入らない */
            buf[pos] = '\0';
            return write_to_provider(handle, level, buf);
        }
        max_data_bytes = (remaining - ELLIPSIS_LEN) / 3;
        if (max_data_bytes == 0)
        {
            /* HEX 1 バイトも入らない — "..." のみ */
            memcpy(buf + pos, "...", ELLIPSIS_LEN);
            pos += ELLIPSIS_LEN;
            buf[pos] = '\0';
            return write_to_provider(handle, level, buf);
        }
        size = max_data_bytes;
    }

    /* HEX 文字列を構築 */
    for (i = 0; i < size; i++)
    {
        if (i > 0)
        {
            buf[pos++] = ' ';
        }
        buf[pos++] = hex_chars[(bytes[i] >> 4) & 0x0F];
        buf[pos++] = hex_chars[bytes[i] & 0x0F];
    }

    /* 切り詰め時は "..." を付与 */
    if (truncated)
    {
        buf[pos++] = ' ';
        buf[pos++] = '.';
        buf[pos++] = '.';
        buf[pos++] = '.';
    }
    buf[pos] = '\0';

    return write_to_provider(handle, level, buf);
}

int LOG_UTIL_API
    log_hex_write(log_provider_t *handle, enum log_level level,
                  const void *data, size_t size, const char *message)
{
    return hex_write_impl(handle, level, data, size, message);
}

int LOG_UTIL_API
    log_hex_writef(log_provider_t *handle, enum log_level level,
                   const void *data, size_t size, const char *format, ...)
{
    char label[LOG_MESSAGE_MAX_BYTES];

    if (handle == NULL || data == NULL || size == 0)
    {
        return 0;
    }

    if (format != NULL)
    {
        va_list args;
        va_start(args, format);
        vsnprintf(label, sizeof(label), format, args);
        va_end(args);
        return hex_write_impl(handle, level, data, size, label);
    }

    return hex_write_impl(handle, level, data, size, NULL);
}

void LOG_UTIL_API
    log_dispose(log_provider_t *handle)
{
    if (handle == NULL)
    {
        return;
    }

#ifdef _WIN32
    etw_provider_dispose(handle->etw);
#else /* !_WIN32 */
    syslog_provider_dispose(handle->syslog_handle);
#endif /* _WIN32 */

    free(handle);
}
