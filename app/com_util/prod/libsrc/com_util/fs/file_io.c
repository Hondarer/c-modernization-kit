#include <com_util/fs/file_io.h>
#include <com_util/fs/path_max.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#if defined(PLATFORM_LINUX)
    #include <stdio.h>
#elif defined(PLATFORM_WINDOWS)
    #include <com_util/base/windows_sdk.h>
    #include <wchar.h>
    #include <stdio.h>
    #include <stdlib.h>
#endif /* PLATFORM_ */

/* ===== Windows 内部ヘルパー ===== */

#if defined(PLATFORM_WINDOWS)

/**
 *  UTF-8 文字列を wchar_t に変換してバッファへ格納する。
 *  変換後の文字数（NULL 含む）が PLATFORM_PATH_MAX を超える場合は -1 を返す。
 */
static int utf8_to_wpath(wchar_t *wbuf, const char *utf8_path)
{
    int n;

    if (utf8_path == NULL || wbuf == NULL)
    {
        return -1;
    }

    n = MultiByteToWideChar(CP_UTF8, 0, utf8_path, -1, wbuf, PLATFORM_PATH_MAX);
    return (n <= 0) ? -1 : n;
}

#endif /* PLATFORM_WINDOWS */

/* ===== ファイルパス系 ===== */

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT FILE *COM_UTIL_API com_util_fopen(const char *path,
                                                   const char *modes,
                                                   int        *errno_out)
{
    if (path == NULL || modes == NULL)
    {
        if (errno_out != NULL)
        {
            *errno_out = EINVAL;
        }
        return NULL;
    }

#if defined(PLATFORM_LINUX)
    {
        FILE *fp;
        errno = 0;
        fp    = fopen(path, modes);
        if (fp == NULL && errno_out != NULL)
        {
            *errno_out = errno;
        }
        return fp;
    }
#elif defined(PLATFORM_WINDOWS)
    {
        wchar_t  wpath[PLATFORM_PATH_MAX];
        wchar_t  wmodes[64];
        FILE    *fp  = NULL;
        errno_t  err;

        if (utf8_to_wpath(wpath, path) < 0)
        {
            if (errno_out != NULL)
            {
                *errno_out = ENAMETOOLONG;
            }
            return NULL;
        }

        if (mbstowcs(wmodes, modes, sizeof(wmodes) / sizeof(wmodes[0])) == (size_t)-1)
        {
            if (errno_out != NULL)
            {
                *errno_out = EINVAL;
            }
            return NULL;
        }

        err = _wfopen_s(&fp, wpath, wmodes);
        if (err != 0)
        {
            if (errno_out != NULL)
            {
                *errno_out = (int)err;
            }
            return NULL;
        }

        return fp;
    }
#endif /* PLATFORM_ */
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_remove(const char *path)
{
    if (path == NULL)
    {
        return -1;
    }

#if defined(PLATFORM_LINUX)
    return remove(path);
#elif defined(PLATFORM_WINDOWS)
    {
        wchar_t wpath[PLATFORM_PATH_MAX];

        if (utf8_to_wpath(wpath, path) < 0)
        {
            return -1;
        }

        return _wremove(wpath);
    }
#endif /* PLATFORM_ */
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_rename(const char *oldpath,
                                                  const char *newpath)
{
    if (oldpath == NULL || newpath == NULL)
    {
        return -1;
    }

#if defined(PLATFORM_LINUX)
    return rename(oldpath, newpath);
#elif defined(PLATFORM_WINDOWS)
    {
        wchar_t woldpath[PLATFORM_PATH_MAX];
        wchar_t wnewpath[PLATFORM_PATH_MAX];

        if (utf8_to_wpath(woldpath, oldpath) < 0)
        {
            return -1;
        }

        if (utf8_to_wpath(wnewpath, newpath) < 0)
        {
            return -1;
        }

        /* POSIX rename() と同じく移動先が存在しても上書きする */
        if (!MoveFileExW(woldpath, wnewpath, MOVEFILE_REPLACE_EXISTING))
        {
            return -1;
        }
        return 0;
    }
#endif /* PLATFORM_ */
}

/* ===== ストリーム操作系 ===== */

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_fclose(FILE *stream)
{
    return fclose(stream);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT size_t COM_UTIL_API com_util_fread(void  *ptr,
                                                    size_t size,
                                                    size_t count,
                                                    FILE  *stream)
{
    return fread(ptr, size, count, stream);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT size_t COM_UTIL_API com_util_fwrite(const void *ptr,
                                                     size_t      size,
                                                     size_t      count,
                                                     FILE       *stream)
{
    return fwrite(ptr, size, count, stream);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT char *COM_UTIL_API com_util_fgets(char *buf,
                                                   int   size,
                                                   FILE *stream)
{
    return fgets(buf, size, stream);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_fputs(const char *str,
                                                 FILE       *stream)
{
    return fputs(str, stream);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_fprintf(FILE       *stream,
                                                   const char *format,
                                                   ...)
{
    int     result;
    va_list args;

    va_start(args, format);
    result = com_util_vfprintf(stream, format, args);
    va_end(args);

    return result;
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_vfprintf(FILE       *stream,
                                                    const char *format,
                                                    va_list     args)
{
#if defined(PLATFORM_WINDOWS)
    return vfprintf_s(stream, format, args);
#else
    return vfprintf(stream, format, args);
#endif /* PLATFORM_ */
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_fflush(FILE *stream)
{
    return fflush(stream);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_feof(FILE *stream)
{
    return feof(stream);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_ferror(FILE *stream)
{
    return ferror(stream);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT void COM_UTIL_API com_util_clearerr(FILE *stream)
{
    clearerr(stream);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT void COM_UTIL_API com_util_rewind(FILE *stream)
{
    rewind(stream);
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_fseek(FILE   *stream,
                                                 int64_t offset,
                                                 int     whence)
{
#if defined(PLATFORM_LINUX)
    return fseeko(stream, (off_t)offset, whence);
#elif defined(PLATFORM_WINDOWS)
    return _fseeki64(stream, (__int64)offset, whence);
#endif /* PLATFORM_ */
}

/* Doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT int64_t COM_UTIL_API com_util_ftell(FILE *stream)
{
#if defined(PLATFORM_LINUX)
    return (int64_t)ftello(stream);
#elif defined(PLATFORM_WINDOWS)
    return (int64_t)_ftelli64(stream);
#endif /* PLATFORM_ */
}
