/**
 *******************************************************************************
 *  @file           libcalc.h
 *  @brief          計算ライブラリ (動的リンク用) のヘッダーファイル。
 *  @author         doxygen-sample team
 *  @date           2025/01/30
 *  @version        1.0.0
 *
 *  このライブラリは基本的な整数演算を提供します。\n
 *  動的リンクによる機能外提供用の API を模しています。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBCALC_H
#define LIBCALC_H

/* DLL エクスポート/インポート定義 */
#ifdef _WIN32
#ifndef __INTELLISENSE__
#ifndef CALC_STATIC
#ifdef CALC_EXPORTS
#define CALC_API __declspec(dllexport)
#else /* CALC_EXPORTS */
#define CALC_API __declspec(dllimport)
#endif /* CALC_EXPORTS */
#else  /* CALC_STATIC */
#define CALC_API
#endif /* CALC_STATIC */
#else  /* __INTELLISENSE__ */
#define CALC_API
#endif /* __INTELLISENSE__ */
#ifndef WINAPI
#define WINAPI __stdcall
#endif /* WINAPI */
#else  /* _WIN32 */
#define CALC_API
#define WINAPI
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C"
{
#endif

#define CALC_KIND_ADD 1      /*!< 加算の演算種別を表す定数。 */
#define CALC_KIND_SUBTRACT 2 /*!< 減算の演算種別を表す定数。 */
#define CALC_KIND_MULTIPLY 3 /*!< 乗算の演算種別を表す定数。 */
#define CALC_KIND_DIVIDE 4   /*!< 除算の演算種別を表す定数。 */

    /**
     *******************************************************************************
     *  @brief          指定された演算種別に基づいて計算を実行します。
     *  @param[in]      kind 演算の種別 (CALC_KIND_ADD など)。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインタ。
     *  @return         成功時は 0、失敗時は -1。
     *
     *  @details
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
     *  使用例:
     *
     *  @code{.c}
     *  int result;
     *  if (calcHandler(CALC_KIND_ADD, 10, 20, &result) == 0) {
     *      printf("Result: %d\n", result);  // 出力: Result: 30
     *  }
     *  @endcode
     *
     *  @warning        無効な kind を指定した場合、ゼロ除算の場合、
     *                  または result が NULL の場合は -1 を返します。
     *                  呼び出し側で戻り値のチェックを行ってください。
     *******************************************************************************
     */
    CALC_API extern int WINAPI calcHandler(int kind, int a, int b, int *result);

#ifdef __cplusplus
}
#endif

#endif /* LIBCALC_H */
