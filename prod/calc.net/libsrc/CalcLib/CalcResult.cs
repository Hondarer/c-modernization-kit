#pragma warning disable 1587
/**
 *******************************************************************************
 *  @file           CalcResult.cs
 *  @brief          計算結果クラス。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
 *
 *  計算演算の結果クラスを定義します。
 *  成功ステータス、結果値、エラーコードを含みます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */
#pragma warning restore 1587

namespace CalcLib
{
    /// <summary>
    /// 計算演算の結果を表します。
    /// </summary>
    public class CalcResult
    {
        /// <summary>
        /// 演算が成功したかどうかを示す値を取得します。
        /// </summary>
        public bool IsSuccess { get; }

        /// <summary>
        /// 計算結果の値を取得します。
        /// この値は <see cref="IsSuccess"/> が true の場合のみ有効です。
        /// </summary>
        public int Value { get; }

        /// <summary>
        /// 演算が失敗した場合のエラーコードを取得します。
        /// 0 (CALC_SUCCESS) は成功を、-1 (CALC_ERROR) は失敗を示します。
        /// </summary>
        public int ErrorCode { get; }

        /// <summary>
        /// <see cref="CalcResult"/> クラスの新しいインスタンスを初期化します。
        /// </summary>
        /// <param name="isSuccess">演算が成功したかどうかを示します。</param>
        /// <param name="value">計算結果の値。</param>
        /// <param name="errorCode">ネイティブライブラリから返されたエラーコード。</param>
        internal CalcResult(bool isSuccess, int value, int errorCode)
        {
            IsSuccess = isSuccess;
            Value = value;
            ErrorCode = errorCode;
        }
    }
}
