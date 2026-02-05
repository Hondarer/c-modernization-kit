# インクルードガードのガイドライン

C/C++ ヘッダーファイルにおけるインクルードガードの命名規則を示します。

## アンダースコアで始まる識別子の予約

C 標準では、アンダースコアで始まる特定の識別子は「実装系」(コンパイラ、標準ライブラリ、OS) のために予約されています。ユーザーコードでこれらの識別子を使用した場合、未定義動作 (undefined behavior) となります。

### 予約規則

以下の識別子はファイルスコープにおいて予約されています。

| パターン | 予約先 | 例 |
| -------- | ------ | -- |
| アンダースコア + 大文字で始まる (`_[A-Z]...`) | 実装系 (いかなる用途でも予約) | `_LIBCALC_H`, `_FILE_UTIL_H` |
| アンダースコア 2 つで始まる (`__...`) | 実装系 (いかなる用途でも予約) | `__GNUC__`, `__cplusplus` |
| アンダースコア 1 つで始まる (`_[a-z]...`) | 通常の識別子としてはファイルスコープで予約 | `_count`, `_internal` |

コンパイラやライブラリが提供するマクロ (`_WIN32`, `__GNUC__`, `_MSC_VER` など) がこのパターンに従っているのは、予約済み名前空間に属しているためです。

### 出典

この予約規則は以下の規格で定義されています。

- **ISO/IEC 9899:2011 (C11)** - 7.1.3 Reserved identifiers
- **ISO/IEC 9899:1999 (C99)** - 7.1.3 Reserved identifiers
- **ISO/IEC 9899:1990 (C90)** - 7.1.3 Reserved identifiers
- **ISO/IEC 14882:2020 (C++20)** - 16.4.5.3 Reserved names [reserved.names]

C11 §7.1.3 から該当部分を引用します。

> All identifiers that begin with an underscore and either an uppercase letter or another underscore are always reserved for any use.
>
> All identifiers that begin with an underscore are always reserved for use as identifiers with file scope in both the ordinary and tag name spaces.

C++ 標準でも同等の規則が定義されています (C++20 では [lex.name] および [reserved.names])。

## 本プロジェクトの命名規則

### インクルードガードの形式

ヘッダーファイル名を大文字に変換し、ハイフンやドットをアンダースコアに置き換えた名前を使用します。先頭にアンダースコアを付けません。

```c
/* file-util.h の場合 */
#ifndef FILE_UTIL_H
#define FILE_UTIL_H

/* ... */

#endif /* FILE_UTIL_H */
```

### 正しい例と誤った例

```c
/* 正しい例 */
#ifndef LIBCALC_H
#define LIBCALC_H

/* 誤った例: アンダースコア + 大文字は実装系に予約されている */
#ifndef _LIBCALC_H
#define _LIBCALC_H
```

### 判定で使用するコンパイラ定義マクロ

`_WIN32`, `_MSC_VER`, `__GNUC__` 等のコンパイラが定義するマクロは、予約済み名前空間を使用することが規格上正しい識別子です。これらはユーザーが定義するものではなく、コンパイラや OS が提供するものであるため、`#if defined(_WIN32)` のような判定はそのまま使用します。

```c
/* コンパイラ定義マクロの判定: 変更不要 */
#ifndef _WIN32
#define CALC_API
#else
#define CALC_API __declspec(dllexport)
#endif
```

## 参考資料

- [ISO/IEC 9899:2011 (C11 Standard)](https://www.iso.org/standard/57853.html)
- [C11 §7.1.3 Reserved identifiers (draft N1570)](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf#page=182)
- [cppreference.com - Identifiers: Reserved identifiers](https://en.cppreference.com/w/c/language/identifier#Reserved_identifiers)
- [CERT C Coding Standard - DCL37-C. Do not declare or define a reserved identifier](https://wiki.sei.cmu.edu/confluence/display/c/DCL37-C.+Do+not+declare+or+define+a+reserved+identifier)
