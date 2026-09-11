# コーディング規範

## 概要

C / C++ コードでの整数型の選択、関数引数の異常入力対応、変数宣言位置の扱いなど、コーディング規範を本書に集約します。  
適用範囲は主に `app/` 配下の C / C++ コードです。

本書は、ログ / トレース、テスト規約、ヘッダー設計など、コーディング規範を順次追加していくことを想定しています。  
現版では「一括変更で対象外とした箇所の明示」「命名規則」「ゼロ初期化」「構造体のパディングと予約フィールド」「整数型の選択」「整数演算の安全性」「関数引数の異常入力対応」「異常状態の検出とプロセス終了」「動的メモリの確保と解放」「エラー処理と戻り値規約」「変数宣言位置と命令文の関係」「式の括弧」「制御構造の制限」「restrict、volatile、inline、register の利用」「関数引数の const 付与と Doxygen 方向タグ」「スレッド安全性の Doxygen 記載」「宣言と定義の関係」「API 設計における概念の分離」「Doxygen コメントのプレースホルダー表記」「Doxygen コメントの @p などコマンド引数と日本語句読点の間隔」「Doxygen コード例内のコメント形式」を記載します。

本書は一般的な方針を定めるものです。  
各 app のドキュメントに優先事項、特化事項がある場合は、それに従ってください。

### 前提とする言語標準

本書は **C17** を前提とします。  
テスト コードなど C++ で記述する部分は **C++17** を前提とします。

C17 より後の標準でのみ利用できる機能 (C23 の `bool` キーワード、`constexpr`、`typeof` など) は使用しません。  
C17 より前の標準にしかない書き方へ戻すこともしません。

標準バージョンは `framework/makefw/makefiles/_flags.mk` の `C_STANDARD` と `CXX_STANDARD` で指定します。  
いずれも既定値は `17` です。個別の app が別の値を指定する場合は、その理由を当該 app のドキュメントに明記します。

