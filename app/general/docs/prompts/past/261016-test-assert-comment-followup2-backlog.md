<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新エージェント: claude code / セッション ID: session_01ACTERhUfA9PYy9ZXAEhsb4
上流文書: `261016-test-assert-comment-followup2-design.md` (最終仕様)、`261016-test-assert-comment-followup2-discussion.md` (検討用 Markdown)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 残課題一覧 (バックログ)

ファイル名: `prompt/test-assert-comment-followup2-backlog.md`

## 案件の識別

案件名: 状態と手順の説明文と空ポインターの残課題への対応 (test-assert-comment-followup2)  
作成日: 2026-10-16  
整理状態: 整理済み

この文書は、設計および実装・検証作業がすべて完了した後に、見送る決定がなされた問題点や、作業中に潜在または別問題として判明したが今回は対策しなかった最終的な残渣 (残課題) を記録したものです。作業中に解消した問題や、仕様・制約として容認した問題点は含みません。各項目はチケット管理システム (GitHub Issues、Jira など) への起票にそのまま利用できる詳細度・具体性で記載されています。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | Markdown のリンク先への空白の挿入 | 作業中摘出 | 中 | 未着手 |
| BL-002 | ポインターをそのまま条件にする書き方の常設の点検 | 見送り | 低 | 未着手 |
| BL-003 | 動作を注入しない呼び出しの期待に付いた手順の説明文の点検 | 作業中摘出 | 低 | 未着手 |
| BL-004 | パラメーター テストのパラメーターのサマリーへの掲載 | ユーザー提起 | 中 | 未着手 (要検討) |

## BL-001 Markdown のリンク先への空白の挿入

### 概要と問題点

- **現状の事象**: `framework/docsfw/bin/text_style_jp.py` の Markdown の整形で、規則 `fullwidth-halfwidth-space` が、リンクの URL 部分 (`](...)` の中) の半角英数字と全角文字の境目にも空白を入れます。`--in-place` を実行すると、リンク先が書き換わり、リンクが壊れます。
    - 見出し「注入 ([Pre-Assert手順])」へのリンク `[注入](#注入-pre-assert手順)` が `[注入](#注入-pre-assert 手順)` になります。
    - 相対パスのリンク `[a](./foo-bar手順.md)` も、`./foo-bar 手順.md` になります。
- **本来あるべき姿**: リンクの URL 部分は、見出しのアンカーやファイル パスとして文字どおりに解釈されるため、整形の対象から外します。リンクの表示文字列 (`[...]` の中) は、従来どおり整形の対象とします。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`prompt/test-assert-comment-followup2-design.md`) に `text_style_jp.py --in-place` を実行した際に発見しました (2026-10-16)。見出し「注入 ([Pre-Assert手順])」へのリンクが壊れました。
- **今回の見送り理由**: 本案件の範囲 (テストの状態と手順の説明文と空ポインターの残課題) 外のため。最終仕様では、リンクをやめて回避しました。

### 影響範囲とリスク

- **影響対象**: `text_style_jp.py` で整形する第一者管理の Markdown 全体。英字の直後に日本語が続く見出し (例: `[Pre-Assert手順]` を含む見出し) へのアンカー リンクと、英字の直後に日本語が続くファイル名への相対リンク。
- **放置した場合のリスク**: `--in-place` による一括整形 (`check-all-markdown-style` スキルなど) で、文書中のリンクが気付かれないまま壊れます。GitHub や MkDocs の表示ではリンク切れになりますが、整形の差分は空白 1 文字のため、レビューで見落としやすくなります。

### 推奨される対応策

- **恒久対応案**: Markdown のインライン リンクと画像 (`[text](url)`、`![alt](url)`) の URL 部分、および参照リンクの定義行 (`[id]: url`) を、整形の前に保護し、整形の後に復元します。既存の「Markdown リンク URL `](url)` 内の `__` を復元する」処理と同じ仕組みを、`fullwidth-halfwidth-space` を含むすべての規則に広げる方針が考えられます。`--test` の自己テストに、アンカー リンクと相対パスのリンクが変わらない例を加えます。
- **参考情報**:
    - 再現手順: 次の 1 行のファイルに `text_style_jp.py --dry-run` を実行すると、`fullwidth-halfwidth-space` が URL の中を指摘します。

        ```text
        x [a](#注入-pre-assert手順) y
        ```

    - 関連する実装: `framework/docsfw/bin/text_style_jp.py`、`framework/docsfw/bin_internal/text_style_jp_engine.py`

