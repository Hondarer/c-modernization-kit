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
| 文字列 ID の列挙 | 文字列を識別する定数です。名前と値は利用者が決めます。 |
| カタログの配列 | 引数スキーマ、分類値、メタデータ、言語別の書式と備考です。 |

この 2 つをまとめたカタログ識別オブジェクトを、呼び出しごとに cplat へ渡します。  
文字列 ID の型が列挙ではなく `int` であるため、利用者は任意の名前の列挙を定義し、その定数をそのまま渡せます。

この app では、両方をコマンド側の `prod/src/cmd/string-catalog-sample/` へ置いています。

| ファイル | 内容 | 種別 |
|---|---|---|
| `sample_messages.jsonc` | カタログ定義の正本 | 手動作成 |
| `gen/sample_messages.h` | 文字列 ID の列挙型、簡易関数の宣言、型付きラッパー | 自動生成 |
| `gen/sample_messages.c` | カタログ配列、添字テーブル、カタログ識別オブジェクト、簡易関数 | 自動生成 |

配列はコピーせず、ポインターだけを保持します。  
カタログを使用する間ずっと有効な領域を渡す必要があるため、静的記憶域期間を持つ配列を想定しています。

カタログ識別オブジェクトはすべてのメンバーを初期化子で設定できるため、`const` の静的記憶域期間を持つ値として定義できます。  
初期化関数を持たないため、初期化順序を考慮する必要がありません。

## 生成の仕組み

生成物は `gen/` へ置き、Git では管理しません。`struct-meta` と同じ扱いです。  
生成は cplat の `bin/string_catalog_gen.py` が行い、ビルドが駆動します。  
定義ファイルか生成器が新しければ `make` が再生成するため、手動で実行する必要はありません。

生成はビルド規則ではなく、app 直下の `makepart.mk` が makefile のパース時に行います。  
コマンドとテストの双方が生成物を参照し、framework はソースの実在をパース時に検査するためです。  
ビルド規則にすると、テストのディレクトリを解釈する時点で生成物が存在せず失敗します。  
`app/sqlite` や `app/cjson` が展開ツールを呼ぶ方式と同じです。

毎回のパースで走るため、生成器は `--if-newer` を受け取ります。  
定義ファイルと生成器のどちらも生成物より古ければ、何もせずに終了します。

利用側は `gen/` を付けて取り込みます。

```c
#include "gen/sample_messages.h"
```

手動で実行する場合、および内容を検証する場合のコマンド例です。

```bash
python3 ../c-platform/bin/string_catalog_gen.py prod/src/cmd/string-catalog-sample/sample_messages.jsonc --out-dir prod/src/cmd/string-catalog-sample/gen
python3 ../c-platform/bin/string_catalog_gen.py prod/src/cmd/string-catalog-sample/sample_messages.jsonc --out-dir prod/src/cmd/string-catalog-sample/gen --check
```

`--check` は書き出さず、既存の生成物が定義と一致するかだけを確かめます。  
生成器は出力を clang-format へ通すため、生成直後の内容がそのまま最終形です。

定義の形式は JSONC です。行コメント、ブロック コメント、末尾コンマを書けます。  
長い文章は文字列の配列で書けます。生成器が空白 1 個で連結するため、定義ファイル上の行を短く保てます。  
JSON を選んだのは cJSON と Python の双方で読めるためで、コメントの前処理は生成器が文字列リテラルを認識しながら行います。

定義ファイルに書くのは、著者、日付、版、文字列の一覧だけです。  
`texts` と `notes` に書ける言語は、生成器の定数 `LANGUAGES` が持ちます。  
言語は cplat が定める仕様であり、カタログが増減できる項目ではないためです。  
生成器は、書かれた言語がその一覧にあるかだけを検査します。

生成器が検査するのは、文字列 ID と固定文字列の重複、引数種別が対応表にあること、位置指定が引数個数に収まること、ニュートラル言語のリソースが欠けていないこと、宣言していない言語が現れないこと、型付きラッパー名が簡易関数名と衝突しないことです。  
`cplat_string_catalog_verify()` が実行時に見ている内容を、生成時へ前倒しします。

文字列 ID の値は、定義の並び順から 1 始まりで生成器が決めます。  
安定させる必要がある識別子は固定文字列 (`id_text`) であり、これは定義ファイルへ明記します。  
ログに出るのは固定文字列なので、定義を並べ替えても外部から見える識別子は動きません。

言語別の書式と備考は、言語をキーとした指示付き初期化子で記載します。

```c
{[CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL] = "Failed to open file {0}. Error code={1}",
 [CPLAT_STRING_CATALOG_LANGUAGE_JAPANESE] = "ファイル {0} を開けませんでした。エラー コード={1}"},
```

位置ではなく言語で対応が決まるため、言語の追加や並べ替えの影響を受けません。  
記載しなかった言語の要素は暗黙にヌル ポインターとなり、ニュートラル言語への読み替えがそのまま働きます。  
リソースを持たない言語は、行ごと省略できます。

