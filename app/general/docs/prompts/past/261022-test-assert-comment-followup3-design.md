<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: claude code / セッション ID: session_018UJwBxipLvskQMgge7hiBW
上流文書: `261022-test-assert-comment-followup3-discussion.md` (検討用 Markdown、合意版: 版 4)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# リンク先の整形、注入しない呼び出しの診断、パラメーターの掲載 最終仕様

ファイル名: `prompt/current/test-assert-comment-followup3-design.md`

## 概要

この仕様は、次の 3 つの変更を定めます。

Table: 本仕様の変更の区分と対象

| 区分 | 変更 | 主な対象 |
| --- | --- | --- |
| リンク先の保護 | text_style_jp が、Markdown のリンク先を整形の対象から外します | `framework/docsfw` |
| 動作を注入しない呼び出しの診断 | text_style_jp が、動作を注入しない `EXPECT_CALL` の書き方の誤りを指摘します | `framework/docsfw`、`app/cplat` のテスト |
| パラメーターの掲載 | testfw が、パラメーター テストのパラメーターを結果の Markdown に名前付きで載せます | `framework/testfw`、`app/tutorial` と `app/porter` のテスト |

対象外は次のとおりです。

- ポインターの条件の常設の点検 (clang-tidy の導入)。本案件の残課題一覧に記録します。
- .NET のパラメーター テスト。テスト メソッドごとに 1 レコードとし、値は「実行結果」のテスト ランナーの出力で確認する現在の表し方を仕様とします。
- 型パラメーター テスト (`TYPED_TEST`) の印字の規約。現在のテスト コードにはありません。項目名と値の表示 ([項目名と値](#項目名と値)) だけを型パラメーターにも適用します。

## リンク先の保護

### 保護の対象

text_style_jp の Markdown の整形で、次のリンク先を整形の対象から外します。リンク テキスト、画像の代替テキスト、リンクのタイトル (`"..."`) は、従来どおり整形の対象です。

Table: 保護するリンク先の記法と範囲

| 記法 | 保護する範囲 | 例 (下線部を保護) |
| --- | --- | --- |
| インライン リンク | `](` の直後から、空白または `)` の直前まで。`<...>` で囲んだリンク先は `>` まで | `[表示](./foo-bar手順.md "タイトル")` の `./foo-bar手順.md` |
| 画像 | インライン リンクと同じ | `![図](images/構成図.png)` の `images/構成図.png` |
| 参照リンクの定義行 | 行頭 (空白 3 個まで) の `[ラベル]:` から、続く空白の後の最初の空白以外の文字の連続まで | `[r]: ./foo-bar手順.md` の全体 |
| ファイル内リンク | インライン リンクと同じ | `[a](#注入-pre-assert手順)` の `#注入-pre-assert手順` |

HTML の属性 (`href`、`src`) は対象外です。

保護は、コード ブロックとインライン コードの保護の後、URL の保護より前に行います。このため、`https://` で始まるリンク先は、リンク先の保護で `)` の直前までが保護され、URL の保護が `)` を越えて後続の文字列を取り込むことはありません。

> [!NOTE]
> URL の保護は「`https://` から空白の直前まで」を保護するため、リンク先の直後に空白がないと、`)` と後続の日本語やリンクまで整形の対象から外れていました。リンク先を先に保護すると、URL の保護はリンク先の内側で止まります。

### 既存の Markdown への適用

リンク先の保護によって、第一者管理の Markdown のうち次の 2 件の整形結果が変わります。2 件に `text_style_jp.py --in-place` を適用します。

Table: リンク先の保護で整形結果が変わる既存の Markdown

| ファイル | 変更前 | 変更後 |
| --- | --- | --- |
| `app/zlib/packages/README.md` | `[公式配布ページ](https://zlib.net/)から取得した` | `[公式配布ページ](https://zlib.net/) から取得した` |
| `framework/docsfw/docs/references/requirements-functional-specification-research.md` | `[第2.8版 本文 PDF](https://...)` | `[第 2.8 版 本文 PDF](https://...)` |

ほかの第一者管理の Markdown の整形結果は変わりません。

### 自己テスト

`text_style_jp.py --test` の自己テストに、次の場合を追加します。

- 英数字と日本語が隣接するリンク先 (インライン リンク、画像、参照リンクの定義行、ファイル内リンク) が変わらないこと。
- タイトル付きのリンクで、リンク先が変わらず、タイトルが整形されること。
- リンク テキストが整形されること。
- `https://` のリンク先の直後に日本語が続く場合に、`)` と日本語の間に空白が入ること。

## 動作を注入しない呼び出しの診断

### 診断の規則

text_style_jp のテスト コメントの診断 (C++) で、`EXPECT_CALL` の文を次のとおり判定します。「動作の指定」は、文に含まれる `.WillOnce(...)` と `.WillRepeatedly(...)` です。「既定の動作だけの指定」は、動作の指定が 1 つ以上あり、そのすべての引数が `DoDefault()` (`testing::` または `::testing::` の修飾を含む) だけである場合です。

Table: EXPECT_CALL の動作の指定に関する診断の規則

| 規則 ID | 指摘する条件 | メッセージ |
| --- | --- | --- |
| `test-comment-expect-call-step` | 既定の動作以外を含む動作の指定があり、`[Pre-Assert手順]` がない | `WillOnce / WillRepeatedly には [Pre-Assert手順] コメントが必要` |
| `test-comment-expect-call-step-without-action` (新規) | 動作の指定がなく、`[Pre-Assert手順]` がある | `動作を指定しない EXPECT_CALL に [Pre-Assert手順] コメントは不要` |
| `test-comment-expect-call-default-action` (新規) | 既定の動作だけの指定である (`[Pre-Assert手順]` の有無を問わない) | `DoDefault() だけを指定する EXPECT_CALL は .Times() で回数を書く` |

判定の例は次のとおりです。

Table: EXPECT_CALL の診断の判定例

| 文 | 指摘 |
| --- | --- |
| `.Times(1)` だけで、`[Pre-Assert手順]` なし | なし |
| `.Times(1)` だけで、`[Pre-Assert手順]` あり | `test-comment-expect-call-step-without-action` |
| `.WillOnce(Return(0))` で、`[Pre-Assert手順]` あり | なし |
| `.WillOnce(Return(0))` で、`[Pre-Assert手順]` なし | `test-comment-expect-call-step` |
| `.WillOnce(DoDefault())` (`[Pre-Assert手順]` の有無を問わない) | `test-comment-expect-call-default-action` |
| `.WillOnce(Return(-1)).WillRepeatedly(DoDefault())` で、`[Pre-Assert手順]` あり | なし |

診断の対象範囲、コメントの関連付け、文字列リテラルとコメントの除外は、既存のテスト コメントの診断と同じです。`.Times(...)` の有無と値は判定に使いません。

自己テストに、上表の各行の場合を追加します。

### 既存のテスト コードの移行

既定の動作だけを指定している次の 3 か所を、gMock で同じ意味の `.Times(...)` に書き換え、対応する `[Pre-Assert手順]` の行を削除します。`[Pre-Assert確認_*]` の説明文は変えません。

Table: 既定の動作だけの指定を回数へ書き換える箇所

| ファイル | 変更前 | 変更後 |
| --- | --- | --- |
| `app/cplat/test/src/libcplatTest/prompt/promptTest/promptAllocFailureTest.cc` | `.WillRepeatedly(DoDefault())` | `.Times(AnyNumber())` |
| `app/cplat/test/src/libcplatTest/runtime/symLoaderInitTest/symLoaderInitTest.cc` | `.WillOnce(DoDefault()).WillRepeatedly(DoDefault())` | `.Times(AtLeast(1))` |
| `app/cplat/test/src/libcplatTest/trace/trace_fileTest/traceFileCoverageTest.cc` | `.WillOnce(DoDefault())` (`cplat_local_lock_lock` の期待) | `.Times(1)` |

> [!NOTE]
> gMock では、`.WillRepeatedly(DoDefault())` は `.Times(AnyNumber())`、`.WillOnce(DoDefault()).WillRepeatedly(DoDefault())` は `.Times(AtLeast(1))`、`.WillOnce(DoDefault())` は `.Times(1)` と同じ回数の期待になります。呼び出された際の動作は、いずれも `ON_CALL` または mock の既定の動作です。

移行後、全テスト コードへの `text_style_jp.py --mode cpp --dry-run` で、上表の 3 規則の指摘は 0 件です。

## パラメーターの掲載

### パラメーターの型の規約

C++ の値パラメーター テスト (`TEST_P`) のパラメーターは、次の規約に従います。

- パラメーターの型は構造体とします。`std::tuple` などの名前のない組は使いません。
- パラメーターの構造体と同じ名前空間に、Google Test が印字に使う `PrintTo()` を定義します。

    ```cpp
    void PrintTo(const ParamTest1TestParam &param, std::ostream *os);
    ```

- `PrintTo()` は、構造体のすべての要素を宣言の順に `名前: 値` の形で印字し、`, ` で区切ります。文字列は `"..."` で囲みます。

    ```text
    a: 1, b: 3, expected: 3
    name: "recv_compress_buf", size: 8264, occurrence: 2
    ```

- テスト名の末尾 (`/0` などの部分) を値から作る場合は、`INSTANTIATE_TEST_SUITE_P` の第 4 引数に、要素から名前を返す関数を渡します。`PrintToStringParamName()` は使いません。

    ```cpp
    [](const TestParamInfo<ParamTest2TestParam> &param_info) { return param_info.param.desc; }
    ```

> [!NOTE]
> Google Test は、パラメーターを `PrintTo()` の多重定義、`operator<<`、バイト列の順に探して印字します。`std` 名前空間には `PrintTo()` を追加できないため、`std::tuple` の印字は値の列 (`(1, 3, 3)`) にしかならず、要素の意味が分かりません。`PrintToStringParamName()` はこの印字をテスト名に使いますが、テスト名には英数字と `_` しか使えないため、名前付きの印字とは両立しません。

### 掲載先

Google Test の `--gtest_list_tests` が出力するパラメーターの印字を、次の 3 か所に載せます。

Table: パラメーターの掲載先と形

| 掲載先 | 掲載の形 |
| --- | --- |
| 全体サマリー (`results/all_tests/summary.md`) の「テスト結果」表 | 列「パラメーター」。表のキャプションは「テスト ID 別の判定とパラメーター」 |
| レコード単位の `results.md` | 実施日時の次の行に `- パラメーター: <値>` |
| 定義単位の `results.md` の「実行レコード」の各レコード | 実施日時の次の行に `- パラメーター: <値>` |

パラメーターのないテストは、従来どおり列を空欄とし、行を出力しません。

テスト コードの冒頭に出力するコンソールの表示は、レコード単位の `results.md` と同じ形にします。テストの終了時にコンソールへ出力する一覧 (テスト ID、判定、Google Test の注記) は変えません。

### 項目名と値

項目名は「パラメーター」とします。値は、Google Test の注記から先頭の `# GetParam() = ` または `# TypeParam = ` を除いた文字列です。値は、従来の備考と同じくインライン コードで表示します。

```text
- 判定: PASSED
- 実施日時: 2026-10-23 10:00:00 JST
- パラメーター: `a: 1, b: 3, expected: 3`
```

### 印字方法のない型の検出

Google Test の注記が `# GetParam() = <数>-byte object <` で始まる場合、パラメーターの型に印字方法がないと判定し、そのレコードの判定を WARNING にします。テストが合格していることが条件で、FAILED のレコードは FAILED のままです。

検出したレコードでは、次の警告行をコンソールとレコードの「実行結果」に出力します。警告行は、全体サマリーの「警告とエラー」にも載ります。

```text
[ WARNING ] パラメーターがバイト列で印字されています。パラメーターの型に PrintTo() を定義してください。
```

WARNING のレコードは、テストの終了時の Warning(s) の件数に数え、定義単位の総合判定にも反映します。テストの終了コードは失敗になりません。

> [!NOTE]
> `operator<<` が要素の一部だけを出力する場合や、`std::tuple` の値の列は、印字の書式から機械的に区別できないため検出しません。規約とレビューで担保します。

### テスト コード節の抜粋

レコード単位と定義単位の「テスト コード」節には、テスト スイート名が一致するすべての `INSTANTIATE_TEST_SUITE_P` を抜粋します。接頭辞 (第 1 引数) が空の登録も含め、行の分け方 (引数の途中での改行) によらず抜粋します。

- `framework/testfw/bin_internal/test_subprocedures.py` の登録の検出で、接頭辞が空の場合も一致させます。
- `framework/testfw/bin_internal/gtest_summary_groups.py` で、定義単位の抜粋に使う接頭辞の集合を、接頭辞が空のテスト名 (`Suite.Test/0`) では空文字列とします。

### 使われていない抽出処理の削除

テスト コードの抽出は `test_subprocedures.py` だけが行います。使われていない次の処理と参照を削除します。

- `framework/testfw/bin_internal/get_test_code_c_cpp.awk`
- `framework/testfw/bin_internal/gtest_summary_groups.py` の `extract_code()`
- `framework/testfw/bin_test/test_results_refinement.py` の、awk を実行する 3 件のテスト
- `framework/testfw/bin_internal/exec_test_c_cpp.sh` の、エビデンスの再生成を判定するハッシュの対象一覧の `get_test_code_c_cpp.awk`
- `app/general/docs/dotnet-test-results-design.md` のファイル一覧の `get_test_code_c_cpp.awk` の行

### パラメーター テストの移行

既存の 4 ファイルを規約に合わせます。

Table: パラメーター テストの移行の内容

| ファイル | 変更 |
| --- | --- |
| `app/tutorial/test/src/param-test/param-test1.cc` | `tuple<int, int, int>` を構造体 (`a`、`b`、`expected`) に置き換え、`PrintTo()` を定義します。3 つの登録 (接頭辞が空、`MultiplicationTests1`、`MultiplicationTests2`) と値は維持します。awk の制約を説明する `NOTE` コメントを削除します |
| `app/tutorial/test/src/param-test/param-test2.cc`、`param-test3.cc` | 名前だけを出力する `operator<<` を `PrintTo()` (`desc: "2_3_6", a: 2, b: 3, expected: 6`) に置き換え、`PrintToStringParamName()` を `desc` を返す関数に置き換えます。テスト名 (`MultiplyTest/2_3_6` など) は変わりません |
| `app/porter/test/src/integration/porterSendRecvTest/tcp_allocation_failure_test.cc` | `allocation_case` に `PrintTo()` (`name: "recv_compress_buf", size: 8264, occurrence: 2`) を定義します |

移行後、パラメーター テストのレコードに WARNING はありません。

## 知見の反映計画

Table: 正本ドキュメントとコード コメントへの反映先

| 反映先 | 反映する内容 |
| --- | --- |
| `framework/docsfw/docs/text_style_jp.md` | 「URL の保護」の後に「リンク先の保護」の節を設け、保護の対象と例を記載します。処理順の一覧の保護の手順にリンク先を加えます。ルール ID の表に 2 規則を追加し、`test-comment-expect-call-step` の説明を「既定の動作以外の動作の指定」に合わせます |
| `framework/testfw/docs/about-test-phase.md` | 「パラメーター テストのパラメーター」の節を設け、パラメーターの型の規約、テスト名の作り方、印字方法のない型の WARNING、.NET の表し方を記載します。「ファイル構成」の備考の記載をパラメーターに改め、定義単位の実行レコードと全体サマリーの記載にパラメーターを加えます。「動作を注入しない呼び出しの期待」に、text_style_jp が診断することを加えます |
| `app/general/.agents/skills/create-unit-test/SKILL.md` | パラメーター テストを作成・変更する場合の参照先として、`about-test-phase.md` の「パラメーター テストのパラメーター」を加えます |
| コード コメント | `PrintTo()` を定義する理由 (Google Test の印字の探索順) を、印字方法の検出処理に `see:` 形式の根拠 URL とともに記載します |

> [!NOTE]
> テスト コード節の抜粋の不具合と、使われていない awk の削除は、実装の補正です。抜粋の規則 (スイート名が一致するすべての登録) は `about-test-phase.md` の既存の記載の範囲内のため、新たな規範としては記載しません。

## 受入条件

Table: 受入条件と確認方法

| 区分 | 確認方法 | 合格の基準 |
| --- | --- | --- |
| リンク先の保護 | `text_style_jp.py --test`、`framework/docsfw` の `bin_test/` | すべて合格 |
| リンク先の保護 | 第一者管理の Markdown 全体への `text_style_jp.py --dry-run` | 2 件への `--in-place` の適用後、変更の指摘がない |
| 診断 | 全テスト コードへの `text_style_jp.py --mode cpp --dry-run` | 3 規則の指摘が 0 件 |
| 診断の移行 | `app/cplat` の該当テスト ディレクトリの `make test` | すべて合格 |
| パラメーターの掲載 | `framework/testfw` の `bin_test/` | すべて合格 |
| パラメーターの掲載 | `app/tutorial` と `app/porter` の `make test` と結果の Markdown | 3 か所にパラメーターが名前付きで載り、テスト コード節に接頭辞が空の登録を含むすべての登録が載る |
| 全体 | リポジトリ ルートの `make test` 1 回 | 本案件に起因する失敗と WARNING がない |
| 全体 | ビルド後の `.warn` ファイル | 本案件に起因する警告がない |
| 文書 | 変更した Markdown への `text_style_jp.py --dry-run` | 変更の指摘がない |

受入条件の確認後に、サブモジュール (`framework/docsfw`、`framework/testfw`、`app/cplat`、`app/porter`、`app/zlib`) を先に、リポジトリ本体を後にコミットしてプッシュし、CI の結果を確認します。
