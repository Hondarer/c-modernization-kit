#pragma warning disable 1587
/**
 *******************************************************************************
 *  @file           CalcLibrary.cs
 *  @brief          calc ライブラリラッパーのメイン公開 API。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/20
 *  @version        1.0.0
 *
 *  P/Invoke を通じてネイティブ calc ライブラリを使用して
 *  計算を実行するためのメイン公開 API を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */
#pragma warning restore 1587

using CalcDotNetLib.Internal;

namespace CalcDotNetLib
{
    /// <summary>
    /// ネイティブ calc ライブラリを使用して基本的な整数演算を実行するための
    /// メソッドを提供します。
    /// </summary>
    public static class CalcLibrary
    {
        /// <summary>
        /// ネイティブライブラリからの成功戻り値コード。
        /// </summary>
        private const int CALC_SUCCESS = 0;

        /// <summary>
        /// ネイティブライブラリからのエラー戻り値コード。
        /// </summary>
        private const int CALC_ERROR = -1;

        /// <summary>
        /// 指定された演算種別に基づいて計算を実行します。
        /// </summary>
        /// <param name="kind">実行する計算の種別。</param>
        /// <param name="a">第一オペランド。</param>
        /// <param name="b">第二オペランド。</param>
        /// <returns>
        /// 結果またはエラー情報を含む <see cref="CalcResult"/>。
        /// 演算が成功したかどうかは <see cref="CalcResult.IsSuccess"/> で判定してください。
        /// </returns>
        /// <example>
        /// <code>
        /// var result = CalcLibrary.Calculate(CalcKind.Add, 10, 20);
        /// if (result.IsSuccess)
        /// {
        ///     Console.WriteLine($"Result: {result.Value}"); // 出力: 30
        /// }
        /// else
        /// {
        ///     Console.WriteLine($"Error: {result.ErrorCode}");
        /// }
        /// </code>
        /// </example>
        public static CalcResult Calculate(CalcKind kind, int a, int b)
        {
            int returnCode = NativeMethods.CalcHandler((int)kind, a, b, out int result);
            return new CalcResult(
                isSuccess: returnCode == CALC_SUCCESS,
                value: result,
                errorCode: returnCode
            );
        }

        /// <summary>
        /// 計算を実行し、失敗した場合は例外をスローします。
        /// </summary>
        /// <param name="kind">実行する計算の種別。</param>
        /// <param name="a">第一オペランド。</param>
        /// <param name="b">第二オペランド。</param>
        /// <returns>計算結果。</returns>
        /// <exception cref="CalcException">
        /// 計算が失敗した場合 (例: ゼロ除算または無効な演算種別)。
        /// </exception>
        /// <example>
        /// <code>
        /// try
        /// {
        ///     int result = CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0);
        /// }
        /// catch (CalcException ex)
        /// {
        ///     Console.WriteLine($"Calculation failed: {ex.Message}");
        /// }
        /// </code>
        /// </example>
        public static int CalculateOrThrow(CalcKind kind, int a, int b)
        {
            var result = Calculate(kind, a, b);
            if (!result.IsSuccess)
            {
                throw new CalcException(
                    result.ErrorCode,
                    $"Calculation failed: kind={kind}, a={a}, b={b}, errorCode={result.ErrorCode}");
            }
            return result.Value;
        }

        /// <summary>
        /// 2 つの整数を加算します。
        /// </summary>
        /// <param name="a">第一オペランド。</param>
        /// <param name="b">第二オペランド。</param>
        /// <returns>和またはエラー情報を含む <see cref="CalcResult"/>。</returns>
        /// <example>
        /// <code>
        /// var result = CalcLibrary.Add(5, 3);
        /// Console.WriteLine(result.Value); // 出力: 8
        /// </code>
        /// </example>
        public static CalcResult Add(int a, int b) => Calculate(CalcKind.Add, a, b);

        /// <summary>
        /// 第一の整数から第二の整数を減算します。
        /// </summary>
        /// <param name="a">第一オペランド (被減数)。</param>
        /// <param name="b">第二オペランド (減数)。</param>
        /// <returns>差またはエラー情報を含む <see cref="CalcResult"/>。</returns>
        /// <example>
        /// <code>
        /// var result = CalcLibrary.Subtract(10, 4);
        /// Console.WriteLine(result.Value); // 出力: 6
        /// </code>
        /// </example>
        public static CalcResult Subtract(int a, int b) => Calculate(CalcKind.Subtract, a, b);

        /// <summary>
        /// 2 つの整数を乗算します。
        /// </summary>
        /// <param name="a">第一オペランド。</param>
        /// <param name="b">第二オペランド。</param>
        /// <returns>積またはエラー情報を含む <see cref="CalcResult"/>。</returns>
        /// <example>
        /// <code>
        /// var result = CalcLibrary.Multiply(6, 7);
        /// Console.WriteLine(result.Value); // 出力: 42
        /// </code>
        /// </example>
        public static CalcResult Multiply(int a, int b) => Calculate(CalcKind.Multiply, a, b);

        /// <summary>
        /// 第一の整数を第二の整数で除算します。
        /// </summary>
        /// <param name="a">第一オペランド (被除数)。</param>
        /// <param name="b">第二オペランド (除数)。</param>
        /// <returns>商またはエラー情報を含む <see cref="CalcResult"/>。</returns>
        /// <remarks>
        /// 整数除算を実行します。ゼロ除算の場合はエラーとなります
        /// (<see cref="CalcResult.IsSuccess"/> は false)。
        /// </remarks>
        /// <example>
        /// <code>
        /// var result = CalcLibrary.Divide(20, 5);
        /// if (result.IsSuccess)
        /// {
        ///     Console.WriteLine(result.Value); // 出力: 4
        /// }
        ///
        /// var errorResult = CalcLibrary.Divide(10, 0);
        /// Console.WriteLine(errorResult.IsSuccess); // 出力: False
        /// </code>
        /// </example>
        public static CalcResult Divide(int a, int b) => Calculate(CalcKind.Divide, a, b);
    }
}
