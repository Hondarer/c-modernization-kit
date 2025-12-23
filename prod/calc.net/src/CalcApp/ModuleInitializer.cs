#pragma warning disable 1587
/**
 *******************************************************************************
 *  @file           ModuleInitializer.cs
 *  @brief          モジュール初期化処理。
 *  @author         c-modernization-kit sample team
 *  @date           2025/12/23
 *  @version        1.0.0
 *
 *  アセンブリ解決のカスタムハンドラを登録し、LD_LIBRARY_PATH (Linux) または
 *  PATH (Windows) からアセンブリを動的に読み込みます。
 *  これにより、C のネイティブライブラリと同じ挙動でアセンブリを解決できます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */
#pragma warning restore 1587

using System;
using System.IO;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace CalcApp
{
    /// <summary>
    /// モジュール初期化クラス。アセンブリ解決ハンドラを最初に登録します。
    /// </summary>
    internal static class ModuleInitializer
    {
        /// <summary>
        /// モジュール初期化メソッド。型の読み込み前に実行されます。
        /// </summary>
        [ModuleInitializer]
        public static void Initialize()
        {
            AppDomain.CurrentDomain.AssemblyResolve += OnAssemblyResolve;
        }

        /// <summary>
        /// アセンブリ解決のカスタムハンドラ。
        /// LD_LIBRARY_PATH (Linux) または PATH (Windows) からアセンブリを探索します。
        /// </summary>
        private static Assembly OnAssemblyResolve(object sender, ResolveEventArgs args)
        {
            var assemblyName = new AssemblyName(args.Name);
            var dllName = assemblyName.Name + ".dll";

            // 環境変数名とパス区切り文字をプラットフォームに応じて決定
            string libraryPathEnv;
            char pathSeparator;

            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
            {
                libraryPathEnv = "PATH";
                pathSeparator = ';';
            }
            else
            {
                libraryPathEnv = "LD_LIBRARY_PATH";
                pathSeparator = ':';
            }

            // 環境変数を取得
            var libraryPath = Environment.GetEnvironmentVariable(libraryPathEnv);
            if (string.IsNullOrEmpty(libraryPath))
            {
                return null;
            }

            // 各パスを探索
            var paths = libraryPath.Split(pathSeparator, StringSplitOptions.RemoveEmptyEntries);
            foreach (var path in paths)
            {
                var assemblyPath = Path.Combine(path.Trim(), dllName);
                if (File.Exists(assemblyPath))
                {
                    return Assembly.LoadFrom(assemblyPath);
                }
            }

            return null;
        }
    }
}
