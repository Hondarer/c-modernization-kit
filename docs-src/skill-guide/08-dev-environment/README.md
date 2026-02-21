# 開発環境・.NET 連携 (ステップ 4 - 自動化・拡張)

VS Code の設定・拡張機能、WSL / MinGW を使った Windows での Linux 互換ビルド環境、および C# / P/Invoke による .NET からの C ライブラリ呼び出しを学びます。
クロスプラットフォーム開発の実践的な環境構築と .NET 連携の実装方法を習得します。

## スキルガイド一覧

| スキルガイド         | 内容                                             |
|----------------------|--------------------------------------------------|
| [VS Code](vscode.md)                   | エディタの設定・拡張機能・デバッグ構成           |
| [WSL / MinGW 環境](wsl-mingw.md)       | Windows での Linux 互換ビルド環境の構築          |
| [C# / P/Invoke 基礎](dotnet-csharp.md) | .NET から C ライブラリを呼び出す P/Invoke の実装 |

## このリポジトリとの関連

- `prod/calc.net/libsrc/CalcLib/Internal/NativeMethods.cs` - P/Invoke による C ライブラリ呼び出し定義
- `prod/calc.net/src/CalcApp/ModuleInitializer.cs` - ネイティブライブラリのロード設定
- `c-modernization-kit.sln` - Visual Studio ソリューションファイル
- `Start-VSCode-With-Env.ps1` - Windows 環境変数セットアップスクリプト

## スキルガイド完了

すべてのステップを完了しました。[スキルガイド トップ](../README.md) に戻って全体を振り返ってください。
