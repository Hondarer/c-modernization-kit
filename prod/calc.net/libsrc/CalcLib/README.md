# CalcLib - calc ライブラリ用 .NET ラッパー

CalcLib は、ネイティブ calc ライブラリ (Linux では libcalc.so、Windows では calc.dll) 用の .NET Standard 2.0 ラッパーです。基本的な整数演算を実行するための、クリーンで慣用的な C# API を提供します。

## 機能

- **.NET Standard 2.0** - .NET Core、.NET 5+、.NET Framework 4.6.1+ と互換性あり
- **クロスプラットフォーム** - Windows と Linux の両方をサポート
- **P/Invoke** - 最大のパフォーマンスのためにネイティブライブラリと直接相互運用
- **2つの API スタイル** - 結果オブジェクトまたは例外、ニーズに合わせて選択可能
- **型安全** - 列挙型と結果クラスによる強い型付け
- **充実したドキュメント** - IntelliSense 用の包括的な XML ドキュメント

## 要件

### ビルド要件

- .NET SDK 5.0 以降
- ネイティブ calc ライブラリ (Linux では libcalc.so、Windows では calc.dll)

### ランタイム要件

- .NET Standard 2.0 互換ランタイム
- ネイティブ calc ライブラリが同じディレクトリまたはシステムライブラリパスに配置されていること

## ビルド

```bash
# ライブラリをビルド
cd prod/calc.net/libsrc/CalcLib
make build

# ビルド成果物をクリーン
make clean

# NuGet パッケージを復元
make restore
```

コンパイルされたライブラリは `prod/calc.net/lib/CalcLib.dll` に配置されます。

## 使用方法

### 基本的な使用方法

```csharp
using CalcLib;

// 結果オブジェクトを使用 (エラー処理に推奨)
var result = CalcLibrary.Add(10, 20);
if (result.IsSuccess)
{
    Console.WriteLine($"Result: {result.Value}"); // 出力: 30
}
else
{
    Console.WriteLine($"Error: {result.ErrorCode}");
}
```

### 例外を使用

```csharp
using CalcLib;

try
{
    int result = CalcLibrary.CalculateOrThrow(CalcKind.Divide, 10, 0);
}
catch (CalcException ex)
{
    Console.WriteLine($"Calculation failed: {ex.Message}");
    Console.WriteLine($"Error code: {ex.ErrorCode}");
}
```

### 便利メソッド

```csharp
using CalcLib;

var addResult = CalcLibrary.Add(5, 3);        // 8
var subResult = CalcLibrary.Subtract(10, 4);  // 6
var mulResult = CalcLibrary.Multiply(6, 7);   // 42
var divResult = CalcLibrary.Divide(20, 5);    // 4
```

## API リファレンス

### CalcLibrary クラス

すべての計算操作のメインエントリポイント。

#### メソッド

- `CalcResult Calculate(CalcKind kind, int a, int b)`
  - 計算を実行し、結果オブジェクトを返す
  - 失敗時に例外をスローしない

- `int CalculateOrThrow(CalcKind kind, int a, int b)`
  - 計算を実行し、結果を返す
  - 失敗時に `CalcException` をスロー

- `CalcResult Add(int a, int b)` - 加算 (a + b)
- `CalcResult Subtract(int a, int b)` - 減算 (a - b)
- `CalcResult Multiply(int a, int b)` - 乗算 (a * b)
- `CalcResult Divide(int a, int b)` - 除算 (a / b)

### CalcKind 列挙型

計算操作の種別を指定。

- `Add = 1` - 加算
- `Subtract = 2` - 減算
- `Multiply = 3` - 乗算
- `Divide = 4` - 除算

### CalcResult クラス

計算操作の結果を表す。

#### プロパティ

- `bool IsSuccess` - 操作が成功したかどうかを示す
- `int Value` - 計算結果 (`IsSuccess` が true の場合のみ有効)
- `int ErrorCode` - エラーコード (0 = 成功、-1 = エラー)

### CalcException クラス

計算が失敗したときにスローされる例外。

#### プロパティ

- `int ErrorCode` - ネイティブライブラリが返すエラーコード
- `string Message` - 詳細なエラーメッセージ

## エラー処理

ネイティブ calc ライブラリは以下のエラーコードを返します。

- `0` (CALC_SUCCESS) - 操作成功
- `-1` (CALC_ERROR) - 操作失敗

一般的な失敗シナリオ:

- **ゼロ除算** - 任意の数をゼロで除算
- **無効な操作種別** - サポートされていない操作を使用 (列挙型を使用している場合は発生しない)

## プラットフォーム固有の注意事項

### Windows

- `calc.dll` を使用
- 呼び出し規約: `__stdcall` (WINAPI)
- ライブラリは実行ファイルと同じディレクトリまたはシステム PATH に配置する必要がある

### Linux

- `libcalc.so` を使用
- 呼び出し規約: `cdecl` (デフォルト)
- ライブラリは実行ファイルと同じディレクトリまたは `LD_LIBRARY_PATH` に配置する必要がある

## スレッドセーフティ

このラッパーのスレッドセーフティは、基盤となるネイティブライブラリに依存します。現在の実装はスレッドセーフティについて何も仮定せず、同期を追加しません。ネイティブライブラリがスレッドセーフでない場合は、`CalcLibrary` メソッドの呼び出しの周りに適切なロックを追加する必要があります。

## 既知の制限事項

1. **整数オーバーフロー** - オーバーフローチェックは実行されません。オーバーフロー時に結果が折り返される可能性があります。
2. **整数除算のみ** - 除算は整数除算です (ゼロに向かって切り捨て)。
3. **限定的なエラー情報** - ネイティブライブラリは成功/失敗のみを返し、詳細なエラーメッセージはありません。

## 例

### 結果オブジェクトによるエラー処理

```csharp
var result = CalcLibrary.Divide(10, 0);
if (!result.IsSuccess)
{
    Console.WriteLine("Division by zero detected!");
    Console.WriteLine($"Error code: {result.ErrorCode}");
}
```

### すべての操作を使用

```csharp
var operations = new[]
{
    (CalcKind.Add, 10, 20, "10 + 20"),
    (CalcKind.Subtract, 30, 15, "30 - 15"),
    (CalcKind.Multiply, 5, 6, "5 * 6"),
    (CalcKind.Divide, 100, 4, "100 / 4")
};

foreach (var (kind, a, b, description) in operations)
{
    var result = CalcLibrary.Calculate(kind, a, b);
    if (result.IsSuccess)
    {
        Console.WriteLine($"{description} = {result.Value}");
    }
}
```
