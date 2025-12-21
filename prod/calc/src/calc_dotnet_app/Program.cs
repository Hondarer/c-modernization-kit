/**
 *******************************************************************************
 *  @file           Program.cs
 *  @brief          calc コマンドの .NET 版。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
 *
 *  コマンドライン引数から 2 つの整数と演算子を受け取り、CalcDotNet ライブラリを
 *  使用して計算結果を標準出力に出力します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

using System;
using CalcDotNetLib;

namespace CalcDotNetApp
{
    /// <summary>
    /// calc コマンドのメインクラス。
    /// </summary>
    public class Program
    {
        /// <summary>
        /// プログラムのエントリーポイント。
        /// </summary>
        /// <param name="args">コマンドライン引数の配列。</param>
        /// <returns>成功時は 0、失敗時は 1 を返します。</returns>
        ///
        /// <example>
        /// 使用例:
        /// <code>
        /// ./calc_dotnet 10 + 20
        /// // 出力: 30
        ///
        /// ./calc_dotnet 15 - 5
        /// // 出力: 10
        ///
        /// ./calc_dotnet 6 x 7
        /// // 出力: 42
        ///
        /// ./calc_dotnet 20 / 4
        /// // 出力: 5
        /// </code>
        /// </example>
        public static int Main(string[] args)
        {
            // 引数の数をチェック
            if (args.Length != 3)
            {
                Console.Error.WriteLine("Usage: calc_dotnet <arg1> <arg2> <arg3>");
                return 1;
            }

            // オペレーターが1文字であることをチェック
            if (string.IsNullOrEmpty(args[1]) || args[1].Length != 1)
            {
                Console.Error.WriteLine("Usage: calc_dotnet <arg1> <arg2> <arg3>");
                return 1;
            }

            // 引数をパース
            if (!int.TryParse(args[0], out int arg1))
            {
                Console.Error.WriteLine($"Error: Invalid number '{args[0]}'");
                return 1;
            }

            if (!int.TryParse(args[2], out int arg3))
            {
                Console.Error.WriteLine($"Error: Invalid number '{args[2]}'");
                return 1;
            }

            // オペレーターから演算種別を決定
            CalcKind kind;
            switch (args[1][0])
            {
                case '+':
                    kind = CalcKind.Add;
                    break;
                case '-':
                    kind = CalcKind.Subtract;
                    break;
                case 'x':
                    kind = CalcKind.Multiply;
                    break;
                case '/':
                    kind = CalcKind.Divide;
                    break;
                default:
                    Console.Error.WriteLine("Usage: calc_dotnet <num1> <+|-|x|/> <num2>");
                    return 1;
            }

            // 計算を実行
            var result = CalcLibrary.Calculate(kind, arg1, arg3);

            if (!result.IsSuccess)
            {
                Console.Error.WriteLine("Error: calcHandler failed");
                return 1;
            }

            // 結果を出力
            Console.WriteLine(result.Value);

            return 0;
        }
    }
}
