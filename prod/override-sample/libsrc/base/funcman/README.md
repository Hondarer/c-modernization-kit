# funcman

動的ライブラリのロードと関数ポインタのキャッシュを管理する汎用モジュールです。  
funcman は function manager の略です。

## 設計方針

funcman はモジュール内にインスタンス変数を持ちません。管理対象の状態はすべて呼び出し側が用意した `funcman_object` に保持されます。そのため、このフォルダのソースコードは `libbase` に限らず任意のライブラリから利用できます。

利用するライブラリ側では、管理対象の関数ごとに `funcman_object` を定義し、それを配列にまとめて funcman の各関数に渡します。`libbase` における実体定義とその配列の管理は `funcman_libbase.c` / `funcman_libbase.h` が担っています。

## ファイル構成

| ファイル | 内容 |
|---|---|
| `funcman_init.c` | 設定ファイルを解析し、`funcman_object` の `lib_name` / `func_name` を設定する |
| `funcman_get_func.c` | `lib_name` / `func_name` をもとに動的ライブラリをロードし、関数ポインタを返す。スレッドセーフ |
| `funcman_is_declared_default.c` | `funcman_object` が明示的デフォルト (`lib_name` / `func_name` ともに `"default"`) かどうかを返す |
| `funcman_dispose.c` | ロード済みライブラリをアンロードしてリソースを解放する |
| `funcman_info.c` | `funcman_object` の現在の状態を標準出力に表示する |
| `load-unload-notes.md` | 動的ライブラリのロード・アンロードに関する実装メモ |

## API

型・構造体・マクロを含む宣言はすべて `libbase.h` に集約されています。

| API | 種別 | 説明 |
|---|---|---|
| `funcman_init` | 関数 | 設定ファイルを読み込み、`funcman_object` 配列を初期化する |
| `funcman_dispose` | 関数 | `funcman_object` 配列のハンドルをすべて解放する |
| `funcman_info` | 関数 | `funcman_object` 配列の内容を表示し、失敗エントリがあれば -1 を返す |
| `funcman_get_func` | マクロ | `_funcman_get_func` を呼び出し、指定型にキャストして関数ポインタを返す |
| `_funcman_get_func` | 関数 | 関数ポインタを取得する内部実装。初回のみロードし、以降はキャッシュを返す |
| `funcman_is_declared_default` | 関数 | 明示的デフォルト判定。`resolved` が未解決の場合は解決を試みてから判定する |
| `funcman_object` | 構造体 | 1 関数分のキャッシュエントリ。呼び出し側が保持する |
| `NEW_FUNCMAN_OBJECT` | マクロ | `funcman_object` 静的変数の初期化子 |

## 呼び出しタイミング

| 関数 | 呼び出しタイミング |
|---|---|
| `funcman_init` | ライブラリのロード時 (constructor / `DllMain(DLL_PROCESS_ATTACH)`) |
| `funcman_dispose` | ライブラリのアンロード時 (destructor / `DllMain(DLL_PROCESS_DETACH)`) |
| `funcman_get_func` | オーバーライド対応関数の呼び出しごとに実行 (2 回目以降はキャッシュを返すため低コスト) |
| `funcman_is_declared_default` | デフォルト動作かオーバーライド動作かをログ等で区別したいときに任意で呼び出し |
| `funcman_info` | デバッグ・診断用途で任意に呼び出し |

## 他ライブラリでの利用方法

1. `libbase.h` をインクルードする
2. 管理対象の関数ごとに関数ポインタ型の `typedef` を定義する
3. `NEW_FUNCMAN_OBJECT` で `funcman_object` 静的変数を定義し、`funcman_object *const` ポインタを用意する
4. `funcman_object *const` ポインタを集めた配列と要素数を定義する
5. ライブラリのロード・アンロードフックから `funcman_init` / `funcman_dispose` を呼び出す
6. 各関数の実装内で `funcman_get_func` を使って関数ポインタを取得し、オーバーライド処理とデフォルト処理を分岐する

`libbase` における 2〜4 の実装例は `funcman_libbase.h` / `funcman_libbase.c` を、5 の実装例は `dllmain_libbase.c` を、6 の実装例は `sample_func.c` を参照してください。
