# C# / P/Invoke 基礎

## 概要

C# (シーシャープ) は .NET 環境で動作するオブジェクト指向言語です。型安全で例外処理が充実しており、Java に似た構文を持ちます。P/Invoke (Platform Invocation Services) は .NET アプリケーションから C/C++ などで書かれたネイティブコード (DLL) を呼び出す仕組みです。

このリポジトリの `prod/calc.net/` は C ライブラリ(`libcalc`)を .NET から利用するための実装例です。`prod/calc.net/libsrc/CalcLib/Internal/NativeMethods.cs` で `[DllImport]` 属性を使った P/Invoke 定義を行い、`CalcLibrary.cs` がこれをラップして .NET らしいインターフェース (例外・型安全・`CalcResult` クラス) を提供します。`CalcApp` がこのライブラリを使うサンプルアプリです。

C 言語開発者が .NET 連携を理解するには、C# の基礎に加えて P/Invoke の仕組みを習得することが重要です。

## 習得目標

- [ ] C# の基本構文 (クラス・メソッド・プロパティ・例外処理) を理解できる
- [ ] `[DllImport]` 属性の基本的な書き方を理解できる
- [ ] C の型と C# の型の対応(`int` → `int`・`char*` → `string`・ポインタ → `ref`/`out`)を理解できる
- [ ] `NativeMethods.cs` の P/Invoke 定義を読み取れる
- [ ] `CalcLibrary.cs` のラッパー実装を読み取れる
- [ ] `CalcException` のような C# カスタム例外を理解できる
- [ ] `ModuleInitializer.cs` によるネイティブライブラリのロード設定を理解できる

## 学習マテリアル

### 公式ドキュメント

- [C# ドキュメント(Microsoft Learn)](https://learn.microsoft.com/ja-jp/dotnet/csharp/) - C# の公式ドキュメント (日本語)
  - [C# 入門](https://learn.microsoft.com/ja-jp/dotnet/csharp/tour-of-csharp/) - C# 言語のツアー
  - [例外処理](https://learn.microsoft.com/ja-jp/dotnet/csharp/fundamentals/exceptions/) - try/catch の使い方
- [P/Invoke の概要(Microsoft Learn)](https://learn.microsoft.com/ja-jp/dotnet/standard/native-interop/pinvoke) - P/Invoke の詳細説明 (日本語)
  - [型のマーシャリング](https://learn.microsoft.com/ja-jp/dotnet/standard/native-interop/type-marshalling) - C と C# の型変換
- [.NET Standard の概要](https://learn.microsoft.com/ja-jp/dotnet/standard/net-standard) - .NET バージョン互換性 (日本語)

## このリポジトリとの関連

### 使用箇所 (具体的なファイル・コマンド)

P/Invoke 定義 (`prod/calc.net/libsrc/CalcLib/Internal/NativeMethods.cs`) の概要:

```csharp
using System.Runtime.InteropServices;

namespace CalcLib.Internal;

internal static class NativeMethods
{
    // C 関数: int calcHandler(int kind, int a, int b, int *result)
    [DllImport("libcalc", CallingConvention = CallingConvention.Cdecl)]
    internal static extern int calcHandler(int kind, int a, int b, out int result);
}
```

C# ラッパー (`prod/calc.net/libsrc/CalcLib/CalcLibrary.cs`) の概要:

```csharp
namespace CalcLib;

public class CalcLibrary
{
    public CalcResult Calculate(CalcKind kind, int a, int b)
    {
        int nativeResult;
        int status = Internal.NativeMethods.calcHandler((int)kind, a, b, out nativeResult);

        if (status != 0)
        {
            throw new CalcException($"計算エラー: status={status}");
        }

        return new CalcResult(nativeResult);
    }
}
```

ネイティブライブラリのロード設定 (`prod/calc.net/src/CalcApp/ModuleInitializer.cs`):

```csharp
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace CalcApp;

internal static class ModuleInitializer
{
    [ModuleInitializer]
    internal static void Initialize()
    {
        // libcalc の検索パスを設定
        NativeLibrary.SetDllImportResolver(
            typeof(CalcLib.CalcLibrary).Assembly,
            DllImportResolver);
    }
    // ...
}
```

CalcLib のファイル構成:

```text
prod/calc.net/libsrc/CalcLib/
+-- CalcLibrary.cs          # メインライブラリクラス
+-- CalcKind.cs             # 計算種別の列挙型
+-- CalcResult.cs           # 計算結果クラス
+-- CalcException.cs        # カスタム例外クラス
+-- Internal/
    +-- NativeMethods.cs    # P/Invoke 定義
```

### 関連ドキュメント

- [.NET RelWithDebInfo ビルド](../../dotnet-relwithdebinfo.md) - .NET ビルド設定の詳細
- [.NET SDK(スキルガイド)](../04-build-system/dotnet-sdk.md) - dotnet コマンドとプロジェクト構造
- [.NET テスト(スキルガイド)](../05-testing/dotnet-testing.md) - .NET ラッパーの単体テスト
- [C ライブラリの種類(スキルガイド)](../03-c-language/c-library-types.md) - P/Invoke で呼び出す DLL の仕組み
