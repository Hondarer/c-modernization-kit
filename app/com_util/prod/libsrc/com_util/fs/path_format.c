#include <com_util/fs/path_format.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(PLATFORM_WINDOWS)
    #include <share.h>
#endif /* PLATFORM_WINDOWS */

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
COM_UTIL_EXPORT FILE *COM_UTIL_API vfopen_fmt(const char *modes, int *errno_out, const char *format, va_list args)
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
#if defined(PLATFORM_LINUX)
    errno = 0;
    FILE *fp = fopen(filename, modes);
    if (fp == NULL && errno_out != NULL)
    {
        *errno_out = errno;
    }
    return fp;
#elif defined(PLATFORM_WINDOWS)
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
#endif /* PLATFORM_ */
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT FILE *COM_UTIL_API fopen_fmt(const char *modes, int *errno_out, const char *format, ...)
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
COM_UTIL_EXPORT int COM_UTIL_API vstat_fmt(util_file_stat_t *buf, const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

    /* 引数チェック */
    if (buf == NULL)
    {
        return -1;
    }

    /* stat を呼び出してファイル情報を取得 */
#if defined(PLATFORM_LINUX)
    return stat(filename, buf);
#elif defined(PLATFORM_WINDOWS)
    /* Windows では _stat64 を使用（util_file_stat_t は struct _stat64 の typedef） */
    return _stat64(filename, buf);
#endif /* PLATFORM_ */
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API stat_fmt(util_file_stat_t *buf, const char *format, ...)
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
COM_UTIL_EXPORT int COM_UTIL_API vremove_fmt(const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

    return remove(filename);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API remove_fmt(const char *format, ...)
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
COM_UTIL_EXPORT int COM_UTIL_API vopen_fmt(int flags, int mode, const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#if defined(PLATFORM_LINUX)
    return open(filename, flags, mode);
#elif defined(PLATFORM_WINDOWS)
    int fd = -1;
    errno_t open_result;

    open_result = _sopen_s(&fd, filename, flags, _SH_DENYNO, mode);
    if (open_result != 0)
    {
        return -1;
    }

    return fd;
#endif /* PLATFORM_ */
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API open_fmt(int flags, int mode, const char *format, ...)
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
COM_UTIL_EXPORT int COM_UTIL_API vaccess_fmt(int mode, const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#if defined(PLATFORM_LINUX)
    return access(filename, mode);
#elif defined(PLATFORM_WINDOWS)
    return _access(filename, mode);
#endif /* PLATFORM_ */
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API access_fmt(int mode, const char *format, ...)
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
COM_UTIL_EXPORT int COM_UTIL_API vmkdir_fmt(const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#if defined(PLATFORM_LINUX)
    return mkdir(filename, 0755);
#elif defined(PLATFORM_WINDOWS)
    return _mkdir(filename);
#endif /* PLATFORM_ */
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API mkdir_fmt(const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = vmkdir_fmt(format, args);
    va_end(args);

    return result;
}
