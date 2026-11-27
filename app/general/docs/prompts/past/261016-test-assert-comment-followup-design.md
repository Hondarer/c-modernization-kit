<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
上流文書: `261016-test-assert-comment-followup-discussion.md` (検討用 Markdown)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。
-->

# テストの確認コメントの残課題への対応 最終仕様

ファイル名: `prompt/test-assert-comment-followup-design.md`

版: 1

状態: 承認済み (2026-10-16 Tetsuo Honda)

## 目的と範囲

テストの確認コメントを判定の直接説明にした前案件で残った課題 (`prompt/test-assert-comment-backlog.md` の BL-001〜BL-011) を解消します。あわせて、C と C++ の空ポインターの表記の規範を新設し、既存コードを移行します。

対象のリポジトリは、リポジトリ本体と、テスト コードまたは規範を持つ全サブモジュール (`app/cplat`、`app/porter`、`app/cjson`、`app/zlib`、`app/sqlite`、`app/lua`、`framework/testfw`、`framework/docsfw`、`framework/makefw`) です。外部由来の配布物 (`framework/testfw/gtest/`、各 app の外部ライブラリのソース) は対象外です。

Table: 残課題ごとの結論

| 課題 | 内容 | 結論 |
| --- | --- | --- |
| BL-001 | Assert の中の操作 | 規則を追加し、操作を Act へ移す。`argparserTest.cc` の 24 テストはマルチ フェーズにする |
| BL-002 | 確認コメントのキャスト式 | 型をそろえるだけのキャストを省く |
| BL-003 | 引数条件付きの `.Times(0)` の文型 | 「X を指定した f() の呼び出しがないこと。」にする |
| BL-004 | nullptr と NULL の表記の揺れ | 空ポインターの規範を新設し、C++ は `nullptr` に移行する |
| BL-005 | 状態と手順の説明文の表記 | 関数名に `()` を付け、バッククォートを使わない |
| BL-006 | 点検での誤検出 | `text_style_jp.py` で引用符の中を整形の対象から外し、規則の点検を常設する |
| BL-007 | `gzopen()` の判定のタグ | `[確認_正常系]` のまま (変更なし) |
| BL-008 | calc.net の確認タグの削除 | 削除を承認 (変更なし) |
| BL-009 | Act の中の `ASSERT_NO_THROW` | 成否を戻り値で返す待機関数を testfw に加え、porter の全 115 件を書き換える |
| BL-010 | Act の途中の `EXPECT_CALL` | マルチ フェーズにする |
| BL-011 | `[Pre-Assert手順]` の欠落 | 276 件に加える。`DoDefault()` だけの動作指定は `.Times(n)` にする |

## テスト フェーズの規則の追加

`framework/testfw/docs/about-test-phase.md` に次の規則を加えます。

### Assert に置く文

Assert フェーズには、判定と、判定のための値の取り出しや集計だけを置きます。

Table: Assert に置ける文

| 文の種類 | Assert に置けるか | 例 |
| --- | --- | --- |
| 判定のマクロ (`EXPECT_*`、`ASSERT_*`、`SUCCEED()`) | 置ける | `EXPECT_EQ(3, verbose);` |
| 被テスト対象の状態を変えない宣言、代入、ループ、検索 | 置ける | ログから該当の行を探すループ、見つかったことを示すフラグ |
| サブ手順の呼び出し (判定だけを行うもの) | 置ける | `testing::expectExportNamesMatch(...)` |
| 被テスト対象への操作 (`[手順]` を付ける呼び出し、書き込み、状態の変更) | 置けない (Act へ移す) | `memset(buf, 0xAB, 16U);`、`cplat_argparser_handle_parse(parser);` |

操作の結果を判定し、さらに別の操作を行う場合は、マルチ フェーズ (Act_1/Assert_1、Act_2/Assert_2 …) に分けます。

### 確認コメントの書き方の追加

前案件で定めた「確認コメントの書き方」に、次の規則を加えます。

- 期待値や引数条件に含まれるキャストのうち、型をそろえるためだけのものは省いて書きます。値の意味を変えるキャスト (符号の扱いを変える `(uint32_t)-1`、ポインターの同一性を比べる `(void *)` など) は残します。

    ```text
    fgets() が buf、sizeof(buf)、kStream を指定して 2 回呼び出されること。
    buf[0] が 0xAB であること。
    ```

