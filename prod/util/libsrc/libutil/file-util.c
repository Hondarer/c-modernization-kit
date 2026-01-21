#include <file-util.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* OS 固有のパス最大長を定義 */
#ifndef _WIN32
#include <limits.h>
#include <sys/stat.h>
#define FILE_PATH_MAX PATH_MAX
#else /* _WIN32 */
#include <windows.h>
#include <sys/stat.h>
#define FILE_PATH_MAX MAX_PATH
#endif /* _WIN32 */

/* Doxygen コメントは、ヘッダに記載 */
FILE_UTIL_API FILE *WINAPI fopen_printf(const char *modes, int *errno_out, const char *format, ...)
{
    va_list args;
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

    /* 可変引数を使用してファイル名をフォーマット */
    va_start(args, format);
    written = vsnprintf(filename, sizeof(filename), format, args);
    va_end(args);

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
#else
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
#endif
}

/* Doxygen コメントは、ヘッダに記載 */
FILE_UTIL_API int WINAPI stat_printf(file_stat_t *buf, const char *format, ...)
{
    va_list args;
    char filename[FILE_PATH_MAX] = {0};
    int written;

    /* 引数チェック */
    if (buf == NULL || format == NULL)
    {
        return -1;
    }

    /* 可変引数を使用してファイル名をフォーマット */
    va_start(args, format);
    written = vsnprintf(filename, sizeof(filename), format, args);
    va_end(args);

    /* フォーマット失敗チェック */
    if (written < 0)
    {
        return -1;
    }

    /* バッファオーバーフローチェック */
    if (written >= (int)sizeof(filename))
    {
        return -1;
    }

    /* stat を呼び出してファイル情報を取得 */
#ifndef _WIN32
    return stat(filename, buf);
#else
    /* Windows では _stat64 を使用（file_stat_t は struct _stat64 の typedef） */
    return _stat64(filename, buf);
#endif
}
