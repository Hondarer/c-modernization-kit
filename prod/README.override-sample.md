# c-modernization-kit サンプル (override-sample)

これは、動的ライブラリのオーバーライド機能を示すサンプルです。

## 概要

`libbase` が公開する `func` 関数は、`useOverride` フラグによって処理を切り替えます。

| useOverride | 動作 |
|---|---|
| `0` | `libbase` 自身が処理を行う (`a + b`) |
| `1` (0 以外) | `dlopen` / `LoadLibrary` で `liboverride` を実行時にロードし、`func_override` に処理を委譲する (`a * b`) |

これにより、メインプログラムを変更せずに、ライブラリの実装を差し替えられることを示します。

## ファイル構成

```text
prod/override-sample/
+-- include/
|   +-- libbase.h             # libbase ヘッダー (func, console_output の宣言)
|   +-- libbase_ext.h         # liboverride ヘッダー (func_override の宣言)
+-- libsrc/
|   +-- base/
|   |   +-- base.c            # func の実装 (useOverride による切り替えロジック)
|   |   +-- console_output.c  # console_output の実装 (printf ラッパー)
|   +-- override/
|       +-- func_override.c   # func_override の実装 (libbase から dlopen で呼ばれる)
+-- src/
|   +-- override-sample/
|       +-- override-sample.c # メインプログラム
+-- lib/                      # ビルド済みライブラリ (libbase.so / liboverride.so / libbase.dll / liboverride.dll)
+-- bin/                      # ビルド済み実行ファイル (override-sample / override-sample.exe)
```

## ライブラリ

### libbase (動的ライブラリ)

`libbase.so` / `libbase.dll` を提供します。

#### func

```c
int func(const int useOverride, const int a, const int b, int *result);
```

- `useOverride == 0` のとき: `*result = a + b` を計算して返す
- `useOverride != 0` のとき: `liboverride.so` / `liboverride.dll` を動的にロードし、`func_override` に処理を委譲する

#### console_output

```c
void console_output(const char *format, ...);
```

`printf` と同じ書式でコンソールに出力する関数です。`liboverride` からも呼び出されます。

### liboverride (動的ライブラリ)

`liboverride.so` / `liboverride.dll` を提供します。`libbase` の `func` から実行時にロードされます。

#### func_override

```c
int func_override(const int useOverride, const int a, const int b, int *result);
```

`func` と同じシグネチャを持ちます。`*result = a * b` を計算して返します。

## 動作の仕組み

```
override-sample (実行ファイル)
    |
    | func(0, 1, 2, &result)        useOverride=0
    +---> libbase.so / libbase.dll
    |         func() が自身で処理
    |         *result = 1 + 2 = 3
    |
    | func(1, 1, 2, &result)        useOverride=1
    +---> libbase.so / libbase.dll
              func() が dlopen / LoadLibrary で liboverride.so / liboverride.dll を呼び出す
                  |
                  +---> liboverride.so / liboverride.dll (実行時ロード)
                            func_override() が処理
                            *result = 1 * 2 = 2
```

## ビルド

```bash
cd prod/override-sample
make
```

ビルド成功後、以下のファイルが生成されます。

| ファイル | 説明 |
|---|---|
| `lib/libbase.so` | ベースライブラリ (Linux) |
| `lib/liboverride.so` | オーバーライドライブラリ (Linux) |
| `bin/override-sample` | メインプログラム (Linux) |
| `lib/libbase.dll` | ベースライブラリ (Windows) |
| `lib/liboverride.dll` | オーバーライドライブラリ (Windows) |
| `bin/override-sample.exe` | メインプログラム (Windows) |

クリーンビルドを行う場合は以下のとおりです。

```bash
make clean && make
```

## 実行

Linux では `LD_LIBRARY_PATH` にライブラリのパスを設定して実行します。

```bash
cd prod/override-sample/bin
LD_LIBRARY_PATH=../lib ./override-sample
```

Windows では `PATH` にライブラリのパスを設定して実行します。

```cmd
cd prod\override-sample\bin
set PATH=%PATH%;..\lib
override-sample.exe
```

### 実行結果

```
func: a=1, b=2 の処理 (*result = a + b;) を行います
rtc: 0
result: 3
func: func_override に移譲します
func_override: a=1, b=2 の処理 (*result = a * b;) を行います
rtc: 0
result: 2
```
