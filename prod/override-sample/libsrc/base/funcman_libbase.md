# funcman_libbase の概要

## 役割

`funcman_libbase.h` / `funcman_libbase.c` は、`libbase` ライブラリが管理するオーバーライド対応関数の **funcman_object 実体と、それに紐付く型・変数の宣言** を提供します。

`funcman` の汎用機能 (`funcman_init`, `_funcman_get_func`, `funcman_dispose` など) は `libbase.h` で定義されています。`funcman_libbase.h` / `funcman_libbase.c` はそれを `libbase` 固有の関数群に接続する「接着剤」の役割を担います。

## ファイルの責務

| ファイル | 責務 |
|---|---|
| `funcman_libbase.h` | 関数ポインタ型の `typedef`、`funcman_object` ポインタ・配列・設定ファイルパスの `extern` 宣言 |
| `funcman_libbase.c` | `funcman_object` 実体の定義、配列・要素数・設定ファイルパスの実体定義、`funcman_info_libbase()` の実装 |

`funcman_object` の実体は `funcman_libbase.c` 内で `static` 変数として定義します。外部からのアクセスは `funcman_object *const` ポインタ経由に限定することで、直接書き換えを防いでいます。

## 利用側との関係

```text
sample_func.c  (呼び出し元)
    |  funcman_get_func(pfo_sample_func, sample_func_t)
    +---> funcman_libbase.h の extern 宣言 (pfo_sample_func)
              |
              funcman_libbase.c の実体 (sfo_sample_func)

dllmain_libbase.c  (初期化・解放)
    |  funcman_init(fobj_array_libbase, fobj_length_libbase, funcman_configpath)
    |  funcman_dispose(fobj_array_libbase, fobj_length_libbase)
    +---> funcman_libbase.h の extern 宣言 (fobj_array_libbase, fobj_length_libbase, funcman_configpath)
              |
              funcman_libbase.c の実体
```

---

## オーバーライド対応関数を追加する場合の作業

`sample_func` に相当する新しい関数 `new_func` を追加する場合の手順です。

### 1. `funcman_libbase.h` への追加

以下の 2 点を追記します。

- `new_func` に対応する関数ポインタ型の `typedef`
- その funcman_object へのポインタの `extern` 宣言

### 2. `funcman_libbase.c` への追加

以下の 2 点を追記します。

- `NEW_FUNCMAN_OBJECT` マクロで funcman_object の `static` 実体を定義し、対応する `const` ポインタを定義
- `fobj_array_libbase` 配列に新しい実体のアドレスを追加

`fobj_length_libbase` は配列サイズから自動計算されるため変更不要です。

### 3. 呼び出し元となる .c ファイルの作成・修正

`new_func` の実装ファイルで、`funcman_get_func` を使って関数ポインタを取得し、オーバーライド処理またはデフォルト処理に分岐させます。`sample_func.c` を参考にしてください。

### 4. オーバーライド側ライブラリへの追加

`liboverride` 側 (または別のオーバーライドライブラリ) に、`new_func` と同じシグネチャを持つ実装関数を追加します。

### 変更ファイルのまとめ

| ファイル | 作業 |
|---|---|
| `funcman_libbase.h` | 型定義・extern 宣言を追加 |
| `funcman_libbase.c` | 実体定義・配列エントリを追加 |
| `libsrc/base/new_func.c` (新規) | funcman を使ったオーバーライド対応の実装 |
| `libsrc/override/new_override_func.c` (新規) | オーバーライド実装 |
| `include/libbase.h` | `new_func` のエクスポート宣言を追加 |
| `include/libbase_ext.h` | オーバーライド関数のエクスポート宣言を追加 |
