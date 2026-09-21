# string-catalog-sample のアーキテクチャー

## この文書の位置付け

この文書は、cplat の文字列カタログ機能を **利用する側** の設計を示します。  
カタログ定義から生成物を作り、生成物を通じて cplat の API を呼び出すまでの構成です。

文字列カタログ自体の要件と外部から観測できる振る舞いは [文字列カタログ 機能仕様](../../c-platform/docs/functional-spec/string_catalog.md) を正本とします。  
書式の構文、引数種別、責務の境界、実装の構成は [string_catalog モジュール](../../c-platform/prod/libsrc/cplat/string_catalog/README.md) にあります。  
公開 API の契約は cplat の公開ヘッダーの Doxygen コメントを正本とし、この文書には複製しません。

## 利用側が用意するもの

cplat はカタログを保持しません。利用側が用意するのは次の 2 つだけです。

| 利用者が用意するもの | 内容 |
|---|---|
| 文字列キーの列挙 | 文字列を識別する定数です。名称と値は利用者が決定します。 |
| カタログの配列 | `key`、`id`、分類値、`brief`、`details`、`remarks`、引数定義、言語別の書式と備考です。 |

`brief` は必須の短い説明です。`details` と `remarks` は省略可能であり、設定した場合は詳細説明および利用上の補足説明として扱われます。

この 2 つをまとめたカタログ識別オブジェクトを、呼び出しごとに cplat へ渡します。  
文字列キーの型が列挙ではなく `int` であるため、利用者は任意の名前の列挙を定義し、その定数をそのまま渡せます。

この app では、カタログの保持形態として 2 種類を示します。  
コマンドが同梱する 3 つのカタログを `prod/src/cmd/string-catalog-command-sample/` へ、ライブラリが外部へ公開する 2 つのカタログを `prod/libsrc/samplecatalog/` へ配置しています。

コマンドが同梱するカタログは次のとおりです。

| ファイル | 内容 | 種別 |
|---|---|---|
| `sample_messages.jsonc` | カタログ定義の正本 | 手動作成 |
| `gen/sample_messages.h` | 文字列キーの列挙型、簡易関数の宣言、型付きラッパー | 自動生成 |
| `gen/sample_messages.c` | カタログ配列、インデックス テーブル、カタログ識別オブジェクト、簡易関数 | 自動生成 |
| `sample_metrics.jsonc` | メトリクス向けカタログ定義の正本 | 手動作成 |
| `gen/sample_metrics.h` | メトリクス向けの列挙型、簡易関数の宣言、型付きラッパー | 自動生成 |
| `gen/sample_metrics.c` | メトリクス向けのカタログ配列、インデックス テーブル、カタログ識別オブジェクト、簡易関数 | 自動生成 |
| `sample_trace.jsonc` | トレース種別のカタログ定義の正本 | 手動作成 |
| `gen/sample_trace.h` | トレース向けの列挙型、簡易関数と出力先の設定の宣言、型付きラッパーとマクロ | 自動生成 |
| `gen/sample_trace.c` | トレース向けのカタログ配列、インデックス テーブル、カタログ識別オブジェクト、簡易関数、出力処理 | 自動生成 |

ライブラリが公開するカタログは次のとおりです。生成物のヘッダーのみが公開ヘッダーの配置先へ出力されます。

| ファイル | 内容 | 種別 |
|---|---|---|
| `libsrc/samplecatalog/samplecatalog_messages.jsonc` | 公開するカタログ定義の正本 | 手動作成 |
| `libsrc/samplecatalog/catalog_settings.jsonc` | エクスポート マクロの接頭辞と定義元のヘッダー | 手動作成 |
| `include/samplecatalog/samplecatalog_messages.h` | 列挙型、簡易関数の宣言、型付きラッパー | 自動生成 |
| `libsrc/samplecatalog/gen/samplecatalog_messages.c` | カタログ配列、インデックス テーブル、カタログ識別オブジェクト、簡易関数 | 自動生成 |
| `libsrc/samplecatalog/samplecatalog_trace.jsonc` | 公開するトレース種別のカタログ定義の正本 | 手動作成 |
| `include/samplecatalog/samplecatalog_trace.h` | 列挙型、出力先の設定の宣言、型付きラッパーとマクロ | 自動生成 |
| `libsrc/samplecatalog/gen/samplecatalog_trace.c` | カタログ配列、インデックス テーブル、簡易関数、出力処理 | 自動生成 |

