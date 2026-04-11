/**
 *******************************************************************************
 *  @file           path_max.h
 *  @brief          OS 固有のパス最大長マクロのヘッダーファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/04
 *
 *  プラットフォームに依存せず、OS のパス最大長を表す定数 @ref PLATFORM_PATH_MAX
 *  を提供します。
 *
 *  @section        platform_path_max PLATFORM_PATH_MAX の展開先
 *
 *  | プラットフォーム | 展開先                                     |
 *  | ---------------- | ------------------------------------------ |
 *  | Linux / macOS    | `<limits.h>` の `PATH_MAX`（通常 4096）    |
 *  | Windows          | `<windows.h>` の `MAX_PATH`（= 260）       |
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef PATH_MAX_H
#define PATH_MAX_H

#include <com_util/base/platform.h>

#ifdef DOXYGEN
    /**
     *  @def            PLATFORM_PATH_MAX
     *  @brief          OS 固有のパス最大長。
     *  @details        Linux / macOS では `limits.h` の `PATH_MAX`、
     *                  Windows では `windows.h` の `MAX_PATH` に展開されます。
     */
    #define PLATFORM_PATH_MAX 4096
#else /* !DOXYGEN */
    #if defined(PLATFORM_LINUX)
        #include <limits.h>
        #define PLATFORM_PATH_MAX PATH_MAX
    #elif defined(PLATFORM_WINDOWS)
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN
        #endif /* WIN32_LEAN_AND_MEAN */
        #include <windows.h>
        #ifdef byte
            #undef byte /* C++17 std::byte と Windows SDK byte typedef の競合を解消 */
        #endif /* byte */
        #define PLATFORM_PATH_MAX MAX_PATH
    #endif /* PLATFORM_ */
#endif     /* DOXYGEN */

#endif /* PATH_MAX_H */
