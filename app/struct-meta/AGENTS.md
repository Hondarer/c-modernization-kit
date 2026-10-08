# AGENTS.md

## 対象

この app は、構造体メタデータを扱うライブラリとコマンド、サンプルを提供します。

## 参照先

作業に関係する文書の該当節を参照してください。

- 利用方法を確認する場合は [README.md](README.md)
- 設計を変更する場合は [アーキテクチャー](docs/architecture.md) の該当節

## 変更時の制約

- カテゴリ間の依存や責務を変更する場合は、アーキテクチャーの「依存方向」を確認してください。
- 新しい機能は事前組み込み型と事後解析型の両方から利用できるカタログ API 上に配置してください。
- 解析器の対応範囲を変更する場合は、アーキテクチャーも更新してください。
- 型やレイアウトを変更する場合は、LP64 / LLP64 の互換性と `struct_meta_internal_layout_find_type()` の正本を維持し、生成コードの `_Static_assert` をビルドで確認してください。
- `parse` は診断と結果コードを返し、プロセスを終了させないでください。flex / bison の再入可能な構成を維持してください。
- 公開入口では、構造体の内容へアクセスする前に記述子を検査してください。
- `prod/src/cmd/makelocal.mk` の順序は `struct-meta-gen`、`struct-meta-sample` を維持してください。
- Doxygen は公開 API 用と内部用の 2 系統です。生成済み `docs/doxybook2_public/` と `docs/doxybook2_internal/` は直接編集しないでください。

## 局所確認

振る舞いを変更した場合は影響する局所テストを実行し、app 全体への影響がある場合は app 直下の `make test` を実行してください。  
`make clean` は構成変更などで再生成が必要な場合に限定し、Doxygen の記法や出力を変更した場合は `make doxy` を実行してください。  
ビルド後は、対象範囲の内容がある `.warn` ファイルを確認してください。

カタログの取得や切り替えを変更した場合は、`./prod/cbin/struct-meta-sample` で次の 2 経路を確認してください。

```text
init sample_types
init prod/src/cmd/struct-meta-sample/sample_types.h
```

構造体一覧と `dump` の結果が一致すること、同じプロセスで対象を切り替えられること、`init` 失敗時に直前の対象が残ることを確認してください。
