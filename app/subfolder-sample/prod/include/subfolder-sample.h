#ifndef SUBFOLDER_SAMPLE_H
#define SUBFOLDER_SAMPLE_H

#include <util/base/platform.h>

#ifdef DOXYGEN

/**
 *  @def            SUBFOLDER_SAMPLE_EXPORT
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                                          | 値                       |
 *  | ------------------------------------------------------------- | ------------------------ |
 *  | Linux (非 Windows)                                            | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時                           | (空)                     |
 *  | Windows / `SUBFOLDER_SAMPLE_STATIC` 定義時 (静的リンク)       | (空)                     |
 *  | Windows / `SUBFOLDER_SAMPLE_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
 *  | Windows / `SUBFOLDER_SAMPLE_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
 */
#define SUBFOLDER_SAMPLE_EXPORT

/**
 *  @def            SUBFOLDER_SAMPLE_API
 *  @brief          呼び出し規約マクロ。
 *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
 *                  Linux (非 Windows) 環境では空に展開されます。\n
 *                  既に定義済みの場合は再定義されません。
 */
#define SUBFOLDER_SAMPLE_API

#else /* !DOXYGEN */

    #define UTIL_DLL_EXPORT_PREFIX SUBFOLDER_SAMPLE
    #include <util/base/dll_exports.h>
    #define SUBFOLDER_SAMPLE_EXPORT UTIL_DLL_EXPORT_VALUE
    #define SUBFOLDER_SAMPLE_API    UTIL_DLL_API_VALUE

#endif /* DOXYGEN */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    SUBFOLDER_SAMPLE_EXPORT extern int SUBFOLDER_SAMPLE_API func(void);
    SUBFOLDER_SAMPLE_EXPORT extern int SUBFOLDER_SAMPLE_API func_a(void);
    SUBFOLDER_SAMPLE_EXPORT extern int SUBFOLDER_SAMPLE_API func_b(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUBFOLDER_SAMPLE_H */
