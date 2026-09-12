# クロスプラットフォーム対応

## 概要

C 言語は Linux と Windows の両方で動作するコードを書けますが、OS やコンパイラによって異なる部分を吸収するための工夫が必要です。プリプロセッサ マクロ (`#ifdef`・`#if defined`) を使った条件コンパイルが主な手段で、OS の判別やコンパイラ固有の属性・宣言の差異を吸収します。

対象ワークスペースは Linux (GCC) と Windows (MSVC) の両方をサポートするクロスプラットフォーム設計になっています。  
公開 API の印は各 app の `*_EXPORT` マクロに集約し、実体は `cplat/base/dll_exports.h` の `CPLAT_DLL_EXPORT` が担います。  
Windows では `dllexport` / `dllimport`、Linux の共有ライブラリでは `__attribute__((visibility("default")))` に展開します。  
Linux の共有ビルドでは makefw が `-fvisibility=hidden` を付け、公開 API 以外を動的シンボル表から外します。

ビルド システム (makefile) 側でも OS やコンパイラの違いを吸収しており、`framework/makefw/` サブモジュールが Linux / Windows の差異をテンプレートとして提供しています。

## 習得目標

- [ ] `#ifdef _WIN32` / `#ifdef __linux__` などのマクロで OS を判別できます。
- [ ] `#ifdef _MSC_VER` / `#ifdef __GNUC__` でコンパイラを判別できます。
- [ ] Windows の `__declspec(dllexport)` / `__declspec(dllimport)` の意味を理解できます。
- [ ] GCC の `__attribute__((visibility("default")))` の意味を理解できます。
- [ ] 共通マクロ (`EXPORT` など) を定義してプラットフォーム差異を吸収できます。
- [ ] `app/example/prod/include/libexample.h` のクロスプラットフォーム宣言を読み取ることができます。

## 学習マテリアル

### 公式ドキュメント

- [DLL の作成と使用 (Microsoft Learn)](https://learn.microsoft.com/ja-jp/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp) - Windows での `__declspec` 使用例 (日本語)
- [GCC リンク オプション](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) - GCC のリンク・可視性オプション (英語)
- [MSVC コンパイラ リファレンス](https://learn.microsoft.com/ja-jp/cpp/build/reference/compiler-command-line-syntax) - MSVC コンパイラのオプション (日本語)
- [cppreference - プリプロセッサ](https://ja.cppreference.com/w/c/preprocessor) - `#ifdef` などのプリプロセッサ指令 (日本語)

## 対象ワークスペースとの関連

### 使用箇所 (具体的なファイル・コマンド)

エクスポート マクロのパターン (`dll_exports.h` を各 app が包む):

```c
#ifndef SAMPLE_STATIC
    #define SAMPLE_STATIC 0
#endif
#ifndef SAMPLE_EXPORTS
    #define SAMPLE_EXPORTS 0
#endif
#include <cplat/base/dll_exports.h>
#define SAMPLE_EXPORT CPLAT_DLL_EXPORT(SAMPLE)
#define SAMPLE_API    CPLAT_DLL_API(SAMPLE)

/* 公開ヘッダーの宣言に付ける (定義側には付けない) */
SAMPLE_EXPORT int SAMPLE_API sample_open(void);
```

関連ファイル:

- `prod/include/<library>/base/dll_exports.h` - エクスポートと可視性を定義する共通テンプレート
- `prod/include/<library>/<library>_export.h` - ライブラリ固有の薄いラッパー
- `framework/makefw/makefiles/makelibsrc_c_cpp.mk` - Linux 共有ビルドの `-fvisibility=hidden`
- [コーディング規範](../../coding-guideline.md) の「共有ライブラリのシンボル可視性」

makefile での OS 判別 (`framework/makefw/` テンプレートより):

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

- [ビルド設計](../../build-design.md) - クロスプラットフォーム ビルドの詳細設計
- [C ライブラリの種類](c-library-types.md) - 静的・動的ライブラリの仕組み
- [GNU Make (スキル ガイド)](../04-build-system/gnu-make.md) - makefile によるクロスプラットフォーム ビルド
- [WSL / MinGW 環境 (スキル ガイド)](../08-dev-environment/wsl-mingw.md) - Windows での Linux 互換ビルド環境
