/**
 *******************************************************************************
 *  @file           file-provider-internal.h
 *  @brief          ファイルプロバイダ内部管理関数のヘッダーファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/03
 *  @version        1.0.0
 *
 *  file-provider.c が公開する内部関数を宣言します。
 *  このヘッダーはモジュール内部でのみ使用します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef TRACE_FILE_PROVIDER_INTERNAL_H
#define TRACE_FILE_PROVIDER_INTERNAL_H

#include <trace-file-util.h>

/**
 *******************************************************************************
 *  @brief          ライブラリアンロード時にファイルプロバイダハンドルを解放します。
 *  @param[in]      handle 解放するファイルプロバイダハンドル。
 *******************************************************************************
 */
void trace_file_provider_dispose_on_unload(trace_file_provider_t *handle);

#endif /* TRACE_FILE_PROVIDER_INTERNAL_H */
