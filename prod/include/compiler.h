/**
 *******************************************************************************
 *  @file           compiler.h
 *  @brief          コンパイラ検出および抽象化マクロのヘッダーファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/02/06
 *
 *  コンパイラの種類とバージョンを検出し、統一的なマクロを定義します。\n
 *  また、コンパイラ固有のインライン制御属性を抽象化します。
 *
 *  @section        compiler_detection コンパイラ検出マクロ
 *
 *  検出されたコンパイラに応じて、以下のマクロを定義します。
 *
 *  | コンパイラ | 識別マクロ       | COMPILER_NAME | COMPILER_VERSION の形式             |
 *  | ---------- | ---------------- | ------------- | ----------------------------------- |
 *  | MSVC       | COMPILER_MSVC    | "MSVC"        | _MSC_VER の値 (例: 1943)            |
 *  | Clang      | COMPILER_CLANG   | "Clang"       | major * 10000 + minor * 100 + patch |
 *  | GCC        | COMPILER_GCC     | "GCC"         | major * 10000 + minor * 100 + patch |
 *  | その他     | COMPILER_UNKNOWN | "Unknown"     | 0                                   |
 *
 *  @note           Clang は __GNUC__ も定義するため、Clang を GCC より先に判定しています。
 *
 *  @section        inline_control インライン制御マクロ
 *
 *  コンパイラごとに異なるインライン強制・抑制の構文を、統一的なマクロで提供します。
 *
 *  | マクロ名     | MSVC                 | GCC/Clang                             | その他 |
 *  | ------------ | -------------------- | ------------------------------------- | ------ |
 *  | FORCE_INLINE | __forceinline        | inline __attribute__((always_inline)) | inline |
 *  | NO_INLINE    | __declspec(noinline) | __attribute__((noinline))             | (空)   |
 *
 *  @section        usage 使用例
    @code{.c}
    #include "compiler.h"

    FORCE_INLINE int fast_add(int a, int b)
    {
        return a + b;
    }

    NO_INLINE void debug_dump(const char *msg)
    {
        fprintf(stderr, "%s\n", msg);
    }
    @endcode
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef COMPILER_H
#define COMPILER_H

#ifdef DOXYGEN
    #define COMPILER_MSVC           /*!< MSVC コンパイラの場合に定義されます。 */
    #define COMPILER_CLANG          /*!< Clang コンパイラの場合に定義されます。 */
    #define COMPILER_GCC            /*!< GCC コンパイラの場合に定義されます。 */
    #define COMPILER_UNKNOWN        /*!< 未知のコンパイラの場合に定義されます。 */
    #define COMPILER_NAME    "name" /*!< コンパイラ名の文字列 ("MSVC", "Clang", "GCC", "Unknown")。 */
    #define COMPILER_VERSION 0      /*!< コンパイラバージョンの数値。 */
#else
    #if defined(_MSC_VER)
        #define COMPILER_MSVC
        #define COMPILER_NAME    "MSVC"
        #define COMPILER_VERSION _MSC_VER
    #elif defined(__clang__)
        #define COMPILER_CLANG
        #define COMPILER_NAME    "Clang"
        #define COMPILER_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
    #elif defined(__GNUC__)
        #define COMPILER_GCC
        #define COMPILER_NAME    "GCC"
        #define COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
    #else
        #define COMPILER_UNKNOWN
        #define COMPILER_NAME    "Unknown"
        #define COMPILER_VERSION 0
    #endif
#endif

#ifdef DOXYGEN
    #define FORCE_INLINE /*!< インライン展開を強制します。コンパイラに応じた属性に展開されます。 */
    #define NO_INLINE    /*!< インライン展開を抑制します。コンパイラに応じた属性に展開されます。 */
#else
    #if defined(COMPILER_MSVC)
        #define FORCE_INLINE __forceinline
        #define NO_INLINE    __declspec(noinline)
    #elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
        #define FORCE_INLINE inline __attribute__((always_inline))
        #define NO_INLINE    __attribute__((noinline))
    #else
        #define FORCE_INLINE inline
        #define NO_INLINE
    #endif
#endif

#endif /* COMPILER_H */
