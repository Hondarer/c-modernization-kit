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

using Xunit;
using CalcDotNetLib;

namespace CalcDotNetLib.Tests
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
            // Act
            var result = CalcLibrary.Add(a, b);

            // Assert
            Assert.True(result.IsSuccess);
            Assert.Equal(expected, result.Value);
            Assert.Equal(0, result.ErrorCode);
        }

        [Fact]
        public void Add_UsingCalculate_ShouldReturnCorrectResult()
        {
            // Act
            var result = CalcLibrary.Calculate(CalcKind.Add, 15, 25);

            // Assert
            Assert.True(result.IsSuccess);
            Assert.Equal(40, result.Value);
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
            // Act
            var result = CalcLibrary.Subtract(a, b);

            // Assert
            Assert.True(result.IsSuccess);
            Assert.Equal(expected, result.Value);
            Assert.Equal(0, result.ErrorCode);
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
            // Act
            var result = CalcLibrary.Multiply(a, b);

            // Assert
            Assert.True(result.IsSuccess);
            Assert.Equal(expected, result.Value);
            Assert.Equal(0, result.ErrorCode);
        }

        #endregion

        #region Division Tests

        [Theory]
        [InlineData(20, 4, 5)]
        [InlineData(10, 3, 3)]  // Integer division
        [InlineData(-9, 3, -3)]
        [InlineData(0, 5, 0)]
        [InlineData(100, 10, 10)]
        public void Divide_ShouldReturnCorrectResult(int a, int b, int expected)
        {
            // Act
            var result = CalcLibrary.Divide(a, b);

            // Assert
            Assert.True(result.IsSuccess);
            Assert.Equal(expected, result.Value);
            Assert.Equal(0, result.ErrorCode);
        }

        [Fact]
        public void Divide_ByZero_ShouldReturnError()
        {
            // Act
            var result = CalcLibrary.Divide(10, 0);

            // Assert
            Assert.False(result.IsSuccess);
            Assert.Equal(-1, result.ErrorCode);
        }

        [Fact]
        public void Divide_ByZero_WithZeroDividend_ShouldReturnError()
        {
            // Act
            var result = CalcLibrary.Divide(0, 0);

            // Assert
            Assert.False(result.IsSuccess);
            Assert.Equal(-1, result.ErrorCode);
        }

        #endregion

        #region CalculateOrThrow Tests

        [Fact]
        public void CalculateOrThrow_Add_Success_ShouldReturnValue()
        {
            // Act
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Add, 5, 3);

            // Assert
            Assert.Equal(8, result);
        }

        [Fact]
        public void CalculateOrThrow_Subtract_Success_ShouldReturnValue()
        {
            // Act
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Subtract, 10, 4);

            // Assert
            Assert.Equal(6, result);
        }

        [Fact]
        public void CalculateOrThrow_Multiply_Success_ShouldReturnValue()
        {
            // Act
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Multiply, 6, 7);

            // Assert
            Assert.Equal(42, result);
        }

        [Fact]
        public void CalculateOrThrow_Divide_Success_ShouldReturnValue()
        {
            // Act
            int result = CalcLibrary.CalculateOrThrow(CalcKind.Divide, 20, 5);

            // Assert
            Assert.Equal(4, result);
        }

        [Fact]
        public void CalculateOrThrow_DivideByZero_ShouldThrowException()
        {
            // Act & Assert
            var exception = Assert.Throws<CalcException>(() =>
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0));

            Assert.Equal(-1, exception.ErrorCode);
            Assert.Contains("Calculation failed", exception.Message);
            Assert.Contains("kind=Divide", exception.Message);
            Assert.Contains("a=10", exception.Message);
            Assert.Contains("b=0", exception.Message);
        }

        #endregion

        #region Edge Cases

        [Theory]
        [InlineData(int.MaxValue, 0, int.MaxValue)]
        [InlineData(int.MinValue, 0, int.MinValue)]
        public void Add_WithExtremeValues_ShouldWork(int a, int b, int expected)
        {
            // Act
            var result = CalcLibrary.Add(a, b);

            // Assert
            Assert.True(result.IsSuccess);
            Assert.Equal(expected, result.Value);
        }

        [Fact]
        public void Multiply_ByZero_ShouldReturnZero()
        {
            // Act
            var result = CalcLibrary.Multiply(12345, 0);

            // Assert
            Assert.True(result.IsSuccess);
            Assert.Equal(0, result.Value);
        }

        [Fact]
        public void Subtract_SameValues_ShouldReturnZero()
        {
            // Act
            var result = CalcLibrary.Subtract(42, 42);

            // Assert
            Assert.True(result.IsSuccess);
            Assert.Equal(0, result.Value);
        }

        #endregion
    }
}