配列はコピーせず、ポインターだけを保持します。  
カタログを使用する間ずっと有効な領域を渡す必要があるため、静的記憶域期間を持つ配列を想定しています。

カタログ識別オブジェクトはすべてのメンバーを初期化子で設定できるため、`const` の静的記憶域期間を持つ値として定義できます。  
初期化関数を持たないため、初期化順序を考慮する必要がありません。

## 生成の仕組み

生成物は `gen/` へ配置し、Git では管理しません。`struct-meta` と同様の扱いです。  
生成は cplat の `bin/string_catalog_gen.py` が行い、ビルドが駆動します。  
定義ファイルまたは生成器が新しければ `make` が再生成するため、手動で実行する必要はありません。

生成はビルド規則ではなく、app 直下の `makepart.mk` が makefile のパース時に行います。  
コマンドとテストの双方が生成物を参照し、framework はソースの実在をパース時に検査するためです。  
ビルド規則にすると、テストのディレクトリを解釈する時点で生成物が存在せず失敗します。  
`app/sqlite` や `app/cjson` が展開ツールを呼び出す方式と同様です。

毎回のパースで実行されるため、生成器は `--if-newer` を受け取ります。  
定義ファイルと生成器のどちらも生成物より古ければ、何もせずに終了します。

利用側は `gen/` を付与してインクルードします。

```c
#include "gen/sample_messages.h"
#include "gen/sample_metrics.h"
```

手動で実行する場合、および内容を検証する場合のコマンド例です。

```bash
python3 ../c-platform/bin/string_catalog_gen.py prod/src/cmd/string-catalog-command-sample/sample_messages.jsonc --out-dir prod/src/cmd/string-catalog-command-sample/gen
python3 ../c-platform/bin/string_catalog_gen.py prod/src/cmd/string-catalog-command-sample/sample_messages.jsonc --out-dir prod/src/cmd/string-catalog-command-sample/gen --check
python3 ../c-platform/bin/string_catalog_gen.py prod/src/cmd/string-catalog-command-sample/sample_metrics.jsonc --out-dir prod/src/cmd/string-catalog-command-sample/gen
python3 ../c-platform/bin/string_catalog_gen.py prod/src/cmd/string-catalog-command-sample/sample_metrics.jsonc --out-dir prod/src/cmd/string-catalog-command-sample/gen --check
```

`--check` はファイルを出力せず、既存の生成物が定義と一致するかのみを検証します。  
生成器は出力を clang-format へ通すため、生成直後の内容がそのまま最終形です。

定義の形式は JSONC です。行コメント、ブロック コメント、末尾コンマを記述できます。  
長い文章は文字列配列で記述できます。生成器が空白 1 個で連結するため、定義ファイル上の行を短く保てます。  
JSON を選定した理由は cJSON と Python の双方で読み込み可能なためであり、コメントの前処理は生成器が文字列リテラルを認識しながら行います。

定義ファイルに記述するのは、著者、日付、版、文字列の一覧のみです。  
`texts` と `notes` に指定可能な言語は、生成器の定数 `LANGUAGES` で保持しています。  
言語は cplat が定める仕様であり、カタログが増減できる項目ではないためです。  
生成器は、記述された言語がその一覧に含まれているかのみを検査します。

生成器が検査するのは、文字列キー `key` の重複、メタデータの型、引数種別が対応表にあること、位置指定が引数個数に収まること、ニュートラル言語のリソースが漏れなく定義されていること、宣言していない言語が現れないこと、型付きラッパー名が簡易関数名と衝突しないことです。  
`cplat_string_catalog_verify()` が実行時に検査する内容を、生成時へ前倒しします。

