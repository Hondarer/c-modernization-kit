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
            Assert.Equal(errorCode, exception.ErrorCode); // [確認_正常系] - エラー コードが正しく設定されていること。
            Assert.Equal(message, exception.Message); // [確認_正常系] - メッセージが正しく設定されていること。
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
            Assert.Equal(errorCode, exception.ErrorCode); // [確認_正常系] - エラー コードが正しく設定されていること。
            Assert.Equal(message, exception.Message); // [確認_正常系] - メッセージが正しく設定されていること。
            Assert.Same(innerException, exception.InnerException); // [確認_正常系] - 内部例外が正しく設定されていること。
        }

        // CalculateOrThrow でゼロ除算が発生した際にスローされる CalcException に詳細なコンテキスト情報が含まれることの確認
        [Fact]
        public void CalcException_ThrownFromCalculateOrThrow_ShouldContainContextInfo()
        {
            // Act
            var exception = Assert.Throws<CalcException>(() =>
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 100, 0)); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Divide, 100, 0) を呼び出す (ゼロ除算)。

            // Assert
            Assert.Equal(CalcLibrary.CALC_ERR_INVALID_ARGUMENT,
                         exception.ErrorCode); // [確認_異常系] - 例外のエラー コードが CALC_ERR_INVALID_ARGUMENT であること。
            Assert.Contains("kind=Divide", exception.Message); // [確認_異常系] - メッセージに "kind=Divide" が含まれること。
            Assert.Contains("a=100", exception.Message); // [確認_異常系] - メッセージに "a=100" が含まれること。
            Assert.Contains("b=0", exception.Message); // [確認_異常系] - メッセージに "b=0" が含まれること。
            Assert.Contains("errorCode=-2", exception.Message); // [確認_異常系] - メッセージに "errorCode=-2" が含まれること。
        }

        // スローされた CalcException を基底の Exception 型として正常に捕捉できることの確認
        [Fact]
        public void CalcException_CanBeCaught_AsException()
        {
            // Arrange
            bool caughtAsException = false; // [状態] - 例外キャッチ フラグを準備する。

            // Act
            try
            {
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0) を呼び出す。
            }
            catch (Exception ex)
            {
                caughtAsException = true;
                Assert.IsType<CalcException>(ex); // [確認_異常系] - 捕捉した例外が CalcException 型であること。
            }

            // Assert
            Assert.True(caughtAsException); // [確認_異常系] - Exception としてキャッチできたこと。
        }

        // スローされた CalcException を専用の CalcException 型として正常に捕捉しエラー コードを取得できることの確認
        [Fact]
        public void CalcException_CanBeCaught_AsCalcException()
        {
            // Arrange
            bool caughtAsCalcException = false; // [状態] - 例外キャッチ フラグを準備する。

            // Act
            try
            {
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0) を呼び出す。
            }
            catch (CalcException ex)
            {
                caughtAsCalcException = true;
                Assert.Equal(CalcLibrary.CALC_ERR_INVALID_ARGUMENT,
                             ex.ErrorCode); // [確認_異常系] - エラー コードが CALC_ERR_INVALID_ARGUMENT であること。
            }

            // Assert
            Assert.True(caughtAsCalcException); // [確認_異常系] - CalcException としてキャッチできたこと。
        }
    }
}
