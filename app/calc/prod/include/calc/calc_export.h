/**
 *******************************************************************************
 *  @file           calc_export.h
 *  @brief          calc の Windows DLL エクスポート/呼び出し規約マクロ。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef CALC_EXPORT_H
#define CALC_EXPORT_H

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
     *  | Windows / `CALC_STATIC` 定義時 (静的リンク)       | (空)                     |
     *  | Windows / `CALC_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
     *  | Windows / `CALC_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
     */
    #define CALC_EXPORT

    /**
     *  @brief          呼び出し規約マクロ。
     *
     *  Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
     *  Linux (非 Windows) 環境では空に展開されます。
     */
    #define CALC_API

#else /* !DOXYGEN */

    #ifndef CALC_STATIC
        #define CALC_STATIC 0
    #endif /* CALC_STATIC */
    #ifndef CALC_EXPORTS
        #define CALC_EXPORTS 0
    #endif /* CALC_EXPORTS */
    #include <com_util/base/dll_exports.h>
    #define CALC_EXPORT COM_UTIL_DLL_EXPORT(CALC)
    #define CALC_API    COM_UTIL_DLL_API(CALC)

#endif /* DOXYGEN */

#endif /* CALC_EXPORT_H */
