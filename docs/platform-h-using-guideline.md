# `platform.h` / `compiler.h` コーディングガイドライン

## 概要

C/C++ コードでは、OS・CPU・コンパイラ差異の判定を `app/com_util/prod/include/com_util/base/platform.h` と `app/com_util/prod/include/com_util/base/compiler.h` に集約しています。

利用側のコードは、処理系依存マクロを直接判定するのではなく、`PLATFORM_*` / `ARCH_*` / `COMPILER_*` / `FORCE_INLINE` / `NO_INLINE` を使って分岐してください。

## 基本ルール

### OS 判定は `PLATFORM_*` を使う

- Linux 判定: `PLATFORM_LINUX`
- Windows 判定: `PLATFORM_WINDOWS`
- 未知の環境: `PLATFORM_UNKNOWN`

`__linux__` や `_WIN32` をアプリケーションのコードへ直接書かないでください。

OS 分岐が必要なコードでは、まず `#include <com_util/base/platform.h>` を追加し、`PLATFORM_*` ベースで分岐します。

原則、`Linux -> Windows` の順で分岐するようにしてください。  
ただし、ファイル全体が Windows 向け実装を意図している場合は、`#if defined(PLATFORM_WINDOWS)` を先頭にします。

```c
#include <com_util/base/platform.h>

#if defined(PLATFORM_LINUX)
    /* Linux 向け処理 */
#elif defined(PLATFORM_WINDOWS)
    /* Windows 向け処理 */
#endif /* PLATFORM_ */
```

Windows 専用バックエンドや Windows 専用 API 実装では、次のように `PLATFORM_WINDOWS` を先頭に置きます。

```c
#include <com_util/base/platform.h>

#if defined(PLATFORM_WINDOWS)
    /* Windows 向け実装 */
#endif /* PLATFORM_WINDOWS */
```

### CPU 判定は `ARCH_*` を使う (拡張用: 現状 x64 専用のため)

- x64 判定: `ARCH_X64`
- x86 判定: `ARCH_X86`
- 未知の環境: `ARCH_UNKNOWN`

アーキテクチャ差異が必要な場合も、`__x86_64__` や `_M_X64` は利用側で直接判定しません。

### コンパイラ判定は `COMPILER_*` を使う

- GCC 判定: `COMPILER_GCC`
- MSVC 判定: `COMPILER_MSVC`
- 未知の処理系: `COMPILER_UNKNOWN`

`__GNUC__` や `_MSC_VER` は `compiler.h` の内部に閉じ込め、利用側では `COMPILER_*` だけを利用します。  
`COMPILER_*` の分岐順は、`GCC -> MSVC` とします。

```c
#include <com_util/base/compiler.h>

#if defined(COMPILER_GCC)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpadded"
#elif defined(COMPILER_MSVC)
    /* MSVC 向け処理 */
#endif /* COMPILER_GCC */
```

### インライン属性は共通マクロを使う

- 強制インライン: `FORCE_INLINE`
- 非インライン化: `NO_INLINE`

`__forceinline` や `__attribute__((noinline))` を利用側へ直接書きません。

### `platform.h` を基本入口にする

OS 判定とコンパイラ判定の両方が絡む可能性があるコードでは、原則として `#include <com_util/base/platform.h>` を使います。

`platform.h` は `compiler.h` を内包しているため、両方の共通マクロを利用できます。

## 直接使用を禁止するマクロ

`platform.h` / `compiler.h` の外側で、以下の新規使用を禁止します。

- `_WIN32`
- `_WIN64`
- `__linux__`
- `_MSC_VER`
- `__GNUC__`
- `__clang__`
- `__x86_64__`
- `_M_X64`
- `__i386__`
- `_M_IX86`

既存コードの修正でも、新しい分岐を追加する場合は共通マクロへ寄せてください。

## 例外として残してよいマクロ

以下は環境制御や言語切り替えのためのマクロであり、このガイドラインの禁止対象外です。

- `__cplusplus`
- `DOXYGEN`
- `__INTELLISENSE__`
- `_GNU_SOURCE`
- `WIN32_LEAN_AND_MEAN`

これらは OS / コンパイラ判定の代替として使うのではなく、本来の目的に限定して使います。

## 分岐の置き方

