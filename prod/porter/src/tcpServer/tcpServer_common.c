/**
 *******************************************************************************
 *  @file           tcpServer_common.c
 *  @brief          TCP サーバーサンプル 共通変数定義。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/17
 *  @version        1.0.0
 *
 *  - g_session_fn : 登録済みセッション処理関数の実体。platform_init() で設定されます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "tcpServer.h"

/** @brief 登録済みセッション処理関数の実体。platform_init() で設定されます。 */
ClientSessionFn g_session_fn = NULL;