処理系拡張 (`__attribute__`、`__declspec` など) の **参照** は、[予約識別子の回避](#予約識別子の回避) の範囲で使用できます。

本書は、再度同じ種類のリファクタリング作業を行うときに決定論的に判断できる詳細さで記述します。  
方針を追記するときは、判定基準の表、判定手順、望ましい/望ましくないコード例、例外条件、検証コマンドを含めます。

関連する既存ガイドラインは [参照](#参照) を参照してください。

### 本書の記述形式

本書は「何をするか」を定める文書です。  
従うべき規範本文と、その判断の背景は、次のとおり書き分けます。

| 記法 | 用途 |
|---|---|
| 平文の段落・表・コード例 | 規範本文。従うべき規則、判定手順、望ましい / 望ましくない例 |
| `> [!NOTE]` | 背景、判断の根拠、業界一般の慣行との関係。規範として従う対象ではありません。 |
| `> [!IMPORTANT]` | 見落とすと規範違反になる要点、規則どうしの適用条件の違い |
| `> [!WARNING]` | 誤った書き方が引き起こす具体的な障害 (ビルド エラー、リーク、情報漏えい、ABI 不整合) |

出典の URL は本文中に置かず、末尾の [参照](#参照) へ集約します。  
本文からは `> [!NOTE]` の中で規格名や条項名を挙げ、詳細は参照節へ誘導します。

admonition の記法と対応する Doxygen タグは [`framework/docsfw/docs/sample/admonition.md`](../../../framework/docsfw/docs/sample/admonition.md) を参照してください。

## 一括変更で対象外とした箇所の明示

一括置換 (raw API 呼び出しのラッパー化、共通結果コードへの移行など) で意図して対象外とした箇所には、対象外である理由をコメントとして該当箇所に残します。  
対象外の判断を撤回して置き換えた場合は、そのコメントも同時に除去します。

> [!WARNING]
> 理由コメントがないと、将来の一括置換や grep ベースの点検で「置換漏れ」と誤認され、不適切に変更されるおそれがあります。

対象外とする範囲を規則として定める場合は、対象パスに適用される `AGENTS.md` またはライブラリのドキュメントへ記載します。  
個々の箇所に残すコメントは、その規則のどれに該当するかが読み取れる内容にします。

## 命名規則

### 適用範囲と用語の定義

本章は `app/` 配下の C / C++ コードで定義するすべての識別子を対象とします。  
外部 OSS 由来のコード、過去資産の保守的移植が対象パスに適用される `AGENTS.md` で宣言されているもの、OS / SDK が定義する型やマクロの alias は対象外です。

命名を決める前に、対象のシンボルがどのスコープに属するかを次の手順で判定します。  
上から順に評価し、最初に一致した行のスコープとします。

| 判定順 | スコープ | 判定基準 |
|---|---|---|
| 1 | 公開 | `prod/include/` 配下のヘッダーで宣言されている |
| 2 | ライブラリ内共有 | `prod/include_internal/` 配下のヘッダーで宣言されている |
| 3 | モジュール内共有 | 実装と同じディレクトリに置いたモジュール私有ヘッダーで宣言されている |
| 4 | ファイル内 | どのヘッダーでも宣言されず、`static` が付いている |
| 5 | 関数内ローカル | 関数本体の内側で宣言されている |

スコープはヘッダーの配置と `static` の有無で判定します。  
ライブラリ内共有の関数・型・外部リンケージ変数は、名前にも公開境界のマーカー (`_internal_`) を含め、公開シンボルと区別します。

モジュール内共有は、1 つのディレクトリに置いた複数の `.c` だけが共有するスコープです。  
`include_internal/` に置かないため他モジュールからは参照できず、公開境界のマーカーは付けません。  
詳細は [モジュール私有ヘッダー](#モジュール私有ヘッダー) を参照してください。

> [!IMPORTANT]
> ヘッダー **ファイル名** の `_internal` サフィックスは、同名の公開ヘッダーが存在するときに限り付けます。  
> シンボル規則とは付与条件が異なります。詳細は [ヘッダー ファイル名の _internal](#ヘッダー-ファイル名の-_internal) を参照してください。

### スコープ別一覧

`<lib>` はライブラリ接頭辞を表すプレースホルダーです。

| スコープ | リンケージ | 宣言場所 | 記法 | 接頭辞 | 例 |
|---|---|---|---|---|---|
| 関数内ローカル変数 | なし | 関数本体 | snake_case | 付けない | `result`、`buf_size` |
| static 関数 | 内部 | `.c` 内 | snake_case | 付けない | `parse_header` |
| ファイル内共有変数 | 内部 | `.c` のファイル スコープ | snake_case | `s_` | `s_instance_count` |
| ライブラリ内共有関数 | 外部 | `include_internal/` | snake_case | `<lib>_internal_` | `sample_internal_registry_add` |
| ライブラリ内共有変数 | 外部 | `include_internal/` の `extern` | snake_case | `g_<lib>_internal_` | `g_sample_internal_default_config` |
| モジュール内共有関数 | 外部 | モジュール私有ヘッダー | snake_case | `<module>_` | `trace_cli_process_line` |
| モジュール内共有変数 | 外部 | モジュール私有ヘッダーの `extern` | snake_case | `g_<module>_` | `g_trace_cli_default_session` (必要最低限に厳選) |
| 公開関数 | 外部 | `include/` | snake_case | `<lib>_` | `sample_file_get_size` |
| 公開共有変数 | 外部 | `include/` の `extern` | snake_case | `g_<lib>_` | `g_sample_default_limits` (必要最低限に厳選) |
| 型 (struct / enum / union / 関数ポインター) | - | 宣言場所に従う | snake_case | 公開は `<lib>_`、ライブラリ内共有は `<lib>_internal_` | `sample_context`、`sample_internal_registry`、`sample_hook_fn` |
| 列挙定数 / マクロ | - | - | 全大文字 | `<LIB>_` | `SAMPLE_TRACE_LEVEL_INFO` |

> [!NOTE]
> 表の各行は、次の原理から導かれます。  
> 個別のケースで判断に迷う場合は、この原理に立ち返って決定してください。
>
> - 変数の `s_` / `g_` は **リンケージ** と「これは変数である」ことを表します。`static` なら `s_`、外部リンケージなら `g_` です。
> - ライブラリ接頭辞は **リンカー名前空間** を表します。外部リンケージを持つシンボルにのみ付け、`static` 関数・`s_` 変数には付けません。
> - `_internal_` は **公開境界** を表します。`include_internal/` で宣言する関数・型には `<lib>_internal_`、外部リンケージ変数には `g_<lib>_internal_` の形で付けます。公開ヘッダーで宣言するシンボルと `static` には付けません。
> - モジュール私有ヘッダーで宣言するシンボルは、リンカー名前空間を共有するため接頭辞を必要としますが、公開境界を越えないため `_internal_` は付けません。接頭辞にはモジュール名を使います。

### 出力引数

関数が呼び出し元へ値を書き戻す仮引数の名前は、`{name}_out` の接尾辞にします。  
`out_{name}` の接頭辞は使いません。

```c
/* 望ましい */
int sample_paths_equal(const char *lhs, const char *rhs, int *equal_out, int *errno_out);
int sample_context_open(const char *path, sample_context **context_out);

/* 望ましくない */
int sample_paths_equal(const char *lhs, const char *rhs, int *out_equal, int *out_errno);
int sample_context_open(const char *path, sample_context **out_context);
```

真偽の出力は、`_out` の直前を真偽を表す語にします。  
例と公開 API での型は [真偽値の型](#真偽値の型) の命名、および [真偽値や状態を返す API の設計](#真偽値や状態を返す-api-の設計) を参照してください。

容量を表す [in] 引数は `{name}_size` とします。  
`out_{name}_sz` や `{name}_out` にはしません。

```c
int sample_path_dirname(char *dir_out, size_t dir_size, sample_error *detail_out, const char *path);
```

`_out` を付けると役割が重複する、または書き戻す値の意味が薄れる名前は使いません。

| 避ける名前 | 理由 | 代わり |
|---|---|---|
| `out` | 方向だけで、書き戻す値の意味が無い | 意味名 + `_out`、または変換先の `dest` |
| `out_buf` / `buf_out` | 「出力バッファー」を二重に述べる | 書き戻す値の意味名 (`value_out`、`key_out`) |
| `dest_out` | `dest` がすでに書き込み先を表す | `dest` |
| `{name}_out_size` / `out_{name}_sz` | 容量は入力であり、出力値ではない | `{name}_size` |

変換・整形 API の書き込み先は、CRT の `strcpy_s` 系に合わせて `dest` / `dest_size` を使います。  
ここへ `_out` は付けません。

```c
int sample_strcpy(char *dest, size_t dest_size, const char *src);
```

出力引数を受ける一時変数は、[関数内ローカル変数](#関数内ローカル変数) のとおり、対応する引数名から `_out` を除いた名前とします。

次は出力引数ではないため、本節の `{name}_out` を付けません。

- 標準出力のハンドル (`STD_OUTPUT_HANDLE` を受ける変数)
- 出力ファイルのパスを表す入力文字列
- Win32 の `nOutBufferSize` に対応する [in] のパイプ容量
- 後始末用の `goto` ラベル (`out_free_buffer` など)

#### 検証

次の検索結果をレビューし、出力引数の `out_` 接頭辞と、上表の不自然な `_out` が残っていないことを確認します。

```bash
rg -n --glob '*.{c,h,cc}' --glob '!**/obj/**' --glob '!app/lua/prod/**' --glob '!app/sqlite/prod/**' \
  --glob '!app/cjson/prod/**/cJSON.[ch]' --glob '!app/cjson/prod/**/cJSON_Utils.[ch]' \
  '\bout_[A-Za-z][A-Za-z0-9_]*|\b(buf_out|dest_out)\b' app
```

ヒットは使用場所を見て、除外対象と出力引数を区別します。

### テストのモック オブジェクト変数

Google Mock の Mock クラスを格納する変数名は、その Mock クラス名をすべて小文字にした識別子とします。  
`NiceMock` / `StrictMock` / `NaggyMock` で包む場合も、テンプレート引数の Mock クラス名を小文字にします。

判定手順は次のとおりです。

1. 変数の型が `Mock_` で始まるか、または `NiceMock` / `StrictMock` / `NaggyMock` のテンプレート引数が `Mock_` で始まるかを確認します。
2. いずれでもない場合は、通常のローカル変数またはメンバー変数の規則を適用します。
3. いずれかである場合は、Mock クラス名の各文字を小文字にした識別子を変数名とします。
4. 型名に含まれない接頭辞や末尾 `_` は付けません。

| 型 | 変数名 |
|---|---|
| `Mock_cplat` | `mock_cplat` |
| `NiceMock<Mock_cplat>` | `mock_cplat` |
| `Mock_stdio` | `mock_stdio` |
| `NiceMock<Mock_stdio>` | `mock_stdio` |

```cpp
/* NG: どの Mock クラスかが名前から分からない */
NiceMock<Mock_cplat> mock_;
NiceMock<Mock_stdio> mock;

/* OK: 型名を小文字にした識別子 */
NiceMock<Mock_cplat> mock_cplat;
NiceMock<Mock_stdio> mock_stdio;
```

> [!IMPORTANT]
> Fixture のメンバーでも同じ規則です。  
> `mock_` や `mock` のような省略名、および型名にない末尾 `_` (`mock_cplat_` など) は使いません。

> [!WARNING]
> `mock_` のまま複数の Mock クラスを扱うと、`file_open` の既定差し替えと `malloc` の失敗注入を別インスタンスへ書いてしまい、本物の I/O が単体テストから呼ばれることがあります。

同一テスト関数に複数の Mock クラスがあるときは、それぞれ型名の小文字を使います。  
引数で Mock オブジェクトを受け取るヘルパーも、仮引数名を同じ規則に揃えます。

例外は次に限ります。

- Google Test / Google Mock 本体、および外部 OSS 由来のテスト補助コード
- 既存コードを変更しない保守。当該変数を触る変更では、同時に本規則へ合わせます

検証コマンドは次のとおりです。対象 app に範囲を限定してください。

```bash
# Mock 型なのに変数名が mock_ または mock になっている宣言
rg -n --glob '*.cc' --glob '*.h' 'Mock_[A-Za-z0-9_]+>\s+mock_;' .
rg -n --glob '*.cc' --glob '*.h' 'Mock_[A-Za-z0-9_]+>\s+mock;' .
```

### 予約識別子の回避

規格が処理系用に予約している識別子の形式は使用しません。

| 禁止する形式 | 予約の根拠 |
|---|---|
| `_t` サフィックス | POSIX.1 の名前空間規定は、標準ヘッダーをインクルードしたときに `_t` で終わる型名を処理系用に予約します |
| アンダースコアで始まるファイル スコープ識別子 | C 標準は、アンダースコアで始まるすべての識別子を、ファイル スコープの通常識別子およびタグ名前空間で予約します |
| 連続するアンダースコア (`__`) を含む識別子 | C++ 標準は、識別子の任意の位置に連続する `__` を含む名前を処理系用に予約します |

`_t` の禁止は `typedef struct` / `typedef enum` / `typedef union` / 関数ポインター typedef のすべてに適用します。  
アンダースコア始まりの禁止と `__` の禁止は、関数名、変数名、型名、マクロ名、インクルード ガードなど、ユーザーが **定義** する識別子に適用します。  
インクルード ガードにおける予約識別子の詳細は [`include-guard-guideline.md`](include-guard-guideline.md) を参照してください。

> [!WARNING]
> 予約識別子を定義すると、将来の libc や処理系の拡張とシンボルが衝突し、原因の分かりにくいビルド エラーや未定義動作を招きます。  
> 処理系や将来のコンパイラ拡張が同じ綴りをマクロや組込み識別子として使うと、診断メッセージから原因を追えなくなります。

> [!NOTE]
> C 単独では、識別子の途中に現れる `__` への制約は相対的に緩いものです。  
> 本リポジトリはテストや一部実装が C++ であり、ヘッダーを共有するため、C ソースでも C++ の予約規則に合わせて `__` を避けます。  
> ライブラリ内共有の境界は `__` ではなく `_internal_` で表します (単一の `_` の並びであり、連続 `__` ではありません)。

処理系が提供する識別子やキーワード拡張の **参照** は禁止対象外です。  
`__FILE__`、`__LINE__`、`__func__`、`__declspec`、`__attribute__`、`_WIN32`、`__GNUC__` など、コンパイラや OS が定義するものを条件判定や属性に使うのは問題ありません。

例外は次の 2 つに限ります。

- OS / SDK / 外部 ABI が定義する型の alias で、元の型名を保存する必要があるもの。この場合は、例外である理由をヘッダーのコメントに残します
- 外部 OSS 由来のコード。改変しません

アンダースコア始まりは、次の 3 つの用途で使われがちです。  
それぞれの代替を以下のとおり定めます。

| 用途 | 規則 | 例 |
|---|---|---|
| マクロが名前を占有した関数の実体 | 呼び出し元情報を引数で受ける形とし、`_at` サフィックスを付ける | `sample_log_write_at(context, message, file, line)` |
| 既定インスタンス版と明示ハンドル版の対 | 明示ハンドル版を正名とし、既定インスタンス版に `_default_` を挟む | `sample_parser_parse(parser, ...)` と `sample_parser_default_parse(...)` |
| テスト専用フック | `_for_test` サフィックスのみで表し、前置きを付けない | `sample_shutdown_reset_for_test()` |

> [!NOTE]
> 既定インスタンス版と明示ハンドル版の対で明示ハンドル版を正名とするのは、ハンドルを先頭引数に取る形が引数順序の規約に準拠した形であり、既定インスタンス版はそこからハンドルを暗黙化した派生形だからです。  
> 規約に準拠した形が装飾のない名前を持つようにします。

### 関数内ローカル変数

snake_case とし、接頭辞は付けません。  
ハンガリアン記法 (`sz`、`lp`、`dw` などの型を表す接頭辞) は使用しません。

#### 結果コード変数 (ret と result)

新規コードでは、結果コードに次の 2 役を使い分けます。

| 変数 | 役割 | 初期化 |
|---|---|---|
| `ret` | 呼び出した API の結果コードを一時的に受ける作業変数 | 1 関数の戻り値だけを受けるときは初期化子でよい。複数関数で使い回すときは宣言と代入を分ける。成功 / 失敗定数やリテラルでは初期化しない |
| `result` | 自関数が呼び出し元へ返す結果コードを蓄える変数 | 付けてよい (例: `int result = SAMPLE_OK;`) |

`rc` と `rtc` は、生産コードにおける `ret` の歴史的な別名です。  
新規の生産コードでは `ret` を使い、`rtc` は追加しません。  
既存の生産コードの `rc` / `rtc` 等を、本規則だけを目的に全面置換しません。  
`result` の別名として `rc` / `rtc` を使いません。

試験側では `rtc` も `rtc_*` も使いません。  
mock 関数本体は `mock_ret`、テスト本体は `actual_ret` (複数なら `actual_ret_<区別>`) です。  
どちらも結果コード以外の戻り値を含み、生産コードの `ret` とは混ぜません。  
対象範囲と対象外は [How to mock](../../../framework/testfw/docs/how-to-mock.md) の「試験側の戻り値中継」を正とします。

```c
/* 望ましい (単純な透過 return — result は増やさない) */
int ret = sample_file_get_size(&size, path);

if (ret != SAMPLE_OK)
{
    return ret;
}

return SAMPLE_OK;
```

```c
/* 望ましい (複数経路で結果コードを組み立てる) */
int ret;
int result = SAMPLE_OK;

ret = sample_open(path, &handle);
if (ret != SAMPLE_OK)
{
    result = ret;
    goto out;
}

ret = sample_read(handle, buffer, size);
if (ret != SAMPLE_OK)
{
    result = ret;
    goto out;
}

out:
    if (handle != NULL)
    {
        sample_close(handle);
    }
    return result;
```

単純に `return ret;` だけで足りるときは、`result` を導入しません。

結果コード以外の値 (サイズ、個数、合計、ポインターなど) を `return` する変数には、`ret` / `rc` / `rtc` も `result` も使いません。  
`size`、`count`、`total` など、意味のある名前を使います。

```c
/* 望ましい (結果コード以外の return) */
size_t total = 0;
/* ... */
return total;
```

その代入が 1 関数の戻り値である場合は、初期化子で受けてよいです。  
複数関数の戻り値で同じ `ret` を使い回す場合は、変数宣言と代入を分離します。  
`rc` / `rtc` を使う場合も同じです。

```c
/* 望ましい (1 関数の戻り値だけを受ける) */
int ret = sample_file_get_size(&size, path);
```

```c
/* 望ましくない (複数関数で使い回すのに、初回だけ宣言を兼ねている) */
int ret = sample_open(path, &handle);

ret = sample_read(handle, buffer, size);
```

```c
/* 望ましくない (まだ呼んでいないのに成功を載せている) */
int ret = SAMPLE_OK;
```

```c
/* 望ましくない (リテラルで埋めている) */
int ret = -1;
```

> [!NOTE]
> 1 関数の戻り値だけを受けるときの `int ret = sample_open(...);` は、最初の受けそのものであり、ダミー値ではありません。  
> 複数関数で使い回すときに宣言と代入を分けるのは、すべての受けが同じ形の代入になり、初回の代入漏れを未初期化警告で見つけやすくするためです。  
> 禁止するのは、成功定数、失敗定数、リテラルによる初期化です。  
> `int ret = SAMPLE_OK;` のように成功値で初期化すると、まだ API を呼んでいないのに成功を返してしまう経路を隠すことがあります。  
> 失敗値で初期化する案も、本リポジトリの使い方とは合いません。同じ `ret` に複数ライブラリ・複数カテゴリの API の結果コードを順に載せるため、初期値として載せるべき「不明エラー」定数を一意に決められません。  
> 最初に呼ぶ API の不明エラー定数を常に入れる規則にすると、呼び出し順や対象 API が変わったときに初期化行も追従させる必要があり、保守性を損ないます。  
> リテラルの `-1` で埋めると、結果コードは名前付き定数との比較を正とする方針から外れます。  
> 成功を既定にして複数出口から返す cleanup では、蓄積側の `result` に初期値を付けます。

> [!NOTE]
> 1 関数の戻り値だけを受けるときの宣言位置は、[変数宣言位置と命令文の関係](#変数宣言位置と命令文の関係) の「利用箇所に近い位置へ置く」原則と両立します。  
> 複数関数で使い回す `ret` は、関数先頭で宣言し、各呼び出しの位置で代入します。

同一関数内で複数の API の結果コードを順に受ける場合、同じ `ret` を代入し直して構いません。  
直前の呼び出し以外の結果コードを後で参照する必要があるときだけ、別変数を使います。

```c
/* 望ましい (同じ ret の使い回し) */
int ret;

ret = sample_open(path, &handle);
if (ret != SAMPLE_OK)
{
    return ret;
}

ret = sample_read(handle, buffer, size);
if (ret != SAMPLE_OK)
{
    sample_close(handle);
    return ret;
}
```

> [!NOTE]
> [Linux Wireless の ath10k / ath11k / ath12k coding style](https://wireless.docs.kernel.org/en/latest/en/users/drivers/ath10k/codingstyle.html#status-error-variables) は、戻り値または状態コードを格納する変数名に `ret` を使うことを明記しています。  
> [BoringSSL API Conventions](https://boringssl.googlesource.com/boringssl/+/HEAD/API-CONVENTIONS.md) は、cleanup を伴う C 関数で `int ret = 0;` と `goto err` を使い、自関数の成否を組み立てる例を示しています。  
> これらの文書と本規範では、戻り値体系や `ret` の詳細な役割が異なるため、同一の規則とは扱いません。  
> これらの実例は、`ret` が本リポジトリ固有の略記ではないことを示します。  
> ローカル変数を短く保つ方針は [Linux kernel coding style](https://www.kernel.org/doc/html/latest/process/coding-style.html) にも示されています。  
> Python などで「変数の使い回しを避ける」と教わるのは、意味の異なる値を同じ名前に載せることへの警戒が本筋です。  
> 結果コードという同一の役割を上書きするだけなら、呼び出しごとに `open_result` のような長い名前を付ける必要はありません。情報は増えず、走査とレビューの負荷だけが増えます。

結果コード用の変数に、呼び出しごとに異なる長い名前 (`open_result`、`file_get_size_ret` など) は付けません。  
本体の出力値は、対応する引数名から `_out` を除いた名前など、意味のある名前を使います。

次の名前は、結果コード変数には採用しません。

| 名前 | 採用しない理由 |
|---|---|
| `err` | 成功 (`0` / `*_OK`) も含む変数を、エラー専用のように読ませる |
| `status` | Win32 や状態機械の status と混同しやすく、本リポジトリの結果コード体系 (`*_OK` / 負の分類) との対応が弱い |

`result` は **自関数が返す結果コードの蓄積** に限り使います。  
計算の合計や変換結果など、結果コードではない値には `result` を使わず、`total` や `size` などの意味名を使います。

出力引数を受ける一時変数は、対応する引数名から `_out` を除いた名前とします。  
仮引数側の接尾辞規則は [出力引数](#出力引数) を参照してください。

ループ カウンターの `i`、`j`、`k` と、走査用の汎用ポインター `p` は、宣言と使用が同一の短い範囲に収まる場合に限り使用できます。

### static 関数

snake_case とし、**ライブラリ接頭辞も `<lib>_internal_` も付けません**。

> [!NOTE]
> ライブラリ接頭辞は外部リンケージを持つシンボルの目印です。  
> `static` 関数に付けると外部から参照できるかのように読め、`nm` による公開シンボルの点検でも偽陽性を生みます。  
> `<lib>_internal_` はライブラリ内共有 (外部リンケージ) の目印であり、ファイル内に閉じた `static` 関数には付けません。

```c
/* 望ましい */
static int parse_header(const unsigned char *buffer, size_t size);

/* 望ましくない (ライブラリ接頭辞が付いている) */
static int sample_parse_header(const unsigned char *buffer, size_t size);

/* 望ましくない (static なのに internal マーカーが付いている) */
static int sample_internal_parse_header(const unsigned char *buffer, size_t size);
```

同一ファイル内で機能のまとまりを表す接頭辞 (`argparser_`、`config_` など) は、ライブラリ接頭辞と異なるため使用できます。

### ファイル内共有変数

`s_` を前置きし、続きを snake_case とします。  
`const` を付けた読み取り専用のテーブルも同様です。

```c
/* 望ましい */
static size_t s_instance_count;
static const char s_hex_chars[] = "0123456789abcdef";

/* 望ましくない (全大文字はマクロと紛らわしい) */
static const char DESCRIPTOR_MAGIC[4] = { 'S', 'M', 'P', 'L' };

/* 望ましくない (static なのに外部リンケージを示唆する g_) */
static volatile sig_atomic_t g_stop_requested;
```

関数内で宣言する `static` 変数は、ファイル内共有変数ではないため `s_` を付けません。  
関数内ローカル変数の規則に従います。

### モジュール内共有関数

モジュール名を接頭辞とし、続きを snake_case とします。  
`<lib>_internal_` は付けません。公開境界を越えず、`include_internal/` にも置かないためです。

```c
/* libsrc/cplat/hashtable/hashtable.h (モジュール私有ヘッダー) */
size_t hashtable_hash_key(const cplat_hashtable *ht, const void *key);

/* 望ましくない (公開境界を越えないのに internal マーカーが付いている) */
size_t cplat_internal_hashtable_hash_key(const cplat_hashtable *ht, const void *key);

/* 望ましくない (接頭辞がなく、ライブラリ内の他モジュールと衝突しうる) */
size_t hash_key(const cplat_hashtable *ht, const void *key);
```

同一ディレクトリ内で閉じる関数は `static` のままとし、私有ヘッダーへ宣言しません。  
配置とインクルード ガードの規則は [モジュール私有ヘッダー](#モジュール私有ヘッダー) を参照してください。

### ライブラリ内共有関数

`<lib>_internal_` を必須とし、続きを snake_case とします。  
`internal` はライブラリ接頭辞の直後に置き、語の途中や末尾には置きません。

```c
/* include_internal/sample/trace/trace_common.h */
int sample_internal_trace_resolve_timestamp(sample_timespec *timestamp_out);

/* 望ましくない (ライブラリ接頭辞がない) */
int trace_resolve_timestamp(sample_timespec *timestamp_out);

/* 望ましくない (公開と同じ接頭辞で、internal マーカーがない) */
int sample_trace_resolve_timestamp(sample_timespec *timestamp_out);

/* 望ましくない (internal の位置が接頭辞の直後ではない) */
int sample_trace_internal_resolve_timestamp(sample_timespec *timestamp_out);
```

昇格・降格では、宣言の移動に加えて接頭辞の付け替え (`sample_internal_foo` と `sample_foo` の相互変換) が必要です。  
呼び出し側もあわせて改名します。

> [!NOTE]
> 公開関数を `<lib>_`、ライブラリ内共有関数を `<lib>_internal_` と分けるのは、名前でも公開境界を表すためです。  
> ヘッダー配置による境界と名前の境界を一致させることで、`nm` やコード レビューで契約外シンボルを判別できます。

### ライブラリ内共有変数

`g_<lib>_internal_` を前置きし、続きを snake_case とします。  
`g_` の直後にライブラリ接頭辞、その直後に `internal_` を置き、語の途中や末尾には置きません。

> [!WARNING]
> 外部リンケージを持つ変数は、リンク時にライブラリ全体で 1 つの名前空間を共有します。  
> 接頭辞がないと、利用側のコードやほかのライブラリとシンボルが衝突します。  
> 公開境界を `g_<lib>_internal_` で表すことで、公開共有変数の `g_<lib>_` とも区別できます。

```c
/* include_internal/sample/base/registry_internal.h */
extern sample_internal_registry g_sample_internal_default_registry;

/* 望ましくない (公開と同じ形で、internal マーカーがない) */
extern sample_internal_registry g_sample_default_registry;
```

### 公開関数

`<lib>_` を必須とし、続きを snake_case とします。  
`<lib>_internal_` は付けません。

カテゴリ名詞と動詞の並び順、生成と破棄の動詞の対応など、公開 API 名の詳細な構成規則は各 app の特化事項ドキュメントで定めます。  
本書は接頭辞と記法までを定めます。

### 公開共有変数

公開ヘッダーで `extern` する共有変数は、必要最低限に厳選します。  
状態や設定を外に出す必要がある場合は、まずアクセサー関数の公開を検討します。

公開する場合は次を満たします。

- 名前は `g_<lib>_` を前置きし、続きを snake_case とする (`_internal_` は付けない)
- 読み取り専用を優先し、可能な限り `const` を付ける
- 変更可能なプロセス大域状態の公開は避ける

> [!WARNING]
> 共有ライブラリの境界をまたぐデータ シンボルは、Windows で `__declspec(dllimport)` の扱いが関数と異なり、インポート ライブラリとの不整合を起こしやすいです。  
> また、値の変更経路が追跡できず、スレッド安全性の保証も困難になります。

> [!NOTE]
> C では既定値テーブルなどの読み取り専用データ シンボルを公開せざるを得ない場面があるため、全面禁止とはしていません。  
> 上記のリスクを理由に、件数と役割を最小に保つ方針としています。

```c
/* 公開が妥当な例 (読み取り専用の既定値) */
extern const sample_limits g_sample_default_limits;

/* 望ましくない (変更可能な状態の公開) */
extern sample_context *g_sample_current_context;
```

### 型の命名規則

型名は snake_case とします。  
公開ヘッダーで定義する型には `<lib>_` を前置きします。  
`include_internal/` のヘッダーで定義する型には `<lib>_internal_` を前置きします。  
`.c` の内部だけで使う型には接頭辞を付けません。

いずれの型でも `_t` サフィックスは付けません。

#### typedef struct

構造体タグ名と typedef 名を同一にします。

完全定義は次の形式で記述します。

```c
/* 公開型 */
typedef struct sample_context
{
    int value;
} sample_context;

/* ライブラリ内共有型 */
typedef struct sample_internal_registry
{
    size_t count;
} sample_internal_registry;
```

不透明型は次の形式で宣言します。

```c
typedef struct sample_context sample_context;
```

API の引数や戻り値でポインターを扱う場合は、型名に `*` を付けて明示します。  
ポインターを typedef 名に隠しません。

```c
sample_context *sample_context_create(void);
void sample_context_dispose(sample_context *context);
```

次のように、ポインターを typedef で隠す形式は禁止します。

```c
typedef struct sample_context *sample_handle;
```

匿名 struct typedef も使用しません。

```c
typedef struct
{
    int value;
} sample_context;
```

#### typedef enum

`typedef struct` と同じく、タグ名と typedef 名を同一にします。  
匿名 enum の typedef は使用しません。

> [!NOTE]
> 匿名 enum を禁止するのは、前方宣言ができず、デバッガーの変数表示やコンパイラの診断メッセージに型名が現れないためです。

```c
/* 望ましい */
typedef enum sample_trace_level
{
    SAMPLE_TRACE_LEVEL_INFO = 0,
    SAMPLE_TRACE_LEVEL_WARNING = 1,
} sample_trace_level;

/* 望ましくない (匿名かつ _t サフィックス) */
typedef enum
{
    SAMPLE_TRACE_LEVEL_INFO,
} sample_trace_level_t;
```

列挙定数は全大文字とし、`<LIB>_` を前置きします。  
値が ABI として固定される enum では、全列挙定数に明示的な値を書き、新しい定数は末尾へ追加します。

#### 関数ポインター typedef

サフィックスは `_fn` に統一します。  
`_callback` / `_func` / `_handler` およびそれらに `_t` を付けた形は使用しません。

```c
/* 望ましい (公開) */
typedef void (*sample_hook_fn)(sample_context *context, void *user_data);

/* 望ましい (ライブラリ内共有) */
typedef void (*sample_internal_sink_write_fn)(const void *data, size_t size);

/* 望ましくない */
typedef void (*sample_hook_callback_t)(sample_context *context, void *user_data);
```

`_fn` の直前に `callback`、`func`、`handler` を重ねません (`sample_hook_callback_fn` は冗長です)。

#### typedef union

`typedef struct` と同じ規則に従います。  
タグ名と typedef 名を同一にし、匿名 union の typedef は使用しません。

#### 型命名の対象外

次の型は本節の対象外です。

- 固定幅整数型と標準ライブラリ型 (`uint32_t`、`size_t` など)
- OS / SDK / 外部 ABI が定義する型の alias
- 外部 OSS 由来のコードが定義する型

### マクロ

本節はマクロの **命名**、**用途の制限**、**本体の書き方** を定めます。  
ソース式の括弧は [式の括弧](#式の括弧) を参照してください。マクロ本体の括弧とは層が異なります。

#### 命名

全大文字とアンダースコアで構成し、`<LIB>_` を前置きします。

```c
/* 望ましい */
#define SAMPLE_PATH_MAX 260

/* 望ましくない (ライブラリ接頭辞がなく、他ライブラリと衝突しうる) */
#define PATH_MAX_LEN 260
```

公開ヘッダーで定義するマクロには、必ずライブラリ接頭辞を付けます。  
`include_internal/` で定義するマクロも `<LIB>_` を前置きします。マクロ名に `_INTERNAL_` を必須とはしません。  
`.c` の内部だけで使うマクロは接頭辞を省略できます。

例外として、関数と同名で呼び出し元情報を注入するマクロは、関数の見た目を保つため小文字で定義します。  
この場合は、注入先となる実体の関数名を「予約識別子の回避」の `_at` 規則に従って命名します。

```c
#define sample_log_write(context, message) \
    sample_log_write_at((context), (message), __FILE__, __LINE__)
```

#### マクロにしてよい用途

関数形式マクロとオブジェクト形式マクロは、次の用途に限り新設します。  
ここに当てはまらない処理や定数は、通常の関数、`static inline` 関数、または (適切な場合の) `enum` / `static const` で表します。

| 用途 | 説明 | 例 |
|---|---|---|
| 呼び出し位置の注入 | `__FILE__` / `__LINE__` / `__func__` を実体関数へ渡す | `sample_log_write` → `sample_log_write_at` |
| 可変個引数の糖衣 | 引数個数のカウントや、実体 `_n` 関数への転送 | `sample_path_concat` |
| 型を取る API | 型名トークンを引数に含むキャストや初期化 | `sample_resolve_as(fobj, type)` |
| 構造体初期化子の定型 | 定型の中括弧初期化子を共有します。 | `SAMPLE_ENTRY_INIT(key, type)` |
| コンパイル時整数定数 | `#if` 分岐、他マクロへの埋め込み、公開ヘッダーの整数定数 | 結果コード、ビット フラグ、パス上限 |
| 処理系・ABI の抽象 | 呼び出し規約、export、インライン強制など | `SAMPLE_EXPORT`、`FORCE_INLINE` |
| API 表の展開 | 1 つの関数一覧から宣言・実装・テストを生成する X マクロ | `MOCK_CJSON_RET` / `MOCK_SQLITE3_VOID` |

第三者共有ライブラリの mock を API 表から生成する方式は [共有ライブラリの mock 化](shared-library-mock-guideline.md) が正本です。  
API 表は include guard を持たず、展開に必要なマクロが未定義なら `#error` で停止する形とします。

> [!IMPORTANT]
> 結果コードを `#define` と `int` で表す方針は [エラー処理と戻り値規約](#エラー処理と戻り値規約) が正です。  
> 本節の「enum を優先する」一般論は、結果コード群には適用しません。

> [!NOTE]
> 業界では定数に `enum` / `static const` を、処理に `static inline` を優先する流れがあります (SEI CERT C PRE00-C など)。  
> 本規範は用途ホワイト リストで「マクロにしてよい場合」を正とし、それ以外を関数や定数表現へ導きます。  
> 公開ヘッダーの整数定数をすべて `static const` にすると、配列サイズや `#if` に使えず、翻訳単位ごとに実体が分かれるため、公開の整数定数は `#define` を正とします。  
> `.c` 内の読み取り専用データには `static const` を使って構いません。

次はマクロにせず、関数へ移します。

- 複数の文からなる処理、ローカル変数が欲しい処理
- 仮引数を本体で複数回評価する計算
- 仮引数以外の識別子を捕捉するファイル内ヘルパー (例: マクロ本体が引数に無い `p->member` を参照する)

#### 本体の書き方

##### 引数と本体の括弧

関数形式マクロで **式として使う** 各仮引数の出現は、括弧で囲みます。  
式に展開するマクロの置換リスト全体も、括弧で囲みます。

```c
/* 望ましい: 引数と全体を括弧 */
#define SAMPLE_ALIGN_UP(x, a) (((x) + ((a) - 1U)) & ~((a) - 1U))

/* 望ましい: 位置注入。引数は括弧 */
#define sample_log_write(context, message) \
    sample_log_write_at((context), (message), __FILE__, __LINE__)

/* 望ましくない: 引数に括弧がなく、呼び出し側の演算子と結合が変わる */
#define SAMPLE_SQR(x) x * x
```

次は括弧を付けません (付けると意味が壊れる、または付けられない)。

| 例外 | 理由 |
|---|---|
| `#` / `##` の被演算子 | トークン連結・文字列化の対象 |
| 型名トークン | `(type)expr` の `type` など |
| 文字列リテラル連結に使う書式引数 | `"[%s:%d] " fmt` の `fmt`。括弧を付けると連結できません。 |
| 構造体初期化子マクロの一部トークン | 中括弧初期化子の構文上の断片 |

```c
/* 例外: 文字列リテラル連結に使う fmt は括弧不可 */
#define sample_log_writef(context, fmt, ...) \
    sample_log_writef_at((context), __FILE__, __LINE__, fmt, ##__VA_ARGS__)
```

##### 複文マクロ

複数の文からなるマクロは、`do { ... } while (0)` で包みます。  
単一の文として使うマクロでも、複文へ拡張する見込みがある場合は同様に包んで構いません。

```c
/* 望ましい */
#define SAMPLE_CLOSE_AND_NULL(p) \
    do                           \
    {                            \
        sample_close((p));       \
        (p) = NULL;              \
    } while (0)

/* 望ましくない: if / else と結合が壊れる */
#define SAMPLE_CLOSE_AND_NULL(p) \
    sample_close((p));           \
    (p) = NULL
```

戻り値が必要な式マクロには `do { ... } while (0)` を使いません。  
式のまま括弧で囲みます。

複文が必要になった時点で、まず `static inline` 関数または通常の関数へ移せないかを検討します。  
関数化できるならマクロにしないでください。

> [!WARNING]
> 複文を中括弧だけ、または連続する文だけで書くと、`if (cond) MACRO(x); else ...` の形でコンパイル エラーや意図しない制御になります。  
> `do { ... } while (0)` は、末尾のセミコロンと制御構文の両方に整合させるための慣用句です。

##### 仮引数の評価回数

各仮引数は、マクロ本体で **高々 1 回** 評価します。  
複数回の評価が必要なロジックは、`static inline` 関数または通常の関数へ移します。

```c
/* 望ましくない: x が 2 回評価される */
#define SAMPLE_TWICE(x) ((x) + (x))

/* 望ましい: 関数なら引数は 1 回だけ評価される */
static inline int sample_twice_int(int x)
{
    return x + x;
}
```

既存のマクロが仮引数を複数回評価している場合は、変更機会に合わせて関数へ移すか、Doxygen に「副作用のある実引数を渡してはなりません」と明記します。  
新規マクロでは複数回評価を採用しません。

##### 処理系拡張

- GNU 文式 (`({ ... })`) は使用しません。MSVC で使えません。
- 空の可変引数に対応するための `##__VA_ARGS__` (GNU / MSVC 拡張) は、可変引数 API の糖衣に限り用いて構いません。
- コンマ演算子で複数の副作用を並べた文マクロは避け、`do { ... } while (0)` または関数へ移します。

> [!NOTE]
> ヘッダーに置く `static inline` の定義規則は、[inline 関数](#inline-関数) を参照してください。
> 本節は、マクロより関数を優先する判断だけを扱います。

#### 検証

括弧の過不足や評価回数は、字句だけでは誤検知が多いため、機械的な必須チェックは定めません。  
レビューでは次を確認します。

- 新設マクロが [マクロにしてよい用途](#マクロにしてよい用途) のいずれかに当てはまるか
- 式として使う仮引数に括弧があるか。式マクロの本体全体に括弧があるか。例外に該当する出現だけが括弧なしか
- 複文マクロが `do { ... } while (0)` になっているか
- 各仮引数の評価が高々 1 回か
- GNU 文式を使っていないか

```bash
# 関数形式マクロの定義箇所の洗い出し (第一者の目視レビュー用。OSS は除外)
grep -rnE '^[[:space:]]*#define[[:space:]]+[A-Za-z_][A-Za-z0-9_]*\(' \
  app --include=*.h --include=*.c \
  | grep -vE 'app/(lua|sqlite)/prod/|app/cjson/prod/(include|libsrc/cjson)/cJSON(\.|_Utils\.)|/obj/'
```

### ヘッダー ファイル名の _internal

ヘッダー **ファイル名** の `_internal` サフィックスは、次の条件のときだけ付けます。

- `prod/include_internal/` 配下のヘッダーである
- かつ、同名の公開ヘッダーが `prod/include/` に存在する (例: 公開 `console.h` に対する `console_internal.h`)
- 対応する公開ヘッダーが存在しない場合はサフィックスなし (例: `path_format.h`、`trace_common.h`)
- 実装と同じディレクトリに置くモジュール私有ヘッダーには付けません (例: `hashtable/hashtable.h`)

> [!IMPORTANT]
> この規則はシンボル名の `<lib>_internal_` とは付与条件が異なります。

| 対象 | 付与条件 | 例 |
|---|---|---|
| ヘッダー ファイル名 | `include_internal/` にあり、同名の公開ヘッダーがあるときだけ | `console_internal.h` / `path_format.h` |
| ライブラリ内共有の関数・型 | `include_internal/` で宣言するすべて | `sample_internal_console_flush` / `sample_internal_registry` |
| ライブラリ内共有の外部リンケージ変数 | `include_internal/` で `extern` するすべて | `g_sample_internal_default_registry` |
| モジュール私有ヘッダーとその宣言 | 付けない | `hashtable.h` / `hashtable_entry_status` |

ファイル名に `_internal` が無くても、`include_internal/` 配下で宣言する関数・型・外部リンケージ変数には公開境界のマーカーを付けます。

### モジュール私有ヘッダー

1 つのディレクトリに置いた複数の `.c` だけが共有する宣言は、実装と同じディレクトリへ置くモジュール私有ヘッダーにまとめます。

`prod/include_internal/` は「ライブラリ内のどのモジュールからでも参照してよい」ことを表す配置です。  
参照範囲が 1 ディレクトリで閉じる宣言をここへ置くと、公開範囲を実態より広く見せます。  
実装と同じディレクトリへ置けば、参照範囲がディレクトリの境界と一致します。

#### 配置とファイル名

- 実装ファイルと同じディレクトリへ置きます。`prod/include/` と `prod/include_internal/` には置きません。
- ディレクトリの中心となる私有ヘッダーは、ディレクトリ名と同じ名前にします (例: `hashtable/hashtable.h`、`trace-cli/trace-cli.h`)。
- 1 つのディレクトリに複数置く場合は、モジュール接頭辞を付けて役割を表します (例: `bench-io/bench_case.h`、`bench-io/bench_timer.h`)。
- `_internal` サフィックスは付けません。これは `include_internal/` 配下のヘッダーのための規則です。

公開ヘッダーと同名になっても構いません。  
私有ヘッダーは引用符形式で取り込み、引用符形式は includer 自身のディレクトリを最初に探すため、同一ディレクトリの私有ヘッダーへ解決されます。  
公開ヘッダーは山かっこ形式のままとし、インクルード パスの直下に同名ファイルを置かないことで、両者が取り違えられないようにします。

```c
/* prod/libsrc/cplat/hashtable/hashtable_arena.c */
#include "hashtable.h"                  /* 同一ディレクトリの私有ヘッダー */
```

```c
/* 私有ヘッダー自身は、公開ヘッダーを山かっこ形式で取り込む */
/* prod/libsrc/cplat/hashtable/hashtable.h */
#include <cplat/hashtable/hashtable.h>  /* 公開ヘッダー */
```

> [!IMPORTANT]
> 私有ヘッダーを持つディレクトリのソースを、テストへシンボリック リンクやコピーで引き込む場合は、テスト側の `makepart.mk` で `INCDIR` へ元ディレクトリを追加します。  
> 引用符形式の探索起点が引き込み先のディレクトリになり、元ディレクトリ基準では解決されないためです。

#### インクルード ガード

モジュール名を全大文字にしたものへ `_PRIVATE_H` を付けます。  
ライブラリや app の接頭辞は付けません。私有ヘッダーは 1 ディレクトリ内でしか取り込まれず、リンカー名前空間にも現れないためです。

| ヘッダー | インクルード ガード |
|---|---|
| `libsrc/cplat/hashtable/hashtable.h` | `HASHTABLE_PRIVATE_H` |
| `src/cmd/trace-cli/trace-cli.h` | `TRACE_CLI_PRIVATE_H` |
| `src/cmd/bench-io/bench_case.h` | `BENCH_CASE_PRIVATE_H` |

モジュール名にハイフンやキャメル ケースが含まれる場合は、アンダースコア区切りの全大文字へ直します (例: `tcpServer.h` は `TCP_SERVER_PRIVATE_H`)。

`_PRIVATE_H` は、同名の公開ヘッダーとガード名が衝突しないことと、私有ヘッダーであることの両方を表します。  
公開ヘッダーのガードは配置パス全体に由来するため (例: `CPLAT_HASHTABLE_HASHTABLE_H`)、同名でも衝突しません。

#### 宣言するシンボル

- 関数・型にはモジュール名を接頭辞として付けます (`hashtable_entry_status`、`trace_cli_process_line`)。
- 公開境界を越えないため、`_internal_` マーカーは付けません。
- 外部リンケージ変数は `g_<module>_` とし、必要最低限に厳選します。
- マクロと列挙定数は、`.c` の内部だけで使うマクロと同じ扱いとし、接頭辞を省略できます。

ほかの `.c` から呼ばない関数は、`static` のまま実装ファイルに残します。  
私有ヘッダーへ宣言するのは、別の `.c` から呼ぶ関数だけです。

#### NULL 検査

モジュール私有ヘッダーで宣言する関数は、NULL を検査しません。  
呼び出し元がすべて同一ディレクトリ内にあり、前提条件を呼び出し側で保証できるためです。  
`static` 関数と同じ扱いとし、前提条件は Doxygen コメントへ記載します。

#### static inline

短い補助関数や領域アクセサーは、`static inline` として私有ヘッダーで定義できます。  
適用範囲の考え方は [inline 関数](#inline-関数) と同じです。

### 既存コードへの適用と例外条件

- 新規コードでは、最初から本章の規則に従います。
- 既存ファイルを変更する際は、変更ファイル内の同種のシンボルすべてに本章を適用します。
- ライブラリ内共有の関数・型が `<lib>_` のまま、または共有変数が `g_<lib>_` のままになっている既存コードは、本規則を目的とした全面改名は求めません。変更対象ファイルに触れる機会に合わせて `<lib>_internal_` / `g_<lib>_internal_` へ移行します。
- 大規模な改名リファクタリングを行う際は、**1 commit = 1 カテゴリ (または 1 ヘッダー)** 単位で進めます。改名以外の変更 (戻り値の意味変更、引数の追加、ロジックの修正) を同じ commit に含めません。
- ABI 凍結を宣言しているシンボルについては、各 app のドキュメントの凍結リストが本章に優先します。凍結の範囲と根拠は当該ドキュメントに明記します。

改名を伴う機械的な置換では、次の手順で安全性を確認します。

1. 置換前の旧名の出現件数を記録します。
2. 新名が未使用であること (衝突がないこと) を確認します。
3. 識別子境界を指定して置換します。行番号を指定した置換は行わない
4. 置換後に旧名が 0 件であること、新名の件数が 1 で記録した件数と一致することを確認します。
5. 識別子以外のトークンが変化していないことを差分で確認します。

### 検証

```bash
# 外部 OSS とビルド生成物を除外する共通条件
EXCL='app/(lua|sqlite)/prod/|app/cjson/prod/(include|libsrc/cjson)/cJSON(\.|_Utils\.)|/obj/|doxybook2_|/prod/(lib|cbin)/'

# _t サフィックスの検出 (例外として明記した alias のみが残る)
grep -rnE '(^|[[:space:]])typedef[[:space:]].*[A-Za-z0-9_]+_t[[:space:]]*(\)|;)|^\}[[:space:]]*[a-z0-9_]+_t[[:space:]]*;' \
  app --include=*.h --include=*.c | grep -vE "$EXCL"

# アンダースコア始まりのファイル スコープ識別子 (0 件であること)
grep -rnE '(^|[^A-Za-z0-9_"])_[a-z][A-Za-z0-9_]*[[:space:]]*\(' \
  app --include=*.h --include=*.c | grep -vE "$EXCL"

# ユーザー定義らしき識別子中の連続 __ (処理系マクロの参照は目視で除外。レビュー併用)
grep -rnE '\b[A-Za-z0-9_]*__[A-Za-z0-9_]*\b' app --include=*.h --include=*.c --include=*.cc --include=*.cpp \
  | grep -vE "$EXCL" | grep -vE '__FILE__|__LINE__|__func__|__FUNCTION__|__PRETTY_FUNCTION__|__declspec|__attribute__|__stdcall|__cdecl|__fastcall|__restrict|__inline|__forceinline|__asm|__volatile|__GNUC__|__clang__|__cplusplus|__x86_64__|__i386__|__linux__|__APPLE__|__WIN32|__WIN64|_WIN32|_MSC_VER|__COUNTER__|__DATE__|__TIME__|__TIMESTAMP__'

# 匿名 typedef (0 件であること)
grep -rnE 'typedef[[:space:]]+(enum|struct|union)[[:space:]]*(\{|$)' \
  app --include=*.h --include=*.c | grep -vE "$EXCL"

# 関数ポインター typedef のサフィックス (0 件であること)
grep -rnE 'typedef[[:space:]].*\(\*[A-Za-z_][A-Za-z0-9_]*\)' \
  app --include=*.h --include=*.c | grep -vE "$EXCL" | grep -vE '\*[a-z0-9_]+_fn\)'

# static 関数のライブラリ接頭辞 (0 件であること。<lib> は対象ライブラリの接頭辞)
grep -rnE '^static[[:space:]][^(;]*\b<lib>_[a-z0-9_]*[[:space:]]*\(' \
  app --include=*.c | grep -vE "$EXCL"

# static 関数への internal マーカー (0 件であること。<lib> は対象ライブラリの接頭辞)
grep -rnE '^static[[:space:]][^(;]*\b<lib>_internal_[a-z0-9_]*[[:space:]]*\(' \
  app --include=*.c | grep -vE "$EXCL"

# 公開ヘッダーに internal マーカー付き識別子が出ていないこと (0 件であること。<lib> は対象ライブラリの接頭辞)
grep -rnE '\b<lib>_internal_' app/*/prod/include --include=*.h | grep -vE "$EXCL"

# 公開ヘッダーの extern 変数 (件数は最小であること。新規追加時は必要性をレビューする)
grep -rnE '^[[:space:]]*extern[[:space:]]' app/*/prod/include --include=*.h \
  | grep -vE "$EXCL" | grep -vE '\('

# モジュール私有ヘッダーのインクルード ガード (すべて _PRIVATE_H で終わること)
# prod 配下で include/ と include_internal/ のどちらにも属さず、生成物でもないヘッダーを対象とする
PRIV="find app/*/prod -name *.h -not -path */prod/include/* -not -path */prod/include_internal/* -not -path */gen/*"
$PRIV | grep -vE "$EXCL" | while read -r h; do
    printf '%s\t%s\n' "$h" "$(grep -m1 '^#ifndef' "$h")"
  done | grep -v '_PRIVATE_H$'

# モジュール私有ヘッダーに internal マーカーが出ていないこと (0 件であること。<lib> は対象ライブラリの接頭辞)
$PRIV | grep -vE "$EXCL" | xargs grep -nE '\b<lib>_internal_'
```

> [!IMPORTANT]
> 次の 2 つは機械検証だけでは判定できません。目視とコード レビューを併用してください。
>
> - `include_internal/` の関数・型・変数がすべて公開境界マーカー付きであること。公開型を引数に取る内部関数や、ファイル スコープ以外の識別子を偽陽性として拾いやすい
> - `static` 変数の `s_` 接頭辞。関数定義を偽陽性として拾うため、`^static` で始まる行を抽出し、宣言子が関数ではないものを目視で選別します。

## ゼロ初期化

### 基本ルール

ローカル変数を宣言時に全体ゼロ クリアする場合は、集成体初期化子を使用し、`memset(&v, 0, sizeof(v))` は使用しません。  
C の構造体、共用体、配列には `= {0}` を使用します。  
C++ の構造体、クラス、共用体には `= {}` を使用します。  
C++ の配列には `= {0}` と `= {}` のどちらも使用できます。

宣言後に、すでに使用中の変数を途中でゼロ クリアし直す場合は、この規則の対象外とし、引き続き `memset` を使用します。  
集成体初期化子は宣言と一体であり、宣言済みの変数への再代入や、ポインター引数が指す既存オブジェクトの途中クリアには使えないためです。

```c
/* 望ましい: 宣言と同時にゼロ初期化する */
sample_options options = {0};
unsigned char digest[32] = {0};
```

```cpp
/* 望ましい: C++ の構造体を宣言と同時に値初期化する */
sample_options options = {};
```

```c
/* 望ましくない: 宣言後に memset でゼロ初期化する */
sample_options options;
memset(&options, 0, sizeof(options));

unsigned char digest[32];
memset(digest, 0, sizeof(digest));
```

```c
/* この規則の対象外: 引数が指す既存オブジェクトを関数内で途中クリアする */
void sample_worker_reset(sample_worker *worker)
{
    memset(worker, 0, sizeof(*worker));
}
```

> [!NOTE]
> C は C++ と異なり、コンストラクターによるメンバーごとの初期化義務がありません。  
> 集成体初期化子であれば、メンバーごとの代入を書かずに宣言と同時に全体をゼロ クリアできます。  
> C の `= {0}` は、先頭の要素へ `0` を明示し、残りの要素も暗黙にゼロ初期化する意図を表します。  
> 空の初期化子 `= {}` は本リポジトリが使用する C17 の標準構文ではないため、C++ と同じ記法へ統一しません。  
> `memset` は実行時に呼び出す関数である一方、集成体初期化子は宣言と初期化が一体であり、対象の型とサイズの対応をコンパイル時に検査できます。  
> また C17 6.7.9 の規定により、`= {0}` は明示していないメンバーや要素だけでなく、パディングも含めてゼロ初期化されます。
>
> C++ の `= {}` は、すべてのメンバーを型の規則に従って値初期化する目的で使用します。  
> `= {0}` は先頭メンバーだけを明示した部分的な初期化子として扱われ、GCC の `-Wmissing-field-initializers` が省略したメンバーを警告するため使用しません。

## 構造体のパディングと予約フィールド

### 基本ルール

`-Wpadded` が指摘する暗黙パディングは、`#pragma GCC diagnostic ignored "-Wpadded"` で抑止せず、構造体定義を見直して解消します。

ただし、**大きいアラインメント順への積極的な並び替えよりも、メンバーの意味上のまとまりと可読性を優先** します。  
意味の近いメンバーを保ったままでは暗黙パディングを避けられない場合は、明示的なパディング メンバーを追加してください。

> [!NOTE]
> 暗黙パディングを残さないのは、構造体のレイアウトを定義から読み取れる状態に保つためです。  
> 明示的なパディング メンバーがあれば、メンバーを追加・変更したときにレイアウトへ与える影響が定義上で見えます。

> [!NOTE]
> `-Wpadded` は業界一般では特殊用途の警告とされ、通常は有効化されません。  
> 本規範が全構造体を対象とするのは、**どの構造体もいずれ永続化されうる** という観点に立つためです。  
> 内部構造体として作られたものが、後からログ出力、設定ファイル、共有メモリ、プロセス間通信へ流用されることは珍しくありません。  
> その時点でレイアウトが暗黙パディング任せになっていると、移行の前提を確認する作業が発生します。  
> 最初からパディングを明示しておけば、永続化の対象になった時点で追加の調査が要りません。  
> 本リポジトリでは `app/makepart.mk` と `framework/testfw/makepart.mk` の `GCC_WARN_BASE` で有効化しています。

### 明示的なパディング メンバー

明示的なパディングを追加するときは、次のルールに従います。

- メンバー名は `pad`、複数必要な場合は `pad1`、`pad2`、... とします。
- コメントで明示的アラインメントのためのパディングであることを簡潔に明示します。
- 幅は不足分だけに留める
- [ゼロ初期化](#ゼロ初期化) に従い、構造体全体をゼロ初期化してから使用します。

```c
typedef struct sample_record
{
    int mode;
    unsigned int pad; /* 明示的アラインメント */
    intptr_t native_handle;
} sample_record;
```

```c
/* 望ましい: ゼロ初期化してからメンバーを設定する */
sample_record record = {0};

record.mode = SAMPLE_MODE_READ;
record.native_handle = handle;
```

> [!WARNING]
> パディング メンバーを初期化せずに構造体をファイル、ソケット、共有メモリへ書き出すと、未初期化のスタック内容が外部へ出ます。  
> パディングは値に意味を持たない領域ですが、**書き出されるバイトとしては意味を持ちます**。  
> 構造体全体を `= {0}` などでゼロ初期化してから値を設定してください。

> [!NOTE]
> `pad` メンバーには `static_assert` によるサイズや位置の表明を求めません。  
> `pad` はアラインメントを揃えるための穴埋めであり、レイアウトが外部との契約になっているとは限らないためです。  
> レイアウトを固定したい構造体では、[予約フィールド](#予約フィールド) の規則に従って `static_assert` を置いてください。

### プラットフォーム依存の条件付きパディング

サポート対象外の 32 ビット環境だけを考慮した条件分岐は追加しません。  
パディングはサポート対象の ABI に基づいて定義します。  
複数アーキテクチャーを正式にサポートする構造体に限り、`ARCH_*` による条件付きパディングを使用します。

プラットフォームやアーキテクチャーごとにパディング有無を切り替える場合は、`#if defined(ARCH_X64)` や `#if defined(PLATFORM_WINDOWS)` のように、プラットフォーム抽象化ヘッダーの共通マクロを使います。  
`__x86_64__` や `_WIN32` を利用側で直接判定しません。

共通マクロの利用規則は、利用するライブラリのプラットフォーム抽象化ガイドラインを参照してください。

> [!NOTE]
> 処理系マクロを利用側で直接判定すると、対応プラットフォームを増減するときの変更箇所が分散します。  
> 判定を抽象化ヘッダーへ集約することで、対応範囲の変更が 1 箇所で済みます。

### 予約フィールド

将来のメンバー追加に備えて領域を確保する場合は、構造体の末尾に予約フィールドを置きます。

#### 適用対象

予約フィールドを置くのは、**レイアウトが外部との契約になっている構造体** に限ります。

| 対象 | 予約フィールド |
|---|---|
| ファイル形式、通信形式、共有メモリのレコード | 置く |
| ABI 凍結を宣言した公開 API の構造体 | 置く |
| ライブラリ内共有の構造体、`.c` 内に閉じた構造体 | 置かない |

内部構造体は再コンパイルでレイアウトを変更できるため、予約フィールドは領域を浪費するだけです。  
必要になった時点でメンバーを追加してください。

#### 規則

- メンバー名は `reserved`、複数必要な場合は `reserved1`、`reserved2`、... とします。
- 要素型は `uint8_t` とし、**要素数を直値のリテラルで記載します**。マクロ定数、`sizeof` 式、`enum` 定数は使用しません。
- 構造体の末尾に配置します。
- **予約領域の先頭オフセットを 8 の倍数にします**。満たせない場合は直前に明示的なパディングを配置します。
- **構造体全体のサイズが 8 の倍数になるように要素数を選ぶ**。結果として要素数も 8 の倍数になります。
- 生成側は予約領域をゼロで埋めます。解釈側は予約領域を読みません。
- `static_assert` で `sizeof` と、後続メンバーがある場合は `offsetof` を固定します。

```c
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef struct sample_record
{
    uint32_t kind;
    uint32_t flags;
    uint64_t timestamp;
    uint8_t reserved[16]; /* 予約領域。生成側はゼロで埋める。 */
} sample_record;

static_assert(sizeof(sample_record) == 32, "sample_record のサイズは 32 バイト固定");
static_assert(offsetof(sample_record, reserved) == 16, "予約領域は 8 バイト境界から始まる");
```

要素数の書き方は、次のいずれも採りません。

```c
/* 望ましくない: マクロ定数。宣言を読んでもバイト数が分からない */
#define SAMPLE_RECORD_RESERVED_SIZE 16
uint8_t reserved[SAMPLE_RECORD_RESERVED_SIZE];

/* 望ましくない: sizeof 式による導出。既存メンバーを変更すると予約領域まで動く */
uint8_t reserved[32 - sizeof(uint32_t) * 2 - sizeof(uint64_t)];

/* 望ましくない: enum 定数 */
enum { sample_record_reserved_size = 16 };
uint8_t reserved[sample_record_reserved_size];
```

> [!IMPORTANT]
> ここでの「要素数を明示する」は、**`16` のような直値のリテラルを書く** ことを指します。  
> 名前付きの定数を定義してそれを書くことではありません。  
> [マクロ](#マクロ) の命名規則がマクロ名にライブラリ接頭辞を求めていることとは、対象が異なります。

> [!NOTE]
> 直値のリテラルとするのは、予約領域のバイト数を構造体の宣言だけで確認できるようにするためです。  
> マクロ定数や `sizeof` 式にすると、レイアウトを読むために定義元をたどる必要が生じ、`static_assert` によるサイズの表明と突き合わせられなくなります。  
> 特に `sizeof` 式による導出は、既存メンバーの型を変更したときに予約領域の大きさが自動で追従してしまい、**レイアウトが変わったことに気付けなくなります**。  
> 実測では、`timestamp` を `uint64_t` から `uint32_t` へ変更すると予約領域が 16 バイトから 20 バイトへ自動で伸び、`sizeof` は 32 のままとなるため、サイズを表明する `static_assert` も通過しました。  
> 予約フィールドは意図的に固定したい対象であるため、この自動追従は害になります。  
> 予約領域を消費するときも、リテラルを書き換えるだけで差分にバイト数の変化が現れます。

#### パディングとの区別

`pad` と `reserved` は、名前で用途を区別します。

| メンバー | 用途 | 値の扱い |
|---|---|---|
| `pad` / `pad1` / `pad2` | 暗黙パディングの明示化。アラインメントを揃えるための穴埋め | 意味を持たない。将来も意味を与えない |
| `reserved` / `reserved1` | 将来のメンバー追加のための領域 | ゼロで埋める。将来メンバーとして意味を与える |

既存メンバーのアラインメントを揃える目的で `reserved` を使いません。  
将来の拡張に備える目的で `pad` を使いません。

#### 予約領域の消費

予約領域を使ってメンバーを追加するときは、追加するメンバーのサイズだけ `reserved` の要素数のリテラルを減らし、**構造体全体のサイズと既存メンバーのオフセットを変えません**。

```c
/* 変更前 */
typedef struct sample_record
{
    uint32_t kind;
    uint32_t flags;
    uint64_t timestamp;
    uint8_t reserved[16];
} sample_record;

/* 変更後: 予約領域の先頭 8 バイトを sequence へ転用する */
typedef struct sample_record
{
    uint32_t kind;
    uint32_t flags;
    uint64_t timestamp;
    uint64_t sequence;
    uint8_t reserved[8];
} sample_record;

static_assert(sizeof(sample_record) == 32, "サイズは変更前後で不変");
```

消費後も `static_assert` によるサイズの表明を残し、レイアウトが変わっていないことをビルド時に確認します。

> [!WARNING]
> 要素数を書かない `uint8_t reserved[];` は、予約領域ではなく **柔軟配列メンバー** になります。  
> `sizeof` に含まれないため領域が確保されず、レコードを読み書きした時点でレイアウトが崩れます。  
> GCC 8.5.0 の実測では、`struct { int32_t x; uint8_t reserved[]; }` の `sizeof` は 4 です。

> [!WARNING]
> 予約領域の先頭オフセットが、後から切り出すメンバーのアラインメントを満たしていないと、サイズを保ったままの転用ができません。  
> GCC 8.5.0 の実測では、`struct { int32_t x; uint8_t reserved[12]; }` (サイズ 16、予約はオフセット 4) から `int64_t` を切り出すと、8 バイト境界へ寄せるためのパディングが入り、サイズが 24 へ増えます。

> [!WARNING]
> 構造体全体のサイズが 8 の倍数にならない要素数を選ぶと、末尾に暗黙パディングが入り `-Wpadded` が発生します。  
> GCC 8.5.0 の実測では、`struct { int64_t x; uint8_t reserved[4]; }` は `warning: padding struct size to alignment boundary` となります。  
> この場合は要素数を 8 へ増やします。

> [!WARNING]
> 予約領域をゼロで埋めずにファイル、ソケット、共有メモリへ書き出すと、未初期化のスタック内容が外部へ出ます。  
> 構造体全体を `= {0}` などでゼロ初期化してから値を設定してください。

> [!NOTE]
> オフセットとサイズをいずれも 8 の倍数に固定するのは、**将来どの型を切り出すかを予約時点で判断しなくて済むようにする** ためです。  
> サポート対象の LP64 / LLP64 では基本型の最大アラインメントが 8 であるため、8 の倍数にしておけば、後から `uint64_t` やポインターを切り出してもオフセットとサイズが変わりません。  
> 4 の倍数で足りる構造体もありますが、判断の余地を残すと、切り出す型が決まった時点で「そのときは 4 で足りると思っていた」という不整合が起こります。  
> 一律 8 とすることで、`-Wpadded` の回避条件も同時に満たせます (8 の倍数は 4 の倍数でもあるため、アラインメント 4 の構造体でも末尾パディングは生じません)。
>
> 16 バイト以上のアラインメントを要求する型 (SIMD 型など) を切り出す予定がある場合は、本規則の対象外です。当該構造体の互換性方針として別途定めてください。

> [!NOTE]
> 要素型を `uint8_t` とするのは、予約領域が「意味を持たないバイトの並び」であり、[整数型の選択](#整数型の選択) の判定 2 (幅に意味がある値) に該当するためです。  
> 符号付きの `int8_t` は、バイト列に符号の意味があるかのように読めるため使いません。  
> バイト数で要素数を書けるため、`static_assert` によるサイズの表明とも対応が取りやすくなります。

> [!NOTE]
> 予約フィールドは、レイアウトを固定したまま拡張余地を残す手法です。  
> 構造体の先頭にサイズや版番号を置き、受け手がそれを見て解釈を切り替える方式もあります (Win32 の `cbSize` など)。  
> どちらを採るかは各 app の互換性方針に従ってください。本節は前者を採る場合の書き方を定めます。

## 整数型の選択

### 基本ルール

整数値を表す型は、**その値にとって幅が意味を持つかどうか** で選択します。  
次の判定を上から順に評価し、最初に一致した行の型を採用します。

| 判定順 | 条件 | 選択する型 |
|---|---|---|
| 1 | 値の意味に対応する型がある | その型 ([値の意味に対応する型](#値の意味に対応する型) を参照) |
| 2 | 幅そのものに意味がある | `<stdint.h>` の固定幅型 ([幅に意味がある値](#幅に意味がある値) を参照) |
| 3 | 上記以外 (幅に意味がない算術の器) | `int` / `unsigned int`。64 bit の範囲が必要な場合は `int64_t` / `uint64_t` |

判定 2 の「幅そのものに意味がある」とは、幅を変えると値の意味や互換性が壊れることを指します。  
外部レイアウト (ファイル形式、通信形式、共有メモリ)、規格が幅を定める計算値、OS API 境界が幅を要請する箇所が該当します。

判定 3 に該当する箇所では、固定幅型 (`int8_t` / `uint8_t` / `int16_t` / `uint16_t` / `int32_t` / `uint32_t`) を使用しません。  
`int` で表せない範囲を扱う場合に限り `int64_t` / `uint64_t` を使用します。

判定 2 に該当し、かつ 8bit または 16bit の値をレイアウトの一部として宣言する場合は、`signed char` / `unsigned char` / `short` / `unsigned short` ではなく固定幅型を使用します。  
バイト列そのものを扱うバッファーの要素型は `uint8_t` とします。

`char` は処理系で符号付き / 符号なしが分かれます。  
整数値として扱う場合は `signed char` / `unsigned char` を明示してください (文字列の要素として扱う場合は `char` を用います)。

`long` をクロスプラットフォームの 64 bit 整数型として使用しません。

> [!WARNING]
> LP64 の Linux x86_64 では `long` が 64bit ですが、LLP64 の Windows x64 では `long` が 32bit です。  
> `long` を 64 bit のつもりで使うと、Windows 側でのみ値が切り捨てられ、片方のプラットフォームでしか再現しない不具合になります。

> [!NOTE]
> 現代的な Linux (GCC)・Windows (MSVC) 環境では、`signed char` / `unsigned char` が 8bit、`short` / `unsigned short` が 16bit、`int` / `unsigned int` が 32bit となります。  
> LP64 (Linux x86_64 など) でも int は 32bit、long が 64bit です。  
> LLP64 (Windows x64) でも int は 32bit、long は 32bit、long long が 64bit です。

> [!NOTE]
> 判定 3 で `int` を既定とするのは、幅に意味がない値に固定幅型を使うと、幅が契約の一部であるかのように読めるためです。  
> 加えて、`signed char` や `short` の演算は整数昇格によって `int` で行われるため、小さな整数の器として宣言しても演算上の利得はありません。  
> 幅に意味がない値には `int` を使い、固定幅型の出現箇所を「幅が契約である箇所」に限定することで、コードを読むだけで契約の所在が分かるようにします。

> [!NOTE]
> MISRA C:2012 Dir 4.6 は、幅と符号性を示す typedef を基本の数値型の代わりに使うことを求めています。  
> SEI CERT C INT00-C は処理系のデータ モデルを理解して仮定を静的表明で裏付けること、INT01-C はオブジェクトのサイズを表す値に `size_t` を使うことを求めています。  
> 本節の判定 1 が INT01-C に、判定 2 が Dir 4.6 の対象範囲に対応します。  
> 本節はこれらと同じく「幅に意味がある値は固定幅型」という立場を採り、そのうえで幅に意味がない値の既定を `int` と定めるものです。

### 値の意味に対応する型

値に対応する標準型、OS API 型、または対象ワークスペースの共通型がある場合は、ビット幅だけで型を決めず、値の意味に対応する型を優先します。  
`size_t`、`ptrdiff_t`、`ssize_t`、`off_t`、`time_t` は、特定の意味と演算規則を持つ型であり、単なる 32 bit または 64 bit の整数型として使用しません。

| 値の意味 | 選択する型 | 使用条件 |
|---|---|---|
| オブジェクト ポインターとの往復変換、アドレス値 | `uintptr_t` | ポインター幅へ追従させる。 |
| 負の無効値を持つ OS ネイティブ ハンドル | `intptr_t` | OS API がポインター幅の整数と負のセンチネル値を要求する場合に限定します。 |
| オブジェクトのバイト サイズ、要素数、`sizeof` の結果、それらと同じ範囲の配列添字 | `size_t` | オブジェクトに関する、負値を取らない値に使用します。 |
| 同一配列内の 2 つのポインターの差 | `ptrdiff_t` | 異なる配列を指すポインター同士の減算には使用しません。 |
| POSIX I/O API が返す処理済みバイト数とエラー値 | `ssize_t` | Linux 実装内の POSIX API 境界に限定し、Windows 側に露出させない |
| POSIX のファイル位置、ファイル サイズ、ファイル オフセット | `off_t` | Linux 実装内の POSIX API 境界に限定、Windows 側に露出させない |
| C / POSIX 時刻 API が扱う時刻の秒部 | `time_t` | `time()` の戻り値や `struct timespec::tv_sec` などに使用し、任意の期間には使用しません。 |
| クロスプラットフォームで受け渡す絶対時刻、単調時刻 | 各 app が定める標準時刻型 | 標準時刻型の定めがある場合はそれに従う (各 app の特化事項を参照) |
| 正規化されたナノ秒部、負になり得る時間差 | `int64_t` | 標準時刻型のナノ秒部または符号付きの差分値に使用します。 |
| 検査済みの非負ナノ秒期間、タイムアウト | `uint64_t` | 負値を受け付けないことを API 仕様で定め、外部入力の負値検査が完了した後に使用します。 |
| クロスプラットフォーム API のファイル位置、ファイル オフセット、I/O 結果 | `int64_t` | Linux の `off_t` / `ssize_t` と Windows の 64 bit API を共通化する場合に使用します。 |
| 文字列から入力し、負値や範囲外を検査するファイル オフセット | `int64_t` | 符号付き整数として解析し、構文、負値、上限を検査してから目的の型へ変換します。 |

64 bit のファイル位置を扱う共通 API には `int64_t` を使用し、Windows 実装では `_fseeki64` / `_ftelli64` / `_lseeki64` との境界で変換し、`off_t` を用いません。

`ssize_t`、`off_t`、`time_t` の幅は処理系に依存します。  
これらの型をファイル形式、通信形式、共有メモリなど、バイナリ レイアウトを固定するデータには使用しません。

> [!WARNING]
> MSVC の UCRT では `off_t` が `long` の別名であるため 32bit となります。  
> `off_t` を 64 bit のファイル位置として使うと、Windows 側でのみ 2GB を超えるファイルを扱えなくなります。

### 文字列入力から意味付き型への変換

コマンド ライン引数などの外部文字列を `size_t` やファイル位置へ変換する場合は、最初に `int64_t` として解析します。  
解析時には、文字列全体が整数として解釈されたこと、変換元の値が `int64_t` の範囲内であること、用途上の下限と上限を満たすことを確認します。

負値を許容しない値は、負値の検査後に変換先の最大値を確認してから `size_t` または `uint64_t` へ変換します。  
負値の検査より前に `SIZE_MAX` などと比較しません。  
クロスプラットフォーム API のファイル位置へ渡す値は、検査後も `int64_t` のまま保持し、Linux 実装内でのみ `off_t` へ変換します。

> [!WARNING]
> 符号付き型と符号なし型を直接比較すると、暗黙変換によって負値が大きな正値になります。  
> 負値の検査より先に上限比較を行うと、負の入力が上限検査を通過します。  
> 比較一般の規則は [整数演算の安全性](#整数演算の安全性) を参照してください。

### 幅に意味がある値

次の用途は [整数型の選択](#整数型の選択) の基本ルールの判定 2 に該当し、固定幅型 (`uint8_t` / `int8_t` / `uint16_t` / `int16_t` / `uint32_t` / `int32_t`) を使用します。

- バイト列入出力 (`uint8_t *` バッファー、ヘッダーや任意長データのポインター)
- ネットワーク バイト順序の値 (`htons` / `htonl` / `ntohs` / `ntohl` の周辺、固定長の通信フィールド)
- ワイヤ プロトコル / 通信パケットの構造体メンバー (`payload_len`、`flags`、`session_id`、`seq_num` 等の幅が仕様で決まっているフィールド)
- アルゴリズム規格上、幅が定義されている計算値 (CRC、暗号鍵長など)
- OS API 境界で固定幅が要請される箇所
    - Windows `DWORD` を経由する API (`Sleep`、`WriteFile`、`GetCurrentProcessId` 等)
    - POSIX `struct timespec::tv_nsec` (long) との境界キャスト
    - atomic 操作の state 変数 (例: `__atomic_compare_exchange_n` の引数)

上記のいずれかに該当することが型宣言から自明でない場合は、当該ヘッダー / コードに「なぜ固定幅が必要か」を簡潔に記載してください。

```c
/* Windows Sleep の DWORD 引数に渡すため uint32_t を維持する */
uint32_t timeout_dword = (uint32_t)timeout_ms;
Sleep(timeout_dword);
```

> [!IMPORTANT]
> 判定 2 に該当する箇所は、幅そのものが外部との契約です。  
> 幅を狭める / 広げる変更は、ファイル形式や通信形式の互換性を壊します。  
> 型を変更する場合は、当該ライブラリの互換性方針を確認してください。

### 真偽値の型

真偽値に `bool` (`<stdbool.h>`) を使ってよいかは、**その型が現れるヘッダーの位置** で決まります。

| 場所 | 真偽値の型 |
|---|---|
| 公開ヘッダー (`prod/include/`) の引数、戻り値、構造体メンバー | `int` の 0 / 非 0。`bool` を使わない |
| ライブラリ内共有ヘッダー (`prod/include_internal/`) | `bool` を使用できます。 |
| `.c` 内のローカル変数、`static` 関数 | `bool` を使用できます。 |

公開 API が真偽の答えを返す場合は、`int *xxx_out` の出力引数とします。  
詳細は [真偽値や状態を返す API の設計](#真偽値や状態を返す-api-の設計) を参照してください。

`bool` を使う箇所では `<stdbool.h>` をインクルードします。  
[前提とする言語標準](#前提とする言語標準) は C17 であるため、`bool` は言語キーワードではありません。

#### 境界での変換

`int` から `bool` へ変換するときは、0 との比較を明示します。

```c
/* 望ましい */
bool is_enabled = (flags != 0);

/* 望ましくない: flags が 0 / 1 へ潰れ、フラグ ワードとしての値を失う */
bool is_enabled = flags;
```

`bool` から `int` への変換は、代入するだけで 0 / 1 になります。

```c
/* ライブラリ内部の bool を、公開 API の出力引数へ渡す */
*equal_out = is_equal;
```

#### 禁止事項

- `memcpy`、`fread`、共用体、ポインター キャストで `bool` の記憶域へ書き込まない
- ファイル形式、通信形式、共有メモリなど、レイアウトを固定するデータに `bool` を置かない

> [!WARNING]
> `_Bool` は 0 か 1 しか保持しない前提でコンパイラが最適化します。  
> 記憶域へ 2 以上のバイトを書き込むと、`if (b)` と `b == true` が食い違う未定義動作になります。  
> 過去資産のコードが `memcpy` や `fread` で構造体ごと読み書きする場合に踏みやすく、再現条件をつかみにくい不具合になります。

> [!WARNING]
> C# の `bool` は P/Invoke で既定 4 バイト (Win32 `BOOL`) としてマーシャリングされます。  
> 公開 API が 1 バイトの `bool` を返すと、`[MarshalAs(UnmanagedType.I1)]` を明示しない限り値が壊れます。  
> **宣言を誤ってもコンパイルは通る** ため、実行時まで検出できません。

> [!NOTE]
> 公開ヘッダーから `bool` を除くのは、次の理由によります。
>
> - `sizeof(_Bool)` は実装定義であり、ABI として契約に書けない。「幅に意味がある値は固定幅型」という [基本ルール](#整数型の選択) の判定 2 と整合しません。
> - 上記 2 つの WARNING が示すとおり、レガシ資産との接続と .NET 相互運用のいずれでも、静かに壊れる経路が生まれる
> - 本リポジトリには Win32 の `BOOL` (`int`) がすでに存在します。ここへ `bool` を加えると、境界ごとに 3 種類から選ぶ判断が恒常的に発生します。
>
> 一方、これらの理由はいずれも値が翻訳単位の外へ出るときにだけ効きます。  
> ライブラリ内部では ABI もマーシャリングも関与せず、記憶域への書き込み経路も自分で管理できるため、`bool` の可読性の利得を採ります。

> [!IMPORTANT]
> `prod/include_internal/` であっても、レイアウトが外部との契約になっている構造体のメンバーには `bool` を使いません。  
> ファイル形式、通信形式、共有メモリのレコードが該当します。  
> 判断基準は [予約フィールド](#予約フィールド) の適用対象と同じです。

#### 命名

真偽値を `int` で表す箇所は、型から真偽と読み取れません。  
名前で示します。

- 変数と構造体メンバー: `is_` / `has_` を前置きする (`is_open`、`has_pending`)
- 出力引数: `_out` の直前を真偽を表す語にする (`equal_out`、`exists_out`、`has_match_out`)

## 整数演算の安全性

### 基本ルール

本章は、整数型を選んだあとの **演算・比較・変換** の安全性を定めます。  
型そのものの選び方は [整数型の選択](#整数型の選択) に従います。

外部文字列の解析と、動的メモリ確保における要素数とサイズの乗算は、すでに別章と cplat の API が検査を担っています。  
本章が対象とするのは、それらに当てはまらない **通常の内部演算と型変換** です。

| 論点 | 内容 | 主な担保手段 |
|---|---|---|
| 符号混在比較 | `unsigned` と `int` の比較で負値が大きな正値へ変わる | `-Wsign-compare` (警告) と本節の規則 |
| 暗黙の縮小変換 | 代入・引数渡し・戻り値で幅が狭まり値が失われる | `-Wconversion` / `-Wsign-conversion` (警告) |
| 明示キャストの正当性 | キャストが警告を消す一方、範囲外の値を静かに壊す | 本節の規則とレビュー (警告では検出できない) |
| 演算のオーバーフロー | 符号付きは未定義動作、符号なしは回り込み | 本節の事前検査 (警告ではほぼ検出できない) |

> [!NOTE]
> SEI CERT C は INT30-C (符号なしの回り込み防止)、INT31-C (変換でデータと符号を失わせないこと)、INT32-C (符号付き演算のオーバーフロー防止)、INT02-C (整数変換規則の理解) を定めています。  
> MISRA C:2012 Rule 10.1 から 10.8 は Essential Type Model に基づき、演算と代入における型の混在を制限します。  
> 本節はこれらと同じ問題意識に立ちつつ、本リポジトリの読み手とクロスプラットフォーム前提に合わせて規則を置きます。

### 符号付きと符号なしの比較

符号付き型と符号なし型を直接比較しません。  
比較する場合は、負値を先に排除してから同じ符号性の型へそろえます。

```c
/* 望ましい。負値を排除してから符号なしへそろえて比較する */
if (value < 0)
{
    return SAMPLE_ERR_INVALID_ARG;
}
if ((size_t)value > limit)
{
    return SAMPLE_ERR_OUT_OF_RANGE;
}

/* 望ましくない。value が負のとき大きな正値として比較される */
if (value > limit)
{
    return SAMPLE_ERR_OUT_OF_RANGE;
}
```

外部文字列から符号なしの目的型へ変換するときの順序は、[文字列入力から意味付き型への変換](#文字列入力から意味付き型への変換) に従います。

> [!NOTE]
> `-Wsign-compare` は `-Wextra` に含まれ、`app/makepart.mk` の `GCC_WARN_BASE` で有効です。  
> 符号混在比較の多くはビルド時に検出できます。

### 縮小変換と符号変換の明示キャスト

幅が狭まる変換、または符号性が変わる変換を明示キャストするときは、次のいずれかを満たします。

1. **直前に範囲検査を置く** (上限と下限。符号なしへ渡す前の負値検査を含む)
2. **検査が不要である理由をコメントで残す**

検査が不要とみなせる例は次のとおりです。

- 直前の `if` で値が変換先の範囲に収まることが確定している
- `strlen` の結果を、上限が定数の固定長バッファーへ渡すなど、変換先の上限が文脈から自明です。
- 同じ関数内ですでに検査済みの値を、別の型名へ付け替えるだけです。

```c
/* 望ましい。範囲検査の直後にキャストする */
if (value < 0 || value > (int64_t)UINT32_MAX)
{
    return SAMPLE_ERR_OUT_OF_RANGE;
}
header->len = (uint32_t)value;

/* 望ましい。検査が不要な理由をコメントに残す */
/* path は PATH_MAX 未満であることが呼び出し規約で保証されている */
name_len = (int)strlen(path);

/* 望ましくない。範囲を確認しないキャスト */
header->len = (uint32_t)value;
```

> [!IMPORTANT]
> 明示キャストは `-Wconversion` / `-Wsign-conversion` の警告を消します。  
> 範囲検査を伴わないキャストは、値を静かに壊したままビルドを通します。  
> 検査を書かない場合は、理由コメントがないと「検査漏れ」と「意図的な省略」を区別できません。

> [!NOTE]
> 検査が自明なキャストに同じ検査を再度書くのは冗長です。  
> 一方で「自明」の判断は読み手によって割れるため、検査を書かない場合は理由をコメントで残します。  
> これは、[一括変更で対象外とした箇所の明示](#一括変更で対象外とした箇所の明示) と同じ形です。

#### 既存コードへの適用

- 新規コードは最初から本規則に従います
- 既存の明示キャストは、変更対象ファイルに触れる機会に合わせて適用します。本規則を目的とした一括対応は求めません

### size_t の減算

`size_t` の減算は、減算の前に大小関係を検査します。

```c
/* 望ましい */
if (total < used)
{
    return SAMPLE_ERR_INVALID_ARG;
}
remain = total - used;

/* 望ましくない。total < used のとき巨大な正値へ回り込む */
remain = total - used;
```

> [!WARNING]
> `size_t` は符号なしです。  
> `a - b` で `a < b` のとき結果は負にはならず、表現可能な最大値付近の正値になります。  
> その値を長さや添字に使うと、バッファー外アクセスにつながります。

### 符号付き整数のオーバーフロー

符号付き整数のオーバーフローは未定義動作です。  
加減乗算の結果が型の範囲を超えないことを、**演算の前** に上限と下限との関係で検査します。

```c
/* 望ましい。加算の事前検査 (int) */
if (b > 0 && a > INT_MAX - b)
{
    return SAMPLE_ERR_OUT_OF_RANGE;
}
if (b < 0 && a < INT_MIN - b)
{
    return SAMPLE_ERR_OUT_OF_RANGE;
}
sum = a + b;

/* 望ましい。非負どうしの乗算の事前検査 */
if (b != 0 && a > limit / b)
{
    return SAMPLE_ERR_OUT_OF_RANGE;
}
product = a * b;
```

処理系組み込みのオーバーフロー検出 (`__builtin_add_overflow` など) は使用しません。  
MSVC に同等の組み込みがなく、プラットフォームごとの条件分岐が増えるためです。  
加減乗算の検査を共通 API へ切り出す必要が生じた時点で、cplat への追加を検討します。

> [!WARNING]
> 演算後の結果を見る事後検査は成立しません。  
> `if (a + b < a)` は符号なしでは成立しますが、符号付きでは加算の時点で未定義動作が発生しており、最適化によって条件そのものが削除されます。  
> 検査は必ず演算の前に行ってください。

> [!NOTE]
> 要素数と要素サイズの乗算は、本章の乗算検査ではなく [配列の確保](#配列の確保) に従います。  
> `calloc(count, size)` または `cplat_calloc` を使い、自前で `count * size` を計算しません。

### 符号なし整数の回り込み

符号なし整数の回り込みは定義された動作です。  
ただし、意図しない回り込みは不具合です。

- 長さ、添字、残量など、回り込みが意味を持たない値では、演算の前に範囲を検査します
- 意図して回り込ませる場合 (巡回カウンター、ハッシュなど) は、その意図をコメントで明示します

```c
/* 望ましい。意図的な回り込みであることをコメントする */
/* 32 bit の巡回シーケンス番号。UINT32_MAX の次は 0 へ戻る */
seq = seq + 1U;
```

### 整数昇格と狭い型同士の演算

`uint16_t` や `uint8_t` どうしの演算は、整数昇格により `int` で計算されます。  
結果を広い型へ代入しても、計算自体が `int` の範囲で桁あふれする場合があります。

```c
/* 望ましくない。a * b は int で計算され、int の範囲で桁あふれしうる */
uint16_t a;
uint16_t b;
uint32_t product;

product = a * b;

/* 望ましい。演算の前に十分な幅へ広げる */
product = (uint32_t)a * (uint32_t)b;
```

> [!NOTE]
> 整数昇格後の型が符号付き `int` になるため、昇格後の乗算が `INT_MAX` を超えると未定義動作になります。  
> 結果の代入先が `uint32_t` であっても、計算の途中で発生した未定義動作は消えません。

### 既存規定への委譲

| 対象 | 従う規定 |
|---|---|
| 外部文字列から整数への変換 | [文字列入力から意味付き型への変換](#文字列入力から意味付き型への変換)。cplat 利用時は `cplat_parse_*` |
| 要素数を伴うメモリ確保の乗算 | [配列の確保](#配列の確保)。cplat 利用時は `cplat_calloc` / `cplat_realloc` 系 |

app 固有の API が関数内部で検査する範囲は、対象パスに適用される `AGENTS.md` とコーディング規範で定義してください。

### 警告オプション

Linux では `app/makepart.mk` と `framework/testfw/makepart.mk` の `GCC_WARN_BASE` で次を有効にしています。

| フラグ | 検出する論点 |
|---|---|
| `-Wsign-compare` (`-Wextra` に含まれる) | 符号付きと符号なしの比較 |
| `-Wconversion` | 値を変えうる暗黙の型変換 (縮小変換を含む) |
| `-Wsign-conversion` | 符号付きと符号なしのあいだの暗黙変換 |

Windows では `framework/makefw/makefiles/_flags.mk` の `CWARNS ?= /W4` により、C4244 / C4267 (縮小変換による値の欠落) と C4245 / C4389 (符号付きと符号なしの不一致) が報告されます。

`-Werror` と `/WX` は設定しません。  
CI は警告を成果物として収集し公開しますが、警告の有無でビルドを失敗させません。

OSS 由来の公開ヘッダーは、提供 app の `appdeps.mk` で `APP_PROD_INCLUDE_CLASS := system` と定義します。  
利用側では Linux の `-isystem` または MSVC の `/external:I` として扱われるため、外来ヘッダー内部の警告だけを分離できます。  
上流の一次ソース自体に必要な警告抑制は、そのソースをコンパイルする末端の `makepart.mk` へ限定して指定します。

> [!NOTE]
> `-Wconversion` が報告しないことは、変換が安全であることを意味しません。  
> MSVC の `/W4` を通すために明示キャストが入っている箇所では、Linux 側の警告も消えます。  
> 明示キャストの正当性は [縮小変換と符号変換の明示キャスト](#縮小変換と符号変換の明示キャスト) で担保します。

### 検証

暗黙変換の残存は、有効化した警告フラグ付きのビルドで確認します。

```bash
# 構文検査の例 (モジュールのインクルード パスは対象に合わせて追加する)
find <module-dir>/prod -name '*.c' -print0 | xargs -0 -n1 \
  gcc -std=c17 -D_DEFAULT_SOURCE -fsyntax-only \
  -Wall -Wextra -Wconversion -Wsign-conversion \
  -I<module-dir>/prod/include -I<module-dir>/prod/include_internal
```

> [!IMPORTANT]
> 明示キャストの直前に範囲検査があるか、理由コメントがあるかは、警告では判定できません。  
> 変更差分のレビューで [縮小変換と符号変換の明示キャスト](#縮小変換と符号変換の明示キャスト) を確認してください。

## 関数引数の異常入力対応

### 基本ルール

関数引数のうち「概念的には正の値のみを想定する」整数値も、型として `int` を採用します。

> [!NOTE]
> 符号なし型で受けると、呼び出し側で計算結果として混入した負値が大きな正値へ変換され、関数側で検出できなくなります。  
> `int` で受けることで、異常な入力を関数の入口で検出できます。

ただし、値の意味に対応する型が「[値の意味に対応する型](#値の意味に対応する型)」で定められている場合、または外部 API が型を指定している場合は、その型を優先します。  
負値を検査する必要がある外部文字列は、符号なしの目的型へ直接変換せず、`int64_t` として解析してから検査します。

負値が渡された場合の挙動は、仕様として明示します。

- 戻り値で結果を返せる関数は、引数不正を表すエラー コードを返します ([エラー処理と戻り値規約](#エラー処理と戻り値規約) を参照)。
- 戻り値を持たない関数 (例: sleep 系) は、無処理 (no-op) で戻ります。

仕様は Doxygen コメントに必ず明記します。

### ポインター引数の NULL 検査

NULL 検査の義務は、その関数が **宣言されているヘッダーの位置** で決まります。

| 層 | 判定基準 | NULL 検査 |
|---|---|---|
| 公開 | `prod/include/` 配下のヘッダーで宣言されている | **必須** |
| ライブラリ内共有 | `prod/include_internal/` 配下のヘッダーで宣言されている | **必須** |
| モジュール内共有 | 実装と同じディレクトリのモジュール私有ヘッダーで宣言されている | 呼び出し側の責務 (検査しない) |
| ファイル内 | どのヘッダーでも宣言されず `static` が付いている | 呼び出し側の責務 (検査しない) |

検査必須の層では、ポインター引数を受けるすべての関数が、本体の先頭で NULL を検査します。  
NULL が渡された場合の挙動は、[関数引数の異常入力対応](#関数引数の異常入力対応) の基本ルールが負値に定めるものと同じ扱いとします。

- 戻り値で結果を返せる関数は、引数不正を表すエラー コードを返します
- 戻り値を持たない関数は、無処理 (no-op) で戻ります
- 戻り値規約の適用対象外 API (互換ラッパーなど) は、元 API の失敗表現に従います

`static` 関数は NULL を検査しません。  
呼び出し元がすべて同一ファイル内にあり、前提条件を呼び出し側で保証できるためです。  
`static` 関数で NULL を検査してよいのは、その関数が NULL を正常な入力として扱う場合に限ります。

モジュール私有ヘッダーで宣言する関数も同じ扱いです。  
呼び出し元がすべて同一ディレクトリ内にあり、前提条件を呼び出し側で保証できるためです。  
前提条件は Doxygen コメントへ記載します。

> [!IMPORTANT]
> 層の判定は **ヘッダーの配置** で行い、名前に `_internal_` が付いているかでは判定しません。  
> [命名規則](#命名規則) のシンボル規則は既存コードへの一括適用を求めていないため、`include_internal/` で宣言されていながら接頭辞が未移行の関数が存在します。  
> これらも検査必須の層に含まれます。

> [!NOTE]
> 層で義務を分けるのは、呼び出し元の見える範囲が異なるためです。  
> 公開とライブラリ内共有は、呼び出し元をコンパイル単位の外に持ちます。前提条件を型でも配置でも強制できないため、関数の入口で防御します。  
> `static` は呼び出し元が同一ファイルに閉じており、渡す値をすべて確認できます。ここでの検査は、すでに保証された前提の重複になります。

#### 省略可能な出力引数

NULL が「その情報は不要」を意味する出力引数は、本規則の対象外です。  
エラー詳細の受け皿など、呼び出し元が受け取りを省略できる引数が該当します。  
この場合は NULL を許容することを Doxygen に明記し、実装側で分岐します。

> [!WARNING]
> 省略可能でない出力引数の NULL 検査を省くと、呼び出し元が誤って NULL を渡した時点で書き込みが起こり、異常終了します。  
> 出力引数は「省略可能なので NULL 許容」か「必須なので NULL 検査」かのいずれかに定め、Doxygen へ明記してください。

#### Doxygen の記述

検査必須の層では、次の 2 つを揃えて記述します。

- 引数の説明に `NULL を渡してはなりません。` と書き、契約を示します。
- `@return` に、NULL を渡した場合に返るエラー コードを書く

```c
/**
 *  @brief          パスからディレクトリ部を取り出します。
 *  @param[out]     dir_out     出力バッファー。NULL を渡してはなりません。
 *  @param[in]      dir_size    @p dir_out のサイズ。0 を渡してはなりません。
 *  @param[out]     detail_out  エラー詳細。不要な場合は NULL を指定できます。
 *  @param[in]      path        対象のパス。NULL を渡してはなりません。
 *  @return         結果コード。引数不正時は @ref SAMPLE_ERR_INVALID_ARGUMENT を返します。
 */
int sample_path_dirname(char *dir_out, size_t dir_size, sample_error *detail_out, const char *path);
```

> [!NOTE]
> 「NULL を渡してはなりません」と書きながら実際に検査することは、矛盾しません。  
> 前者は呼び出し側が守るべき契約の表明であり、後者は契約が破られたときに未定義動作を避けるための防御です。  
> 契約を「NULL の場合はエラーを返します」と書き換えると、NULL を渡す呼び出しが正当な使い方であるかのように読めます。

#### NULL を正当入力とする API

比較や演算など、NULL をエラーにせず定義済みの値を返す API は、拒否契約ではなく許容契約として書きます。

- `@param` に `NULL を指定できます` と、そのときの戻り値または無処理を書きます。
- その契約を `@warning` には置きません。
- 単体テストの確認タグは `[確認_正常系]` とします。

#### 既存コードへの適用

- 新規の公開関数とライブラリ内共有関数は、最初から本規則に従います
- 既存関数は、変更対象ファイルに触れる機会に合わせて適用します。本規則を目的とした一括対応は求めません

#### 検証

ポインター引数を持つ非 `static` の関数定義のうち、本体の先頭付近に NULL 検査がないものを抽出します。

```bash
cat > /tmp/nullchk.awk <<'AWK'
/^[A-Za-z_][A-Za-z0-9_ \t*]*\([^;]*\*[^;]*\)[ \t]*$/ && !/^static/ {
    fn = $0; ln = FNR; buf = ""; n = 0
    while (n < 8 && (getline line) > 0) { buf = buf line "\n"; n++ }
    if (buf !~ /== NULL/) printf "%s:%d: %s\n", FILENAME, ln, fn
}
AWK

find <module-dir>/prod/libsrc -name '*.c' -exec awk -f /tmp/nullchk.awk {} +
```

> [!IMPORTANT]
> この抽出は補助です。次の理由から、結果はコード レビューで判定してください。
>
> - 複数行にわたる関数定義や、`static` を戻り値型の後ろに置く書き方を拾えません
> - ポインター引数がすべて省略可能な出力引数である関数を、誤って報告します
> - 検査を別のヘルパー関数へ委ねている関数を、誤って報告します

### 例

```c
/**
 *  @brief          指定時間ロックを待機します。
 *  @param[in]      timeout_ms タイムアウト (ms)。負値は @ref SAMPLE_ERR_INVALID_ARGUMENT を返します。
 *  @return         結果コード。
 */
int sample_lock_lock(sample_lock *mtx, int timeout_ms)
{
    if (timeout_ms < 0)
    {
        return SAMPLE_ERR_INVALID_ARGUMENT;
    }
    /* ... */
}

/**
 *  @brief          指定ミリ秒だけ呼び出しスレッドを停止します。
 *  @param[in]      ms 停止時間 (ms)。0 以下は無処理で戻ります。
 */
void sample_sleep_ms(int ms)
{
    if (ms <= 0)
    {
        return;
    }
    /* ... */
}
```

## 異常状態の検出とプロセス終了

### assert を使用しない

`app/` 配下では `assert` (`<assert.h>`) を使用しません。  
外部 OSS 由来のコードは本規則の対象外です。

異常状態は [関数引数の異常入力対応](#関数引数の異常入力対応) の引数検査で検出し、結果コードで返します。

コンパイル時に判定できる条件には `static_assert` を使用します。  
`static_assert` はコンパイル時に評価され、`NDEBUG` の影響を受けないため、本規則の対象外です。

> [!WARNING]
> 本リポジトリの既定のビルド構成は `RelWithDebInfo` であり、`NDEBUG` が定義されます。  
> `Release` でも定義され、`Debug` でのみ未定義です。  
> したがって `assert` を書いても、**製品ビルドでも `make test` でも評価されません**。  
> GCC 8.5.0 の実測では、`assert(bump() == 0);` の `bump` 呼び出し回数が `Debug` 相当で 1 回、`RelWithDebInfo` 相当で **0 回** でした。

> [!NOTE]
> `assert` を使わないのは、**ビルド構成によって挙動が変わる二重の状態を作らない** ためです。  
> 構成を変えたテストで検出力を上げるよりも、製品と同一の構成で問題を見つけることを優先します。  
> この判断により、製品とテストのビルド構成を変更する必要がなくなります。

> [!NOTE]
> 現代の一般的な C では、契約違反 (プログラマーのバグ) の表明に `assert` を使うのが一般的です。  
> 本規範はそれを採らず、契約違反も実行時エラーも一律に引数検査と結果コードで扱います。  
> ただし SEI CERT C MSC11-C が「assertion を実行時エラーの検査に使ってはならない」とする点とは矛盾しません。  
> 本規範は assertion 自体を使わないため、その誤用が起こりません。

### 異常状態の検出手段

条件の種類に応じて、次の機構を使います。

| 条件の種類 | 機構 | 有効なビルド |
|---|---|---|
| コンパイル時に判定できる不変条件 (サイズ、オフセット、ABI 値) | `static_assert` | すべて |
| 公開 / `include_internal` の API 引数契約 | 引数検査 + 結果コード | すべて |
| 外部入力・環境起因の失敗 | 結果コード + エラー詳細 | すべて |
| プロセスの継続が危険な状態 | `abort()` の直接呼び出し | すべて |

本表に `assert` の行はありません。  
すべての機構が、ビルド構成によらず常に有効であることが本章の要点です。

### プロセスの異常終了 (abort)

`abort()` を呼んでよいのは、次のいずれかに該当する場合に **限ります**。

- プロセスをそのまま稼働させることが構造上困難です。
- プロセスの継続が危険である (誤ったデータの書き出し、内部状態の破壊の伝播など)

次の規則に従います。

- `assert` を経由せず、`abort()` を直接呼ぶ
- `NDEBUG` の定義有無に関わらず有効とします。ビルド構成によって挙動を変えない
- 呼び出す直前に、原因を特定できる情報を記録します。
- プロセスを終了させうる関数は、その旨を Doxygen に明記します。

次の場合は `abort()` を呼びません。

| 状況 | 正しい扱い |
|---|---|
| 引数が不正 (NULL、負値、範囲外) | 結果コードを返す |
| 外部入力が不正 | 結果コードを返す |
| ファイル、通信、メモリ確保の失敗 | 結果コードを返す |
| 呼び出し元が回復できます。 | 結果コードを返す |

```c
/**
 *  @brief          レコードを書き出します。
 *  @param[in]      writer  書き出し先。NULL を渡してはなりません。
 *  @param[in]      record  書き出すレコード。NULL を渡してはなりません。
 *  @return         結果コード。引数不正時は @ref SAMPLE_ERR_INVALID_ARGUMENT を返します。
 *  @attention      内部状態の破壊を検出した場合、本関数はプロセスを終了させます。
 */
int sample_writer_put(sample_writer *writer, const sample_record *record)
{
    if ((writer == NULL) || (record == NULL))
    {
        /* 引数不正は回復可能。結果コードで返す */
        return SAMPLE_ERR_INVALID_ARGUMENT;
    }

    if (writer->magic != SAMPLE_WRITER_MAGIC)
    {
        /* 内部状態が壊れている。続行すると誤ったオフセットへ書き出す */
        sample_trace_write(SAMPLE_TRACE_LEVEL_ERROR, "writer の magic が不正です (offset=%lld)",
                           (long long)writer->offset);
        abort();
    }

    /* ... */
    return SAMPLE_OK;
}
```

```c
/* 望ましくない: 回復可能な失敗で abort している */
int sample_config_load(const char *path)
{
    int ret;

    ret = sample_file_open(path);
    if (ret != SAMPLE_OK)
    {
        abort(); /* 設定ファイルが無いだけ。呼び出し元が既定値で継続できる */
    }

    return SAMPLE_OK;
}
```

> [!IMPORTANT]
> 共有ライブラリが `abort()` すると、**利用者のプロセスごと終了します**。  
> ライブラリでは原則として結果コードで返し、`abort()` は上記の条件を満たす場合の最終手段としてください。

> [!NOTE]
> `exit()` ではなく `abort()` を使うのは、内部状態が壊れている前提だからです。  
> `exit()` は `atexit` ハンドラーと stdio のフラッシュを実行するため、壊れたデータを書き出す、または二次障害を招きます。  
> `abort()` は `SIGABRT` を発生させ、クラッシュ ダンプを採取できます。

> [!NOTE]
> 「継続が危険な状態で `abort()` する」のは業界一般でも採られている形です。  
> glibc の `__stack_chk_fail` (スタック保護の検出)、`__fortify_fail` (バッファー オーバーフローの検出)、`malloc` のヒープ破壊検出は、いずれも `abort()` します。  
> いずれも「検出した時点で、続行するとより大きな被害が出る」種類の条件です。

> [!IMPORTANT]
> Windows で `abort()` を呼んだときの挙動 (メッセージ出力、Windows エラー報告への通知) は、サービス プロセスとして動作させる場合に確認が必要です。  
> 本規範は Linux 環境でのみ挙動を確認しており、Windows の具体的な挙動と制御方法は未確認です。  
> サービスとして常駐するプログラムへ `abort()` を導入する場合は、実機で確認してください。

### 検証

```bash
# assert の使用と assert.h のインクルード (0 件であること)
# static_assert / _Static_assert は語境界の条件により検出されない
grep -rnE '(^|[^A-Za-z0-9_])assert[[:space:]]*\(|#[[:space:]]*include[[:space:]]*<assert\.h>' \
  app --include=*.c --include=*.h \
  | grep -vE 'app/(lua|sqlite)/prod/|app/cjson/prod/(include|libsrc/cjson)/cJSON(\.|_Utils\.)|/obj/|doxybook2_'

# abort() の呼び出し箇所 (件数は最小であること)
grep -rnE '(^|[^A-Za-z0-9_])abort[[:space:]]*\(' app --include=*.c \
  | grep -vE 'app/(lua|sqlite)/prod/|app/cjson/prod/(include|libsrc/cjson)/cJSON(\.|_Utils\.)|/obj/'
```

抽出した `abort()` の各箇所について、次を確認します。

- 継続が危険な状態に限られており、回復可能な失敗に使っていないこと
- 直前に原因を特定できる情報を記録していること
- プロセスを終了させうる関数の Doxygen に、その旨が書かれていること

## 動的メモリの確保と解放

### 確保失敗の検査

確保関数 (`malloc`、`calloc`、`realloc`) の戻り値は、呼び出した直後に NULL を検査します。

確保に失敗した場合は、[異常状態の検出とプロセス終了](#異常状態の検出とプロセス終了) の方針に従い、結果コードで返します。  
メモリ確保の失敗は回復可能な失敗であり、`abort()` を呼びません。

```c
/* 望ましい */
buffer = (uint8_t *)calloc(count, sizeof(*buffer));
if (buffer == NULL)
{
    return SAMPLE_ERR_OUT_OF_MEMORY;
}
```

### 確保サイズの書き方

確保サイズは `sizeof(*p)` の形で書きます。`p` は確保結果の代入先です。  
型名を直接書く形 (`sizeof(sample_entry)`) は使用しません。

代入先が構造体メンバーの場合も同じ規則を適用し、`sizeof(*obj->member)` と書きます。

| 代入先 | 書き方 |
|---|---|
| ローカル変数 | `handle = (sample_tracer *)malloc(sizeof(*handle));` |
| 宣言と初期化 | `size_t *order = (size_t *)calloc(count, sizeof(*order));` |
| 構造体メンバー | `win->packets = (sample_packet *)calloc(count, sizeof(*win->packets));` |

代入先にメンバー名や変数名が存在しない場合に限り、型名を書けます。  
バイト列そのものを扱うバッファーのように、確保サイズが要素型から導かれない場合が該当します。

```c
/* 対象外。sizeof の被演算子が型ではなく文字列リテラルである */
handle->lock_path = (char *)malloc(path_len + sizeof(SAMPLE_LOCK_SUFFIX));
```

> [!NOTE]
> `sizeof(*p)` を使うと、宣言の型を変更したときに確保サイズが自動的に追従します。  
> 型名を書く形では、宣言だけを変更して確保行を追従し忘れると、必要より小さい領域を確保したままビルドが通ります。

> [!IMPORTANT]
> 「宣言と初期化」の形では、`sizeof` の被演算子がまだ初期化されていない変数を指します。  
> `sizeof` は被演算子を評価しない (可変長配列を除く) ため、正当な書き方です。  
> 本規範は可変長配列を使用しないため、この例外に該当する箇所はありません。

### 戻り値のキャスト

確保関数の戻り値は、代入先の型へキャストします。

```c
/* 望ましい */
specs = (sample_spec *)calloc(count, sizeof(*specs));

/* 望ましくない。型の食い違いが検出されない */
specs = calloc(count, sizeof(*specs));
```

> [!NOTE]
> 確保関数は `void *` を返し、`void *` からオブジェクト ポインターへの変換は無診断です。  
> キャストを書くと、キャスト後の型と代入先の型の不一致が制約違反となり、`-Wincompatible-pointer-types` で検出されます。  
> 宣言の型を変更して確保行を追従し忘れた場合が、これで捕まります。

> [!NOTE]
> 「確保関数の戻り値をキャストしてはならない」という業界一般の指針は、C89 で暗黙の `int` 戻り値が許容されていた時代に、キャストが `<stdlib.h>` の取り込み漏れを覆い隠したことを根拠にしています。  
> 本規範が前提とする C17 では暗黙の宣言が制約違反であり、キャストの有無にかかわらず診断されます。根拠が失効しているため、本規範はキャストを求める側を採ります。

> [!IMPORTANT]
> キャストと `sizeof(*p)` は、別の欠陥を捕まえる補完関係にあります。  
> 片方だけでは、もう片方の欠陥が検出されないまま残ります。
>
> | 欠陥 | キャスト | `sizeof(*p)` |
> |---|---|---|
> | 確保する型が宣言と食い違う | 検出します。 | 検出しません。 |
> | `sizeof` の被演算子が別の型を指す | 検出しません。 | 原理的に発生しません。 |

### 配列の確保

要素数を伴う確保には `calloc(count, size)` を使用します。  
`malloc(count * size)` の形は使用しません。

```c
/* 望ましい */
entries = (sample_entry *)calloc(count, sizeof(*entries));

/* 望ましくない */
entries = (sample_entry *)malloc(count * sizeof(*entries));
```

> [!WARNING]
> `malloc(count * size)` は、乗算が `size_t` を回り込むと **失敗せずに成功します**。  
> 要求バイト数が 0 に化けたうえで確保が成功するため、以後の書き込みがすべてヒープ破壊になります。  
> `calloc` は要素数とサイズを別引数で受け取るため、この回り込みを検出して NULL を返します。
>
> GCC 8.5.0 / glibc での実測です。
>
> ```c
> size_t huge = SIZE_MAX / 4U + 1U;
>
> /* 回り込みを検出して NULL を返す */
> void *a = calloc(huge, 8U);
>
> /* 有効な非 NULL を返す。0 バイトの確保に成功する */
> void *b = malloc(huge * 8U);
> ```

> [!IMPORTANT]
> `calloc` はゼロ初期化を行います。  
> 確保方法を `calloc` から `malloc` へ戻す変更は、初期化の前提を無言で変えます。  
> 確保方法を変更するときは、ゼロ初期化に依存している箇所がないことを確認してください。

`realloc` には要素数とサイズを分けて渡す形が標準にないため、伸長時の乗算は呼び出し側に残ります。  
[再確保 (realloc)](#再確保-realloc) の規則に加え、要素数の上限を呼び出し側で検査します。  
確保サイズ以外の一般の整数演算におけるオーバーフロー検査は、[整数演算の安全性](#整数演算の安全性) に従います。

> [!NOTE]
> MISRA C:2012 Dir 4.12 と Rule 21.3 は動的メモリの使用そのものを禁止しています (いずれも Required)。  
> これは組み込み向けの前提に立つ規則であり、本リポジトリには適用しません。

### 長さ 0 の確保

長さ 0 の確保は行いません。要素数が 0 になりうる経路では、確保を呼び出す前に分岐します。

```c
/* 望ましい */
if (count == 0U)
{
    *entries_out = NULL;
    *count_out = 0U;
    return SAMPLE_OK;
}
entries = (sample_entry *)calloc(count, sizeof(*entries));
```

> [!WARNING]
> `malloc(0)` と `calloc(0, size)` の戻り値は処理系定義です。  
> glibc は非 NULL の一意なポインターを返しますが、NULL を返す実装もあります。  
> NULL を確保失敗と判定する規約と組み合わさると、**処理系を移しただけで成功が失敗に変わります**。

### 再確保 (realloc)

`realloc` の戻り値は、元のポインターとは別の変数で受けます。  
NULL を検査したうえで、成功した場合にのみ元のポインターへ代入します。

```c
/* 望ましい */
sample_entry *new_entries;

new_entries = (sample_entry *)realloc(entries, new_count * sizeof(*new_entries));
if (new_entries == NULL)
{
    return SAMPLE_ERR_OUT_OF_MEMORY;
}
entries = new_entries;
```

> [!WARNING]
> 戻り値を元のポインターへ直接代入すると、失敗時に **元の領域が参照不能になります**。  
> `realloc` は失敗しても元の領域を解放しないため、ポインターを NULL で上書きした時点で解放する手段が失われます。
>
> ```c
> /* 望ましくない */
> entries = realloc(entries, new_count * sizeof(*entries));
> ```

### 解放と NULL 代入

解放した後もスコープに残るポインターには、`free` の直後に NULL を代入します。  
構造体メンバーと外部リンケージ変数が対象です。

```c
/* 望ましい */
free(ctx->entries);
ctx->entries = NULL;
```

関数から抜ける直前のローカル変数には、NULL を代入しません。  
以後参照されないため、代入が読み手に「後続で使う」という誤った期待を与えます。

```c
/* 望ましくない。直後に関数を抜けるローカル変数 */
out_free_buffer:
    free(buffer);
    buffer = NULL;
    return ret;
```

> [!NOTE]
> 解放済みポインターを残すと、二重解放と解放後参照の入口になります。  
> 一方で、以後参照されないローカル変数への代入は、静的解析でも無駄な代入として報告されます。  
> 対象を「解放後もスコープに残るポインター」に限るのは、この両方を避けるためです。

`free(NULL)` は無害であり、解放の前に NULL を検査する必要はありません。

```c
/* 望ましくない。冗長な検査 */
if (ctx->entries != NULL)
{
    free(ctx->entries);
}
```

### 所有権の表明

確保した領域を返す API は、解放の責任がどちらにあるかを Doxygen の **本文** に記載します。  
`@return` には戻り値の意味だけを書き、解放責任は本文で述べます。

```c
/**
 *  @brief          設定名を複製して返します。
 *
 *  返却した領域は @ref sample_free で解放してください。
 *
 *  @param[in]      config  設定。NULL を渡してはなりません。
 *  @return         複製した文字列へのポインター。失敗時は NULL を返します。
 */
char *sample_config_dup_name(const sample_config *config);
```

確保した領域を返す API には、対になる解放関数を用意します。  
標準の `free` を呼ばせる形にはしません。

> [!NOTE]
> 対になる解放関数を用意するのは、確保と解放を同じライブラリ内で完結させるためです。  
> 共有ライブラリとして配布する場合、利用者側と確保側で C ランタイムが異なると、`free` に渡したポインターが解放側のヒープに属さない状態になりえます。

破棄関数は、実装側の防御として NULL を受け入れてかまいません。  
ただし呼び出し元は、破棄関数へ NULL を渡さないようにします。

> [!IMPORTANT]
> 破棄関数が NULL を受け入れることは、呼び出し元が NULL ガードを省いてよいという意味ではありません。  
> 呼び出し元のコードだけを読んだときに、そのポインターが有効であることが読み取れる形を保ちます。

### 検証

```bash
# malloc による配列確保 (乗算を伴う確保。0 件であること)
grep -rnE '(malloc|realloc)[[:space:]]*\([^;]*[^*/]\*[^;]*sizeof' app --include=*.c \
  | grep -vE 'app/(lua|sqlite)/prod/|app/cjson/prod/(include|libsrc/cjson)/cJSON(\.|_Utils\.)|/obj/|/test/'

# realloc の戻り値を元のポインターへ直接代入 (0 件であること)
# 後方参照を使うため PCRE (-P) を指定する。左側の語境界がないと new_ptr = realloc(ptr, ...) を誤検出する
grep -rnP '(?<![A-Za-z0-9_])([A-Za-z_][A-Za-z0-9_]*(?:(?:->|\.)[A-Za-z_][A-Za-z0-9_]*)*)\s*=\s*\(?[^;=]*realloc\s*\(\s*\1\s*[,)]' \
  app --include=*.c | grep -vE 'app/(lua|sqlite)/prod/|app/cjson/prod/(include|libsrc/cjson)/cJSON(\.|_Utils\.)|/obj/'

# 確保関数の戻り値をキャストしていない箇所
grep -rnE '=[[:space:]]*(malloc|calloc|realloc)[[:space:]]*\(' app --include=*.c \
  | grep -vE 'app/(lua|sqlite)/prod/|app/cjson/prod/(include|libsrc/cjson)/cJSON(\.|_Utils\.)|/obj/'

# 確保サイズに型名を書いている箇所
grep -rnE '(malloc|calloc|realloc)[[:space:]]*\([^;]*sizeof[[:space:]]*\([[:space:]]*[A-Za-z_]' \
  app --include=*.c | grep -vE 'app/(lua|sqlite)/prod/|app/cjson/prod/(include|libsrc/cjson)/cJSON(\.|_Utils\.)|/obj/|/test/'
```

最後の 1 つは、`sizeof` の被演算子が型ではない場合 (文字列リテラルのマクロなど) も抽出します。  
抽出結果は、代入先にメンバー名や変数名が存在するかを目視で確認してください。

## エラー処理と戻り値規約

### 基本ルール

結果コードを戻り値で返す関数は、ライブラリ (app) ごとに 1 つの共通結果コード群へ統一します。  
共通結果コードは、ライブラリの接頭辞を付けた `#define` として専用ヘッダーに集約します。  
以下、接頭辞を `SAMPLE` とした例で示します。

- 成功は `SAMPLE_OK` (0) のみとします。非 0 はすべて「要求した操作が完遂されなかった」ことを表します。
- エラーは負値とします。`SAMPLE_ERR` (-1) は、分類済みコードに該当しないその他のエラーを表します。
- 分類済みのエラーは -2 以降の負値コード (`SAMPLE_ERR_INVALID_ARGUMENT` など) を使用します。
- カテゴリ (モジュール) ごとの結果コード型 (enum) やカテゴリ固有の成功/失敗定数は新設しません。戻り値の型は `int` とします。
- 成功を 1、失敗を 0 とする逆向きの規約、および 1/0/-1 のような三値の規約は使用しません。

各コードの値は ABI として凍結します。既存の値の変更は禁止し、コードの追加は末尾 (より小さい負値) への追記のみとします。

> [!NOTE]
> 結果コードを名前付き enum ではなく `#define` と `int` で表すのは、次の理由によります。
>
> - 値を ABI として凍結し、追加時も再割り当てしないため、型としての網羅性 (`-Wswitch` による分岐漏れ検出) よりも値の安定性を優先します。
> - 分類済みコードは `int` として受け渡され、`errno_out` などの生の詳細値と同じ経路に現れるため、enum 型による型安全性が実質的に機能しません。
> - C の enum は基底型が実装定義であり、ABI を固定する用途に向かない

> [!IMPORTANT]
> [typedef enum](#typedef-enum) の匿名 enum 禁止は、**型として使う enum** を対象とした規則です。  
> 本節が結果コードに enum を採らないこととは、対象と目的が異なります。  
> 状態や種別を表す型を新設する場合は、匿名 enum を避けてタグ名と typedef 名を持つ enum を定義してください。

### 判定慣用句

呼び出し側の成否判定は、コード名との比較を正とします。

```c
int ret;

ret = sample_resource_attach(path, &handle);
if (ret != SAMPLE_OK)
{
    return ret;
}
```

全エラーが負値のため `ret < 0` も等価ですが、名前比較を推奨します。  
特定のエラーを区別する場合は、`ret == SAMPLE_ERR_TIMEOUT` のようにコード名で比較します。  
`-1` などの数値リテラルとの比較は行いません。

### 戻り値とエラー詳細の役割分担

戻り値は「分類済みの結果コード」を伝達し、OS 由来の詳細は出力引数で伝達します。

| 伝達手段 | 内容 |
|---|---|
| 戻り値 (`int`) | `SAMPLE_OK` または負値の分類済みエラー コード |
| `int *errno_out` などの出力引数 | 生の詳細値。Linux では `errno`、Windows では `GetLastError()` の値 |

分類済みコードでは失われる詳細 (`ENOENT` と `EACCES` の区別など) が必要な API のみ、`errno_out` を提供します。  
`errno`、`GetLastError()`、`HRESULT` などの OS エラー値を、共通結果コードとして直接返しません。

### 真偽値や状態を返す API の設計

「一致したか」「実行したか」などの真偽の答えと、操作の成否は分離します。

- 真偽の答えは `int *xxx_out` の出力引数で返し、戻り値は結果コードとします。
- 出力引数の値は、戻り値が `SAMPLE_OK` の場合のみ有効とします。

公開ヘッダーで `bool` を使わない理由は [真偽値の型](#真偽値の型) を参照してください。

```c
/* 望ましい: 成否と真偽値を分離 */
int sample_paths_equal(const char *lhs, const char *rhs, int *equal_out, int *errno_out);

/* 望ましくない: 1 (一致) / 0 (不一致) / -1 (失敗) の三値 */
int sample_paths_equal(const char *lhs, const char *rhs, int *errno_out);
```

### 詳細コードの扱い

解析エラーの種別など、共通結果コードより細かい粒度の分類が必要な場合は、app 単位の単一コード集合へコードを追加することを原則とします。

モジュール固有のコード体系を別に設けることは例外とし、採用する場合は共通結果コードと値が重複しないこと、および取得用 API と出力引数のどちらで伝達するかを、そのライブラリの特化事項として明記します。

ただし、レガシ マイグレーション案件や製品リリース後の改修など、既存の戻り値とその意味を維持する ABI 契約を重視する場合は、本節の原則よりも各 app のポリシーを優先します。  
この場合は、原則から外れる範囲と維持すべき戻り値規約を、そのライブラリの特化事項として明記します。

> [!NOTE]
> 1 系統に集約するのは、符号の規約が揃い、粗い分類と細かい分類を同じ判定慣用句で扱えるようにするためです。  
> 一方、既存の呼び出し元やバイナリ互換の制約から、コード集合の統合や値の再割り当てが行えない場合があります。  
> 各 app のポリシーを優先するのは、この制約を認めるためです。

### 適用対象外

以下の API 群は、共通結果コードの適用対象外です。

| 対象外の API 群 | 理由 |
|---|---|
| 標準 C / OS API の互換ラッパー (`FILE *`/NULL、0/EOF、fd/-1、`BOOL` などを元 API と同じ規約で返す層) | 元 API の戻り値規約を保存し、元 API の感覚・差し替えとして使えること自体が設計意図 |
| ハンドル生成系 (`*_create` など) | 成功時ポインター / 失敗時 NULL というポインター返却 API の慣用に従う |
| 値をそのまま返す関数 (getter、比較関数など) | 結果コードの概念が適用されない |
| 戻り値を持たない関数 (`*_destroy` など) | 同上 |

互換ラッパー層を対象外とする場合は、対象外とする範囲と根拠を当該ライブラリのドキュメントに明記します。  
対象外の API を新設する場合は、元 API との対応と戻り値規約をヘッダーの Doxygen コメントに明記します。

### 各ライブラリの特化事項

本節は一般的な方針を定めるものです。  
各 app のドキュメントに優先事項、特化事項 (共通結果コードのヘッダー位置、コード一覧、適用対象外の具体的な範囲など) がある場合は、それに従ってください。

### 検証

```bash
# 数値リテラル比較や三値規約の残存確認 (対象モジュール配下)
grep -nE '(==|!=)[[:space:]]*-1\b' <dir>/*.c

# 局所テスト
cd <module-dir> && make test
```

## 変数宣言位置と命令文の関係

### 基本ルール

関数内の変数宣言は、[前提とする言語標準](#前提とする言語標準) の C17 に従い、ブロック途中の宣言を許容します。  
変数宣言はスコープを必要最小限にし、誤認を防ぐため利用箇所に近い位置へ置きます。

一方で、可読性のため次の配置を推奨します。

- 同一ブロック内で早い段階に使う変数は、ブロック先頭付近に集めて宣言します。
- 命令文の後でしか初期値が確定しない変数は、その直後に宣言します。

> [!NOTE]
> スコープを最小にするのは、変数の生存範囲を読み手が追える長さに保ち、初期化前の参照や意図しない再利用を防ぐためです。  
> 一方で宣言を無条件に利用直前へ置くと、宣言が関数の後半へ散在し、変数の一覧を把握できなくなります。  
> 上記 2 つの推奨は、この 2 つの要請の折り合いを示すものです。

### 結果コードを受ける変数の例外

結果コード用の `ret` (および別名の `rc` / `rtc`) は、1 関数の戻り値だけを受けるときはその位置で宣言し、初期化子へ置いて構いません。  
複数関数の戻り値で同じ `ret` を使い回すときは、関数先頭で宣言し、各呼び出しの位置で代入します。  
成功定数、失敗定数、リテラルによる初期化は避けます。  
`result` は自関数が返す結果コードの蓄積に限り、初期化して構いません。

理由と例は [関数内ローカル変数](#関数内ローカル変数) の「結果コード変数 (ret と result)」を参照してください。

### for ループ変数

ループ変数を `for` の初期化式で宣言する記法を許容します。  
`for (int i = 0; ... )` のように記述し、ループ変数の有効範囲をループ内へ限定してください。

### 例

```c
int process_items(const sample_item *items, int count)
{
    int total = 0;

    if ((items == NULL) || (count <= 0))
    {
        return -1;
    }

    for (int i = 0; i < count; ++i)
    {
        total += items[i].value;
    }

    return total;
}
```

```c
int load_and_apply(sample_config_handle *handle, const char *path)
{
    int ret;

    ret = read_config_file(path);
    if (ret != 0)
    {
        return ret;
    }

    /* read_config_file の結果を見てから宣言したほうが意図を読み取りやすい */
    sample_config cfg = {0};
    ret = parse_config(path, &cfg);
    if (ret != 0)
    {
        return ret;
    }

    return apply_config(handle, &cfg);
}
```

```c
/* 非推奨例: 宣言が後半へ散在し、変数の役割を追いにくい */
int calculate_total(const int *values, int count)
{
    if ((values == NULL) || (count <= 0))
    {
        return -1;
    }

    int total = 0;
    for (int i = 0; i < count; ++i)
    {
        total += values[i];
    }

    /* 利用箇所が離れており読み手に負荷をかける */
    int average = total / count;
    return average * count;
}
```

## 式の括弧

### 基本ルール

演算子の優先順位を読み手の記憶に委ねる書き方をしません。  
種類の異なる演算子を 1 つの式に混在させる場合は、結合のまとまりを括弧で明示します。

本節は、コンパイラが警告する範囲よりも広く括弧を求めます。  
括弧の要否を式ごとに判断せず、次の表に該当したら機械的に付けてください。

| 混在の形 | 規則 | 例 |
|---|---|---|
| 比較演算子の左右に算術式 | 算術式を括弧で囲む | `if (pos > (max_body - ellipsis_len))` |
| 比較演算子の左右にビット演算式・シフト式 | ビット演算式・シフト式を括弧で囲む | `if ((flags & SAMPLE_FLAG_DONE) != 0)` |
| `&&` と `\|\|` の混在 | `&&` の各項を括弧で囲む | `if ((a && b) \|\| (c && d))` |
| ビット演算子どうしの混在 (`&` `\|` `^`) | 各項を括弧で囲む | `value = ((a & mask) \| (b & ~mask));` |
| シフト演算子と算術演算子の混在 | シフト式を括弧で囲む | `size = ((1 << shift) + header_len);` |
| 単項 `!` の対象が 2 項式 | 対象を括弧で囲む | `if (!(a == b))` |

### 例

```c
/* 望ましい */
if (pos > (max_body - ellipsis_len))
{
    return SAMPLE_ERR_INVALID_ARGUMENT;
}

if ((flags & SAMPLE_FLAG_DONE) != 0)
{
    return SAMPLE_OK;
}

if ((is_open && has_data) || (is_eof && has_pending))
{
    return SAMPLE_OK;
}
```

```c
/* 望ましくない: 比較と算術の関係が読み手に委ねられる */
if (pos > max_body - ellipsis_len)

/* 望ましくない: 意図と異なる結合になる (`&` は `!=` より優先順位が低い) */
/* flags & (SAMPLE_FLAG_DONE != 0) と解釈され、判定結果が反転する */
if (flags & SAMPLE_FLAG_DONE != 0)

/* 望ましくない: && と || の結合順が読み手に委ねられる */
if (is_open && has_data || is_eof && has_pending)
```

### 括弧を求めない場合

次の場合は括弧を付けません。冗長になり、かえって式の構造が見えにくくなります。

- 同種の演算子だけで構成される式 (`a + b + c`、`a && b && c`)
- 単項演算子とメンバー参照、添字、関数呼び出しの組み合わせ (`*p`、`p->member`、`arr[i]`、`f(x)`)
- 代入の右辺全体 (`total = a + b;` の右辺に括弧は不要)
- `return` の対象が単一の式である場合 (`return count;`)

```c
/* 望ましくない: 同種の演算子に括弧を重ねている */
total = ((a + b) + c);

/* 望ましい */
total = a + b + c;
```

> [!NOTE]
> 本節は、業界一般の C の規約より丁寧に括弧を求めるものです。  
> GCC / Clang の `-Wparentheses` が警告するのは、ビット演算子と比較の混在、および `&&` と `\|\|` の混在に限られます。  
> 比較演算子と算術演算子の組み合わせは、優先順位の上では算術演算子が先に評価されるため結合そのものは意図どおりになり、コンパイラも警告しません。  
> それでも括弧を求めるのは、**読み手が「この式は優先順位を確認しなくてよい」と判断できる状態を優先する** ためです。  
> 判断軸は「業界一般の慣行」ではなく「本リポジトリの読み手」であり、[三項演算子の禁止](#三項演算子の禁止) と同じ考え方に立ちます。

> [!NOTE]
> 括弧の要否を式ごとに判断させないのは、判断の余地を残すと「この式は自明だから不要」という線引きが書き手ごとに揺れ、レビューでの指摘も一貫しなくなるためです。  
> 表に該当したら機械的に付ける形にすることで、書き手とレビュー担当の双方が同じ結論に至ります。

### 検証

括弧の不足は、コンパイラ警告で一部だけ検出できます。  
`-Wparentheses` は `-Wall` に含まれ、`app/makepart.mk` の `GCC_WARN_BASE` で有効になっています。

```bash
cd <module-dir> && make 2>&1 | grep -iE 'parentheses|小括弧'
```

> [!IMPORTANT]
> `-Wparentheses` が検出するのは本節が求める範囲の一部です。  
> GCC 8.5.0 の実測では、ビット演算子と比較の混在 (`flags & SAMPLE_FLAG_DONE != 0`) および `&&` と `\|\|` の混在は警告されますが、**比較演算子と算術演算子の混在 (`pos > max_body - ellipsis_len`) は警告されません**。  
> 表の全項目はコード レビューで確認してください。

## 制御構造の制限

### 資源解放における goto の限定的許容

`goto` は、**関数末尾に置いた解放ラベルへの前方ジャンプ** に限り使用できます。  
複数の資源を確保する関数で、確保できた分だけを逆順に解放する goto チェーンを含みます。

次の使い方は認めません。

| 使い方 | 理由 |
|---|---|
| 後方ジャンプ (ループの代用) | 制御フローが構文構造から読み取れなくなります。 |
| 条件分岐の代用 | `if` / `else` で表せる制御を分かりにくくします。 |
| ブロックの内側へ飛び込む goto | 飛び越した宣言と初期化の対応が追えなくなります。 |

解放対象が 1 つ以下の場合、または解放が不要な場合は、`goto` を使わず早期 return とします。

ラベル名は、そこで何をするかを表す名前とします。  
`out_free_buffer` のように解放対象を含め、`err1` / `err2` のような連番は使いません。  
ラベルは関数内スコープであるため、ライブラリ接頭辞を付けず snake_case とします ([命名規則](#命名規則) と整合)。

解放対象のポインターは、最初のジャンプが起こりうる位置より前で NULL に初期化し、解放関数が NULL を安全に受け取れることを前提とします。

```c
/* 望ましい: 確保できた分だけを逆順に解放する goto チェーン */
int sample_session_open(const char *path, sample_session **session_out)
{
    int ret;
    sample_session *session = NULL;
    unsigned char *buffer = NULL;
    sample_file *file = NULL;

    session = sample_session_create();
    if (session == NULL)
    {
        return SAMPLE_ERR_NO_MEMORY;
    }

    buffer = malloc(SAMPLE_SESSION_BUFFER_SIZE);
    if (buffer == NULL)
    {
        ret = SAMPLE_ERR_NO_MEMORY;
        goto out_dispose_session;
    }

    ret = sample_file_open(path, &file);
    if (ret != SAMPLE_OK)
    {
        goto out_free_buffer;
    }

    sample_session_attach(session, buffer, file);
    *session_out = session;
    return SAMPLE_OK;

out_free_buffer:
    free(buffer);
out_dispose_session:
    sample_session_dispose(session);
    return ret;
}
```

```c
/* 望ましくない: 後方ジャンプでループを代用している */
int sample_retry_connect(sample_session *session, int max_attempt)
{
    int attempt = 0;

retry:
    attempt++;
    if (sample_session_connect(session) != SAMPLE_OK)
    {
        if (attempt < max_attempt)
        {
            goto retry;
        }
        return SAMPLE_ERR_TIMEOUT;
    }

    return SAMPLE_OK;
}
```

```c
/* 望ましくない: 連番ラベルで、何を解放するのか名前から分からない */
err2:
    free(buffer);
err1:
    sample_session_dispose(session);
    return ret;
```

> [!WARNING]
> ラベル以降で参照する変数は、いずれのジャンプ元よりも前で初期化してください。  
> 宣言をジャンプで飛び越すと、解放処理が未初期化のポインターを受け取り、未定義動作になります。  
> 宣言位置の考え方は [変数宣言位置と命令文の関係](#変数宣言位置と命令文の関係) を参照してください。

> [!NOTE]
> 本規範は、以前は `goto` を全面禁止していました。  
> 資源解放に限った前方ジャンプは、業界一般の C において確立した手法であり、禁止によって生じるネストの深化と解放漏れのほうが害が大きいと判断し、条件付きの許容へ改めています。
>
> - Linux kernel coding style は、複数の箇所から抜ける関数で共通の後始末が要る場合に `goto` が有用であるとし、後始末が不要なら直接 return せよとしています。利点として無条件文の追いやすさ、ネストの削減、経路を追加したときの更新漏れの防止を挙げています。ラベル名を `out_free_buffer` とし `err1` / `err2` を避ける指針も、ここに拠ります
> - SEI CERT C MEM12-C は、複数の資源を確保する関数が途中で return するとリークするため、goto チェーンの使用を検討せよとしています (Severity: Low、Likelihood: Probable、Priority: P2 L3)
> - MISRA C:2012 で `goto` を使用すべきでないとする Rule 15.1 は Advisory であり、必須ではありません。必須は Rule 15.2 (同一関数内の後方に宣言されたラベルへジャンプすること、すなわち前方ジャンプのみ) と Rule 15.3 (ラベルは goto と同一ブロックまたはそれを囲むブロックで宣言されること) です。本節が認める範囲は、この 2 つの Required 規則の内側に収まります
> - Dijkstra の「Go To Statement Considered Harmful」が批判の対象としたのは非構造的なジャンプであり、関数末尾へ前方ジャンプする本節の用法とは別のものです
>
> 出典は [参照](#参照) を参照してください。

> [!IMPORTANT]
> 本改定は許容の拡大であり、既存コードの書き換えを求めるものではありません。  
> 全面禁止の下で多段の入れ子や解放ヘルパーで記述されている既存コードを、goto チェーンへ書き換える義務はありません。

#### 検証

前方ジャンプに限られていることの機械検証は困難なため、`goto` とラベルを抽出してレビューで確認します。

```bash
grep -rnE '(^|[^A-Za-z0-9_])goto[[:space:]]|^[a-z_][a-z0-9_]*:[[:space:]]*$' app --include=*.c --include=*.h \
  | grep -vE 'app/(lua|sqlite)/prod/|app/cjson/prod/(include|libsrc/cjson)/cJSON(\.|_Utils\.)|/obj/|doxybook2_' \
  | grep -vE ':[[:space:]]*(public|private|protected):'
```

最後の除外は、C++ のテスト ヘッダーにあるアクセス指定子がラベルとして拾われるためのものです。

抽出した箇所について、次を確認します。

- ラベルが `goto` より後方 (関数末尾側) にあること
- ラベルが解放処理のみを行い、通常経路の処理を含まないこと
- ラベル名が解放対象を表していること

### 三項演算子の禁止

`app/` 配下では三項演算子 (`?:`) を使用しません。  
既存コードを変更する際に三項演算子があった場合は、除去します。  
外部 OSS 由来のコードは対象外です。

代替は `if` / `else` 文、または定数テーブル引きとします。

```c
/* 望ましくない */
const char *label = (level >= SAMPLE_TRACE_LEVEL_WARNING) ? "WARN" : "INFO";

/* 望ましい: if / else */
const char *label;
if (level >= SAMPLE_TRACE_LEVEL_WARNING)
{
    label = "WARN";
}
else
{
    label = "INFO";
}
```

```c
/* 望ましい: 分岐が値の対応表であればテーブル引き */
static const char *const s_level_labels[] = { "INFO", "WARN", "ERROR" };

const char *label = s_level_labels[level];
```

> [!NOTE]
> 三項演算子は、業界一般の C では禁止されない構文です。  
> 本規範が禁止するのは、習熟度に幅のある読み手を前提として可読性を確保するためであり、判断軸は「業界一般の慣行」ではなく「本リポジトリの読み手」です。  
> 同じ理由から、入れ子の三項演算子や、三項演算子を引数に埋め込む書き方は特に避ける対象となります。  
> `goto` を条件付きで許容する判断とは、拠って立つ軸が異なります。

#### 検証

`?` は文字列リテラル、Doxygen コメント、プリプロセッサ条件にも現れるため、grep は補助とし、目視を併用します。

```bash
grep -rnE '\?' app --include=*.c --include=*.h \
  | grep -vE 'app/(lua|sqlite)/prod/|app/cjson/prod/(include|libsrc/cjson)/cJSON(\.|_Utils\.)|/obj/|doxybook2_'
```

## restrict、volatile、inline、register の利用

`restrict`、`volatile`、`inline`、`register` は、コンパイラによる最適化やメモリ アクセスの評価に影響します。  
これらを移植性やスレッド同期の代替として使用しません。  
効果が処理系定義にとどまるものは使用しません。

### restrict による非 alias 契約

`restrict` は、修飾したポインターを通じて参照するオブジェクトへ、同じ実行中に別の経路からアクセスしないという契約を表します。  
この契約を呼び出し元まで確認でき、性能計測で改善を確認した場合だけ使用します。

`restrict` を使用できる場所は `.c` ファイル内の `static` 関数の宣言と定義に限定します。  
`prod/include/` と `prod/include_internal/` のヘッダーでは使用しません。  
公開関数とライブラリ内共有関数では、定義側だけに付ける形も含めて使用しません。

使用前に次の条件をすべて満たすことを確認します。

1. 対象関数が性能上の問題になっていることを計測で確認します。
2. 修飾するポインターごとに、呼び出し元を含む非 alias の根拠を示します。
3. GCC と MSVC の両方で性能を計測し、改善を確認します。
4. 使用箇所のコメントに非 alias の根拠と計測条件を記載します。

```c
/* 望ましくない: 公開ヘッダーが利用者へ非 alias 契約を要求する */
SAMPLE_EXPORT int sample_sum(const int *restrict lhs, const int *restrict rhs, size_t count);
```

```c
/* 望ましい: .c 内で根拠と効果を確認した処理に限定する */
/* lhs と rhs は別々に確保された配列です。計測条件: {条件を記載します。} */
static int sum_arrays(const int *restrict lhs, const int *restrict rhs, const size_t count)
{
    /* ... */
}
```

> [!WARNING]
> `restrict` は単なる最適化のヒントではありません。
> 非 alias 契約に違反すると、C17 の規則上は動作が未定義になります。

### volatile とスレッド同期

`volatile` は、対象へのアクセスを抽象機械の規則に従って評価させます。  
操作の不可分性、スレッド間の可視性、メモリ順序は保証しないため、スレッド同期には使用しません。

スレッド間で共有する状態は、mutex、rwlock、condition variable など、各 app が定める同期機構で保護します。  
標準 C の `_Atomic` と `<stdatomic.h>` は、MSVC の C11/C17 モードで利用できないため使用しません。  
ロックを使用しない共有状態が必要な場合は、GCC と Windows のメモリ順序を共通化する atomic API を別途設計します。

`volatile` を使用できる用途は次のとおりです。

| 用途 | 条件 |
|---|---|
| シグナル ハンドラーと通常処理の間のフラグ | `volatile sig_atomic_t` を使用します。 |
| メモリ マップド I/O | 対象プラットフォームの仕様がアクセスを要求します。 |
| コンパイラまたは OS API が要求する型 | 要求元と理由をコメントに記載します。 |
| 最適化で除去させない意図的なメモリ アクセス | 目的と根拠をコメントに記載します。 |

後二つの用途では、根拠となる仕様の URL を `see: {URL}` の形式でコメントに残します。

```c
/* 望ましい: 非同期シグナルから設定するフラグ */
static volatile sig_atomic_t s_stop_requested;
```

```c
/* 望ましくない: volatile だけでスレッド間の同期を試みる */
static volatile int s_worker_stopped;
```

ロックで保護している状態へ `volatile` を重ねても、追加の同期効果はありません。  
既存の `volatile` は一括変更せず、変更時に用途を分類し、不要な修飾を個別に削除します。  
スレッド間で共有される `volatile` を変更する場合は、データ競合の有無を確認し、同期機構へ置き換えます。

### inline 関数

`inline` はインライン展開を保証しません。  
外部リンケージを持つ inline 定義は外部定義の配置規則を必要とするため、本リポジトリでは使用しません。

`static inline` 関数は `prod/include_internal/` のヘッダーと、モジュール私有ヘッダーだけで定義します。  
使用してよい用途は次の 2 つです。

- 複数の `.c` ファイルで共有する短い補助処理であり、通常関数にすると目的に対して呼び出しの負担が大きい場合
- 既存の関数へ薄く転送するだけの関数を多数用意する場合。実体を持つ翻訳単位と公開シンボルを増やさずに済みます

後者は、型付きの入口を機械的に並べる場合に当てはまります。  
転送先の契約を変えず、本体が 1 つの呼び出しに収まることを条件とします。  
`prod/include/` の公開ヘッダーへ新しい `static inline` 関数を追加しません。  
`.c` ファイルでは `inline` を指定せず、コンパイラの最適化へ任せます。

`inline` と `extern inline` を単独で使用しません。  
強制インライン化マクロを使用する場合も、内部ヘッダーの `static inline` と同じ適用範囲に限定し、`static FORCE_INLINE` の形にします。  
強制インライン化は、通常の最適化で要求性能を満たせず、GCC と MSVC の計測で改善を確認した場合だけ使用します。  
使用箇所には強制する理由、計測条件、コード サイズへの影響をコメントで記載します。

```c
/* 望ましい: prod/include_internal/ の短い補助関数 */
static inline int sample_internal_is_valid(const int value)
{
    return value >= 0;
}
```

```c
/* 望ましい: モジュール私有ヘッダーの領域アクセサー (接頭辞はモジュール名) */
static inline unsigned char *hashtable_entry_status(const cplat_hashtable *ht, size_t rec)
{
    return hashtable_entries(ht) + rec * ht->entry_stride + sizeof(uint64_t);
}
```

```c
/* 望ましくない: 外部定義の配置が必要になる */
inline int sample_is_valid(const int value)
{
    return value >= 0;
}
```

既存の公開ヘッダーにある `static inline` 関数は、この規則の追加だけを理由に変更しません。  
既存箇所の監査は別の変更で実施します。

### register を使用しない

`app/` 配下では `register` を使用しません。  
記憶域クラス指定子を書かず、変数をレジスターへ置くかどうかはコンパイラの最適化へ任せます。  
C++ で記述するテスト コードでも使用しません。  
外部 OSS 由来のコードは対象外です。

レガシ C の移植で `register` が付いた宣言を持ち込む場合は、移植時に除去します。  
除去はアドレス取得の制約を外す方向の変更であり、変数の値と生存期間は変わりません。

```c
/* 望ましくない: register でレジスター割り当てを指示する */
register size_t index = 0;
register int total = 0;
```

```c
/* 望ましい: 記憶域クラス指定子を書かず、最適化へ任せる */
size_t index = 0;
int total = 0;
```

> [!WARNING]
> `register` を付けたオブジェクトへは単項 `&` を適用できません。  
> C17 の制約違反であり、後からアドレスを取る変更を加えるとビルド エラーになります。  
> C++ では `/std:c++17` 以降の MSVC が C5033 を報告します。

> [!NOTE]
> C17 の `register` は「アクセスをできるだけ速くすることを提案する」指定であり、提案がどの程度有効かは処理系定義です (N1570 6.7.1)。  
> 現在のコンパイラはレジスター割り当てをレジスター アロケーターで決めるため、この提案は最適化結果を変えません。  
> C++ では C++11 で非推奨となり、C++17 で言語から削除されました (P0001R1)。キーワード自体は将来の転用に備えて予約されたまま残ります。  
> GNU 拡張の明示レジスター変数 (`register int *p asm("r12");`) だけは `register` キーワードを必須とします。GCC 専用であり、Linux/GCC と Windows/MSVC の双方で動作する構成を維持する要件を満たさないため、本規範では使用しません。

### 検証

次の検索結果をレビューし、使用場所と理由が規則に合っていることを確認します。

```bash
rg -n '\b(restrict|_Restrict)\b|\bvolatile\b|\b(static[[:space:]]+)?inline\b|\bFORCE_INLINE\b' app \
  --glob '*.{c,h}' \
  --glob '!app/<external-module>/**' \
  --glob '!**/obj/**' --glob '!**/packages/**'
```

`restrict` と強制インライン化には、レビュー資料として GCC と MSVC の計測結果を添付します。  
`volatile` は許容用途、同期機構、根拠コメントの有無を確認します。

`register` は、`eventlog-register` のようなコマンド名、「register 系 API」のような日本語本文、`failed to register` のような英語メッセージにも現れます。  
記憶域クラス指定子としての使用だけを拾うため、直後に識別子が続く形へ絞り込み、grep は補助として目視を併用します。

```bash
rg -n '(^|[^-[:alnum:]_])register[[:space:]]+[A-Za-z_]' app \
  --glob '*.{c,h,cc,cpp,hpp}' \
  --glob '!app/<external-module>/**' \
  --glob '!**/obj/**' --glob '!**/packages/**'
```

## 関数引数の const 付与と Doxygen 方向タグ

対象ワークスペースのすべての C コード (`app/` 配下) で、関数引数には次のルールで `const` を付与し、Doxygen の `@param[in/out/in,out]` を厳密化します。新規関数だけでなく、既存関数の変更時にも適用します。

### 引数の分類

各引数は **意味的方向 (Doxygen タグ)** と **物理的種別 (型)** の 2 軸で分類します。

| 意味的方向 | 説明 |
| --- | --- |
| `[in]` | 関数は値を読むだけで、関数復帰後の中身は呼び出し元視点で変わらない |
| `[out]` | 関数復帰後に初めて意味のある値が書き込まれる (初期化前の値は見ない) |
| `[in,out]` | 関数復帰前後の両方で意味があり、関数が書き換えうる |

| 物理的種別 | 例 |
| --- | --- |
| ポインター引数 | `T *p`, `const T *p`, `T **pp`, 配列 `T arr[]` |
| 値渡し引数 (リテラル) | `int n`, `size_t len`, `enum E e` |

**opaque handle (`sample_context *handle` 等) / 同期プリミティブ (mutex, rwlock, condvar, thread, lock, once_flag) / `FILE *` は、関数本体で内部状態を変更しても常に `[in]` とします。**  
`_dispose` / `_close` / `_stop` 系も例外なく `[in]` です。

唯一の例外は、**opaque handle を生成して呼び出し元へ返す出力引数** です。  
`sample_context **context_out` のように、関数復帰後に呼び出し元がハンドルそのものを受け取る場合は `[out]` とします。

```c
/* [out]: ハンドルを生成して返す */
int sample_context_open(const char *path, sample_context **context_out);

/* [in]: 生成済みのハンドルを操作する (内部状態が変わっても [in]) */
int sample_context_write(sample_context *context, const void *data, size_t size);
void sample_context_close(sample_context *context);
```

`[in,out]` を使うのは、呼び出し元が関数復帰後に同じポインター経由でデータを読み戻す場合に限ります。

> [!NOTE]
> opaque handle を `[in]` で統一するのは、**ハンドルの内部実装が利用者から見えない** ためです。  
> 利用者はメンバーを直接読み戻すことができず、受け取ったハンドルを再度 API へ渡すだけです。  
> 関数が内部状態を変更しても、それは利用者から観測できる「引数の方向」には現れません。  
> したがって、ハンドルを生成して返す場合を除き、方向タグは `[in]` に定まります。
>
> これは Doxygen 一般の意味論 (引数が指すデータの方向を表す) とは異なる、本リポジトリのローカル規約です。  
> 外部から来た読み手が誤りと判断しないよう、opaque handle を新設する際は本節を参照してください。

| `[in,out]` を使う例 | 理由 |
| --- | --- |
| `strcat` の `dest` | 内容を append、呼び出し元が後で文字列として読む |
| `path_normalize` の `path` | in-place 編集、呼び出し元が後で読む |
| `compress`/`crypto` の `dst_len` | 入力: バッファー サイズ / 出力: 必要サイズ |
| `file_open` の `file` | 既存ハンドル状態を読んで再オープン判定 + 新規オープン結果を書き込む |

物理的な mutex 取得や内部状態変更は `@par スレッド セーフ` で言及し、`[in/out]` には反映しません。

### ポインター引数の const 付与判定

**手順** (ヘッダー宣言と impl 定義の両方に const を付ける):

1. 引数の意味的方向を決定します。`[out]` または `[in,out]` ならば const は付けません。
2. `[in]` の場合、impl (Linux/Windows 両方) を読み、次のいずれかに該当するか確認します。

   | 項目 | 該当時 |
   | --- | --- |
   | `*p = ...` または `p->member = ...` の代入がある | const **不可** |
   | `p` または `&p->member` を書き換え系 OS API に渡す (`fstat` の出力先など) | const **不可** |
   | `p` を内部 mutex / rwlock / condvar の取得対象として渡す (`pthread_mutex_lock`、`EnterCriticalSection`、利用するライブラリの同期プリミティブ API 等) | const **不可** (mutable 扱い) |
   | 上記いずれにも該当しません。 | const **可** |

3. const 可と判定したら、**ヘッダー宣言と impl 定義の両方** に `const T *` を付けます。

宣言と定義での修飾子・マクロの配置一覧は [宣言と定義の関係](#宣言と定義の関係) を参照します。

**例外**:

- `FILE *` 引数には慣習として `const` を付けません (C 標準 stdio に合わせる)。  
  Doxygen タグは上記の opaque handle ルールと同様に常に `[in]` とします。
- 不透明 handle で内部 mutex/rwlock を取得する関数は、論理的には read-only でも const を付けません。`-Wcast-qual` と整合させるためです。

**機械的フィルターの grep 例** (`<arg>` は引数名、`<dir>` は対象ディレクトリ):

```bash
# ポインター引数の書き換え検査
grep -nE '(\*[[:space:]]*<arg>[[:space:]]*=|<arg>->[a-z_]+[[:space:]]*=|\&<arg>->[a-z_]+)' <dir>/*.c

# 内部 lock 取得検査 (利用するライブラリの同期プリミティブ API があればパターンに追加する)
grep -nE '(pthread_mutex_lock|EnterCriticalSection)' <dir>/*.c
```

> [!IMPORTANT]
> grep は補助です。最終判定は impl の手読みで行います。

### 二重ポインターと const

1 段のポインターでは `T *` から `const T *` への暗黙変換は安全です。  
2 段では `T **` から `const T **` への暗黙変換はできません。  
これは不便さではなく、const を破る経路を塞ぐ言語規則です。

```c
const T c = ...;
T *p;
const T **pp = &p; /* もしこれが許されると */
*pp = &c;          /* p が const オブジェクトを指す */
*p = ...;          /* const を非 const 経由で書き換える → 未定義動作 */
```

#### 型の読み方

| 型 | 意味 | 典型用途 |
|---|---|---|
| `T **` | 内側も外側も書き換え可 | ハンドル出力 `T **out`、可変なポインター配列 |
| `const T **` | 「`const T *` へのポインター」。**スロットは書き換え可**、指す先の `T` は const | 解析結果として `const char *` を格納する先 |
| `T *const *` | 「`T *` への const ポインター」。**スロットは書き換え不可**、指す先の `T` は非 const 可 | 読み取り専用の `argv` 風配列 |
| `const T *const *` | スロットも指す先も書き換えない読み取り専用のポインター配列 | 読み取り専用の `const char *` 配列を渡す API |

> [!IMPORTANT]
> `const T **` と `const T *const *` は別物です。  
> 「読み取り専用のポインター配列」を表したいときに `const T **` と書くと、変換不能と危険なキャストの両方に突き当たります。

#### 用途ごとの型の選び方

| 用途 | 仮引数の型 | Doxygen の方向 (目安) |
|---|---|---|
| ハンドルや領域を生成して返す | `T **out` (const なし) | `[out]` |
| 関数が `const T *` をスロットへ書き込む | `const T **` | `[out]` |
| ポインター配列を読むだけ (指す先も読んだだけ) | `const T *const *` | `[in]` |
| ポインター配列のスロットは固定、指す先は非 const 可 (argv 形) | `T *const *` | `[in]` |

```c
/* 望ましい: 読み取り専用のポインター配列 */
void sample_print_names(const char *const *names, size_t count);

/* 望ましい: argv 形 (スロット非書き換え) */
int sample_parse_args(int argc, char *const *argv);

/* 望ましい: 出力スロット。呼び出し元は const char * 変数のアドレスを渡す */
int sample_get_name(const char **name_out);

/* 望ましい: ハンドル出力 */
int sample_context_open(const char *path, sample_context **context_out);
```

本リポジトリの既存例:

- `char *const *argv` — `cplat_argparser_init` など
- `cplat_sym_loader_entry *const *` — `cplat_sym_loader_init` など
- `const char **storage` — 文字列オプションの出力スロット (`argparser` の register 系)

#### 禁止する書き方

ビルドを通すために `T **` を `const T **` へキャストしてはなりません。

```c
/* 望ましくない: 読み取り専用配列なのに const T ** とし、呼び出し側でキャスト */
void sample_print_names(const char **names, size_t count);
char *items[] = {"a", "b"};
sample_print_names((const char **)items, 2); /* 危険な方向のキャスト */
```

API 側の型を `const T *const *` または `T *const *` に修正するか、呼び出し側で適切な型の配列・一時変数を用意します。

> [!WARNING]
> `T **` → `const T **` のキャストは、前述の const 破り経路を自分で開く行為です。  
> 「コンパイラを黙らせるためのキャスト」として使わないでください。

> [!NOTE]
> `T **` → `const T **` の禁止は C と C++ で共通です。  
> 安全側の多重 const (`const T *const *` など) への暗黙変換は、C++ では通ることが多く、C では通らないことがあります。  
> 公開 C API の型は C の規則を正とし、暗黙変換に頼らず用途に合った型を宣言します。  
> 値渡し引数の top-level const (宣言と定義で付ける位置を分ける規則) とは層が異なります。本節はポインターが指す先の修飾を扱います。

#### 既存コードへの適用

- 新規関数では、用途に応じて上表の型を最初から選びます。
- 既存関数の const 化では、二重ポインターに当たったら本節で型を決め直します。危険なキャストで通さないでください。
- すでに正しい既存 API (`char *const *`、出力用の `const char **` など) を、本節を理由に一括で書き換えません。変更機会に合わせて誤用だけを直します。

### 値渡し引数 (リテラル) の const 付与判定

値渡し引数は **impl 側 (定義) のみ** に `const` を付けます。ヘッダー宣言には付けません。

```c
/* ヘッダー (宣言) */
int foo(int n);

/* impl (定義) */
int foo(const int n)
{
    return n * 2;
}
```

宣言と定義での修飾子・マクロの配置一覧は [宣言と定義の関係](#宣言と定義の関係) を参照します。

> [!NOTE]
> 宣言と定義で扱いを分けるのは、**安全性の観点** によります。
>
> - **宣言に付けると呼び出し元へ波及する**。ヘッダーは契約の表明であり、mock 宣言、シグネチャを写している箇所、利用側のドキュメントがすべて追従の対象になります。値渡し const は呼び出し元の書き方を何ら制約しないため、追従の手間に見合いません
> - **定義に付けないと関数内の意図せぬ変更を招く**。引数を作業用変数として書き換えても、コンパイラは何も言いません。定義側の const は、この書き換えをビルド時に止める唯一の手段です
>
> 定義側のみに付けることで、呼び出し元への波及を避けながら、関数内の不慮の書き換えを防げます。

> [!NOTE]
> C 言語では top-level の値渡し const は関数の型に含まれず、ABI にも宣言と定義の互換性にも影響しません。  
> このため、宣言と定義で修飾が異なっていてもコンパイル エラーにはなりません。  
> clang-tidy の `readability-avoid-const-params-in-decls` も **宣言側** の top-level const だけを警告する仕様であり、本節の方針は静的解析の既定とも整合します。

**手順**:

1. 引数の意味的方向を決定します。値渡しで `[in,out]` 相当が必要ならポインター化を検討します (ABI 変更を伴うため別 commit 推奨)。
2. 関数本体で引数を読むだけなら、impl 側に `const` を付けます。
3. 再代入 (`n = ...`、`n++`、`n--`、`n += ...` 等) があれば、[[in] 引数は関数内部で更新しないことを原則とする](#in-引数は関数内部で更新しないことを原則とする) に従って impl を整理してから const を付けます。

**機械的フィルターの grep 例**:

```bash
grep -nE '\b<arg>[[:space:]]*(\+\+|--|=|\+=|-=|\*=|/=|%=)' <dir>/*.c
```

### [in] 引数は関数内部で更新しないことを原則とする

意味的に `[in]` の引数は関数内部で更新しません。  
再代入やループ カウンターとして使い回しているコードは、const 化対応の一環として impl を整理します。

> [!NOTE]
> 引数の使い回しは const を付けられない技術的な理由であると同時に、読み手が「引数の元の値」を追えなくなる可読性の問題でもあります。

```c
/* 望ましくない: in 引数 n を使い回し */
int foo(int n)
{
    while (n > 0)
    {
        process(n);
        n--;
    }
    return 0;
}

/* 望ましい: ローカル変数で作業し、引数は const */
int foo(const int n)
{
    int remaining = n;

    while (remaining > 0)
    {
        process(remaining);
        remaining--;
    }

    return 0;
}
```

「真に書き換えが必要」と判定された場合は Doxygen を `[in,out]` に修正します。  
値渡しで `[in,out]` の場合はポインター化を検討します (ABI 変更を伴うため別 commit にします)。

### Doxygen @param[in/out/in,out] の厳密化

const 付与とセットで、Doxygen タグも impl の挙動に合わせて見直します。

**典型的な誤りパターン**:

| 実際の挙動 | 誤り | 正しいタグ |
| --- | --- | --- |
| 出力バッファー (初期化前の値を見ない) | `[in]` が付いている | `[out]` |
| in-place データ編集 (呼び出し元が復帰後に読む) | `[in]` のみ | `[in,out]` |
| opaque handle への操作 (`_dispose`/`_write`/`_set_*` 等) | `[in,out]` が付いている | `[in]` |

### 適用範囲と作業の進め方

- 新規関数では本ルールに従って最初から const と Doxygen タグを正しく付けます。
- 既存関数を変更する際は、変更ファイル内の関数全てに本ルールを適用します。
- 大規模な const 化リファクタリングを行う際は、**1 commit = 1 ヘッダー (カテゴリ)** 単位で進めます。ヘッダー変更、impl 変更、対応する mock 追従、Doxygen タグ修正を同じ commit にまとめます。

> [!NOTE]
> 1 commit にヘッダーと impl と mock をまとめるのは、`-Wcast-qual` 警告の発生箇所が commit 内に局所化され、レビューが容易になるためです。

### 検証

各 commit / PR で次を確認します。

```bash
# Linux ビルド (-Wcast-qual で警告 0 を確認)
cd <module-dir> && make 2>&1 | grep -E 'warning|error'

# 局所テスト
cd <module-dir> && make test

# Doxygen 警告チェック (該当ターゲットがあれば)
cd <module-dir> && make doxy 2>&1 | grep -i warning
```

全体リファクタリング完了後は、他モジュールへの影響を確認するため、ワークスペースの許可条件に従ってルートの `make` / `make test` を実行します。

> [!NOTE]
> 非 const から const への変更は、呼び出し側で暗黙変換が可能なため、通常は他モジュールへ影響しません。

### 既存の模範例

本ルールに沿っている既存ヘッダーの一覧は、各 app のドキュメントに模範例として記載します。  
新規実装時の参考には、対象 app の特化事項ドキュメントを参照してください。

### mock 追従 (test 配下を持つモジュールの場合)

ヘッダーの const 化などでシグネチャを変更する場合は、対応する mock の追従を同じ変更に含めます。  
Doxygen 本文だけの変更では mock のシグネチャ変更を必要としません。

`delegate_real_*` 宣言、`MOCK_METHOD(...)` 宣言、および `MOCK_WEAK_IMPL(...)` の引数型を、ヘッダー宣言と完全一致させます。

> [!NOTE]
> `ON_CALL(...).WillByDefault(Invoke(delegate_real_*))` および `EXPECT_CALL(...)` の matcher は型推論で追従するため、通常は無修正で済みます。  
> ただし `Matcher<T*>` のように明示的に型を指定している箇所があれば、あわせて修正が必要です。

## スレッド安全性の Doxygen 記載

公開 API のスレッド安全性は、利用者が必要な排他制御を判断できる内容で宣言します。  
Doxygen の `@par` は任意の表題を持つ段落を作るコマンドであり、それ自体はスレッド安全性の意味を定義しません。

### 適用範囲

`prod/include/` で宣言する新規または変更対象の公開関数には、`@par スレッド セーフ` を記載します。  
`prod/include_internal/` の関数は、共有状態、ハンドル、オブジェクトの寿命、コールバックなど、並行実行に関する契約がある場合に記載します。  
`static` 関数には記載しません。

既存の公開関数は、宣言または実装を変更する際に本規則を適用します。  
既存 API 全体の記載を変更する監査は、機能変更とは別の変更で実施します。

### 分類

関数を次の三つに分類します。

| 分類 | 記載する条件 |
|---|---|
| スレッド セーフ | 呼び出し側が追加の排他制御を行わずに、対象となる同時呼び出しを実行できます。 |
| 条件付きスレッド セーフ | ハンドル、引数、呼び出し順など、明示した条件を満たす場合だけ同時呼び出しを実行できます。 |
| スレッド セーフではありません。 | 対象となる呼び出しを呼び出し側で直列化する必要がある |

「対象となる同時呼び出し」は、同じ関数だけでなく、同じ状態へアクセスする別の API との同時呼び出しも含みます。  
分類を判断する際は、次の項目を確認します。

- 同一ハンドルへの同時呼び出し
- 異なるハンドルへの同時呼び出し
- グローバル状態またはライブラリ内共有状態へのアクセス
- 引数が指す領域とハンドルの寿命
- コールバックからの再入
- 呼び出し側に必要な mutex などの排他制御

条件付きスレッド セーフでは、該当する条件と禁止する同時操作を本文に列挙します。  
「条件付きスレッド セーフです」だけでは利用者が排他制御を判断できないため、その一文だけで記載を終えません。

再入可能性、非同期シグナル安全性、ロック フリー性は、スレッド安全性とは別の性質です。  
これらを保証する API だけが、`@par スレッド セーフ` とは別の段落で保証範囲を記載します。

### 記載例

スレッド セーフな関数は、次のように記載します。

```c
/**
 * @par スレッド セーフ
 * 本関数はスレッド セーフです。
 */
```

条件付きスレッド セーフな関数は、条件と必要な排他制御を記載します。

```c
/**
 * @par スレッド セーフ
 * 異なるハンドルに対する呼び出しは、同時に実行できます。
 * 同一ハンドルに対する呼び出しは、呼び出し側で直列化してください。
 */
```

スレッド セーフではない関数は、直列化する範囲を記載します。

```c
/**
 * @par スレッド セーフ
 * 本関数はスレッド セーフではありません。
 * 本関数と同じ共有状態へアクセスする API の呼び出しを、呼び出し側で直列化してください。
 */
```

「スレッド セーフではない」という分類だけを理由に、動作が未定義であるとは記載しません。  
データ競合やオブジェクトの寿命違反によって動作が未定義になる場合は、未定義になる具体的な条件を記載します。

### 検証

公開関数の Doxygen と `@par スレッド セーフ` を抽出し、宣言ごとの分類と条件をレビューします。

```bash
rg -n '@par[[:space:]]+スレッド[[:space:]]+セーフ|^[A-Z][A-Z0-9_]*(?:_EXPORT|_API).*\(' app \
  --glob '**/prod/include/**/*.h' \
  --glob '!app/<external-module>/**'
```

条件付きスレッド セーフな関数では、同一または異なるハンドル、共有状態、寿命、再入、呼び出し側の排他制御のうち、実装に関係する条件が本文にあることを確認します。  
Doxygen を生成する変更では、対象 app の `make doxy` を実行し、警告がないことを確認します。

## 宣言と定義の関係

公開 API の関数では、修飾子とマクロを **宣言 (ヘッダー)** と **定義 (impl の `.c`)** のどちらに置くかを次のとおり統一します。  
宣言を契約の単一の源とし、定義側の重複を排します。

| 対象 | 宣言 (ヘッダー) | 定義 (.c) |
| --- | --- | --- |
| エクスポート / 呼び出し規約マクロ (`EXAMPLE_EXPORT` / `EXAMPLE_API` 等) | 付ける | 付けない |
| 値渡し引数の top-level const (`const int n`) | 付けない | `[in]` に付ける |
| ポインター参照先の const (`const T *p`) | 付ける | 付ける |

`example` の例を示します。

宣言 (ヘッダー):

```c
EXAMPLE_EXPORT extern int EXAMPLE_API example_handler(int kind, int a, int b, int *result_out);
```

定義 (`.c`):

```c
/* Doxygen コメントは、ヘッダーに記載 */

int example_handler(const int kind, const int a, const int b, int *result_out)
{
    /* ... */
}
```

値渡し const とポインター const の配置理由は、それぞれ [値渡し引数 (リテラル) の const 付与判定](#値渡し引数-リテラル-の-const-付与判定) と [ポインター引数の const 付与判定](#ポインター引数の-const-付与判定) を参照します。

> [!NOTE]
> エクスポート / 呼び出し規約マクロを定義側に付けないのは、次の理由によります。
>
> - MSVC は先行する宣言から `__declspec(dllexport)` と `__stdcall` を継承します。
> - GCC/Clang は、定義が公開ヘッダーの先行宣言 (visibility 付き) と整合するとき、その可視性を定義へ適用します。
> - `.c` は対応する公開ヘッダーを include 済みである (例: `example_handler.c` は `<example/example_spec.h>` を include)。このため定義側にマクロを重ねても情報が重複するだけで、新たな意味を持たない
> - 重複を排し、宣言を唯一の契約源とすることで保守性が上がる
> - Windows と Linux で「宣言に付け、定義に付けない」を統一します。

### 共有ライブラリのシンボル可視性

公開境界は [命名規則](#命名規則) のヘッダー配置と接頭辞に加え、**共有ライブラリの動的シンボル表** でも表します。

| 項目 | Windows (MSVC) | Linux (GCC/Clang) |
|---|---|---|
| 既定の動的エクスポート | 明示 `dllexport` のみ | 既定は default (全部出やすい) |
| 本リポジトリの対策 | `*_EXPORT` → `dllexport` / `dllimport` | 共有ビルドで `-fvisibility=hidden` + `*_EXPORT` → `visibility("default")` |
| 印の置き場所 | 宣言のみ (`*_EXPORT`) | 宣言のみ (同上) |

規則:

1. 動的シンボル表に載せてよいのは、`prod/include/` の公開 API に `*_EXPORT` を付けたシンボルだけとします。
2. `include_internal/` で宣言する関数・変数に `*_EXPORT` を付けない。同一ライブラリ内の外部リンケージは持ってよいが、default 可視にはしません。
3. Linux のライブラリ ビルドは、makefw が `-fvisibility=hidden` を付与する (static の `.a` を含みます。shared へ静的リンクしたときの漏れ防止)。公開 API は `*_EXPORT` により default 可視になります。
4. 静的リンク (`PREFIX_STATIC`) では `*_EXPORT` は空に展開する (Windows / Linux 共通)。
5. ビルドを通すためだけに内部シンボルへ default 可視や `dllexport` を付けません。
6. export テーブルと実際の動的シンボルは、Windows / Linux とも **不足と想定外の完全一致** で検査する (`exportTest` / `expectExportNamesMatch`)。Linux のリンカー合成シンボル (`__bss_start` 等) のみ検査対象外とします。
7. OSS (`lua` / `sqlite` / `cjson`) は各 app の既存ビルド方針を維持します。export 検査の厳格化は共通フレームワーク側の挙動であり、OSS の製品ソースやビルド手順の改変は行いません。

> [!IMPORTANT]
> `<lib>_internal_` という **名前** だけでは、Linux の `.so` からシンボルが消えません。  
> 動的エクスポートを閉じるには、hidden 既定と公開印 (`*_EXPORT`) の組み合わせが必要です。

> [!WARNING]
> テストが共有ライブラリ (`LIBS += <lib>`) と、同じライブラリに含まれる `.c` の `TEST_SRCS` / `ADD_SRCS` を **同時に** リンクしてはなりません。  
> 既定可視性ではシンボル介入 (interposition) で偶然 1 つの TLS に見えていたものが、hidden 化後は **TLS や静的状態が二重化** し、失敗します。  
> 共有ライブラリをリンクするテストでは、当該 `.c` を `TEST_SRCS` / `ADD_SRCS` に入れず、公開 API 経由で検証します。  
> `TEST_SRCS` をやめたあとに残ったシンボリック リンクは手動削除が必要です (makefw の TEST_SRCS 留意事項を参照)。

> [!NOTE]
> 実装の中心は `cplat/base/dll_exports.h` の `CPLAT_DLL_EXPORT` と、`framework/makefw` の `makelibsrc_c_cpp.mk` です。  
> 各 app の `{lib}_export.h` はそれを薄く包みます。

### 検証

配置ルール (定義側に EXPORT を付けないこと) の最終確認は MSVC ビルド (`Start-VSCode-With-Env.cmd` 環境) で行います。  
Linux では `*_API` が空でも、`*_EXPORT` は共有ビルドで visibility を持つため、公開 API の漏れは動的シンボル表とリンクで検出できます。

```bash
# Linux: 共有ライブラリの動的シンボル (公開 API が載り、内部ヘルパーが載らないこと)
nm -D --defined-only app/<lib>/prod/lib/lib<name>.so | head

# exportTest (不足と想定外の完全一致)
cd app/<lib>/test/.../exportTest && make test
```

> [!WARNING]
> 定義側に誤って `*_EXPORT` を付けても、Linux ではビルドが通ることがあります。  
> 配置ルールの逸脱は差分レビューと MSVC ビルドで確認してください。

## API 設計における概念の分離

### 基本ルール

用途が異なる属性は、暗黙に共有させず独立した概念として設計します。

| ルール | 内容 |
|---|---|
| 属性の独立 | 用途が異なる属性 (例: インスタンス名 / インスタンス識別番号 / 出力ファイル名 / ファイル識別番号) は別フィールド・別 setter にします。 |
| getter の提供 | setter を作る属性には、対応する getter (確認手段) も用意します。 |
| デフォルト値の独立 | ある属性のデフォルト値は、他の属性の設定に影響されない形で定義します。 |
| 排他の区別 | プロセス間排他とプロセス内排他は別概念として扱います。占有モードでもプロセス内は調停して同一資源への出力をサポートします。 |

> [!NOTE]
> 概念を暗黙に結合すると、ライブラリが識別名を設定した瞬間に出力ファイル名まで変わるなど、利用者の意図しない副作用が生じます。  
> 識別番号も同様で、用途 (OS トレースの識別 / ファイルの分離) が異なるなら共有しません。

### 判定手順

1. 新しい setter を追加するとき、その値が既存のどの属性のデフォルト値・導出値に影響するかを列挙します。
2. 影響がある場合、その影響が利用者の期待どおりか検討します。期待と異なり得るなら属性を分離します。
3. 分離した各属性に getter を用意します。

## Doxygen コメントのプレースホルダー表記

### 基本ルール

Doxygen コメント内で可変部分 (プレースホルダー) を表すときは、山括弧 `<` `>` ではなく波括弧 `{` `}` を使用します。

```c
/* NG: Doxygen が <ファイル名> を XML/HTML タグと解釈する */
/**
 *  デフォルト パスは log/<ファイル名>.log です。
 *  識別名は @c <name>-<identifier> です。
 */

/* OK: 波括弧であればタグと解釈されない */
/**
 *  デフォルト パスは log/{ファイル名}.log です。
 *  識別名は @c {name}-{identifier} です。
 */
```

> [!WARNING]
> Doxygen は `<...>` を XML/HTML タグとして解釈し、`warning: Unsupported xml/html tag <ファイル名> found` 警告を出力します。  
> この解釈は XML 出力にも影響し、XML を入力とする Doxybook2 の Markdown 変換が正しく行えなくなります。  
> `@c` の指定やバッククォートのコード スパンの外にある場合は、日本語のプレースホルダーでも警告の対象です。

### 適用範囲

- Doxygen コメント (`/** */`) 内のすべてのプレースホルダー表記に適用します。
- 通常の C コメント (`/* */`) は Doxygen の処理対象外ですが、将来の Doxygen 化やコピーを考慮して `{}` に統一します。
- 関数テンプレート構文 (`template <typename T>` など) をコード ブロック (`@code` / バッククォート) 内に書く場合は対象外です。コード ブロック内の `<` `>` はタグと解釈されません。

### 検証

`make doxy` 実行後に生成される `doxy*.warn` で「Unsupported xml/html tag」が検出されないことを確認します。

```bash
grep "Unsupported xml/html tag" <module-dir>/doxy*.warn
```

> [!IMPORTANT]
> `doxy*.warn` は生成物です。警告が残っていても手では編集せず、コメント側を修正して再生成してください。

## Doxygen コメントの @p などコマンド引数と日本語句読点の間隔

### 基本ルール

`@p` `@c` `@a` `@b` `@e` `@em` `@ref` など、空白区切りの 1 語を引数に取る Doxygen コマンドでは、引数の直後に日本語句読点 (`、` `。` `，` `．`) を続ける場合、引数と句読点の間に半角スペースを入れます。

**NG: 句読点が引数に取り込まれる**

`@return 成功時は @p buf、EOF またはエラー時は NULL を返します。`

**OK: 半角スペースで区切る**

`@return 成功時は @p buf 、EOF またはエラー時は NULL を返します。`

> [!WARNING]
> `@p` などのコマンドは、空白区切りの次の 1 トークンをまるごと引数として読み取ります。  
> 引数と日本語句読点の間に空白がないと、句読点が引数に取り込まれ、意図しない書式や `@ref` のリンク解決失敗を招きます。  
> Doxygen はこのケースで警告を出さないため、生成物 (XML/HTML) を確認しない限り気付けません。

> [!NOTE]
> Doxygen 1.15.0 の XML 出力で、実際に不正な入れ子となることを検証済みです。

### 適用範囲

- `@p` `@c` `@a` `@b` `@e` `@em` `@ref` の直後に `、` `。` `，` `．` が続く箇所すべてに適用します。
- コマンドの手前に句読点が続く場合 (`、@p` など) は Doxygen が正しく解釈するため対象外です。

### 検証

`framework/docsfw/bin/text_style_jp.py` の `doxygen-inline-arg-punctuation-spacing` ルールで検出できます。

```bash
python framework/docsfw/bin/text_style_jp.py <対象ファイル> --dry-run
```

## Doxygen コード例内のコメント形式

### 基本ルール

Doxygen コメント (`/** */`) 内の `@code` ~ `@endcode` に書くコード例では、コメントに `/* */` を使用せず `//` を使用します。

```c
/* NG: 「既定値」の後のコメント終端記号が、外側のドキュメント コメントを終端させる */
/**
 *  @par 使用例
    @code{.c}
    int count = 1; /* 既定値 */
    @endcode
 */

/* OK: // であればドキュメント コメントは終端しない */
/**
 *  @par 使用例
    @code{.c}
    int count = 1; // 既定値
    @endcode
 */
```

> [!WARNING]
> コード例内の `*/` が外側のドキュメント コメントを途中で終端させ、以降のコード例が実コードとして解析されてビルド エラーになります。  
> clang-format も実コードと誤認し、コメント内容を再インデントする差分を提示します。

### 適用範囲

- ヘッダー・ソースの Doxygen コメント内の `@code` ブロックすべてに適用します。
- Markdown ファイル (`.md`) 内のコード フェンスは C コメントの外にあるため対象外です (`/* */` を使用できます)。

### 検証

コード例を追加・変更したモジュールがビルド エラーにならないこと、および `git clang-format --diff` がコメント内容の再インデントを提示しないことを確認します。

## 参照

- [`source-style-guideline.md`](source-style-guideline.md) - `.gitattributes` / `.editorconfig` / `.clang-format` によるソース スタイル維持
- [`include-guard-guideline.md`](include-guard-guideline.md) - インクルード ガード命名規則
- [POSIX の名前空間規定](https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_02_02) - `_t` で終わる型名が処理系用に予約されること
- [C の予約識別子 (cppreference)](https://en.cppreference.com/w/c/language/identifier) - アンダースコアで始まる識別子がファイル スコープで予約されること
- [C++ の識別子 (cppreference)](https://en.cppreference.com/w/cpp/language/identifiers) - 連続する `__` を含む識別子が処理系用に予約されること
- 各 app の特化事項ドキュメント (`docs/{app}/` 配下) - プラットフォーム抽象化、標準時刻型、共通結果コードなどの app 固有規則
- [POSIX `<stddef.h>`](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/stddef.h.html) - `size_t` / `ptrdiff_t` の定義
- [POSIX `<sys/types.h>`](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_types.h.html) - `ssize_t` / `off_t` / `time_t` の定義
- [Microsoft C ランタイムの標準型](https://learn.microsoft.com/ja-jp/cpp/c-runtime-library/standard-types?view=msvc-170) - MSVC における `size_t` / `ptrdiff_t` / `off_t` の定義
- [Microsoft C ランタイムの `fseek` / `_fseeki64`](https://learn.microsoft.com/ja-jp/cpp/c-runtime-library/reference/fseek-fseeki64?view=msvc-170) - Windows で 64 bit ファイル位置を扱う API
- MISRA C:2012 Dir 4.6 (`typedefs that indicate size and signedness should be used in place of the basic numerical types`) - 幅に依存する値へ実装定義幅の基本型を使わないこと
- [SEI CERT C INT00-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/integers-int/int00-c/) - 処理系のデータ モデルを理解し、仮定を静的表明で裏付けること
- [SEI CERT C INT01-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/integers-int/int01-c/) - オブジェクトのサイズを表す値に `size_t` を使うこと
- [Linux kernel coding style](https://www.kernel.org/doc/html/latest/process/coding-style.html) - ローカル変数を短く保つ方針、共通の後始末がある関数における goto の使用とラベル命名、および 14 章のメモリ確保の指針 (`sizeof(*p)` の推奨)
- [Linux Wireless の ath10k / ath11k / ath12k coding style](https://wireless.docs.kernel.org/en/latest/en/users/drivers/ath10k/codingstyle.html#status-error-variables) - 戻り値または状態コードを格納する変数名に `ret` を使う規定
- [BoringSSL API Conventions](https://boringssl.googlesource.com/boringssl/+/HEAD/API-CONVENTIONS.md) - cleanup を伴う C 関数で `int ret = 0;` と `goto err` を使い、自関数の成否を組み立てる例
- [C++ Core Guidelines ES.20](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es20-always-initialize-an-object) - 常時初期化を勧める側の一般則 (本規範の `ret` 未初期化は結果コード作業変数に限る例外)
- [SEI CERT C MEM12-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/memory-management-mem/mem12-c/) - 資源の確保と解放を伴う関数でエラー時に goto チェーンを使うこと
- MISRA C:2012 Rule 15.1 / 15.2 / 15.3 - goto の使用を戒める 15.1 は Advisory、前方ジャンプを求める 15.2 とラベル スコープを定める 15.3 が Required
- [Edsger W. Dijkstra, `Go To Statement Considered Harmful`](https://www.cs.utexas.edu/~EWD/transcriptions/EWD02xx/EWD215.html) - 非構造的なジャンプに対する批判
- [SEI CERT C MEM31-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/rules/memory-management-mem/mem31-c/) - 不要になった領域を解放すること
- [SEI CERT C MEM35-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/rules/memory-management-mem/mem35-c/) - オブジェクトに十分なメモリを確保すること (確保サイズの乗算オーバーフローを含む)
- [SEI CERT C MEM36-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/rules/memory-management-mem/mem36-c/) - `realloc` でオブジェクトのアラインメントを変更しないこと
- [SEI CERT C MEM01-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/memory-management-mem/mem01-c/) - `free` の直後にポインターへ新しい値を格納すること
- [SEI CERT C MEM04-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/memory-management-mem/mem04-c/) - 長さ 0 の確保に注意すること
- [SEI CERT C INT30-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/rules/integers-int/int30-c/) - 符号なし整数の演算を回り込ませないこと
- [SEI CERT C INT31-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/rules/integers-int/int31-c/) - 整数変換でデータと符号を失わせないこと
- [SEI CERT C INT32-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/rules/integers-int/int32-c/) - 符号付き整数の演算をオーバーフローさせないこと
- [SEI CERT C INT02-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/integers-int/int02-c/) - 整数変換規則を理解すること
- MISRA C:2012 Rule 10.1 から 10.8 (Essential Type Model) - 演算と代入における型の混在を制限する規則群。本規範は全面採用せず参考とします。
- [GCC の警告オプション](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html) - `-Wconversion` / `-Wsign-conversion` / `-Wsign-compare` の定義
- [MSVC のコンパイラ警告 C4244](https://learn.microsoft.com/ja-jp/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4244) - 縮小変換による値の欠落
- [MSVC のコンパイラ警告 C4245](https://learn.microsoft.com/ja-jp/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4245) - 符号付きと符号なしの不一致
- MISRA C:2012 Dir 4.12 / Rule 21.3 - 動的メモリの使用と `<stdlib.h>` の確保・解放関数の使用を禁止する規則 (いずれも Required)。組み込み向けの前提であり本規範では採用しません。
- [SEI CERT C PRE00-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/preprocessor-pre/pre00-c/) - 関数形式マクロよりインライン関数や静的関数を優先すること
- [SEI CERT C PRE01-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/preprocessor-pre/pre01-c/) - マクロ内の仮引数を括弧で囲むこと
- [SEI CERT C PRE02-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/preprocessor-pre/pre02-c/) - マクロ置換リスト全体を括弧で囲むこと
- [SEI CERT C PRE10-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/preprocessor-pre/pre10-c/) - 複数文のマクロを do-while で包むこと
- MISRA C:2012 Rule 20.7 - 関数形式マクロの仮引数を括弧で囲むこと (本規範の括弧規則の参考)
- [C++ FAQ: Const correctness](https://isocpp.org/wiki/faq/const-correctness) - `T **` を `const T **` へ変換できない理由と、`const T *const *` への誘導
- [SEI CERT C EXP05-C](https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/expressions-exp/exp05-c/) - const 修飾を捨てるキャストを行わないこと
- [ISO/IEC 9899:201x Committee Draft N1570](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf) - `restrict`、`volatile`、inline 関数、`register`、データ競合に関する C11 の公開委員会草案
- [SEI CERT C EXP43-C](https://wiki.sei.cmu.edu/confluence/spaces/c/pages/87151927/EXP43-C.+Avoid+undefined+behavior+when+using+restrict-qualified+pointers) - `restrict` の非 alias 契約へ違反して未定義動作を起こさないこと
- [SEI CERT C POS40-C](https://wiki.sei.cmu.edu/confluence/display/c/POS40-C.%2BDo%2Bnot%2Buse%2Bvolatile%2Bas%2Ba%2Bsynchronization%2Bprimitive) - `volatile` を同期プリミティブとして使用しないこと
- [Microsoft C キーワード](https://learn.microsoft.com/ja-jp/cpp/c-language/c-keywords?view=msvc-170) - MSVC の C11/C17 モードにおける `restrict`、`inline`、`_Atomic` のサポート状況
- [Microsoft `__restrict`](https://learn.microsoft.com/ja-jp/cpp/cpp/extension-restrict?view=msvc-170) - MSVC 拡張と標準 C の `restrict` の相違
- [GCC Inline](https://gcc.gnu.org/onlinedocs/gcc-9.4.0/gcc/Inline.html) - ISO C と GNU C における inline 関数の定義規則
- [Remove Deprecated Use of the register Keyword (P0001R1)](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0001r1.html) - C++17 で `register` を言語から削除し、キーワードを予約のまま残すこと
- [MSVC のコンパイラ警告 C5033](https://learn.microsoft.com/ja-jp/cpp/error-messages/compiler-warnings/c5033?view=msvc-170) - `/std:c++17` 以降で `register` が記憶域クラスとして扱われないこと
- [GCC Explicit Register Variables](https://gcc.gnu.org/onlinedocs/gcc/Explicit-Register-Variables.html) - `register` キーワードを必須とする GNU 拡張 (本規範では使用しません)
- [Doxygen Special Commands Reference](https://www.doxygen.nl/manual/commands.html#cmdpar) - `@par` が任意の表題を持つ段落を作ること
- [POSIX.1-2024 Definitions](https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap03.html) - thread-safe function の定義
