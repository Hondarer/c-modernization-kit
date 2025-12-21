# calc_dotnet - .NET 電卓コマンド

calc コマンドライン電卓の .NET 版です。このアプリケーションは、CalcDotNet ライブラリラッパーを使用して基本的な整数演算を実行します。

## 概要

`calc_dotnet` は、2つの整数と演算子を受け取り、CalcDotNet ラッパーを介してネイティブ calc ライブラリを使用して計算を実行し、結果を出力するコマンドライン電卓です。

## 機能

- **シンプルなコマンドラインインターフェース** - 使いやすい電卓
- **4つの基本演算** - 加算、減算、乗算、除算
- **ネイティブライブラリ統合** - ネイティブ calc ライブラリへの P/Invoke に CalcDotNet ラッパーを使用
- **クロスプラットフォーム** - Windows と Linux の両方で動作
- **エラー処理** - 無効な入力と操作に対する適切なエラーメッセージ

## 要件

### ビルド要件

- .NET SDK 9.0 以降
- CalcDotNet ライブラリ (自動的に参照される)
- ネイティブ calc ライブラリ (Linux では libcalc.so、Windows では calc.dll)

### ランタイム要件

- .NET 9.0 ランタイム
- ネイティブ calc ライブラリが同じディレクトリまたはシステムライブラリパスに配置されていること

## ビルド

```bash
# アプリケーションをビルド
cd prod/calc/src/calc_dotnet
make build

# ビルド成果物をクリーン
make clean

# NuGet パッケージを復元
make restore
```

コンパイルされた実行ファイルは `prod/calc/bin/calc_dotnet` に配置されます。

## 使用方法

```bash
calc_dotnet <num1> <operator> <num2>
```

### 演算子

- `+` - 加算
- `-` - 減算
- `x` - 乗算
- `/` - 除算

### 例

```bash
# 加算
./calc_dotnet 10 + 20
# 出力: 30

# 減算
./calc_dotnet 15 - 5
# 出力: 10

# 乗算
./calc_dotnet 6 x 7
# 出力: 42

# 除算
./calc_dotnet 20 / 4
# 出力: 5

# 除算 (整数除算)
./calc_dotnet 10 / 3
# 出力: 3

# エラーケース - ゼロ除算
./calc_dotnet 10 / 0
# 出力: Error: calcHandler failed
# 終了コード: 1
```

## コマンドライン引数

プログラムは正確に 3 つの引数を必要とします。

1. **num1** - 第 1 整数オペランド
2. **operator** - 単一文字の演算子 (+, -, x, /)
3. **num2** - 第 2 整数オペランド

### 引数の検証

- 3 つすべての引数を提供する必要がある
- 演算子は正確に 1 文字である必要がある
- 第 1 引数と第 3 引数は有効な整数である必要がある
- 演算子は +, -, x, / のいずれかである必要がある

## エラー処理

プログラムは成功または失敗に基づいて異なる終了コードを返します。

- **終了コード 0** - 成功
- **終了コード 1** - エラー (無効な引数、計算失敗など)

### エラーメッセージ

- `Usage: calc_dotnet <arg1> <arg2> <arg3>` - 引数の数が不正、または演算子の形式が無効
- `Error: Invalid number '<value>'` - 無効な整数引数
- `Usage: calc_dotnet <num1> <+|-|x|/> <num2>` - 無効な演算子
- `Error: calcHandler failed` - 計算失敗 (ゼロ除算など)

## 実装の詳細

### アーキテクチャ

```text
calc_dotnet (コンソールアプリ)
    ↓
CalcDotNet (ラッパーライブラリ)
    ↓
libcalc.so / calc.dll (ネイティブライブラリ)
```

### プログラムフロー

1. コマンドライン引数を解析
2. 引数の数と形式を検証
3. 文字列を整数に変換
4. 演算子を CalcKind 列挙型にマッピング
5. CalcLibrary.Calculate() を呼び出し
6. 結果をチェックして値またはエラーを出力
7. 適切な終了コードを返す

### C 版との比較

.NET 版 (`calc_dotnet`) は、C 版 (`calc`) と同じ機能を提供します。

| 機能 | C 版 | .NET 版 |
|------|------|---------|
| コマンドラインインターフェース | ✓ | ✓ |
| 4 つの基本演算 | ✓ | ✓ |
| エラー処理 | ✓ | ✓ |
| 終了コード | ✓ | ✓ |
| ネイティブライブラリ | 直接呼び出し | P/Invoke ラッパー |
| プラットフォームサポート | Windows/Linux | Windows/Linux |
| 整数解析 | atoi() | int.TryParse() |
| エラーメッセージ | fprintf(stderr) | Console.Error |

## プラットフォーム固有の注意事項

### Linux

```bash
# ライブラリパスを設定して実行
LD_LIBRARY_PATH=/path/to/lib:$LD_LIBRARY_PATH ./calc_dotnet 10 + 20
```

Makefile は `make run` を使用すると自動的に `LD_LIBRARY_PATH` を設定します。

### Windows

ネイティブ DLL はビルド時に出力ディレクトリに自動的にコピーされます。追加の設定は不要です。

## ビルドシステムとの統合

アプリケーションは既存の Makefile ビルドシステムに統合されています。

```bash
# プロジェクトルートからビルド
cd prod/calc/src
make  # calc_dotnet を含むすべてのアプリケーションをビルド

# calc_dotnet のみをビルド
cd prod/calc/src/calc_dotnet
make build
```

## トラブルシューティング

### ネイティブライブラリが見つからない

**エラー:**

```text
Error loading native library
```

**解決方法:**

1. ネイティブライブラリがビルドされていることを確認:
   ```bash
   cd prod/calc/libsrc/calc
   make build
   ```
2. ライブラリが `prod/calc/lib/` に存在することを確認
3. Linux では `LD_LIBRARY_PATH` を設定:
   ```bash
   export LD_LIBRARY_PATH=/path/to/c-modernization-kit/prod/calc/lib:$LD_LIBRARY_PATH
   ```

### アプリケーションのビルドが失敗する

**エラー:**

```text
Project reference not found
```

**解決方法:**

1. 最初に CalcDotNet ライブラリをビルド:
   ```bash
   cd prod/calc/libsrc/calc_dotnet
   make build
   ```
2. 次に calc_dotnet をビルド:
   ```bash
   cd prod/calc/src/calc_dotnet
   make build
   ```

## ファイル

- `Program.cs` - メインアプリケーションロジック
- `calc_dotnet.csproj` - プロジェクト設定
- `Makefile` - ビルド統合
- `README.md` - このドキュメント
