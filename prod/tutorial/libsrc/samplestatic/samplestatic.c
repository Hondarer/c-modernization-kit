/**
 *******************************************************************************
 *  @file           libsrc/samplestatic/samplestatic.c
 *  @brief          samplestatic 関数の実装ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/25
 *  @version        1.0.0
 *
 *  static 変数へのアクセスを提供する関数を実装します。\n
 *  テストフレームワークでの static 変数のテスト手法を示すサンプルです。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <samplestatic.h>

/**
 * @brief   内部で使用する static 変数。
 *
 * この変数はテストフレームワークの inject 機能により、
 * テストコードから直接アクセス可能です。
 */
static int static_int;

/* doxygen コメントは、ヘッダに記載 */
int samplestatic(void)
{
    return static_int;
}
