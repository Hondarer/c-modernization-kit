<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: claude code / セッション ID: session_01ACTERhUfA9PYy9ZXAEhsb4
上流文書: `261016-test-assert-comment-followup2-discussion.md` (検討用 Markdown)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 状態と手順の説明文と空ポインターの残課題への対応 最終仕様

ファイル名: `prompt/test-assert-comment-followup2-design.md`

版: 2

状態: 承認済み

## 目的と範囲

テストの確認コメントの残課題 (`prompt/test-assert-comment-followup-backlog.md` の BL-001〜BL-007) を解消します。状態と手順の説明文の文型を定めて既存のテストを移行し、説明文の空ポインターの表記、スマート ポインターの判定、記号の書き方、関数名の `()` の付け忘れの点検をそろえます。

対象は、リポジトリ本体と、テスト コードまたは規範を持つ全サブモジュールです。外部由来の配布物 (`framework/testfw/gtest/`、各 app の外部ライブラリのソース) と自動生成された成果物は対象外です。

Table: 残課題ごとの結論

| 課題 | 内容 | 結論 |
| --- | --- | --- |
| BL-001 | 状態と手順の説明文の書き方 | 注入、呼び出し、既定動作、待機の 4 種類の文型を定め、全テストを移行する |
| BL-002 | 説明文の NULL | C++ のテスト コードの説明文は一律 nullptr と書く |
| BL-003 | 関数名を動詞として使う説明文 | 呼び出しの文型に直す |
| BL-004 | スマート ポインターをそのまま条件にする書き方 | 空ポインターの規範の対象に含め、10 か所を直す |
| BL-005 | コメント中の単独の波括弧 | 記号の文字そのものは二重引用符で囲む。エスケープ シーケンスは C の文字列リテラルの形で書く |
| BL-006 | 空ポインターの規範の検証の精度 | 点検の仕組みは新設しない。既存の逸脱は一時的な点検で 1 回だけ探して直す |
| BL-007 | 関数名の `()` の付け忘れ | `text_style_jp.py` に診断を常設し、既存の付け忘れを直す |

あわせて、モックの委譲先と差し替え前のヘッダーを指す「本物」という語を、指す対象に応じた語へ言い換えます。テスト ファイルに残っている `text_style_jp.py` の既存の指摘も直します。

## 状態と手順の説明文の文型

`framework/testfw/docs/about-test-phase.md` の「状態と手順の説明文の表記」の節を、次の内容に改めます。

### 共通原則

`[状態]`、`[手順]`、`[Pre-Assert手順]` の説明文は、次の原則に従います。文型を定める 4 種類 (注入、呼び出し、既定動作、待機) 以外の説明文 (データの用意、値の書き込み、状態の変更など) は、この原則だけに従います。

- 文末を動詞の終止形 (「〜する。」) で終えます。
- 操作の対象を書きます。
- 関数名には引数なしの `()` を付け、識別子をバッククォートで囲みません (現行の規則)。
- 関数名は、コードで実際に呼んでいる関数の名前を書きます。関数名を動詞として使いません (「push() する」は使いません)。

