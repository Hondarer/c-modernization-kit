/**
 *******************************************************************************
 *  @file           libcalcbase.h
 *  @brief          計算ライブラリ (静的リンク用) のヘッダーファイル。
 *  @author         doxygen-sample team
 *  @date           2025/01/30
 *  @version        1.0.0
 *
 *  このライブラリは基本的な整数演算を提供します。<br />
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
 *  @return         a と b の合計値。
 *
 *  @details
 *  この関数は 2 つの整数を受け取り、その合計を返します。
 *  オーバーフローのチェックは行いません。
 *
 *  使用例:
 *
 *  @code{.c}
 *  int result = add(10, 20);
 *  printf("Result: %d\n", result);  // 出力: Result: 30
 *  @endcode
 *
 *  @note           オーバーフローが発生する可能性がある場合は、
 *                  呼び出し側で範囲チェックを行ってください。
 *******************************************************************************
 */
extern int add(int a, int b);

#ifdef __cplusplus
}
#endif

#endif /* LIBCALCBASE_H */
