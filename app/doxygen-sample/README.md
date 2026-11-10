# doxygen-sample

Doxygen と Doxybook2 のコメント構文、画像、相互参照、出力変換を確認するサンプルです。

## 入口

- [作業規則](AGENTS.md)
- [Doxygen の入口](prod/README.md)
- [発行文書](docs/README.md)
- [生成手順](../../framework/doxyfw/docs/makefile-usage.md)

生成結果は `make doxy` 後に `pages/doxygen/` と `docs/doxybook2_*` で確認できます。  
生成物を直接編集せず、`prod/` 配下の入力を変更してください。

## 実装規範との違い

この app は Doxygen 記法と生成結果の比較用であり、一般の実装規範に従う実装例ではありません。  
`enum.h` のタグなし列挙型や、`direction.h` の非推奨な `[inout]` および方向指定なしの引数は、比較目的で維持しています。  
一般の実装では、[共通コーディング規範](../general/docs/coding-guideline.md) に従いタグと `typedef` 名を一致させ、引数の方向は `[in]`、`[out]`、`[in,out]` を明記します。