`key` は処理から項目を参照する文字列キーで、列挙定数の名前になります。列挙値は、定義の並び順から 1 始まりで生成器が決めます。  
プログラムは列挙定数の名前で参照するため、定義を並べ替えても再生成と再ビルドで追従します。列挙値そのものは並び順で変化します。

`id` は処理上は意味を持たない補足の文字列です。省略可能であり、重複検査も行いません。生成器は、省略された `id` を NULL として出力します。

言語別の書式と備考は、言語をキーとした指示付き初期化子で定義します。

```c
{[CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL] = "Failed to open file {0}. Error code={1}",
 [CPLAT_STRING_CATALOG_LANGUAGE_JAPANESE] = "ファイル {0} を開けませんでした。エラー コード={1}"},
```

位置ではなく言語で対応が決定されるため、言語の追加や並べ替えの影響を受けません。  
記述しなかった言語の要素は暗黙にヌル ポインターとなり、ニュートラル言語へのフォールバック処理がそのまま適用されます。  
リソースを持たない言語は、行ごと省略できます。

現在のカタログでは、ニュートラル言語の書式を暫定的に英語と同じ表現とし、英語の要素を省略しています。  
英語をニュートラル言語と分ける必要が生じた時点で、英語の要素を追加します。

## 名前空間の分離

利用者側の識別子は `sample_` で始め、cplat 側の `cplat_string_catalog_` と分けます。  
サンプルであっても両者が同じ接頭辞を共有していると、どこまでが cplat の提供物かを名前から判断できません。

| 側 | 接頭辞 | 例 |
|---|---|---|
| cplat | `cplat_string_catalog` | `cplat_string_catalog_format()`、`CPLAT_STRING_CATALOG_LANGUAGE_JAPANESE` |
| 利用者 | `sample_messages` | `sample_messages_catalog()`、`SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED` |
| 利用者 | `sample_metrics` | `sample_metrics_catalog()`、`SAMPLE_METRICS_KEY_THROUGHPUT_REPORT` |

型付きラッパーの関数名も利用者側の名前空間に収めます。  
`cplat_` を前置すると、利用者が定義した関数が cplat のリンカー名前空間を名乗ることになるためです。

モジュール接頭辞は、定義ファイルの名前そのものです。  
`sample_messages.jsonc` の場合は `sample_messages` となり、生成物のファイル名、カタログ指定を省略する簡易関数の名前、文字列キーの列挙名 `sample_messages_key` がここから決定されます。  
C の識別子の一部になるため、英小文字で始まる snake_case のみを受け付けます。

名前空間を変更する場合は、定義ファイルの名前を変更します。  
同一名称を定義内にも記述すると、ファイル名との間に不整合が生じる余地を残すためです。

生成される `.c` 内の補助マクロにも、モジュール接頭辞を大文字化した名称を付与します。  
例えば `SAMPLE_MESSAGES_ENTRY_COUNT` と `SAMPLE_METRICS_ENTRY_COUNT` になるため、複数の生成物を同一ビルドへインクルードしても名前が衝突しません。

## 生成物を汎用に保つ

生成物が依存してよいのは、cplat の文字列カタログの公開ヘッダーと標準ヘッダーのみです。  
特定の app の型や列挙へ依存させません。生成物をどの app からもそのまま再利用できるようにするためです。

そのため分類値は直接の値として保持します。  
定義ファイルにも整数で記述し、意味はコメントで示します。生成器は整数以外を受け付けません。

```c
/* 生成される表。分類値は 1 で、CPLAT_TRACE_LEVEL_ERROR を意味する */
{SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED,
 1,
 2,
```

生成物の Doxygen に現れるディレクトリ名は、定義ファイルの配置場所から導出します。  
絶対パスの中で最も近い `prod` または `test` を起点とし、そこから下を app 直下からの相対パスとします。  
生成器内に配置先を保持せず、定義ファイルにも記述しません。同一内容を 2 か所で管理すると不整合が生じるためです。

## 分類値とトレース レベル

分類値は、cplat 自体は意味を持たない `int` です。  
重大度、用途、出力先など、値の意味と有効な範囲は利用者が決定します。cplat は値を検査せず、保持して返すのみです。

