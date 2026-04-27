/**
 *******************************************************************************
 *  @file           libbase.h
 *  @brief          ベースライブラリ (動的リンク用) のヘッダーファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  このライブラリは動的ライブラリのオーバーライド機能を示すサンプルです。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBBASE_H
#define LIBBASE_H

#include <com_util/base/platform.h>

#include <stddef.h>
#include <com_util/runtime/sym_loader.h>

#ifdef DOXYGEN

    /**
     *  @def            BASE_EXPORT
     *  @brief          DLL エクスポート/インポート制御マクロ。
     *  @details        ビルド条件に応じて以下の値を取ります。
     *
     *  | 条件                                              | 値                       |
     *  | ------------------------------------------------- | ------------------------ |
     *  | Linux (非 Windows)                                | (空)                     |
     *  | Windows / `__INTELLISENSE__` 定義時               | (空)                     |
     *  | Windows / `BASE_STATIC` 定義時 (静的リンク)       | (空)                     |
     *  | Windows / `BASE_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
     *  | Windows / `BASE_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
     */
    #define BASE_EXPORT

    /**
     *  @def            BASE_API
     *  @brief          呼び出し規約マクロ。
     *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
     *                  Linux (非 Windows) 環境では空に展開されます。\n
     *                  すでに定義済みの場合は再定義されません。
     */
    #define BASE_API

#else /* !DOXYGEN */

    #ifndef BASE_STATIC
        #define BASE_STATIC 0
    #endif /* BASE_STATIC */
    #ifndef BASE_EXPORTS
        #define BASE_EXPORTS 0
    #endif /* BASE_EXPORTS */
    #include <com_util/base/dll_exports.h>
    #define BASE_EXPORT COM_UTIL_DLL_EXPORT(BASE)
    #define BASE_API    COM_UTIL_DLL_API(BASE)

#endif /* DOXYGEN */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *******************************************************************************
     *  @brief          計算処理を行います。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインタ。
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *  @warning        result が NULL の場合は -1 を返します。
     *******************************************************************************
     */
    BASE_EXPORT extern int BASE_API sample_func(const int a, const int b, int *result);

    /**
     *******************************************************************************
     *  @brief          printf と同じ書式でコンソールに出力します。
     *  @param[in]      format printf 互換の書式文字列。
     *  @param[in]      ... 書式文字列に対応する引数。
     *
     *  @details        この関数は printf のラッパーです。\n
     *                  動的ライブラリ内から呼び出し元プロセスのコンソールに出力します。
     *
     *  @par            使用例
     *  @code{.c}
        console_output("result: %d\n", 42);  // 出力: result: 42
     *  @endcode
     *******************************************************************************
     */
    BASE_EXPORT extern void BASE_API console_output(const char *format, ...);

    /**
     *******************************************************************************
     *  @brief          libbase が管理する com_util_sym_loader_entry_t ポインタ配列の内容を標準出力に表示します。
     *  @return         すべてのエントリが正常に解決されている場合は 0、1 つでも失敗している場合は -1 を返します。
     *******************************************************************************
     */
    BASE_EXPORT extern int BASE_API sym_loader_info_libbase(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBASE_H */
