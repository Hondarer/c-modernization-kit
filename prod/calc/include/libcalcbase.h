/**
 *******************************************************************************
 *  @file           libcalcbase.h
 *  @brief          計算ライブラリ (静的リンク用) のヘッダーファイル。
 *  @author         doxygen-sample team
 *  @date           2025/01/30
 *  @version        1.0.0
 *
 *  このライブラリは基本的な整数演算を提供します。\n
 *  静的リンクによる機能の内部関数を模しています。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBCALCBASE_H
#define LIBCALCBASE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *******************************************************************************
 *  @brief          2 つの整数を加算します。
 *  @param[in]      a 第一オペランド。
 *  @param[in]      b 第二オペランド。
 *  @param[out]     result 計算結果を格納するポインタ。
 *  @return         成功時は 0、失敗時は -1。
 *
 *  @details
 *  この関数は 2 つの整数を受け取り、その合計を result に格納します。
 *  オーバーフローのチェックは行いません。
 *
 *  使用例:
 *
 *  @code{.c}
 *  int result;
 *  if (add(10, 20, &result) == 0) {
 *      printf("Result: %d\n", result);  // 出力: Result: 30
 *  }
 *  @endcode
 *
 *  @note           オーバーフローが発生する可能性がある場合は、
 *                  呼び出し側で範囲チェックを行ってください。
 *  @warning        result が NULL の場合、-1 を返します。
 *******************************************************************************
 */
extern int add(int a, int b, int *result);

/**
 *******************************************************************************
 *  @brief          2 つの整数を減算します。
 *  @param[in]      a 第一オペランド。
 *  @param[in]      b 第二オペランド。
 *  @param[out]     result 計算結果を格納するポインタ。
 *  @return         成功時は 0、失敗時は -1。
 *
 *  @details
 *  この関数は 2 つの整数を受け取り、その差を result に格納します。
 *  オーバーフローのチェックは行いません。
 *
 *  使用例:
 *
 *  @code{.c}
 *  int result;
 *  if (subtract(10, 3, &result) == 0) {
 *      printf("Result: %d\n", result);  // 出力: Result: 7
 *  }
 *  @endcode
 *
 *  @note           オーバーフローが発生する可能性がある場合は、
 *                  呼び出し側で範囲チェックを行ってください。
 *  @warning        result が NULL の場合、-1 を返します。
 *******************************************************************************
 */
extern int subtract(int a, int b, int *result);

/**
 *******************************************************************************
 *  @brief          2 つの整数を乗算します。
 *  @param[in]      a 第一オペランド。
 *  @param[in]      b 第二オペランド。
 *  @param[out]     result 計算結果を格納するポインタ。
 *  @return         成功時は 0、失敗時は -1。
 *
 *  @details
 *  この関数は 2 つの整数を受け取り、その積を result に格納します。
 *  オーバーフローのチェックは行いません。
 *
 *  使用例:
 *
 *  @code{.c}
 *  int result;
 *  if (multiply(5, 4, &result) == 0) {
 *      printf("Result: %d\n", result);  // 出力: Result: 20
 *  }
 *  @endcode
 *
 *  @note           オーバーフローが発生する可能性がある場合は、
 *                  呼び出し側で範囲チェックを行ってください。
 *  @warning        result が NULL の場合、-1 を返します。
 *******************************************************************************
 */
extern int multiply(int a, int b, int *result);

/**
 *******************************************************************************
 *  @brief          2 つの整数を除算します。
 *  @param[in]      a 被除数。
 *  @param[in]      b 除数。
 *  @param[out]     result 計算結果を格納するポインタ。
 *  @return         成功時は 0、失敗時は -1。
 *
 *  @details
 *  この関数は 2 つの整数を受け取り、その商を result に格納します。
 *  整数除算のため、小数点以下は切り捨てられます。
 *
 *  使用例:
 *
 *  @code{.c}
 *  int result;
 *  if (divide(20, 4, &result) == 0) {
 *      printf("Result: %d\n", result);  // 出力: Result: 5
 *  }
 *  @endcode
 *
 *  @warning        ゼロ除算の場合、または result が NULL の場合は -1 を返します。
 *******************************************************************************
 */
extern int divide(int a, int b, int *result);

#ifdef __cplusplus
}
#endif

#endif /* LIBCALCBASE_H */
