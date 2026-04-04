/**
 *******************************************************************************
 *  @file           syslog-provider-internal.h
 *  @brief          syslog プロバイダ内部管理関数のヘッダーファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/03
 *  @version        1.0.0
 *
 *  syslog-provider.c が公開する内部関数を宣言します。
 *  このヘッダーはモジュール内部でのみ使用します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef TRACE_SYSLOG_PROVIDER_INTERNAL_H
#define TRACE_SYSLOG_PROVIDER_INTERNAL_H

#ifndef _WIN32
#include <trace-syslog-util.h>

/**
 *******************************************************************************
 *  @brief          ライブラリアンロード時に syslog プロバイダハンドルを解放します。
 *  @param[in]      handle 解放する syslog プロバイダハンドル。
 *******************************************************************************
 */
void syslog_provider_dispose_on_unload(syslog_provider_t *handle);

#endif /* !_WIN32 */

#endif /* TRACE_SYSLOG_PROVIDER_INTERNAL_H */
