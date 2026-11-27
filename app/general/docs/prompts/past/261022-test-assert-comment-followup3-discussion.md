<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新エージェント: claude code / セッション ID: session_018UJwBxipLvskQMgge7hiBW
上流文書: `261016-test-assert-comment-followup2-backlog.md` (前案件の残課題一覧)。削除されている場合があります。
下流文書: `261022-test-assert-comment-followup3-design.md` (最終仕様)、`261022-test-assert-comment-followup3-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# リンク先の整形、注入しない呼び出しの診断、パラメーターの掲載の検討

ファイル名: `prompt/current/test-assert-comment-followup3-discussion.md`

## 検討の識別

検討 ID: test-assert-comment-followup3

版: 4

状態: 合意済み

## 作業 WBS と進捗管理

この検討用 Markdown が管理する作業 WBS です。Q&A のターンごとに必要に応じて見直します。フェーズ 3 の項目は、[Q-001](#q-001-本案件で対処する残課題) の回答で対象外になったものを削除し、具体的な対象ファイルを追記します。見送りの決定や範囲外の気付きは、どの段階でも生じた時点で `test-assert-comment-followup3-backlog.md` に追記します。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 前提条件と要求事項の調査 (`prompt/current/test-assert-comment-followup2-backlog.md` の BL-001〜BL-004、`framework/docsfw/bin_internal/text_style_jp_engine.py`、`text_style_jp_frontends.py`、`framework/testfw/docs/about-test-phase.md`、`framework/testfw/bin_internal/get_test_code_c_cpp.awk`、`results_markdown.py`、`exec_test_c_cpp.sh`、パラメーター テストの結果、clang-tidy の導入状況)
    - ✅ 1.2. 課題・論点の抽出と明確化のための確認項目 ([Q-001](#q-001-本案件で対処する残課題)〜[Q-013](#q-013-前案件の残課題一覧の扱い)、版 2 で [Q-014](#q-014-net-のパラメーターの表し方の明記)〜[Q-018](#q-018-印字方法のない型の検出)、版 3 で [Q-019](#q-019-使われていないテスト-コード抽出の-awk-の扱い)、[Q-020](#q-020-リンク先の保護で整形結果が変わる既存の-markdown) を追加) の作成
    - ✅ 1.3. Q&A ターンごとのユーザー回答の受付と記録 (JST 日時、Git ユーザー名。版 2 で Q-001〜Q-013、版 3 で Q-014〜Q-018、版 4 で Q-019、Q-020 を記録)
    - ✅ 1.4. 回答に基づく前提・制約の検証と WBS の見直し (版 2 で、BL-002 の項目を削除し、4.4 を細分化し、5.3 を追加。版 3 で、4.4.3 の対象を `test_subprocedures.py` に訂正。版 4 で、4.1、4.4.3、4.9 の対象を確定)
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定 (懸念がないことをもってフェーズ 2 へ着手。合意版: 版 4)
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `prompt/current/test-assert-comment-followup3-design.md` の作成
    - ✅ 3.2. 最終仕様における受入条件と整合性の確認
    - ✅ 3.3. 対話セッションにおける最終仕様 (design.md) の承認の受付 (第 2 ゲート: 作業着手の合意)
- ✅ 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. BL-001: リンク先の保護 (`framework/docsfw/bin_internal/text_style_jp_frontends.py`、`framework/docsfw/bin/text_style_jp.py` の自己テスト、`framework/docsfw/docs/text_style_jp.md`。[Q-020](#q-020-リンク先の保護で整形結果が変わる既存の-markdown) により、`app/zlib/packages/README.md` と `framework/docsfw/docs/references/requirements-functional-specification-research.md` への `--in-place` の適用)
    - ✅ 4.2. BL-003: 動作を注入しない呼び出しの診断 (`framework/docsfw/bin_internal/text_style_jp_frontends.py` の 2 規則の追加と `test-comment-expect-call-step` の調整、`framework/docsfw/bin/text_style_jp.py` の自己テスト、`framework/docsfw/docs/text_style_jp.md`)
    - ✅ 4.3. BL-003: 既存のテスト コードの移行 (`promptAllocFailureTest.cc`、`symLoaderInitTest.cc`、`traceFileCoverageTest.cc` の `DoDefault()` だけの指定 3 か所を `.Times()` へ)
    - ✅ 4.4. BL-004: パラメーターの掲載
        - ✅ 4.4.1. 規範の更新 (`framework/testfw/docs/about-test-phase.md` のパラメーターの印字の規約と掲載先)
        - ✅ 4.4.2. 結果生成の変更 (`framework/testfw/bin_internal/results_markdown.py`、`exec_test_c_cpp.sh`、`gtest_summary_groups.py`。パラメーターの掲載、項目名、バイト列の印字の WARNING)
        - ✅ 4.4.3. テスト コード節の抜粋の変更 (`framework/testfw/bin_internal/test_subprocedures.py`。[Q-019](#q-019-使われていないテスト-コード抽出の-awk-の扱い) により、`get_test_code_c_cpp.awk`、`gtest_summary_groups.py` の `extract_code()`、`bin_test/test_results_refinement.py` の該当テスト、`exec_test_c_cpp.sh` のハッシュの対象一覧、`app/general/docs/dotnet-test-results-design.md` の記載の削除)
        - ✅ 4.4.4. パラメーター テストの移行 (`app/tutorial/test/src/param-test/param-test1.cc`〜`param-test3.cc`、`app/porter/test/src/integration/porterSendRecvTest/tcp_allocation_failure_test.cc`)
    - ✅ 4.5. 正本ドキュメントおよびコード コメントへの永続的知見の反映
    - ✅ 4.6. 局所テストおよび機械的検証 ([Q-011](#q-011-受入時のテストの範囲) の A: 局所的な確認とリポジトリ ルートの `make test` 1 回、`.warn` の確認、変更した Markdown への `text_style_jp.py`)
    - ✅ 4.7. 受入条件への適合確認 (全完確認)
    - ✅ 4.8. 残課題一覧 (`prompt/current/test-assert-comment-followup3-backlog.md`) の再構築と整理 (解消・容認した項目の除外、起票できる詳細度への書き直し)
    - ✅ 4.9. コミットとプッシュ、CI の追跡 ([Q-012](#q-012-コミットとプッシュの扱い)。サブモジュール `framework/docsfw`、`framework/testfw`、`app/cplat`、`app/porter`、`app/zlib` を先に、`app/tutorial` と `app/general` を含むリポジトリ本体を後に)
- ✅ 5. 成果物の承認の受付 (第 3 ゲート)
    - ✅ 5.1. 成果物・検証結果・残課題の提示
    - ✅ 5.2. 対話セッションにおける成果物の承認の受付 (「承認」または「完了」の受領によるスキル完了)
    - ✅ 5.3. 前案件の 3 文書の `prompt/past/` への移動 ([Q-013](#q-013-前案件の残課題一覧の扱い)。`mv` で行い、ステージングはしない)

## 対象と参照資料

対象成果物: text_style_jp の Markdown のリンク先の保護とテスト コメントの診断、cplat のテスト 3 ファイル、testfw の結果生成と規範、パラメーター テスト 4 ファイル

今回の対象範囲: `prompt/current/test-assert-comment-followup2-backlog.md` の BL-001、BL-003、BL-004 ([Q-001](#q-001-本案件で対処する残課題))。BL-002 は本案件の残課題一覧 (`prompt/current/test-assert-comment-followup3-backlog.md`) に引き継ぎます。

書き込み可能な検討用 Markdown: この文書 (`prompt/current/test-assert-comment-followup3-discussion.md`)

参照資料と版: リポジトリ本体 `1d45f005`、`framework/docsfw` `2f8fe23` の作業ツリー (2026-10-22 時点)

## 調査結果

### BL-001 リンク先への空白の挿入

- 現在も再現します。次の 3 行に `text_style_jp.py --dry-run` を実行すると、3 か所すべてで `fullwidth-halfwidth-space` が URL の中を指摘します。

    ```text
    x [a](#注入-pre-assert手順) y
    [b](./foo-bar手順.md)
    [r]: ./foo-bar手順.md
    ```

- Markdown の整形で保護しているのは、表示文字列にファイル名を含むリンクの全体 (`_FILE_LINK_PATTERN`) と `https://` などの URL (`_protect_urls`) だけです。相対パスとアンカーのリンク先は保護していません。
- C/C++ のコメントの整形には、リンクの後半 `](...)` を保護するパターン (`_MARKDOWN_LINK_TAIL_PATTERN`) がすでにあります。Markdown の整形では使っていません。
- 第一者管理の Markdown には、現時点で壊れたリンク先 (英数字と日本語の境目に空白が入ったもの) や、壊れる条件を満たすリンク先はありません。HTML の `href` と `src` に日本語を含むものもありません。

### BL-002 ポインターの条件の常設の点検

- この環境と CI コンテナー (`/home/user/oracle-linux-container/src/Dockerfile`) には clang-tidy がありません。コンテナーは LLVM のソースから clang-format だけをビルドしています。
- Windows の Visual Studio には `VC/Tools/Llvm/x64/bin/clang-tidy.exe` があります。
- makefw には、clang-tidy に渡すコンパイル オプションの一覧 (`compile_commands.json`) を作る仕組みがありません。
- 導入には、コンテナーの変更 (別リポジトリ)、makefw の変更、CI の手順の追加が必要です。

### BL-003 動作を注入しない呼び出しの診断

- 既存の診断 `test-comment-expect-call-step` は、`WillOnce` / `WillRepeatedly` があるのに `[Pre-Assert手順]` がない `EXPECT_CALL` を指摘します。逆向き (動作がないのに `[Pre-Assert手順]` がある) は検査していません。
- `about-test-phase.md` の「動作を注入しない呼び出しの期待」は、次の 2 点を定めています。
    - `WillOnce` / `WillRepeatedly` に `DoDefault()` だけを渡す `EXPECT_CALL` は、動作の指定をやめて `.Times(n)` で書く。
    - 動作を注入しないため、`[Pre-Assert手順]` は書かない。
- 一時スクリプトで全テスト コードの `EXPECT_CALL` 1,743 文を走査した結果は次のとおりです。
    - 動作の指定がないのに `[Pre-Assert手順]` がある文: 0 件 (前案件で 7 行を削除済み)
    - `DoDefault()` だけを渡し、`[Pre-Assert手順]` を書いている文: 3 件。規範に反しています。
        - `app/cplat/test/src/libcplatTest/prompt/promptTest/promptAllocFailureTest.cc` 182 行 (`.WillRepeatedly(DoDefault())`)
        - `app/cplat/test/src/libcplatTest/runtime/symLoaderInitTest/symLoaderInitTest.cc` 571 行 (`.WillOnce(DoDefault()).WillRepeatedly(DoDefault())`)
        - `app/cplat/test/src/libcplatTest/trace/trace_fileTest/traceFileCoverageTest.cc` 515 行 (`.WillOnce(DoDefault())`)

### BL-004 パラメーター テストのパラメーターの掲載

- C++ のパラメーター テストは 4 ファイルです (`app/tutorial/test/src/param-test/param-test1.cc`〜`param-test3.cc`、`app/porter/test/src/integration/porterSendRecvTest/tcp_allocation_failure_test.cc`)。型パラメーターのテスト (`TYPED_TEST`) はありません。
- 現在の掲載は次のとおりです。
    - summary の「テスト結果」表の備考と、レコード単位の `results.md` の「備考」に、Google Test の `# GetParam() = ...` がそのまま載ります。
    - 定義単位の `results.md` の「実行レコード」には載りません。
- `# GetParam() = ...` の内容は、パラメーターの型の印字方法で決まります。Google Test は、`PrintTo()` の多重定義、`operator<<`、バイト列の順に探して印字します。

    | テスト | パラメーターの型 | 現在の備考 |
    | --- | --- | --- |
    | `param-test1.cc` | `tuple<int, int, int>` | `# GetParam() = (1, 3, 3)` (要素の意味が分からない) |
    | `param-test2.cc`、`param-test3.cc` | 構造体。`operator<<` が名前だけを出力 | `# GetParam() = 2_3_6` (値が分からない) |
    | `tcp_allocation_failure_test.cc` | 構造体。印字方法なし | `# GetParam() = 24-byte object <48-43 48-00 ...>` (バイト列。ポインターのアドレスを含み、ビルドごとに変わる) |

- `param-test2.cc` と `param-test3.cc` は、`PrintToStringParamName()` でテスト名 (`MultiplyTest/2_3_6`) を作るため、`operator<<` に名前だけを出力させています。Google Test のテスト名には英数字と `_` しか使えないため、すべての値を印字するように変えると、テスト名の作り方も変える必要があります。
- .NET (`app/calc.net`) の `[Theory]` は、テスト メソッドごとに 1 レコードで、パラメーターごとに分かれていません。値は「実行結果」の VSTest の出力に `(a: -5, b: 5, expected: 0)` のように名前付きで載ります。
- 「テスト コード」節の `INSTANTIATE_TEST_SUITE_P` は、`framework/testfw/bin_internal/test_subprocedures.py` の `SourceIndex.report()` が、接頭辞が 1 文字以上ある登録だけを抜粋します。接頭辞が空の登録 (`INSTANTIATE_TEST_SUITE_P(, ParamTest1Test, ...)`) は、定義単位とレコード単位のどちらの `results.md` にも含まれません。`param-test1.cc` には、使われていない `get_test_code_c_cpp.awk` の書き方の制約を説明する `NOTE` コメントがあります (版 3 で訂正。[Q-019](#q-019-使われていないテスト-コード抽出の-awk-の扱い))。

## 現行の合意内容

ユーザーの指示は「`prompt/current/test-assert-comment-followup2-backlog.md` の対処プランを進める」(2026-10-22 の対話) です。各課題の「推奨される対応策」は、前案件のエージェントが記載した案であり、要件として扱い、採否と範囲を Q で確認しています。

版 4 時点で合意した内容は次のとおりです。

- 範囲: BL-001、BL-003、BL-004 を対処します。BL-002 は残課題に引き継ぎます ([Q-001](#q-001-本案件で対処する残課題))。
- BL-001:
    - インライン リンクと画像のリンク先、および参照リンクの定義行のリンク先を、整形の対象から外します ([Q-002](#q-002-リンク先を保護する記法))。
    - リンクのタイトルは整形の対象に残します ([Q-003](#q-003-リンクのタイトルの扱い))。
    - 修正によって整形結果が変わる既存の Markdown 2 件 (`app/zlib/packages/README.md`、`framework/docsfw/docs/references/requirements-functional-specification-research.md`) に `--in-place` を適用します ([Q-020](#q-020-リンク先の保護で整形結果が変わる既存の-markdown))。
- BL-003:
    - 動作の指定がない `EXPECT_CALL` の `[Pre-Assert手順]` と、`DoDefault()` だけの指定の両方を指摘します ([Q-004](#q-004-動作を注入しない呼び出しの診断の範囲))。
    - 規則名は `test-comment-expect-call-step-without-action` と `test-comment-expect-call-default-action` です ([Q-006](#q-006-追加する診断の規則名))。
    - 既存の 3 か所を `.Times()` に書き換えます ([Q-005](#q-005-dodefault-だけを指定した既存箇所の扱い))。
- BL-004:
    - Google Test の印字を値の出所とし、パラメーターの型に名前付きで印字する `PrintTo()` を定義する規約を定めます ([Q-007](#q-007-パラメーターの値の出所))。
    - summary の表、定義単位の「実行レコード」、レコード単位の文書の 3 か所に掲載します ([Q-008](#q-008-パラメーターの掲載先))。
    - C++ だけを対象とし、.NET の表し方は仕様として扱います ([Q-009](#q-009-net-のパラメーター-テストの扱い))。
    - テスト コード節には、テスト スイート名が一致するすべての登録を、行の分け方によらず抜粋します ([Q-010](#q-010-テスト-コード節の-instantiate_test_suite_p-の抜粋))。
    - 使われていない `get_test_code_c_cpp.awk` と、その呼び出し、テスト、参照を削除します ([Q-019](#q-019-使われていないテスト-コード抽出の-awk-の扱い))。
    - `about-test-phase.md` に .NET の表し方を明記します ([Q-014](#q-014-net-のパラメーターの表し方の明記))。
    - パラメーターは構造体とし、`tuple` を使いません。`param-test1.cc` を書き換えます ([Q-015](#q-015-tuple-を使うパラメーター-テストの扱い))。
    - 3 か所の項目名を「パラメーター」に改め、値は `# GetParam() = ` と `# TypeParam = ` を除いて表示します ([Q-016](#q-016-パラメーターの項目名と値の表示))。
    - `PrintTo()` は、すべての要素を宣言の順に `名前: 値` の形で `, ` で区切って印字し、文字列は `"..."` で囲みます ([Q-017](#q-017-パラメーターの印字の書式))。
    - バイト列の印字を検出したレコードの判定を WARNING にします ([Q-018](#q-018-印字方法のない型の検出))。
- 検証: 局所的な確認と、リポジトリ ルートの `make test` 1 回 ([Q-011](#q-011-受入時のテストの範囲))。
- Git: 受入条件の確認後に、サブモジュール、リポジトリ本体の順にコミットとプッシュを行い、CI を追跡します ([Q-012](#q-012-コミットとプッシュの扱い))。
- 前案件の 3 文書: 第 3 ゲートの承認後に `prompt/past/` へ `mv` で移動します ([Q-013](#q-013-前案件の残課題一覧の扱い))。

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | 各課題の「本来あるべき姿」(`test-assert-comment-followup2-backlog.md`) |
| 範囲・境界 | 確認済み | [Q-001](#q-001-本案件で対処する残課題)、[Q-009](#q-009-net-のパラメーター-テストの扱い)、[Q-013](#q-013-前案件の残課題一覧の扱い)、[Q-014](#q-014-net-のパラメーターの表し方の明記)、[Q-019](#q-019-使われていないテスト-コード抽出の-awk-の扱い)、[Q-020](#q-020-リンク先の保護で整形結果が変わる既存の-markdown) |
| 入力・根拠 | 確認済み | [調査結果](#調査結果) |
| 整合性・依存関係 | 確認済み | [Q-004](#q-004-動作を注入しない呼び出しの診断の範囲)、[Q-005](#q-005-dodefault-だけを指定した既存箇所の扱い)、[Q-007](#q-007-パラメーターの値の出所)、[Q-015](#q-015-tuple-を使うパラメーター-テストの扱い)、[Q-017](#q-017-パラメーターの印字の書式) |
| 実現性・例外 | 確認済み | [Q-002](#q-002-リンク先を保護する記法)、[Q-003](#q-003-リンクのタイトルの扱い) (一時フォルダーの試作で、既存の自己テストの合格を確認。第一者管理の Markdown 398 件のうち 2 件で整形結果が変わることを確認し、[Q-020](#q-020-リンク先の保護で整形結果が変わる既存の-markdown) で適用を決定)、[Q-008](#q-008-パラメーターの掲載先)、[Q-010](#q-010-テスト-コード節の-instantiate_test_suite_p-の抜粋)、[Q-016](#q-016-パラメーターの項目名と値の表示)、[Q-018](#q-018-印字方法のない型の検出) |
| 受入条件・検証 | 確認済み | [Q-011](#q-011-受入時のテストの範囲)、[Q-012](#q-012-コミットとプッシュの扱い)、[Q-020](#q-020-リンク先の保護で整形結果が変わる既存の-markdown) |

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-本案件で対処する残課題) | 本案件で対処する残課題 | 解決済み | なし |
| [Q-002](#q-002-リンク先を保護する記法) | リンク先を保護する記法 (BL-001) | 解決済み | [Q-001](#q-001-本案件で対処する残課題) |
| [Q-003](#q-003-リンクのタイトルの扱い) | リンクのタイトルの扱い (BL-001) | 解決済み | [Q-002](#q-002-リンク先を保護する記法) |
| [Q-004](#q-004-動作を注入しない呼び出しの診断の範囲) | 動作を注入しない呼び出しの診断の範囲 (BL-003) | 解決済み | [Q-001](#q-001-本案件で対処する残課題) |
| [Q-005](#q-005-dodefault-だけを指定した既存箇所の扱い) | DoDefault() だけを指定した既存箇所の扱い (BL-003) | 解決済み | [Q-004](#q-004-動作を注入しない呼び出しの診断の範囲) |
| [Q-006](#q-006-追加する診断の規則名) | 追加する診断の規則名 (BL-003) | 解決済み | [Q-004](#q-004-動作を注入しない呼び出しの診断の範囲) |
| [Q-007](#q-007-パラメーターの値の出所) | パラメーターの値の出所 (BL-004) | 解決済み | [Q-001](#q-001-本案件で対処する残課題) |
| [Q-008](#q-008-パラメーターの掲載先) | パラメーターの掲載先 (BL-004) | 解決済み | [Q-007](#q-007-パラメーターの値の出所) |
| [Q-009](#q-009-net-のパラメーター-テストの扱い) | .NET のパラメーター テストの扱い (BL-004) | 解決済み | [Q-001](#q-001-本案件で対処する残課題) |
| [Q-010](#q-010-テスト-コード節の-instantiate_test_suite_p-の抜粋) | テスト コード節の INSTANTIATE_TEST_SUITE_P の抜粋 (BL-004) | 解決済み | [Q-007](#q-007-パラメーターの値の出所) |
| [Q-011](#q-011-受入時のテストの範囲) | 受入時のテストの範囲 | 解決済み | [Q-001](#q-001-本案件で対処する残課題) |
| [Q-012](#q-012-コミットとプッシュの扱い) | コミットとプッシュの扱い | 解決済み | なし |
| [Q-013](#q-013-前案件の残課題一覧の扱い) | 前案件の残課題一覧の扱い | 解決済み | [Q-001](#q-001-本案件で対処する残課題) |
| [Q-014](#q-014-net-のパラメーターの表し方の明記) | .NET のパラメーターの表し方の明記 (BL-004) | 解決済み | [Q-009](#q-009-net-のパラメーター-テストの扱い) |
| [Q-015](#q-015-tuple-を使うパラメーター-テストの扱い) | tuple を使うパラメーター テストの扱い (BL-004) | 解決済み | [Q-007](#q-007-パラメーターの値の出所) |
| [Q-016](#q-016-パラメーターの項目名と値の表示) | パラメーターの項目名と値の表示 (BL-004) | 解決済み | [Q-008](#q-008-パラメーターの掲載先) |
| [Q-017](#q-017-パラメーターの印字の書式) | パラメーターの印字の書式 (BL-004) | 解決済み | [Q-007](#q-007-パラメーターの値の出所) |
| [Q-018](#q-018-印字方法のない型の検出) | 印字方法のない型の検出 (BL-004) | 解決済み | [Q-007](#q-007-パラメーターの値の出所) |
| [Q-019](#q-019-使われていないテスト-コード抽出の-awk-の扱い) | 使われていないテスト コード抽出の awk の扱い (BL-004) | 解決済み | [Q-010](#q-010-テスト-コード節の-instantiate_test_suite_p-の抜粋) |
| [Q-020](#q-020-リンク先の保護で整形結果が変わる既存の-markdown) | リンク先の保護で整形結果が変わる既存の Markdown (BL-001) | 解決済み | [Q-002](#q-002-リンク先を保護する記法)、[Q-011](#q-011-受入時のテストの範囲) |

## 明確化のための確認項目

再質問や過去の質問への補足質問は、過去のやり取りを編集せず、新たな質問番号 (Q-ID) で継続します。意図不明とされた質問も回答済みとして確定し、そのターンの回答を踏まえて新たな質問番号で再生成します。

### Q-001 本案件で対処する残課題

**Status**

解決済み

**Question**

前案件の残課題 BL-001〜BL-004 のうち、本案件で対処するものはどれですか。

**Context**

4 件は、変更対象と規模が大きく異なります。

| 課題 | 変更対象 | 規模 |
| --- | --- | --- |
| BL-001 リンク先への空白の挿入 | `framework/docsfw` の text_style_jp | 小。保護パターンの追加と自己テスト |
| BL-002 ポインターの条件の常設の点検 | CI コンテナー (別リポジトリ `oracle-linux-container`)、`framework/makefw`、CI | 大。clang-tidy のビルドまたは導入、`compile_commands.json` の生成、Linux と Windows の CI 手順、ポインター以外の指摘の抑止の調整 |
| BL-003 動作を注入しない呼び出しの診断 | `framework/docsfw` の text_style_jp、cplat のテスト 3 か所 | 小 |
| BL-004 パラメーターの掲載 | `framework/testfw` の結果生成と規範、パラメーター テスト 4 ファイル | 中。表し方の検討が先に要る |

BL-001 と BL-003 は同じツールの変更で、まとめて検証できます。BL-004 は testfw の結果の Markdown と規範の変更で、BL-001、BL-003 とは独立しています。BL-002 は、コンテナーと makefw の変更を伴い、CI の時間も増えるため、他の 3 件と独立した判断が多く残ります (clang-tidy の入手方法、対象の構成、CI での失敗の扱いなど)。BL-002 を含める場合は、次の版でこれらを追加の Q として確認します。

**Answer**

選択方式: 複数選択

- [x] A: BL-001 リンク先への空白の挿入
- [ ] B: BL-002 ポインターの条件の常設の点検
- [x] C: BL-003 動作を注入しない呼び出しの診断
- [x] D: BL-004 パラメーター テストのパラメーターの掲載
- [ ] E: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A、C、D  
理由: BL-001、BL-003、BL-004 は、このリポジトリの framework の中で完結し、局所テストで検証できます。BL-002 は、別リポジトリのコンテナーの変更と CI の構成の判断が中心で、他の 3 件と検証の方法も異なるため、独立した案件に分けると、最終仕様の承認の単位を小さく保てます。BL-002 を見送る場合は、本案件の残課題一覧に引き継ぎます。

**Related**

[Q-013](#q-013-前案件の残課題一覧の扱い)

**Assessment**

BL-001、BL-003、BL-004 を本案件の対象とします。推奨どおりです。BL-002 は本案件の残課題一覧に引き継ぎます。

### Q-002 リンク先を保護する記法

**Status**

解決済み

**Question**

BL-001 で、整形の対象から外すリンク先の記法はどれですか。

**Context**

Markdown でリンク先を書く記法は 3 種類あります。いずれもリンク先は文字どおりに解釈されるため、空白が 1 文字入るだけで壊れます。

```markdown
[表示](#注入-pre-assert手順)          ← インライン リンク。画像 ![alt](path) も同じ形
[S01]: ./foo-bar手順.md "タイトル"    ← 参照リンクの定義行
<a href="./foo-bar手順.md">表示</a>   ← HTML の属性
```

保護は、整形の前にリンク先を退避し、整形の後に戻す方式です。C/C++ のコメントの整形で使っている `](...)` の保護パターンを Markdown にも使えます。リンクの表示文字列 (`[...]` の中) は、従来どおり整形します。

第一者管理の Markdown には、HTML の `href` と `src` に日本語を含むものはありません ([調査結果](#bl-001-リンク先への空白の挿入))。HTML の属性は、`<a href="...">` の行全体が HTML ブロックとして扱われるかどうかでも整形の結果が変わるため、保護の対象にすると確認の範囲が広がります。

**Answer**

選択方式: 複数選択

- [x] A: (推奨) インライン リンクと画像のリンク先 (`](...)` の中)
- [x] B: (推奨) 参照リンクの定義行のリンク先 (`[id]:` の直後の URL)
- [ ] C: HTML の `href` と `src` 属性の値
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A、B  
理由: 課題の再現例 3 件のうち 3 件を解消し、Markdown のリンク先の記法をすべて含みます。HTML の属性は、現時点で該当する文書がなく、保護による効果を確認できないためです。

**Related**

[Q-003](#q-003-リンクのタイトルの扱い)、[Q-020](#q-020-リンク先の保護で整形結果が変わる既存の-markdown)

**Assessment**

インライン リンクと画像のリンク先、および参照リンクの定義行のリンク先を保護します。課題の再現例 3 件をすべて含みます。HTML の属性は対象外です。

### Q-003 リンクのタイトルの扱い

**Status**

解決済み

**Question**

BL-001 で、リンク先に続くタイトル (`"..."`) を整形の対象に残しますか。

**Context**

リンク先の後ろには、マウスを重ねたときに表示されるタイトルを書けます。リポジトリでは、参照リンクの定義行で使っています。

```markdown
[S02]: https://www.ipa.go.jp/archive/publish/tn20191220.html "IPA ユーザーのための要件定義ガイド 第 2 版"
```

タイトルは読み手に表示される文章のため、本文と同じ規範で整形する考え方と、リンク先と一体の記法として保護する考え方があります。

案 A (タイトルを整形の対象に残す) の場合、`"第2版"` は `"第 2 版"` に整形され、リンク先だけが保護されます。  
案 B (タイトルも保護する) の場合、`"第2版"` はそのまま残ります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) タイトルを整形の対象に残し、リンク先だけを保護する。
- [ ] B: タイトルも保護し、整形しない。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: タイトルは表示される文章で、現在も整形されています。保護する範囲を、壊れると機能を失うリンク先に限ると、既存の文書の整形結果を変えずに済みます。

**Related**

[Q-002](#q-002-リンク先を保護する記法)、[Q-020](#q-020-リンク先の保護で整形結果が変わる既存の-markdown)

**Assessment**

タイトルは整形の対象に残し、リンク先だけを保護します。[Q-002](#q-002-リンク先を保護する記法) の回答と整合します。

### Q-004 動作を注入しない呼び出しの診断の範囲

**Status**

解決済み

**Question**

BL-003 で、text_style_jp に追加する診断の範囲はどれですか。

**Context**

`about-test-phase.md` の「動作を注入しない呼び出しの期待」は、次の 2 点を定めています。

1. `DoDefault()` だけを渡す `EXPECT_CALL` は、動作の指定をやめて `.Times(n)` で書く。
2. 動作を注入しない `EXPECT_CALL` には、`[Pre-Assert手順]` を書かない。

BL-003 の記載は 2. だけを対象にし、`DoDefault()` の指定を動作の注入とみなす案でした。この案では、1. の逸脱は検出されず、`DoDefault()` だけの指定に `[Pre-Assert手順]` を書くと、既存の診断 `test-comment-expect-call-step` がむしろ要求します。現に、1. に反する既存箇所が 3 件あります ([調査結果](#bl-003-動作を注入しない呼び出しの診断))。

```cpp
// 案 A では、次の 2 文がどちらも指摘される
EXPECT_CALL(mock_cplat, cplat_malloc(_))
    .Times(1); // [Pre-Assert確認_正常系] - ...
               // [Pre-Assert手順] - ...        ← 2. 動作がないのに手順がある
EXPECT_CALL(mock_cplat, cplat_malloc(_))
    .WillOnce(DoDefault()); // [Pre-Assert確認_正常系] - ...  ← 1. Times(1) で書く

// 次の文は指摘しない (DoDefault() と別の動作の組み合わせ)
EXPECT_CALL(mock_cplat, cplat_malloc(_))
    .WillOnce(Return(nullptr))
    .WillRepeatedly(DoDefault()); // [Pre-Assert確認_異常系] - ...
                                  // [Pre-Assert手順] - ...
```

案 B では 2. だけを指摘し、`DoDefault()` だけの指定は動作がないものとして扱います。`DoDefault()` だけの指定に `[Pre-Assert手順]` があれば指摘しますが、`.Times(n)` への書き換えは求めません。  
案 C は BL-003 の記載どおりで、`DoDefault()` を動作の注入とみなします。上記の 3 件は指摘されません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 1. と 2. の両方を指摘する。
- [ ] B: 2. だけを指摘し、`DoDefault()` だけの指定は動作がないものとして扱う。
- [ ] C: 2. だけを指摘し、`DoDefault()` の指定は動作の注入とみなす (BL-003 の記載どおり)。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 規範の 2 点を両方とも機械的に検査でき、既存の診断 `test-comment-expect-call-step` との矛盾 (`DoDefault()` だけの指定に手順を要求する) もなくなります。

**Related**

[Q-005](#q-005-dodefault-だけを指定した既存箇所の扱い)、[Q-006](#q-006-追加する診断の規則名)

**Assessment**

規範の 2 点 (`DoDefault()` だけの指定、動作がない `EXPECT_CALL` の `[Pre-Assert手順]`) の両方を指摘します。既存の `test-comment-expect-call-step` は、`DoDefault()` だけの指定に `[Pre-Assert手順]` を要求しないように合わせて変更する必要があります (最終仕様で定めます)。

### Q-005 DoDefault() だけを指定した既存箇所の扱い

**Status**

解決済み

**Question**

BL-003 で、`DoDefault()` だけを指定している既存の 3 か所を、本案件で規範どおりに書き換えますか。

**Context**

3 か所は `app/cplat` のテストです ([調査結果](#bl-003-動作を注入しない呼び出しの診断))。規範どおりに書き換えると、次のようになります。gMock では、動作を指定しない呼び出しは `ON_CALL` の既定の動作 (このリポジトリの mock では製品の処理への委譲) になるため、テストの振る舞いは変わりません。

```cpp
// 現状 (promptAllocFailureTest.cc)
EXPECT_CALL(mock_cplat, cplat_malloc(_))
    .WillRepeatedly(
        DoDefault()); // [Pre-Assert確認_正常系] - 書式バッファー以外の cplat_malloc() が任意の回数呼び出されること。
                      // [Pre-Assert手順] - cplat_malloc() から製品 (prod) の処理へ委譲する。

// 書き換え後
EXPECT_CALL(mock_cplat, cplat_malloc(_))
    .Times(AnyNumber()); // [Pre-Assert確認_正常系] - 書式バッファー以外の cplat_malloc() が任意の回数呼び出されること。
```

| 現状の指定 | 書き換え後 |
| --- | --- |
| `.WillRepeatedly(DoDefault())` | `.Times(AnyNumber())` |
| `.WillOnce(DoDefault()).WillRepeatedly(DoDefault())` | `.Times(AtLeast(1))` |
| `.WillOnce(DoDefault())` | `.Times(1)` |

`[Pre-Assert手順]` がなくなるため、エビデンスの `### 手順` から 3 行が消えます。確認件数は変わりません。

[Q-004](#q-004-動作を注入しない呼び出しの診断の範囲) で A を選んだ場合、書き換えないと text_style_jp の診断が 3 件残ります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 本案件で書き換える。
- [ ] B: 書き換えず、本案件の残課題一覧に記録する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 規範と診断を追加するときは、既存のコードも同じ変更で移行する方針がこれまでの案件で示されています。3 か所の書き換えはテストの振る舞いを変えません。

**Related**

[Q-004](#q-004-動作を注入しない呼び出しの診断の範囲)

**Assessment**

既存の 3 か所を `.Times()` に書き換え、`[Pre-Assert手順]` を削除します。[Q-004](#q-004-動作を注入しない呼び出しの診断の範囲) の A と整合し、変更後に診断の指摘は残りません。

### Q-006 追加する診断の規則名

**Status**

解決済み

**Question**

BL-003 で追加する診断の規則名 (`--dry-run` の出力と `text_style_jp.md` の規則表に載る ID) はどれにしますか。

**Context**

既存の関連する規則は `test-comment-expect-call-check` (`[Pre-Assert確認_*]` の不足) と `test-comment-expect-call-step` (`[Pre-Assert手順]` の不足) です。[Q-004](#q-004-動作を注入しない呼び出しの診断の範囲) の A では、指摘が 2 種類になります。

| 指摘の内容 | 案 A | 案 B |
| --- | --- | --- |
| 動作の指定がないのに `[Pre-Assert手順]` がある | `test-comment-expect-call-step-without-action` | `test-comment-expect-call-extra-step` |
| `DoDefault()` だけを指定している | `test-comment-expect-call-default-action` | `test-comment-expect-call-default-only` |

[Q-004](#q-004-動作を注入しない呼び出しの診断の範囲) で B または C を選んだ場合は、1 行目の名前だけを使います。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `test-comment-expect-call-step-without-action` と `test-comment-expect-call-default-action`
- [ ] B: `test-comment-expect-call-extra-step` と `test-comment-expect-call-default-only`
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 既存の `test-comment-expect-call-step` に「何がない状態か」を付け足す形で、指摘の条件を名前から読み取れます。案 B の `extra` は「余分」の理由を表さず、`default-only` は何が既定なのかが名前から分かりにくくなります。

**Related**

[Q-004](#q-004-動作を注入しない呼び出しの診断の範囲)

**Assessment**

規則名は `test-comment-expect-call-step-without-action` と `test-comment-expect-call-default-action` とします。

### Q-007 パラメーターの値の出所

**Status**

解決済み

**Question**

BL-004 で、レコードごとのパラメーターの値をどこから得て掲載しますか。

**Context**

現在は、Google Test がテスト一覧に出力する `# GetParam() = ...` が備考に載ります。この文字列は、パラメーターの型の印字方法で決まり、型によっては値が分からないか、バイト列になります ([調査結果](#bl-004-パラメーター-テストのパラメーターの掲載))。値の出所には、次の 3 案があります。

**案 A: Google Test の印字を使い、印字方法の規約を定める**

パラメーターの型には、各要素を名前付きで印字する `PrintTo()` を定義することを規範 (`about-test-phase.md`) に定めます。結果の生成は、現在の `# GetParam() = ...` をそのまま使います。

```cpp
struct multiply_case
{
    int a;
    int b;
    int expected;
};

static void PrintTo(const multiply_case &p, std::ostream *os)
{
    *os << "a=" << p.a << ", b=" << p.b << ", expected=" << p.expected;
}
// → # GetParam() = a=2, b=3, expected=6
```

`Combine()`、`Range()`、`ValuesIn()` で生成したパラメーターも、レコードごとに実際の値が載ります。一方、次の移行が要ります。

- `tuple` を使う `param-test1.cc` は、構造体に変えるか、`tuple` のままでは要素の名前が載らないことを容認する必要があります (次の版で確認します)。
- `param-test2.cc` と `param-test3.cc` は、テスト名を `PrintToStringParamName()` から、名前の要素を返す関数に変える必要があります。テスト名 (`MultiplyTest/2_3_6`) は変わりません。
- `tcp_allocation_failure_test.cc` には `PrintTo()` を追加します。

**案 B: テスト コードの `INSTANTIATE_TEST_SUITE_P` の記述から抜き出す**

テスト コードの変更は要りません。`Values(make_tuple(2, 3, 6), ...)` の記述を解析して、レコードの番号と対応付けます。`Combine()`、`Range()`、`ValuesIn()` や、別の場所で定義した配列を使う場合は値を得られず、要素の名前も分かりません。

**案 C: testfw に値を記録する仕組みを追加する**

テスト本体で値を記録するマクロ (例: `TESTFW_PARAM("a", p.a)`) を testfw に追加し、その出力を結果に載せます。値と名前は確実に載りますが、Google Test の標準の仕組みではなく、テスト本体に記録の行が増えます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) Google Test の印字を使い、パラメーターの型に名前付きで印字する `PrintTo()` を定義する規約を定める。
- [ ] B: テスト コードの `INSTANTIATE_TEST_SUITE_P` の記述から抜き出す。
- [ ] C: testfw に値を記録する仕組みを追加する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: Google Test の標準の印字の仕組みに従うため、パラメーターの生成方法によらず、実際に実行した値がレコードごとに載ります。結果の生成側の変更は掲載先の追加だけで済み、移行の対象も 4 ファイルにとどまります。

**Related**

[Q-008](#q-008-パラメーターの掲載先)、[Q-010](#q-010-テスト-コード節の-instantiate_test_suite_p-の抜粋)、[Q-015](#q-015-tuple-を使うパラメーター-テストの扱い)、[Q-017](#q-017-パラメーターの印字の書式)、[Q-018](#q-018-印字方法のない型の検出)

**Assessment**

Google Test の印字を値の出所とし、パラメーターの型に名前付きで印字する `PrintTo()` を定義する規約を定めます。規約の詳細 (`tuple` の扱い、印字の書式、印字方法のない型の検出) は [Q-015](#q-015-tuple-を使うパラメーター-テストの扱い)、[Q-017](#q-017-パラメーターの印字の書式)、[Q-018](#q-018-印字方法のない型の検出) で確認します。

### Q-008 パラメーターの掲載先

**Status**

解決済み

**Question**

BL-004 で、レコードごとのパラメーターを掲載する場所はどれですか。

**Context**

パラメーター テストの結果は、3 種類の Markdown に分かれます。

| 掲載先 | 現在の掲載 |
| --- | --- |
| summary の「テスト結果」表の備考列 | `# GetParam() = ...` が載る |
| 定義単位の `results/<Suite>.<Test>/results.md` の「実行レコード」の各レコード | 載らない (判定、実施日時、詳細へのリンクだけ) |
| レコード単位の `results.md` の「備考」 | `# GetParam() = ...` が載る |

定義単位の `results.md` は、テスト項目の詳細 (状態、手順、確認) と、すべてのレコードの結果を 1 か所で読める文書です。ここにパラメーターがないため、各レコードの値を知るには、summary に戻るか、レコード単位の文書を開く必要があります。

案 B を選んだ場合の「実行レコード」の例です。

```markdown
### MultiplicationTests1/ParamTest1Test.MultiplyTest/0

- 判定: PASSED
- 実施日時: 2026-10-17 17:26:17 JST
- パラメーター: `a=2, b=3, expected=6`
- 詳細: [results.md](MultiplicationTests1/0/results.md)
```

**Answer**

選択方式: 複数選択

- [x] A: (推奨) summary の「テスト結果」表の備考列 (現状を維持)
- [x] B: (推奨) 定義単位の `results.md` の「実行レコード」の各レコード (新設)
- [x] C: (推奨) レコード単位の `results.md` の「備考」(現状を維持)
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A、B、C  
理由: 現在の掲載を維持したまま、テスト項目の詳細と同じ文書で各レコードの値を読めるようにします。項目名を「備考」のままにするか「パラメーター」に改めるかは、[Q-007](#q-007-パラメーターの値の出所) の回答を踏まえて次の版で確認します。

**Related**

[Q-007](#q-007-パラメーターの値の出所)、[Q-016](#q-016-パラメーターの項目名と値の表示)

**Assessment**

3 か所すべてに掲載します。項目名と値の表示は [Q-016](#q-016-パラメーターの項目名と値の表示) で確認します。

### Q-009 .NET のパラメーター テストの扱い

**Status**

解決済み

**Question**

BL-004 で、.NET のパラメーター テスト (xUnit の `[Theory]`) を本案件の対象に含めますか。

**Context**

.NET の結果は、テスト メソッドごとに 1 レコードで、`[InlineData]` ごとに分かれていません ([調査結果](#bl-004-パラメーター-テストのパラメーターの掲載))。値は「実行結果」の VSTest の出力に名前付きで載っています。

```text
  成功 CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult(a: -5, b: 5, expected: 0) [< 1 ms]
```

C++ と同じく「レコードごとのパラメーター」を載せるには、.NET の結果をパラメーターごとのレコードに分ける変更が要り、C++ の変更とは別の規模の作業になります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 本案件では C++ だけを対象とし、.NET は本案件の残課題一覧に記録する。
- [ ] B: .NET も本案件の対象に含める。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: 残課題にする必要はない。仕様として扱う。

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: .NET の値は名前付きですでに結果に残っており、BL-004 が挙げた「値が一切残らない」問題は C++ に限られます。.NET のレコードの分割は、結果の構造の変更を伴うため、独立して検討したほうが判断しやすくなります。

**Related**

[Q-001](#q-001-本案件で対処する残課題)、[Q-014](#q-014-net-のパラメーターの表し方の明記)

**Assessment**

C++ だけを対象とします。補足により、.NET の表し方 (パラメーターごとにレコードを分けず、値は「実行結果」の出力で確認する) は残課題とせず、仕様として扱います。選択肢 A の後半 (残課題一覧への記録) は、補足に従って適用しません。仕様を規範に明記するかどうかは [Q-014](#q-014-net-のパラメーターの表し方の明記) で確認します。

### Q-010 テスト コード節の INSTANTIATE_TEST_SUITE_P の抜粋

**Status**

解決済み

**Question**

BL-004 で、「テスト コード」節に `INSTANTIATE_TEST_SUITE_P` をどのように抜粋しますか。

**Context**

現在は、`get_test_code_c_cpp.awk` がレコードの接頭辞と一致する登録を探して抜粋します。そのため、接頭辞が空の登録は定義単位の `results.md` に含まれず、`param-test1.cc` では 3 件の登録のうち 2 件だけが載ります ([調査結果](#bl-004-パラメーター-テストのパラメーターの掲載))。また、抜粋のために、テスト コードに「接頭辞とテスト スイート名を同じ行に書き、行末に `,` を付ける」という書き方の制約を課しています。

案 A では、テスト スイート名が一致するすべての登録を、行の分け方によらず抜粋します。書き方の制約と `param-test1.cc` の `NOTE` コメントはなくなります。  
案 B では、`INSTANTIATE_TEST_SUITE_P` を抜粋しません。パラメーターの値は [Q-008](#q-008-パラメーターの掲載先) の掲載先で読めるため、テスト コード節は `TEST_P` の本体だけになります。`Combine()` などの生成方法はテスト コード節から読めなくなります。  
案 C では、現状を維持します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) テスト スイート名が一致するすべての登録を、行の分け方によらず抜粋する。
- [ ] B: `INSTANTIATE_TEST_SUITE_P` を抜粋しない。
- [ ] C: 現状を維持する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 定義単位の文書は全レコードを扱うため、登録もすべて載るのが一貫します。clang-format の折り返しで書き方の制約が崩れる心配もなくなります (`tcp_allocation_failure_test.cc` の登録はすでに制約に反しています)。

**Related**

[Q-007](#q-007-パラメーターの値の出所)、[Q-008](#q-008-パラメーターの掲載先)、[Q-019](#q-019-使われていないテスト-コード抽出の-awk-の扱い)

**Assessment**

テスト スイート名が一致するすべての登録を、行の分け方によらず抜粋します。`param-test1.cc` の書き方の制約を説明する `NOTE` コメントは削除の対象になります。

### Q-011 受入時のテストの範囲

**Status**

解決済み

**Question**

成果物の受入時に実行するテストの範囲はどれですか。

**Context**

変更の影響は次のとおりです。

| 課題 | 影響を受けるもの | 局所的な確認 |
| --- | --- | --- |
| BL-001、BL-003 | text_style_jp の整形と診断 | `text_style_jp.py --test`、`framework/docsfw` の `bin_test/`、第一者管理の Markdown 全体への `--dry-run` (整形の結果が変わらないこと)、全テスト コードへの `--mode cpp --dry-run` (新しい診断の件数) |
| BL-003 の移行 | `app/cplat` のテスト 3 ファイル | `app/cplat` の該当テスト ディレクトリでの `make test` |
| BL-004 | testfw の結果生成と、パラメーター テストを持つ app | `framework/testfw` の `bin_test/`、`app/tutorial` と `app/porter` の `make test`、結果の Markdown の目視確認 |

リポジトリ ルートでの `make test` は、全 app のテストを実行するため、時間がかかります。testfw の結果生成は全 app のテスト結果に影響するため、パラメーター テスト以外の結果が変わらないことの確認には全体のテストが確実です。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 上表の局所的な確認に加え、リポジトリ ルートで `make test` を 1 回実行する。
- [ ] B: 上表の局所的な確認だけを行う。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A (BL-004 を対象に含める場合)、B (含めない場合)  
理由: BL-004 は全 app のテスト結果の Markdown を生成する処理を変更するため、パラメーター テスト以外の結果が変わらないことを全体で確かめる必要があります。BL-004 を含めない場合、変更は text_style_jp と cplat の 3 ファイルに限られ、局所的な確認で足ります。

**Related**

[Q-001](#q-001-本案件で対処する残課題)、[Q-020](#q-020-リンク先の保護で整形結果が変わる既存の-markdown)

**Assessment**

局所的な確認に加え、受入時にリポジトリ ルートで `make test` を 1 回実行します。BL-004 を対象に含めたため、推奨の条件と整合します。

### Q-012 コミットとプッシュの扱い

**Status**

解決済み

**Question**

成果物の作成後に、コミットとプッシュをエージェントが行いますか。

**Context**

`AGENTS.md` は、ユーザーの個別の指示があるまでステージングとコミットを行わないことを定めています。変更はサブモジュール (`framework/docsfw`、`framework/testfw`、`app/cplat` など) とリポジトリ本体の参照の更新にまたがります。

**Answer**

選択方式: 単一選択

- [x] A: 受入条件の確認後に、サブモジュール、リポジトリ本体の順にコミットとプッシュを行い、CI を追跡する。
- [ ] B: コミットとプッシュは行わない (ユーザーが行う)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨なし  
理由: Git の書き込み操作の可否はユーザーの判断事項で、エージェントが推奨する根拠がありません。

**Assessment**

受入条件の確認後に、サブモジュール、リポジトリ本体の順にコミットとプッシュを行い、CI を追跡します。この回答を本案件のコミットとプッシュに限る許可として扱います。

### Q-013 前案件の残課題一覧の扱い

**Status**

解決済み

**Question**

前案件の残課題一覧 `prompt/current/test-assert-comment-followup2-backlog.md` を、本案件でどのように扱いますか。

**Context**

前案件の検討用 Markdown、最終仕様、残課題一覧は `prompt/current/` にあります。過去の案件の文書は `prompt/past/` に置かれています。本案件で対処しない課題 ([Q-001](#q-001-本案件で対処する残課題) で選ばなかったもの) は、本案件の残課題一覧 `test-assert-comment-followup3-backlog.md` に引き継ぐ予定です。

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) 変更しない。対処しない課題は本案件の残課題一覧に引き継ぎ、前案件の文書の移動はユーザーが行う。
- [x] B: 本案件の完了時に、前案件の 3 文書を `prompt/past/` へ移動する (`mv` で行い、ステージングはしない)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:06:18 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 前案件の文書は合意済みの記録で、`prompt/past/` へ移動する時期と要否は、案件の外の運用で決まるためです。

**Related**

[Q-001](#q-001-本案件で対処する残課題)

**Assessment**

第 3 ゲートで成果物の承認を受けた後に、前案件の 3 文書 (`test-assert-comment-followup2-discussion.md`、`-design.md`、`-backlog.md`) を `mv` で `prompt/past/` へ移動します。ステージングはしません。

### Q-014 .NET のパラメーターの表し方の明記

**Status**

解決済み

**Question**

.NET のパラメーター テストの表し方 (パラメーターごとにレコードを分けず、値は「実行結果」の出力で確認する) を、規範に明記しますか。

**Context**

[Q-009](#q-009-net-のパラメーター-テストの扱い) の補足で、.NET の表し方を残課題とせず、仕様として扱うことになりました。本案件で `about-test-phase.md` に C++ のパラメーターの掲載を定めると、.NET については何も書かれていない状態になります。読み手は、.NET でパラメーターが「実行レコード」に載らないことを、規範の漏れか仕様かを区別できません。

案 A の場合、`about-test-phase.md` のパラメーター テストの結果の節に、次のような段落を加えます。

```markdown
.NET のパラメーター テスト (xUnit の `[Theory]` など) は、テスト メソッドごとに 1 レコードとし、パラメーターごとに分けません。各パラメーターの値は、「実行結果」のテスト ランナーの出力に名前付きで載ります。
```

案 B の場合、規範は C++ の掲載だけを定め、.NET には触れません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `about-test-phase.md` に .NET の表し方を明記する。
- [ ] B: 明記しない。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:25:19 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 仕様として扱う決定を規範に残すと、C++ と .NET の違いが意図したものであることを、後の読み手が確認できます。

**Related**

[Q-009](#q-009-net-のパラメーター-テストの扱い)

**Assessment**

`about-test-phase.md` のパラメーター テストの結果の節に、.NET はテスト メソッドごとに 1 レコードとし、値は「実行結果」のテスト ランナーの出力で確認することを明記します。

### Q-015 tuple を使うパラメーター テストの扱い

**Status**

解決済み

**Question**

`tuple` をパラメーターに使う `param-test1.cc` を、規約に合わせてどのように扱いますか。

**Context**

[Q-007](#q-007-パラメーターの値の出所) で、パラメーターの型に名前付きで印字する `PrintTo()` を定義する規約を定めることになりました。`std::tuple` は標準ライブラリの型のため、利用者が `PrintTo()` を定義して印字を変えることはできません (Google Test は型と同じ名前空間の `PrintTo()` を探すため、`std` 名前空間への追加が必要になり、C++ の規格で禁じられています)。そのため、`tuple` のままでは要素の名前が載りません。

`param-test1.cc` は、`tuple` の使い方と、接頭辞の異なる複数の登録 (`INSTANTIATE_TEST_SUITE_P`) を示すチュートリアルです。構造体を使う例は `param-test2.cc` と `param-test3.cc` にあります。

案 A の場合、`param-test1.cc` を構造体と `PrintTo()` に書き換えます。規約は「パラメーターは名前付きの要素を持つ構造体にする」と定め、`tuple` の例はなくなります。

```cpp
INSTANTIATE_TEST_SUITE_P(MultiplicationTests1, ParamTest1Test,
                         Values(ParamTest1TestParam{2, 3, 6}, ParamTest1TestParam{4, 5, 20}));
// → # GetParam() = a: 2, b: 3, expected: 6
```

案 B の場合、`tuple` を規約の例外として認め、`tuple` の印字は値だけ (`(2, 3, 6)`) になることを容認します。要素の意味は、テスト コードの `[状態]` の説明文 (`パラメーターから a を取り出す。` など) で補います。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 構造体と `PrintTo()` に書き換え、規約で `tuple` を使わないことを定める。
- [ ] B: `tuple` を規約の例外として認め、値だけの印字を容認する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:25:19 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: BL-004 の目的 (テスト コードを読み解かずに各レコードの値を確認できる) を、例外なく満たせます。接頭辞の異なる複数の登録の例は、構造体に変えても残せます。

**Related**

[Q-007](#q-007-パラメーターの値の出所)、[Q-017](#q-017-パラメーターの印字の書式)

**Assessment**

`param-test1.cc` を構造体と `PrintTo()` に書き換え、規約で `tuple` を使わないことを定めます。接頭辞の異なる複数の登録は残します。

### Q-016 パラメーターの項目名と値の表示

**Status**

解決済み

**Question**

[Q-008](#q-008-パラメーターの掲載先) で決めた 3 か所に、パラメーターをどのような項目名と値で表示しますか。

**Context**

現在は、summary の列名とレコード単位の項目名が「備考」で、値は Google Test のテスト一覧の文字列 (`# GetParam() = ...`) をそのまま使っています。この文字列は、型パラメーターのテストでは `# TypeParam = ...` になります。現在のリポジトリには型パラメーターのテストはありません。

案 A (項目名を「パラメーター」に改め、値の接頭辞を除く) の場合です。

```markdown
| テスト ID | 結果 | パラメーター |
| --- | --- | --- |
| [ParamTest1Test.MultiplyTest/MultiplicationTests1/0](...) | PASSED | `a: 2, b: 3, expected: 6` |

- パラメーター: `a: 2, b: 3, expected: 6`
```

案 B (現在の項目名と値を維持する) の場合です。

```markdown
| テスト ID | 結果 | 備考 |
| --- | --- | --- |
| [ParamTest1Test.MultiplyTest/MultiplicationTests1/0](...) | PASSED | `# GetParam() = a: 2, b: 3, expected: 6` |

- 備考: `# GetParam() = a: 2, b: 3, expected: 6`
```

案 A では、型パラメーターのテストの `# TypeParam = int` も、接頭辞を除いて `int` と表示します。パラメーター テスト以外のテストと .NET のテストでは、現在と同じく空欄になります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 項目名を「パラメーター」に改め、値は `# GetParam() = ` と `# TypeParam = ` を除いて表示する。
- [ ] B: 項目名「備考」と Google Test の文字列をそのまま使う。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:25:19 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 列と項目の内容がパラメーターだけであることを名前で示せます。`# GetParam() = ` は Google Test の出力の書式で、読み手にとって意味のない文字列です。

**Related**

[Q-008](#q-008-パラメーターの掲載先)

**Assessment**

3 か所の項目名 (summary の列名を含む) を「パラメーター」に改め、値は `# GetParam() = ` と `# TypeParam = ` を除いて表示します。

### Q-017 パラメーターの印字の書式

**Status**

解決済み

**Question**

規約で定める `PrintTo()` の印字の書式はどれにしますか。

**Context**

.NET の VSTest は、パラメーターを `(a: -5, b: 5, expected: 0)` のように、`名前: 値` を `, ` で区切って出力します。文字列は `"..."` で囲みます。

| 案 | 例 (`param-test2.cc` の構造体) |
| --- | --- |
| A | `desc: "2_3_6", a: 2, b: 3, expected: 6` |
| B | `desc=2_3_6, a=2, b=3, expected=6` |

規約は、構造体のすべての要素を宣言の順に印字することとします。`tcp_allocation_failure_test.cc` の構造体は `name: "recv_compress_buf", size: 8264, occurrence: 2` のようになります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `名前: 値` を `, ` で区切り、文字列は `"..."` で囲む (.NET の出力と同じ形)。
- [ ] B: `名前=値` を `, ` で区切り、文字列は囲まない。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:25:19 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: .NET のエビデンスと同じ形で読めます。文字列を囲むと、空白や `,` を含む文字列でも要素の区切りと区別できます。

**Related**

[Q-007](#q-007-パラメーターの値の出所)、[Q-015](#q-015-tuple-を使うパラメーター-テストの扱い)

**Assessment**

`名前: 値` を `, ` で区切り、文字列は `"..."` で囲みます。構造体のすべての要素を宣言の順に印字します。

### Q-018 印字方法のない型の検出

**Status**

解決済み

**Question**

パラメーターの型に印字方法がない場合 (Google Test がバイト列を出力する場合) を、testfw で機械的に検出しますか。

**Context**

`PrintTo()` も `operator<<` もない型は、`# GetParam() = 24-byte object <48-43 48-00 ...>` のようなバイト列で印字されます ([調査結果](#bl-004-パラメーター-テストのパラメーターの掲載))。値が読めず、ポインターを含むとビルドごとに変わります。規約を定めても、新しいテストで `PrintTo()` を書き忘れると、この状態に戻ります。Google Test のバイト列の書式は `<数>-byte object <` で始まるため、文字列で検出できます。

検出した場合の扱いには、次の案があります。

- 案 A: そのレコードの判定を WARNING にします。WARNING は、現在 Google Test が `[ WARNING ]` を出力した場合の判定で、テストは失敗扱いにならず、summary の「集計」と終了時の表示に件数が出ます。
- 案 B: 判定は変えず、summary に注記 (`> [!WARNING]`) を出します。
- 案 C: 検出せず、規範とレビューで担保します。

`tuple` の値だけの印字 ([Q-015](#q-015-tuple-を使うパラメーター-テストの扱い)) や、`operator<<` が名前だけを出力する場合は、いずれの案でも検出できません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) バイト列の印字を検出し、そのレコードの判定を WARNING にする。
- [ ] B: バイト列の印字を検出し、判定は変えず summary に注記を出す。
- [ ] C: 検出しない。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 08:25:19 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: WARNING は「合格だが確認が要る」状態を表す既存の判定で、件数が集計とテストの終了時の表示に出るため、書き忘れに気付きやすくなります。テストの失敗にはならないため、CI を止めません。

**Related**

[Q-007](#q-007-パラメーターの値の出所)

**Assessment**

Google Test のバイト列の印字 (`<数>-byte object <`) を検出し、そのレコードの判定を WARNING にします。

### Q-019 使われていないテスト コード抽出の awk の扱い

**Status**

解決済み

**Question**

結果の生成に使われていない `framework/testfw/bin_internal/get_test_code_c_cpp.awk` を、本案件で削除しますか。

**Context**

版 1 の [調査結果](#bl-004-パラメーター-テストのパラメーターの掲載) と [Q-010](#q-010-テスト-コード節の-instantiate_test_suite_p-の抜粋) の Context では、「テスト コード」節の `INSTANTIATE_TEST_SUITE_P` を `get_test_code_c_cpp.awk` が抜粋すると記載しました。この記載は誤りでした。版 3 での再調査の結果は次のとおりです。

- 「テスト コード」節は、定義単位とレコード単位のどちらも `framework/testfw/bin_internal/test_subprocedures.py` の `SourceIndex.report()` が生成しています。登録の検出は正規表現 `INSTANTIATE_TEST_SUITE_P\s*\(\s*(\w+)\s*,` で、接頭辞が 1 文字以上ある登録だけに一致します。接頭辞が空の登録が載らない原因はこの正規表現です。行の分け方には依存しません。
- `get_test_code_c_cpp.awk` を呼び出す関数 `extract_code()` (`gtest_summary_groups.py`) は、どこからも呼ばれていません。awk を参照しているのは、`framework/testfw/bin_test/test_results_refinement.py` の 3 件のテスト、`exec_test_c_cpp.sh` の再生成判定用のハッシュの対象一覧、`app/general/docs/dotnet-test-results-design.md` のファイル一覧だけです。
- `param-test1.cc` の `NOTE` コメント (「get_test_code.awk での構文解析の都合で…」) は、この awk の制約を説明するもので、現在の抜粋には当てはまりません。

[Q-010](#q-010-テスト-コード節の-instantiate_test_suite_p-の抜粋) の決定 (テスト スイート名が一致するすべての登録を、行の分け方によらず抜粋する) は、`test_subprocedures.py` の正規表現の修正で実現します。awk を直す必要はありません。

案 A の場合、awk、`extract_code()`、awk を対象とする 3 件のテスト、ハッシュの対象一覧の項目、`dotnet-test-results-design.md` の記載を削除します。  
案 B の場合、awk は変更せず、本案件の残課題一覧に記録します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 本案件で削除する。
- [ ] B: 削除せず、本案件の残課題一覧に記録する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 12:21:45 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 使われていない抜粋処理と、実際の処理とで規則が異なる状態が残ると、本案件で抜粋の規則を変えた後に、読み手がどちらが正しいかを誤認するおそれがあります。削除の範囲は testfw の中で閉じ、局所テストで確認できます。

**Related**

[Q-010](#q-010-テスト-コード節の-instantiate_test_suite_p-の抜粋)

**Assessment**

`get_test_code_c_cpp.awk`、それを呼び出す `extract_code()`、awk を対象とする `test_results_refinement.py` の 3 件のテスト、`exec_test_c_cpp.sh` のハッシュの対象一覧の項目、`dotnet-test-results-design.md` の記載を、本案件で削除します。テスト コード節の抜粋は `test_subprocedures.py` だけが担うことになり、[Q-010](#q-010-テスト-コード節の-instantiate_test_suite_p-の抜粋) の決定と矛盾しません。作業 WBS の 4.4.3 に対象を追記しました。

### Q-020 リンク先の保護で整形結果が変わる既存の Markdown

**Status**

解決済み

**Question**

BL-001 の修正によって整形結果が変わる既存の Markdown 2 件に、本案件で `text_style_jp.py --in-place` を適用しますか。

**Context**

一時フォルダーに複製した text_style_jp に [Q-002](#q-002-リンク先を保護する記法) と [Q-003](#q-003-リンクのタイトルの扱い) の決定どおりの保護を加え、第一者管理の Markdown 398 件 (外部由来の配布物、テスト結果、`doxybook2` の生成物を除く) の整形結果を現行版と比較しました。396 件は同一で、次の 2 件が変わります。

| ファイル | 現行の整形結果 | 修正後の整形結果 |
| --- | --- | --- |
| `app/zlib/packages/README.md` (サブモジュール `app/zlib`) 3 行目 | `[公式配布ページ](https://zlib.net/)から取得した` | `[公式配布ページ](https://zlib.net/) から取得した` |
| `framework/docsfw/docs/references/requirements-functional-specification-research.md` 1067 行目 | `[第2.8版 本文 PDF](https://...)` | `[第 2.8 版 本文 PDF](https://...)` |

原因は、URL の保護 (`text_style_jp_engine.py` の正規表現 `https?://\S+`) が、リンク先の閉じ括弧 `)` を越えて、空白が現れるまでの後続の文字列も保護していたことです。このため、上の 2 か所では、リンクの直後の日本語や、次のリンクのリンク テキストが整形の対象から外れていました。BL-001 の修正でリンク先が先に保護されるため、URL の保護はリンク先の内側で止まります。変わった結果は、いずれもリポジトリのほかのリンク (`[...](...) の`) や数字の表記 (`第 2.8 版`) と同じ形です。

[Q-011](#q-011-受入時のテストの範囲) の Context では、受入の確認として「第一者管理の Markdown 全体への `--dry-run` (整形の結果が変わらないこと)」を挙げていました。案 A の場合、この条件を「上の 2 件だけが変わり、その 2 件に `--in-place` を適用した後は全体の `--dry-run` で変更がないこと」に読み替えます。`app/zlib` はサブモジュールのため、コミットの対象にサブモジュール `app/zlib` が加わります。  
案 B の場合、2 件は変更せず、次にそのファイルを編集して `text_style_jp.py` を適用した時点で変わります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 本案件で 2 件に `--in-place` を適用する。
- [ ] B: 2 件は変更しない。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-23 12:21:45 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 変わる内容は整形の規則どおりの結果で、修正前は保護の範囲の誤りで整形されていなかった箇所です。2 件を残すと、`--dry-run` による全体の確認で差分が出る状態が続き、別の作業でファイルを編集した人に無関係な差分が混ざります。

**Related**

[Q-002](#q-002-リンク先を保護する記法)、[Q-003](#q-003-リンクのタイトルの扱い)、[Q-011](#q-011-受入時のテストの範囲)

**Assessment**

BL-001 の修正後、`app/zlib/packages/README.md` と `framework/docsfw/docs/references/requirements-functional-specification-research.md` に `text_style_jp.py --in-place` を適用します。[Q-011](#q-011-受入時のテストの範囲) の受入の確認のうち「第一者管理の Markdown 全体への `--dry-run` で整形の結果が変わらないこと」は、「この 2 件だけが変わり、適用後は全体の `--dry-run` で変更がないこと」と読み替えます。コミットの対象にサブモジュール `app/zlib` が加わります。作業 WBS の 4.1 と 4.9 に反映しました。

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `prompt/current/test-assert-comment-followup3-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 4

最終仕様 (design.md) の合意版 (第 2 ゲート): 承認済み (Tetsuo Honda、2026-10-23 の対話セッション)

更新予定の成果物: 作業 WBS の 4.1〜4.5 に記載したファイル

受入条件: [現行の合意内容](#現行の合意内容) の各項目が成果物に反映されていること。[Q-011](#q-011-受入時のテストの範囲) の局所的な確認 ([Q-020](#q-020-リンク先の保護で整形結果が変わる既存の-markdown) による読み替えを含む) とリポジトリ ルートの `make test` 1 回が合格し、`.warn` に本案件起因の警告がないこと

## 反映と検証の記録

対応箇所は次のとおりです (いずれも 2026-10-23)。

- BL-001: `framework/docsfw/bin_internal/text_style_jp_frontends.py` (リンク先の保護)、`bin/text_style_jp.py` (自己テスト)、`docs/text_style_jp.md` (「リンク先の保護」の節、処理順序)。`app/zlib/packages/README.md` と `framework/docsfw/docs/references/requirements-functional-specification-research.md` に `--in-place` を適用しました。
- BL-003: `text_style_jp_frontends.py` (2 規則の追加と `test-comment-expect-call-step` の判定の変更)、`bin/text_style_jp.py` (自己テスト)、`docs/text_style_jp.md` (タグの対応表、診断の表、ルール ID の表)、`app/cplat` の 3 ファイル (`.Times()` への書き換え)。
- BL-004: `framework/testfw/bin_internal/results_markdown.py` (パラメーターの表示、バイト列の検出)、`exec_test_c_cpp.sh` (警告行、ハッシュの対象一覧)、`gtest_summary_groups.py` (実行レコードのパラメーター、接頭辞の算出、`extract_code()` の削除)、`test_subprocedures.py` (接頭辞が空の登録)、`get_test_code_c_cpp.awk` の削除、`bin_test/test_results_refinement.py` と `bin_test/test_path_spaces.py` (awk の比較の削除、パラメーターと WARNING のテスト)、`docs/about-test-phase.md`、`app/general/.agents/skills/create-unit-test/SKILL.md`、`app/general/docs/dotnet-test-results-design.md`、`app/tutorial` の 3 ファイル、`app/porter` の 1 ファイル。

検証結果は次のとおりです。

Table: 本案件の検証結果

| 確認方法 | 結果 |
| --- | --- |
| `text_style_jp.py --test` | すべて合格 |
| `framework/docsfw/bin_test` の `test_test_comment_subprocedures`、`test_testfw_count_comments` | 6 件合格 |
| 第一者管理の Markdown 398 件の整形結果 | 変化は 3 件 (`framework/docsfw/styles/widdershins/` の外部由来の README)。修正前の text_style_jp でも同じ結果になり、本案件に起因しません |
| 全テスト コード 680 ファイルの 3 規則の診断 | 移行前は移行対象の 3 か所だけ。移行後は 0 件 |
| `app/cplat` の 3 テスト ディレクトリの `make test` | 58、20、45 件合格。初回は、`app/cplat` のライブラリが最新のコミットより古く、無関係のテストも失敗したため、`app/cplat` を再ビルドしてから再実行しました |
| `framework/testfw/bin_test` | 109 件合格 |
| `app/tutorial` の `param-test` の `make test` | 10 件合格。3 か所にパラメーターが名前付きで載り、テスト コード節に接頭辞が空の登録が載ることを確認しました。ラムダの引数名 `info` が `-Wshadow` の警告になったため `param_info` に改め、規範の例にも反映しました |
| リポジトリ ルートの `make test` | 終了コード 0。124 ディレクトリで 2162 件合格、FAILED と WARNING なし。`app/porter` の `tcp_allocation_failure_test` のパラメーターが名前付きで載ることを確認しました。`app/cjson`、`app/zlib`、`app/lua`、`app/sqlite` は署名が一致したため省略されました (残課題 BL-003) |
| `.warn` ファイル | 内容があるファイルなし |
| 変更した Markdown への `text_style_jp.py --dry-run` | 指摘なし |
| CI (`CI (Build, Test, and Docs)`、run 39903975923、`b66dd26e`) | 成功。Oracle Linux 8、9、10 と Windows のビルドとテスト、警告の集約、Pages の発行のすべてが成功 |

最終仕様からの具体化は次のとおりです。

- パラメーターの値は、従来の備考と同じくインライン コードで表示します。最終仕様の例を合わせて修正しました。
- バイト列の判定と警告文は `results_markdown.py` に置きました。実行結果はソースの文字コードで記録するため、testfw の文字コードの別名 (`shiftjis` など) を扱える Python で警告文を変換します。