0 は分類なしを表します。  
利用者が 0 を意味のある分類値として登録することもできますが、その場合はカタログに存在しない文字列キーと区別できません。  
区別が必要な場合は、事前に `cplat_string_catalog_get_entry()` で項目の有無を確認します。

この app では、分類値をトレース レベルとして使用します。  
値は cplat の `cplat_trace_level` と同一です。生成物は `cplat_trace_level` に依存せず、値を解釈する側のみが `<cplat/trace/tracer.h>` をインクルードします。

範囲外の値は、コマンドの `trace_level_of()` が `CPLAT_TRACE_LEVEL_NONE` へフォールバックします。  
意味付けを行う階層で制限するため、cplat とそのテストは分類値の範囲を確認しません。

文字列リソース種別では、この app はトレースの出力機構を持ちません。  
レベルは、利用側が出力先やフィルター条件を決定するための情報として保持します。

トレース種別のカタログでは、分類値の記述形式が変わります。  
定義には整数ではなく `level` へトレース レベルの定数名を記述し、生成器が対応する整数へ変換して分類値に格納します。  
定数名で記述するのは、トレース種別の生成物が分類値をトレース レベルとして解釈し、出力先へ渡すためです。

## トレース種別のカタログ

`sample_trace.jsonc` は、`kind` に `trace` を指定したトレース種別の定義です。  
生成物は、組み立てた文字列をトレースへ出力する入口を提供します。

文字列リソース種別との違いは、次の 3 点です。

- 先頭の引数が、文字列の格納先ではなくトレーサーのハンドルになります
- 呼び出し位置と実行コンテキストが、引数配列の 40 番から 6 個の引数として渡されます
- 分類値を `level` へトレース レベルの定数名で記述し、生成器が整数へ変換します

呼び出し側から見た引数の並びは、両方の種別で一致します。

```c
/* 文字列リソース種別 */
sample_messages_key_file_open_failed(text, sizeof(text), "config.json", 2);

/* トレース種別。出力先は事前に sample_trace_set_tracer() で設定する */
sample_trace_key_file_open_failed("config.json", 2);
```

文脈引数の位置指定と内容の対応は次のとおりです。  
前の 4 つは呼び出し位置で確定するため関数形式マクロが渡し、残る 2 つは実行時の値のため型付きラッパーの内部で取得します。

| 位置指定 | 引数名 | 引数種別 | 値 |
|---|---|---|---|
| `{40}` | `source_file_path` | `STRING` | 呼び出し位置のソース ファイル。コンパイラへ渡した表記のまま |
| `{41}` | `source_file_name` | `STRING` | 呼び出し位置のソース ファイル名。ディレクトリを除いた表記 |
| `{42}` | `source_line` | `INT32` | 呼び出し位置の行番号 |
| `{43}` | `function_name` | `STRING` | 呼び出し位置の関数名 |
| `{44}` | `process_id` | `UINT32` | 出力を要求したプロセスの ID |
| `{45}` | `thread_id` | `UINT32` | 出力を要求したスレッドの ID |

書式が文脈引数の位置指定を持たない場合、組み立てた文字列に文脈値は現れません。  
出力へ含める必要が生じた場合は、定義の書式へ位置指定を追加します。実装の変更は不要です。

`sample_trace.jsonc` の `SAMPLE_TRACE_KEY_STATE_DUMP` が、書式から文脈引数を参照する例です。

```text
キューの長さは 7 です。[string-catalog-command-sample.c:273 write_traces] パス=string-catalog-command-sample.c プロセス=1917265 スレッド=1917265
```

記述した引数の個数から 39 番までは、値を受け取らないインデックスです。  
書式から参照すると定義エラーとなります。書式が参照可能なのは、利用者の引数と 40 番から 45 番までです。

同じ対応表は、生成ヘッダーのファイル コメントにも出力されます。

### 出力先のトレーサー

出力先のトレーサーは、カタログが保持します。  
`sample_trace_set_tracer()` で設定し、`sample_trace_get_tracer()` で取得します。呼び出しごとにトレーサーを渡すオーバーヘッドを解消するためです。

