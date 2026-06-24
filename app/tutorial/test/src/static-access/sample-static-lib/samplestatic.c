/**
 *******************************************************************************
 *  @file           libsrc/samplestatic/samplestatic.c
 *  @brief          static 変数へアクセスする samplestatic 関数を提供します。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/25
 *  @version        1.0.0
 *
 *  テスト フレームワークで static 変数をテストする方法を示します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include "samplestatic.h"

/**
 * @brief   内部で使用する static 変数。
 *
 * この変数はテスト フレームワークの inject 機能により、
 * テスト コードから直接アクセス可能です。
 */
static int static_int;

/* Doxygen コメントは、ヘッダーに記載 */

int samplestatic(void)
{
    return static_int;
}
