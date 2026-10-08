/**
 *******************************************************************************
 *  @file           calcbase_spec.h
 *  @brief          静的リンク用 calcbase ライブラリの API を公開します。
 *  @author         c-modernization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  このライブラリは基本的な整数演算を提供します。\n
 *  静的リンクによる機能の内部関数を模しています。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef CALCBASE_SPEC_H
#define CALCBASE_SPEC_H

#include <calc/calc_const.h>

/**
 *  @ingroup        CALCBASE_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          2 つの整数を加算します。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインター。NULL を渡してはなりません。
     *  @return         成功時は @ref CALC_OK を返します。
     *  @return         @p result が NULL の場合は @ref CALC_ERR_INVALID_ARGUMENT を返します。
     *
     *  この関数は 2 つの整数を受け取り、その合計を result に格納します。
     *
     *  @par            使用例
        @code{.c}
        int result;
        if (calcbase_add(10, 20, &result) == CALC_OK)
        {
            printf("Result: %d\n", result);  // 出力: Result: 30
        }
        @endcode
     *
     *  @note           オーバーフローが発生する可能性がある場合は、
     *                  呼び出し側で範囲チェックを行ってください。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    extern int calcbase_add(int a, int b, int *result);

    /**
     *  @brief          2 つの整数を減算します。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインター。NULL を渡してはなりません。
     *  @return         成功時は @ref CALC_OK を返します。
     *  @return         @p result が NULL の場合は @ref CALC_ERR_INVALID_ARGUMENT を返します。
     *
     *  この関数は 2 つの整数を受け取り、その差を result に格納します。
     *
     *  @par            使用例
        @code{.c}
        int result;
        if (calcbase_subtract(10, 3, &result) == CALC_OK)
        {
            printf("Result: %d\n", result);  // 出力: Result: 7
        }
        @endcode
     *
     *  @note           オーバーフローが発生する可能性がある場合は、
     *                  呼び出し側で範囲チェックを行ってください。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    extern int calcbase_subtract(int a, int b, int *result);

    /**
     *  @brief          2 つの整数を乗算します。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインター。NULL を渡してはなりません。
     *  @return         成功時は @ref CALC_OK を返します。
     *  @return         @p result が NULL の場合は @ref CALC_ERR_INVALID_ARGUMENT を返します。
     *
     *  この関数は 2 つの整数を受け取り、その積を result に格納します。
     *
     *  @par            使用例
        @code{.c}
        int result;
        if (calcbase_multiply(5, 4, &result) == CALC_OK)
        {
            printf("Result: %d\n", result);  // 出力: Result: 20
        }
        @endcode
     *
     *  @note           オーバーフローが発生する可能性がある場合は、
     *                  呼び出し側で範囲チェックを行ってください。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    extern int calcbase_multiply(int a, int b, int *result);

    /**
     *  @brief          2 つの整数を除算します。
     *  @param[in]      a 被除数。
     *  @param[in]      b 除数。0 を渡してはなりません。
     *  @param[out]     result 計算結果を格納するポインター。NULL を渡してはなりません。
     *  @return         成功時は @ref CALC_OK を返します。
     *  @return         @p result が NULL の場合、または @p b が 0 の場合は @ref CALC_ERR_INVALID_ARGUMENT を返します。
     *
     *  この関数は 2 つの整数を受け取り、その商を result に格納します。
     *  整数除算のため、小数点以下は切り捨てられます。
     *
     *  @par            使用例
        @code{.c}
        int result;
        if (calcbase_divide(20, 4, &result) == CALC_OK)
        {
            printf("Result: %d\n", result);  // 出力: Result: 5
        }
        @endcode
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    extern int calcbase_divide(int a, int b, int *result);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* CALCBASE_SPEC_H */
