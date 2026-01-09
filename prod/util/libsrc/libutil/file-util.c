#include <file-util.h>
#include <stdlib.h>
#include <string.h>

/* OS 固有のパス最大長を定義 */
#ifndef _WIN32
#include <limits.h>
#define FILE_PATH_MAX PATH_MAX
#else /* _WIN32 */
#include <windows.h>
#define FILE_PATH_MAX MAX_PATH
#endif /* _WIN32 */

/**
 *  @brief          printf 形式でファイル名を指定する fopen ラッパー関数
 *
 *  この関数は、printf と同じ形式でファイル名を指定してファイルを開きます。
 *  内部で vsnprintf を使用してファイル名をフォーマットし、fopen を呼び出します。
 *
 *  @param[in]      modes ファイルオープンモード ("r", "w", "a", "rb", "wb", etc.)
 *  @param[in]      format ファイル名のフォーマット文字列 (printf 形式)
 *  @param[in]      ... フォーマット文字列の可変引数
 *  @return         成功時は FILE ポインタ、失敗時は NULL
 *
 *  @note           ファイル名の最大長は OS の規定値です (Windows: MAX_PATH=260, Linux: PATH_MAX=通常4096)
 *  @note           使用例: FILE *fp = fopen_printf("r", "data_%d.txt", 123);
 */
FILE_UTIL_API FILE *WINAPI fopen_printf(const char *modes, const char *format, ...)
{
    va_list args;
    char filename[FILE_PATH_MAX] = {0};
    int written;

    /* 引数チェック */
    if (modes == NULL || format == NULL)
    {
        return NULL;
    }

    /* 可変引数を使用してファイル名をフォーマット */
    va_start(args, format);
    written = vsnprintf(filename, sizeof(filename), format, args);
    va_end(args);

    /* バッファオーバーフローチェック */
    if (written >= (int)sizeof(filename))
    {
        return NULL;
    }

    /* fopen を呼び出してファイルを開く */
#ifndef _WIN32
    return fopen(filename, modes);
#else
    FILE *fp = NULL;
    errno_t err = fopen_s(&fp, filename, modes);
    if (err != 0)
    {
        return NULL;
    }
    return fp;
#endif
}
