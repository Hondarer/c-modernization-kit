/**
 *******************************************************************************
 *  @file           dll_exports.h
 *  @brief          Windows DLL エクスポート/呼び出し規約マクロの共通テンプレート。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/09
 *  @version        1.0.0
 *
 *  呼び出し側は `UTIL_DLL_EXPORT_PREFIX` にプレフィックスを定義してから
 *  このヘッダーをインクルードしてください。
 *
 *  例:
    @code{.c}
    #define UTIL_DLL_EXPORT_PREFIX CALC
    #include <util/base/dll_exports.h>
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

#include <util/base/platform.h>

#ifndef UTIL_DLL_EXPORT_PREFIX
    #error "UTIL_DLL_EXPORT_PREFIX must be defined before including <util/base/dll_exports.h>"
#endif /* UTIL_DLL_EXPORT_PREFIX */

#ifndef DOXYGEN
    #ifndef UTIL_DLL_PP_CAT_IMPL__
        #define UTIL_DLL_PP_CAT_IMPL__(a, b) a##b
    #endif /* UTIL_DLL_PP_CAT_IMPL__ */
    #ifndef UTIL_DLL_PP_CAT__
        #define UTIL_DLL_PP_CAT__(a, b) UTIL_DLL_PP_CAT_IMPL__(a, b)
    #endif /* UTIL_DLL_PP_CAT__ */
#endif     /* !DOXYGEN */

#ifdef UTIL_DLL_EXPORT_VALUE
    #undef UTIL_DLL_EXPORT_VALUE
#endif /* UTIL_DLL_EXPORT_VALUE */

#ifdef UTIL_DLL_API_VALUE
    #undef UTIL_DLL_API_VALUE
#endif /* UTIL_DLL_API_VALUE */

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
    #define UTIL_DLL_EXPORT_VALUE

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
    #define UTIL_DLL_API_VALUE
#else /* !DOXYGEN */
    #if defined(PLATFORM_LINUX)
        #define UTIL_DLL_EXPORT_VALUE
        #define UTIL_DLL_API_VALUE
    #elif defined(PLATFORM_WINDOWS)
        #ifndef __INTELLISENSE__
            #if UTIL_DLL_PP_CAT__(UTIL_DLL_EXPORT_PREFIX, _STATIC)
                #define UTIL_DLL_EXPORT_VALUE
            #elif UTIL_DLL_PP_CAT__(UTIL_DLL_EXPORT_PREFIX, _EXPORTS)
                #define UTIL_DLL_EXPORT_VALUE __declspec(dllexport)
            #else /* !UTIL_DLL_PP_CAT__(UTIL_DLL_EXPORT_PREFIX, _EXPORTS) */
                #define UTIL_DLL_EXPORT_VALUE __declspec(dllimport)
            #endif /* UTIL_DLL_PP_CAT__(UTIL_DLL_EXPORT_PREFIX, _STATIC) */
        #else      /* __INTELLISENSE__ */
            #define UTIL_DLL_EXPORT_VALUE
        #endif /* __INTELLISENSE__ */
        #define UTIL_DLL_API_VALUE __stdcall
    #else /* !PLATFORM_LINUX && !PLATFORM_WINDOWS */
        #define UTIL_DLL_EXPORT_VALUE
        #define UTIL_DLL_API_VALUE
    #endif /* PLATFORM_ */
#endif     /* DOXYGEN */

#undef UTIL_DLL_EXPORT_PREFIX
