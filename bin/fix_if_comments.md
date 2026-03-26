# fix_if_comments.py — 設計ノート

## 背景

### C プリプロセッサ条件分岐とコメントの慣習

C 言語のプリプロセッサ条件分岐（`#ifdef` / `#ifndef` / `#if`）は、
コンパイラが処理する前に展開されるため、通常のコードと異なりコメントによる
意図の明示が重要になる。

特に大規模なクロスプラットフォームコードでは、`#endif` の閉じ位置が
対応する `#if` から遠く離れることが多く、どの `#if` に対応するのかが
コメントなしでは即座に判断できない。

```c
/* コメントなし: どの #if を閉じているか一目でわからない */
#ifndef _WIN32
    /* ... 100行以上の Linux コード ... */
#else
    /* ... 100行以上の Windows コード ... */
#endif
```

```c
/* コメントあり: 対応関係が一目瞭然 */
#ifndef _WIN32
    /* ... 100行以上の Linux コード ... */
#else /* _WIN32 */
    /* ... 100行以上の Windows コード ... */
#endif /* _WIN32 */
```

### 業界標準プロジェクトの調査

主要な OSS プロジェクトがどのようなコメント規則を採用しているかを調査した。

| プロジェクト | `#ifdef X` の `#else` | `#ifndef X` の `#else` | `#endif` |
|---|---|---|---|
| Linux kernel | `/* !X */` | `/* X */` | `/* X */` |
| glibc | `/* !X */` | `/* X */` | `/* X */` |
| OpenSSL | `/* !X */` | `/* X */` | `/* X */` |
| curl | `/* !X */` | `/* X */` | `/* X */` |

共通する原則は以下のとおりである。

1. **`#else` コメントは「そのブロックで真となる条件」を示す**
   - `#ifdef X` の else ブロックは「X が未定義」なので `/* !X */`
   - `#ifndef X` の else ブロックは「X が定義済み」なので `/* X */`

2. **`#endif` コメントは「対応する `#if` のマクロ名」を示す**
   - `!` は付けない。`/* X */` 形式に統一する
   - `/* ifndef X */` や `/* ifdef X */` のようにディレクティブ名は含めない

3. **マクロ名のみ（ディレクティブ名なし）**
   - `/* ifndef _WIN32 */` ではなく `/* _WIN32 */`
   - ディレクティブ名を含めると可読性が下がり、またリファクタリング時
     （`#ifdef` → `#ifndef` への変換など）にコメントの更新漏れが起きやすい

## 採用したルール

```
#ifndef MACRO
    /* Linux / 非 MACRO のコード */
#else /* MACRO */          ← MACRO が真（定義済み）
    /* Windows / MACRO のコード */
#endif /* MACRO */         ← 対応マクロ名のみ

#ifdef MACRO
    /* MACRO のコード */
#else /* !MACRO */         ← !MACRO が真（未定義）
    /* 非 MACRO のコード */
#endif /* MACRO */         ← 対応マクロ名のみ（! なし）
```

### `#endif` に `!` を付けない理由

`#endif` は `#if`・`#ifdef`・`#ifndef` のいずれを閉じるかに関わらず、
閉じているブロックを特定する識別子として機能する。
`/* _WIN32 */` と書けば「`_WIN32` に関係するブロックの終端」と明確に伝わり、
`/* !_WIN32 */` と書いてしまうと「どちら側の終端か」が曖昧になる。

Linux kernel が `#endif /* HAVE_FOO */` と書いて `#endif /* !HAVE_FOO */` と
書かない理由もこれと同じである。

### `#else` にディレクティブ名を含めない理由

```c
/* NG: ディレクティブ名あり */
#ifndef _WIN32
    ...
#else /* ifndef _WIN32 */   ← リファクタリングで #ifdef に変えたとき更新が必要
```

```c
/* OK: マクロ名のみ */
#ifndef _WIN32
    ...
#else /* _WIN32 */          ← ディレクティブが変わってもコメントの意味は保たれる
```

## スクリプトの動作概要

`fix_if_comments.py` は上記ルールを porter ソースコード全体に機械的に適用する。

### アルゴリズム

**パス 1 — ブロック構造解析**

行を先頭から順に読み、スタックで `#if`/`#ifdef`/`#ifndef` のネストを追跡する。

- `#ifdef MACRO` / `#ifndef MACRO` → スタックに `{kind, macro, has_else=False}` を積む
- `#if EXPR`（複雑な式）→ スタックに `{kind='if', macro='', has_else=False}` を積む（修正対象外）
- `#else` → スタックトップの `has_else` を `True` に、行番号と対応マクロを記録
- `#endif` → スタックをポップし、行番号・マクロ・`has_else` を記録

**パス 2 — コメント修正**

- `#else` の行: `#ifdef`/`#ifndef` に対応するもののみ期待コメントと比較し、
  異なれば行を置換する
- `#endif` の行: `#ifdef`/`#ifndef` に対応するもの **すべて**（`#else` の有無を問わず）
  期待コメントと比較し、異なれば行を置換する

`#if EXPR`（複雑な式）ブロックの `#else` / `#endif` は変更しない。

### 使い方

```bash
# ドライラン（変更内容を unified diff 形式で表示）
python3 bin/fix_if_comments.py --dry-run prod/porter

# 実際に適用
python3 bin/fix_if_comments.py prod/porter

# 特定ファイルのみ
python3 bin/fix_if_comments.py src/foo.c include/bar.h
```
