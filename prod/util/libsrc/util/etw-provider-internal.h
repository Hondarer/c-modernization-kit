/**
 *******************************************************************************
 *  @file           etw-provider-internal.h
 *  @brief          ETW プロバイダ内部管理関数のヘッダーファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/03
 *  @version        1.0.0
 *
 *  etw-provider.c が公開する内部関数を宣言します。
 *  このヘッダーはモジュール内部でのみ使用します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef TRACE_ETW_PROVIDER_INTERNAL_H
#define TRACE_ETW_PROVIDER_INTERNAL_H

#ifdef _WIN32
#include <trace-etw-util.h>

/**
 *******************************************************************************
 *  @brief          ライブラリアンロード時に ETW プロバイダハンドルを解放します。
 *  @param[in]      handle 解放する ETW プロバイダハンドル。
 *  @param[in]      process_terminating プロセス終了による呼び出しの場合は 1、
 *                  明示的なアンロードの場合は 0 を指定します。
 *******************************************************************************
 */
void etw_provider_dispose_on_unload(etw_provider_t *handle, int process_terminating);

#endif /* _WIN32 */

#endif /* TRACE_ETW_PROVIDER_INTERNAL_H */
