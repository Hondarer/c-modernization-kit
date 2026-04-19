#ifndef MODULE_INFO_H
#define MODULE_INFO_H

#include <com_util/base/platform.h>

#if defined(PLATFORM_LINUX)
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif /* _GNU_SOURCE */
#endif /* PLATFORM_LINUX */

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef DOXYGEN

/**
 *  @def            MODULE_INFO_EXPORT
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                                    | 値                       |
 *  | ------------------------------------------------------- | ------------------------ |
 *  | Linux (非 Windows)                                      | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時                     | (空)                     |
 *  | Windows / `MODULE_INFO_STATIC` 定義時 (静的リンク)      | (空)                     |
 *  | Windows / `MODULE_INFO_EXPORTS` 定義時 (DLL ビルド)     | `__declspec(dllexport)`  |
 *  | Windows / `MODULE_INFO_EXPORTS` 未定義時 (DLL 利用側)   | `__declspec(dllimport)`  |
 */
#define MODULE_INFO_EXPORT

/**
 *  @def            MODULE_INFO_API
 *  @brief          呼び出し規約マクロ。
 *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
 *                  Linux (非 Windows) 環境では空に展開されます。
 */
#define MODULE_INFO_API

#else /* !DOXYGEN */

    #ifndef MODULE_INFO_STATIC
        #define MODULE_INFO_STATIC 0
    #endif /* MODULE_INFO_STATIC */
    #ifndef MODULE_INFO_EXPORTS
        #define MODULE_INFO_EXPORTS 0
    #endif /* MODULE_INFO_EXPORTS */
    #include <com_util/base/dll_exports.h>
    #define MODULE_INFO_EXPORT COM_UTIL_DLL_EXPORT(MODULE_INFO)
    #define MODULE_INFO_API    COM_UTIL_DLL_API(MODULE_INFO)

#endif /* DOXYGEN */

MODULE_INFO_EXPORT int MODULE_INFO_API
    module_info_get_path(char *out_path, size_t out_path_sz, const void *func_addr);

MODULE_INFO_EXPORT int MODULE_INFO_API
    module_info_get_basename(char *out_basename, size_t out_basename_sz, const void *func_addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MODULE_INFO_H */