未設定のまま出力を要求した場合は、文字列の組み立ても出力も行わず `CPLAT_ERR_INVALID_ARGUMENT` を返します。  
`cplat_tracer_write_at()` は NULL のハンドルを受け取ると何もせず成功を返すため、そのまま渡すと設定漏れが成功扱いで見落とされます。

設定はスレッド セーフではありません。出力を開始する前に設定します。

設定を解除する呼び出しと、トレーサーの破棄は、原則として不要です。  
`cplat_tracer_create()` が終了コールバックを登録し、プロセスの終了時に cplat がすべてのトレーサーを破棄するためです。  
破棄したあとに出力を要求する経路が存在しないため、カタログの設定を解除する必要もありません。

トレーサーを明示的に破棄し、そのあとに出力を要求しうる場合に限り、破棄の前に NULL を設定します。  
サンプル コマンドでは、生成直後の設定や開始に失敗した経路だけで明示的に破棄しています。ハンドルを NULL にして以降の誤用を防ぐためです。

## カタログ指定を省略する簡易関数

カタログを引数に指定する形式は、複数のカタログを扱う場合に必要ですが、1 つのみを使用する呼び出し側には冗長となります。  
そこで、カタログの指定を省略して呼び出すための簡易関数を利用側の生成物に配置します。

```c
/* sample_messages.c が持つ */
static const cplat_string_catalog s_catalog = {
    s_entries, s_key_index, SAMPLE_MESSAGES_ENTRY_COUNT, SAMPLE_MESSAGES_KEY_INDEX_COUNT};

int sample_messages_format(char *dest, size_t dest_size, int string_key, ...)
{
    /* s_catalog を補って cplat_string_catalog_format() を呼び出す */
}
```

この簡易関数を cplat ではなく生成物へ配置するのは、既定カタログという状態を cplat に持たせないためです。  
cplat は状態を持たないまま、呼び出し側は 1 つのカタログを簡潔に記述できます。  
可変長引数はいったん `va_list` にして `cplat_string_catalog_vformat()` へ中継します。

簡易関数の名前はモジュール接頭辞のままです。  
`sample_messages_get_category()` のように cplat 側の動詞を反映しないのは、この関数群が特定のカタログに固有の利用側 API であるためです。

生成物には、文字列キーから項目全体を取得する `sample_messages_entry()` も含まれます。  
返された項目から、説明文、引数の名前と説明、分類値、書式、備考をまとめて参照できます。

## 文字列キーごとの型付きラッパー

可変長引数を取る関数では、引数の個数や型をコンパイラが検査できません。  
書式と引数スキーマがカタログの中にあり、書式文字列が呼び出しの実引数ではないためです。  
`printf` に付けられる `format` 属性も、付与対象の引数が存在しないため使用できません。

そこで、文字列キーごとに引数の型を固定したラッパーを生成物へ並べます。  
通常のプロトタイプ検査が働き、Doxygen コメントによって利用者は各引数の意味をインテリセンス上で参照できます。

```c
static inline int sample_messages_key_file_open_failed(char *dest, size_t dest_size,
                                                       const char *file_path, int error_code);
```

トレース種別では、同じ名前を関数形式マクロとして出力します。  
呼び出し位置は展開位置で確定する必要があり、`__FILE__` と `__LINE__` と `__func__` をマクロでしか取得できないためです。  
マクロは引数を `static inline` の `_with_source` 関数へ渡すのみとし、引数の個数と型の検査はその関数のプロトタイプ宣言によって行われます。

```c
static inline int sample_trace_key_file_open_failed_with_source(cplat_tracer *tracer, const char *file_path,
                                                                const int error_code, ...);

#define sample_trace_key_file_open_failed(tracer, file_path, error_code) \
    sample_trace_key_file_open_failed_with_source((tracer), (file_path), (error_code), __FILE__, ...)
```

関数名は文字列キーから機械的に導出します。文字列キーの定数名を小文字化しただけの名前です。  
導出は生成器の `wrapper_name()` が行い、`test_string_catalog_gen.py` が規則を固定しています。

