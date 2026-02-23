# c-modernization-kit サンプル (override-sample)

これは、動的ライブラリのオーバーライド機能を示すサンプルです。

## 概要

`libbase` が公開する `sample_func` 関数は、起動時に読み込む設定ファイルによって処理を切り替えます。

| 設定ファイルの状態 | 動作 |
|---|---|
| 存在しない (または定義なし) | `libbase` 自身が処理を行う (`a + b`) |
| `sample_func liboverride override_func` を定義 | `dlopen` / `LoadLibrary` で `liboverride` を実行時にロードし、`override_func` に処理を委譲する (`a * b`) |

オーバーライドの切り替えは `libbase` がロードされるタイミング (constructor / `DllMain`) で行われます。メインプログラムを変更せずに、設定ファイルを置くだけでライブラリの実装を差し替えられることを示します。

## ファイル構成

```text
prod/override-sample/
+-- include/
|   +-- libbase.h              # libbase ヘッダー (sample_func, console_output 等の宣言、funcman 型定義)
|   +-- libbase_ext.h          # liboverride ヘッダー (override_func の宣言)
|   +-- dllmain.h              # 汎用 DLL ロード・アンロードフックヘッダー
+-- libsrc/
|   +-- base/
|   |   +-- funcman/           # 関数ポインタキャッシュ管理 (funcman)
|   |   |   +-- funcman_init.c          # 設定ファイルからの初期化
|   |   |   +-- funcman_get_func.c      # 関数ポインタ取得 (スレッドセーフ)
|   |   |   +-- funcman_is_declared_default.c  # 明示的デフォルト判定
|   |   |   +-- funcman_dispose.c       # クリーンアップ処理
|   |   |   +-- funcman_info.c          # 情報表示
|   |   +-- funcman_libbase.h  # funcman オブジェクトの extern 宣言
|   |   +-- funcman_libbase.c  # funcman オブジェクトの実体定義
|   |   +-- sample_func.c      # sample_func の実装 (funcman 経由でオーバーライドを呼び出す)
|   |   +-- console_output.c   # console_output の実装 (printf ラッパー)
|   |   +-- dllmain_libbase.c  # onLoad / onUnload の実装
|   |   +-- get_lib_info.c     # ライブラリパス取得ユーティリティ
|   +-- override/
|       +-- override_func.c    # override_func の実装 (libbase から dlopen で呼ばれる)
+-- src/
|   +-- override-sample/
|       +-- override-sample.c  # メインプログラム
+-- sample-config/
|   +-- libbase_extdef.txt     # 設定ファイルのサンプル
+-- lib/                       # ビルド済みライブラリ (libbase.so / liboverride.so / libbase.dll / liboverride.dll)
+-- bin/                       # ビルド済み実行ファイル (override-sample / override-sample.exe)
```

## ライブラリ

### libbase (動的ライブラリ)

`libbase.so` / `libbase.dll` を提供します。

#### sample_func

```c
int WINAPI sample_func(const int a, const int b, int *result);
```

- 設定ファイルで `override_func` が定義されているとき: `liboverride.so` / `liboverride.dll` を動的にロードし、`override_func` に処理を委譲する
- それ以外のとき: `*result = a + b` を計算して返す

#### console_output

```c
void WINAPI console_output(const char *format, ...);
```

`printf` と同じ書式でコンソールに出力する関数です。`liboverride` からも呼び出せます。

#### get_lib_path / get_lib_basename

```c
int WINAPI get_lib_path(char *out_path, const size_t out_path_sz, const void *func_addr);
int WINAPI get_lib_basename(char *out_basename, const size_t out_basename_sz, const void *func_addr);
```

指定した関数が所属する共有ライブラリの絶対パスまたは basename (パスなし・拡張子なし) を取得します。設定ファイルパスの構築に内部で使用しています。

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

| プラットフォーム | 設定ファイルパス |
|---|---|
| Linux | `/tmp/libbase_extdef.txt` |
| Windows | `%TEMP%\libbase_extdef.txt` |

ファイルが存在しない場合はデフォルト動作になります。ファイルのフォーマットは以下のとおりです。

```text
# コメント行 (# 以降はコメント)
func_key  lib_name  func_name
```

| lib_name / func_name の値 | 動作 |
|---|---|
| ともに `default` | 明示的デフォルト。設定ファイルなしと同様にデフォルト処理を行う |
| ライブラリ名 / 関数名 | 指定したライブラリを動的ロードし、関数に処理を委譲する |

`sample-config/libbase_extdef.txt` に設定ファイルのサンプルがあります。初期状態では `default default` (明示的デフォルト) が設定されており、オーバーライドする行はコメントアウトされています。

`sample_func` をオーバーライドする場合は以下の行を記述します。

```text
sample_func  liboverride  override_func
```

### 呼び出しフロー

```text
override-sample (実行ファイル)
    |
    | sample_func(1, 2, &result)
    +---> libbase.so / libbase.dll
              |
              funcman が解決済み関数ポインタを確認
              |
              [設定ファイルなし / 定義なし → デフォルト動作]
              |  *result = 1 + 2 = 3
              |
              [設定ファイルで liboverride / override_func を定義 → オーバーライド動作]
              |  funcman が liboverride.so / liboverride.dll を dlopen / LoadLibrary
              |  override_func に処理を委譲
              +---> liboverride.so / liboverride.dll (実行時ロード)
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

### Linux

#### デフォルト動作 (設定ファイルなし)

```bash
cd prod/override-sample/bin
LD_LIBRARY_PATH=../lib ./override-sample
```

出力例:

```text
sample_func: a=1, b=2 の処理 (*result = a + b;) を行います
result: 3
```

#### オーバーライド動作 (設定ファイルあり)

`sample-config/libbase_extdef.txt` を `/tmp` にコピーし、オーバーライド行のコメントを外して配置します。

```bash
cp prod/override-sample/sample-config/libbase_extdef.txt /tmp/libbase_extdef.txt
# エディタで /tmp/libbase_extdef.txt を編集し、以下の行のコメントを外す
# sample_func  liboverride  override_func
cd prod/override-sample/bin
LD_LIBRARY_PATH=../lib ./override-sample
```

または直接書き込む場合は以下のとおりです。

```bash
echo "sample_func liboverride override_func" > /tmp/libbase_extdef.txt
cd prod/override-sample/bin
LD_LIBRARY_PATH=../lib ./override-sample
```

出力例:

```text
sample_func: 拡張処理が見つかりました。拡張処理に移譲します
override_func: a=1, b=2 の処理 (*result = a * b;) を行います
result: 2
```

### Windows

#### デフォルト動作 (設定ファイルなし)

```cmd
cd prod\override-sample\bin
set PATH=%PATH%;..\lib
override-sample.exe
```

#### オーバーライド動作 (設定ファイルあり)

`sample-config\libbase_extdef.txt` を `%TEMP%` にコピーし、オーバーライド行のコメントを外して配置します。

```cmd
copy prod\override-sample\sample-config\libbase_extdef.txt %TEMP%\libbase_extdef.txt
rem エディタで %TEMP%\libbase_extdef.txt を編集し、以下の行のコメントを外す
rem sample_func  liboverride  override_func
cd prod\override-sample\bin
set PATH=%PATH%;..\lib
override-sample.exe
```

または直接書き込む場合は以下のとおりです。

```cmd
echo sample_func liboverride override_func > %TEMP%\libbase_extdef.txt
cd prod\override-sample\bin
set PATH=%PATH%;..\lib
override-sample.exe
```
