#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>

/* プラットフォーム固有の stat 構造体の typedef */
#ifndef _WIN32
typedef struct stat file_stat_t;
#else /* _WIN32 */
typedef struct _stat64 file_stat_t;
#endif /* _WIN32 */

/* DLL エクスポート/インポート定義 */
#ifndef _WIN32
#define FILE_UTIL_API
#define WINAPI
#else /* _WIN32 */
#ifndef __INTELLISENSE__
#ifndef FILE_UTIL_STATIC
#ifdef FILE_UTIL_EXPORTS
#define FILE_UTIL_API __declspec(dllexport)
#else /* FILE_UTIL_EXPORTS */
#define FILE_UTIL_API __declspec(dllimport)
#endif /* FILE_UTIL_EXPORTS */
#else  /* FILE_UTIL_STATIC */
#define FILE_UTIL_API
#endif /* FILE_UTIL_STATIC */
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
         *  @brief          printf 形式でファイル名を指定してファイルを開きます。
         *
         *  本関数は、呼び出し元が printf と同様の書式指定を用いてファイル名を組み立てられるようにするための fopen ラッパー関数です。
         *
         *  指定された書式文字列 (format) と可変引数 (...) からファイル名を生成し、その結果を用いてファイルをオープンします。
         *
         *  書式展開には vsnprintf を使用し、生成されたファイル名が OS の制限や内部バッファ長に収まらない場合、またはファイルのオープンに失敗した場合は NULL を返します。
         *
         *  失敗理由の取得が必要な場合は errno_out を指定してください。指定された場合、環境に応じたエラー コードを格納します。
         *
         *  @param[in]      modes
         *                  ファイルのオープン モード ("r", "w", "a", "rb", "wb" など)。
         *
         *  @param[out]     errno_out
         *                  エラー コードの格納先。
         *                  Linux では errno の値、Windows では fopen_s の戻り値を格納します。
         *                  NULL を指定した場合、エラー コードの取得は行いません。
         *
         *  @param[in]      format
         *                  ファイル名の書式文字列 (printf 形式)。
         *
         *  @param[in]      ...
         *                  書式文字列に対応する可変引数。
         *
         *  @return         成功した場合は FILE* を返します。失敗した場合は NULL を返します。
         *
         *  @note           ファイル名の最大長は OS の制限に従います (Windows: MAX_PATH=260, Linux: PATH_MAX=通常4096)。
         *
         *  @par            使用例 (エラー コードの取得なし)
         *                  @code
         *                  FILE *fp = fopen_printf("r", NULL, "data_%d.txt", 123);
         *                  @endcode
         *
         *  @par            使用例 (エラー コードの取得あり)
         *                  @code
         *                  int err;
         *                  FILE *fp = fopen_printf("r", &err, "data_%d.txt", 123);
         *                  @endcode
         */
        FILE_UTIL_API FILE *WINAPI fopen_printf(const char *modes, int *errno_out, const char *format, ...)
#ifdef __GNUC__
            __attribute__((format(printf, 3, 4)))
#endif
            ;

        /**
         *  @brief          printf 形式でファイル名を指定する stat ラッパー関数
         *
         *  この関数は、printf と同じ形式でファイル名を指定してファイル情報を取得します。
         *  内部で vsnprintf を使用してファイル名をフォーマットし、stat を呼び出します。
         *
         *  @param[out]     buf ファイル情報を格納する構造体へのポインタ (file_stat_t 型)
         *  @param[in]      format ファイル名のフォーマット文字列 (printf 形式)
         *  @param[in]      ... フォーマット文字列の可変引数
         *  @return         成功時は 0、失敗時は -1
         *
         *  @note           ファイル名の最大長は OS の規定値です (Windows: MAX_PATH=260, Linux: PATH_MAX=通常4096)
         *  @note           file_stat_t は、Linux では struct stat、Windows では struct _stat64 の typedef です
         *  @note           使用例: file_stat_t st; int ret = stat_printf(&st, "data_%d.txt", 123);
         *  @note           Linux では stat()、Windows では _stat64() を使用します
         *  @warning        Linux と Windows では構造体のフィールドが異なるため、プラットフォーム固有のコードが必要です
         *                  - Windows には st_blksize, st_blocks フィールドがありません
         *                  - st_ctime は Linux ではメタデータ変更時刻、Windows では作成時刻を表します
         */
        FILE_UTIL_API int WINAPI stat_printf(file_stat_t *buf, const char *format, ...)
#ifdef __GNUC__
            __attribute__((format(printf, 2, 3)))
#endif
            ;

#ifdef __cplusplus
}
#endif

#endif /* FILE_UTIL_H */
