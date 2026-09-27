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
            bool isWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows); // [手順] - Windows プラットフォームかどうか確認する。
            bool isLinux = RuntimeInformation.IsOSPlatform(OSPlatform.Linux); // [手順] - Linux プラットフォームかどうか確認する。

            // Assert
            Assert.True(isWindows || isLinux, "Platform should be Windows or Linux"); // [確認_正常系] - 実行環境が Windows または Linux プラットフォームとして判定されること。
        }

        // 現在のプラットフォーム上で各計算演算 (加算・減算・乗算・除算) が正常に動作することの確認
        [Fact]
        public void CalcLibrary_ShouldWork_OnCurrentPlatform()
        {
            // このテストは、現在のプラットフォーム (Windows または Linux) で
            // ライブラリが読み込まれ、呼び出せることを検証します。

            // Act_1
            var addResult = CalcLibrary.Add(10, 20); // [手順] - CalcLibrary.Add(10, 20) を呼び出す。

            // Assert_1
            Assert.True(addResult.IsSuccess); // [確認_正常系] - 加算の結果が成功であること。
            Assert.Equal(30, addResult.Value); // [確認_正常系] - 加算の結果が 30 であること。

            // Act_2
            var subtractResult = CalcLibrary.Subtract(30, 15); // [手順] - CalcLibrary.Subtract(30, 15) を呼び出す。

            // Assert_2
            Assert.True(subtractResult.IsSuccess); // [確認_正常系] - 減算の結果が成功であること。
            Assert.Equal(15, subtractResult.Value); // [確認_正常系] - 減算の結果が 15 であること。

            // Act_3
            var multiplyResult = CalcLibrary.Multiply(5, 6); // [手順] - CalcLibrary.Multiply(5, 6) を呼び出す。

            // Assert_3
            Assert.True(multiplyResult.IsSuccess); // [確認_正常系] - 乗算の結果が成功であること。
            Assert.Equal(30, multiplyResult.Value); // [確認_正常系] - 乗算の結果が 30 であること。

            // Act_4
            var divideResult = CalcLibrary.Divide(100, 4); // [手順] - CalcLibrary.Divide(100, 4) を呼び出す。

            // Assert_4
            Assert.True(divideResult.IsSuccess); // [確認_正常系] - 除算の結果が成功であること。
            Assert.Equal(25, divideResult.Value); // [確認_正常系] - 除算の結果が 25 であること。
        }

        // 現在のプラットフォームでネイティブ ライブラリに例外なくアクセスできることの確認
        [Fact]
        public void NativeLibrary_ShouldBeAccessible()
        {
            // このテストは、ネイティブ ライブラリにアクセスでき、
            // 現在のプラットフォームで P/Invoke が正しく機能することを検証します。

            // Act
            var exception = Record.Exception(() =>
            {
                var result = CalcLibrary.Add(1, 1); // [手順] - CalcLibrary.Add(1, 1) を呼び出す。
                Assert.True(result.IsSuccess); // [確認_正常系] - 呼び出し結果が成功であること。
            });

            // Assert
            Assert.Null(exception); // [確認_正常系] - 例外が発生しないこと。
        }
    }
}
