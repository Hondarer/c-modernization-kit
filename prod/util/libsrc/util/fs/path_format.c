#include <util/fs/path_format.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* ===== 内部ヘルパーマクロ ===== */

/**
 *  ファイル名フォーマットの共通処理。
 *  va_list から PATH_FORMAT_PATH_MAX バッファにファイル名を展開し、
 *  失敗時は指定された値を返す。
 *  成功時は filename[] に結果が格納される。
 */
#define FMTIO_FORMAT_FILENAME(format, args, fail_return) \
    char filename[PATH_FORMAT_PATH_MAX] = {0};                  \
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

/* ===== pathf_vfopen / pathf_fopen ===== */

/* Doxygen コメントは、ヘッダに記載 */
FILE *PATH_FORMAT_API pathf_vfopen(const char *modes, int *errno_out, const char *format, va_list args)
{
    char filename[PATH_FORMAT_PATH_MAX] = {0};
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
FILE *PATH_FORMAT_API pathf_fopen(const char *modes, int *errno_out, const char *format, ...)
{
    FILE *result;
    va_list args;

    va_start(args, format);
    result = pathf_vfopen(modes, errno_out, format, args);
    va_end(args);

    return result;
}

/* ===== pathf_vstat / pathf_stat ===== */

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API pathf_vstat(util_file_stat_t *buf, const char *format, va_list args)
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
int PATH_FORMAT_API pathf_stat(util_file_stat_t *buf, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = pathf_vstat(buf, format, args);
    va_end(args);

    return result;
}

/* ===== pathf_vremove / pathf_remove ===== */

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API pathf_vremove(const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

    return remove(filename);
}

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API pathf_remove(const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = pathf_vremove(format, args);
    va_end(args);

    return result;
}

/* ===== pathf_vopen / pathf_open ===== */

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API pathf_vopen(int flags, int mode, const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#ifndef _WIN32
    return open(filename, flags, mode);
#else /* _WIN32 */
    return _open(filename, flags, mode);
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API pathf_open(int flags, int mode, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = pathf_vopen(flags, mode, format, args);
    va_end(args);

    return result;
}

/* ===== pathf_vaccess / pathf_access ===== */

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API pathf_vaccess(int mode, const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#ifndef _WIN32
    return access(filename, mode);
#else /* _WIN32 */
    return _access(filename, mode);
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API pathf_access(int mode, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = pathf_vaccess(mode, format, args);
    va_end(args);

    return result;
}

/* ===== pathf_vmkdir / pathf_mkdir ===== */

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API pathf_vmkdir(const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#ifndef _WIN32
    return mkdir(filename, 0755);
#else /* _WIN32 */
    return _mkdir(filename);
#endif /* _WIN32 */
}

/* Doxygen コメントは、ヘッダに記載 */
int PATH_FORMAT_API pathf_mkdir(const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = pathf_vmkdir(format, args);
    va_end(args);

    return result;
}
