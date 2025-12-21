#pragma warning disable 1587
/**
 *******************************************************************************
 *  @file           PlatformInfo.cs
 *  @brief          プラットフォーム検出ユーティリティ。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
 *
 *  現在のプラットフォームを検出し、適切なネイティブ
 *  ライブラリ名を決定するためのユーティリティを提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */
#pragma warning restore 1587

using System;
using System.Runtime.InteropServices;

namespace CalcDotNetLib.Internal
{
    /// <summary>
    /// プラットフォーム検出ユーティリティを提供します。
    /// </summary>
    internal static class PlatformInfo
    {
        /// <summary>
        /// 現在のプラットフォームが Windows かどうかを示す値を取得します。
        /// </summary>
        public static bool IsWindows => RuntimeInformation.IsOSPlatform(OSPlatform.Windows);

        /// <summary>
        /// 現在のプラットフォームが Linux かどうかを示す値を取得します。
        /// </summary>
        public static bool IsLinux => RuntimeInformation.IsOSPlatform(OSPlatform.Linux);

        /// <summary>
        /// 現在のプラットフォーム用のネイティブライブラリ名を取得します。
        /// </summary>
        /// <returns>ライブラリ名 (Windows の場合は "calc.dll"、Linux の場合は "libcalc.so")。</returns>
        /// <exception cref="PlatformNotSupportedException">
        /// 現在のプラットフォームが Windows または Linux ではない場合にスローされます。
        /// </exception>
        public static string GetLibraryName()
        {
            if (IsWindows)
                return "calc.dll";
            if (IsLinux)
                return "libcalc.so";

            throw new PlatformNotSupportedException(
                "Windows および Linux プラットフォームのみがサポートされています。現在のプラットフォーム: " +
                RuntimeInformation.OSDescription);
        }
    }
}
