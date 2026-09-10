/**
 *******************************************************************************
 *  @file           message_catalog.h
 *  @brief          message_catalog ライブラリの公開 API をまとめて取り込みます。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  message_catalog ライブラリの公開ヘッダーを 1 つにまとめたヘッダーです。\n
 *  利用者は `#include <message_catalog.h>` で本ライブラリの全公開 API にアクセスできます。
 *
 *  アンブレラ ヘッダーは利便性と引き換えにコンパイル時間がかかります。\n
 *  個別ヘッダーを利用するか、アンブレラ ヘッダーを利用するかは利用者にて選択してください。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef MESSAGE_CATALOG_H
#define MESSAGE_CATALOG_H

/**
 *  @defgroup       MESSAGE_CATALOG_PUBLIC_API 公開 API (message_catalog)
 *  @brief          message_catalog ライブラリの公開 API です。
 */

#include <message_catalog/message_catalog_argument.h>
#include <message_catalog/message_catalog_const.h>
#include <message_catalog/message_catalog_entry.h>
#include <message_catalog/message_catalog_language.h>
#include <message_catalog/message_catalog_spec.h>

#endif /* MESSAGE_CATALOG_H */
