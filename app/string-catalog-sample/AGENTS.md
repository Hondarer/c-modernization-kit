# AGENTS.md

## 対象

この app は、cplat の文字列カタログ機能を利用する側のサンプルです。  
カタログ定義から文字列 ID の列挙とカタログの表を生成し、言語を切り替えながら文字列を組み立てます。

`app/c-platform` の `cplat_string_catalog_*`、`cplat_trace_level`、`cplat_console_init/dispose` を利用します。  
文字列カタログの実装、生成器、機能仕様は `app/c-platform` にあり、この app には含みません。

## 作業別の参照先

- 対象の目的と構成を確認する場合は [README.md](README.md)
- 利用側の設計 (定義から生成物、生成物から呼び出し) を変更する場合は [アーキテクチャー](docs/architecture.md) の該当節
- 文字列カタログの要件と外部から観測できる振る舞いは [文字列カタログ 機能仕様](../c-platform/docs/functional-spec/string_catalog.md)
- 文字列カタログの責務境界と変更時の制約は [string_catalog モジュール](../c-platform/prod/libsrc/cplat/string_catalog/README.md)
- C の規範は [コーディング規範](../general/docs/coding-guideline.md)
- テスト構成は [テスト方法](../../framework/testfw/docs/how-to-test.md)

## 変更時の制約

- 生成物は `prod/src/cmd/string-catalog-sample/gen/` へ置き、Git では管理しません。ビルドが cplat の `bin/string_catalog_gen.py` を駆動するため、手で実行する必要はありません。変更するのは `sample_messages.jsonc` です。
- 生成は app 直下の `makepart.mk` が makefile のパース時に行います。ビルド規則にしないでください。テストのディレクトリを解釈する時点で生成物が必要になるためです。
- 生成器そのものを変更する場合は `app/c-platform` 側で行い、その単体テストを実行してください。
- 生成ヘッダーは `#include "gen/sample_messages.h"` の形で取り込みます。テストから引き込む場合は `INCDIR` へ `gen` を加えてください。
- `sample_messages.jsonc` がカタログ定義の正本です。形式は JSONC で、行コメント、ブロック コメント、末尾コンマ、長文のための文字列配列を書けます。
- 利用者側の識別子は `sample_` で始めます。cplat 側の `cplat_string_catalog_` と名前空間を分け、どちらの資産かを名前だけで判別できるようにするためです。型付きラッパーの関数名も利用側の名前空間とし、`cplat_` を前置しません。
- カタログ定義に書くのは、著者、日付、版、文字列の一覧だけです。導出できる名前と、cplat が定める仕様を持ち込まないでください。
    - モジュール接頭辞は定義ファイルの名前です。`sample_messages.jsonc` なら `sample_messages` になります。英小文字で始まる snake_case にしてください。
    - 生成物のファイル名はモジュール接頭辞そのもの、文字列 ID の列挙名はそれに `_id` を続けた名前です。
    - `texts` と `notes` に書ける言語は、生成器の定数 `LANGUAGES` が持ちます。
    - Doxygen が示す置き場所は、定義ファイルの位置から導出します。絶対パスの中で最も近い `prod` または `test` が起点です。
- カタログの名前空間を移す場合は、定義ファイルの名前を変えてください。生成物のファイル名と識別子がすべて追従します。
- 生成物が依存してよいのは、cplat の公開ヘッダーと標準ヘッダーだけです。特定の app の型や列挙へ依存させないでください。生成物をそのまま持ち運べるようにするためです。
- 分類値は生値で保持します。定義ファイルにも整数で書き、意味はコメントで示してください。分類値の解釈には `cplat_trace_level` を使用します。値を解釈する側が `<cplat/trace/tracer.h>` を直接インクルードします。
- 分類値の重大度の意味付け (`cplat_trace_level`) は本 app の規約です。`trace_level_of()` で範囲外の値をフォールバックし、表示名のインデックスとして安全に使用できるようにします。
- 文字列 ID の値は、定義の並び順から 1 始まりで生成器が決めます。安定させる必要がある識別子は固定文字列 (`id_text`) であり、これは定義ファイルへ明記します。
- 文字列 ID の定数名には、モジュール接頭辞を含めてください。型付きラッパーの関数名が定数名の小文字形になるため、簡易関数の名前と衝突する余地を無くします。
- 生成物はカタログ識別オブジェクトと、カタログ指定を省略する簡易関数 (`sample_messages_format` など) を持ちます。cplat の文字列カタログ API を増やした場合は、対応する簡易関数も同じ変更で追加してください。
- 型付きラッパーは `static inline` として `sample_messages.h` に置き、`.c` へ実体を作らないでください。文字列 ID の数だけ公開シンボルが増えることを避けます。関数形式マクロにしないでください。型検査が働かなくなります。
- 添字表を変更した場合は、カタログの配列と食い違っていないことを `cplat_string_catalog_verify()` で確認してください。
- ニュートラル言語の要素を欠かさずに定義してください。ほかの言語の要素は、省略するとニュートラル言語へ読み替えられます。

## 局所確認

振る舞いを変更した場合は、影響する局所テストを実行してください。  
app 全体の確認には app 直下の `make test` を使用できます。  
ビルド後は、対象範囲の内容がある `.warn` を確認してください。

カタログの内容を変更した場合は、次のいずれかで整合を確認してください。  
サンプル コマンドは、起動直後に `sample_messages_verify()` を呼び出します。

```bash
./prod/cbin/string-catalog-sample
cd test/src/cmd/sampleMessagesTest && make test
```
