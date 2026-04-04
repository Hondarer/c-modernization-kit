/**
 *******************************************************************************
 *  @file           libutil_lifecycle.c
 *  @brief          libutil.so / util.dll のロード・アンロード時処理。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/03
 *  @version        1.0.0
 *
 *  プラットフォームごとのフック (Linux constructor/destructor, Windows DllMain)
 *  は shared_lib_lifecycle が提供します。
 *  このファイルは libutil における onLoad / onUnload の実装を定義します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <util/base/shared_lib_lifecycle.h>

#include "console/console_internal.h"
#include "trace/trace_internal.h"

/**
 *******************************************************************************
 *  @brief          ライブラリロード時に呼び出されます。
 *******************************************************************************
 */
static void onLoad(void)
{
}

/**
 *******************************************************************************
 *  @brief          ライブラリアンロード時に呼び出されます。
 *  @param[in]      process_terminating プロセス終了時は 1、明示的アンロード時は 0。
 *******************************************************************************
 */
static void onUnload(int process_terminating)
{
    console_dispose_on_unload(process_terminating);
    trace_registry_dispose_all_on_unload(process_terminating);
}
