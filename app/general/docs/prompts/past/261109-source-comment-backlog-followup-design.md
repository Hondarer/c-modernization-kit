<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: codex
上流文書: `261109-source-comment-backlog-followup-discussion.md` (検討用 Markdown)。削除されている場合があります。
再開手順: agree-before-implement スキルを読み、上流文書があれば作業 WBS と承認状態を確認してください。上流文書がない場合は、この最終仕様を単体で参照してください。
-->

# ソース コメント残課題 3 件の対処仕様

仕様版: 1  
状態: 2026-11-09 に仕様版 1 を承認済み

## 対象と完了条件

BL-003、BL-004、BL-028 を対処します。  
今回扱わない 11 件は、元の課題 ID と経緯を残して新しい残課題一覧へ引き継ぎます。  
ソースと文書の変更は、以下の 3 件に必要な箇所へ限定します。

## BL-003 は EACCES をロック競合として扱わない

`app/cplat/prod/libsrc/cplat/sync/sync_linux.c` の `map_wait_rc()` から `EACCES` の専用分岐を除きます。  
`EACCES` が渡された場合は、ほかの未分類コードと同じ `CPLAT_ERR_UNKNOWN` を返します。  
成功、`ETIMEDOUT`、`EBUSY`、`EAGAIN` の分類は維持します。

`syncBoundaryTest.cc` と `syncFailureInjectionTest.cc` では、`EACCES` を人工的に渡した場合の期待値を `CPLAT_ERR_UNKNOWN` へ更新し、テスト項目の説明を結果に合わせます。  
公開 API の宣言と結果コードは変更しません。

## BL-004 は同じコメント内の複数の brief を順に連結する

`app/struct-meta/prod/libsrc/struct_meta/parse/doc.c` は、1 個の Doxygen コメントで行頭にあるすべての `@brief` を記載順に取り出します。  
各本文には現在の空白除去と連続空白の畳み込みを適用し、空の本文は連結しません。  
本文が複数ある場合は、既存の `struct_meta_internal_parse_doc_concat()` と同じ改行 1 文字で連結します。  
本文が 1 個だけの場合は、その本文を返します。1 個もない場合と確保に失敗した場合は、現行の戻り値 `NULL` を維持します。  
文中の `@brief` や名前が前方一致する別タグは引き続き除外します。

`app/struct-meta/docs/architecture.md` の Doxygen 属性の説明に、同一コメント内の複数 `@brief` を記載順に連結する規則を記載します。  
`structMetaParseDocTest.cc` に、複数の brief、単一の brief、空の brief と別タグの境界を確認する局所テストを追加します。  
前置コメントと後置コメントの優先順位、および汎用属性の解析規則は維持します。

## BL-028 は過去の要件参照へ正本の UUID を併記する

次の 5 箇所に、表示する要件 ID の直後へ正本と同じ HTML コメントを記載します。  
過去の検討内容と課題の記録は残します。

| 要件 ID と UUID | 対象文書 |
| --- | --- |
| `CPLAT-COMPRESS-FUNC-002` <!-- cplat-req: uuid=7bb7c876-6877-4159-afd3-51955f4d7d45 --> | `app/general/docs/prompts/past/261107-source-comment-backlog.md`、`261108-source-comment-cleanup-backlog.md`、`261108-source-comment-correctness-backlog.md`、`261108-source-comment-mechanical-backlog.md` |
| `CPLAT-RUNTIME-FUNC-014` <!-- cplat-req: uuid=bd2d6d79-1efb-4bf0-a62f-937b6191b8a4 --> | `prompts/past/process-priority-discussion.md` |

Table: UUID を併記する過去の要件参照

## 残課題の記録

実装と局所検証を終えた後、前回の `../current/261108-source-comment-residual-backlog.md` へ対処経緯を追記します。  
既存の未コミット変更は保持し、当該箇所へ必要な追記だけを行います。  
新しい `261109-source-comment-backlog-followup-backlog.md` には、未対処の 11 件と今回判明した未解決事項だけを残します。  
解消した 3 件は、新しい残課題一覧から除外します。

## 局所検証と受入条件

- `syncTest` の変更した 2 テストを含む局所テストを実行し、`EACCES` が `CPLAT_ERR_UNKNOWN` となることを確認します。
- `structMetaParseDocTest` の局所テストで、複数の brief の順序と区切り、単一の brief、空の brief、別タグの扱いを確認します。
- 変更した C/C++ を対象に局所ビルドを実行し、対象範囲の内容がある `.warn` ファイルを確認します。
- `python3 app/general/bin/check_functional_spec.py --root .` が、要件参照の問題を 0 件として成功することを確認します。
- 変更した Markdown はファイルごとに `text_style_jp.py --dry-run` を実行し、問題がなければ `--in-place` を実行します。

framework または複数 app にまたがる全体テストは、実行前に費用を説明してユーザーに確認します。
