#pragma warning disable 1587
/**
 *******************************************************************************
 *  @file           CrossPlatformTests.cs
 *  @brief          クロスプラットフォーム互換性とプラットフォーム検出機能を検証します。
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
        // 現在の実行プラットフォームがサポート対象の OS であることの確認
        [Fact]
        public void CurrentPlatform_ShouldBeSupportedPlatform()
        {
            // Act
            bool isWindows = RuntimeInformation.IsOSPlatform(
                OSPlatform
                    .Windows); // [手順] - OSPlatform.Windows を指定して RuntimeInformation.IsOSPlatform() を呼び出す。
            bool isLinux = RuntimeInformation.IsOSPlatform(
                OSPlatform
                    .Linux); // [手順] - OSPlatform.Linux を指定して RuntimeInformation.IsOSPlatform() を呼び出す。

            // Assert
            Assert.True(isWindows || isLinux, "Platform should be Windows or Linux"); // [確認_正常系] - isWindows || isLinux が true であること。(実行環境が Windows または Linux であることの確認)
        }

        // 現在のプラットフォーム上で各計算演算 (加算・減算・乗算・除算) が正常に動作することの確認
        [Fact]
        public void CalcLibrary_ShouldWork_OnCurrentPlatform()
        {
            // このテストは、現在のプラットフォーム (Windows または Linux) で
            // ライブラリが読み込まれ、呼び出せることを検証します。

            // Act_1
            var addResult = CalcLibrary.Add(10, 20); // [手順] - 10 と 20 を指定して CalcLibrary.Add() を呼び出す。

            // Assert_1
            Assert.True(addResult.IsSuccess); // [確認_正常系] - addResult.IsSuccess が true であること。
            Assert.Equal(30, addResult.Value); // [確認_正常系] - addResult.Value が 30 であること。(10 と 20 の和であることの確認)

            // Act_2
            var subtractResult =
                CalcLibrary.Subtract(30, 15); // [手順] - 30 と 15 を指定して CalcLibrary.Subtract() を呼び出す。

            // Assert_2
            Assert.True(subtractResult.IsSuccess); // [確認_正常系] - subtractResult.IsSuccess が true であること。
            Assert.Equal(15, subtractResult.Value); // [確認_正常系] - subtractResult.Value が 15 であること。(30 と 15 の差であることの確認)

            // Act_3
            var multiplyResult =
                CalcLibrary.Multiply(5, 6); // [手順] - 5 と 6 を指定して CalcLibrary.Multiply() を呼び出す。

            // Assert_3
            Assert.True(multiplyResult.IsSuccess); // [確認_正常系] - multiplyResult.IsSuccess が true であること。
            Assert.Equal(30, multiplyResult.Value); // [確認_正常系] - multiplyResult.Value が 30 であること。(5 と 6 の積であることの確認)

            // Act_4
            var divideResult =
                CalcLibrary.Divide(100, 4); // [手順] - 100 と 4 を指定して CalcLibrary.Divide() を呼び出す。

            // Assert_4
            Assert.True(divideResult.IsSuccess); // [確認_正常系] - divideResult.IsSuccess が true であること。
            Assert.Equal(25, divideResult.Value); // [確認_正常系] - divideResult.Value が 25 であること。(100 と 4 の商であることの確認)
        }

        // 現在のプラットフォームでネイティブ ライブラリに例外なくアクセスできることの確認
        [Fact]
        public void NativeLibrary_ShouldBeAccessible()
        {
            // このテストは、ネイティブ ライブラリにアクセスでき、
            // 現在のプラットフォームで P/Invoke が正しく機能することを検証します。

            // Act
            CalcResult result = null;
            var exception = Record.Exception(() =>
                                             {
                                                 result = CalcLibrary.Add(
                                                     1, 1); // [手順] - 1 と 1 を指定して CalcLibrary.Add() を呼び出す。
                                             });

            // Assert
            Assert.Null(exception); // [確認_正常系] - exception が null であること。(CalcLibrary.Add() が例外を送出しないことの確認)
            Assert.True(result.IsSuccess); // [確認_正常系] - result.IsSuccess が true であること。
        }
    }
}
