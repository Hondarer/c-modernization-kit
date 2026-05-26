/**
 *******************************************************************************
 *  @file           base_spec.h
 *  @brief          base ライブラリ (動的リンク用) のヘッダーファイル。
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

#ifndef BASE_SPEC_H
#define BASE_SPEC_H

#include <com_util/base/platform.h>
#include <com_util/runtime/sym_loader.h>
#include <stddef.h>

#include <base/base_export.h>

/**
 *  @ingroup        BASE_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          計算処理を行います。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインタ。
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *  @warning        result が NULL の場合は -1 を返します。
     */
    BASE_EXPORT extern int BASE_API sample_func(const int a, const int b, int *result);

    /**
     *  @brief          printf と同じ書式でコンソールに出力します。
     *  @param[in]      format printf 互換の書式文字列。
     *  @param[in]      ... 書式文字列に対応する引数。
     *
     *  この関数は printf のラッパーです。\n
     *  動的ライブラリ内から呼び出し元プロセスのコンソールに出力します。
     *
     *  @par            使用例
     *  @code{.c}
         console_output("result: %d\n", 42);  // 出力: result: 42
     *  @endcode
     */
    BASE_EXPORT extern void BASE_API console_output(const char *format, ...);

    /**
     *  @brief          libbase が管理する com_util_sym_loader_entry_t ポインタ配列の内容を標準出力に表示します。
     *  @return         すべてのエントリが正常に解決されている場合は 0、1 つでも失敗している場合は -1 を返します。
     */
    BASE_EXPORT extern int BASE_API sym_loader_info_libbase(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* BASE_SPEC_H */