### 小さい差異は同一ファイル内で分岐する

型定義、include、定数、薄いラッパー程度の差異であれば、同一ファイル内の `PLATFORM_*` 分岐に留めます。

この書き方を使う場面:

- OS ごとに include するシステムヘッダーが違う
- typedef だけが違う
- 呼び出す API 名だけが違う
- テストのモック名だけが違う

実例:

- `app/com_util/prod/include/com_util/fs/path_format.h`
- `app/com_util/prod/libsrc/com_util/runtime/module_info.c`
- `app/com_util/test/src/libcom_utilTest/statfTest/statfTest.cc`

### 差異が大きい実装はファイル分割する

プラットフォームごとに処理フローや依存 API 群が大きく異なる場合は、`*_linux.c` / `*_windows.c` のようにファイルを分けます。

共通ヘッダーで型やフック関数を定義し、実装ファイル側で `PLATFORM_*` ごとに閉じます。

`app/com_util/prod/libsrc/com_util/compress` のように、Linux 用ファイルと Windows 用ファイルを同じビルド対象の一覧へ載せる構成では、各ファイルが「自分の対象 OS だけを実装する」形にします。

`*_linux.c` 側は、Linux 実装本体を `#if defined(PLATFORM_LINUX)` で囲い、Windows + MSVC でそのファイルが走査されたときだけ C4206 を抑止します。

```c
#include <com_util/base/platform.h>

#if defined(PLATFORM_LINUX)
    /* Linux 向け実装 */

#elif defined(PLATFORM_WINDOWS) && defined(COMPILER_MSVC)
    #pragma warning(disable : 4206)
#endif
```

MSVC では、中身のない翻訳単位に対して C4206 ("translation unit is empty") を出すことがあります。  
そのため、`*_linux.c` のような片側専用ファイルでも、Windows ビルドで同じソース一覧を使う場合は `#elif defined(PLATFORM_WINDOWS) && defined(COMPILER_MSVC)` を併記してください。

実例:

- `app/com_util/prod/libsrc/com_util/compress/compress_linux.c`
- `app/com_util/prod/libsrc/com_util/compress/compress_windows.c`
- `app/porter/prod/src/tech-sample/tcpServer/tcpServer.h`
- `app/porter/prod/src/tech-sample/tcpServer/tcpServer_linux.c`
- `app/porter/prod/src/tech-sample/tcpServer/tcpServer_windows.c`
- `app/com_util/prod/libsrc/com_util/trace/backends/etw/trace_etw.c`

判断目安:

- 依存ヘッダーが大きく違う
- 関数本体の大半が分岐で埋まる
- Linux と Windows で別のシステムモデルを使う

### 分岐の軸を混ぜない

OS 差異は `PLATFORM_*`、コンパイラ差異は `COMPILER_*` で書きます。

`#ifdef _WIN32` の見た目でも、実際には pragma や属性の違いを扱っているなら、`PLATFORM_*` ではなく `COMPILER_*` へ置き換えます。

分岐順も軸ごとに扱いを分けます。

- `PLATFORM_*`:
  原則は `Linux -> Windows`。ただし、ファイル全体が Windows 向け実装なら Windows を先頭にする
- `COMPILER_*`:
  `GCC -> MSVC` とする

判断目安:

- システム API、パス、ソケット、スレッド、ファイル操作の違い:
  `PLATFORM_*`
- pragma、attribute、警告抑制、関数属性の違い:
  `COMPILER_*`

## 公開ヘッダーのルール

### export / calling convention の公開マクロ名をライブラリ方針に合わせる

公開 API で使う export / calling convention マクロ名は、ライブラリごとの方針に合わせて統一します。

実例:

- `CALC_EXPORT` / `CALC_API`
- `POTR_EXPORT` / `POTR_API`
- `COM_UTIL_EXPORT` / `COM_UTIL_API`

### 定義本体は `dll_exports.h` に寄せる

公開ヘッダーごとに `__declspec(dllexport)` や `__stdcall` を定義せず、`app/com_util/prod/include/com_util/base/dll_exports.h` を使って定義します。

