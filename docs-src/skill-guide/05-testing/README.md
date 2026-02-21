# テスト自動化 (フェーズ 4 - 品質向上)

Google Test によるC コードの単体テスト、コードカバレッジの計測、xUnit による .NET テストを学びます。
テストの自動化により、コード変更時の品質を継続的に担保できます。

## スキルガイド一覧

| スキルガイド | 内容 |
|------------|------|
| [Google Test / Google Mock](google-test.md) | C/C++ 単体テストフレームワーク |
| [コードカバレッジ](code-coverage.md) | gcov / lcov / OpenCppCoverage によるカバレッジ計測 |
| [.NET テスト（xUnit）](dotnet-testing.md) | xUnit による .NET 単体テスト |

## このリポジトリとの関連

- `test/` - テストコードのルートディレクトリ
- `test/src/calc/libcalcbaseTest/` - `libcalcbase` ライブラリの単体テスト
- `test/libsrc/mock_calcbase/` - モック実装（Google Mock 使用例）
- `testfw/` - テストフレームワーク（サブモジュール）

## 次のステップ

テスト自動化を習得したら、[ドキュメント自動化](../06-documentation/README.md) に進んでください。
