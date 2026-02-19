# C ライブラリの種類

## 概要

C 言語では、コードを再利用するためにライブラリとしてまとめる方法が 2 種類あります。**静的ライブラリ**（Linux: `.a`、Windows: `.lib`）は実行ファイルにリンク時に組み込まれ、**動的ライブラリ**（Linux: `.so`、Windows: `.dll`）は実行時に読み込まれます。それぞれにメリット・デメリットがあり、用途に応じて使い分けます。

このリポジトリの `prod/calc/` はこの 2 種類のライブラリを意図的に使い分けています。`libcalcbase`（add / subtract / multiply / divide）は静的ライブラリとして提供され、`libcalc`（calcHandler）は動的ライブラリとして提供されます。`src/shared-and-static-calc/` は両方を同時にリンクするサンプルです。

また、.NET 連携においては `libcalc.dll`（Windows）または `libcalc.so`（Linux）を P/Invoke で呼び出す実装が `prod/calc.net/` に含まれています。ライブラリの種類と動作の違いを理解することは、このリポジトリのビルド構成を理解するための重要なステップです。

## 習得目標

- [ ] 静的ライブラリと動的ライブラリの違いを説明できる
- [ ] GCC で静的ライブラリ（`.a`）をビルドできる（`ar` コマンド）
- [ ] GCC で動的ライブラリ（`.so`）をビルドできる（`-shared -fPIC` オプション）
- [ ] 実行ファイルに静的ライブラリをリンクできる（`-l` / `-L` オプション）
- [ ] 実行ファイルに動的ライブラリをリンクし、実行時パスを設定できる（`LD_LIBRARY_PATH`）
- [ ] Windows での DLL ビルドに必要な `__declspec(dllexport)` / `__declspec(dllimport)` を理解できる
- [ ] `prod/calc/` のライブラリ構成を読み取ることができる

## 学習マテリアル

### 公式ドキュメント

- [GCC リンクオプション](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) — `-l`・`-L`・`-shared`・`-fPIC` などのオプション説明（英語）
- [DLL の作成と使用（Microsoft Learn）](https://learn.microsoft.com/ja-jp/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp) — Windows DLL の作成チュートリアル（日本語）
- [cppreference — C 言語](https://ja.cppreference.com/w/c) — C 言語リファレンス（日本語）

### チュートリアル・入門

- [P/Invoke の概要（Microsoft Learn）](https://learn.microsoft.com/ja-jp/dotnet/standard/native-interop/pinvoke) — .NET から C ライブラリを呼び出す仕組みの解説（日本語）

## このリポジトリとの関連

### 使用箇所（具体的なファイル・コマンド）

ライブラリの構成:

| ライブラリ | 種別 | Linux | Windows | 実装ファイル |
|----------|------|-------|---------|------------|
| `libcalcbase` | 静的 | `libcalcbase.a` | `libcalcbase.lib` | `prod/calc/libsrc/calcbase/` |
| `libcalc` | 動的 | `libcalc.so` | `libcalc.dll` | `prod/calc/libsrc/calc/` |

各プログラムのリンク方式:

| プログラム | リンク | ソース |
|----------|-------|-------|
| `add` | `libcalcbase` を静的リンク | `prod/calc/src/add/add.c` |
| `calc` | `libcalc` を動的リンク | `prod/calc/src/calc/calc.c` |
| `shared-and-static-calc` | 両方をリンク | `prod/calc/src/shared-and-static-calc/` |

ヘッダーファイル:

- `prod/calc/include/libcalc_const.h` — 定数定義（`CALC_SUCCESS` など）
- `prod/calc/include/libcalcbase.h` — 静的ライブラリ用ヘッダー
- `prod/calc/include/libcalc.h` — 動的ライブラリ用ヘッダー

### 関連ドキュメント

- [ビルド設計](../../build-design.md) — このリポジトリのビルド構成の詳細
- [GNU Make（スキルガイド）](../03-build-system/gnu-make.md) — Makefile によるビルド自動化
- [C# / P/Invoke（スキルガイド）](../07-dev-environment/dotnet-csharp.md) — .NET から C ライブラリを呼び出す
