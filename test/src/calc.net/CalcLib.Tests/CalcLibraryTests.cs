#pragma warning disable 1587
/**
 *******************************************************************************
 *  @file           CalcLibraryTests.cs
 *  @brief          CalcLibrary の単体テスト。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
 *
 *  CalcLibrary クラスの包括的な単体テストを含み、
 *  すべての計算演算とエラー処理をテストします。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */
#pragma warning restore 1587

using Xunit;
using CalcLib;

namespace CalcLib.Tests
{
    /// <summary>
    /// CalcLibrary クラスの単体テスト。
    /// </summary>
    public class CalcLibraryTests
    {
        #region Addition Tests

        [Theory]
        [InlineData(10, 20, 30)]
        [InlineData(-5, 5, 0)]
        [InlineData(0, 0, 0)]
        [InlineData(100, -50, 50)]
        [InlineData(-10, -20, -30)]
        public void Add_ShouldReturnCorrectResult(int a, int b, int expected)
        {
            var result = CalcLibrary.Add(a, b); // [手順] - CalcLibrary.Add(a, b) を呼び出す。

            Assert.True(result.IsSuccess); // [確認] - 結果が成功であること。
            Assert.Equal(expected, result.Value); // [確認] - 期待値と一致すること。
            Assert.Equal(0, result.ErrorCode); // [確認] - エラーコードが 0 であること。
        }

        [Fact]
        public void Add_UsingCalculate_ShouldReturnCorrectResult()
        {
            var result = CalcLibrary.Calculate(CalcKind.Add, 15, 25); // [手順] - CalcLibrary.Calculate(CalcKind.Add, 15, 25) を呼び出す。

            Assert.True(result.IsSuccess); // [確認] - 結果が成功であること。
            Assert.Equal(40, result.Value); // [確認] - 期待値 40 と一致すること。
        }

        #endregion

        #region Subtraction Tests

        [Theory]
        [InlineData(20, 10, 10)]
        [InlineData(5, -5, 10)]
        [InlineData(0, 0, 0)]
        [InlineData(-10, -5, -5)]
        [InlineData(100, 150, -50)]
        public void Subtract_ShouldReturnCorrectResult(int a, int b, int expected)
        {
            var result = CalcLibrary.Subtract(a, b); // [手順] - CalcLibrary.Subtract(a, b) を呼び出す。

            Assert.True(result.IsSuccess); // [確認] - 結果が成功であること。
            Assert.Equal(expected, result.Value); // [確認] - 期待値と一致すること。
            Assert.Equal(0, result.ErrorCode); // [確認] - エラーコードが 0 であること。
        }

        #endregion

        #region Multiplication Tests

        [Theory]
        [InlineData(5, 4, 20)]
        [InlineData(-3, 3, -9)]
        [InlineData(0, 100, 0)]
        [InlineData(-5, -5, 25)]
        [InlineData(7, 6, 42)]
        public void Multiply_ShouldReturnCorrectResult(int a, int b, int expected)
        {
            var result = CalcLibrary.Multiply(a, b); // [手順] - CalcLibrary.Multiply(a, b) を呼び出す。

            Assert.True(result.IsSuccess); // [確認] - 結果が成功であること。
            Assert.Equal(expected, result.Value); // [確認] - 期待値と一致すること。
            Assert.Equal(0, result.ErrorCode); // [確認] - エラーコードが 0 であること。
        }

        #endregion

        #region Division Tests

        [Theory]
        [InlineData(20, 4, 5)]
        [InlineData(10, 3, 3)]  // 整数除算
        [InlineData(-9, 3, -3)]
        [InlineData(0, 5, 0)]
        [InlineData(100, 10, 10)]
        public void Divide_ShouldReturnCorrectResult(int a, int b, int expected)
        {
            var result = CalcLibrary.Divide(a, b); // [手順] - CalcLibrary.Divide(a, b) を呼び出す。

            Assert.True(result.IsSuccess); // [確認] - 結果が成功であること。
            Assert.Equal(expected, result.Value); // [確認] - 期待値と一致すること。
            Assert.Equal(0, result.ErrorCode); // [確認] - エラーコードが 0 であること。
        }

