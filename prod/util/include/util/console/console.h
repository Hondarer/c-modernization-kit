#ifndef CONSOLE_H
#define CONSOLE_H

#include <util/base/platform.h>

/**
 *  @file           console.h
 *  @brief          コンソール UTF-8 ヘルパー API。
 *  @details        Windows 環境で stdout / stderr を内部パイプに差し替え、
 *                  コンソール (TTY) 出力は @c WriteConsoleW 経由で UTF-16 として
 *                  送出し、パイプやファイルへは UTF-8 バイト列をそのまま書き戻します。\n
 *                  Linux 環境では @c console_init は何もせず 0 を返します。\n
 *                  呼び出し側は @c \#ifdef @c _WIN32 ガード不要でクロスプラットフォームに
 *                  使用できます。
 *
 *  @par            使用例
 *  @code
 *  #include <util/console/console.h>
 *  #include <stdio.h>
 *
 *  int main(void) {
 *      console_init();              // stdout / stderr を差し替え
 *      printf("こんにちは\n");       // コンソールでは WriteConsoleW、パイプでは UTF-8
 *      fprintf(stderr, "警告\n");
 *      console_dispose();
 *      return 0;
 *  }
 *  @endcode
 */

/* ===== DLL エクスポート / インポート制御マクロ ===== */

#ifdef DOXYGEN

/**
 *  @def            CONSOLE_EXPORT
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                                     | 値                       |
 *  | -------------------------------------------------------- | ------------------------ |
 *  | Linux (非 Windows)                                       | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時                      | (空)                     |
 *  | Windows / `CONSOLE_STATIC` 定義時 (静的リンク)      | (空)                     |
 *  | Windows / `CONSOLE_EXPORTS` 定義時 (DLL ビルド)     | `__declspec(dllexport)`  |
 *  | Windows / `CONSOLE_EXPORTS` 未定義時 (DLL 利用側)   | `__declspec(dllimport)`  |
 */
#define CONSOLE_EXPORT

/**
 *  @def            CONSOLE_API
 *  @brief          呼び出し規約マクロ。
 *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
 *                  Linux (非 Windows) 環境では空に展開されます。\n
 *                  既に定義済みの場合は再定義されません。
 */
#define CONSOLE_API

#else /* !DOXYGEN */

    #define UTIL_DLL_EXPORT_PREFIX CONSOLE
    #include <util/base/dll_exports.h>
    #define CONSOLE_EXPORT UTIL_DLL_EXPORT_VALUE
    #define CONSOLE_API    UTIL_DLL_API_VALUE

#endif /* DOXYGEN */

/* ===== API 関数 ===== */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          コンソールヘルパーを初期化する。
     *  @details        Windows 環境では stdout と stderr を内部パイプに差し替え、
     *                  バックグラウンドスレッドを起動します。\n
     *                  スレッドはパイプから UTF-8 バイト列を受け取り、出力先がコンソール
     *                  (TTY) の場合は @c WriteConsoleW で UTF-16 として書き出します。
     *                  パイプやファイルへは UTF-8 バイト列をそのまま転送します。\n
     *                  Linux 環境では何もしません。\n
     *                  stdin には触れません。\n
     *                  本関数はプログラム開始時に一度だけ呼び出すことを想定しています。\n
     *                  初期化に失敗した場合は stderr に警告を出力し、何もせずに返ります。
     *
     *  @note           リソースはライブラリアンロード時に自動解放されます。
     *                  明示的に解放する場合は @c console_dispose を呼び出してください。
     */
    CONSOLE_EXPORT void CONSOLE_API console_init(void);

    /**
     *  @brief          コンソールヘルパーを終了し、リソースを解放する。
     *  @details        Windows 環境ではバックグラウンドスレッドを停止し、
     *                  stdout / stderr を元のハンドルに戻します。\n
     *                  Linux 環境では何もしません。\n
     *                  @c console_init を呼び出していない場合も安全に呼び出せます。
     */
    CONSOLE_EXPORT void CONSOLE_API console_dispose(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CONSOLE_H */
