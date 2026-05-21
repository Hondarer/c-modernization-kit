/**
 *******************************************************************************
 *  @file           override_export.h
 *  @brief          override の Windows DLL エクスポート/呼び出し規約マクロ。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef OVERRIDE_EXPORT_H
#define OVERRIDE_EXPORT_H

#ifdef DOXYGEN

    /**
     *  @brief          DLL エクスポート/インポート制御マクロ。
     *
     *  ビルド条件に応じて以下の値を取ります。
     *
     *  | 条件                                                   | 値                       |
     *  | ------------------------------------------------------ | ------------------------ |
     *  | Linux (非 Windows)                                     | (空)                     |
     *  | Windows / `__INTELLISENSE__` 定義時                    | (空)                     |
     *  | Windows / `BASE_EXT_STATIC` 定義時 (静的リンク)       | (空)                     |
     *  | Windows / `BASE_EXT_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
     *  | Windows / `BASE_EXT_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
     */
    #define BASE_EXT_EXPORT

    /**
     *  @brief          呼び出し規約マクロ。
     *
     *  Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
     *  Linux (非 Windows) 環境では空に展開されます。\n
     *  すでに定義済みの場合は再定義されません。
     */
    #define BASE_EXT_API

#else /* !DOXYGEN */

    #ifndef BASE_EXT_STATIC
        #define BASE_EXT_STATIC 0
    #endif /* BASE_EXT_STATIC */
    #ifndef BASE_EXT_EXPORTS
        #define BASE_EXT_EXPORTS 0
    #endif /* BASE_EXT_EXPORTS */
    #include <com_util/base/dll_exports.h>
    #define BASE_EXT_EXPORT COM_UTIL_DLL_EXPORT(BASE_EXT)
    #define BASE_EXT_API    COM_UTIL_DLL_API(BASE_EXT)

#endif /* DOXYGEN */

#endif /* OVERRIDE_EXPORT_H */
