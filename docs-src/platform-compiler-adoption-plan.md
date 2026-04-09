# `platform.h` / `compiler.h` 全面採用計画

## Summary

`prod` / `test` 配下のネイティブ C/C++ コードについて、OS・CPU・コンパイラ判定の入口を `prod/util/include/util/base/platform.h` に統一する。`platform.h` が `compiler.h` を内包している前提を使い、呼び出し側は原則 `platform.h` のみを include して `PLATFORM_*` / `ARCH_*` / `COMPILER_*` / `FORCE_INLINE` / `NO_INLINE` を使う方針とする。

今回の計画では原則として構造変更は行わない。既存の公開 API 名、DLL export マクロ名、呼び出し規約マクロ名、OS 別実装分割は維持し、`_WIN32` / `__GNUC__` / `_MSC_VER` などの直書き条件を適切な共通マクロへ置換することを主眼にする。

現状の重点対象は `prod/porter` と `prod/util` である。探索結果では OS 判定直書きが `prod/porter` に 581 件、`prod/util` に 244 件、コンパイラ判定直書きが `prod/util` に 40 件ある。

## Key Changes

### 1. 採用ルールを先に固定する

- ネイティブコードでは、OS 判定は `PLATFORM_WINDOWS` / `PLATFORM_LINUX` / `PLATFORM_UNKNOWN` を使う。
- CPU 判定は `ARCH_X64` / `ARCH_X86` / `ARCH_UNKNOWN` を使う。
  prod, test ともに、X64 をターゲットとしており、CPU 判定マクロは対象が無い認識。
- コンパイラ判定と属性は `COMPILER_*` / `FORCE_INLINE` / `NO_INLINE` を使う。
  `FORCE_INLINE` / `NO_INLINE` については、対象が無い認識。
- `platform.h` / `compiler.h` 以外での `_WIN32` / `_WIN64` / `__linux__` / `_MSC_VER` / `__clang__` / `__GNUC__` / `__x86_64__` / `_M_X64` / `__i386__` / `_M_IX86` の新規使用は禁止にする。
- `platform.h` と `compiler.h` の興味の対象外である `__cplusplus`、`DOXYGEN`、`__INTELLISENSE__`、`_GNU_SOURCE`、`WIN32_LEAN_AND_MEAN` のような環境制御・文書化用マクロは残す。

### 2. 置換対象を 3 種類に分けて進める

- 単純な OS 分岐:
  `#ifdef _WIN32` / `#ifndef _WIN32` を `PLATFORM_WINDOWS` / `PLATFORM_LINUX` ベースへ置換する。対象は `prod/util`、`prod/porter`、`test/util`、`test/porter`、`test/override-sample` など。
  元のコードが Linux 優先で　else で Windows としているため、配置パターンは以下。
  - `#if defined(PLATFORM_LINUX)` ~ `#elif defined(PLATFORM_WINDOWS)` ~ `#endif` (プラットフォームによる分岐があり、両方のプラットフォームの処理がある)
  - `#if defined(PLATFORM_LINUX)` ~ `#endif` (Linux の処理がある)
  - `#if defined(PLATFORM_WINDOWS)` ~ `#endif` (Windows の処理がある)
  - 現在、`#ifdef _WIN32` / `#ifndef _WIN32` であっても、その内容がコンパイラ差異に基づくものであれば、以下の構文にすること。
    - `#if defined(COMPILER_GCC)` ~ `#elif defined(COMPILER_MSVC)` ~ `#endif`
    - `#if defined(COMPILER_GCC)` ~ `#endif`
    - `#if defined(COMPILER_MSVC)` ~ `#endif`
- コンパイラ分岐・属性:
  Clang は利用していないため、興味としては GCC/G++ と MSVC のみでよい。
  `#ifdef __GNUC__` や `#if defined(__GNUC__) || defined(__clang__)` は GCC と MSVC を基準に整理し、Clang は `COMPILER_UNKNOWN` 扱いとする。`format(printf, ...)` などは、チェック構文付きと構文無しを `#if defined(COMPILER_GCC)` ~ `#else` ~ `#endif` で分岐する。GCC側を優先とする。
- 公開 API の export / calling convention:
  `CALC_EXPORT`、`POTR_EXPORT`、`TRACE_LOGGER_EXPORT` など既存の公開マクロ名は維持し、その定義内部だけを `PLATFORM_WINDOWS` / `COMPILER_MSVC` ベースへ置換する。利用側コードの互換性は変えない。
- 直接 include する OS ヘッダーは残してよいが、include ガード条件は `PLATFORM_*` ベースに寄せる。

### 3. 実装順序を固定する

1. 基盤ヘッダー整備
- `platform.h` は変更しない。
- `compiler.h` は GCC / MSVC / Unknown の3系統に整理し、Clang 専用分岐は削除する。

2. 公開ヘッダーの置換
- `prod/calc/include/libcalc.h`
- `prod/porter/include/porter.h`
- `prod/util/include/util/...`
- `prod/override-sample/include/...`
- `prod/subfolder-sample/include/...`
ここでは public macro 名を変えず、判定条件のみ置換する。

3. `prod/util` の内部実装を置換

4. `prod/porter` の内部実装を置換

5. `test` 配下を最後に置換

## Assumptions

- 対象は `prod` / `test` 配下のネイティブ C/C++ のみで、`.NET` / C# は対象外とする。
- `platform.h` / `compiler.h` だけで吸収しきれない特殊属性は、今回は条件式の統一に留める。
- Unknown platform / compiler への拡張は将来課題とし、今回の主対象は Linux / Windows と GCC / MSVC の整理である。Clang は考慮しない。
