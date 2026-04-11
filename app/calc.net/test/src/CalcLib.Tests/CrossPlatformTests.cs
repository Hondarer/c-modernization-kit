#pragma warning disable 1587
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
#pragma warning restore 1587

using System;
using System.Runtime.InteropServices;
using Xunit;
using CalcLib;

namespace CalcLib.Tests
{
    /// <summary>
    /// クロスプラットフォーム互換性テスト。
    /// </summary>
    public class CrossPlatformTests
    {
        [Fact]
        public void CurrentPlatform_ShouldBeSupportedPlatform()
        {
            bool isWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows); // [手順] - Windows プラットフォームかどうか確認する。
            bool isLinux = RuntimeInformation.IsOSPlatform(OSPlatform.Linux); // [手順] - Linux プラットフォームかどうか確認する。

            Assert.True(isWindows || isLinux, "Platform should be Windows or Linux"); // [確認] - Windows または Linux であること。
        }

        [Fact]
        public void CalcLibrary_ShouldWork_OnCurrentPlatform()
        {
            // このテストは、現在のプラットフォーム (Windows または Linux) で
            // ライブラリがロードされ、呼び出せることを検証します。

            var addResult = CalcLibrary.Add(10, 20); // [手順] - CalcLibrary.Add(10, 20) を呼び出す。
            var subtractResult = CalcLibrary.Subtract(30, 15); // [手順] - CalcLibrary.Subtract(30, 15) を呼び出す。
            var multiplyResult = CalcLibrary.Multiply(5, 6); // [手順] - CalcLibrary.Multiply(5, 6) を呼び出す。
            var divideResult = CalcLibrary.Divide(100, 4); // [手順] - CalcLibrary.Divide(100, 4) を呼び出す。

            Assert.True(addResult.IsSuccess); // [確認] - 加算の結果が成功であること。
            Assert.Equal(30, addResult.Value); // [確認] - 加算の結果が 30 であること。

            Assert.True(subtractResult.IsSuccess); // [確認] - 減算の結果が成功であること。
            Assert.Equal(15, subtractResult.Value); // [確認] - 減算の結果が 15 であること。

            Assert.True(multiplyResult.IsSuccess); // [確認] - 乗算の結果が成功であること。
            Assert.Equal(30, multiplyResult.Value); // [確認] - 乗算の結果が 30 であること。

            Assert.True(divideResult.IsSuccess); // [確認] - 除算の結果が成功であること。
            Assert.Equal(25, divideResult.Value); // [確認] - 除算の結果が 25 であること。
        }


        [Fact]
        public void NativeLibrary_ShouldBeAccessible()
        {
            // このテストは、ネイティブライブラリにアクセスでき、
            // 現在のプラットフォームで P/Invoke が正しく機能することを検証します。

            var exception = Record.Exception(() => // [手順] - CalcLibrary.Add(1, 1) を呼び出し、例外をキャプチャする。
            {
                var result = CalcLibrary.Add(1, 1); // [手順] - CalcLibrary.Add(1, 1) を呼び出す。
                Assert.True(result.IsSuccess); // [確認] - 結果が成功であること。
            });

            Assert.Null(exception); // [確認] - 例外が発生しないこと。
        }
    }
}