`[状態]` は、テストの開始時点で用意されている状態を書きます。関数を呼び出す `[状態]` にも、[関数の呼び出し](#関数の呼び出し) の文型は適用しません。

```cpp
char buf[32];                     // [状態] - 出力バッファーを用意する。
ASSERT_EQ(0, compile("^b", &re)); // [状態] - パターン "^b" をコンパイルしておく。
```

### 注入 ([Pre-Assert手順])

`[Pre-Assert手順]` は、動作を注入する関数の名前を必ず書き、「f() から」で始めます。

Table: 注入の文型

| 注入する動作 | 文型 | 例 |
| --- | --- | --- |
| 戻り値を返す | f() から X を返す。 | cplat_malloc() から nullptr を返す。 |
| 呼び出しごとに戻り値を変える | f() から 1 回目は X、2 回目は Y を返す。 | cplat_fgets() から 1 回目は "first"、2 回目は "second" を返す。 |
| errno や出力引数を設定して戻り値を返す | f() から、〜を設定して X を返す。 | send() から、errno に EPIPE を設定して -1 を返す。cplat_ui_language_get_tag() から、出力バッファーに指定した表示言語タグを設定して CPLAT_OK を返す。 |
| 何もせずに戻る (戻り値なし) | f() から戻る。 | cplat_console_init() から戻る。 |
| 動作を伴って戻る (戻り値なし) | f() から、〜して戻る。 | freeaddrinfo() から、渡された解決結果を確かめて戻る。 |
| 元の処理へ委譲する | f() から 1 回目は X を返し、以降は〈委譲先〉へ委譲する。 | cplat_malloc() から 1 回目は nullptr を返し、以降は製品 (prod) の処理へ委譲する。 |

動詞は「返却する」ではなく「返す」を使います。委譲先の書き方は [委譲先と差し替え前のヘッダーの呼び方](#委譲先と差し替え前のヘッダーの呼び方) に従います。

```cpp
EXPECT_CALL(mock_unistd_, fsync(_, _, _, kFakeFd))
    .WillOnce(Return(0)); // [Pre-Assert確認_正常系] - fsync() が kFakeFd を指定して 1 回呼び出されること。
                          // [Pre-Assert手順] - fsync() から 0 を返す。
```

> [!NOTE]
> `results.md` の手順節には、`[手順]` と `[Pre-Assert手順]` の説明文だけが並んで出力されます。関数名がないと、どの関数がどの値を返すかを手順節だけでは判別できません。確認タグの「f() の戻り値」と同じく、関数を明記します。

### 関数の呼び出し ([手順])

文の主な操作が関数の呼び出しである `[手順]` (戻り値を変数で受ける文を含む) は、文末を「f() を呼び出す。」とし、テストの観点となる引数や状態を「〜を指定して」「〜の状態で」の形で前に置きます。引数をすべて列挙する必要はありません。

```cpp
int result = cplat_tracer_write_at(handle, CPLAT_TRACE_LEVEL_INFO, nullptr,
                                   "test message"); // [手順] - INFO レベルと "test message" を指定して cplat_tracer_write_at() を呼び出す。
int actual_ret_push = potr_internal_window_send_push(&win, &pkt); // [手順] - 満杯のウィンドウに 5 件目のパケットを指定して potr_internal_window_send_push() を呼び出す。
```

代入やループなど、関数の呼び出しでない `[手順]` は [共通原則](#共通原則) に従います。

### 既定動作 ([状態] の ON_CALL)

`ON_CALL` の `[状態]` は、既定動作の内容を「注入」の節の文型で書き、「ように既定動作を設定する。」を続けます。

```cpp
ON_CALL(mock_cplat, cplat_isatty(_)).WillByDefault(Return(0)); // [状態] - cplat_isatty() から 0 を返すように既定動作を設定する。
ON_CALL(mock_stdlib_, atexit(_, _, _, _))
    .WillByDefault(Invoke(delegate_real_atexit)); // [状態] - atexit() から libc の処理へ委譲するように既定動作を設定する。
```

> [!NOTE]
> `ON_CALL` は Arrange に置くため状態節に出力され、`[Pre-Assert手順]` の注入値とは別の節に出ます。既定動作の内容を書いても、注入値と並んで矛盾して読まれることはありません。

### 待機

プロセスの出力や終了を待つ説明文は、呼び出しの文型の例外とし、「〈プロセス〉が〈出力〉を出力するまで待機する。」「〈プロセス〉が終了するまで待機する。」と書きます。

```cpp
bool actual_ret_wait_hello =
    tryWaitForOutput(recv_h_, "Hello Porter", 3000); // [手順] - RECIEVER が "Hello Porter" を出力するまで待機する。
```

> [!NOTE]
> 待機の関数はテストの進行を合わせる手段で、読み手にとっての要点はどのプロセスの何を待つかです。説明文とコードの対応は、確認タグ (「"Hello Porter" を待つ tryWaitForOutput() の戻り値が true であること。」) で取れます。

### 説明文の空ポインターの表記

C++ のテスト コードでは、エビデンス タグの説明文 (`[状態]`、`[手順]`、`[Pre-Assert手順]`、確認タグの判定、条件、総合期待) の空ポインターを一律「nullptr」と書きます。C で書かれた製品コードの振る舞いを説明する場合も同じです。

```text
key に nullptr を指定して cplat_hashtable_get() を呼び出す。
handle が nullptr であること。(cplat_tracer_dispose() がハンドルを nullptr にしたことの確認)
```

### 記号の文字そのもの

説明文で記号の文字そのもの (波括弧など) を指す場合は、二重引用符で囲みます。エスケープ シーケンスは、C の文字列リテラルの形で二重引用符に入れて書きます。

```text
対を成さない "}" を含む書式を指定して cplat_internal_string_catalog_render_text() を呼び出す。
dest が "value={ 123 }" であること。("{{" と "}}" が 1 文字の波括弧になったことの確認)
"\x1B[1~" を指定して test_pinned_prompt_read_key() を呼び出す。
```

> [!NOTE]
> `text_style_jp.py` は、コメント中の二重引用符で囲んだ部分を整形の対象から外します。引用符の外にある単独の括弧は、文字そのものか括弧かを機械的に判定できず、括弧の内側の空白を除く整形の対象になります。

## 委譲先と差し替え前のヘッダーの呼び方

モックの委譲先と、override ヘッダーが読み込む差し替え前のヘッダーや API を、次のとおり呼びます。「本物」と「実際の処理」は使いません。

Table: 委譲先と差し替え前のヘッダーの呼び方

| 対象 | 製品コード (`prod`) | libc、OpenSSL などのライブラリ | 特定のライブラリに限らない説明 |
| --- | --- | --- | --- |
| 委譲先の処理 | 製品 (prod) の処理 | libc の処理、OpenSSL の処理 | 差し替え前の処理 |
| ヘッダー | 製品 (prod) のヘッダー | libc のヘッダー、OpenSSL のヘッダー | 差し替え前のヘッダー |
| API | 製品 (prod) の API | libc の API、OpenSSL の API | 差し替え前の API |

この呼び方を `framework/testfw/docs/how-to-mock.md` の「用語」の節に定め、`about-test-phase.md` の注入の文型から参照します。次の箇所の「本物」を言い換えます。

Table: 「本物」を言い換える箇所

| 場所 | 件数 (目安) |
| --- | --- |
| テスト コードの状態と手順の説明文 | 63 行 (製品コードへの委譲 43 行、OpenSSL 9 行、libc 11 行) |
| `framework/testfw/docs/how-to-mock.md`、`mock-injection-technique.md` | 21 か所 |
| `framework/testfw/include/` の mock ヘッダーのコメント | 7 か所 |
| `framework/testfw/libsrc/mock_libc/mock_fcntl.cc` のコメント | 1 か所 |
| テスト コードの通常のコメント (`argparserAllocFailureTest.cc`、`promptAllocFailureTest.cc`) | 2 か所 |
| `app/general/docs/testing-tutorial.md`、`coding-guideline.md` | 2 か所 |

`mock-injection-technique.md` は標準ライブラリの mock の説明のため、処理とヘッダーの「本物」は「libc の処理」「libc のヘッダー」になります。`how-to-mock.md` の用語定義など、libc、OpenSSL、app の関数の mock に共通する説明は「差し替え前の〜」と書きます。

## スマート ポインターの判定

`app/general/docs/coding-guideline.md` の「空ポインターの表記」で、「ポインターをそのまま条件にしない」規則がスマート ポインター (`std::shared_ptr`、`std::unique_ptr`) にも適用されることを明記し、望ましい例を加えます。

```cpp
// 望ましい: スマート ポインターも nullptr と比べる
if (handle == nullptr)
{
    return false;
}
int pid = (handle != nullptr) ? (int)handle->pid : -1;
```

次の 10 か所を直します。

Table: スマート ポインターをそのまま条件にしている箇所

| ファイル | 箇所 |
| --- | --- |
| `framework/testfw/libsrc/test_com/processController.cc` | `handle ? … : …` 2 か所、`if (!handle)` 4 か所 |
| `framework/testfw/libsrc/test_com/processController_linux.cc` | `if (!handle)` 1 か所 |
| `framework/testfw/libsrc/test_com/processController_windows.cc` | `if (!handle)` 1 か所 |
| `app/porter/test/src/integration/porterSendRecvTest/porterSendRecvTest.cc` | `if (send_h_)`、`if (recv_h_)` |

## ポインターをそのまま条件にする既存箇所の点検

一時フォルダーで、libclang (PyPI の `libclang`) による点検スクリプトを 1 回だけ実行します。スクリプトは、構文木の条件式 (`if`、`while`、`do`、`for` の条件、`!`、`? :` の条件、`&&` と `||` の各オペランド) のうち、型がポインターまたはスマート ポインターであるものを報告します。

- 対象: C と C++ の第一者管理のソース (製品コード、テスト コード、`framework/testfw`)。外部由来の配布物は除きます。
- コンパイル オプション: app ごとに、インクルード パス (`prod/include/`、`prod/include_internal/`、`framework/testfw/include`、gtest のヘッダーなど) とプラットフォームのマクロ (`PLATFORM_LINUX`) を推定して与えます。
- 報告された箇所は、C は `ptr != NULL` / `ptr == NULL`、C++ は `ptr != nullptr` / `ptr == nullptr` に直します。
- スクリプトはリポジトリに置きません。解析できなかったファイル (推定したオプションで型が解決できないもの) と、Windows のみの分岐 (`PLATFORM_WINDOWS`) は一覧にして報告し、残課題に記録します。

> [!IMPORTANT]
> この点検は既存コードの移行のための 1 回限りのものです。新しいコードの逸脱を機械的に検出する仕組みは設けないため、規範の検証節 (`grep` による候補の抽出) は変えません。

## 関数名の () の付け忘れの診断

`framework/docsfw/bin/text_style_jp.py` の C++ の処理に、診断 `test-comment-function-paren` を加えます。自動修正はしません。

1. ファイルのコード部分 (コメントと文字列リテラルを除く) から、直後に `(` が続く識別子を集めます。同じファイルで `(` が続かない位置にも現れる識別子 (変数、型、メンバーなど) と、キーワード (`if`、`sizeof` など)、すべて大文字の識別子 (マクロ)、2 文字以下の識別子は除きます。
2. エビデンス タグの説明文 (二重引用符と文字リテラルの中を除く) で、1. の識別子が `()` なしに現れる箇所を指摘します。直前が `.`、`->`、`::`、または直後が `.`、`::` の識別子 (`mmap.dat`、`string::npos` など) は指摘しません。

`--test` の自己テストに、指摘する例と指摘しない例を加えます。`framework/docsfw/docs/text_style_jp.md` の規則表に行を加えます。

## 既存コードの移行

Table: 既存コードの移行の対象

| 作業 | 対象 | 規模 (目安) |
| --- | --- | --- |
| `[Pre-Assert手順]` の文型 | 全テスト | 1,573 行 (関数名の追加 964 行を含む) |
| 関数を呼び出す `[手順]` の文型 | 全テスト | 約 4,000 行 (関数名を動詞として使う 28 行を含む) |
| `ON_CALL` の `[状態]` の文型 | 全テスト | 121 行 |
| 説明文の空ポインターの表記 | C++ のテスト | 約 1,280 行 (状態と手順 719 行、確認タグの条件と総合期待 560 行) |
| 「本物」の言い換え | テスト、testfw、`app/general/docs` | [「本物」を言い換える箇所](#委譲先と差し替え前のヘッダーの呼び方) の表 |
| スマート ポインターの判定 | `framework/testfw`、porter | 10 か所 |
| 記号の文字そのものとエスケープ シーケンス | string_catalog、string-catalog-sample、prompt のテスト | 26 行 |
| ポインターをそのまま条件にする既存箇所 | C と C++ のソース | 一時的な点検の結果による |
| 関数名の `()` の付け忘れ | 全テスト | 約 310 件 (`test-comment-function-paren` の指摘) |
| `text_style_jp.py` の既存の指摘 | テスト ファイル 64 ファイル | 397 件 |
| 規範の文書の例 | `about-test-phase.md`、`how-to-mock.md`、`app/general/docs/testing-tutorial.md`、`framework/testfw/docs/text_style_jp.md` など、変更した規則の例を含む文書 | 該当箇所 |

`text_style_jp.py` の既存の指摘は、識別子や固有名詞への誤検出でないことを 1 件ずつ確かめてから直します。誤検出と判断した指摘は直さず、一覧にして提示します。

## 作業の進め方

- 規則の文書 (`about-test-phase.md`、`how-to-mock.md`、`coding-guideline.md`) と `text_style_jp.py` の診断は、委譲せずに先に更新し、書き換えの手本にします。
- テスト コードの説明文の書き換えは、範囲 (app、テスト ディレクトリ) ごとに codex CLI (`gpt-6.1-sol`、reasoning effort low) と Claude のサブエージェント (Sonnet) に委譲し、同時に動かすのはそれぞれ 1 本までとします。委譲先には `make` と `clean` を実行させません。
- 同じファイルへの変更 (文型、空ポインターの表記、「本物」の言い換え、`()` の付け忘れ、既存の指摘) は、1 つの委譲の中でまとめて行います。

## 受入条件

1. テスト関数ごとの確認件数が、書き換えの前後で一致すること (今回の変更は確認件数を変えません)。
2. リポジトリ ルートの `make test` が失敗 0 件で、内容のある `.warn` がないこと。
3. `framework/docsfw` の `text_style_jp.py --test` が成功すること。
4. 変更した C/C++ のテストに `text_style_jp.py --mode cpp --dry-run` を実行し、指摘が 0 件であること (誤検出と判断して提示した指摘を除く)。`test-comment-function-paren` を含みます。
5. 説明文の文型を、変更の種類ごとに抽出して点検すること。`[Pre-Assert手順]` がすべて「f() から」で始まり、「返却」「本物」「実際の処理」が説明文に残っていないこと。C++ のテストの説明文に「NULL」が残っていないこと。
6. スマート ポインターの 10 か所と、一時的な点検で報告された箇所が直っていること。解析できなかったファイルの一覧を提示すること。
7. `git clang-format` の差分が残っていないこと。
8. 変更した Markdown が `text_style_jp.py --dry-run` で指摘なしであること。
9. 書き換えの差分を変更の種類ごとに抽出して点検し、判断に迷った箇所を一覧で提示すること。
10. サブモジュール、リポジトリ本体の順にコミットとプッシュを行い、CI が成功すること。

> [!NOTE]
> ルートの `make test` は、framework を依存順に並列で実行し、続けて `app` の `test` を実行します。同時に動かす `make` はこの 1 本だけとします。`text_style_jp.py --dry-run` は 1 ファイル 3 秒程度かかり、全テスト ファイル (839 ファイル) で 45 分程度です。

## 知見の反映計画

Table: 知見の反映計画

| 反映先 | 反映する内容 |
| --- | --- |
| `framework/testfw/docs/about-test-phase.md` | 状態と手順の説明文の共通原則と 4 種類の文型、説明文の空ポインターの表記、記号の文字そのものとエスケープ シーケンスの書き方。Arrange と Pre-Assert の節の例を新しい文型に直す |
| `framework/testfw/docs/how-to-mock.md` | 委譲先と差し替え前のヘッダーの呼び方 (用語の節) |
| `app/general/docs/coding-guideline.md` | 「空ポインターの表記」がスマート ポインターにも適用されること |
| `framework/docsfw/docs/text_style_jp.md` | `test-comment-function-paren` の規則 |

## 残課題として記録する事項

次は今回対応せず、`prompt/test-assert-comment-followup2-backlog.md` に記録します。

- 型を考慮した点検の仕組み (BL-006 の継続。新しいコードでポインターをそのまま条件にする逸脱を機械的に検出する方法)
- 一時的な点検で解析できなかったファイルと、Windows のみの分岐
- そのほか、作業中に判明して対応しなかった事項