```c
#ifndef CALC_STATIC
    #define CALC_STATIC 0
#endif
#ifndef CALC_EXPORTS
    #define CALC_EXPORTS 0
#endif
#include <com_util/base/dll_exports.h>
#define CALC_EXPORT COM_UTIL_DLL_EXPORT(CALC)
#define CALC_API    COM_UTIL_DLL_API(CALC)
```

makefile から渡す場合は `CFLAGS += /DCALC_EXPORTS` のように値なしで定義してもよく、
このワークスペースでは GCC / Clang / MSVC が暗黙に `1` を与える前提で扱います。

実例:

- `app/calc/prod/include/libcalc.h`
- `app/porter/prod/include/porter.h`
- `app/com_util/prod/include/com_util/fs/path_format.h`

### 単一プラットフォーム専用 API はヘッダー全体で閉じる

Linux 専用 API、Windows 専用 API のように、公開面そのものが片側専用である場合は、ヘッダー全体を `PLATFORM_*` で囲います。

利用側も同じ条件で include / 呼び出しを行ってください。

実例:

- `app/com_util/prod/include/com_util/trace/trace_syslog.h`
- `app/com_util/prod/include/com_util/trace/trace_etw.h`

## コンパイラ依存構文のルール

### GCC 向けの属性は `COMPILER_GCC` で囲う

`__attribute__((format(...)))` や `#pragma GCC diagnostic` は GCC 専用構文です。

ヘッダーやテストへ書く場合も、必ず `COMPILER_GCC` でガードします。  
コンパイラ差異を書くときは、ファイルの主題が Windows 寄りでも GCC 優先で並べます。

実例:

- `app/com_util/prod/include/com_util/fs/path_format.h`
- `app/tutorial/test/src/param-test/param-test2.cc`
- `app/tutorial/test/src/param-test/param-test3.cc`

### OS 依存とコンパイラ依存を同じ分岐へ押し込まない

Windows であっても GCC 系を使う可能性、Linux であっても将来別処理系を使う可能性があります。

属性や pragma の差異は、OS ではなくコンパイラで判断してください。

## テストコードのルール

### モック差異は吸収マクロや helper に閉じる

プラットフォームごとにモック対象の API 名が異なる場合、テスト本体へ条件分岐をばらまかず、先頭で吸収マクロを定義してから使います。

```c
#if defined(PLATFORM_LINUX)
    #define STAT_MOCK_METHOD stat
#elif defined(PLATFORM_WINDOWS)
    #define STAT_MOCK_METHOD stat64
#endif /* PLATFORM_ */
```

実例:

- `app/com_util/test/src/libcom_utilTest/statfTest/statfTest.cc`

### 非対応環境のテストは明示的に skip する

片側プラットフォーム専用のテストは、反対側で無理に疑似実行せず、`GTEST_SKIP()` を使って「対象外」であることを明示します。

実例:

- `app/com_util/test/src/libcom_utilTest/syslog_providerTest/syslog_providerTest.cc`
- `app/com_util/test/src/libcom_utilTest/etw_providerTest/etw_providerTest.cc`
- `app/com_util/test/src/libcom_utilTest/etw_sessionTest/etw_sessionTest.cc`

### OS API を直接書く処理はテストヘルパーへ寄せる

一時ファイル作成やパス操作のように OS API を呼び分ける必要がある場合、テストケース本体ではなく helper へ閉じ込めます。

実例:

- `app/porter/test/include/porter_test_helper.h`

## レビュー時のチェックリスト

- `platform.h` / `compiler.h` の外で予約済みマクロを直接使っていないか
- OS 差異を `COMPILER_*` で、またはコンパイラ差異を `PLATFORM_*` で判定していないか
- `PLATFORM_*` の分岐順がファイルの意図と一致しているか
- `COMPILER_*` の分岐順が `GCC -> MSVC` になっているか
- 小さい差異なのに不要なファイル分割をしていないか
- 大きい差異なのに 1 ファイルへ分岐を詰め込みすぎていないか
- 公開ヘッダーで export / calling convention を独自実装していないか
- 単一プラットフォーム専用 API を無条件で include / 呼び出ししていないか
- 非対応プラットフォームのテストが `GTEST_SKIP()` で明示されているか

## 関連ドキュメント

- `app/com_util/prod/include/com_util/base/platform.md`
- `app/com_util/prod/include/com_util/base/compiler.md`
- `docs/include-guard-guideline.md`
