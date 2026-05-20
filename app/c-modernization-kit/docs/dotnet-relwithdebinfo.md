# .NET プロジェクトにおける RelWithDebInfo ビルド構成

## 概要

RelWithDebInfo (Release with Debug Information) は、リリース最適化を適用しながらデバッグ情報も含めるビルド構成です。CMake などのビルドシステムで一般的に使用されている概念を .NET プロジェクトに適用したものです。

## RelWithDebInfo の意義

### 標準的なビルド構成との比較

.NET プロジェクトでは、標準で以下の2つのビルド構成が提供されています:

| 構成 | 最適化 | デバッグシンボル | 用途 |
|------|--------|------------------|------|
| Debug | 無効 | 有効 | 開発・デバッグ時 |
| Release | 有効 | 無効（または限定的） | 本番リリース時（デバッグ情報不要の場合） |
| **RelWithDebInfo** | **有効** | **有効** | **本番リリース時・パフォーマンス測定・トラブルシューティング** |

### RelWithDebInfo が有用な場面

1. **本番リリース時の推奨構成**
   - 最適化によるパフォーマンス維持
   - デバッグシンボルにより、本番環境での問題発生時に詳細なスタックトレースを取得可能
   - クラッシュダンプ解析時に関数名やソースファイル情報が利用可能
   - .pdb ファイルをシンボルサーバーに配置することで、必要時のみ利用可能

2. **パフォーマンス測定・プロファイリング**
   - 本番環境と同等の最適化された状態でプロファイリング
   - デバッグシンボルにより、ボトルネックの関数名・行番号を特定可能
   - CPU プロファイラーやメモリプロファイラーでの詳細な解析

3. **本番環境のトラブルシューティング**
   - 最適化されたコードで問題を正確に再現
   - エラーログやスタックトレースに関数名やソースファイル情報が含まれる
   - デバッガーをアタッチしての調査が可能

4. **統合テスト・受け入れテスト**
   - 本番リリースと同じビルド構成でテスト実行
   - 問題発生時のデバッグが容易
   - パフォーマンステストでの正確な測定

5. **継続的インテグレーション (CI)**
   - デフォルトのビルド構成として設定することで、開発中も本番に近い最適化を常に適用
   - パフォーマンス劣化の早期発見
   - デバッグシンボルを含むため、CI でのテスト失敗時の調査が容易

## .NET における RelWithDebInfo の実装

### 構成の定義

RelWithDebInfo 構成は、以下の MSBuild プロパティで定義されます。

```xml
<PropertyGroup Condition="'$(Configuration)' == 'RelWithDebInfo'">
  <Optimize>true</Optimize>
  <DebugType>portable</DebugType>
  <DebugSymbols>true</DebugSymbols>
</PropertyGroup>
```

#### プロパティの説明

- **Optimize**: `true` に設定することで、Release と同等のコンパイラ最適化を有効化
- **DebugType**: `portable` を指定することで、クロスプラットフォーム対応の .pdb ファイルを生成
- **DebugSymbols**: `true` に設定することで、デバッグシンボルの生成を有効化

## 実装手順

### 1. 各プロジェクトファイル (.csproj) への構成追加

プロジェクト内のすべての `.csproj` ファイルに RelWithDebInfo 構成を追加します。

**追加場所**: `</PropertyGroup>` と `<ItemGroup>` の間など、適切な位置

```xml
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <!-- 既存のプロパティ -->
  </PropertyGroup>

  <!-- RelWithDebInfo 構成の追加 -->
  <PropertyGroup Condition="'$(Configuration)' == 'RelWithDebInfo'">
    <Optimize>true</Optimize>
    <DebugType>portable</DebugType>
    <DebugSymbols>true</DebugSymbols>
  </PropertyGroup>

  <ItemGroup>
    <!-- 既存のアイテム -->
  </ItemGroup>
</Project>
```

### 2. ソリューションファイル (.sln) への構成追加

#### 2.1 ソリューション構成の追加

`GlobalSection(SolutionConfigurationPlatforms)` に RelWithDebInfo を追加します。

```text
Global
  GlobalSection(SolutionConfigurationPlatforms) = preSolution
    RelWithDebInfo|Any CPU = RelWithDebInfo|Any CPU
    Debug|Any CPU = Debug|Any CPU
    Release|Any CPU = Release|Any CPU
  EndGlobalSection
```

**重要**: RelWithDebInfo を最初に配置することで、一部の IDE でデフォルト構成として認識されやすくなります。

#### 2.2 各プロジェクトの構成マッピング追加

