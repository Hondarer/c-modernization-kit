---
name: check-coding-guideline
description: |
  app/ 配下の C / C++ コードを書く・変更するときに、
  docs/c-modernization-kit/coding-guideline.md に沿って
  整数型・異常入力対応・変数宣言位置・const + Doxygen 方向タグ・
  Doxygen プレースホルダー表記を点検するためのスキルです。
  判定基準と例は原典に集約されているため、本スキルは概要と参照に徹します。
when_to_use: |
  - app/ 配下の C / C++ コードを新規作成するとき
  - app/ 配下の C / C++ コードを変更するとき
  - コーディング規範に沿っているかをレビュー・確認したいとき
  - docs/c-modernization-kit/coding-guideline.md 本文を更新したとき
---

# コーディング規範チェック

このスキルは `docs/c-modernization-kit/coding-guideline.md` の薄いラッパーです。  
判定基準・grep 例・コード例は原典に集約されているため、本書では参照先のみを示します。

## 原典

- `docs/c-modernization-kit/coding-guideline.md`

## 関連文書

- `AGENTS.md` (`app/` 配下の `goto` / 三項演算子の禁止)
- `docs/c-modernization-kit/source-style-guideline.md`
- `docs/c-modernization-kit/include-guard-guideline.md`
- `docs/com_util/platform-abstraction-guideline.md`
- `maintain-doxygen-comment` skill (Doxygen タグ運用の正本)

## 適用範囲

- 主対象: `app/` 配下の C / C++ コード
- 変更ファイル内の関数全てに適用する (関数単位の部分適用は避ける)
- 大規模な const 化リファクタリングは「1 commit = 1 ヘッダー (カテゴリ)」単位で進める  
  (詳細は原典 §6)

## チェック項目 (原典のどの節を見るか)

1. **整数型の選択**
    - 8 / 16 / 32 / 64 bit の型選択と、固定幅型を例外として残してよい用途
    - → 原典「整数型の選択」を参照

2. **関数引数の異常入力対応**
    - 概念的に正値のみ想定する引数の `int` 採用、負値挙動の仕様明記
    - → 原典「関数引数の異常入力対応」を参照

3. **変数宣言位置と命令文の関係**
    - C17 スタイルでの宣言位置、`for` ループ変数のスコープ限定
    - → 原典「変数宣言位置と命令文の関係」を参照

4. **関数引数の const 付与と Doxygen 方向タグ**
    - 意味的方向 `[in]` / `[out]` / `[in,out]` の確定、ポインター / 値渡しでの const 付与判定、  
      opaque handle / 同期プリミティブ / `FILE *` は常に `[in]`、mock 追従
    - 機械的フィルターの grep 例、典型誤りパターン、模範例ファイル、検証コマンドはすべて原典に記載
    - → 原典「関数引数の const 付与と Doxygen 方向タグ」§1〜§9 を参照

5. **宣言と定義での修飾子・マクロの配置**
    - エクスポート / 呼び出し規約マクロ (`CALC_EXPORT` / `CALC_API` 等) は宣言のみ、定義側には付けない
    - 値渡し引数の top-level const は定義側のみ、ポインター参照先 const は宣言・定義の両方
    - → 原典「宣言と定義の関係」を参照

6. **Doxygen コメントのプレースホルダー表記**
    - プレースホルダーは `<...>` ではなく `{...}` を使う (doxybook2 が XML タグと誤認するため)
    - 検証は `doxy*.warn` の「Unsupported xml/html tag」で行う
    - → 原典「Doxygen コメントのプレースホルダー表記」を参照

## 作業の進め方

1. 変更対象のヘッダー / impl の関数シグネチャを列挙する
2. 上記 6 項目について、原典の該当節を読みながら判定する
3. 判定結果を impl とヘッダー両方に反映する
4. 必要に応じて mock を追従する (原典 §9)
5. 原典「検証」節のコマンドで局所ビルド・テスト・Doxygen 警告を確認する

## 確認項目

- 整数型方針に沿っているか。例外的に固定幅型を残した箇所には理由コメントがあるか
- 異常入力 (負値・NULL) のガードと Doxygen 仕様明記が揃っているか
- 変数宣言位置が利用箇所に近く、可読性を損ねていないか
- ポインター引数の const 付与判定が impl と整合しているか
- Doxygen `@param[in/out/in,out]` が impl の挙動と一致しているか
- opaque handle / 同期プリミティブ / `FILE *` が常に `[in]` か
- 値渡し引数の const は impl 側のみで、ヘッダー宣言には付いていないか
- エクスポート / 呼び出し規約マクロ (`CALC_EXPORT` / `CALC_API` 等) が宣言のみに付き、定義側には付いていないか
- mock 追従 (test 配下を持つモジュール) が完了しているか
- Doxygen コメントのプレースホルダーが `{}` 表記になっているか (`<>` を使用していないか)
- `app/` 配下では `goto` / 三項演算子を使用していないか (AGENTS.md)
