/**
 *******************************************************************************
 *  @file           samplestatic.h
 *  @brief          static 変数アクセスのサンプルライブラリヘッダーファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/25
 *  @version        1.0.0
 *
 *  このライブラリは static 変数へのアクセス方法を示すサンプルです。\n
 *  テストフレームワークでの static 変数のテスト手法を説明します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _SAMPLESTATIC_H
#define _SAMPLESTATIC_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *******************************************************************************
     *  @brief          static 変数の値を取得します。
     *  @return         static 変数 static_int の現在の値を返します。
     *
     *  @details
     *  この関数は内部の static 変数の値を返します。\n
     *  テストフレームワークでは、inject ファイルを使用して static 変数を
     *  直接操作することができます。
     *
     *  使用例:
     *
     *  @code{.c}
     *  int value = samplestatic();
     *  printf("Value: %d\n", value);
     *  @endcode
     *
     *  @note           static 変数のテスト方法については、testfw のドキュメントを
     *                  参照してください。
     *******************************************************************************
     */
    extern int samplestatic(void);

#ifdef __cplusplus
}
#endif

#endif // _SAMPLESTATIC_H
