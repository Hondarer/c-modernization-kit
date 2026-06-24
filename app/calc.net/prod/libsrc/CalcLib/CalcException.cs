#pragma warning disable 1587
/**
 *******************************************************************************
 *  @file           CalcException.cs
 *  @brief          ネイティブ calc ライブラリの計算エラーを表すカスタム例外を定義します。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */
#pragma warning restore 1587

using System;

namespace CalcLib
{
    /// <summary>
    /// 計算演算が失敗した際にスローされる例外。
    /// </summary>
    public class CalcException : Exception
    {
        /// <summary>
        /// ネイティブ ライブラリから返されたエラー コードを取得します。
        /// </summary>
        public int ErrorCode { get; }

        /// <summary>
        /// <see cref="CalcException"/> クラスの新しいインスタンスを初期化します。
        /// </summary>
        /// <param name="errorCode">ネイティブ ライブラリから返されたエラー コード。</param>
        /// <param name="message">失敗を説明するエラー メッセージ。</param>
        public CalcException(int errorCode, string message)
            : base(message)
        {
            ErrorCode = errorCode;
        }

        /// <summary>
        /// <see cref="CalcException"/> クラスの新しいインスタンスを初期化します。
        /// </summary>
        /// <param name="errorCode">ネイティブ ライブラリから返されたエラー コード。</param>
        /// <param name="message">失敗を説明するエラー メッセージ。</param>
        /// <param name="innerException">この例外の原因となった例外。</param>
        public CalcException(int errorCode, string message, Exception innerException)
            : base(message, innerException)
        {
            ErrorCode = errorCode;
        }
    }
}
