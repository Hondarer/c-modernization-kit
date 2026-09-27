# struct-meta

`struct-meta` は、C 構造体をメタデータで記述し、その内容を共通の方法で参照、JSON 変換、対話編集、表示するための実験的な基盤です。  
公開機能は `libstruct_meta` にまとめ、ヘッダーと実装を責務別に分けています。

記述子を得る経路は 2 系統あります。

- **事前組み込み型** — `struct-meta-gen` が生成した C ソースを実行体へ組み込みます。レイアウトはコンパイラが決めます。
- **事後解析型** — 実行時に C ヘッダーを構文解析して記述子を組み立てます。コンパイラを必要としません。

どちらも同じカタログ ハンドル (`struct_meta_catalog`) になるため、利用側のコードは経路を意識しません。  
両者のレイアウトがずれないよう、生成コードへ `_Static_assert` を出力して毎ビルド照合します。  
詳細は [アーキテクチャー](docs/architecture.md) を参照してください。

## 構成

| パス | 責務 |
|---|---|
| `prod/include/struct_meta/catalog/` | 記述子の集合の取得、検索、破棄 |
| `prod/include/struct_meta/parse/` | 構文解析の診断 |
| `prod/include/struct_meta/meta/` | 記述子、汎用属性、記述子検査 |
| `prod/include/struct_meta/access/` | フィールド、配列要素、文字列パスによるアクセス |
| `prod/include/struct_meta/json/` | cJSON および JSON ファイルとの相互変換 |
| `prod/include/struct_meta/patch/` | 対話形式の編集 |
| `prod/include/struct_meta/print/` | テキスト表示 |
| `prod/src/cmd/struct-meta-gen/` | 記述子を C ソースとして書き出す PoC |
| `prod/src/cmd/struct-meta-sample/` | 生成結果とライブラリを使う動作確認コマンド |

Table: struct-meta の構成パスと責務一覧

生成ファイルは `gen/` に置かれ、Git では管理しません。  
生成器は Doxygen コメントの `@struct_meta{key}` と `@struct_meta{key=value}` を解析し、構造体またはフィールドの汎用 key/value 属性として保持します。  
属性の記述方法と制約は [アーキテクチャー](docs/architecture.md#doxygen-属性の書式) を参照してください。  
文字列とバイト配列の判定、および `meta.kind` と `meta.format` は [文字列とバイト配列](docs/architecture.md#文字列とバイト配列) を参照してください。

## 実行

```sh
make
make test
./prod/cbin/struct-meta-sample
./prod/cbin/struct-meta-sample --help
```

操作対象は、起動後に `init` で定めます。

| コマンド | 経路 | 内容 |
|---|---|---|
| `init sample_types` | 事前組み込み型 | ビルド時に生成して実行体へ組み込んだカタログを使います。 |
| `init <header-path>` | 事後解析型 | 指定した C ヘッダーを実行時に構文解析します。コンパイラは不要です。 |
| `init` | - | 使い方と、選択できる組み込みカタログ名を表示します。 |

Table: struct-meta-sample のカタログ初期化コマンド一覧

`init` はカタログを用意してから構造体一覧を表示し、番号で選ばせます。  
選んだ記述子のサイズで領域を確保し、ゼロ初期化します。  
組み込みカタログの名前は生成カタログのステムと同じで、`sample_types.h` からは `sample_types` になります。  
表に無い引数はヘッダーのパスとして扱うため、実行中に何度でも対象を切り替えられます。  
構造体の選択は、番号または構造体名の完全一致で行います。  
構造体を選択すると、プロンプトは `struct-meta-sample(catalog/struct)>` の形式で表示されます。  
たとえば、`sample_types` の `person` を選ぶと `struct-meta-sample(sample_types/person)>` になります。

対象を定めた後は、`patch`、`patch <field-path>`、`dump`、`dumpjson`、`dumpbin`、`help`、`exit` に加えて、次の入出力コマンドを使用できます。

| コマンド | 形式 | 内容 |
|---|---|---|
| `loadjson <path>` / `savejson <path>` | JSON | 記述子に従って JSON と相互変換します。 |
| `catjson <path>` | JSON | ファイルの内容をテキストとして表示します。 |
| `dumpjson` | JSON | 現在の値をファイルへ保存せず、整形 JSON として標準出力へ表示します。`savejson` と `catjson` を続けて実行した場合と同じ内容です。 |
| `loadbin <path>` / `savebin <path>` | バイナリ | 記述子が表すバイト列をそのまま読み書きします。 |
| `catbin <path>` | バイナリ | ファイルの内容を 16 進ダンプで表示します。 |
| `dumpbin` | バイナリ | 現在の値をファイルへ保存せず、16 進ダンプとして標準出力へ表示します。`savebin` と `catbin` を続けて実行した場合と同じ内容です。 |

Table: struct-meta-sample の入出力コマンド一覧

`catjson` と `catbin` はファイルを読むだけなので、対象の選択を必要としません。  
それ以外の値を扱うコマンドは、`init` で対象を定めるまで受け付けません。

`savebin` はメモリ上の像をそのまま書き出すため、パディングも含みます。  
`init` が構造体全体をゼロ初期化するため、パディングの内容は決まります。  
`loadbin` は、大きさが記述子と一致しないファイルを受け付けません。  
足りなければ未初期化の領域が残り、余っていれば別のレイアウトのファイルである可能性が高いためです。

`patch` はメニューを順に辿り、フィールドを番号または現在位置のメンバー名の完全一致で選択します。  
配列要素の選択は番号で行います。  
`patch addresses[0].city` は指定したパスの値を直接編集します。  
メニューには現在位置と各候補の完全パスが表示されるため、そのパスを次回の `patch <field-path>` に利用できます。  
設計と依存方向は [アーキテクチャー](docs/architecture.md) を参照してください。
