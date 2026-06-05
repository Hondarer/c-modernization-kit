# GCC / MSVC ツールチェーン

## 概要

C 言語のコンパイルには、ソース ファイル (`.c`) を機械語 (`.o`) に変換するコンパイラと、オブジェクト ファイルを実行ファイルやライブラリに結合するリンカーが必要です。Linux では GCC (GNU Compiler Collection) が広く使われ、Windows では MSVC (Microsoft Visual C++) が標準的です。

このリポジトリは GCC (Linux 環境) と MSVC (Windows 環境) の両方をサポートしています。`framework/makefw/` サブモジュールのビルド テンプレートが環境に応じたコンパイラ コマンドを選択します。Linux 環境 (WSL を含む) では `gcc`・`g++`・`ar` コマンドを使用し、Windows 環境では `cl.exe`・`link.exe`・`lib.exe` を使用します。

コンパイラのオプション (デバッグ情報・最適化・警告設定) とリンカーのオプション (ライブラリ パス・ライブラリ名) を理解することで、ビルド エラーの原因特定と Makefile のカスタマイズができるようになります。

## 習得目標

### GCC (Linux)

- [ ] `gcc -c` でソース ファイルをオブジェクト ファイルにコンパイルできる
- [ ] `gcc -o` でオブジェクト ファイルを実行ファイルにリンクできる
- [ ] `-I` (インクルード パス)・`-L` (ライブラリ パス)・`-l` (ライブラリ名) オプションを理解できる
- [ ] `-g` (デバッグ情報)・`-O2` (最適化)・`-Wall` (警告) オプションを使用できる
- [ ] `ar rcs` で静的ライブラリ (`.a`) を作成できる
- [ ] `-shared -fPIC` で動的ライブラリ (`.so`) を作成できる
- [ ] MSVC での同等操作 (`cl.exe`・`/MD`・`/LD`) を理解できる

### MSVC (Windows)

- [ ] `cl.exe /c` でソース ファイルをオブジェクト ファイルにコンパイルできる
- [ ] `link.exe` または `cl.exe` でオブジェクト ファイルを実行ファイルにリンクできる
- [ ] `/I` (インクルード パス)・`/LIBPATH` (ライブラリ パス)・追加の依存ファイル設定を理解できる
- [ ] `/Zi` (デバッグ情報)・`/O2` (最適化)・`/W4` (警告レベル) を使用できる
- [ ] `lib.exe` で静的ライブラリ (`.lib`) を作成できる
- [ ] `cl.exe /LD` で動的ライブラリ (`.dll`) を作成できる
- [ ] `/MD`・`/MT` の違い (CRT リンク方式) を理解できる

## 学習マテリアル

### 公式ドキュメント

- [GCC 公式ドキュメント](https://gcc.gnu.org/onlinedocs/) - GCC の完全なドキュメント (英語)
    - [GCC コンパイル オプション](https://gcc.gnu.org/onlinedocs/gcc/Invoking-GCC.html) - コマンド ライン オプション一覧
    - [GCC リンク オプション](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) - リンカー関連オプション
- [MSVC コンパイラ リファレンス](https://learn.microsoft.com/ja-jp/cpp/build/reference/compiler-command-line-syntax) - MSVC コンパイラの構文 (日本語)

### チュートリアル・入門

- [DLL の作成と使用 (Microsoft Learn)](https://learn.microsoft.com/ja-jp/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp) - Windows での DLL 作成チュートリアル (日本語)

## このリポジトリとの関連

### 使用箇所 (具体的なファイル・コマンド)

Linux (GCC) での代表的なビルド コマンド:

```bash
# オブジェクト ファイルのコンパイル
gcc -c -Wall -Wextra -g -fPIC \
    -I app/calc/prod/include \
    app/calc/prod/libsrc/calcbase/add.c \
    -o add.o

# 静的ライブラリの作成
ar rcs app/calc/prod/lib/libcalcbase.a add.o subtract.o multiply.o divide.o

# 動的ライブラリの作成
gcc -shared -fPIC \
    -o app/calc/prod/lib/libcalc.so \
    calcHandler.o

# 静的リンクで実行ファイルをビルド
gcc -o app/calc/prod/bin/add \
    app/calc/prod/src/add/add.o \
    -L app/calc/prod/lib -lcalcbase

# 動的リンクで実行ファイルをビルド
gcc -o app/calc/prod/bin/calc \
    app/calc/prod/src/calc/calc.o \
    -L app/calc/prod/lib -lcalc \
    -Wl,-rpath,'$$ORIGIN/../lib'
```

カバレッジ計測用のコンパイル オプション (`framework/testfw/` で使用):

```bash
gcc -c --coverage -fprofile-arcs -ftest-coverage ...
```

### 関連ドキュメント

- [GNU Make (スキル ガイド)](gnu-make.md) - makefile でのコンパイラ コマンドの自動化
- [C ライブラリの種類 (スキル ガイド)](../03-c-language/c-library-types.md) - 静的・動的ライブラリのビルド
- [コード カバレッジ (スキル ガイド)](../05-testing/code-coverage.md) - gcov / lcov によるカバレッジ計測
- [WSL / MinGW 環境 (スキル ガイド)](../08-dev-environment/wsl-mingw.md) - Windows での環境構築
