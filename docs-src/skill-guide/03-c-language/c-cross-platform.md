# クロスプラットフォーム対応

## 概要

C 言語は Linux と Windows の両方で動作するコードを書けますが、OS やコンパイラによって異なる部分を吸収するための工夫が必要です。プリプロセッサマクロ（`#ifdef`・`#if defined`）を使った条件コンパイルが主な手段で、OS の判別やコンパイラ固有の属性・宣言の差異を吸収します。

このリポジトリは Linux（GCC）と Windows（MSVC）の両方をサポートするクロスプラットフォーム設計になっています。`prod/calc/include/libcalc.h` では、Windows の DLL エクスポート宣言（`__declspec(dllexport)`）と Linux の可視性属性（`__attribute__((visibility("default")))`）を条件コンパイルで切り替えています。同様のパターンは `prod/` 配下の他のヘッダーファイルにも見られます。

ビルドシステム（Makefile）側でも OS やコンパイラの違いを吸収しており、`makefw/` サブモジュールが Linux / Windows の差異をテンプレートとして提供しています。

## 習得目標

- [ ] `#ifdef _WIN32` / `#ifdef __linux__` などのマクロで OS を判別できる
- [ ] `#ifdef _MSC_VER` / `#ifdef __GNUC__` でコンパイラを判別できる
- [ ] Windows の `__declspec(dllexport)` / `__declspec(dllimport)` の意味を理解できる
- [ ] GCC の `__attribute__((visibility("default")))` の意味を理解できる
- [ ] 共通マクロ（`EXPORT` など）を定義してプラットフォーム差異を吸収できる
- [ ] `prod/calc/include/libcalc.h` のクロスプラットフォーム宣言を読み取れる

## 学習マテリアル

### 公式ドキュメント

- [DLL の作成と使用（Microsoft Learn）](https://learn.microsoft.com/ja-jp/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp) — Windows での `__declspec` 使用例（日本語）
- [GCC リンクオプション](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) — GCC のリンク・可視性オプション（英語）
- [MSVCコンパイラリファレンス](https://learn.microsoft.com/ja-jp/cpp/build/reference/compiler-command-line-syntax) — MSVC コンパイラのオプション（日本語）
- [cppreference — プリプロセッサ](https://ja.cppreference.com/w/c/preprocessor) — `#ifdef` などのプリプロセッサ指令（日本語）

## このリポジトリとの関連

### 使用箇所（具体的なファイル・コマンド）

クロスプラットフォーム宣言のパターン（`prod/calc/include/libcalc.h` の例）:

```c
/* DLL エクスポート/インポートのクロスプラットフォーム定義 */
#if defined(_WIN32) || defined(_WIN64)
  #ifdef LIBCALC_EXPORTS
    #define LIBCALC_API __declspec(dllexport)
  #else
    #define LIBCALC_API __declspec(dllimport)
  #endif
#else
  #define LIBCALC_API __attribute__((visibility("default")))
#endif
```

関連ファイル:

- `prod/calc/include/libcalc.h` — 動的ライブラリのエクスポート宣言
- `prod/calc/include/libcalcbase.h` — 静的ライブラリのヘッダー
- `prod/calc/include/libcalc_const.h` — OS に依存しない定数定義

Makefile での OS 判別（`makefw/` テンプレートより）:

```makefile
ifeq ($(OS),Windows_NT)
    # Windows 向けの設定
    SHARED_EXT := dll
    LIB_EXT    := lib
else
    # Linux 向けの設定
    SHARED_EXT := so
    LIB_EXT    := a
endif
```

### 関連ドキュメント

- [ビルド設計](../../build-design.md) — クロスプラットフォームビルドの詳細設計
- [C ライブラリの種類](c-library-types.md) — 静的・動的ライブラリの仕組み
- [GNU Make（スキルガイド）](../04-build-system/gnu-make.md) — Makefile によるクロスプラットフォームビルド
- [WSL / MinGW 環境（スキルガイド）](../08-dev-environment/wsl-mingw.md) — Windows での Linux 互換ビルド環境