## BL-002 ポインターをそのまま条件にする書き方の常設の点検

### 概要と問題点

- **現状の事象**: コーディング規範 (`app/general/docs/coding-guideline.md` の空ポインターの表記) は、ポインターとスマート ポインターを `if (p)` や `if (!p)` のようにそのまま条件にせず、`p != NULL` (C) や `p == nullptr` (C++) と比較して書くことを求めています。しかし、この規範からの逸脱を常に検出する仕組みはありません。条件式の値がポインターかどうかは型を解析しないと判定できず、正規表現による点検では整数や bool と区別できないためです。
- **本来あるべき姿**: 新たな逸脱が CI などで機械的に検出されることが望まれます。

### 経緯と見送り理由

- **摘出契機**: 最終仕様の検討で、型を考慮した点検の方式を確認しました。
- **今回の見送り理由**: ユーザーの判断により、点検の仕組みは新設せず、既存の逸脱だけを一時的な点検で 1 回だけ探して直しました。一時的な点検は、libclang で Linux の構成 (`PLATFORM_LINUX`) の翻訳単位 1076 個を解析し、`if`、`while`、`for`、`?:`、`!`、`&&`、`||` の被演算子がポインター型かスマート ポインター型である箇所を報告するものです。この点検には次の限界があり、検出の漏れが残っている可能性があります。
    - 次の 12 ファイルは、ビルド時だけに作られるシンボリック リンクや、ビルド時だけに定義されるマクロに依存するため、解析エラーを含みます。エラーの箇所より後の構文は解析されていない可能性があります (正規表現で目視の点検は行いました)。
        - `app/cplat/test/src/libcplatTest/exportTest/exportTest.cc`
        - `app/cplat/test/src/libcplatTest/hashtable/hashtableTest/hashtable_inject.c`
        - `app/cplat/test/src/libcplatTest/string_catalog/` 配下の 7 ファイル (`stringCatalogArgumentTest.cc`、`stringCatalogFilterPatternCompileTest.cc`、`stringCatalogFilterStructureTest.cc`、`stringCatalogFilterApplyTest.cc`、`stringCatalogFilterSourceTest.cc`、`stringCatalogRenderTest.cc`、`stringCatalogValidateTest.cc`)
        - `app/zlib/test/libsrc/mock_zlib/mock_zlib.cc`、`app/zlib/test/src/cmd/zlibTest/zlibTest.cc`、`app/zlib/test/src/libzlibTest/exportTest/exportTest.cc`
    - Windows だけでコンパイルされるコード (`PLATFORM_WINDOWS` の分岐と `*_windows.c` など) は、libclang では解析していません。正規表現で単純な名前の条件だけを抽出し、目視で点検しました。関数の戻り値や配列の要素を条件にする書き方は、この点検の対象外です。

### 影響範囲とリスク

- **影響対象**: 第一者管理の C/C++ のコード全体 (製品コード、テスト コード、testfw)。
- **放置した場合のリスク**: 規範に反する書き方が新たに混入しても、レビューでしか気付けません。動作には影響しないため、気付かれないまま増えるおそれがあります。

### 推奨される対応策

- **恒久対応案**: clang-tidy の `readability-implicit-bool-conversion` (`AllowPointerConditions: false`) を、Linux と Windows の両方の構成で、ビルドと同じコンパイル オプション (`compile_commands.json`) を使って実行する方針が考えられます。ビルドと同じオプションを使えば、上記の解析エラーと Windows の分岐の漏れが解消します。整数や bool の暗黙の変換も同じ検査で報告されるため、ポインター以外の指摘を抑止するオプションの調整が必要です。
- **参考情報**: 一時的な点検で検出して直した箇所は、`app/cplat/prod/libsrc/cplat/runtime/module.c` (3 か所)、`app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` (1 か所)、`framework/testfw/include/processController.h` と `framework/testfw/libsrc/test_com/processController*.cc`、`app/porter/test/src/integration/porterSendRecvTest/porterSendRecvTest.cc` です。

