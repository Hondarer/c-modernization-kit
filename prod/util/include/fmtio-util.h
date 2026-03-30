#ifndef FMTIO_UTIL_H
#define FMTIO_UTIL_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

/* プラットフォーム固有の stat 構造体の typedef */
#ifndef _WIN32
typedef struct stat file_stat_t;
#else  /* _WIN32 */
typedef struct _stat64 file_stat_t;
#endif /* _WIN32 */

/* OS 固有のパス最大長を定義 */

#ifdef DOXYGEN
    /**
     *  @def            FILE_PATH_MAX
     *  @brief          OS 固有のパス最大長。
     *  @details        Linux では `limits.h` の `PATH_MAX`、Windows では `windows.h` の `MAX_PATH` に展開されます。
     */
#else /* !DOXYGEN */
    #ifndef _WIN32
        #include <limits.h>
        #include <sys/stat.h>
        #include <fcntl.h>
        #include <unistd.h>
        #define FILE_PATH_MAX PATH_MAX
    #else /* _WIN32 */
        #include <sys/stat.h>
        #include <windows.h>
        #include <fcntl.h>
        #include <io.h>
        #include <direct.h>
        #define FILE_PATH_MAX MAX_PATH
    #endif /* _WIN32 */
#endif     /* DOXYGEN */

/* accessf 用のモード定数 */

#ifdef DOXYGEN
    /**
     *  @def            FMTIO_F_OK
     *  @brief          ファイルの存在を確認するモード定数。
     *  @details        accessf() の mode 引数に使用します。
     */
    #define FMTIO_F_OK 0

    /**
     *  @def            FMTIO_R_OK
     *  @brief          ファイルの読み取り権限を確認するモード定数。
     *  @details        accessf() の mode 引数に使用します。
     */
    #define FMTIO_R_OK 4

    /**
     *  @def            FMTIO_W_OK
     *  @brief          ファイルの書き込み権限を確認するモード定数。
     *  @details        accessf() の mode 引数に使用します。
     */
    #define FMTIO_W_OK 2
#else /* !DOXYGEN */
    #ifndef _WIN32
        #define FMTIO_F_OK F_OK
        #define FMTIO_R_OK R_OK
        #define FMTIO_W_OK W_OK
    #else /* _WIN32 */
        #define FMTIO_F_OK 0
        #define FMTIO_R_OK 4
        #define FMTIO_W_OK 2
    #endif /* _WIN32 */
#endif     /* DOXYGEN */

#ifdef DOXYGEN

    /**
     *  @def            FMTIO_UTIL_EXPORT
     *  @brief          DLL エクスポート/インポート制御マクロ。
     *  @details        ビルド条件に応じて以下の値を取ります。
     *
     *  | 条件                                                   | 値                       |
     *  | ------------------------------------------------------ | ------------------------ |
     *  | Linux (非 Windows)                                     | (空)                     |
     *  | Windows / `__INTELLISENSE__` 定義時                    | (空)                     |
     *  | Windows / `FMTIO_UTIL_STATIC` 定義時 (静的リンク)       | (空)                     |
     *  | Windows / `FMTIO_UTIL_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
     *  | Windows / `FMTIO_UTIL_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
     */
    #define FMTIO_UTIL_EXPORT

    /**
     *  @def            FMTIO_UTIL_API
     *  @brief          呼び出し規約マクロ。
     *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
     *                  Linux (非 Windows) 環境では空に展開されます。\n
     *                  既に定義済みの場合は再定義されません。
     */
    #define FMTIO_UTIL_API

#else /* !DOXYGEN */

    #ifndef _WIN32
        #define FMTIO_UTIL_EXPORT
        #define FMTIO_UTIL_API
    #else /* _WIN32 */
        #ifndef __INTELLISENSE__
            #ifndef FMTIO_UTIL_STATIC
                #ifdef FMTIO_UTIL_EXPORTS
                    #define FMTIO_UTIL_EXPORT __declspec(dllexport)
                #else /* !FMTIO_UTIL_EXPORTS */
                    #define FMTIO_UTIL_EXPORT __declspec(dllimport)
                #endif /* FMTIO_UTIL_EXPORTS */
            #else      /* FMTIO_UTIL_STATIC */
                #define FMTIO_UTIL_EXPORT
            #endif /* FMTIO_UTIL_STATIC */
        #else      /* __INTELLISENSE__ */
            #define FMTIO_UTIL_EXPORT
        #endif /* __INTELLISENSE__ */
        #ifndef FMTIO_UTIL_API
            #define FMTIO_UTIL_API __stdcall
        #endif /* FMTIO_UTIL_API */
    #endif     /* _WIN32 */

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
        @code
        FILE *fp = fopenf("r", NULL, "data_%d.txt", 123);
        @endcode
     *
     *  @par            使用例 (エラー コードの取得あり)
        @code
        int err;
        FILE *fp = fopenf("r", &err, "data_%d.txt", 123);
        @endcode
     */
    FMTIO_UTIL_EXPORT FILE *FMTIO_UTIL_API fopenf(const char *modes, int *errno_out, const char *format, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 3, 4)))
#endif /* __GNUC__ */
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
     *  @par            使用例
        @code
        file_stat_t st; int ret = statf(&st, "data_%d.txt", 123);
        @endcode
     *  @note           Linux では stat()、Windows では _stat64() を使用します
     *  @warning        Linux と Windows では構造体のフィールドが異なるため、プラットフォーム固有のコードが必要です
     *                      - Windows には st_blksize, st_blocks フィールドがありません
     *                      - st_ctime は Linux ではメタデータ変更時刻、Windows では作成時刻を表します
     */
    FMTIO_UTIL_EXPORT int FMTIO_UTIL_API statf(file_stat_t *buf, const char *format, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 2, 3)))
