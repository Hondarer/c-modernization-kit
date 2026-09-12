# c-modernization-kit サンプル (override-sample)

これは、動的ライブラリのオーバーライド機能を示すサンプルです。

## 概要

`libbase` が公開する `sample_func` 関数は、起動時に読み込む設定ファイルによって処理を切り替えます。

sym_loader 機構 (関数の動的呼び出しキャッシュ) は `app/c-platform/prod/` の `libcplat` に統合されており、`libbase` はそれをリンクして利用します。

| 設定ファイルの状態 | 動作 |
|---|---|
| 存在しない (または定義なし) | `libbase` 自身が処理を行う (`a + b`) |
| `sample_func` に `liboverride` / `override_func` を定義 | `dlopen` / `LoadLibrary` で `liboverride` を実行時にロードし、`override_func` に処理を委譲する (`a * b`) |

オーバーライドの切り替えは `libbase` がロードされるタイミング (constructor / `DllMain`) で行われます。メイン プログラムを変更せずに、設定ファイルを置くだけでライブラリの実装を差し替えられることを示します。

## ファイル構成

```text
app/override-sample/prod/
+-- include/
|   +-- base/
|   |   +-- base_export.h      # base の DLL エクスポートマクロ
|   |   +-- base_const.h       # base の結果コード定数
|   |   +-- base_spec.h        # base ヘッダー (sample_func, base_console_output 等の宣言)
|   +-- override/
|   |   +-- override_export.h  # override の DLL エクスポートマクロ
|   |   +-- override_spec.h    # override ヘッダー (override_func の宣言)
+-- libsrc/
|   +-- base/
|   |   +-- sym_loader_libbase.h  # sym_loader エントリの extern 宣言
|   |   +-- sym_loader_libbase.c  # sym_loader エントリの実体定義
|   |   +-- sample_func.c      # sample_func の実装 (sym_loader 経由でオーバーライドを呼び出す)
|   |   +-- console_output.c   # base_console_output の実装 (printf ラッパー)
|   |   +-- dllmain_libbase.c  # onLoad / onUnload の実装
|   +-- override/
|       +-- override_func.c    # override_func の実装 (libbase から dlopen で呼ばれる)
+-- src/
|   +-- override-sample/
|       +-- override-sample.c  # メインプログラム
+-- sample-config/
|   +-- libbase_extdef.json    # 設定ファイルのサンプル
+-- lib/                       # ビルド済みライブラリ (libbase.so / liboverride.so / libbase.dll / liboverride.dll)
+-- bin/                       # ビルド済み実行ファイル (override-sample / override-sample.exe)
```

sym_loader 機構・DllMain ヘルパー・ライブラリ パス取得ユーティリティは `app/c-platform/prod/` の `libcplat` (`libcplat.so` / `libcplat.dll`) に統合されています。

## ライブラリ

### libbase (動的ライブラリ)

`libbase.so` / `libbase.dll` を提供します。

#### sample_func

```c
int WINAPI sample_func(const int a, const int b, int *result);
```

- 設定ファイルで `override_func` が定義されているとき: `liboverride.so` / `liboverride.dll` を動的にロードし、`override_func` に処理を委譲します。
- それ以外のとき: `*result = a + b` を計算して返す

#### base_console_output

```c
void WINAPI base_console_output(const char *format, ...);
```

`printf` と同じ書式でコンソールに出力する関数です。  
`liboverride` からも呼び出しています。これは、動的にロードされた拡張処理から基底ライブラリの関数を呼び出すことができることのサンプル実装です。

### liboverride (動的ライブラリ)

`liboverride.so` / `liboverride.dll` を提供します。`libbase` の `sample_func` から設定に応じて実行時にロードされます。

#### override_func

```c
int WINAPI override_func(const int a, const int b, int *result);
```

`sample_func` と同じシグネチャを持ちます。`*result = a * b` を計算して返します。

## 動作の仕組み

### 設定ファイル

`libbase` はロード時 (constructor / `DllMain(DLL_PROCESS_ATTACH)`) に設定ファイルを読み込みます。

