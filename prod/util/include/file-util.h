#ifndef _FILE_UTIL_H
#define _FILE_UTIL_H

#include <stdio.h>
#include <stdarg.h>

/* DLL エクスポート/インポート定義 */
#ifndef _WIN32
#define FILE_UTIL_API
#define WINAPI
#else /* _WIN32 */
#ifndef __INTELLISENSE__
#ifdef FILE_UTIL_EXPORTS
#define FILE_UTIL_API __declspec(dllexport)
#else /* FILE_UTIL_EXPORTS */
#define FILE_UTIL_API __declspec(dllimport)
#endif /* FILE_UTIL_EXPORTS */
#else  /* __INTELLISENSE__ */
#define FILE_UTIL_API
#endif /* __INTELLISENSE__ */
#ifndef WINAPI
#define WINAPI __stdcall
#endif /* WINAPI */
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C"
{
#endif

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
        FILE_UTIL_API FILE WINAPI *fopen_printf(const char *modes, const char *format, ...)
#ifdef __GNUC__
            __attribute__((format(printf, 2, 3)))
#endif
            ;

#ifdef __cplusplus
}
#endif

#endif /* _FILE_UTIL_H */
