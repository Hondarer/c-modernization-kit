# インクルード ガードのガイドライン

C/C++ ヘッダー ファイルにおけるインクルード ガードの命名規則と、規格上の予約識別子に関する方針を示します。

## インクルード ガードの命名規則

### 命名形式

ヘッダー ファイル名を大文字に変換し、ハイフンやドットをアンダースコアに置き換えた名前を使用します。**先頭にアンダースコアを付けてはなりません。**

```c
/* fmtio-util.h の場合 */
#ifndef FMTIO_UTIL_H
#define FMTIO_UTIL_H

/* ... */

#endif /* FMTIO_UTIL_H */
```

### 正しい例と誤った例

```c
/* 正しい例: ファイル名に基づき、アンダースコアで開始しない */
#ifndef LIBEXAMPLE_H
#define LIBEXAMPLE_H

/* 誤った例: 先頭のアンダースコア + 大文字はコンパイラ・標準ライブラリ用に予約されている */
#ifndef _LIBEXAMPLE_H
#define _LIBEXAMPLE_H
```

## 規則の根拠 (予約識別子の回避)

インクルード ガードの先頭にアンダースコアを禁止するのは、C/C++ 規格においてアンダースコアで始まる特定の識別子が「実装系 (コンパイラ、標準ライブラリ、OS)」のために予約されているためです。ユーザー コードでこれらの識別子を定義すると未定義動作 (undefined behavior) となります。

### 予約規則

以下の識別子パターンは、ファイル スコープにおいて実装系のために予約されています。

| パターン | 予約範囲 | 代表例 |
| -------- | -------- | ------ |
| アンダースコア + 大文字 (`_[A-Z]...`) | 実装系 (あらゆる用途で予約) | `_LIBEXAMPLE_H`, `_FMTIO_UTIL_H` |
| 連続する 2 つのアンダースコア (`__...`) | 実装系 (あらゆる用途で予約) | `__GNUC__`, `__cplusplus` |
| アンダースコア 1 つで始まる小文字 (`_[a-z]...`) | ファイル スコープの識別子 (通常名・タグ名) として予約 | `_count`, `_internal` |

Table: アンダースコアで始まる予約識別子のパターン

コンパイラや標準ライブラリが提供するマクロ (`_WIN32`, `__GNUC__`, `_MSC_VER` など) がこれらの命名パターンを使用しているのは、予約済み名前空間に属しているためです。

### 規格上の根拠

本規則は、以下の標準規格に基づいています。

- **ISO/IEC 9899:2011 (C11)**: §7.1.3 Reserved identifiers
- **ISO/IEC 9899:1999 (C99)**: §7.1.3 Reserved identifiers
- **ISO/IEC 9899:1990 (C90)**: §7.1.3 Reserved identifiers
- **ISO/IEC 14882:2020 (C++20)**: §16.4.5.3 Reserved names [reserved.names]

> **C11 §7.1.3 より引用:**
>
> - All identifiers that begin with an underscore and either an uppercase letter or another underscore are always reserved for any use.
> - All identifiers that begin with an underscore are always reserved for use as identifiers with file scope in both the ordinary and tag name spaces.

## 条件付きコンパイルにおける定義済みマクロの扱い

ユーザー コード側で `_WIN32` や `_MSC_VER` などの識別子を新規に **定義** することは禁止されますが、コンパイラや OS が提供する定義済みマクロを `#if defined(...)` や `#ifndef` で **判定** することは規格上正当な利用法です。

条件付きコンパイル判定における定義済みマクロは、変更せずそのまま使用します。

```c
/* コンパイラ定義済みマクロの判定: 定義の参照・判定であるため変更不要 */
#ifndef _WIN32
#define EXAMPLE_API
#else
#define EXAMPLE_API __declspec(dllexport)
#endif
```

## 参考資料

- [ISO/IEC 9899:2011 (C11 Standard)](https://www.iso.org/standard/57853.html)
- [C11 §7.1.3 Reserved identifiers (draft N1570)](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf#page=182)
- [cppreference.com - Identifiers: Reserved identifiers](https://en.cppreference.com/w/c/language/identifier#Reserved_identifiers)
- [CERT C Coding Standard - DCL37-C. Do not declare or define a reserved identifier](https://wiki.sei.cmu.edu/confluence/display/c/DCL37-C.+Do+not+declare+or+define+a+reserved+identifier)
