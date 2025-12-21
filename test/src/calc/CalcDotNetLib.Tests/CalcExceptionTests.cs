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

using System;
using Xunit;
using CalcDotNetLib;

namespace CalcDotNetLib.Tests
{
    /// <summary>
    /// CalcException クラスの単体テスト。
    /// </summary>
    public class CalcExceptionTests
    {
        [Fact]
        public void CalcException_WithErrorCodeAndMessage_ShouldSetProperties()
        {
            // Arrange
            int errorCode = -1;
            string message = "Test error message";

            // Act
            var exception = new CalcException(errorCode, message);

            // Assert
            Assert.Equal(errorCode, exception.ErrorCode);
            Assert.Equal(message, exception.Message);
        }

        [Fact]
        public void CalcException_WithInnerException_ShouldSetProperties()
        {
            // Arrange
            int errorCode = -1;
            string message = "Test error message";
            var innerException = new InvalidOperationException("Inner exception");

            // Act
            var exception = new CalcException(errorCode, message, innerException);

            // Assert
            Assert.Equal(errorCode, exception.ErrorCode);
            Assert.Equal(message, exception.Message);
            Assert.Same(innerException, exception.InnerException);
        }

        [Fact]
        public void CalcException_ThrownFromCalculateOrThrow_ShouldContainContextInfo()
        {
            // Act & Assert
            var exception = Assert.Throws<CalcException>(() =>
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 100, 0));

            // Verify exception contains context information
            Assert.Equal(-1, exception.ErrorCode);
            Assert.Contains("kind=Divide", exception.Message);
            Assert.Contains("a=100", exception.Message);
            Assert.Contains("b=0", exception.Message);
            Assert.Contains("errorCode=-1", exception.Message);
        }

        [Fact]
        public void CalcException_CanBeCaught_AsException()
        {
            // Arrange
            bool caughtAsException = false;

            // Act
            try
            {
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0);
            }
            catch (Exception ex)
            {
                caughtAsException = true;
                Assert.IsType<CalcException>(ex);
            }

            // Assert
            Assert.True(caughtAsException);
        }

        [Fact]
        public void CalcException_CanBeCaught_AsCalcException()
        {
            // Arrange
            bool caughtAsCalcException = false;

            // Act
            try
            {
                CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0);
            }
            catch (CalcException ex)
            {
                caughtAsCalcException = true;
                Assert.Equal(-1, ex.ErrorCode);
            }

            // Assert
            Assert.True(caughtAsCalcException);
        }
    }
}
