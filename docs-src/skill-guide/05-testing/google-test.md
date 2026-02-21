# Google Test / Google Mock

## 概要

Google Test（gtest）は C/C++ 用の単体テストフレームワークです。テストケースを `TEST()` マクロで定義し、`EXPECT_EQ`・`ASSERT_EQ` などのアサーションマクロで期待値を検証します。Google Mock（gmock）は gtest と組み合わせて使うモックライブラリで、依存関係を持つコードのテストを可能にします。

このリポジトリの `test/` ディレクトリには Google Test を使用したテストコードが含まれています。`testfw/` サブモジュールが Google Test のラッパーと実行支援スクリプトを提供しており、`add`・`subtract`・`multiply`・`divide` の各関数と `calcHandler` に対するテストが実装されています。標準 C ライブラリ関数のモックも `test/libsrc/mock_calcbase/` と `test/libsrc/mock_calc/` で提供されています。

C 言語のコードを Google Test でテストするには C++ でテストコードを書く必要があります（`extern "C"` によるリンク）。`testfw/` の仕組みを理解することで、新たなテストケースを追加できるようになります。

## 習得目標

- [ ] `TEST()`・`TEST_F()` マクロでテストケースを定義できる
- [ ] `EXPECT_EQ`・`EXPECT_NE`・`EXPECT_TRUE`・`EXPECT_FALSE` を使用できる
- [ ] `ASSERT_*` と `EXPECT_*` の違いを説明できる
- [ ] テストフィクスチャ（`::testing::Test` の派生クラス）を作成できる
- [ ] `extern "C"` を使って C 関数を C++ テストコードから呼び出せる
- [ ] テストを実行し、結果レポートを確認できる
- [ ] `test/src/calc/libcalcbaseTest/addTest/` のテストコードを読み取れる

## 学習マテリアル

### 公式ドキュメント

- [Google Test User's Guide](https://google.github.io/googletest/) — Google Test の公式ガイド（英語）
  - [Primer](https://google.github.io/googletest/primer.html) — 基本的なテスト記述方法
  - [Advanced Topics](https://google.github.io/googletest/advanced.html) — フィクスチャ・パラメータ化テスト
- [gMock for Dummies](https://google.github.io/googletest/googlemock/docs/for_dummies.html) — Google Mock 入門（英語）

### チュートリアル・入門

- [testfw README](../../testfw/README.md) — このリポジトリで使用するテストフレームワークの説明

## このリポジトリとの関連

### 使用箇所（具体的なファイル・コマンド）

テストコードの構成:

```text
test/
├── src/calc/
│   ├── main/
│   │   ├── addTest/          # add コマンドのテスト
│   │   ├── calcTest/         # calc コマンドのテスト
│   │   └── shared-and-static-calcTest/
│   └── libcalcbaseTest/
│       ├── addTest/           # add 関数のテスト
│       ├── subtractTest/      # subtract 関数のテスト
│       ├── multiplyTest/      # multiply 関数のテスト
│       └── divideTest/        # divide 関数のテスト
└── libsrc/
    ├── mock_calcbase/         # calcbase モック実装
    └── mock_calc/             # calc モック実装
```

テストコードの基本パターン（C 関数のテスト例）:

```cpp
#include <gtest/gtest.h>

extern "C" {
#include "libcalcbase.h"
#include "libcalc_const.h"
}

TEST(AddTest, PositiveNumbers) {
    int result = 0;
    EXPECT_EQ(CALC_SUCCESS, add(1, 2, &result));
    EXPECT_EQ(3, result);
}

TEST(AddTest, NegativeNumbers) {
    int result = 0;
    EXPECT_EQ(CALC_SUCCESS, add(-1, -2, &result));
    EXPECT_EQ(-3, result);
}
```

テストの実行:

```bash
# テストのビルドと実行
cd test/src/calc/libcalcbaseTest/addTest
make
./addTest
```

### 関連ドキュメント

- [テストチュートリアル](../../testing-tutorial.md) — このリポジトリでのテスト実践ガイド（最重要）
- [コードカバレッジ（スキルガイド）](code-coverage.md) — テストカバレッジの計測
