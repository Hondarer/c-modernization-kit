# .NET テスト (xUnit)

## 概要

xUnit.net は .NET 向けの単体テスト フレームワークです。`[Fact]` 属性でテスト メソッドを定義し、`Assert` クラスのメソッドで期待値を検証します。`dotnet test` コマンドでテストを実行でき、GitHub Actions などの CI 環境とも容易に統合できます。

対象ワークスペースの `.NET` プロジェクト (`app/example.net/prod/`) に対するテストは、xUnit を使用して実装します。`ExampleLib` が内部で呼び出す C ライブラリ (P/Invoke 経由) の動作を .NET レイヤーから検証することが目的です。

.NET テストを理解することで、C ライブラリの .NET ラッパーが正しく動作しているかを自動的に確認できるようになります。テスト結果は GitHub Actions で自動実行され、PR のマージ判断に活用されます。

## 習得目標

- [ ] xUnit のテスト プロジェクト (`.csproj`) を作成できます。
- [ ] `[Fact]` でテスト メソッドを定義できます。
- [ ] `Assert.Equal`・`Assert.True`・`Assert.Throws` を使用できます。
- [ ] `[Theory]` と `[InlineData]` でパラメーター化テストを記述できます。
- [ ] `dotnet test` でテストを実行し結果を確認できます。

## 学習マテリアル

### 公式ドキュメント

- [dotnet test による単体テスト (Microsoft Learn)](https://learn.microsoft.com/ja-jp/dotnet/core/testing/unit-testing-with-dotnet-test) - xUnit と dotnet test の入門 (日本語)
- [xUnit 公式ドキュメント](https://xunit.net/docs/getting-started/netcore/cmdline) - xUnit のスタート ガイド (英語)
- [dotnet test コマンド リファレンス](https://learn.microsoft.com/ja-jp/dotnet/core/tools/dotnet-test) - テスト実行コマンドの詳細 (日本語)

### 日本語コンテンツ

- [.NET テストのドキュメント (Microsoft Learn)](https://learn.microsoft.com/ja-jp/dotnet/core/testing/) - .NET テスト全般の日本語ドキュメント

## 対象ワークスペースとの関連

### 使用箇所 (具体的なファイル・コマンド)

ExampleLib のテスト コード例:

```csharp
using Xunit;
using ExampleLib;

public class ExampleLibraryTests
{
    [Fact]
    public void Add_TwoPositiveNumbers_ReturnsSum()
    {
        var lib = new ExampleLibrary();
        var result = lib.Calculate(ExampleKind.Add, 3, 4);
        Assert.Equal(7, result.Value);
    }

    [Theory]
    [InlineData(1, 2, 3)]
    [InlineData(-1, -2, -3)]
    [InlineData(0, 5, 5)]
    public void Add_VariousInputs_ReturnsCorrectSum(int a, int b, int expected)
    {
        var lib = new ExampleLibrary();
        var result = lib.Calculate(ExampleKind.Add, a, b);
        Assert.Equal(expected, result.Value);
    }

    [Fact]
    public void Divide_ByZero_ThrowsExampleException()
    {
        var lib = new ExampleLibrary();
        Assert.Throws<ExampleException>(() => lib.Calculate(ExampleKind.Divide, 1, 0));
    }
}
```

テストの実行:

```bash
# テストを実行
dotnet test app/example.net/prod/test/ExampleLibTest/ExampleLibTest.csproj
```

`.csproj` でのテスト プロジェクト設定:

```xml
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <TargetFramework>net8.0</TargetFramework>
    <IsPackable>false</IsPackable>
  </PropertyGroup>
  <ItemGroup>
    <PackageReference Include="Microsoft.NET.Test.Sdk" Version="17.*" />
    <PackageReference Include="xunit" Version="2.*" />
    <PackageReference Include="xunit.runner.visualstudio" Version="2.*" />
  </ItemGroup>
  <ItemGroup>
    <ProjectReference Include="../../libsrc/ExampleLib/ExampleLib.csproj" />
  </ItemGroup>
</Project>
```

### 関連ドキュメント

- [.NET テスト結果設計](../../dotnet-test-results-design.md) - 対象ワークスペースのテスト結果管理
- [.NET SDK (スキル ガイド)](../04-build-system/dotnet-sdk.md) - dotnet コマンドの基礎
- [C# / P/Invoke (スキル ガイド)](../08-dev-environment/dotnet-csharp.md) - テスト対象の実装詳細
