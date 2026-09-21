---
name: maintain-doxygen-comment
description: app の C/C++ Doxygen コメントを作成・変更・レビューする際に、宣言側の契約と生成規則を確認します。
---

# Doxygen コメントの保守

`app/general/docs/doxygen-comment-guideline.md` を参照し、宣言側を正本として実装と契約を照合してください。  
タグの仕様が必要なら `framework/doxyfw/docs/commands.md`、新しい形式の雛形が必要なら `framework/doxyfw/docs/cheatsheet.md` の該当節を確認してください。

公開関数の定義直前には `/* Doxygen コメントは、ヘッダーに記載 */` を配置し、同じ説明を複製しないでください。  
変更する契約に応じて引数方向、NULL、所有権、戻り値、スレッド安全性を確認してください。  
ファイル内スコープの `static` 変数や関数を参照する場合は、未解決参照による警告を防ぐため、`@ref` ではなくバッククォートを使用してください。

タグ、参照、公開契約、出力構造を変更した場合は対象 app の `make doxy` で警告と生成結果を確認してください。  
誤字や字下げだけの修正は差分確認を基本とし、レビューでは必要な場合に生成してください。  
生成済み Doxybook2 Markdown は直接編集しないでください。
