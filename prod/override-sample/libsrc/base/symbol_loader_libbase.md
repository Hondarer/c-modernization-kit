# symbol_loader_libbase の概要

## 役割

`symbol_loader_libbase.h` / `symbol_loader_libbase.c` は、`libbase` ライブラリが管理するオーバーライド対応関数の **symbol_loader_entry_t 実体と、それに紐付く型・変数の宣言** を提供します。

`symbol_loader` の汎用機能 (`symbol_loader_init`, `symbol_loader_resolve`, `symbol_loader_dispose` など) は `libbase.h` で定義されています。`symbol_loader_libbase.h` / `symbol_loader_libbase.c` はそれを `libbase` 固有の関数群に接続する「接着剤」の役割を担います。

## ファイルの責務

| ファイル | 責務 |
|---|---|
| `symbol_loader_libbase.h` | 関数ポインタ型の `typedef`、`symbol_loader_entry_t` ポインタ・配列・設定ファイルパスの `extern` 宣言 |
| `symbol_loader_libbase.c` | `symbol_loader_entry_t` 実体の定義、配列・要素数・設定ファイルパスの実体定義、`symbol_loader_info_libbase()` の実装 |

`symbol_loader_entry_t` の実体は `symbol_loader_libbase.c` 内で `static` 変数として定義します。外部からのアクセスは `symbol_loader_entry_t *const` ポインタ経由に限定することで、直接書き換えを防いでいます。

## 利用側との関係

```text
sample_func.c  (呼び出し元)
    |  symbol_loader_resolve_as(pfo_sample_func, sample_func_t)
    +---> symbol_loader_libbase.h の extern 宣言 (pfo_sample_func)
              |
              symbol_loader_libbase.c の実体 (sfo_sample_func)

dllmain_libbase.c  (初期化・解放)
    |  symbol_loader_init(fobj_array_libbase, fobj_length_libbase, symbol_loader_configpath)
    |  symbol_loader_dispose(fobj_array_libbase, fobj_length_libbase)
    +---> symbol_loader_libbase.h の extern 宣言 (fobj_array_libbase, fobj_length_libbase, symbol_loader_configpath)
              |
              symbol_loader_libbase.c の実体
```

---

## オーバーライド対応関数を追加する場合の作業

`sample_func` に相当する新しい関数 `new_func` を追加する場合の手順です。

### 1. `symbol_loader_libbase.h` への追加

以下の 2 点を追記します。

- `new_func` に対応する関数ポインタ型の `typedef`
- その symbol_loader_entry_t へのポインタの `extern` 宣言

### 2. `symbol_loader_libbase.c` への追加

以下の 2 点を追記します。

- `SYMBOL_LOADER_ENTRY_INIT` マクロで symbol_loader_entry_t の `static` 実体を定義し、対応する `const` ポインタを定義
- `fobj_array_libbase` 配列に新しい実体のアドレスを追加

`fobj_length_libbase` は配列サイズから自動計算されるため変更不要です。

### 3. 呼び出し元となる .c ファイルの作成・修正

`new_func` の実装ファイルで、`symbol_loader_resolve_as` を使って関数ポインタを取得し、オーバーライド処理またはデフォルト処理に分岐させます。`sample_func.c` を参考にしてください。

### 4. オーバーライド側ライブラリへの追加

`liboverride` 側 (または別のオーバーライドライブラリ) に、`new_func` と同じシグネチャを持つ実装関数を追加します。

> **警告: 循環参照によるスタックオーバーフロー**
>
> オーバーライド実装の中で、オーバーライド元の関数 (`new_func`) を直接または間接的に呼び出すと無限再帰となりスタックオーバーフローが発生します。
>
> ```text
> new_func (libbase)
>   └─ symbol_loader_resolve_as でオーバーライド関数を取得
>        └─ new_override_func (liboverride) を呼び出す
>              └─ new_func (libbase) を呼び出す  ← 無限再帰
> ```
>
> オーバーライド実装はオーバーライド元の関数を呼ばないよう設計してください。同様に、複数の関数をオーバーライドする場合、A のオーバーライドが B を呼び、B のオーバーライドが A を呼ぶような間接的な循環にも注意が必要です。関数を追加する際は、呼び出し経路全体を把握したうえで設計してください。

### 変更ファイルのまとめ

| ファイル | 作業 |
|---|---|
| `symbol_loader_libbase.h` | 型定義・extern 宣言を追加 |
| `symbol_loader_libbase.c` | 実体定義・配列エントリを追加 |
| `libsrc/base/new_func.c` (新規) | symbol_loader を使ったオーバーライド対応の実装 |
| `libsrc/override/new_override_func.c` (新規) | オーバーライド実装 |
| `include/libbase.h` | `new_func` のエクスポート宣言を追加 |
| `include/libbase_ext.h` | オーバーライド関数のエクスポート宣言を追加 |