`GlobalSection(ProjectConfigurationPlatforms)` で、各プロジェクトに RelWithDebInfo のマッピングを追加します。

```
GlobalSection(ProjectConfigurationPlatforms) = postSolution
  {PROJECT-GUID}.RelWithDebInfo|Any CPU.ActiveCfg = RelWithDebInfo|Any CPU
  {PROJECT-GUID}.RelWithDebInfo|Any CPU.Build.0 = RelWithDebInfo|Any CPU
  {PROJECT-GUID}.Debug|Any CPU.ActiveCfg = Debug|Any CPU
  {PROJECT-GUID}.Debug|Any CPU.Build.0 = Debug|Any CPU
  {PROJECT-GUID}.Release|Any CPU.ActiveCfg = Release|Any CPU
  {PROJECT-GUID}.Release|Any CPU.Build.0 = Release|Any CPU
EndGlobalSection
```

各プロジェクトの GUID について、同様のマッピングを追加します。

### 3. Directory.Build.props でのデフォルト構成設定

プロジェクトのルートディレクトリに `Directory.Build.props` ファイルを作成し、デフォルトのビルド構成を RelWithDebInfo に設定します。

```xml
<Project>
  <!--
    .NET用 Directory.Build.props

    このファイルは、.NET SDK のビルドシステムにより自動的に認識され、
    このディレクトリおよびサブディレクトリ内のすべての .csproj プロジェクトに対して
    プロジェクトファイルよりも先に評価されます。

    主な用途:
    - プロジェクト全体で共通のプロパティやビルド設定を一元管理
    - デフォルト値の設定 (プロジェクトファイルで上書き可能)

    参考: https://learn.microsoft.com/ja-jp/visualstudio/msbuild/customize-by-directory
  -->

  <!-- デフォルトのビルド構成を RelWithDebInfo に設定 -->
  <!--
    -c オプション未指定時の動作:
    - dotnet build → RelWithDebInfo でビルド
    - dotnet test → RelWithDebInfo でビルド
    - dotnet run → RelWithDebInfo でビルド

    明示的な構成指定で上書き可能:
    - dotnet build -c Debug → Debug でビルド
    - dotnet build -c Release → Release でビルド
  -->
  <PropertyGroup>
    <Configuration Condition="'$(Configuration)' == ''">RelWithDebInfo</Configuration>
  </PropertyGroup>
</Project>
```

#### Directory.Build.props の仕組み

- このファイルは、.NET SDK により自動的に検出され、すべてのプロジェクトファイルよりも**先に**評価されます
- プロジェクトファイル内で `<Configuration>` を設定することもできますが、SDK の評価タイミングの問題で効果がない場合があります
- `Directory.Build.props` を使用することで、確実にデフォルト構成を設定できます

## 使用方法

### コマンドラインでのビルド

#### デフォルト構成でビルド (RelWithDebInfo)

```bash
dotnet build
```

`Directory.Build.props` の設定により、構成を指定しない場合は RelWithDebInfo でビルドされます。

#### 明示的に構成を指定

```bash
# Debug でビルド
dotnet build -c Debug

# Release でビルド
dotnet build -c Release

# RelWithDebInfo でビルド (明示的)
dotnet build -c RelWithDebInfo
```

### テストの実行

```bash
# デフォルト (RelWithDebInfo) でテスト実行
dotnet test

# 特定の構成でテスト実行
dotnet test -c Debug
dotnet test -c RelWithDebInfo
```

### アプリケーションの実行

```bash
# デフォルト (RelWithDebInfo) で実行
dotnet run --project <project-path>

# 特定の構成で実行
dotnet run --project <project-path> -c Release
```

### ビルド出力の確認

構成が正しく適用されているかは、ビルド出力のパスで確認できます:

```bash
dotnet build
# 出力: bin/RelWithDebInfo/net9.0/app.dll

dotnet build -c Debug
# 出力: bin/Debug/net9.0/app.dll
```

## IDE における対応状況

### Visual Studio Code + C# Dev Kit

**制限事項**: C# Dev Kit には、UI からビルド構成を選択する機能が現時点で存在しません。

- ソリューションエクスプローラーでの構成選択 UI がない
- `csharp.preview.improvedLaunchExperience` 設定は起動構成の選択用で、ビルド構成の切り替えには使用できない

**回避策**:
1. **統合ターミナルを使用** (推奨)
   - VS Code の統合ターミナルから `dotnet build -c <構成>` を実行
   - `Directory.Build.props` の設定により、デフォルトで RelWithDebInfo が使用される