#endif /* __GNUC__ */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定してファイルを開きます (va_list 版)。
     *
     *  fopenf() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
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
     *  @see            fopenf
     */
    FMTIO_UTIL_EXPORT FILE *FMTIO_UTIL_API vfopenf(const char *modes, int *errno_out, const char *format, va_list args)
#ifdef __GNUC__
        __attribute__((format(printf, 3, 0)))
#endif /* __GNUC__ */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定する stat ラッパー関数 (va_list 版)。
     *
     *  statf() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
     *
     *  @param[out]     buf ファイル情報を格納する構造体へのポインタ (file_stat_t 型)
     *  @param[in]      format ファイル名のフォーマット文字列 (printf 形式)
     *  @param[in]      args フォーマット文字列に対応する可変引数リスト
     *  @return         成功時は 0、失敗時は -1
     *
     *  @see            statf
     */
    FMTIO_UTIL_EXPORT int FMTIO_UTIL_API vstatf(file_stat_t *buf, const char *format, va_list args)
#ifdef __GNUC__
        __attribute__((format(printf, 2, 0)))
#endif /* __GNUC__ */
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
        @code
        int ret = removef("data_%d.txt", 123);
        @endcode
     *
     *  @see            vremovef
     */
    FMTIO_UTIL_EXPORT int FMTIO_UTIL_API removef(const char *format, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 1, 2)))
#endif /* __GNUC__ */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定してファイルを削除します (va_list 版)。
     *
     *  removef() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
     *
     *  @param[in]      format
     *                  ファイル名の書式文字列 (printf 形式)。
     *
     *  @param[in]      args
     *                  書式文字列に対応する可変引数リスト。
     *
     *  @return         成功時は 0、失敗時は非ゼロ値を返します。
     *
     *  @see            removef
     */
    FMTIO_UTIL_EXPORT int FMTIO_UTIL_API vremovef(const char *format, va_list args)
#ifdef __GNUC__
        __attribute__((format(printf, 1, 0)))
#endif /* __GNUC__ */
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
        @code
        int fd = openf(O_WRONLY | O_CREAT | O_TRUNC, 0644, "log_%d.txt", pid);
        @endcode
     *
     *  @see            vopenf
     */
    FMTIO_UTIL_EXPORT int FMTIO_UTIL_API openf(int flags, int mode, const char *format, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 3, 4)))
#endif /* __GNUC__ */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定してファイルを開きます (低レベル、va_list 版)。
     *
     *  openf() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
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
     *  @see            openf
     */
    FMTIO_UTIL_EXPORT int FMTIO_UTIL_API vopenf(int flags, int mode, const char *format, va_list args)
#ifdef __GNUC__
        __attribute__((format(printf, 3, 0)))
#endif /* __GNUC__ */
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
     *                  - FMTIO_F_OK: ファイルの存在を確認
     *                  - FMTIO_R_OK: 読み取り権限を確認
     *                  - FMTIO_W_OK: 書き込み権限を確認
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
        @code
        if (accessf(FMTIO_F_OK, "config_%d.txt", instance_id) == 0)
        {
            // ファイルが存在する
        }
        @endcode
     *
     *  @see            vaccessf
     */
    FMTIO_UTIL_EXPORT int FMTIO_UTIL_API accessf(int mode, const char *format, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 2, 3)))
#endif /* __GNUC__ */
        ;

    /**
     *  @brief          printf 形式でファイル名を指定してアクセス可否を確認します (va_list 版)。
     *
     *  accessf() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
     *
     *  @param[in]      mode
     *                  確認するアクセスモード (FMTIO_F_OK, FMTIO_R_OK, FMTIO_W_OK)。
     *
     *  @param[in]      format
     *                  ファイル名の書式文字列 (printf 形式)。
     *
     *  @param[in]      args
     *                  書式文字列に対応する可変引数リスト。
     *
     *  @return         アクセス可能な場合は 0、不可の場合は -1 を返します。
     *
     *  @see            accessf
     */
    FMTIO_UTIL_EXPORT int FMTIO_UTIL_API vaccessf(int mode, const char *format, va_list args)
#ifdef __GNUC__
        __attribute__((format(printf, 2, 0)))
#endif /* __GNUC__ */
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
        @code
        int ret = mkdirf("logs_%04d", year);
        @endcode
     *
     *  @see            vmkdirf
     */
    FMTIO_UTIL_EXPORT int FMTIO_UTIL_API mkdirf(const char *format, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 1, 2)))
#endif /* __GNUC__ */
        ;

    /**
     *  @brief          printf 形式でディレクトリ名を指定してディレクトリを作成します (va_list 版)。
     *
     *  mkdirf() と同等ですが、可変引数リストの代わりに va_list を受け取ります。
     *
     *  @param[in]      format
     *                  ディレクトリ名の書式文字列 (printf 形式)。
     *
     *  @param[in]      args
     *                  書式文字列に対応する可変引数リスト。
     *
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *  @see            mkdirf
     */
    FMTIO_UTIL_EXPORT int FMTIO_UTIL_API vmkdirf(const char *format, va_list args)
#ifdef __GNUC__
        __attribute__((format(printf, 1, 0)))
#endif /* __GNUC__ */
        ;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FMTIO_UTIL_H */
