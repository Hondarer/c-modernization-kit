#include <fmtio-util.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* ===== 内部ヘルパーマクロ ===== */

/**
 *  ファイル名フォーマットの共通処理。
 *  va_list から FILE_PATH_MAX バッファにファイル名を展開し、
 *  失敗時は指定された値を返す。
 *  成功時は filename[] に結果が格納される。
 */
#define FMTIO_FORMAT_FILENAME(format, args, fail_return) \
    char filename[FILE_PATH_MAX] = {0};                  \
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

/* ===== vfopenf / fopenf ===== */

/* Doxygen コメントは、ヘッダに記載 */
FILE *FMTIO_UTIL_API vfopenf(const char *modes, int *errno_out, const char *format, va_list args)
{
    char filename[FILE_PATH_MAX] = {0};
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
FILE *FMTIO_UTIL_API fopenf(const char *modes, int *errno_out, const char *format, ...)
{
    FILE *result;
    va_list args;

    va_start(args, format);
    result = vfopenf(modes, errno_out, format, args);
    va_end(args);

    return result;
}

/* ===== vstatf / statf ===== */

/* Doxygen コメントは、ヘッダに記載 */
int FMTIO_UTIL_API vstatf(file_stat_t *buf, const char *format, va_list args)
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
    /* Windows では _stat64 を使用（file_stat_t は struct _stat64 の typedef） */
    return _stat64(filename, buf);
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
int FMTIO_UTIL_API statf(file_stat_t *buf, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vstatf(buf, format, args);
    va_end(args);

    return result;
}

/* ===== vremovef / removef ===== */

/* Doxygen コメントは、ヘッダに記載 */
int FMTIO_UTIL_API vremovef(const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

    return remove(filename);
}

/* Doxygen コメントは、ヘッダに記載 */
int FMTIO_UTIL_API removef(const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vremovef(format, args);
    va_end(args);

    return result;
}

/* ===== vopenf / openf ===== */

/* Doxygen コメントは、ヘッダに記載 */
int FMTIO_UTIL_API vopenf(int flags, int mode, const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#ifndef _WIN32
    return open(filename, flags, mode);
#else /* _WIN32 */
    return _open(filename, flags, mode);
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
int FMTIO_UTIL_API openf(int flags, int mode, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vopenf(flags, mode, format, args);
    va_end(args);

    return result;
}

/* ===== vaccessf / accessf ===== */

/* Doxygen コメントは、ヘッダに記載 */
int FMTIO_UTIL_API vaccessf(int mode, const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#ifndef _WIN32
    return access(filename, mode);
#else /* _WIN32 */
    return _access(filename, mode);
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
int FMTIO_UTIL_API accessf(int mode, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vaccessf(mode, format, args);
    va_end(args);

    return result;
}

/* ===== vmkdirf / mkdirf ===== */

/* Doxygen コメントは、ヘッダに記載 */
int FMTIO_UTIL_API vmkdirf(const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#ifndef _WIN32
    return mkdir(filename, 0755);
#else /* _WIN32 */
    return _mkdir(filename);
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
int FMTIO_UTIL_API mkdirf(const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vmkdirf(format, args);
    va_end(args);

    return result;
}