2. **tasks.json でビルドタスクを定義**
   - `.vscode/tasks.json` に異なる構成のビルドタスクを作成
   - キーボードショートカットでタスクを実行

3. **launch.json でデバッグ構成を指定**
   - デバッグ実行時の構成を変更可能

### Visual Studio (Windows)

- ソリューションエクスプローラーの上部にある構成ドロップダウンで RelWithDebInfo を選択可能
- `.sln` ファイルで RelWithDebInfo を最初に配置することで、デフォルトとして認識されやすい

### JetBrains Rider

- ツールバーの構成ドロップダウンで選択可能
- ビルド構成の管理 UI が充実

## ベストプラクティス

### 1. 継続的インテグレーション (CI) での使用

RelWithDebInfo をデフォルト構成に設定することで、以下のメリットがあります:

- **パフォーマンス劣化の早期発見**: 開発中も常に最適化されたコードでテスト
- **本番環境に近い状態でのテスト**: より実環境に近い条件でのテスト実行
- **問題発生時のデバッグが容易**: デバッグシンボルにより、スタックトレースが詳細

### 2. プロジェクト構成の推奨事項

```
project-root/
+-- Directory.Build.props        # デフォルト構成: RelWithDebInfo
+-- solution.sln                 # RelWithDebInfo を最初に配置
+-- src/
|   +-- Project1/
|   |   +-- Project1.csproj     # RelWithDebInfo 構成を定義
|   +-- Project2/
|       +-- Project2.csproj     # RelWithDebInfo 構成を定義
+-- test/
    +-- Tests/
        +-- Tests.csproj         # RelWithDebInfo 構成を定義
```

### 3. チーム開発での運用

- **Directory.Build.props をリポジトリに含める**: チーム全体で同じデフォルト構成を使用
- **ドキュメント化**: プロジェクトの README に RelWithDebInfo を使用していることを明記
- **CI/CD パイプラインでの明示的指定**: CI では `-c RelWithDebInfo` を明示的に指定することを推奨

## トラブルシューティング

### デフォルト構成が適用されない

**症状**: `dotnet build` を実行しても Debug でビルドされる

**原因と対策**:
1. `Directory.Build.props` がプロジェクトルートに存在するか確認
2. ファイルのエンコーディングが UTF-8 であることを確認
3. XML の構文エラーがないか確認

### C# Dev Kit で RelWithDebInfo が選択できない

**症状**: C# Dev Kit のソリューションエクスプローラーに RelWithDebInfo が表示されない

**対策**:
1. VS Code のウィンドウをリロード
2. `.sln` ファイルが正しく更新されているか確認
3. ソリューションをクローズして再度開く

### ビルド出力のパスが想定と異なる

**症状**: `bin/Debug/` にビルドされる

**確認事項**:
1. プロジェクトファイルで明示的に `<Configuration>Debug</Configuration>` が設定されていないか確認
2. IDE の設定で構成がオーバーライドされていないか確認

## 参考資料

- [MSBuild によるディレクトリのカスタマイズ](https://learn.microsoft.com/ja-jp/visualstudio/msbuild/customize-by-directory)
- [.NET のビルド構成について](https://learn.microsoft.com/ja-jp/visualstudio/ide/understanding-build-configurations)
- [dotnet build コマンド](https://learn.microsoft.com/ja-jp/dotnet/core/tools/dotnet-build)

## まとめ

RelWithDebInfo ビルド構成を .NET プロジェクトに追加することで、以下の利点が得られます:

1. **本番環境での運用改善**:
   - リリース最適化によるパフォーマンス維持
   - デバッグシンボルにより、本番環境での問題発生時に詳細なスタックトレースを取得
   - クラッシュダンプやエラーログからの迅速な原因特定

2. **開発効率の向上**:
   - パフォーマンス測定とデバッグを同時に実施可能
   - プロファイリング時に関数名・行番号を特定可能

3. **品質の向上**:
   - 本番環境と同じビルド構成でのテスト実施
   - 最適化による問題の早期発見（Release のみで発生する問題の検出）

4. **トラブルシューティングの効率化**:
   - 本番環境での問題再現時にデバッグ情報が利用可能
   - シンボルサーバーとの連携による柔軟な運用

**推奨事項**: 多くのプロダクションシステムでは、RelWithDebInfo を本番リリースのデフォルト構成として採用しています。デバッグシンボルのオーバーヘッドは最小限であり、トラブルシューティングの効率向上による利点が大きく上回ります。

特に CI/CD パイプラインのデフォルト構成として RelWithDebInfo を採用することで、継続的にパフォーマンスを監視しながら、問題発生時には迅速なデバッグが可能になります。
