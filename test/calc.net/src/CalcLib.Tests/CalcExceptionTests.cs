#pragma warning disable 1587
/**
 *******************************************************************************
 *  @file           CalcExceptionTests.cs
 *  @brief          CalcException の単体テスト。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
 *
 *  CalcException クラスの単体テストを含み、
 *  例外のプロパティと動作を検証します。
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
        [Fact]
        public void CalcException_WithErrorCodeAndMessage_ShouldSetProperties()
        {
            int errorCode = -1; // [状態] - エラーコードを準備する。
            string message = "Test error message"; // [状態] - メッセージを準備する。

            var exception = new CalcException(errorCode, message); // [手順] - CalcException を作成する。

            Assert.Equal(errorCode, exception.ErrorCode); // [確認] - エラーコードが正しく設定されていること。
            Assert.Equal(message, exception.Message); // [確認] - メッセージが正しく設定されていること。
        }

        [Fact]
        public void CalcException_WithInnerException_ShouldSetProperties()
        {
            int errorCode = -1; // [状態] - エラーコードを準備する。
            string message = "Test error message"; // [状態] - メッセージを準備する。
            var innerException = new InvalidOperationException("Inner exception"); // [状態] - 内部例外を準備する。

            var exception = new CalcException(errorCode, message, innerException); // [手順] - CalcException を内部例外付きで作成する。

            Assert.Equal(errorCode, exception.ErrorCode); // [確認] - エラーコードが正しく設定されていること。
            Assert.Equal(message, exception.Message); // [確認] - メッセージが正しく設定されていること。
            Assert.Same(innerException, exception.InnerException); // [確認] - 内部例外が正しく設定されていること。
        }

        [Fact]
        public void CalcException_ThrownFromCalculateOrThrow_ShouldContainContextInfo()
        {
            var exception = Assert.Throws<CalcException>(() => // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Divide, 100, 0) を呼び出す (ゼロ除算)。
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 100, 0)); // [確認] - CalcException が発生すること。

            Assert.Equal(-1, exception.ErrorCode); // [確認] - 例外のエラーコードが -1 であること。
            Assert.Contains("kind=Divide", exception.Message); // [確認] - メッセージに "kind=Divide" が含まれること。
            Assert.Contains("a=100", exception.Message); // [確認] - メッセージに "a=100" が含まれること。
            Assert.Contains("b=0", exception.Message); // [確認] - メッセージに "b=0" が含まれること。
            Assert.Contains("errorCode=-1", exception.Message); // [確認] - メッセージに "errorCode=-1" が含まれること。
        }

        [Fact]
        public void CalcException_CanBeCaught_AsException()
        {
            bool caughtAsException = false; // [状態] - 例外キャッチフラグを準備する。

            try
            {
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0) を呼び出す。
            }
            catch (Exception ex)
            {
                caughtAsException = true;
                Assert.IsType<CalcException>(ex); // [確認] - CalcException 型であること。
            }

            Assert.True(caughtAsException); // [確認] - Exception としてキャッチできたこと。
        }

        [Fact]
        public void CalcException_CanBeCaught_AsCalcException()
        {
            bool caughtAsCalcException = false; // [状態] - 例外キャッチフラグを準備する。

            try
            {
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0) を呼び出す。
            }
            catch (CalcException ex)
            {
                caughtAsCalcException = true;
                Assert.Equal(-1, ex.ErrorCode); // [確認] - エラーコードが -1 であること。
            }

            Assert.True(caughtAsCalcException); // [確認] - CalcException としてキャッチできたこと。
        }
    }
}
