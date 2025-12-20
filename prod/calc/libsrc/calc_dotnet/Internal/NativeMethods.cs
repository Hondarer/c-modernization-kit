/**
 *******************************************************************************
 *  @file           NativeMethods.cs
 *  @brief          ネイティブ calc ライブラリの P/Invoke 宣言。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
 *
 *  ネイティブ libcalc.so (Linux) または calc.dll (Windows)
 *  ライブラリとの相互運用のためのプラットフォーム固有の P/Invoke 宣言を含みます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

using System.Runtime.InteropServices;

namespace CalcDotNet.Internal
{
    /// <summary>
    /// ネイティブ calc ライブラリの P/Invoke 宣言を含みます。
    /// </summary>
    internal static class NativeMethods
    {
        /// <summary>
        /// プラットフォーム固有のライブラリ名。
        /// Windows: calc.dll
        /// Linux: libcalc.so
        /// </summary>
        private const string LibraryName =
#if WINDOWS
            "calc.dll";
#else
            "libcalc.so";
#endif

        /// <summary>
        /// ネイティブ calcHandler 関数の宣言。
        /// 指定された演算種別に基づいて計算を実行します。
        /// </summary>
        /// <param name="kind">計算の種別 (加算、減算、乗算、除算)。</param>
        /// <param name="a">第一オペランド。</param>
        /// <param name="b">第二オペランド。</param>
        /// <param name="result">計算結果 (出力パラメータ)。</param>
        /// <returns>成功時は 0 (CALC_SUCCESS)、失敗時は -1 (CALC_ERROR)。</returns>
        [DllImport(LibraryName,
                   CallingConvention =
#if WINDOWS
                       CallingConvention.StdCall,  // Windows では WINAPI (__stdcall)
#else
                       CallingConvention.Cdecl,    // Linux ではデフォルト (cdecl)
#endif
                   EntryPoint = "calcHandler")]
        internal static extern int CalcHandler(int kind, int a, int b, out int result);
    }
}