## BL-003 動作を注入しない呼び出しの期待に付いた手順の説明文の点検

### 概要と問題点

- **現状の事象**: `framework/testfw/docs/about-test-phase.md` の「動作を注入しない呼び出しの期待」は、`WillOnce` などで動作を指定しない `EXPECT_CALL` (`.Times(AnyNumber())` など回数だけの期待) に `[Pre-Assert手順]` を書かないと定めています。しかし、`text_style_jp.py` の診断はこの規則を検査しません。本案件では、規則に反して `[Pre-Assert手順]` を付けた既存の 7 行 (`app/struct-meta/test/src/structMetaPatchTest/structMetaPatchTest.cc` 3 行、`app/cplat/test/src/libcplatTest/argparser/argparserTest/argparserTest.cc` 4 行) を見つけて削除しました。
- **本来あるべき姿**: 動作を注入しない `EXPECT_CALL` に `[Pre-Assert手順]` が付いている場合に、機械的に指摘されることが望まれます。

### 経緯と見送り理由

- **摘出契機**: 説明文の書き換えの委譲先から、「動作を注入していない `EXPECT_CALL` に `[Pre-Assert手順]` がある」との報告を受け、全テスト コードを一時的なスクリプトで走査しました (2026-10-17)。
- **今回の見送り理由**: 本案件で常設する診断は `test-comment-function-paren` (関数名の `()` の付け忘れ) だけと合意しており、この検査は範囲外のためです。

### 影響範囲とリスク

- **影響対象**: 全 app と `framework/testfw` のテスト コード。
- **放置した場合のリスク**: 既定の動作を `[Pre-Assert手順]` に書くと、エビデンスの `### 手順` に注入していない動作が並び、注入した値と矛盾して読まれるおそれがあります。

### 推奨される対応策

- **恒久対応案**: `framework/docsfw/bin_internal/text_style_jp_frontends.py` の C/C++ のテスト コメントの診断に、`EXPECT_CALL(` で始まる文を `;` まで読み、`.Will` を含まない文に付いた `[Pre-Assert手順]` を指摘する規則を追加します。`text_style_jp.py --test` の自己テストに、指摘する例と指摘しない例 (`WillOnce(Return(...))` と `DoDefault()` の組み合わせ) を加えます。
- **参考情報**: 一時的な走査は、`EXPECT_CALL` の文と、その行末および直後のコメント行のタグを対応付ける単純な方式で行いました。

## BL-004 パラメーター テストのパラメーターのサマリーへの掲載

### 概要と問題点

- **現状の事象**: C/C++ のパラメーター テスト (`TEST_P` と `INSTANTIATE_TEST_SUITE_P`) で、テスト コードが指定したパラメーターの値は、テスト結果の Markdown に体系的には掲載されていません。`app/tutorial/test/src/param-test` の結果では、次のとおりです。
    - `results/all_tests/summary.md` の「テスト結果」表の備考列と、レコード単位の `results.md` の「備考」には、Google Test のテスト一覧 (`--gtest_list_tests`) が出力する `# GetParam() = ...` の文字列がそのまま載ります (`framework/testfw/bin_internal/exec_test_c_cpp.sh` が `--comment` として渡しています)。
    - `tuple` を `Values(make_tuple(1, 3, 3), ...)` で渡す `param-test1.cc` では `# GetParam() = (1, 3, 3)` となり、値は分かりますが、どの要素が a、b、期待する戻り値なのかは分かりません。
    - 構造体を渡し、`operator<<` が名前だけを出力する `param-test2.cc` と `param-test3.cc` では、`# GetParam() = 2_3_6` となり、a、b、期待する戻り値の値は載りません。
    - 定義単位の `results/<Suite>.<Test>/results.md` の「実行レコード」節には、レコードごとのパラメーターが載りません。
    - 「テスト項目の詳細」の `### 状態` は「パラメーターから a を取り出す。」のようにコメントの文面だけで、レコードごとの具体的な値は分かりません。
    - 「テスト コード」節の抜粋に含まれる `INSTANTIATE_TEST_SUITE_P` が一定しません。`param-test1.cc` では接頭辞のある 2 行だけが含まれ、接頭辞が空の行は含まれません。複数行にわたる `param-test2.cc` と `param-test3.cc` の `INSTANTIATE_TEST_SUITE_P` は含まれません。
