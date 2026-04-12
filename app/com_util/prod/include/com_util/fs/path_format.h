#ifndef PATH_FORMAT_H
#define PATH_FORMAT_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <com_util/base/platform.h>

/* プラットフォーム固有の stat 構造体の typedef */
#if defined(PLATFORM_LINUX)
typedef struct stat util_file_stat_t;
#elif defined(PLATFORM_WINDOWS)
typedef struct _stat64 util_file_stat_t;
#endif /* PLATFORM_ */

/* OS 固有のパス最大長を定義 */

#include <com_util/fs/path_max.h>

#if defined(PLATFORM_LINUX)
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#elif defined(PLATFORM_WINDOWS)
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <io.h>
    #include <direct.h>
#endif /* PLATFORM_ */

/* access_fmt 用のモード定数 */

#ifdef DOXYGEN
    /**
     *  @def            ACCESS_FMT_F_OK
     *  @brief          ファイルの存在を確認するモード定数。
     *  @details        access_fmt() の mode 引数に使用します。
     */
    #define ACCESS_FMT_F_OK 0

    /**
     *  @def            ACCESS_FMT_R_OK
     *  @brief          ファイルの読み取り権限を確認するモード定数。
     *  @details        access_fmt() の mode 引数に使用します。
     */
    #define ACCESS_FMT_R_OK 4

    /**
     *  @def            ACCESS_FMT_W_OK
     *  @brief          ファイルの書き込み権限を確認するモード定数。
     *  @details        access_fmt() の mode 引数に使用します。
     */
    #define ACCESS_FMT_W_OK 2
#else /* !DOXYGEN */
    #if defined(PLATFORM_LINUX)
        #define ACCESS_FMT_F_OK F_OK
        #define ACCESS_FMT_R_OK R_OK
        #define ACCESS_FMT_W_OK W_OK
    #elif defined(PLATFORM_WINDOWS)
        #define ACCESS_FMT_F_OK 0
        #define ACCESS_FMT_R_OK 4
        #define ACCESS_FMT_W_OK 2
    #endif /* PLATFORM_ */
#endif     /* DOXYGEN */

#ifdef DOXYGEN

    /**
     *  @def            PATH_FORMAT_EXPORT
     *  @brief          DLL エクスポート/インポート制御マクロ。
     *  @details        ビルド条件に応じて以下の値を取ります。
     *
     *  | 条件                                                   | 値                       |
     *  | ------------------------------------------------------ | ------------------------ |
     *  | Linux (非 Windows)                                     | (空)                     |
     *  | Windows / `__INTELLISENSE__` 定義時                    | (空)                     |
     *  | Windows / `PATH_FORMAT_STATIC` 定義時 (静的リンク)       | (空)                     |
     *  | Windows / `PATH_FORMAT_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
     *  | Windows / `PATH_FORMAT_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
     */
    #define PATH_FORMAT_EXPORT

    /**
     *  @def            PATH_FORMAT_API
     *  @brief          呼び出し規約マクロ。
     *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
     *                  Linux (非 Windows) 環境では空に展開されます。\n
     *                  すでに定義済みの場合は再定義されません。
     */
    #define PATH_FORMAT_API

#else /* !DOXYGEN */

    #define COM_UTIL_DLL_EXPORT_PREFIX PATH_FORMAT
    #include <com_util/base/dll_exports.h>
    #define PATH_FORMAT_EXPORT COM_UTIL_DLL_EXPORT_VALUE
    #define PATH_FORMAT_API    COM_UTIL_DLL_API_VALUE

