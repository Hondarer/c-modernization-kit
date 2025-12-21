# CalcDotNetLib テスト

CalcDotNetLib ライブラリの包括的な単体テスト。

## 概要

このテストスイートは、xUnit を使用して CalcDotNetLib ラッパーライブラリの機能を検証します。すべての計算操作、エラー処理、例外動作、クロスプラットフォーム互換性のテストが含まれます。

## 要件

- .NET SDK 9.0 以降
- xUnit テストフレームワーク
- ネイティブ calc ライブラリ (Linux では libcalc.so、Windows では calc.dll)

## テストの実行

### Make を使用

```bash
# すべてのテストを実行
cd test/src/CalcDotNetLib.Tests
make test

# 実行せずにテストをビルド
make build

# テスト成果物をクリーン
make clean

# NuGet パッケージを復元
make restore
```

### dotnet CLI を使用

```bash
# すべてのテストを実行
dotnet test CalcDotNetLib.Tests.csproj

# 詳細出力で実行
dotnet test CalcDotNetLib.Tests.csproj --verbosity detailed

# 特定のテストクラスを実行
dotnet test --filter "FullyQualifiedName~CalcLibraryTests"
```

## テスト構造

### CalcLibraryTests.cs

`CalcLibrary` クラスのメインテストスイート。

#### テストカテゴリ

1. **加算テスト**
   - 正の数
   - 負の数
   - ゼロ値
   - 正と負の混合

2. **減算テスト**
   - 標準的な減算
   - 負の数の減算
   - ゼロ操作

3. **乗算テスト**
   - 正の乗算
   - 負の乗算
   - ゼロの乗算

4. **除算テスト**
   - 標準的な除算
   - 整数除算 (切り捨て)
   - ゼロ除算 (エラーケース)

5. **CalculateOrThrow テスト**
   - 成功する操作
   - エラー時の例外スロー

6. **エッジケース**
   - 極端な値 (int.MaxValue、int.MinValue)
   - ゼロ操作
   - 同じ値の操作

### CalcExceptionTests.cs

`CalcException` クラスのテスト。

- 例外プロパティ
- 内部例外を持つ例外
- 例外のキャッチ動作
- 例外メッセージ内のコンテキスト情報

### CrossPlatformTests.cs

クロスプラットフォーム互換性のテスト。

- プラットフォーム検出
- ライブラリ名の解決
- 現在のプラットフォームでの P/Invoke 機能
- ネイティブライブラリのアクセス可能性

## テストカバレッジ

テストスイートは以下の包括的なカバレッジを目指しています。

- すべての公開 API メソッド
- すべての計算操作
- エラー条件
- 例外処理
- クロスプラットフォーム機能

### 現在のカバレッジ

- ✅ 加算操作
- ✅ 減算操作
- ✅ 乗算操作
- ✅ 除算操作
- ✅ ゼロ除算エラー処理
- ✅ 例外のスローとキャッチ
- ✅ プラットフォーム検出
- ✅ クロスプラットフォームライブラリローディング

## トラブルシューティング

### テストのビルドが失敗する

テストのビルドが失敗する場合:

```bash
# NuGet パッケージを復元
make restore

# クリーンして再ビルド
make clean
make build
```

### プラットフォーム固有の問題

**Linux:**

- `libcalc.so` がライブラリパスに存在することを確認
- .csproj ファイルが自動的に DLL を出力ディレクトリにコピー

**Windows:**

- `calc.dll` がライブラリパスに存在することを確認
- .csproj ファイルが自動的に DLL を出力ディレクトリにコピー

## 新しいテストの追加

### テストの例

```csharp
[Fact]
public void MyNewTest()
{
    // Arrange
    int a = 10;
    int b = 20;

    // Act
    var result = CalcLibrary.Add(a, b);

    // Assert
    Assert.True(result.IsSuccess);
    Assert.Equal(30, result.Value);
}
```

### Theory テスト (データ駆動)

```csharp
[Theory]
[InlineData(5, 3, 8)]
[InlineData(10, 20, 30)]
[InlineData(-5, 5, 0)]
public void Add_WithVariousInputs_ShouldReturnCorrectResult(int a, int b, int expected)
{
    // Act
    var result = CalcLibrary.Add(a, b);

    // Assert
    Assert.True(result.IsSuccess);
    Assert.Equal(expected, result.Value);
}
```

## テスト結果

テスト結果はコンソール出力に表示されます。CI/CD 統合のためには、テスト結果ファイルを生成できます。

```bash
dotnet test --logger "trx;LogFileName=test-results.trx"
```

## 継続的インテグレーション

CI/CD パイプラインの場合は、以下のコマンドを使用します。

```bash
# 詳細ログでテストを実行
dotnet test --logger "console;verbosity=detailed" --results-directory ./test-results
```