- **本来あるべき姿**: テストのエビデンスを読む人が、各レコードがどのパラメーターで実行され、どの値を期待したのかを、テスト コードを読み解かずに確認できることが望まれます。ただし、パラメーターの渡し方は多様なため、どのシナリオで何をどこに表すかを先に検討する必要があります。

### 経緯と見送り理由

- **摘出契機**: ユーザーから、`app/tutorial/test/src/param-test` を題材に、テスト コードで指定しているパラメーターをサマリーの Markdown に掲載することを検討する課題として追加するよう指示を受けました (2026-10-17)。
- **今回の見送り理由**: 本案件の範囲 (テストの状態と手順の説明文と空ポインターの残課題) 外のため。様々なシナリオでの表し方の検討が必要で、方針の合意が先に要ります。

### 影響範囲とリスク

- **影響対象**: `framework/testfw` のテスト結果の Markdown の生成 (`bin_internal/exec_test_c_cpp.sh`、`bin_internal/results_markdown.py`、`bin_internal/get_test_code_c_cpp.awk`) と、`framework/testfw/docs/about-test-phase.md` のエビデンスの規範。パラメーター テストを持つ全 app のテスト結果。.NET のパラメーター テスト (xUnit の `[Theory]` など) の表し方との整合も対象です。
- **放置した場合のリスク**: パラメーター テストのエビデンスから、実行したパラメーターと期待値を確認できず、レビューや監査でテスト コードを読み解く手間が残ります。`operator<<` で名前だけを出力する書き方では、値がエビデンスに一切残りません。

### 推奨される対応策

- **恒久対応案**: 実装の前に、次の観点で表し方を検討し、規範 (`about-test-phase.md`) に定めます。
    - **掲載先**: summary の「テスト結果」表、定義単位の `results.md` の「実行レコード」節、レコード単位の `results.md` のどれに、どの粒度で載せるか。表のセルに長い値を載せるか、リンク先に寄せるか。
    - **値の出所**: Google Test の出力 (`# GetParam() = ...`) をそのまま使うか、テスト コードの `INSTANTIATE_TEST_SUITE_P` の記述を抜粋するか、テスト側で表示用の文字列を返す規約 (`PrintTo` や `operator<<` の書き方) を定めるか。
    - **検討すべきシナリオ**:
        - `tuple` を渡す場合 (要素の意味を表す名前がない)。
        - 構造体を渡し、`operator<<` が名前だけを返す場合 (`PrintToStringParamName()` でテスト名に使う書き方)。
        - `Combine()`、`Range()`、`ValuesIn()`、`Bool()` でパラメーターを生成し、レコード数が多い場合。
        - 印字方法のない型やポインターを渡し、Google Test がバイト列やアドレスを出力する場合。
        - `|`、改行、バッククォートなど、Markdown の表や記法を壊す文字を含む値の場合。
        - 同じ `TEST_P` を複数の `INSTANTIATE_TEST_SUITE_P` で登録する場合 (接頭辞が空の登録を含む)。
        - 型パラメーターのテスト (`TYPED_TEST`、`TYPED_TEST_P`) で、Google Test が `# TypeParam = ...` を出力する場合。
        - .NET のパラメーター テストとの表し方の対称性。
    - あわせて、「テスト コード」節の抜粋に `INSTANTIATE_TEST_SUITE_P` が一定して含まれない事象 (接頭辞が空の登録と、複数行にわたる登録が欠ける) も、掲載方法の検討の中で扱います。
- **参考情報**:
    - 題材: `app/tutorial/test/src/param-test/param-test1.cc`、`param-test2.cc`、`param-test3.cc` と、その `results/` 配下の Markdown。
    - 関連する規範: `framework/testfw/docs/about-test-phase.md` (パラメーター テストの結果ファイルの配置と、`回数=PARAM` の確認件数)、`framework/testfw/docs/how-to-test.md` (結果ファイルのパス)。
