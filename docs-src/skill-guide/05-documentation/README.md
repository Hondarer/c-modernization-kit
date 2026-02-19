# ドキュメント自動化 (フェーズ 3 - 品質向上)

Markdown・Doxygen・Pandoc・PlantUML を組み合わせて、ソースコードから高品質なドキュメントを自動生成する方法を学びます。
ドキュメントをコードと同じリポジトリで管理することで、常に最新の状態を維持できます。

## スキルガイド一覧

| スキルガイド | 内容 |
|------------|------|
| [Markdown](markdown.md) | ドキュメント記法の基礎 |
| [Doxygen](doxygen.md) | C/C++ ソースコードからのドキュメント生成 |
| [Pandoc](pandoc.md) | Markdown から HTML / docx への変換 |
| [PlantUML](plantuml.md) | テキストベースの UML 図表作成 |

## このリポジトリとの関連

- `doxyfw/` - Doxygen ドキュメント生成フレームワーク（サブモジュール）
- `docsfw/` - Markdown 発行フレームワーク（サブモジュール）
- `Doxyfile.part.calc` - C プロジェクト用 Doxygen 設定
- `docs-src/` - ドキュメントソース（Markdown ファイル群）
- `docs/` - 生成済みドキュメント（HTML）

## 次のステップ

ドキュメント自動化を習得したら、[フェーズ 4 - 自動化・拡張](../06-ci-cd/README.md) に進んでください。
