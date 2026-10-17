#pragma warning disable 1587
/**
 *******************************************************************************
 *  @file           CalcExceptionTests.cs
 *  @brief          CalcException のプロパティと動作を検証する単体テストを実装します。
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
using Xunit;
using CalcLib;

namespace CalcLib.Tests
{
    /// <summary>
    /// CalcException クラスの単体テスト。
    /// </summary>
    public class CalcExceptionTests
    {
        // エラー コードとメッセージを指定して CalcException を生成した場合に各プロパティが正しく保持されることの確認
        [Fact]
        public void CalcException_WithErrorCodeAndMessage_ShouldSetProperties()
        {
            // Arrange
            int errorCode = -1; // [状態] - エラー コードを準備する。
            string message = "Test error message"; // [状態] - メッセージを準備する。

            // Act
            var exception = new CalcException(errorCode, message); // [手順] - CalcException を作成する。

            // Assert
            Assert.Equal(errorCode, exception.ErrorCode); // [確認_正常系] - exception.ErrorCode が errorCode と一致すること。
            Assert.Equal(message, exception.Message); // [確認_正常系] - exception.Message が message と一致すること。
        }

        // 内部例外を指定して CalcException を生成した場合に各プロパティおよび内部例外が正しく保持されることの確認
        [Fact]
        public void CalcException_WithInnerException_ShouldSetProperties()
        {
            // Arrange
            int errorCode = -1; // [状態] - エラー コードを準備する。
            string message = "Test error message"; // [状態] - メッセージを準備する。
            var innerException = new InvalidOperationException("Inner exception"); // [状態] - 内部例外を準備する。

            // Act
            var exception = new CalcException(errorCode, message, innerException); // [手順] - CalcException を内部例外付きで作成する。

            // Assert
            Assert.Equal(errorCode, exception.ErrorCode); // [確認_正常系] - exception.ErrorCode が errorCode と一致すること。
            Assert.Equal(message, exception.Message); // [確認_正常系] - exception.Message が message と一致すること。
            Assert.Same(innerException, exception.InnerException); // [確認_正常系] - exception.InnerException が innerException と同じインスタンスであること。
        }

        // CalculateOrThrow でゼロ除算が発生した際にスローされる CalcException に詳細なコンテキスト情報が含まれることの確認
        [Fact]
        public void CalcException_ThrownFromCalculateOrThrow_ShouldContainContextInfo()
        {
            // Act
            var thrown = Record.Exception(
                () =>
                {
                    CalcLibrary.CalculateOrThrow(
                        CalcKind.Divide, 100,
                        0); // [手順] - ゼロ除算となる 100 と 0 を指定して CalcLibrary.CalculateOrThrow() を呼び出す。
                });

            // Assert
            var exception = Assert.IsType<CalcException>(thrown); // [確認_異常系] - thrown の型が CalcException であること。(ゼロ除算で CalcLibrary.CalculateOrThrow() が CalcException を送出したことの確認)
            Assert.Equal(CalcLibrary.CALC_ERR_INVALID_ARGUMENT,
                         exception.ErrorCode); // [確認_異常系] - exception.ErrorCode が CalcLibrary.CALC_ERR_INVALID_ARGUMENT であること。
            Assert.Contains("kind=Divide", exception.Message); // [確認_異常系] - exception.Message が "kind=Divide" を含むこと。
            Assert.Contains("a=100", exception.Message); // [確認_異常系] - exception.Message が "a=100" を含むこと。
            Assert.Contains("b=0", exception.Message); // [確認_異常系] - exception.Message が "b=0" を含むこと。
            Assert.Contains("errorCode=-2", exception.Message); // [確認_異常系] - exception.Message が "errorCode=-2" を含むこと。
        }

        // スローされた CalcException を基底の Exception 型として正常に捕捉できることの確認
        [Fact]
        public void CalcException_CanBeCaught_AsException()
        {
            // Arrange
            bool caughtAsException = false; // [状態] - 例外キャッチ フラグを準備する。
            Exception ex = null;

            // Act
            try
            {
                CalcLibrary.CalculateOrThrow(
                    CalcKind.Divide, 10,
                    0); // [手順] - ゼロ除算となる 10 と 0 を指定して CalcLibrary.CalculateOrThrow() を呼び出す。
            }
            catch (Exception caught)
            {
                caughtAsException = true;
                ex = caught;
            }

            // Assert
            Assert.True(caughtAsException); // [確認_異常系] - caughtAsException が true であること。(Exception として捕捉できたことの確認)
            Assert.IsType<CalcException>(ex); // [確認_異常系] - ex の型が CalcException であること。
        }

        // スローされた CalcException を専用の CalcException 型として正常に捕捉しエラー コードを取得できることの確認
        [Fact]
        public void CalcException_CanBeCaught_AsCalcException()
        {
            // Arrange
            bool caughtAsCalcException = false; // [状態] - 例外キャッチ フラグを準備する。
            CalcException ex = null;

            // Act
            try
            {
                CalcLibrary.CalculateOrThrow(
                    CalcKind.Divide, 10,
                    0); // [手順] - ゼロ除算となる 10 と 0 を指定して CalcLibrary.CalculateOrThrow() を呼び出す。
            }
            catch (CalcException caught)
            {
                caughtAsCalcException = true;
                ex = caught;
            }

            // Assert
            Assert.True(caughtAsCalcException); // [確認_異常系] - caughtAsCalcException が true であること。(CalcException として捕捉できたことの確認)
            Assert.Equal(CalcLibrary.CALC_ERR_INVALID_ARGUMENT,
                         ex.ErrorCode); // [確認_異常系] - ex.ErrorCode が CalcLibrary.CALC_ERR_INVALID_ARGUMENT であること。
        }
    }
}
