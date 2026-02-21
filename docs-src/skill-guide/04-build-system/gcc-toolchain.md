# GCC / MSVC ツールチェイン

## 概要

C 言語のコンパイルには、ソースファイル (`.c`) を機械語 (`.o`) に変換するコンパイラと、オブジェクトファイルを実行ファイルやライブラリに結合するリンカが必要です。Linux では GCC (GNU Compiler Collection) が広く使われ、Windows では MSVC (Microsoft Visual C++)  が標準的です。

このリポジトリは GCC (Linux 環境) と MSVC (Windows 環境) の両方をサポートしています。`makefw/` サブモジュールのビルドテンプレートが環境に応じたコンパイラコマンドを選択します。Linux 環境 (WSL を含む) では `gcc`・`g++`・`ar` コマンドを使用し、Windows 環境では `cl.exe`・`link.exe`・`lib.exe` を使用します。

コンパイラのオプション (デバッグ情報・最適化・警告設定) とリンカのオプション (ライブラリパス・ライブラリ名) を理解することで、ビルドエラーの原因特定と Makefile のカスタマイズができるようになります。

## 習得目標

### GCC (Linux)

- [ ] `gcc -c` でソースファイルをオブジェクトファイルにコンパイルできる
- [ ] `gcc -o` でオブジェクトファイルを実行ファイルにリンクできる
- [ ] `-I` (インクルードパス)・`-L` (ライブラリパス)・`-l` (ライブラリ名) オプションを理解できる
- [ ] `-g` (デバッグ情報)・`-O2` (最適化)・`-Wall` (警告)オプションを使用できる
- [ ] `ar rcs` で静的ライブラリ (`.a`) を作成できる
- [ ] `-shared -fPIC` で動的ライブラリ (`.so`) を作成できる
- [ ] MSVC での同等操作 (`cl.exe`・`/MD`・`/LD`) を理解できる

### MSVC (Windows)

- [ ] `cl.exe /c` でソースファイルをオブジェクトファイルにコンパイルできる
- [ ] `link.exe` または `cl.exe` でオブジェクトファイルを実行ファイルにリンクできる
- [ ] `/I` (インクルードパス)・`/LIBPATH` (ライブラリパス)・追加の依存ファイル設定を理解できる
- [ ] `/Zi` (デバッグ情報)・`/O2` (最適化)・`/W4` (警告レベル) を使用できる
- [ ] `lib.exe` で静的ライブラリ (`.lib`) を作成できる
- [ ] `cl.exe /LD` で動的ライブラリ (`.dll`) を作成できる
- [ ] `/MD`・`/MT` の違い (CRT リンク方式) を理解できる

## 学習マテリアル

### 公式ドキュメント

- [GCC 公式ドキュメント](https://gcc.gnu.org/onlinedocs/) - GCC の完全なドキュメント (英語)
  - [GCC コンパイルオプション](https://gcc.gnu.org/onlinedocs/gcc/Invoking-GCC.html) - コマンドラインオプション一覧
  - [GCC リンクオプション](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) - リンカ関連オプション
- [MSVCコンパイラリファレンス](https://learn.microsoft.com/ja-jp/cpp/build/reference/compiler-command-line-syntax) - MSVC コンパイラの構文 (日本語)

### チュートリアル・入門

- [DLL の作成と使用 (Microsoft Learn)](https://learn.microsoft.com/ja-jp/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp) - Windows での DLL 作成チュートリアル (日本語)

## このリポジトリとの関連

### 使用箇所 (具体的なファイル・コマンド)

Linux (GCC) での代表的なビルドコマンド:

```bash
# オブジェクトファイルのコンパイル
gcc -c -Wall -Wextra -g -fPIC \
    -I prod/calc/include \
    prod/calc/libsrc/calcbase/add.c \
    -o add.o

# 静的ライブラリの作成
ar rcs prod/calc/lib/libcalcbase.a add.o subtract.o multiply.o divide.o

# 動的ライブラリの作成
gcc -shared -fPIC \
    -o prod/calc/lib/libcalc.so \
    calcHandler.o

# 静的リンクで実行ファイルをビルド
gcc -o prod/calc/bin/add \
    prod/calc/src/add/add.o \
    -L prod/calc/lib -lcalcbase

# 動的リンクで実行ファイルをビルド
gcc -o prod/calc/bin/calc \
    prod/calc/src/calc/calc.o \
    -L prod/calc/lib -lcalc \
    -Wl,-rpath,'$$ORIGIN/../lib'
```

カバレッジ計測用のコンパイルオプション (`testfw/` で使用):

```bash
gcc -c --coverage -fprofile-arcs -ftest-coverage ...
```

### 関連ドキュメント

- [GNU Make (スキルガイド)](gnu-make.md) - makefile でのコンパイラコマンドの自動化
- [C ライブラリの種類 (スキルガイド)](../03-c-language/c-library-types.md) - 静的・動的ライブラリのビルド
- [コードカバレッジ (スキルガイド)](../05-testing/code-coverage.md) - gcov / lcov によるカバレッジ計測
- [WSL / MinGW 環境 (スキルガイド)](../08-dev-environment/wsl-mingw.md) - Windows での環境構築
