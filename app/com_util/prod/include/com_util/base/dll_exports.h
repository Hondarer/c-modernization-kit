/**
 *******************************************************************************
 *  @file           dll_exports.h
 *  @brief          Windows DLL エクスポート/呼び出し規約マクロの共通テンプレート。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/09
 *  @version        1.0.0
 *
 *  呼び出し側は `COM_UTIL_DLL_EXPORT_PREFIX` にプレフィックスを定義してから
 *  このヘッダーをインクルードしてください。
 *
 *  例:
    @code{.c}
    #define COM_UTIL_DLL_EXPORT_PREFIX CALC
    #include <com_util/base/dll_exports.h>
    @endcode
 *
 *  上記の例では以下の公開マクロを定義します。
 *  - CALC_EXPORT
 *  - CALC_API
 *
 *  判定には以下のプレフィックス派生マクロを利用します。
 *  - CALC_STATIC
 *  - CALC_EXPORTS
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>

#ifndef COM_UTIL_DLL_EXPORT_PREFIX
    #error "COM_UTIL_DLL_EXPORT_PREFIX must be defined before including <com_util/base/dll_exports.h>"
#endif /* COM_UTIL_DLL_EXPORT_PREFIX */

#ifndef DOXYGEN
    #ifndef COM_UTIL_DLL_PP_CAT_IMPL__
        #define COM_UTIL_DLL_PP_CAT_IMPL__(a, b) a##b
    #endif /* COM_UTIL_DLL_PP_CAT_IMPL__ */
    #ifndef COM_UTIL_DLL_PP_CAT__
        #define COM_UTIL_DLL_PP_CAT__(a, b) COM_UTIL_DLL_PP_CAT_IMPL__(a, b)
    #endif /* COM_UTIL_DLL_PP_CAT__ */
#endif     /* !DOXYGEN */

#ifdef COM_UTIL_DLL_EXPORT_VALUE
    #undef COM_UTIL_DLL_EXPORT_VALUE
#endif /* COM_UTIL_DLL_EXPORT_VALUE */

#ifdef COM_UTIL_DLL_API_VALUE
    #undef COM_UTIL_DLL_API_VALUE
#endif /* COM_UTIL_DLL_API_VALUE */

#ifdef DOXYGEN
    /**
     *  @brief          DLL エクスポート/インポート修飾子。
     *
     *  プラットフォームとビルド構成に応じて展開されます。
     *
     *  | 条件                                              | 展開値                    |
     *  | ------------------------------------------------- | ------------------------- |
     *  | Linux                                             | (空)                      |
     *  | Windows / 静的リンク (`PREFIX_STATIC` 定義あり)   | (空)                      |
     *  | Windows / DLL ビルド (`PREFIX_EXPORTS` 定義あり)  | `__declspec(dllexport)`   |
     *  | Windows / DLL 利用側                              | `__declspec(dllimport)`   |
     */
    #define COM_UTIL_DLL_EXPORT_VALUE

    /**
     *  @brief          呼び出し規約修飾子。
     *
     *  プラットフォームに応じて展開されます。
     *
     *  | 条件    | 展開値        |
     *  | ------- | ------------- |
     *  | Linux   | (空)          |
     *  | Windows | `__stdcall`   |
     */
    #define COM_UTIL_DLL_API_VALUE
#else /* !DOXYGEN */
    #if defined(PLATFORM_LINUX)
        #define COM_UTIL_DLL_EXPORT_VALUE
        #define COM_UTIL_DLL_API_VALUE
    #elif defined(PLATFORM_WINDOWS)
        #ifndef __INTELLISENSE__
            #if COM_UTIL_DLL_PP_CAT__(COM_UTIL_DLL_EXPORT_PREFIX, _STATIC)
                #define COM_UTIL_DLL_EXPORT_VALUE
            #elif COM_UTIL_DLL_PP_CAT__(COM_UTIL_DLL_EXPORT_PREFIX, _EXPORTS)
                #define COM_UTIL_DLL_EXPORT_VALUE __declspec(dllexport)
            #else /* !COM_UTIL_DLL_PP_CAT__(COM_UTIL_DLL_EXPORT_PREFIX, _EXPORTS) */
                #define COM_UTIL_DLL_EXPORT_VALUE __declspec(dllimport)
            #endif /* COM_UTIL_DLL_PP_CAT__(COM_UTIL_DLL_EXPORT_PREFIX, _STATIC) */
        #else      /* __INTELLISENSE__ */
            #define COM_UTIL_DLL_EXPORT_VALUE
        #endif /* __INTELLISENSE__ */
        #define COM_UTIL_DLL_API_VALUE __stdcall
    #else /* !PLATFORM_LINUX && !PLATFORM_WINDOWS */
        #define COM_UTIL_DLL_EXPORT_VALUE
        #define COM_UTIL_DLL_API_VALUE
    #endif /* PLATFORM_ */
#endif     /* DOXYGEN */

#undef COM_UTIL_DLL_EXPORT_PREFIX
