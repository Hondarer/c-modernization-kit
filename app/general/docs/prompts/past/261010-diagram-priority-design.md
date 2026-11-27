# 図表作成ルールの優先順位変更仕様

## 合意版と検討文書

検討 ID: diagram-priority-20261010

合意版: 1

承認者: Tetsuo Honda

本対話の「承認する。最終仕様を作成して。」により、検討文書の版 1 が承認されています。本仕様に従った対象文書の更新は、本対話の「作業の着手を許可します」により承認されています。承認確認日時は 2026-10-10 01:22:03 JST、承認者は Tetsuo Honda です。

検討文書: [図表作成ルールの優先順位変更の検討](261010-diagram-priority-discussion.md)。更新範囲は [Q-001](261010-diagram-priority-discussion.md#q-001-更新範囲)、記録の保持は [Q-002](261010-diagram-priority-discussion.md#q-002-作業記録の削除範囲)、検証方法は [Q-003](261010-diagram-priority-discussion.md#q-003-検証方法) に対応します。

## 図表作成の優先順位

1. Mermaid を優先して使用します。
2. Mermaid で表現できない場合は PlantUML を使用します。
3. Mermaid と PlantUML の両方で表現できない場合は draw.io を使用し、`.drawio.svg` 形式で保存します。

## 更新対象と内容

次の 6 ファイルの方針、一覧、比較表の順位と用途説明、および関連リンク説明を更新します。

- `framework/docsfw/docs/sample/README.md`
- `app/general/docs/skill-guide/README.md`
- `app/general/docs/skill-guide/06-documentation/README.md`
- `app/general/docs/skill-guide/06-documentation/mermaid.md`
- `app/general/docs/skill-guide/06-documentation/plantuml.md`
- `app/general/docs/skill-guide/06-documentation/drawio.md`

既存図、描画処理、および無関係な文書は変更しません。

## 検証方法

1. 各対象 Markdown に `text_style_jp.py --dry-run` を実行し、変換内容を確認してから `--in-place` を実行します。
2. 差分を確認します。
3. 旧優先順位の記述が残っていないか検索します。
4. ローカル リンクの参照先を確認します。

発行ビルドと全体テストは対象外とします。

## 受入条件

- 対象 6 ファイルの説明が指定の優先順位に一致していること。
- 旧優先順位の説明が残らず、所定の検証が完了していること。
- 既存図と描画処理に変更がないこと。

## 作業記録の保持

検討文書と本仕様を、Git 管理対象外の `prompt/diagram-priority-discussion.md` と `prompt/diagram-priority-design.md` に保持します。Git 管理対象には移動しません。
