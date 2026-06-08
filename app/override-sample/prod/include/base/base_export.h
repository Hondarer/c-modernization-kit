/**
 *******************************************************************************
 *  @file           base_export.h
 *  @brief          base の Windows DLL エクスポート/呼び出し規約マクロ。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef BASE_EXPORT_H
#define BASE_EXPORT_H

/**
 *  @ingroup        BASE_PUBLIC_API
 *  @{
 */

#ifdef DOXYGEN

    /**
     *  @brief          DLL エクスポート/インポート制御マクロ。
     *
     *  ビルド条件に応じて以下の値を取ります。
     *
     *  | 条件                                              | 値                       |
     *  | ------------------------------------------------- | ------------------------ |
     *  | Linux (非 Windows)                                | (空)                     |
     *  | Windows / `__INTELLISENSE__` 定義時               | (空)                     |
     *  | Windows / `BASE_STATIC` 定義時 (静的リンク)       | (空)                     |
     *  | Windows / `BASE_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
     *  | Windows / `BASE_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
     */
    #define BASE_EXPORT

    /**
     *  @brief          呼び出し規約マクロ。
     *
     *  Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
     *  Linux (非 Windows) 環境では空に展開されます。\n
     *  すでに定義済みの場合は再定義されません。
     */
    #define BASE_API

#else /* !DOXYGEN */

    #ifndef BASE_STATIC
        #define BASE_STATIC 0
    #endif /* BASE_STATIC */
    #ifndef BASE_EXPORTS
        #define BASE_EXPORTS 0
    #endif /* BASE_EXPORTS */
    #include <com_util/base/dll_exports.h>
    #define BASE_EXPORT COM_UTIL_DLL_EXPORT(BASE)
    #define BASE_API    COM_UTIL_DLL_API(BASE)

#endif /* DOXYGEN */

/** @} */

#endif /* BASE_EXPORT_H */
