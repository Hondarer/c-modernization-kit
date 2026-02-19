# .NET SDK

## 概要

.NET SDK は、C# などの言語でアプリケーションやライブラリを開発するためのツールセットです。`dotnet` コマンドによりビルド・テスト・公開などの操作を行います。Visual Studio がなくてもコマンドラインだけで開発できます。

このリポジトリの `prod/calc.net/` は C ライブラリ（`libcalc`）を .NET から利用するためのラッパーライブラリとサンプルアプリケーションです。`CalcLib` が C ライブラリへの .NET インターフェースを提供し、`CalcApp` がそれを利用するサンプルアプリです。ビルドは `makefw/` が提供する Makefile テンプレートで `dotnet build` コマンドとして実行されます。

`Directory.Build.props`（リポジトリルートに配置）により、複数の .NET プロジェクトに共通のビルド設定を適用しています。また、`RelWithDebInfo`（Release with Debug Information）ビルド設定を使用した最適化とデバッグ情報の共存についても理解が必要です。

## 習得目標

- [ ] `dotnet build`・`dotnet run`・`dotnet test` などの基本コマンドを実行できる
- [ ] `.csproj` ファイルの基本構造（`<Project>`・`<PropertyGroup>`・`<ItemGroup>`）を読み取れる
- [ ] `Directory.Build.props` による共通設定の仕組みを理解できる
- [ ] `dotnet publish` で実行可能な成果物を生成できる
- [ ] `<ProjectReference>` によるプロジェクト参照を理解できる
- [ ] `TargetFramework`（`net8.0` など）の意味を理解できる

## 学習マテリアル

### 公式ドキュメント

- [dotnet コマンドリファレンス](https://learn.microsoft.com/ja-jp/dotnet/core/tools/) — `dotnet build`・`dotnet run`・`dotnet test` などのリファレンス（日本語）
- [Directory.Build.props の使用](https://learn.microsoft.com/ja-jp/visualstudio/msbuild/customize-by-directory) — 共通ビルドプロパティのカスタマイズ（日本語）
- [.NET Standard の概要](https://learn.microsoft.com/ja-jp/dotnet/standard/net-standard) — .NET バージョンと互換性（日本語）

### チュートリアル・入門

- [.NET チュートリアル — Hello World](https://learn.microsoft.com/ja-jp/dotnet/core/tutorials/with-visual-studio-code) — VS Code で .NET アプリを作成（日本語）
- [xUnit による単体テスト](https://learn.microsoft.com/ja-jp/dotnet/core/testing/unit-testing-with-dotnet-test) — .NET テストの入門（日本語）

## このリポジトリとの関連

### 使用箇所（具体的なファイル・コマンド）

プロジェクト構成:

```text
prod/calc.net/
├── libsrc/CalcLib/CalcLib.csproj      # C ライブラリの .NET ラッパー
└── src/CalcApp/CalcApp.csproj         # サンプルアプリケーション
```

`Directory.Build.props`（ルートの共通設定）:

```xml
<Project>
  <PropertyGroup>
    <!-- すべての .NET プロジェクトに適用される共通設定 -->
    <Nullable>enable</Nullable>
    <ImplicitUsings>enable</ImplicitUsings>
  </PropertyGroup>
</Project>
```

基本的なビルドコマンド:

```bash
# CalcLib をビルド
dotnet build prod/calc.net/libsrc/CalcLib/CalcLib.csproj

# CalcApp をビルド
dotnet build prod/calc.net/src/CalcApp/CalcApp.csproj

# CalcApp を実行
dotnet run --project prod/calc.net/src/CalcApp/CalcApp.csproj
```

### 関連ドキュメント

- [.NET RelWithDebInfo ビルド](../../dotnet-relwithdebinfo.md) — RelWithDebInfo 設定の詳細
- [C# / P/Invoke（スキルガイド）](../07-dev-environment/dotnet-csharp.md) — .NET から C ライブラリを呼び出す実装
- [.NET テスト（スキルガイド）](../04-testing/dotnet-testing.md) — xUnit による .NET テスト