#endif /* DOXYGEN */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          printf 形式でファイル名を指定してファイルを開きます。
     *
     *  本関数は、呼び出し元が printf と同様の書式指定を用いてファイル名を組み立てられるようにするための
     *  fopen ラッパー関数です。
     *
     *  指定された書式文字列 (format) と可変引数 (...)
     *  からファイル名を生成し、その結果を用いてファイルをオープンします。
     *
     *  書式展開には vsnprintf を使用し、生成されたファイル名が
     *  OS の制限や内部バッファ長に収まらない場合、またはファイルのオープンに失敗した場合は NULL を返します。
     *
     *  失敗理由の取得が必要な場合は errno_out を指定してください。
     *  指定された場合、環境に応じたエラーコードを格納します。
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
     *  @code{.c}
        FILE *fp = fopen_fmt("r", NULL, "data_%d.txt", 123);
     *  @endcode
     *
     *  @par            使用例 (エラー コードの取得あり)
     *  @code{.c}
        int err;
        FILE *fp = fopen_fmt("r", &err, "data_%d.txt", 123);
     *  @endcode
     */
    PATH_FORMAT_EXPORT FILE *PATH_FORMAT_API fopen_fmt(const char *modes, int *errno_out, const char *format, ...)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 3, 4)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定する stat ラッパー関数
     *
     *  この関数は、printf と同じ形式でファイル名を指定してファイル情報を取得します。
     *  内部で vsnprintf を使用してファイル名をフォーマットし、stat を呼び出します。
     *
     *  @param[out]     buf ファイル情報を格納する構造体へのポインタ (util_file_stat_t 型)
     *  @param[in]      format ファイル名のフォーマット文字列 (printf 形式)
     *  @param[in]      ... フォーマット文字列の可変引数
     *  @return         成功時は 0、失敗時は -1
     *
     *  @note           ファイル名の最大長は OS の規定値です (Windows: MAX_PATH=260, Linux: PATH_MAX=通常4096)
     *  @note           util_file_stat_t は、Linux では struct stat、Windows では struct _stat64 の typedef です
     *  @par            使用例
     *  @code{.c}
        util_file_stat_t st; int ret = stat_fmt(&st, "data_%d.txt", 123);
     *  @endcode
     *  @note           Linux では stat()、Windows では _stat64() を使用します
     *  @warning        Linux と Windows では構造体のフィールドが異なるため、プラットフォーム固有のコードが必要です
     *                      - Windows には st_blksize, st_blocks フィールドがありません
     *                      - st_ctime は Linux ではメタデータ変更時刻、Windows では作成時刻を表します
     */
    PATH_FORMAT_EXPORT int PATH_FORMAT_API stat_fmt(util_file_stat_t *buf, const char *format, ...)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 2, 3)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定してファイルを開きます (va_list 版)。
     *
     *  fopen_fmt() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
     *  上位のラッパー関数やマクロから va_list を転送する場合に使用します。
     *
     *  @param[in]      modes
     *                  ファイルのオープン モード ("r", "w", "a", "rb", "wb" など)。
     *
     *  @param[out]     errno_out
     *                  エラー コードの格納先。NULL を指定した場合、エラー コードの取得は行いません。
     *
     *  @param[in]      format
     *                  ファイル名の書式文字列 (printf 形式)。
     *
     *  @param[in]      args
     *                  書式文字列に対応する可変引数リスト。
     *
     *  @return         成功した場合は FILE* を返します。失敗した場合は NULL を返します。
     *
     *  @see            fopen_fmt
     */
    PATH_FORMAT_EXPORT FILE *PATH_FORMAT_API vfopen_fmt(const char *modes, int *errno_out, const char *format, va_list args)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 3, 0)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定する stat ラッパー関数 (va_list 版)。
     *
     *  stat_fmt() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
     *
     *  @param[out]     buf ファイル情報を格納する構造体へのポインタ (util_file_stat_t 型)
     *  @param[in]      format ファイル名のフォーマット文字列 (printf 形式)
     *  @param[in]      args フォーマット文字列に対応する可変引数リスト
     *  @return         成功時は 0、失敗時は -1
     *
     *  @see            stat_fmt
     */
    PATH_FORMAT_EXPORT int PATH_FORMAT_API vstat_fmt(util_file_stat_t *buf, const char *format, va_list args)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 2, 0)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定してファイルを削除します。
     *
     *  本関数は、printf と同じ形式でファイル名を指定してファイルを削除するための
     *  remove ラッパー関数です。
     *
     *  指定された書式文字列 (format) と可変引数 (...)
     *  からファイル名を生成し、その結果を用いてファイルを削除します。
     *
     *  @param[in]      format
     *                  ファイル名の書式文字列 (printf 形式)。
     *
     *  @param[in]      ...
     *                  書式文字列に対応する可変引数。
     *
     *  @return         成功時は 0、失敗時は非ゼロ値を返します。
     *
     *  @note           ファイル名の最大長は OS の制限に従います (Windows: MAX_PATH=260, Linux: PATH_MAX=通常4096)。
     *
     *  @par            使用例
     *  @code{.c}
        int ret = remove_fmt("data_%d.txt", 123);
     *  @endcode
     *
     *  @see            vremove_fmt
     */
    PATH_FORMAT_EXPORT int PATH_FORMAT_API remove_fmt(const char *format, ...)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 1, 2)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定してファイルを削除します (va_list 版)。
     *
     *  remove_fmt() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
     *
     *  @param[in]      format
     *                  ファイル名の書式文字列 (printf 形式)。
     *
     *  @param[in]      args
     *                  書式文字列に対応する可変引数リスト。
     *
     *  @return         成功時は 0、失敗時は非ゼロ値を返します。
     *
     *  @see            remove_fmt
     */
    PATH_FORMAT_EXPORT int PATH_FORMAT_API vremove_fmt(const char *format, va_list args)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 1, 0)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定してファイルを開きます (低レベル)。
     *
     *  本関数は、printf と同じ形式でファイル名を指定して低レベルファイルオープンを行うための
     *  open ラッパー関数です。
     *
     *  Linux では open()、Windows では _open() を使用します。
     *
     *  @param[in]      flags
     *                  ファイルオープンフラグ (O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_TRUNC, O_APPEND など)。
     *
     *  @param[in]      mode
     *                  ファイル作成時のパーミッション (O_CREAT 指定時に使用)。
     *                  Linux では 0644 など、Windows では _S_IREAD | _S_IWRITE など。
     *                  O_CREAT を指定しない場合は 0 を渡してください。
     *
     *  @param[in]      format
     *                  ファイル名の書式文字列 (printf 形式)。
     *
     *  @param[in]      ...
     *                  書式文字列に対応する可変引数。
     *
     *  @return         成功時はファイルディスクリプタ (0 以上の整数)、失敗時は -1 を返します。
     *
     *  @note           ファイル名の最大長は OS の制限に従います (Windows: MAX_PATH=260, Linux: PATH_MAX=通常4096)。
     *  @note           Windows の CreateFileA が提供する FILE_SHARE_DELETE や FILE_FLAG_WRITE_THROUGH 等の
     *                  高度な機能は本関数ではサポートしません。
     *
     *  @par            使用例
     *  @code{.c}
        int fd = open_fmt(O_WRONLY | O_CREAT | O_TRUNC, 0644, "log_%d.txt", pid);
     *  @endcode
     *
     *  @see            vopen_fmt
     */
    PATH_FORMAT_EXPORT int PATH_FORMAT_API open_fmt(int flags, int mode, const char *format, ...)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 3, 4)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定してファイルを開きます (低レベル、va_list 版)。
     *
     *  open_fmt() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
     *
     *  @param[in]      flags
     *                  ファイルオープンフラグ。
     *
     *  @param[in]      mode
     *                  ファイル作成時のパーミッション。
     *
     *  @param[in]      format
     *                  ファイル名の書式文字列 (printf 形式)。
     *
     *  @param[in]      args
     *                  書式文字列に対応する可変引数リスト。
     *
     *  @return         成功時はファイルディスクリプタ、失敗時は -1 を返します。
     *
     *  @see            open_fmt
     */
    PATH_FORMAT_EXPORT int PATH_FORMAT_API vopen_fmt(int flags, int mode, const char *format, va_list args)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 3, 0)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定してアクセス可否を確認します。
     *
     *  本関数は、printf と同じ形式でファイル名を指定してファイルの存在や
     *  アクセス権限を確認するための access ラッパー関数です。
     *
     *  Linux では access()、Windows では _access() を使用します。
     *
     *  @param[in]      mode
     *                  確認するアクセスモード。以下の定数を使用してください。
     *                  - ACCESS_FMT_F_OK: ファイルの存在を確認
     *                  - ACCESS_FMT_R_OK: 読み取り権限を確認
     *                  - ACCESS_FMT_W_OK: 書き込み権限を確認
     *
     *  @param[in]      format
     *                  ファイル名の書式文字列 (printf 形式)。
     *
     *  @param[in]      ...
     *                  書式文字列に対応する可変引数。
     *
     *  @return         アクセス可能な場合は 0、不可の場合は -1 を返します。
     *
     *  @note           ファイル名の最大長は OS の制限に従います (Windows: MAX_PATH=260, Linux: PATH_MAX=通常4096)。
     *  @note           Windows の _access() は実行権限チェック (X_OK) をサポートしないため、
     *                  FMTIO_X_OK は提供しません。
     *
     *  @par            使用例
     *  @code{.c}
        if (access_fmt(ACCESS_FMT_F_OK, "config_%d.txt", instance_id) == 0)
        {
            // ファイルが存在する
        }
     *  @endcode
     *
     *  @see            vaccess_fmt
     */
    PATH_FORMAT_EXPORT int PATH_FORMAT_API access_fmt(int mode, const char *format, ...)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 2, 3)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定してアクセス可否を確認します (va_list 版)。
     *
     *  access_fmt() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
     *
     *  @param[in]      mode
     *                  確認するアクセスモード (ACCESS_FMT_F_OK, ACCESS_FMT_R_OK, ACCESS_FMT_W_OK)。
     *
     *  @param[in]      format
     *                  ファイル名の書式文字列 (printf 形式)。
     *
     *  @param[in]      args
     *                  書式文字列に対応する可変引数リスト。
     *
     *  @return         アクセス可能な場合は 0、不可の場合は -1 を返します。
     *
     *  @see            access_fmt
     */
    PATH_FORMAT_EXPORT int PATH_FORMAT_API vaccess_fmt(int mode, const char *format, va_list args)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 2, 0)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          printf 形式でディレクトリ名を指定してディレクトリを作成します。
     *
     *  本関数は、printf と同じ形式でディレクトリ名を指定してディレクトリを作成するための
     *  mkdir ラッパー関数です。
     *
     *  Linux では mkdir() をパーミッション 0755 で呼び出します。
     *  Windows では _mkdir() を呼び出します (パーミッション指定はありません)。
     *
     *  @param[in]      format
     *                  ディレクトリ名の書式文字列 (printf 形式)。
     *
     *  @param[in]      ...
     *                  書式文字列に対応する可変引数。
     *
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *  @note           ファイル名の最大長は OS の制限に従います (Windows: MAX_PATH=260, Linux: PATH_MAX=通常4096)。
     *  @note           親ディレクトリが存在しない場合は失敗します (再帰的なディレクトリ作成は行いません)。
     *
     *  @par            使用例
     *  @code{.c}
        int ret = mkdir_fmt("logs_%04d", year);
     *  @endcode
     *
     *  @see            vmkdir_fmt
     */
    PATH_FORMAT_EXPORT int PATH_FORMAT_API mkdir_fmt(const char *format, ...)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 1, 2)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          printf 形式でディレクトリ名を指定してディレクトリを作成します (va_list 版)。
     *
     *  mkdir_fmt() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
     *
     *  @param[in]      format
     *                  ディレクトリ名の書式文字列 (printf 形式)。
     *
     *  @param[in]      args
     *                  書式文字列に対応する可変引数リスト。
     *
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *  @see            mkdir_fmt
     */
    PATH_FORMAT_EXPORT int PATH_FORMAT_API vmkdir_fmt(const char *format, va_list args)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 1, 0)))
#endif /* COMPILER_GCC */
        ;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PATH_FORMAT_H */
