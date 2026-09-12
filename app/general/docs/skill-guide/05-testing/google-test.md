# Google Test / Google Mock

## 概要

Google Test (gtest) は C/C++ 用の単体テスト フレームワークです。テスト ケースを `TEST()` マクロで定義し、`EXPECT_EQ`・`ASSERT_EQ` などのアサーション マクロで期待値を検証します。Google Mock (gmock) は gtest と組み合わせて使うモック ライブラリで、依存関係を持つコードのテストを可能にします。

対象ワークスペースの `app/example/test/` ディレクトリには Google Test を使用したテスト コードが含まれています。`framework/testfw/` サブモジュール (論理名: `testfw`) が Google Test のラッパーと実行支援スクリプトを提供しており、`add`・`subtract`・`multiply`・`divide` の各関数と `exampleHandler` に対するテストが実装されています。標準 C ライブラリ関数のモックも `app/example/test/libsrc/mock_examplebase/` と `app/example/test/libsrc/mock_example/` で提供されています。

C 言語のコードを Google Test でテストするには C++ でテスト コードを書く必要があります。`framework/testfw/` の仕組みを理解することで、新たなテスト ケースを追加できるようになります。

## 習得目標

- [ ] `TEST()`・`TEST_F()` マクロでテスト ケースを定義できます。
- [ ] `EXPECT_EQ`・`EXPECT_NE`・`EXPECT_TRUE`・`EXPECT_FALSE` を使用できます。
- [ ] `ASSERT_*` と `EXPECT_*` の違いを説明できます。
- [ ] テスト フィクスチャ (`::testing::Test` の派生クラス) を作成できます。
- [ ] テストを実行し、結果レポートを確認できます。
- [ ] `app/example/test/src/libexamplebaseTest/addTest/` のテスト コードを読み取ることができます。

## 学習マテリアル

### 公式ドキュメント

- [Google Test User's Guide](https://google.github.io/googletest/) - Google Test の公式ガイド (英語)
    - [Primer](https://google.github.io/googletest/primer.html) - 基本的なテスト記述方法
    - [Advanced Topics](https://google.github.io/googletest/advanced.html) - フィクスチャ・パラメータ化テスト
- [gMock for Dummies](https://google.github.io/googletest/googlemock/docs/for_dummies.html) - Google Mock 入門 (英語)

### チュートリアル・入門

- [testfw README](../../../../../framework/testfw/README.md) - 対象ワークスペースで使用するテスト フレームワークの説明

## 対象ワークスペースとの関連

### 使用箇所 (具体的なファイル・コマンド)

テスト コードの構成:

```text
app/example/test/
+-- src/
|   +-- main/
|   |   +-- addTest/          # add コマンドのテスト
|   |   +-- exampleTest/         # example コマンドのテスト
|   |   +-- shared-and-static-exampleTest/
|   +-- libexamplebaseTest/
|       +-- addTest/           # add 関数のテスト
|       +-- subtractTest/      # subtract 関数のテスト
|       +-- multiplyTest/      # multiply 関数のテスト
|       +-- divideTest/        # divide 関数のテスト
+-- libsrc/
|   +-- mock_examplebase/         # examplebase モック実装
|   +-- mock_example/             # example モック実装
```

テスト コードの基本パターン (C 関数のテスト例):

```cpp
#include <testfw.h>

#include "libexamplebase.h"
#include "libexample_const.h"

TEST(AddTest, PositiveNumbers) {
    int result = 0;
    EXPECT_EQ(EXAMPLE_SUCCESS, add(1, 2, &result));
    EXPECT_EQ(3, result);
}

TEST(AddTest, NegativeNumbers) {
    int result = 0;
    EXPECT_EQ(EXAMPLE_SUCCESS, add(-1, -2, &result));
    EXPECT_EQ(-3, result);
}
```

テストの実行:

```bash
# テストのビルドと実行
make test
```

### 関連ドキュメント

- [テスト チュートリアル](../../testing-tutorial.md) - 対象ワークスペースでのテスト実践ガイド
- [コード カバレッジ (スキル ガイド)](code-coverage.md) - テスト カバレッジの計測
