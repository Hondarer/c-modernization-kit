#ifndef CONSOLE_H
#define CONSOLE_H

#include <com_util/base/platform.h>
#include <com_util_export.h>

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
 *  @code{.c}
    #include <com_util/console/console.h>
    #include <stdio.h>

    int main(void) {
        console_init();              // stdout / stderr を差し替え
        printf("こんにちは\n");       // コンソールでは WriteConsoleW、パイプでは UTF-8
        fprintf(stderr, "警告\n");
        console_dispose();
        return 0;
    }
 *  @endcode
 */

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
    COM_UTIL_EXPORT void COM_UTIL_API console_init(void);

    /**
     *  @brief          コンソールヘルパーを終了し、リソースを解放する。
     *  @details        Windows 環境ではバックグラウンドスレッドを停止し、
     *                  stdout / stderr を元のハンドルに戻します。\n
     *                  Linux 環境では何もしません。\n
     *                  @c console_init を呼び出していない場合も安全に呼び出せます。
     */
    COM_UTIL_EXPORT void COM_UTIL_API console_dispose(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CONSOLE_H */
