#pragma warning disable 1587
/**
 *******************************************************************************
 *  @file           CalcKind.cs
 *  @brief          計算演算種別の列挙型。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
 *
 *  calc ライブラリでサポートされる計算演算の種別を定義します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */
#pragma warning restore 1587

namespace CalcDotNetLib
{
    /// <summary>
    /// 計算演算の種別を表します。
    /// </summary>
    public enum CalcKind
    {
        /// <summary>
        /// 加算演算 (a + b)。
        /// </summary>
        Add = 1,

        /// <summary>
        /// 減算演算 (a - b)。
        /// </summary>
        Subtract = 2,

        /// <summary>
        /// 乗算演算 (a * b)。
        /// </summary>
        Multiply = 3,

        /// <summary>
        /// 除算演算 (a / b)。
        /// </summary>
        Divide = 4
    }
}
