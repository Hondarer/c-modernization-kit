#include <com_util/fs/path_format.h>
#include <com_util/fs/file_io.h>
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

/* ===== com_util_vfopen_fmt / com_util_fopen_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT FILE *COM_UTIL_API com_util_vfopen_fmt(const char *modes, int *errno_out, const char *format, va_list args)
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

    /* パスを com_util_fopen 経由で開く（UTF-8 対応・プラットフォーム差異吸収） */
    return com_util_fopen(filename, modes, errno_out);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT FILE *COM_UTIL_API com_util_fopen_fmt(const char *modes, int *errno_out, const char *format, ...)
{
    FILE *result;
    va_list args;

    va_start(args, format);
    result = com_util_vfopen_fmt(modes, errno_out, format, args);
    va_end(args);

    return result;
}

/* ===== com_util_vstat_fmt / com_util_stat_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_vstat_fmt(util_file_stat_t *buf, const char *format, va_list args)
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
COM_UTIL_EXPORT int COM_UTIL_API com_util_stat_fmt(util_file_stat_t *buf, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = com_util_vstat_fmt(buf, format, args);
    va_end(args);

    return result;
}

/* ===== com_util_vremove_fmt / com_util_remove_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_vremove_fmt(const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

    return com_util_remove(filename);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_remove_fmt(const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = com_util_vremove_fmt(format, args);
    va_end(args);

    return result;
}

/* ===== com_util_vopen_fmt / com_util_open_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_vopen_fmt(int flags, int mode, const char *format, va_list args)
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
COM_UTIL_EXPORT int COM_UTIL_API com_util_open_fmt(int flags, int mode, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = com_util_vopen_fmt(flags, mode, format, args);
    va_end(args);

    return result;
}

/* ===== com_util_vaccess_fmt / com_util_access_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_vaccess_fmt(int mode, const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#if defined(PLATFORM_LINUX)
    return access(filename, mode);
#elif defined(PLATFORM_WINDOWS)
    return _access(filename, mode);
#endif /* PLATFORM_ */
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_access_fmt(int mode, const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = com_util_vaccess_fmt(mode, format, args);
    va_end(args);

    return result;
}

/* ===== com_util_vmkdir_fmt / com_util_mkdir_fmt ===== */

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_vmkdir_fmt(const char *format, va_list args)
{
    FMTIO_FORMAT_FILENAME(format, args, -1)

#if defined(PLATFORM_LINUX)
    return mkdir(filename, 0755);
#elif defined(PLATFORM_WINDOWS)
    return _mkdir(filename);
#endif /* PLATFORM_ */
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_mkdir_fmt(const char *format, ...)
{
    int result;
    va_list args;

    va_start(args, format);
    result = com_util_vmkdir_fmt(format, args);
    va_end(args);

    return result;
}
