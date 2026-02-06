/**
 *******************************************************************************
 *  @file           platform.h
 *  @brief          プラットフォームおよびアーキテクチャ検出マクロのヘッダーファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2025/11/22
 *
 *  ビルド対象の OS とプロセッサアーキテクチャを検出し、統一的なマクロを定義します。
 *
 *  @section        platform_detection プラットフォーム検出マクロ
 *
 *  検出されたプラットフォームに応じて、以下のマクロを定義します。
 *
 *  | プラットフォーム | 識別マクロ           | PLATFORM_NAME       |
 *  | ---------------- | -------------------- | ------------------- |
 *  | Windows          | PLATFORM_WINDOWS     | "Windows"           |
 *  | Linux            | PLATFORM_LINUX       | "Linux"             |
 *  | macOS            | PLATFORM_MACOS       | "macOS"             |
 *  | Apple (非 macOS) | PLATFORM_APPLE_OTHER | "Apple (non-macOS)" |
 *  | その他           | PLATFORM_UNKNOWN     | "Unknown"           |
 *
 *  @note           Apple プラットフォームでは TargetConditionals.h を使用して
 *                  macOS と iOS 等を区別しています。
 *
 *  @section        arch_detection アーキテクチャ検出マクロ
 *
 *  検出されたアーキテクチャに応じて、以下のマクロを定義します。
 *
 *  | アーキテクチャ | 識別マクロ   | ARCH_NAME |
 *  | -------------- | ------------ | --------- |
 *  | x86_64         | ARCH_X64     | "x64"     |
 *  | x86 (32bit)    | ARCH_X86     | "x86"     |
 *  | AArch64        | ARCH_ARM64   | "ARM64"   |
 *  | ARM (32bit)    | ARCH_ARM     | "ARM"     |
 *  | その他         | ARCH_UNKNOWN | "Unknown" |
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include "compiler.h"

#ifdef DOXYGEN
    #define PLATFORM_WINDOWS     /*!< Windows の場合に定義されます。 */
    #define PLATFORM_LINUX       /*!< Linux の場合に定義されます。 */
    #define PLATFORM_MACOS       /*!< macOS の場合に定義されます。 */
    #define PLATFORM_APPLE_OTHER /*!< macOS 以外の Apple プラットフォームの場合に定義されます。 */
    #define PLATFORM_UNKNOWN     /*!< 未知のプラットフォームの場合に定義されます。 */
    #define PLATFORM_NAME                                                                                              \
        "name" /*!< プラットフォーム名の文字列 ("Windows", "Linux", "macOS", "Apple (non-macOS)", "Unknown")。 */
#else
    #if defined(_WIN32) || defined(_WIN64)
        #define PLATFORM_WINDOWS
        #define PLATFORM_NAME "Windows"
    #elif defined(__linux__)
        #define PLATFORM_LINUX
        #define PLATFORM_NAME "Linux"
    #elif defined(__APPLE__)
        #include <TargetConditionals.h>
        #if TARGET_OS_MAC && !TARGET_OS_IPHONE
            #define PLATFORM_MACOS
            #define PLATFORM_NAME "macOS"
        #else
            #define PLATFORM_APPLE_OTHER
            #define PLATFORM_NAME "Apple (non-macOS)"
        #endif
    #else
        #define PLATFORM_UNKNOWN
        #define PLATFORM_NAME "Unknown"
    #endif
#endif

#ifdef DOXYGEN
    #define ARCH_X64            /*!< x86_64 アーキテクチャの場合に定義されます。 */
    #define ARCH_X86            /*!< x86 (32bit) アーキテクチャの場合に定義されます。 */
    #define ARCH_ARM64          /*!< AArch64 アーキテクチャの場合に定義されます。 */
    #define ARCH_ARM            /*!< ARM (32bit) アーキテクチャの場合に定義されます。 */
    #define ARCH_UNKNOWN        /*!< 未知のアーキテクチャの場合に定義されます。 */
    #define ARCH_NAME    "name" /*!< アーキテクチャ名の文字列 ("x64", "x86", "ARM64", "ARM", "Unknown")。 */
#else
    #if defined(__x86_64__) || defined(_M_X64)
        #define ARCH_X64
        #define ARCH_NAME "x64"
    #elif defined(__i386__) || defined(_M_IX86)
        #define ARCH_X86
        #define ARCH_NAME "x86"
    #elif defined(__aarch64__) || defined(_M_ARM64)
        #define ARCH_ARM64
        #define ARCH_NAME "ARM64"
    #elif defined(__arm__) || defined(_M_ARM)
        #define ARCH_ARM
        #define ARCH_NAME "ARM"
    #else
        #define ARCH_UNKNOWN
        #define ARCH_NAME "Unknown"
    #endif
#endif

#endif /* PLATFORM_H */
