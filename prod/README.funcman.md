# c-modernization-kit (funcman)

これは、関数の動的呼び出し機構 (funcman) を提供する静的ライブラリです。

## 概要

`libfuncman.a` は、テキスト設定ファイルに基づいて実行時に関数を動的に差し替えるキャッシュ機構を提供します。

| コンポーネント | 役割 |
|---|---|
| `funcman_object` 構造体 | 関数ポインタ・ライブラリ名・ロード状態を保持するキャッシュエントリ |
| `funcman_init` | 設定ファイルを読み込み、`funcman_object` 配列を初期化する |
| `funcman_get_func` | キャッシュから関数ポインタを取得する (スレッドセーフ) |
| `funcman_dispose` | ロードしたライブラリをすべて解放する |
| `dllmain-util.h` | Linux/Windows 共通の DLL ロード・アンロードフックヘッダー |

設定ファイルを置くだけで動的ライブラリのオーバーライドが可能となり、メインプログラムを変更せずにライブラリの実装を差し替えることができます。

## ファイル構成

```text
prod/funcman/
+-- include/
|   +-- funcman.h              # funcman 公開 API ヘッダー
|   +-- dllmain-util.h         # 汎用 DLL ロード・アンロードフックヘッダー
+-- libsrc/
|   +-- funcman_init.c         # 設定ファイルからの初期化
|   +-- funcman_get_func.c     # 関数ポインタ取得 (スレッドセーフ)
|   +-- funcman_dispose.c      # クリーンアップ処理
|   +-- funcman_info.c         # 情報表示
|   +-- funcman_is_declared_default.c  # 明示的デフォルト判定
|   +-- get_lib_info.c         # ライブラリパス取得ユーティリティ
|   +-- makefile               # ビルド makefile
|   +-- makepart.mk            # ビルド設定 (TARGET, OUTPUT_DIR)
+-- lib/                       # ビルド済みライブラリ (libfuncman.a)
```

## API

### funcman_object

```c
typedef struct {
    const char *func_key;             // 識別キー
    char lib_name[FUNCMAN_NAME_MAX];  // 拡張子なしライブラリ名
    char func_name[FUNCMAN_NAME_MAX]; // 関数シンボル名
    MODULE_HANDLE handle;             // キャッシュ済みハンドル
    void *func_ptr;                   // キャッシュ済み関数ポインタ
    int resolved;                     // 解決済フラグ
    // ...
} funcman_object;
```

静的変数として定義する場合は `NEW_FUNCMAN_OBJECT` マクロで初期化します。

```c
static funcman_object fo_my_func = NEW_FUNCMAN_OBJECT("my_func", my_func_t);
```

### funcman_init

```c
void funcman_init(funcman_object *const *fobj_array, size_t fobj_length, const char *configpath);
```

設定ファイルを読み込み、`funcman_object` 配列の `lib_name` / `func_name` を設定します。
constructor / `DllMain(DLL_PROCESS_ATTACH)` から呼び出してください。

設定ファイルのフォーマット:

```text
# コメント行 (# 以降はコメント)
func_key  lib_name  func_name
```

| lib_name / func_name の値 | 動作 |
|---|---|
| ともに `default` | 明示的デフォルト。デフォルト処理を行う |
| ライブラリ名 / 関数名 | 指定したライブラリを動的ロードし、関数に処理を委譲する |

### funcman_get_func

```c
#define funcman_get_func(fobj, type) ((type)_funcman_get_func(fobj))
```

キャッシュから関数ポインタを取得します。スレッドセーフです。
未ロードの場合は `dlopen` / `LoadLibrary` でライブラリをロードします。

```c
my_func_t fn = funcman_get_func(&fo_my_func, my_func_t);
if (fn != NULL) {
    fn(a, b, &result);
}
```

### funcman_is_declared_default

```c
int funcman_is_declared_default(const funcman_object *fobj);
```

`lib_name` / `func_name` がともに `default` の場合に 1 を返します。

### funcman_dispose

```c
void funcman_dispose(funcman_object *const *fobj_array, size_t fobj_length);
```

ロードしたすべての共有ライブラリを解放します。
destructor / `DllMain(DLL_PROCESS_DETACH)` から呼び出してください。

### funcman_info

```c
int funcman_info(funcman_object *const *fobj_array, size_t fobj_length);
```

各 `funcman_object` の状態を標準出力に表示します。
すべてのエントリが正常に解決されていれば 0、1 つでも失敗していれば -1 を返します。

### get_lib_path / get_lib_basename

```c
int get_lib_path(char *out_path, size_t out_path_sz, const void *func_addr);
int get_lib_basename(char *out_basename, size_t out_basename_sz, const void *func_addr);
```

指定した関数が所属する共有ライブラリの絶対パスまたは basename (パスなし・拡張子なし) を取得します。
設定ファイルパスの動的構築などに使用します。

## dllmain-util.h

`dllmain-util.h` をインクルードした `.c` ファイルに、プラットフォーム別の DLL ロード・アンロードフックを提供します。

インクルード元の `.c` ファイルは以下の 2 関数を定義する必要があります。

```c
static void onLoad(void);    // ライブラリロード時に呼び出される
static void onUnload(int process_terminating);  // ライブラリアンロード時に呼び出される
```

| プラットフォーム | フックの仕組み |
|---|---|
| Linux | `__attribute__((constructor/destructor))` により `onLoad` / `onUnload` を自動呼び出し |
| Windows | `DllMain` の `DLL_PROCESS_ATTACH` / `DLL_PROCESS_DETACH` により呼び出し |

## 使用方法

新しいオーバーライド対応ライブラリ (`libfoo.so`) を作成する場合の手順です。

### 1. ビルド設定

`makepart.mk` に以下を追加します。

```makefile
LIBS   += funcman
LIBSDIR += $(WORKSPACE_FOLDER)/prod/funcman/lib
```

### 2. funcman_object の定義

```c
#include <funcman.h>

typedef int (*foo_func_t)(int, int, int *);

static funcman_object fo_foo_func = NEW_FUNCMAN_OBJECT("foo_func", foo_func_t);
static funcman_object *const fobj_array[] = { &fo_foo_func };
static const size_t fobj_length = sizeof(fobj_array) / sizeof(fobj_array[0]);
```

### 3. DllMain / constructor での初期化

```c
#include <dllmain-util.h>

static void onLoad(void) {
    funcman_init(fobj_array, fobj_length, "/tmp/libfoo_extdef.txt");
}

static void onUnload(int process_terminating) {
    (void)process_terminating;
    funcman_dispose(fobj_array, fobj_length);
}
```

### 4. 関数の実装

```c
int foo_func(int a, int b, int *result) {
    foo_func_t fn = funcman_get_func(&fo_foo_func, foo_func_t);
    if (fn != NULL) {
        return fn(a, b, result);  // オーバーライド処理に委譲
    }
    // デフォルト処理
    *result = a + b;
    return 0;
}
```

## ビルド

```bash
cd prod/funcman/libsrc
make
```

ビルド成功後、以下のファイルが生成されます。

| ファイル | 説明 |
|---|---|
| `lib/libfuncman.a` | funcman 静的ライブラリ |

クリーンビルドを行う場合は以下のとおりです。

```bash
make clean && make
```