| プラットフォーム | 設定ファイル パス |
|---|---|
| Linux | `/tmp/libbase_extdef.json` |
| Windows | `%TEMP%\libbase_extdef.json` |

ファイルが存在しない場合は既定の動作になります。ファイルのフォーマットは次のとおりです。  
`//` 行コメントと C 形式のブロック コメントを利用できます。

```json
// コメント例
{
  "func_key": {
    "lib": "lib_name",
    "func": "func_name"
  }
}
```

| `lib` / `func` の値 | 動作 |
|---|---|
| ともに `default` | 明示的な既定指定。設定ファイルなしと同様に既定の処理を行います。 |
| ライブラリ名 / 関数名 | 指定したライブラリを動的ロードし、関数に処理を委譲します。 |

`sample-config/libbase_extdef.json` に設定ファイルのサンプルがあります。初期状態では明示的な既定指定 (`"lib": "default"`, `"func": "default"`) が設定されており、オーバーライドする定義はコメント アウトされています。

`sample_func` をオーバーライドする場合は次のように記述します。

```json
{
  "sample_func": {
    "lib": "liboverride",
    "func": "override_func"
  }
}
```

### 呼び出しフロー

```text
override-sample (実行ファイル)
    |
    | sample_func(1, 2, &result)
    +---> libbase.so / libbase.dll
              |
              sym_loader が解決済み関数ポインターを確認
              |
              [設定ファイルなし / 定義なし → 既定動作]
              |  *result = 1 + 2 = 3
              |
              [設定ファイルで liboverride / override_func を定義 → オーバーライド動作]
              |  sym_loader が liboverride.so / liboverride.dll を dlopen / LoadLibrary
              |  override_func に処理を委譲
              +---> liboverride.so / liboverride.dll (実行時ロード)
                        *result = 1 * 2 = 2
```

## ビルド

```bash
cd app/override-sample/prod
make
```

ビルド成功後、次のファイルが生成されます。

| ファイル | 説明 |
|---|---|
| `lib/libbase.so` | ベース ライブラリ (Linux) |
| `lib/liboverride.so` | オーバーライド ライブラリ (Linux) |
| `bin/override-sample` | メイン プログラム (Linux) |
| `lib/libbase.dll` | ベース ライブラリ (Windows) |
| `lib/liboverride.dll` | オーバーライド ライブラリ (Windows) |
| `bin/override-sample.exe` | メイン プログラム (Windows) |

クリーン ビルドを行う場合は次のとおりです。

```bash
make clean && make
```

## 実行

### Linux

#### 既定動作 (設定ファイルなし)

```bash
cd app/override-sample/prod/cbin
LD_LIBRARY_PATH=../lib ./override-sample
```

出力例:

```text
sample_func: a=1, b=2 の処理 (*result = a + b;) を行います
result: 3
```

#### オーバーライド動作 (設定ファイルあり)

`sample-config/libbase_extdef.json` をコピーして編集するか、次のように直接書き込みます。

```bash
printf '%s\n' '{"sample_func":{"lib":"liboverride","func":"override_func"}}' > /tmp/libbase_extdef.json
cd app/override-sample/prod/cbin
LD_LIBRARY_PATH=../lib ./override-sample
```

出力例:

```text
sample_func: 拡張処理が見つかりました。拡張処理に移譲します
override_func: a=1, b=2 の処理 (*result = a * b;) を行います
result: 2
```

### Windows

#### 既定動作 (設定ファイルなし)

```cmd
cd prod\override-sample\bin
set PATH=%PATH%;..\lib
override-sample.exe
```

#### オーバーライド動作 (設定ファイルあり)

`sample-config\libbase_extdef.json` をコピーして編集するか、次のように直接書き込みます。

```cmd
echo {"sample_func":{"lib":"liboverride","func":"override_func"}} > %TEMP%\libbase_extdef.json
cd prod\override-sample\bin
set PATH=%PATH%;..\lib
override-sample.exe
```
