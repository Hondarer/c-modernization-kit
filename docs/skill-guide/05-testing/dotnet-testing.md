# .NET テスト (xUnit)

## 概要

xUnit.net は .NET 向けの単体テストフレームワークです。`[Fact]` 属性でテストメソッドを定義し、`Assert` クラスのメソッドで期待値を検証します。`dotnet test` コマンドでテストを実行でき、GitHub Actions などの CI 環境とも容易に統合できます。

このリポジトリの `.NET` プロジェクト (`prod/calc.net/`) に対するテストは、xUnit を使用して実装します。`CalcLib` が内部で呼び出す C ライブラリ (P/Invoke 経由) の動作を .NET レイヤーから検証することが目的です。

.NET テストを理解することで、C ライブラリの .NET ラッパーが正しく動作しているかを自動的に確認できるようになります。テスト結果は GitHub Actions で自動実行され、PR のマージ判断に活用されます。

## 習得目標

- [ ] xUnit のテストプロジェクト (`.csproj`) を作成できる
- [ ] `[Fact]` でテストメソッドを定義できる
- [ ] `Assert.Equal`・`Assert.True`・`Assert.Throws` を使用できる
- [ ] `[Theory]` と `[InlineData]` でパラメータ化テストを書ける
- [ ] `dotnet test` でテストを実行し結果を確認できる

## 学習マテリアル

### 公式ドキュメント

- [dotnet test による単体テスト(Microsoft Learn)](https://learn.microsoft.com/ja-jp/dotnet/core/testing/unit-testing-with-dotnet-test) - xUnit と dotnet test の入門 (日本語)
- [xUnit 公式ドキュメント](https://xunit.net/docs/getting-started/netcore/cmdline) - xUnit のスタートガイド (英語)
- [dotnet test コマンドリファレンス](https://learn.microsoft.com/ja-jp/dotnet/core/tools/dotnet-test) - テスト実行コマンドの詳細 (日本語)

### 日本語コンテンツ

- [.NET テストのドキュメント(Microsoft Learn)](https://learn.microsoft.com/ja-jp/dotnet/core/testing/) - .NET テスト全般の日本語ドキュメント

## このリポジトリとの関連

### 使用箇所 (具体的なファイル・コマンド)

CalcLib のテストコード例:

```csharp
using Xunit;
using CalcLib;

public class CalcLibraryTests
{
    [Fact]
    public void Add_TwoPositiveNumbers_ReturnsSum()
    {
        var lib = new CalcLibrary();
        var result = lib.Calculate(CalcKind.Add, 3, 4);
        Assert.Equal(7, result.Value);
    }

    [Theory]
    [InlineData(1, 2, 3)]
    [InlineData(-1, -2, -3)]
    [InlineData(0, 5, 5)]
    public void Add_VariousInputs_ReturnsCorrectSum(int a, int b, int expected)
    {
        var lib = new CalcLibrary();
        var result = lib.Calculate(CalcKind.Add, a, b);
        Assert.Equal(expected, result.Value);
    }

    [Fact]
    public void Divide_ByZero_ThrowsCalcException()
    {
        var lib = new CalcLibrary();
        Assert.Throws<CalcException>(() => lib.Calculate(CalcKind.Divide, 1, 0));
    }
}
```

テストの実行:

```bash
# テストを実行
dotnet test prod/calc.net/test/CalcLibTest/CalcLibTest.csproj
```

`.csproj` でのテストプロジェクト設定:

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
    <ProjectReference Include="../../libsrc/CalcLib/CalcLib.csproj" />
  </ItemGroup>
</Project>
```

### 関連ドキュメント

- [.NET テスト結果設計](../../dotnet-test-results-design.md) - このリポジトリのテスト結果管理
- [.NET SDK(スキルガイド)](../04-build-system/dotnet-sdk.md) - dotnet コマンドの基礎
- [C# / P/Invoke(スキルガイド)](../08-dev-environment/dotnet-csharp.md) - テスト対象の実装詳細
