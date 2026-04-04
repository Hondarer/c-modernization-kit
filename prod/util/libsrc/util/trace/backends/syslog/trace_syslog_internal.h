/**
 *******************************************************************************
 *  @file           trace_syslog_internal.h
 *  @brief          syslog プロバイダ内部管理関数のヘッダーファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/03
 *  @version        1.0.0
 *
 *  trace_syslog.c が公開する内部関数を宣言します。
 *  このヘッダーはモジュール内部でのみ使用します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef TRACE_SYSLOG_PROVIDER_INTERNAL_H
#define TRACE_SYSLOG_PROVIDER_INTERNAL_H

#ifndef _WIN32
#include <util/trace/trace_syslog.h>

/**
 *******************************************************************************
 *  @brief          ライブラリアンロード時に syslog プロバイダハンドルを解放します。
 *  @param[in]      handle 解放する syslog プロバイダハンドル。
 *******************************************************************************
 */
void trace_syslog_sink_destroy_on_unload(trace_syslog_sink_t *handle);

#endif /* !_WIN32 */

#endif /* TRACE_SYSLOG_PROVIDER_INTERNAL_H */