現在のカタログでは、ニュートラル言語の書式を暫定的に英語と同じ表現とし、英語の要素を省略しています。  
英語をニュートラル言語と分ける必要が生じた時点で、英語の要素を追加します。

## 名前空間の分離

利用者側の識別子は `sample_` で始め、cplat 側の `cplat_string_catalog_` と分けます。  
サンプルであっても両者が同じ接頭辞を共有していると、どこまでが cplat の提供物かを名前から判断できません。

| 側 | 接頭辞 | 例 |
|---|---|---|
| cplat | `cplat_string_catalog` | `cplat_string_catalog_format()`、`CPLAT_STRING_CATALOG_LANGUAGE_JAPANESE` |
| 利用者 | `sample_messages` | `sample_messages_catalog()`、`SAMPLE_MESSAGES_ID_FILE_OPEN_FAILED` |

型付きラッパーの関数名も利用者側の名前空間に収めます。  
`cplat_` を前置すると、利用者が定義した関数が cplat のリンカー名前空間を名乗ることになるためです。

モジュール接頭辞は、定義ファイルの名前そのものです。  
`sample_messages.jsonc` なら `sample_messages` になり、生成物のファイル名、カタログ指定を省略する簡易関数の名前、文字列 ID の列挙名 `sample_messages_id` がここから決まります。  
C の識別子の一部になるため、英小文字で始まる snake_case だけを認めます。

名前空間を移すには、定義ファイルの名前を変えます。  
同じ名前を定義の中にも書くと、ファイル名と食い違う余地が残るためです。

## 生成物を汎用に保つ

生成物が依存してよいのは、cplat の文字列カタログの公開ヘッダーと標準ヘッダーだけです。  
特定の app の型や列挙へ依存させません。生成物をどの app からもそのまま持ち運べるようにするためです。

そのため分類値は生値で保持します。  
定義ファイルにも整数で書き、意味はコメントで示します。生成器は整数以外を受け付けません。

```c
/* 生成される表。分類値は 1 で、CPLAT_TRACE_LEVEL_ERROR を意味する */
{SAMPLE_MESSAGES_ID_FILE_OPEN_FAILED,
 1,
 2,
```

生成物の Doxygen に現れるディレクトリ名は、定義ファイルの置き場所から導出します。  
絶対パスの中で最も近い `prod` または `test` を起点とし、そこから下を app 直下からの相対パスとします。  
生成器の中に配置先を持たず、定義ファイルにも書きません。同じ内容を 2 か所で管理すると食い違うためです。

## 分類値とトレース レベル

分類値は、cplat が意味を持たない `int` です。  
重大度、用途、出力先など、値の意味と有効な範囲は利用者が決めます。cplat は値を検査せず、保持して返すだけです。

0 は分類なしを表します。  
利用者が 0 を意味のある分類値として登録することもできますが、その場合はカタログに存在しない文字列 ID と区別できません。  
区別が必要な場合は、先に `cplat_string_catalog_get_id_text()` で存在を確認します。

この app では、分類値をトレース レベルとして使います。  
値は cplat の `cplat_trace_level` と同一です。生成物は `cplat_trace_level` に依存せず、値を解釈する側だけが `<cplat/trace/tracer.h>` を include します。

範囲外の値は、コマンドの `trace_level_of()` が `CPLAT_TRACE_LEVEL_NONE` へ切り詰めます。  
意味付けを行う階層で切り詰めるため、cplat とそのテストは分類値の範囲を確認しません。

この app はトレースの出力機構を持ちません。  
レベルは、利用側が出力先や絞り込みを決めるための情報として保持します。

## カタログ指定を省略する簡易関数

カタログを引数に取る形は、複数のカタログを扱うために必要ですが、1 つしか使わない呼び出し側には手間です。  
そこで、カタログの指定を省略して呼び出すための簡易関数を利用側の生成物に配置します。

```c
/* sample_messages.c が持つ */
static const cplat_string_catalog s_catalog = {s_entries, s_id_index, ENTRY_COUNT, ID_INDEX_COUNT};

int sample_messages_format(char *dest, size_t dest_size, int string_id, ...)
{
    /* s_catalog を補って cplat_string_catalog_format() を呼び出す */
}
```

この簡易関数を cplat ではなく生成物へ配置するのは、既定カタログという状態を cplat に持たせないためです。  
cplat は状態を持たないまま、呼び出し側は 1 つのカタログを短く書けます。  
可変長引数はいったん `va_list` にして `cplat_string_catalog_vformat()` へ中継します。

簡易関数の名前はモジュール接頭辞のままです。  
`sample_messages_get_category()` のように cplat 側の動詞を写さないのは、この関数群が特定のカタログに固有の利用側 API であるためです。

## 文字列 ID ごとの型付きラッパー

