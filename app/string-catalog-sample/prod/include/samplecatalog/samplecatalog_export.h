/**
 *******************************************************************************
 *  @file           samplecatalog_export.h
 *  @brief          samplecatalog の Windows DLL エクスポートおよび呼び出し規約マクロを定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/20
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef SAMPLECATALOG_EXPORT_H
#define SAMPLECATALOG_EXPORT_H

/**
 *  @ingroup        SAMPLECATALOG
 *  @{
 */

#ifdef DOXYGEN

    /**
     *  @brief          DLL エクスポート/インポート制御マクロです。
     *
     *  ビルド条件に応じて次の値を取ります。
     *
     *  | 条件                                                     | 値                      |
     *  | -------------------------------------------------------- | ----------------------- |
     *  | Linux (非 Windows)                                       | (空)                    |
     *  | Windows / `__INTELLISENSE__` 定義時                      | (空)                    |
     *  | Windows / `SAMPLECATALOG_STATIC` 定義時 (静的リンク)     | (空)                    |
     *  | Windows / `SAMPLECATALOG_EXPORTS` 定義時 (DLL ビルド)    | `__declspec(dllexport)` |
     *  | Windows / `SAMPLECATALOG_EXPORTS` 未定義時 (DLL 利用側)  | `__declspec(dllimport)` |
     */
    #define SAMPLECATALOG_EXPORT

    /**
     *  @brief          呼び出し規約マクロです。
     *
     *  Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
     *  Linux (非 Windows) 環境では空に展開されます。
     */
    #define SAMPLECATALOG_API

#else /* !DOXYGEN */

    #ifndef SAMPLECATALOG_STATIC
        #define SAMPLECATALOG_STATIC 0
    #endif /* SAMPLECATALOG_STATIC */
    #ifndef SAMPLECATALOG_EXPORTS
        #define SAMPLECATALOG_EXPORTS 0
    #endif /* SAMPLECATALOG_EXPORTS */
    #include <cplat/base/dll_exports.h>
    #define SAMPLECATALOG_EXPORT CPLAT_DLL_EXPORT(SAMPLECATALOG)
    #define SAMPLECATALOG_API    CPLAT_DLL_API(SAMPLECATALOG)

#endif /* DOXYGEN */

/** @} */

#endif /* SAMPLECATALOG_EXPORT_H */
