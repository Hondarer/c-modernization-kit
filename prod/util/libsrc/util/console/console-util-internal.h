/**
 *******************************************************************************
 *  @file           console-util-internal.h
 *  @brief          コンソールヘルパー内部関数のヘッダーファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/04
 *  @version        1.0.0
 *
 *  console-util.c が公開する内部関数を宣言します。
 *  このヘッダーはモジュール内部でのみ使用します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef CONSOLE_UTIL_INTERNAL_H
#define CONSOLE_UTIL_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 *******************************************************************************
 *  @brief          ライブラリアンロード時にコンソールヘルパーを解放します。
 *  @param[in]      process_terminating プロセス終了による呼び出しの場合は 1、
 *                  明示的なアンロードの場合は 0 を指定します。
 *******************************************************************************
 */
void console_dispose_on_unload(int process_terminating);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CONSOLE_UTIL_INTERNAL_H */
