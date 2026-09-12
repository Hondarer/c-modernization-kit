/**
 *******************************************************************************
 *  @file           string_catalog.h
 *  @brief          string_catalog ライブラリの公開 API をまとめて取り込みます。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  string_catalog ライブラリの公開ヘッダーを 1 つにまとめたヘッダーです。\n
 *  利用者は `#include <string_catalog.h>` で本ライブラリの全公開 API にアクセスできます。
 *
 *  アンブレラ ヘッダーは利便性がある一方で、コンパイル時間が増加する可能性があります。\n
 *  個別ヘッダーを利用するか、アンブレラ ヘッダーを利用するかは利用環境に応じて選択してください。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef STRING_CATALOG_H
#define STRING_CATALOG_H

/**
 *  @defgroup       STRING_CATALOG_PUBLIC_API 公開 API (string_catalog)
 *  @brief          string_catalog ライブラリの公開 API です。
 */

#include <string_catalog/string_catalog_argument.h>
#include <string_catalog/string_catalog_const.h>
#include <string_catalog/string_catalog_entry.h>
#include <string_catalog/string_catalog_language.h>
#include <string_catalog/string_catalog_spec.h>

#endif /* STRING_CATALOG_H */