```text
SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED
  → 小文字化 → sample_messages_key_file_open_failed
```

文字列キーの定数名にモジュール接頭辞を含めるため、ラッパー名は自然に利用者側の名前空間へ収まります。  
同一生成物が出力する簡易関数 (`sample_messages_format()` など) と名前が衝突する定数名は、生成器が定義エラーとして除外します。

語の除去や入れ替えは行わないため、規則に例外がありません。  
区切りのアンダースコアは 1 個です。2 個続けると、C++ が処理系用に予約する識別子になります。

引数の型は、引数種別から cplat の公開ヘッダー `cplat/string_catalog/argument.h` の対応表で決まります。  
引数名と説明は生成元の定義が持つ情報であり、生成されたカタログの引数定義へ保持されます。

`static inline` とするのは、実体を持つ翻訳単位を増やさず、文字列キーの数だけ公開シンボルが増加することを回避するためです。  
関数形式マクロは採用しません。展開後が可変長引数の呼び出しのままで、型検査が機能しないためです。  
`static_assert` をマクロへ組み込む設計も採用できません。`static_assert` は宣言であって式ではなく、値を返す形式にするには GNU 文式が必要で、[コーディング規範](../../general/docs/coding-guideline.md) が MSVC 非対応を理由に禁止しています。

検査の強さは診断の種類で異なります。  
引数の個数違いはコンパイル エラーです。  
ポインターと整数の取り違えは `-Wint-conversion` の警告、幅や符号の取り違えは `-Wconversion` と `-Wsign-conversion` の警告として `.warn` に現れます。  
可変長引数のままでは、いずれの診断も出力されません。

## インデックス表による探索

文字列キーからカタログを検索する処理は、既定では線形探索です。  
利用者は、文字列キーをインデックスとしてカタログの配列インデックスを検索するインデックス表を付与できます。

```text
cplat_string_catalog = {entries, key_index, entry_count, key_index_count}

  key_index[string_key] -> entries のインデックス (未登録は負の値)
```

インデックス テーブルを渡すことで、線形探索からインデックス参照へ置き換えます。  
文字列キーが小さい非負整数である場合に使用できます。  
文字列キーがインデックス テーブルの範囲を超える場合は、線形探索へフォールバックします。

生成器はインデックス表も出力するため、利用側が手動で整合性を保つ必要はありません。  
`cplat_string_catalog_verify()` は、各文字列がインデックス表を通して自身を参照できることも確認します。

## カタログの点検

`cplat_string_catalog_verify()` は、カタログ全体の書式が引数スキーマと矛盾しないことを確認します。  
カタログは静的に確定するため、点検は起動時に一度実行すれば十分です。  
複数のカタログを使う場合は、カタログごとに点検します。

点検では、文字列キーの重複、項目メタデータと引数定義の欠落も確認します。`id` は処理では意味を持たないため、未設定と重複を確認しません。

サンプル コマンドは、起動直後に `sample_messages` と `sample_metrics` の両方を点検します。  
不正が検出された場合はエラー終了します。

## 依存関係

この app は `app/c-platform` に依存します。`appdeps.mk` の `APP_DEPS` に `c-platform` を指定し、コマンドとテストは `LIBS += cplat` でリンクします。

文字列カタログの API に加えて、コンソールの UTF-8 初期化 (`cplat_console_init` / `cplat_console_dispose`) と、分類値の意味付けに使うトレース レベルの列挙 (`cplat_trace_level`) を利用します。

結果コードは cplat 共通の `CPLAT_OK` と `CPLAT_ERR_*` です。

## テストの構成

文字列カタログ自体の単体テストは `app/c-platform` にあります。この app に残るのは、利用側の資産を対象とするテストだけです。

| テスト | 置き場所 | 対象 |
|---|---|---|
| `sampleMessagesTest` | `test/src/cmd/` | コマンドが用意するカタログとインデックス表 |
| `exportTest` | `test/src/libsamplecatalog/` | ライブラリが実際に公開するシンボルの一覧とシグネチャ |
| `contextTest` | `test/src/libsamplecatalog/` | app が定義するコンテキスト引数として渡す連番の巡回 |
| `catalogIntegrationTest` | `test/src/integration/` | コマンドのカタログと cplat の組み立てを結合した確認 |