- 引数条件付きの `.Times(0)` は、「X を指定した f() の呼び出しがないこと。」と書きます。引数条件のない `.Times(0)` は、従来どおり「f() が呼び出されないこと。」とします。

    ```text
    "/tmp/output.bin" と等しい文字列、"wb" と等しい文字列を指定した cplat_fopen() の呼び出しがないこと。
    ```

- C++ のテスト コードの空判定は `nullptr` と比べ、確認コメントは「nullptr 以外であること」「nullptr であること」と書きます (コードの規則は [空ポインターの表記](#空ポインターの表記) による)。

### 状態と手順の説明文の表記

`[状態]`、`[手順]`、`[Pre-Assert手順]` の説明文では、確認コメントと同じく、関数名に引数なしの `()` を付け、識別子をバッククォートで囲みません。文の書き方 (主語や文型) は定めません。

```cpp
buf = (uint8_t *)cplat_malloc(16U); // [手順] - 16 バイトを指定して cplat_malloc() を呼び出す。
ON_CALL(mock_cplat, cplat_strncpy(_, _, _, _)).WillByDefault(...); // [状態] - cplat_strncpy() の既定動作を設定する。
```

### 動作を注入しない呼び出しの期待

`WillOnce` / `WillRepeatedly` に `DoDefault()` だけを渡す `EXPECT_CALL` は、動作の指定をやめ、`.Times(n)` で呼び出し回数を書きます。動作を注入しないため、`[Pre-Assert手順]` は書きません。

```cpp
EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
    .Times(1); // [Pre-Assert確認_正常系] - printf() が "3\n" と等しい文字列を指定して 1 回呼び出されること。
```

`DoDefault()` と別の動作を組み合わせる `EXPECT_CALL` (1 回目は失敗を返し、2 回目は既定の動作など) は、注入する動作を `[Pre-Assert手順]` に書きます。`WillOnce` / `WillRepeatedly` で動作を注入するその他の `EXPECT_CALL` は、現行の規則どおり `[Pre-Assert手順]` を書きます。

> [!NOTE]
> 既定の動作を `[Pre-Assert手順]` に書くと、`ON_CALL` と同じく、注入した値と既定の値が手順節に並んで矛盾して読まれるおそれがあります。`DoDefault()` だけの指定は回数の期待と同じ意味のため、回数で書きます。

### 呼び出しの順序に依存する期待

1 回目の呼び出しでは許し、2 回目の呼び出しでは禁じる期待のように、Act の途中で設定が必要な `EXPECT_CALL` は、マルチ フェーズに分け、2 回目の Act の前の Pre-Assert に置きます。間の Assert に判定がない場合は、フェーズ コメントだけを置きます。

```cpp
// Act_1
(void)format_job_failed(&actual_matched); // [手順] - 壊れた公開内容で組み立てる。

// Assert_1

// Pre-Assert_2
EXPECT_CALL(mock_cplat, cplat_local_lock_try_lock(_))
    .Times(0); // [Pre-Assert確認_異常系] - cplat_local_lock_try_lock() が呼び出されないこと。(同じ公開内容の取り込みを試みないことの確認)

// Act_2
(void)format_job_failed(&actual_matched); // [手順] - もう一度組み立てる。

// Assert_2
EXPECT_NE(0, actual_matched); // [確認_異常系] - actual_matched が 0 以外であること。(以前の条件を維持したことの確認)
```

## 空ポインターの表記

`app/general/docs/coding-guideline.md` に「空ポインターの表記」の章を設けます。置き場所は「ゼロ初期化」の章の直後とし、「概要」の章の一覧に追加します。

### 基本ルール

Table: 言語ごとの空ポインターの表記

| 翻訳の言語 | 対象 | 空ポインターの値 | 判定 |
| --- | --- | --- | --- |
| C | `.c` と、`.c` から読み込まれるヘッダー | `NULL` | `ptr == NULL` / `ptr != NULL` |
| C++ | `.cc`、`.cpp` と、C++ のソースだけから読み込まれるヘッダー | `nullptr` | `ptr == nullptr` / `ptr != nullptr` (Google Test では `EXPECT_EQ(nullptr, ptr)` など) |

次の書き方は使いません。

- C の `0`、`(void *)0` による空ポインター
- C++ の `NULL`、`0` による空ポインター
- `!ptr`、`if (ptr)`、`ptr ? a : b` など、ポインターをそのまま条件にする書き方

### 判定手順

1. ファイルの拡張子が `.c` であれば C の規則に従います。
2. ヘッダーは、1 つでも `.c` から読み込まれる場合 (公開ヘッダー `prod/include/` と内部共有ヘッダー `prod/include_internal/` を含む) に C の規則に従います。
3. それ以外 (`.cc`、`.cpp`、テストの mock ヘッダーなど C++ だけから読み込まれるヘッダー) は C++ の規則に従います。

### 例外

- 外部由来の配布物と自動生成された成果物は対象外です。
- コメントと文字列の中の `NULL` は、この章の対象外です。テストの確認コメントは `about-test-phase.md` に従います。

### 検証

規則の逸脱を次の方法で確かめます。コマンドの詳細は規範の本文に記載します。

- C++ のソースのコード上の `NULL` (コメントと文字列を除く) が 0 件であること。
- C のソースで、ポインターへ `0` を代入する箇所と、ポインターをそのまま条件にする箇所がないこと (候補を抽出し、型を確かめる)。

> [!NOTE]
> C17 には `nullptr` がなく (C23 で追加)、このリポジトリは C17 を前提とするため、C では `NULL` を使います。C++ の `nullptr` は型付きの空ポインターで、どのポインター型へもキャストなしで変換でき、多重定義やテンプレートの型推論で整数と取り違えられません。

## 共通処理の変更

### 成否を返す待機関数 (framework/testfw)

`framework/testfw/include/processController.h` に次の関数を加えます。既存の `waitForOutput()` は変えません。

```cpp
/**
 * stdout に指定パターンが出現するまで待機する。
 *
 * タイムアウトした場合も例外を送出せず、false を返す。
 *
 * @param pattern    待機する文字列 (部分一致)
 * @param timeout_ms タイムアウト (ms)。-1 で無制限。
 * @param output     受信した stdout (パターン出現位置まで含む) の格納先。不要な場合は nullptr。
 * @return           パターンが出現した場合は true、タイムアウトした場合は false。
 */
extern bool tryWaitForOutput(AsyncProcessHandle &handle, const string &pattern, int timeout_ms = 5000,
                             string *output = nullptr);
```

`framework/testfw/docs/process-control-api.md` に関数の説明を加え、例を `tryWaitForOutput()` と判定の分離に書き換えます。`framework/testfw/docs/trace-level-process-control.md` の関数表に行を加えます。

`app/porter/test/src/integration/porterSendRecvTest/porterSendRecvTest.cc` の `ASSERT_NO_THROW(waitForOutput(...))` (115 件) をすべて書き換えます。

```cpp
// Act の中 (被テスト対象の判定)
bool actual_ret_wait_hello =
    tryWaitForOutput(recv_h_, "Hello Porter", 3000); // [手順] - RECIEVER が "Hello Porter" を出力するまで待機する。
ASSERT_TRUE(actual_ret_wait_hello); // [確認_正常系] - "Hello Porter" を待つ tryWaitForOutput() の戻り値が true であること。

// Arrange の中 (準備の確認)
ASSERT_TRUE(tryWaitForOutput(recv_h_, "受信待機中", 5000)); // [状態] - RECIEVER が "受信待機中" を出力するまで待機する。
                                                           // [状態確認] - tryWaitForOutput() の戻り値が true であること。
```

Act の中の判定は、現行の「Act の中の判定」の規則 (後続の手順の有無による扱い) に従います。

### text_style_jp の変更 (framework/docsfw)

`framework/docsfw/bin/text_style_jp.py` (C/C++ のフロントエンドを含む) を次のとおり変更し、`--test` の自己テストに対応するテストを加えます。`framework/docsfw/docs/text_style_jp.md` の規則表を更新します。

1. C/C++ のコメントの整形で、同じ行の中の二重引用符で囲んだ部分と、C の文字リテラルの形 (`'x'`、`'\n'` など) の一重引用符で囲んだ部分を、文字列として整形の対象から外します。Markdown の処理は変えません。
2. 次の診断を `test-comment-*` に加えます。いずれも診断だけで、自動修正はしません。

Table: 追加する点検の規則

| 規則名 | 診断する内容 |
| --- | --- |
| `test-comment-act-expect` | Act フェーズの中の `EXPECT_*` |
| `test-comment-assert-step` | Assert フェーズの中の `[手順]` |
| `test-comment-pre-assert-phase` | Pre-Assert フェーズ以外の `[Pre-Assert確認_*]` と `[Pre-Assert手順]` |
| `test-comment-backtick` | エビデンス タグの説明文の中のバッククォート (文字リテラルの形の `` '`' `` は除く) |
| `test-comment-state-check-expectation` | `[状態確認]` の説明文の総合期待 (`。(〜ことの確認)`) |

## 既存コードの移行

Table: 既存コードの移行の対象

| 作業 | 対象 | 規模 (目安) |
| --- | --- | --- |
| Assert の中の操作を Act へ移す | `argparserTest.cc` の 24 テスト (マルチ フェーズ化)、`allocTest.cc` ほか `[手順]` や操作を含む Assert | 約 50 件 |
| 確認コメントのキャストを省く | 全テスト | 約 250 行 |
| 引数条件付きの `.Times(0)` の文型 | 全テスト | 10 件 |
| 状態と手順の説明文の表記 | 全 app と `framework/testfw` のテスト | 約 1,000 行 |
| C++ のコード上の `NULL` を `nullptr` へ | 全 C++ ソース (テスト コード、mock、`framework/testfw/libsrc`) | 約 3,200 件 |
| 確認コメントの空判定の表記 | 全テスト | `NULL` の表記約 300 件 |
| C のポインターへの `0` を `NULL` へ | C のソース | 候補約 30 件 (型を確かめる) |
| ポインターをそのまま条件にする箇所 | C と C++ のソース | 候補約 10 件 (型を確かめる) |
| `ASSERT_NO_THROW(waitForOutput(...))` | `porterSendRecvTest.cc` | 115 件 |
| Act の途中の `EXPECT_CALL` | `stringCatalogFilterSourceTest.cc` | 2 件 |
| `[Pre-Assert手順]` の追加 | 全テスト (`test-comment-expect-call-step` の指摘) | 276 件 |
| `DoDefault()` だけの動作指定を `.Times(n)` へ | 全テスト | 約 7 件 |
| 規範の文書の例 | `about-test-phase.md`、`app/general/docs/testing-tutorial.md` など、変更した規則の例を含む文書 | 該当箇所 |

C++ のコードで、ポインターに `0` を使っている箇所は移行の対象外です。作業中に見つけた場合は残課題に記録します。

## 作業の進め方

- 書き換えは、範囲ごとに codex CLI (`gpt-6.1-sol`、推論 low) に委譲し、同時に動かすのは 5 本程度までとします。委譲先には `make` と `clean` を実行させません。
- 規則の文書と共通処理 (`about-test-phase.md`、`coding-guideline.md`、`tryWaitForOutput()`、`text_style_jp.py`) は先に更新し、書き換えの手本にします。
- 同じファイルへの変更 (例: `NULL` の置き換えと説明文の表記) は、1 つの委譲の中でまとめて行います。

## 受入条件

1. テスト関数ごとの確認件数が、書き換えの前後で一致すること (今回の変更はいずれも確認件数を変えません)。
2. 変更した各 app、`framework/testfw` の `make test` が失敗 0 件で、内容のある `.warn` がないこと。
3. `framework/docsfw` の `text_style_jp.py --test` が成功すること。
4. 変更した C/C++ のテストに `text_style_jp.py --mode cpp --dry-run` を実行し、`test-comment-*` の指摘 (追加した規則を含む) が 0 件であること。ほかの規則の指摘が書き換えによって増えていないこと。
5. C++ のソースのコード上の `NULL` が 0 件であること (外部由来を除く)。
6. `git clang-format` の差分が残っていないこと。
7. 変更した Markdown が `text_style_jp.py --dry-run` で指摘なしであること。
8. 書き換えの差分を変更の種類ごとに抽出して点検し、判断に迷った箇所 (残したキャストなど) を一覧で提示すること。
9. コミットとプッシュをサブモジュール、リポジトリ本体の順に行い、CI が成功すること。

> [!NOTE]
> 全 app と framework にまたがる `make test` は、1 本ずつ順に実行して 1〜2 時間程度かかります。並行して実行すると成果物が壊れるため、同時に動かす `make` は 1 本だけとします。

## 残課題として記録する事項

次は今回対応せず、`prompt/test-assert-comment-followup-backlog.md` に記録します。

- 状態と手順の説明文の書き方 (主語や文型) の規則化
- 作業中に見つけた、C++ のコードでポインターに `0` を使っている箇所 (見つかった場合)
- そのほか、作業中に判明して対応しなかった事項
