/**
 *******************************************************************************
 *  @file           base_spec.h
 *  @brief          動的リンク用 base ライブラリの API を公開します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  このライブラリは動的ライブラリのオーバーライド機能を示すサンプルです。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef BASE_SPEC_H
#define BASE_SPEC_H

#include <cplat/base/platform.h>
#include <cplat/runtime/sym_loader.h>
#include <stddef.h>

#include <base/base_const.h>
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
     *  @brief          base_calc の差し替え実装が満たす関数ポインター型です。
     *
     *  差し替え実装は、この型と同じシグネチャで定義します。\n
     *  引数と戻り値の契約は @ref base_calc と同じです。
     */
    typedef int(BASE_API *base_calc_fn)(int a, int b, int *result);

    /**
     *  @brief          計算処理を行います。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインター。NULL を渡してはなりません。
     *  @return         成功時は @ref BASE_OK を返します。
     *  @return         @p result が NULL の場合は @ref BASE_ERR_INVALID_ARGUMENT を返します。
     *
     *  既定の実装は a + b を計算して @p result に格納します。\n
     *  設定ファイルに差し替えキー `base_calc` が定義されている場合は、
     *  指定されたライブラリの差し替え実装に処理を委譲し、その戻り値を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。
     *  本関数と同じ共有状態へアクセスする API の呼び出しを、呼び出し側で直列化してください。
     */
    BASE_EXPORT extern int BASE_API base_calc(int a, int b, int *result);

    /**
     *  @brief          printf と同じ書式でコンソールに出力します。
     *  @param[in]      format printf 互換の書式文字列。
     *  @param[in]      ... 書式文字列に対応する引数。
     *
     *  この関数は printf のラッパーです。\n
     *  動的ライブラリ内から呼び出し元プロセスのコンソールに出力します。
     *
     *  @par            使用例
        @code{.c}
         base_console_output("result: %d\n", 42);  // 出力: result: 42
        @endcode
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。
     *  本関数と同じ共有状態へアクセスする API の呼び出しを、呼び出し側で直列化してください。
     */
    BASE_EXPORT extern void BASE_API base_console_output(const char *format, ...);

    /**
     *  @brief          libbase が管理する cplat_sym_loader_entry ポインター配列の内容を標準出力に出力します。
     *  @return         すべてのエントリが正常に解決されている場合は @ref BASE_OK を返します。
     *  @return         1 つでも失敗している場合は @ref BASE_ERR_UNKNOWN を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。
     *  本関数と同じ共有状態へアクセスする API の呼び出しを、呼び出し側で直列化してください。
     */
    BASE_EXPORT extern int BASE_API base_sym_loader_info(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* BASE_SPEC_H */