        [Fact]
        public void Divide_ByZero_ShouldReturnError()
        {
            var result = CalcLibrary.Divide(10, 0); // [手順] - CalcLibrary.Divide(10, 0) を呼び出す (ゼロ除算)。

            Assert.False(result.IsSuccess); // [確認] - 結果が失敗であること。
            Assert.Equal(-1, result.ErrorCode); // [確認] - エラーコードが -1 であること。
        }

        [Fact]
        public void Divide_ByZero_WithZeroDividend_ShouldReturnError()
        {
            var result = CalcLibrary.Divide(0, 0); // [手順] - CalcLibrary.Divide(0, 0) を呼び出す (ゼロ除算)。

            Assert.False(result.IsSuccess); // [確認] - 結果が失敗であること。
            Assert.Equal(-1, result.ErrorCode); // [確認] - エラーコードが -1 であること。
        }

        #endregion

        #region CalculateOrThrow Tests

        [Fact]
        public void CalculateOrThrow_Add_Success_ShouldReturnValue()
        {
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Add, 5, 3); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Add, 5, 3) を呼び出す。

            Assert.Equal(8, result); // [確認] - 結果が 8 であること。
        }

        [Fact]
        public void CalculateOrThrow_Subtract_Success_ShouldReturnValue()
        {
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Subtract, 10, 4); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Subtract, 10, 4) を呼び出す。

            Assert.Equal(6, result); // [確認] - 結果が 6 であること。
        }

        [Fact]
        public void CalculateOrThrow_Multiply_Success_ShouldReturnValue()
        {
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Multiply, 6, 7); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Multiply, 6, 7) を呼び出す。

            Assert.Equal(42, result); // [確認] - 結果が 42 であること。
        }

        [Fact]
        public void CalculateOrThrow_Divide_Success_ShouldReturnValue()
        {
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Divide, 20, 5); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Divide, 20, 5) を呼び出す。

            Assert.Equal(4, result); // [確認] - 結果が 4 であること。
        }

        [Fact]
        public void CalculateOrThrow_DivideByZero_ShouldThrowException()
        {
            var exception = Assert.Throws<CalcException>(() => // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0) を呼び出す (ゼロ除算)。
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0)); // [確認] - CalcException が発生すること。

            Assert.Equal(-1, exception.ErrorCode); // [確認] - 例外のエラーコードが -1 であること。
            Assert.Contains("Calculation failed", exception.Message); // [確認] - メッセージに "Calculation failed" が含まれること。
            Assert.Contains("kind=Divide", exception.Message); // [確認] - メッセージに "kind=Divide" が含まれること。
            Assert.Contains("a=10", exception.Message); // [確認] - メッセージに "a=10" が含まれること。
            Assert.Contains("b=0", exception.Message); // [確認] - メッセージに "b=0" が含まれること。
        }

        #endregion

        #region Edge Cases

        [Theory]
        [InlineData(int.MaxValue, 0, int.MaxValue)]
        [InlineData(int.MinValue, 0, int.MinValue)]
        public void Add_WithExtremeValues_ShouldWork(int a, int b, int expected)
        {
            var result = CalcLibrary.Add(a, b); // [手順] - CalcLibrary.Add(a, b) を呼び出す (極端な値)。

            Assert.True(result.IsSuccess); // [確認] - 結果が成功であること。
            Assert.Equal(expected, result.Value); // [確認] - 期待値と一致すること。
        }

        [Fact]
        public void Multiply_ByZero_ShouldReturnZero()
        {
            var result = CalcLibrary.Multiply(12345, 0); // [手順] - CalcLibrary.Multiply(12345, 0) を呼び出す (ゼロ乗算)。

            Assert.True(result.IsSuccess); // [確認] - 結果が成功であること。
            Assert.Equal(0, result.Value); // [確認] - 値が 0 であること。
        }

        [Fact]
        public void Subtract_SameValues_ShouldReturnZero()
        {
            var result = CalcLibrary.Subtract(42, 42); // [手順] - CalcLibrary.Subtract(42, 42) を呼び出す (同じ値)。

            Assert.True(result.IsSuccess); // [確認] - 結果が成功であること。
            Assert.Equal(0, result.Value); // [確認] - 値が 0 であること。
        }

        #endregion
    }
}