可変長引数を取る関数では、引数の個数や型をコンパイラが検査できません。  
書式と引数スキーマがカタログの中にあり、書式文字列が呼び出しの実引数ではないためです。  
`printf` に付けられる `format` 属性も、付ける先の引数が無いため使えません。

そこで、文字列 ID ごとに引数の型を固定したラッパーを生成物へ並べます。  
通常のプロトタイプ検査が働き、Doxygen コメントによって利用者は各引数の意味をインテリセンス上で参照できます。

```c
static inline int sample_messages_id_file_open_failed(char *dest, size_t dest_size,
                                                      const char *file_path, int error_code);
```

関数名は文字列 ID から機械的に導出します。文字列 ID の定数名を小文字化しただけの名前です。  
導出は生成器の `wrapper_name()` が行い、`test_string_catalog_gen.py` が規則を固定しています。

```text
SAMPLE_MESSAGES_ID_FILE_OPEN_FAILED
  → 小文字化 → sample_messages_id_file_open_failed
```

文字列 ID の定数名にモジュール接頭辞を含めるため、ラッパー名は自然に利用者側の名前空間へ収まります。  
同じ生成物が出す簡易関数 (`sample_messages_format()` など) と名前が衝突する定数名は、生成器が定義の誤りとして弾きます。

語の除去や入れ替えは行わないため、規則に例外がありません。  
区切りのアンダースコアは 1 個です。2 個続けると、C++ が処理系用に予約する識別子になります。

引数の型は、引数種別から cplat の公開ヘッダー `cplat/string_catalog/argument.h` の対応表で決まります。  
引数名は生成元の定義が持つ情報であり、カタログのデータには含まれません。

`static inline` とするのは、実体を持つ翻訳単位を増やさず、文字列 ID の数だけ公開シンボルが増えることを避けるためです。  
関数形式マクロは採用しません。展開後が可変長引数の呼び出しのままで、型検査が働かないためです。  
`static_assert` をマクロへ組み込む形も採れません。`static_assert` は宣言であって式ではなく、値を返す形にするには GNU 文式が必要で、[コーディング規範](../../general/docs/coding-guideline.md) が MSVC 非対応を理由に禁止しています。

検査の強さは診断の種類で異なります。  
引数の個数違いはコンパイル エラーです。  
ポインターと整数の取り違えは `-Wint-conversion` の警告、幅や符号の取り違えは `-Wconversion` と `-Wsign-conversion` の警告として `.warn` に現れます。  
可変長引数のままでは、いずれの診断も出ません。

## 添字表による探索

文字列 ID からカタログを引く探索は、既定では線形探索です。  
利用者は、文字列 ID を添字としてカタログの添字を引く表を添えられます。

```text
cplat_string_catalog = {entries, id_index, entry_count, id_index_count}

  id_index[string_id] -> entries の添字 (未登録は負の値)
```

添字テーブルを渡すことで、線形探索からインデックス参照へ置き換えます。  
文字列 ID が小さい非負整数である場合に使用できます。  
文字列 ID が添字テーブルの範囲を超える場合は、線形探索へフォールバックします。

生成器は添字表も書き出すため、利用側が手で整合を保つ必要はありません。  
`cplat_string_catalog_verify()` は、各文字列が添字表を通して自分自身へ到達することも確認します。

## カタログの点検

`cplat_string_catalog_verify()` は、カタログ全体の書式が引数スキーマと矛盾しないことを確認します。  
カタログは静的に確定するため、点検は起動時に一度実行すれば十分です。  
複数のカタログを使う場合は、カタログごとに点検します。

サンプル コマンドは、起動直後に `sample_messages_verify()` を呼び出します。  
不正が検出された場合はエラー終了します。

## 依存関係

この app は `app/c-platform` に依存します。`appdeps.mk` の `APP_DEPS` に `c-platform` を指定し、コマンドとテストは `LIBS += cplat` でリンクします。

文字列カタログの API に加えて、コンソールの UTF-8 初期化 (`cplat_console_init` / `cplat_console_dispose`) と、分類値の意味付けに使うトレース レベルの列挙 (`cplat_trace_level`) を利用します。

結果コードは cplat 共通の `CPLAT_OK` と `CPLAT_ERR_*` です。

## テストの構成

文字列カタログ自体の単体テストは `app/c-platform` にあります。この app に残るのは、利用側の資産を対象とするテストだけです。

| テスト | 置き場所 | 対象 |
|---|---|---|
| `sampleMessagesTest` | `test/src/cmd/` | コマンドが用意するカタログと添字表 |
| `catalogIntegrationTest` | `test/src/integration/` | コマンドのカタログと cplat の組み立てを結合した確認 |

`sampleMessagesTest` を `test/src/cmd/` に置くのは、対象がライブラリではなく、コマンドが用意するソースであるためです。  
`catalogIntegrationTest` は個々のソースのカバレッジを目的としないため `TEST_SRCS` を宣言せず、生成物を `ADD_SRCS` で取り込み、cplat を実体でリンクします。
