/**
 *******************************************************************************
 *  @file           CalcException.cs
 *  @brief          計算エラー用のカスタム例外。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
 *
 *  ネイティブ calc ライブラリで発生する計算エラー用の
 *  カスタム例外クラスを定義します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

using System;

namespace CalcDotNet
{
    /// <summary>
    /// 計算演算が失敗した際にスローされる例外。
    /// </summary>
    public class CalcException : Exception
    {
        /// <summary>
        /// ネイティブライブラリから返されたエラーコードを取得します。
        /// </summary>
        public int ErrorCode { get; }

        /// <summary>
        /// <see cref="CalcException"/> クラスの新しいインスタンスを初期化します。
        /// </summary>
        /// <param name="errorCode">ネイティブライブラリから返されたエラーコード。</param>
        /// <param name="message">失敗を説明するエラーメッセージ。</param>
        public CalcException(int errorCode, string message)
            : base(message)
        {
            ErrorCode = errorCode;
        }

        /// <summary>
        /// <see cref="CalcException"/> クラスの新しいインスタンスを初期化します。
        /// </summary>
        /// <param name="errorCode">ネイティブライブラリから返されたエラーコード。</param>
        /// <param name="message">失敗を説明するエラーメッセージ。</param>
        /// <param name="innerException">この例外の原因となった例外。</param>
        public CalcException(int errorCode, string message, Exception innerException)
            : base(message, innerException)
        {
            ErrorCode = errorCode;
        }
    }
}
