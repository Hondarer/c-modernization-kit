#pragma warning disable 1587
/**
 *******************************************************************************
 *  @file           CalcLibraryTests.cs
 *  @brief          CalcLibrary の計算演算とエラー処理を検証する単体テストを実装します。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
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

        // 加算演算において正しい計算結果と成功ステータスが返されることの確認
        [Theory]
        [InlineData(10, 20, 30)]
        [InlineData(-5, 5, 0)]
        [InlineData(0, 0, 0)]
        [InlineData(100, -50, 50)]
        [InlineData(-10, -20, -30)]
        public void Add_ShouldReturnCorrectResult(int a, int b, int expected)
        {
            // Act
            var result = CalcLibrary.Add(a, b); // [手順] - CalcLibrary.Add(a, b) を呼び出す。

            // Assert
            Assert.True(result.IsSuccess); // [確認_正常系 回数=PARAM] - result.IsSuccess が true であること。
            Assert.Equal(expected, result.Value); // [確認_正常系 回数=PARAM] - result.Value が expected と一致すること。
            Assert.Equal(0, result.ErrorCode); // [確認_正常系 回数=PARAM] - result.ErrorCode が 0 であること。
        }

        // Calculate メソッドで加算種別を指定した場合に正しい計算結果が返されることの確認
        [Fact]
        public void Add_UsingCalculate_ShouldReturnCorrectResult()
        {
            // Act
            var result = CalcLibrary.Calculate(CalcKind.Add, 15, 25); // [手順] - CalcLibrary.Calculate(CalcKind.Add, 15, 25) を呼び出す。

            // Assert
            Assert.True(result.IsSuccess); // [確認_正常系] - result.IsSuccess が true であること。
            Assert.Equal(40, result.Value); // [確認_正常系] - result.Value が 40 であること。(15 と 25 の和であることの確認)
        }

        #endregion

        #region Subtraction Tests

        // 減算演算において正しい計算結果と成功ステータスが返されることの確認
        [Theory]
        [InlineData(20, 10, 10)]
        [InlineData(5, -5, 10)]
        [InlineData(0, 0, 0)]
        [InlineData(-10, -5, -5)]
        [InlineData(100, 150, -50)]
        public void Subtract_ShouldReturnCorrectResult(int a, int b, int expected)
        {
            // Act
            var result = CalcLibrary.Subtract(a, b); // [手順] - CalcLibrary.Subtract(a, b) を呼び出す。

            // Assert
            Assert.True(result.IsSuccess); // [確認_正常系 回数=PARAM] - result.IsSuccess が true であること。
            Assert.Equal(expected, result.Value); // [確認_正常系 回数=PARAM] - result.Value が expected と一致すること。
            Assert.Equal(0, result.ErrorCode); // [確認_正常系 回数=PARAM] - result.ErrorCode が 0 であること。
        }

        #endregion

        #region Multiplication Tests

        // 乗算演算において正しい計算結果と成功ステータスが返されることの確認
        [Theory]
        [InlineData(5, 4, 20)]
        [InlineData(-3, 3, -9)]
        [InlineData(0, 100, 0)]
        [InlineData(-5, -5, 25)]
        [InlineData(7, 6, 42)]
        public void Multiply_ShouldReturnCorrectResult(int a, int b, int expected)
        {
            // Act
            var result = CalcLibrary.Multiply(a, b); // [手順] - CalcLibrary.Multiply(a, b) を呼び出す。

            // Assert
            Assert.True(result.IsSuccess); // [確認_正常系 回数=PARAM] - result.IsSuccess が true であること。
            Assert.Equal(expected, result.Value); // [確認_正常系 回数=PARAM] - result.Value が expected と一致すること。
            Assert.Equal(0, result.ErrorCode); // [確認_正常系 回数=PARAM] - result.ErrorCode が 0 であること。
        }

        #endregion

        #region Division Tests

        // 除算演算において正しい商と成功ステータスが返されることの確認
        [Theory]
        [InlineData(20, 4, 5)]
        [InlineData(10, 3, 3)]  // 整数除算
        [InlineData(-9, 3, -3)]
        [InlineData(0, 5, 0)]
        [InlineData(100, 10, 10)]
        public void Divide_ShouldReturnCorrectResult(int a, int b, int expected)
        {
            // Act
            var result = CalcLibrary.Divide(a, b); // [手順] - CalcLibrary.Divide(a, b) を呼び出す。

            // Assert
            Assert.True(result.IsSuccess); // [確認_正常系 回数=PARAM] - result.IsSuccess が true であること。
            Assert.Equal(expected, result.Value); // [確認_正常系 回数=PARAM] - result.Value が expected と一致すること。
            Assert.Equal(0, result.ErrorCode); // [確認_正常系 回数=PARAM] - result.ErrorCode が 0 であること。
        }

        // 除数に 0 を指定した場合に除算が失敗しエラー コードが返されることの確認
        [Fact]
        public void Divide_ByZero_ShouldReturnError()
        {
            // Act
            var result = CalcLibrary.Divide(10, 0); // [手順] - CalcLibrary.Divide(10, 0) を呼び出す (ゼロ除算)。

            // Assert
            Assert.False(result.IsSuccess); // [確認_異常系] - 除数 0 の除算での result.IsSuccess が false であること。
            Assert.Equal(CalcLibrary.CALC_ERR_INVALID_ARGUMENT,
                         result.ErrorCode); // [確認_異常系] - result.ErrorCode が CalcLibrary.CALC_ERR_INVALID_ARGUMENT であること。
        }

        // 被除数と除数の両方に 0 を指定した場合に除算が失敗しエラー コードが返されることの確認
        [Fact]
        public void Divide_ByZero_WithZeroDividend_ShouldReturnError()
        {
            // Act
            var result = CalcLibrary.Divide(0, 0); // [手順] - CalcLibrary.Divide(0, 0) を呼び出す (ゼロ除算)。

            // Assert
            Assert.False(result.IsSuccess); // [確認_異常系] - 被除数と除数がともに 0 の除算での result.IsSuccess が false であること。
            Assert.Equal(CalcLibrary.CALC_ERR_INVALID_ARGUMENT,
                         result.ErrorCode); // [確認_異常系] - result.ErrorCode が CalcLibrary.CALC_ERR_INVALID_ARGUMENT であること。
        }

        #endregion

        #region CalculateOrThrow Tests

        // CalculateOrThrow で加算演算が成功した場合に計算結果の値が直接返されることの確認
        [Fact]
        public void CalculateOrThrow_Add_Success_ShouldReturnValue()
        {
            // Act
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Add, 5, 3); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Add, 5, 3) を呼び出す。

            // Assert
            Assert.Equal(8, result); // [確認_正常系] - CalcLibrary.CalculateOrThrow() の戻り値が 8 であること。(5 と 3 の和であることの確認)
        }

        // CalculateOrThrow で減算演算が成功した場合に計算結果の値が直接返されることの確認
        [Fact]
        public void CalculateOrThrow_Subtract_Success_ShouldReturnValue()
        {
            // Act
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Subtract, 10, 4); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Subtract, 10, 4) を呼び出す。

            // Assert
            Assert.Equal(6, result); // [確認_正常系] - CalcLibrary.CalculateOrThrow() の戻り値が 6 であること。(10 と 4 の差であることの確認)
        }

        // CalculateOrThrow で乗算演算が成功した場合に計算結果の値が直接返されることの確認
        [Fact]
        public void CalculateOrThrow_Multiply_Success_ShouldReturnValue()
        {
            // Act
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Multiply, 6, 7); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Multiply, 6, 7) を呼び出す。

            // Assert
            Assert.Equal(42, result); // [確認_正常系] - CalcLibrary.CalculateOrThrow() の戻り値が 42 であること。(6 と 7 の積であることの確認)
        }

        // CalculateOrThrow で除算演算が成功した場合に計算結果の値が直接返されることの確認
        [Fact]
        public void CalculateOrThrow_Divide_Success_ShouldReturnValue()
        {
            // Act
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Divide, 20, 5); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Divide, 20, 5) を呼び出す。

            // Assert
            Assert.Equal(4, result); // [確認_正常系] - CalcLibrary.CalculateOrThrow() の戻り値が 4 であること。(20 と 5 の商であることの確認)
        }

        // CalculateOrThrow でゼロ除算が発生した際に CalcException がスローされることの確認
        [Fact]
        public void CalculateOrThrow_DivideByZero_ShouldThrowException()
        {
            // Act
            var thrown = Record.Exception(() =>
            {
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0); // [手順] - CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0) を呼び出す (ゼロ除算)。
            });

            // Assert
            var exception = Assert.IsType<CalcException>(thrown); // [確認_異常系] - thrown の型が CalcException であること。(ゼロ除算で CalcLibrary.CalculateOrThrow() が CalcException を送出したことの確認)
            Assert.Equal(CalcLibrary.CALC_ERR_INVALID_ARGUMENT,
                         exception.ErrorCode); // [確認_異常系] - exception.ErrorCode が CalcLibrary.CALC_ERR_INVALID_ARGUMENT であること。
            Assert.Contains("Calculation failed", exception.Message); // [確認_異常系] - exception.Message が "Calculation failed" を含むこと。
            Assert.Contains("kind=Divide", exception.Message); // [確認_異常系] - exception.Message が "kind=Divide" を含むこと。
            Assert.Contains("a=10", exception.Message); // [確認_異常系] - exception.Message が "a=10" を含むこと。
            Assert.Contains("b=0", exception.Message); // [確認_異常系] - exception.Message が "b=0" を含むこと。
        }

        #endregion

        #region Edge Cases

        // 境界値 (int.MaxValue, int.MinValue) を含む加算がオーバーフローすることなく正しく処理されることの確認
        [Theory]
        [InlineData(int.MaxValue, 0, int.MaxValue)]
        [InlineData(int.MinValue, 0, int.MinValue)]
        public void Add_WithExtremeValues_ShouldWork(int a, int b, int expected)
        {
            // Act
            var result = CalcLibrary.Add(a, b); // [手順] - CalcLibrary.Add(a, b) を呼び出す (極端な値)。

            // Assert
            Assert.True(result.IsSuccess); // [確認_正常系 回数=PARAM] - result.IsSuccess が true であること。
            Assert.Equal(expected, result.Value); // [確認_正常系 回数=PARAM] - result.Value が expected と一致すること。
        }

        // ゼロとの乗算結果が 0 になることの確認
        [Fact]
        public void Multiply_ByZero_ShouldReturnZero()
        {
            // Act
            var result = CalcLibrary.Multiply(12345, 0); // [手順] - CalcLibrary.Multiply(12345, 0) を呼び出す (ゼロ乗算)。

            // Assert
            Assert.True(result.IsSuccess); // [確認_正常系] - result.IsSuccess が true であること。
            Assert.Equal(0, result.Value); // [確認_正常系] - result.Value が 0 であること。(12345 と 0 の積であることの確認)
        }

        // 同じ値同士の減算結果が 0 になることの確認
        [Fact]
        public void Subtract_SameValues_ShouldReturnZero()
        {
            // Act
            var result = CalcLibrary.Subtract(42, 42); // [手順] - CalcLibrary.Subtract(42, 42) を呼び出す (同じ値)。

            // Assert
            Assert.True(result.IsSuccess); // [確認_正常系] - result.IsSuccess が true であること。
            Assert.Equal(0, result.Value); // [確認_正常系] - result.Value が 0 であること。(同じ値 42 同士の差であることの確認)
        }

        #endregion
    }
}
