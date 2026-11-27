/**
 *******************************************************************************
 *  @file           export.h
 *  @brief          process-orchestration-sample ライブラリのシンボル公開マクロを定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/11/27
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

#ifndef PROCESS_ORCHESTRATION_EXPORT_H
#define PROCESS_ORCHESTRATION_EXPORT_H

#include <cplat/base/platform.h>

#if defined(PLATFORM_WINDOWS)
    #if defined(POS_SHARED)
        #if defined(POS_EXPORTS)
            #define POS_EXPORT __declspec(dllexport)
        #else /* !POS_EXPORTS */
            #define POS_EXPORT __declspec(dllimport)
        #endif /* POS_EXPORTS */
    #else      /* !POS_SHARED */
        #define POS_EXPORT
    #endif /* POS_SHARED */
    #define POS_API __cdecl
#else /* !PLATFORM_WINDOWS */
    #if defined(POS_SHARED)
        #define POS_EXPORT __attribute__((visibility("default")))
    #else /* !POS_SHARED */
        #define POS_EXPORT
    #endif /* POS_SHARED */
    #define POS_API
#endif /* PLATFORM_WINDOWS */

#endif /* PROCESS_ORCHESTRATION_EXPORT_H */
