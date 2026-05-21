/**
 *******************************************************************************
 *  @file           override_spec.h
 *  @brief          override ライブラリ (動的リンク用) のヘッダーファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  このライブラリは libbase から動的にロードされ、
 *  処理を引き受けるオーバーライド関数を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef OVERRIDE_SPEC_H
#define OVERRIDE_SPEC_H

#include <override/override_export.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          sample_func のオーバーライド実装。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインタ。
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *                  libbase の sample_func から動的にロードされ呼び出されます。\n
     *                  a * b を計算して result に格納します。
     *
     *  @par            使用例
     *  @code{.c}
         int result;
         if (override_func(1, 2, &result) == 0) {
             console_output("result: %d\n", result);  // 出力: result: 2
         }
     *  @endcode
     *
     *  @warning        result が NULL の場合は -1 を返します。
     */
    BASE_EXT_EXPORT extern int BASE_EXT_API override_func(const int a, const int b, int *result);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OVERRIDE_SPEC_H */
