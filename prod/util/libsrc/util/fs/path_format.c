#include <util/fs/path_format.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* ===== 内部ヘルパーマクロ ===== */

/**
 *  ファイル名フォーマットの共通処理。
 *  va_list から PLATFORM_PATH_MAX バッファにファイル名を展開し、
 *  失敗時は指定された値を返す。
 *  成功時は filename[] に結果が格納される。
 */
#define FMTIO_FORMAT_FILENAME(format, args, fail_return) \
    char filename[PLATFORM_PATH_MAX] = {0};                  \
    int written;                                         \
    if (format == NULL)                                  \
    {                                                    \
        return (fail_return);                            \
    }                                                    \
    written = vsnprintf(filename, sizeof(filename), format, args); \
    if (written < 0)                                     \
    {                                                    \
        return (fail_return);                            \
    }                                                    \
    if (written >= (int)sizeof(filename))                 \
    {                                                    \
        return (fail_return);                            \
    }

/* ===== vfopen_fmt / fopen_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
FILE *PATH_FORMAT_API vfopen_fmt(const char *modes, int *errno_out, const char *format, va_list args)
{
    char filename[PLATFORM_PATH_MAX] = {0};
    int written;

    /* 引数チェック */
    if (modes == NULL || format == NULL)
    {
        if (errno_out != NULL)
        {
            *errno_out = EINVAL;
        }
        return NULL;
    }

    /* ファイル名をフォーマット */
    written = vsnprintf(filename, sizeof(filename), format, args);

    /* フォーマット失敗チェック */
    if (written < 0)
    {
        if (errno_out != NULL)
        {
            *errno_out = EINVAL;
        }
        return NULL;
    }

    /* バッファオーバーフローチェック */
    if (written >= (int)sizeof(filename))
    {
        if (errno_out != NULL)
        {
            *errno_out = ENAMETOOLONG;
        }
        return NULL;
    }

    /* fopen を呼び出してファイルを開く */
#ifndef _WIN32
    errno = 0;
    FILE *fp = fopen(filename, modes);
    if (fp == NULL && errno_out != NULL)
    {
        *errno_out = errno;
    }
    return fp;
#else /* _WIN32 */
    FILE *fp = NULL;
    errno_t err = fopen_s(&fp, filename, modes);
    if (err != 0)
    {
        if (errno_out != NULL)
        {
            *errno_out = err;
        }
        return NULL;
    }
    return fp;
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
FILE *PATH_FORMAT_API fopen_fmt(const char *modes, int *errno_out, const char *format, ...)
{
    FILE *result;
    va_list args;

    va_start(args, format);
    result = vfopen_fmt(modes, errno_out, format, args);
    va_end(args);

    return result;
}

/* ===== vstat_fmt / stat_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API vstat_fmt(util_file_stat_t *buf, const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

    /* 引数チェック */
    if (buf == NULL)
    {
        return -1;
    }

    /* stat を呼び出してファイル情報を取得 */
#ifndef _WIN32
    return stat(filename, buf);
#else /* _WIN32 */
    /* Windows では _stat64 を使用（util_file_stat_t は struct _stat64 の typedef） */
    return _stat64(filename, buf);
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API stat_fmt(util_file_stat_t *buf, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vstat_fmt(buf, format, args);
    va_end(args);

    return result;
}

/* ===== vremove_fmt / remove_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API vremove_fmt(const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

    return remove(filename);
}

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API remove_fmt(const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vremove_fmt(format, args);
    va_end(args);

    return result;
}

/* ===== vopen_fmt / open_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API vopen_fmt(int flags, int mode, const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#ifndef _WIN32
    return open(filename, flags, mode);
#else /* _WIN32 */
    return _open(filename, flags, mode);
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API open_fmt(int flags, int mode, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vopen_fmt(flags, mode, format, args);
    va_end(args);

    return result;
}

/* ===== vaccess_fmt / access_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API vaccess_fmt(int mode, const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#ifndef _WIN32
    return access(filename, mode);
#else /* _WIN32 */
    return _access(filename, mode);
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API access_fmt(int mode, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vaccess_fmt(mode, format, args);
    va_end(args);

    return result;
}

/* ===== vmkdir_fmt / mkdir_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API vmkdir_fmt(const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#ifndef _WIN32
    return mkdir(filename, 0755);
#else /* _WIN32 */
    return _mkdir(filename);
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API mkdir_fmt(const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vmkdir_fmt(format, args);
    va_end(args);

    return result;
}
