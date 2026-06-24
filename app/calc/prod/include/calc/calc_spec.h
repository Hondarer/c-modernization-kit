/**
 *******************************************************************************
 *  @file           calc_spec.h
 *  @brief          動的リンク用 calc ライブラリの API を公開します。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  このライブラリは基本的な整数演算を提供します。\n
 *  動的リンクによる機能外提供用の API を模しています。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef CALC_SPEC_H
#define CALC_SPEC_H

#include <calc/calc_const.h>
#include <calc/calc_export.h>

/**
 *  @ingroup        CALC_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          指定された演算種別に基づいて計算を実行します。
     *  @param[in]      kind 演算の種別 (CALC_KIND_ADD など)。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインター。
     *  @return         成功時は CALC_SUCCESS、失敗時は CALC_SUCCESS 以外の値を返します。
     *
     *  この関数は演算種別を受け取り、対応する計算関数を呼び出します。
     *  現在サポートされている演算種別は以下の通りです。
     *
     *  | 演算種別            | 説明             |
     *  | ------------------- | ---------------- |
     *  | CALC_KIND_ADD       | 加算を実行       |
     *  | CALC_KIND_SUBTRACT  | 減算を実行       |
     *  | CALC_KIND_MULTIPLY  | 乗算を実行       |
     *  | CALC_KIND_DIVIDE    | 除算を実行       |
     *
     *  @par            使用例
     *  @code{.c}
        int result;
        if (calcHandler(CALC_KIND_ADD, 10, 20, &result) == CALC_SUCCESS) {
            printf("Result: %d\n", result);  // 出力: Result: 30
        }
     *  @endcode
     *
     *  @warning        無効な kind を指定した場合、ゼロ除算の場合、
     *                  または result が NULL の場合は失敗を返します。
     *                  呼び出し側で戻り値のチェックを行ってください。
     */
    CALC_EXPORT extern int CALC_API calcHandler(int kind, int a, int b, int *result);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* CALC_SPEC_H */
