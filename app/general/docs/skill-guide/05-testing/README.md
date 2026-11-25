# テスト自動化 (ステップ 4 - 品質向上)

Google Test による C コードの単体テスト、コード カバレッジの計測、xUnit による .NET テストを学びます。  
テストの自動化により、コード変更時の品質を継続的に担保できます。

## スキル ガイド一覧

Table: スキル ガイド一覧

| スキル ガイド                | 内容                                               |
|-----------------------------|----------------------------------------------------|
| [Google Test / Google Mock](google-test.md) | C/C++ 単体テスト フレームワーク                     |
| [コード カバレッジ](code-coverage.md)        | gcov/lcov/OpenCppCoverage によるカバレッジ計測 |
| [.NET テスト (xUnit)](dotnet-testing.md)     | xUnit による .NET 単体テスト                       |

## 対象ワークスペースとの関連

- `app/example/test/` - テスト コードのルート ディレクトリ
- `app/example/test/src/libexamplebaseTest/` - `libexamplebase` ライブラリの単体テスト
- `app/example/test/libsrc/mock_examplebase/` - モック実装 (Google Mock 使用例)
- `framework/testfw/` - テスト フレームワーク (サブモジュール、論理名: `testfw`)

## 次のステップ

テスト自動化を習得したら、[ドキュメント自動化](../06-documentation/README.md) に進んでください。
