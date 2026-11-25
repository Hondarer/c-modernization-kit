# C 言語発展トピック (ステップ 3 - ビルド理解)

C ライブラリの種類とクロスプラットフォーム対応の知識は、対象ワークスペースのコード構造を理解するうえで不可欠です。  
静的ライブラリ・動的ライブラリの違いと、Linux/Windows 両対応のコード記述方法を学びます。

## スキル ガイド一覧

Table: スキル ガイド一覧

| スキル ガイド                 | 内容                                             |
|------------------------------|--------------------------------------------------|
| [C ライブラリの種類](c-library-types.md)          | 静的ライブラリ・動的ライブラリの違いとリンク方法 |
| [クロスプラットフォーム対応](c-cross-platform.md) | Linux/Windows 対応マクロとビルド条件分岐         |

## 対象ワークスペースとの関連

- `app/example/prod/libsrc/examplebase/` - 静的ライブラリ (`libexamplebase`) の実装例
- `app/example/prod/libsrc/example/` - 動的ライブラリ (`libexample`) の実装例
- `app/example/prod/include/libexample.h` - 動的ライブラリ用エクスポート宣言 (`EXAMPLE_API` マクロ)

## 次のステップ

C 言語の発展トピックを習得したら、[ビルド システム](../04-build-system/README.md) に進んでください。
