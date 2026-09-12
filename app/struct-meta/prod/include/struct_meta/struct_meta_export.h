/**
 *******************************************************************************
 *  @file           struct_meta_export.h
 *  @brief          struct_meta の DLL エクスポートと呼び出し規約マクロを定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/08/16
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

#ifndef STRUCT_META_EXPORT_H
#define STRUCT_META_EXPORT_H

/**
 *  @ingroup        STRUCT_META_PUBLIC_API
 *  @{
 */

#ifdef DOXYGEN

    /**
     *  @brief          DLL エクスポート/インポート制御マクロ。
     *
     *  ビルド条件に応じて次の値を取ります。
     *
     *  | 条件                                                | 値                                        |
     *  | ---------------------------------------------------- | ----------------------------------------- |
     *  | Linux / `STRUCT_META_STATIC` 定義時 (静的リンク)    | (空)                                      |
     *  | Linux / 共有ライブラリ (静的リンクでない)           | `__attribute__((visibility("default")))` |
     *  | Windows / `__INTELLISENSE__` 定義時                 | (空)                                      |
     *  | Windows / `STRUCT_META_STATIC` 定義時 (静的リンク)  | (空)                                      |
     *  | Windows / `STRUCT_META_EXPORTS` 定義時 (DLL ビルド) | `__declspec(dllexport)`                   |
     *  | Windows / `STRUCT_META_EXPORTS` 未定義時 (DLL 利用側)| `__declspec(dllimport)`                  |
     */
    #define STRUCT_META_EXPORT

    /**
     *  @brief          呼び出し規約マクロ。
     *
     *  Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
     *  Linux (非 Windows) 環境では空に展開されます。
     */
    #define STRUCT_META_API

#else /* !DOXYGEN */

    #ifndef STRUCT_META_STATIC
        #define STRUCT_META_STATIC 0
    #endif /* STRUCT_META_STATIC */
    #ifndef STRUCT_META_EXPORTS
        #define STRUCT_META_EXPORTS 0
    #endif /* STRUCT_META_EXPORTS */
    #include <cplat/base/dll_exports.h>
    #define STRUCT_META_EXPORT CPLAT_DLL_EXPORT(STRUCT_META)
    #define STRUCT_META_API    CPLAT_DLL_API(STRUCT_META)

#endif /* DOXYGEN */

/** @} */

#endif /* STRUCT_META_EXPORT_H */
