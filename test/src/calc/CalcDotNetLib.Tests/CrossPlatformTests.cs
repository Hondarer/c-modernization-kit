/**
 *******************************************************************************
 *  @file           CrossPlatformTests.cs
 *  @brief          クロスプラットフォーム互換性テスト。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
 *
 *  クロスプラットフォーム互換性と
 *  プラットフォーム検出機能を検証するテストを含みます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

using System;
using System.Runtime.InteropServices;
using Xunit;
using CalcDotNetLib;

namespace CalcDotNetLib.Tests
{
    /// <summary>
    /// クロスプラットフォーム互換性テスト。
    /// </summary>
    public class CrossPlatformTests
    {
        [Fact]
        public void CurrentPlatform_ShouldBeSupportedPlatform()
        {
            // Act
            bool isWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
            bool isLinux = RuntimeInformation.IsOSPlatform(OSPlatform.Linux);

            // Assert - we support Windows or Linux
            Assert.True(isWindows || isLinux, "Platform should be Windows or Linux");
        }

        [Fact]
        public void CalcLibrary_ShouldWork_OnCurrentPlatform()
        {
            // This test verifies that the library can be loaded and called
            // on the current platform (Windows or Linux)

            // Act
            var addResult = CalcLibrary.Add(10, 20);
            var subtractResult = CalcLibrary.Subtract(30, 15);
            var multiplyResult = CalcLibrary.Multiply(5, 6);
            var divideResult = CalcLibrary.Divide(100, 4);

            // Assert
            Assert.True(addResult.IsSuccess);
            Assert.Equal(30, addResult.Value);

            Assert.True(subtractResult.IsSuccess);
            Assert.Equal(15, subtractResult.Value);

            Assert.True(multiplyResult.IsSuccess);
            Assert.Equal(30, multiplyResult.Value);

            Assert.True(divideResult.IsSuccess);
            Assert.Equal(25, divideResult.Value);
        }


        [Fact]
        public void NativeLibrary_ShouldBeAccessible()
        {
            // This test verifies that the native library can be accessed
            // and that P/Invoke works correctly on the current platform

            // Act & Assert - if the library cannot be loaded, this will throw DllNotFoundException
            var exception = Record.Exception(() =>
            {
                var result = CalcLibrary.Add(1, 1);
                Assert.True(result.IsSuccess);
            });

            Assert.Null(exception);
        }
    }
}
