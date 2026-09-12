# C ライブラリの種類

## 概要

C 言語では、コードを再利用するためにライブラリとしてまとめる方法が 2 種類あります。静的ライブラリ (Linux: `.a`、Windows: `.lib`) は実行ファイルにリンク時に組み込まれ、動的ライブラリ (Linux: `.so`、Windows: `.dll`) は実行時に読み込まれます。それぞれにメリット・デメリットがあり、用途に応じて使い分けます。

対象ワークスペースの `app/example/prod/` はこの 2 種類のライブラリを意図的に使い分けています。`libexamplebase` (add / subtract / multiply / divide) は静的ライブラリとして提供され、`libexample` (exampleHandler) は動的ライブラリとして提供されます。`libexample` は `libexamplebase` に依存しており、ライブラリの生成時に参照されます。`src/shared-and-static-example/` は両方を同時にリンクするサンプルです。

また、.NET 連携においては `libexample.dll` (Windows) または `libexample.so` (Linux) を P/Invoke で呼び出す実装が `app/example.net/prod/` に含まれています。ライブラリの種類と動作の違いを理解することは、対象ワークスペースのビルド構成を理解するための重要なステップです。

## 習得目標

### プラットフォーム共通

- [ ] 静的ライブラリと動的ライブラリの違いを説明できます。
- [ ] `app/example/prod/` のライブラリ構成を読み取ることができます。

### Linux

- [ ] GCC で静的ライブラリ (`.a`) をビルドできます (`ar` コマンド)。
- [ ] GCC で動的ライブラリ (`.so`) をビルドできます (`-shared -fPIC` オプション)。
- [ ] 実行ファイルに静的ライブラリをリンクできます (`-l` / `-L` オプション)。
- [ ] 実行ファイルに動的ライブラリをリンクし、実行時パスを設定できます (`LD_LIBRARY_PATH`)。

### Windows

- [ ] MSVC で静的ライブラリ (`.lib`) をビルドできます (`lib.exe` コマンド)。
- [ ] MSVC で動的ライブラリ (`.dll`) をビルドできます (`/LD` オプション)。
- [ ] 実行ファイルに静的ライブラリをリンクできます (追加の依存ファイル設定)。
- [ ] 実行ファイルに動的ライブラリをリンクし、実行時パスを設定できます (`PATH`)。
- [ ] Windows での DLL ビルドに必要な `__declspec(dllexport)` / `__declspec(dllimport)` を理解できます。

## 学習マテリアル

### 公式ドキュメント

- [GCC リンク オプション](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) - `-l`・`-L`・`-shared`・`-fPIC` などのオプション説明 (英語)
- [DLL の作成と使用 (Microsoft Learn)](https://learn.microsoft.com/ja-jp/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp) - Windows DLL の作成チュートリアル (日本語)
- [cppreference - C 言語](https://ja.cppreference.com/w/c) - C 言語リファレンス (日本語)

### チュートリアル・入門

- [P/Invoke の概要 (Microsoft Learn)](https://learn.microsoft.com/ja-jp/dotnet/standard/native-interop/pinvoke) - .NET から C ライブラリを呼び出す仕組みの解説 (日本語)

## 対象ワークスペースとの関連

### 使用箇所 (具体的なファイル・コマンド)

ライブラリの構成:

| ライブラリ    | 種別 | Linux           | Windows           | 実装ファイル                 |
|---------------|------|-----------------|-------------------|------------------------------|
| `libexamplebase` | 静的 | `libexamplebase.a` | `libexamplebase.lib` | `app/example/prod/libsrc/examplebase/` |
| `libexample`     | 動的 | `libexample.so`    | `libexample.dll`     | `app/example/prod/libsrc/example/`     |

Table: ライブラリ構成一覧

各プログラムのリンク方式:

| プログラム               | リンク                     | ソース                                  |
|--------------------------|----------------------------|-----------------------------------------|
| `add`                    | `libexamplebase` を静的リンク | `app/example/prod/src/add/add.c`               |
| `example`                   | `libexample` を動的リンク     | `app/example/prod/src/example/example.c`             |
| `shared-and-static-example` | 両方をリンク               | `app/example/prod/src/shared-and-static-example/` |

Table: 各プログラムのリンク方式

ヘッダー ファイル:

- `app/example/prod/include/libexample_const.h` - 定数定義 (`EXAMPLE_SUCCESS` など)
- `app/example/prod/include/libexamplebase.h` - 静的ライブラリ用ヘッダー
- `app/example/prod/include/libexample.h` - 動的ライブラリ用ヘッダー

### 関連ドキュメント

- [ビルド設計](../../build-design.md) - 対象ワークスペースのビルド構成の詳細
- [GNU Make (スキル ガイド)](../04-build-system/gnu-make.md) - makefile によるビルド自動化
- [C# / P/Invoke (スキル ガイド)](../08-dev-environment/dotnet-csharp.md) - .NET から C ライブラリを呼び出す