`sampleMessagesTest` を `test/src/cmd/` に置くのは、対象がライブラリではなく、コマンドが用意するソースであるためです。  
`catalogIntegrationTest` は個々のソースのカバレッジを目的としないため `TEST_SRCS` を宣言せず、生成物を `ADD_SRCS` で取り込み、cplat を実体でリンクします。

## カタログを外部へ公開する

`prod/libsrc/samplecatalog/` のカタログは、カタログ定義の `export` によって、ライブラリの外部から使用できます。  
生成器は公開範囲に応じて、関数の宣言へ app のエクスポート マクロと呼び出し規約マクロを出力します。

マクロの接頭辞と定義元のヘッダーは app 内で共通のため、カタログ定義が `settings` で指す `catalog_settings.jsonc` に置きます。  
接頭辞からマクロ名を導く規約は、`cplat/base/dll_exports.h` の `CPLAT_DLL_EXPORT(prefix)` と同じです。

公開範囲は `api` と `full` の 2 つです。  
この app は `api` を使用し、戻り値が cplat の構造体を指す関数と `_verify` を公開しません。  
公開すると、利用側が `cplat_string_catalog` のレイアウトへ依存することになるためです。  
カタログの点検は提供元の責務とし、`samplecatalog_initialize()` の中で完了させます。

型付きラッパーは `static inline` であり、利用側の翻訳単位で展開されます。  
呼び先は生成物の簡易関数 `@MODULE@_format` であるため、文字列リソース種別では利用側が cplat の関数を呼びません。  
トレース種別では、関数形式マクロが `cplat_path_basename(__FILE__)` を展開位置で評価するため、利用側も cplat をリンクします。

ライブラリは共有ライブラリとして配置します。  
カタログが保持するトレースの出力先と、cplat が持つプロセス全体の言語設定を、1 つの実体に保つためです。

外部との契約になるのは、文字列キーの値と名前、引数の個数と型、分類値、および `id` です。  
書式と備考の文言は契約に含まれないため、ライブラリの再ビルドだけで変更できます。  

## app が定義するコンテキスト引数

トレース種別のカタログ項目は、呼び出し位置と実行コンテキストを文脈引数として自動的に受け取ります。  
`{40}` から `{45}` までの 6 個は cplat が定義し、`{46}` から 4 個までを app が定義できます。

この app では、`catalog_settings.jsonc` の `context` 節で 1 個を追加しています。

| 位置指定 | 引数名 | 引数種別 | 取得式 |
|---|---|---|---|
| `{46}` | `sequence_number` | `INT32` | `samplecatalog_next_sequence_number()` |

位置指定は、設定ファイルの記述順に `{46}` から詰めて割り当てられます。  
ワークスペースは全体を一括でビルドするため、番号を定義へ固定する仕組みは設けていません。

引数配列は、app が定義するコンテキスト引数の個数にかかわらず `{46}` から `{49}` までを常に確保します。  
記述しなかった番号は、値を受け取らないインデックスとして残ります。  
個数が増減しても配列構造が変化しないようにするためです。

取得式は書式から参照した場合のみ意味を持ちますが、評価自体は出力要求ごとに行われます。  
そのため、取得処理は軽量、失敗しない、スレッド セーフである必要があります。

公開するカタログでは、取得関数もライブラリの外部へ公開します。  
取得式は生成ヘッダーの `static inline` 関数内で展開され、利用側の翻訳単位から呼び出されるためです。  
実行結果では、ライブラリの内部からの出力と利用側からの出力に、通し番号として連番が現れます。

app が定義するコンテキスト引数の増減は、公開したカタログでは非互換の変更です。  
可変長引数の並びが利用側のバイナリへ直接組み込まれるため、ライブラリのみを差し替えると値に不整合が生じます。  
番号空間を確保してもこの制約は変わらず、増減した場合は利用側の再コンパイルが必要です。

