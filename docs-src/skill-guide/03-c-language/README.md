# C 言語発展トピック (ステップ 3 - ビルド理解)

C ライブラリの種類とクロスプラットフォーム対応の知識は、このリポジトリのコード構造を理解するうえで不可欠です。
静的ライブラリ・動的ライブラリの違いと、Linux / Windows 両対応のコード記述方法を学びます。

## スキルガイド一覧

| スキルガイド | 内容 |
|------------|------|
| [C ライブラリの種類](c-library-types.md) | 静的ライブラリ・動的ライブラリの違いとリンク方法 |
| [クロスプラットフォーム対応](c-cross-platform.md) | Linux / Windows 対応マクロとビルド条件分岐 |

## このリポジトリとの関連

- `prod/calc/libsrc/calcbase/` - 静的ライブラリ（`libcalcbase`）の実装例
- `prod/calc/libsrc/calc/` - 動的ライブラリ（`libcalc`）の実装例
- `prod/calc/include/libcalc.h` - 動的ライブラリ用エクスポート宣言（`CALC_API` マクロ）

## 次のステップ

C 言語の発展トピックを習得したら、[ビルドシステム](../04-build-system/README.md) に進んでください。
