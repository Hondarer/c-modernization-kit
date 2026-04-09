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

### `#ifdef` / `#ifndef` チェーン

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

### 単一マクロの `#if defined(MACRO)`

`#elif` を含まない単一マクロの `#if defined(MACRO)` は、
`#ifdef MACRO` と同じ規則で扱う。

```c
#if defined(MACRO)
    /* MACRO のコード */
#endif /* MACRO */
```

```c
#if defined(MACRO)
    /* MACRO のコード */
#else /* !MACRO */
    /* 非 MACRO のコード */
#endif /* MACRO */
```

### `#if defined()` / `#elif defined()` チェーン

複数のマクロを `#if defined()`/`#elif defined()` で列挙するチェーンの場合、
`#else` はすべての条件が偽であることを示す。`#endif` は、チェーンが
単純な `#elif defined(MACRO)` だけで構成される場合に限り、定義名群から
意味のある共通接頭辞を抽出できればコメントを付与する。

```
#if defined(MACRO_A)
    /* MACRO_A のコード */
#elif defined(MACRO_B)
    /* MACRO_B のコード */
#else /* !MACRO_A && !MACRO_B */   ← チェーン内の全マクロを && で否定
    /* フォールバックコード */
#endif /* MACRO_ */
```

一方、`#elif` に複合条件を含む場合は `#else` / `#endif` ともにコメントを付与しない。

```c
#if defined(PLATFORM_LINUX)
    /* Linux のコード */
#elif defined(PLATFORM_WINDOWS) && defined(COMPILER_MSVC)
    /* Windows + MSVC 専用のコード */
#else
    /* フォールバックコード */
#endif
```

### `PLATFORM_LINUX` / `PLATFORM_WINDOWS` 二択分岐

Linux と Windows を使い分ける二択分岐は、`#else /* PLATFORM_WINDOWS */` ではなく
`#elif defined(PLATFORM_WINDOWS)` を正規形とする。

```c
#if defined(PLATFORM_LINUX)
    /* Linux のコード */
#elif defined(PLATFORM_WINDOWS)
    /* Windows のコード */
#endif /* PLATFORM_ */
```

**この形に統一する理由:**  
Windows 側も明示的な条件式にそろえることで分岐の意味が直ちに読み取れ、
今後第三のプラットフォーム分岐が必要になったときも `#elif` の連鎖として
自然に拡張できる。`#endif` は単純な `defined(MACRO)` 列挙で、定義名群の
共通接頭辞がある場合に限って `/* PLATFORM_ */` のように統一する。

**共通接頭辞の抽出規則:**  
最長共通接頭辞を求め、その後で最後の `_` まで丸める。
丸めた結果が `_` や `__` のみになる場合は、意味のある接頭辞とはみなさず
コメントを付与しない。

```c
#if defined(COMPILER_GCC)
#elif defined(COMPILER_CLANG)
#elif defined(COMPILER_MSVC)
#endif /* COMPILER_ */
```

```c
#if defined(_MSC_VER)
#elif defined(__clang__)
#elif defined(__GNUC__)
#endif
```

**使用例 (`prod/calc/include/libcalc.h`):**

```c
#if defined(PLATFORM_LINUX)
    #define CALC_EXPORT
    #define CALC_API
#elif defined(PLATFORM_WINDOWS)
    /* ... Windows 固有の定義 ... */
#else /* !PLATFORM_LINUX && !PLATFORM_WINDOWS */
    #define CALC_EXPORT
    #define CALC_API
#endif /* PLATFORM_ */
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

`fix_if_comments.py` は上記ルールをソースコード全体に機械的に適用する。

### アルゴリズム

**パス 1 — ブロック構造解析**

行を先頭から順に読み、スタックで `#if`/`#ifdef`/`#ifndef` のネストを追跡する。

- `#ifdef MACRO` / `#ifndef MACRO` → スタックに `{kind, macro, has_else=False, has_elif=False}` を積む
- `#if defined(MACRO)` → スタックに `{kind='if_defined', macros=[MACRO], has_else=False, has_elif=False}` を積む
- `#elif defined(MACRO)` → スタックトップが `if_defined` の場合、`macros` に追記し `has_elif=True` にする
- 単一 `defined(MACRO)` に一致しない `#elif` → スタックトップが `if_defined` の場合、`complex_elif=True` にする
- `#if EXPR`（複雑な式）→ スタックに `{kind='if', macro='', has_else=False, has_elif=False}` を積む（修正対象外）
- `#else` → スタックトップの `has_else` を `True` に、行番号と対応マクロを記録
- `#endif` → スタックをポップし、行番号・マクロ・`has_else`・`has_elif` を記録

**パス 2 — コメント修正**

- `#else` の行:
  - `#ifdef`/`#ifndef` 対応 → `/* !MACRO */` または `/* MACRO */`
  - `#if defined(PLATFORM_LINUX)` 直下で `#else /* PLATFORM_WINDOWS */`
    → `#elif defined(PLATFORM_WINDOWS)`
  - `#if defined(MACRO)` 単独対応 → `/* !MACRO */`
  - `#elif` を含む `#if defined()` チェーン → `/* !MACRO_A && !MACRO_B && ... */`
  - 複合条件を含む `#elif` チェーン → `#else` のコメントを除去
- `#endif` の行:
  - `#ifdef`/`#ifndef` 対応 → `/* MACRO */`（`#else` の有無を問わず）
  - `#elif` のない単一マクロの `#if defined(MACRO)` → `/* MACRO */`
  - 単純な `#elif defined(MACRO)` チェーン → 共通接頭辞があれば `/* PREFIX */`
  - 複合条件を含む `#elif` チェーン → コメントなし
  - 共通接頭辞が `_` / `__` のみ、または存在しない場合 → コメントなし

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
