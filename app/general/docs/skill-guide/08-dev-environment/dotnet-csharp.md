# C# / P/Invoke 基礎

## 概要

C# (シーシャープ) は .NET 環境で動作するオブジェクト指向言語です。型安全で例外処理が充実しており、Java に似た構文を持ちます。P/Invoke (Platform Invocation Services) は .NET アプリケーションから C/C++ などで書かれたネイティブ コード (DLL) を呼び出す仕組みです。

対象ワークスペースの `app/example.net/prod/` は C ライブラリ (`libexample`) を .NET から利用するための実装例です。`app/example.net/prod/libsrc/ExampleLib/Internal/NativeMethods.cs` で `[DllImport]` 属性を使った P/Invoke 定義を行い、`ExampleLibrary.cs` がこれをラップして .NET らしいインターフェース (例外・型安全・`ExampleResult` クラス) を提供します。`ExampleApp` がこのライブラリを使うサンプル アプリです。

C 言語開発者が .NET 連携を理解するには、C# の基礎に加えて P/Invoke の仕組みを習得することが重要です。

## 習得目標

- [ ] C# の基本構文 (クラス・メソッド・プロパティ・例外処理) を理解できます。
- [ ] `[DllImport]` 属性の基本的な書き方を理解できます。
- [ ] C の型と C# の型の対応 (`int` → `int`・`char*` → `string`・ポインタ → `ref`/`out`) を理解できます。
- [ ] `NativeMethods.cs` の P/Invoke 定義を読み取ることができます。
- [ ] `ExampleLibrary.cs` のラッパー実装を読み取ることができます。
- [ ] `ExampleException` のような C# カスタム例外を理解できます。
- [ ] `ModuleInitializer.cs` によるネイティブ ライブラリのロード設定を理解できます。

## 学習マテリアル

### 公式ドキュメント

- [C# ドキュメント (Microsoft Learn)](https://learn.microsoft.com/ja-jp/dotnet/csharp/) - C# の公式ドキュメント (日本語)
    - [C# 入門](https://learn.microsoft.com/ja-jp/dotnet/csharp/tour-of-csharp/) - C# 言語のツアー
    - [例外処理](https://learn.microsoft.com/ja-jp/dotnet/csharp/fundamentals/exceptions/) - try/catch の使い方
- [P/Invoke の概要 (Microsoft Learn)](https://learn.microsoft.com/ja-jp/dotnet/standard/native-interop/pinvoke) - P/Invoke の詳細説明 (日本語)
    - [型のマーシャリング](https://learn.microsoft.com/ja-jp/dotnet/standard/native-interop/type-marshalling) - C と C# の型変換
- [.NET Standard の概要](https://learn.microsoft.com/ja-jp/dotnet/standard/net-standard) - .NET バージョン互換性 (日本語)

## 対象ワークスペースとの関連

### 使用箇所 (具体的なファイル・コマンド)

P/Invoke 定義 (`app/example.net/prod/libsrc/ExampleLib/Internal/NativeMethods.cs`) の概要:

```csharp
using System.Runtime.InteropServices;

namespace ExampleLib.Internal;

internal static class NativeMethods
{
    // C 関数: int exampleHandler(int kind, int a, int b, int *result)
    [DllImport("libexample", CallingConvention = CallingConvention.Cdecl)]
    internal static extern int exampleHandler(int kind, int a, int b, out int result);
}
```

C# ラッパー (`app/example.net/prod/libsrc/ExampleLib/ExampleLibrary.cs`) の概要:

```csharp
namespace ExampleLib;

public class ExampleLibrary
{
    public ExampleResult Calculate(ExampleKind kind, int a, int b)
    {
        int nativeResult;
        int status = Internal.NativeMethods.exampleHandler((int)kind, a, b, out nativeResult);

        if (status != 0)
        {
            throw new ExampleException($"計算エラー: status={status}");
        }

        return new ExampleResult(nativeResult);
    }
}
```

ネイティブ ライブラリのロード設定 (`app/example.net/prod/src/ExampleApp/ModuleInitializer.cs`):

```csharp
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace ExampleApp;

internal static class ModuleInitializer
{
    [ModuleInitializer]
    internal static void Initialize()
    {
        // libexample の検索パスを設定
        NativeLibrary.SetDllImportResolver(
            typeof(ExampleLib.ExampleLibrary).Assembly,
            DllImportResolver);
    }
    // ...
}
```

ExampleLib のファイル構成:

```text
app/example.net/prod/libsrc/ExampleLib/
+-- ExampleLibrary.cs          # メインライブラリクラス
+-- ExampleKind.cs             # 計算種別の列挙型
+-- ExampleResult.cs           # 計算結果クラス
+-- ExampleException.cs        # カスタム例外クラス
+-- Internal/
    +-- NativeMethods.cs    # P/Invoke 定義
```

### 関連ドキュメント

- [.NET RelWithDebInfo ビルド](../../dotnet-relwithdebinfo.md) - .NET ビルド設定の詳細
- [.NET SDK (スキル ガイド)](../04-build-system/dotnet-sdk.md) - dotnet コマンドとプロジェクト構造
- [.NET テスト (スキル ガイド)](../05-testing/dotnet-testing.md) - .NET ラッパーの単体テスト
- [C ライブラリの種類 (スキル ガイド)](../03-c-language/c-library-types.md) - P/Invoke で呼び出す DLL の仕組み
