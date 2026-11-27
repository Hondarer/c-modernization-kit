<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
下流文書: `261016-test-assert-comment-followup-design.md` (最終仕様)、`261016-test-assert-comment-followup-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# テストの確認コメントの残課題への対応の検討

ファイル名: `prompt/test-assert-comment-followup-discussion.md`

## 検討の識別

検討 ID: test-assert-comment-followup

版: 3

状態: 合意済み (版 3)

## 作業 WBS と進捗管理

この検討用 Markdown が管理する作業 WBS です。Q&A のターンごとに必要に応じて見直します。フェーズ 3 の項目は、回答によって対象外になったものを削除し、具体的な対象ファイルを追記します。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 前提条件と要求事項の調査 (`prompt/test-assert-comment-backlog.md` の BL-001〜BL-011、現行のテスト コード、`framework/testfw/docs/about-test-phase.md`、`framework/docsfw/docs/text_style_jp.md`)
    - ✅ 1.2. 課題・論点の抽出と明確化のための確認項目 ([Q-001](#q-001-今回の対象とする残課題)〜[Q-019](#q-019-dodefault-の扱い)、版 2 で [Q-020](#q-020-空ポインターの規範の置き場所)〜[Q-024](#q-024-c-の既存コードの移行範囲) を追加) の作成
    - ✅ 1.3. Q&A ターンごとのユーザー回答の受付と記録 (JST 日時、Git ユーザー名。Q-001〜Q-024 を記録)
    - ✅ 1.4. 回答に基づく前提・制約の検証と WBS の見直し (版 2 で WBS 4.1b、4.4b、4.9 を見直し)
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定 (懸念がないことをもってフェーズ 2 へ着手。版 3)
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `prompt/test-assert-comment-followup-design.md` の作成
    - ✅ 3.2. 最終仕様における受入条件と整合性の確認
    - ✅ 3.3. 対話セッションにおける最終仕様 (design.md) の承認の受付 (第 2 ゲート: 作業着手の合意。2026-10-16 Tetsuo Honda「推進」)
- ✅ 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. 規則の更新 (`framework/testfw/docs/about-test-phase.md`、`process-control-api.md`、`trace-level-process-control.md`、`mock-injection-technique.md`、`app/general/docs/testing-tutorial.md` の例。`text_style_jp.md` は 4.2 で更新)
    - ✅ 4.1b. 空ポインターの規範の追加 (`app/general/docs/coding-guideline.md` の「空ポインターの表記」。C の本番コードでポインターをそのまま条件にしていた 5 か所の修正を含む)
    - ✅ 4.2. 共通処理の更新 ([Q-014](#q-014-act-に残した-assert_no_throw-の直し方) の結果による `framework/testfw` の待機関数、[Q-010](#q-010-text_style_jp-の誤検出への対応)、[Q-011](#q-011-新しい規則の機械点検の常設) の結果による `framework/docsfw/bin/text_style_jp.py`)
    - ✅ 4.3. Assert の中の操作の移動とマルチ フェーズ化 (BL-001。`app/cplat/test/src/libcplatTest/argparser/argparserTest/argparserTest.cc` ほか)
    - ✅ 4.4. 確認コメントの表記の修正 (BL-002 キャスト、BL-003 `.Times(0)` の文型、BL-004 nullptr と NULL)
    - ✅ 4.4b. 既存コードの空ポインターの表記の移行 ([Q-022](#q-022-c-の空判定の書き方)、[Q-024](#q-024-c-の既存コードの移行範囲) の回答による範囲)
    - ✅ 4.5. 状態と手順の説明文の表記の一括移行 (BL-005。全 app と `framework/testfw` のテスト コード)
    - ✅ 4.6. 個別箇所の修正 (BL-007 `app/zlib`、BL-008 `app/calc.net`、BL-009 `app/porter`、BL-010 `app/cplat` string_catalog、BL-011 `DoDefault()`。4.2 の `text_style_jp.py` と 4.3〜4.6 は codex CLI で着手し、codex の利用上限により agy が引き継いだ。`socketTest.cc` の `[Pre-Assert手順]` 43 件は 13:56 以降に codex で実施)
    - ✅ 4.7. 局所テストおよび機械的検証 (変更した各 app と framework の `make test` を 1 本ずつ実行、`.warn` の確認、確認件数の照合、`git clang-format`、`text_style_jp.py --mode cpp --dry-run`、変更した Markdown への `text_style_jp.py`)
    - ✅ 4.8. 受入条件への適合確認 (全完確認)
    - ✅ 4.9. 残課題記録 (`prompt/test-assert-comment-followup-backlog.md` の作成。[Q-009](#q-009-状態と手順の説明文の移行範囲) の補足により、状態と手順の説明文の書き方 (主語や文型) の規則化を必ず記録する)
    - ✅ 4.10. コミットとプッシュ、CI の追跡 ([Q-018](#q-018-コミットとプッシュの扱い) により実施。サブモジュール、リポジトリ本体の順。初回の CI で docsfw の `bin_test/test_testfw_count_comments.py` が新しい点検に指摘されて失敗したため、入力にフェーズの見出しを加えて再実行し、すべて成功)
- ✅ 5. 成果物の承認の受付 (第 3 ゲート)
    - ✅ 5.1. 成果物・検証結果・残課題の提示
    - ✅ 5.2. 対話セッションにおける成果物の承認の受付 (「承認」または「完了」の受領によるスキル完了。2026-10-17 にユーザーが「完了」と宣言)

## 対象と参照資料

対象成果物: 未確定 ([Q-001](#q-001-今回の対象とする残課題) の回答で確定)

今回の対象範囲: `prompt/test-assert-comment-backlog.md` の BL-001〜BL-011 のうち、[Q-001](#q-001-今回の対象とする残課題) で選んだもの

書き込み可能な検討用 Markdown: この文書 (`prompt/test-assert-comment-followup-discussion.md`)

参照資料と版:

- `prompt/test-assert-comment-backlog.md` (2026-10-12 作成。以後の変更なし)
- `prompt/test-assert-comment-design.md` (前案件の最終仕様 版 2)
- `framework/testfw/docs/about-test-phase.md` (2026-10-16 時点の `HEAD`)
- `framework/docsfw/docs/text_style_jp.md`、`framework/docsfw/bin/text_style_jp.py` (`framework/docsfw` 593a6e9)
- テスト コード: リポジトリ本体と全サブモジュールの `test/src/` 配下 (2026-10-16 時点の `HEAD`)

## 調査結果

2026-10-16 時点のコードを調べた結果です。件数は機械的な抽出によるもので、誤差を含みます。

Table: 残課題ごとの現状

| 課題 | 現状 | 件数 |
| --- | --- | --- |
| BL-001 | Assert の中に判定以外の文がある。うち `[手順]` を付けた操作は `argparserTest.cc` の 50 件で、24 テストが「空の Act の直後に Assert を置き、Assert の中にブロックを並べる」形 | 判定以外の文 352 件 (宣言 63、代入 80、呼び出し 200、制御 9) |
| BL-002 | 確認コメントにキャスト式が出る。`(int)` 68、`(size_t)` 58、`static_cast<int>` 19 など。`[Pre-Assert確認_*]` に限らず `[確認_*]` にもある (例: `(uint8_t *)NULL 以外`) | 246 行 |
| BL-003 | 引数条件付きの `.Times(0)` が「f() が X を指定して呼び出されないこと。」の文型になっている | 10 件 (引数条件のない「呼び出されないこと」を含めると 138 件) |
| BL-004 | 「nullptr 以外」と「NULL 以外」が混在する。コード側は `(EXPECT\|ASSERT)_(NE\|EQ)(nullptr` が 352 件、`NULL` が 186 件。「NULL 以外」は `app/cplat` だけ | 説明文 nullptr 472、NULL 304 |
| BL-005 | `[状態]`、`[手順]`、`[Pre-Assert手順]` の説明文に、`()` のない関数名やバッククォートが残る | 説明文 8,540 行のうち `()` のない関数名 約 848 行、バッククォート 139 行 (`app/cplat` 16 ファイル、`app/porter` 1、`app/service-sample` 1) |
| BL-006 | 作業用の点検スクリプトは削除済み。`text_style_jp.py --mode cpp` が、確認コメント中の文字列 (`"value={ 123 }"`、`}`) とキャスト式を `space-inside-brackets`、`supplemental-bracket` で指摘する | 確認コメントの行で 20 件 (`space-inside-brackets` 14、`supplemental-bracket` 6) |
| BL-007 | `app/zlib/test/src/cmd/zlibTest/zlibTest.cc` の `gzopen()` の判定 2 件が `[確認_正常系]` | 2 件 |
| BL-008 | `app/calc.net` の 2 テストで判定のない確認タグを削除した。例外を投げる経路は `CalculateOrThrow_DivideByZero_ShouldThrowException` が別に確認している | 2 件 |
| BL-009 | `waitForOutput()` は `framework/testfw/include/processController.h` の共通関数で、タイムアウト時に例外を送出し、受信した出力を返す。利用者は `app/porter` だけ。`ASSERT_NO_THROW` は 115 件で、うち Act の中の `[確認_*]` が 72 件、Arrange の `[状態確認]` が残り | 72 件 (Act) |
| BL-010 | `stringCatalogFilterSourceTest.cc` 363 行目と 1214 行目付近で、Act の途中に `EXPECT_CALL(...).Times(0)` を置く | 2 件 |
| BL-011 | `about-test-phase.md` 101 行目と `text_style_jp.md` 374 行目は、`WillOnce` / `WillRepeatedly` を伴う `EXPECT_CALL` に `[Pre-Assert手順]` を求める。`test-comment-expect-call-step` の指摘は、`WillOnce(Return(...))` 221、`WillOnce(DoAll(...))` 11、`WillRepeatedly(Return(...))` 8、`DoDefault()` を含むもの 7 など。`app/cplat` 250 件 (うち `socketTest.cc` 109)、`app/porter` 20、`app/calc` 4、`app/struct-meta` 2 | 276 件 |

> [!NOTE]
> BL-011 は、バックログでは「`DoDefault()` など既定の動作の扱いを最終仕様が決めていない」と記録しました。全テストを点検し直すと、指摘の大半は `WillOnce(Return(...))` で、`DoDefault()` は 7 件だけでした。現行の規範はすでに「戻り値を返す動作は `[Pre-Assert手順]` に書く」と定めており、BL-011 の大半は規則の欠落ではなく、規則に合っていない既存のコードです ([Q-017](#q-017-pre-assert-手順の欠落への対応))。`DoDefault()` は、`about-test-phase.md` 35 行目が「既定動作を `[Pre-Assert手順]` に書くと、注入値と矛盾して解釈されるおそれがある」として `ON_CALL` での記載を禁じており、書き方が決まっていません ([Q-019](#q-019-dodefault-の扱い))。

## 現行の合意内容

ユーザーが明示した内容と、解決済み Q の決定のみを根拠付きで記載します。

- 起点: `prompt/test-assert-comment-backlog.md` を基にした作業計画を、agree-before-implement スキルで新たに開始する (2026-10-16 Tetsuo Honda の指示)。
- 対象: BL-001〜BL-011 のすべて ([Q-001](#q-001-今回の対象とする残課題))。
- Assert には判定と、被テスト対象の状態を変えない値の取り出しや集計を置く。被テスト対象への操作は Act へ移す ([Q-002](#q-002-assert-に置いてよい文の範囲))。`argparserTest.cc` の 24 テストはマルチ フェーズにする ([Q-003](#q-003-argparser-の空の-act-の直し方))。
- 確認コメントでは、型をそろえるだけのキャストを省く ([Q-004](#q-004-確認コメントのキャスト式))。引数条件付きの `.Times(0)` は「X を指定した f() の呼び出しがないこと。」とする ([Q-005](#q-005-引数条件付きで呼び出されないことの文型))。
- C++ のテスト コードの空判定を `nullptr` にそろえ、説明文も nullptr とする ([Q-006](#q-006-nullptr-と-null-の表記))。NULL と nullptr の規範の充実も今回の範囲とする ([Q-006](#q-006-nullptr-と-null-の表記) の補足)。
- `coding-guideline.md` に「空ポインターの表記」の章を設ける ([Q-020](#q-020-空ポインターの規範の置き場所))。C は `NULL` だけを使い、`== NULL` / `!= NULL` で比べる ([Q-021](#q-021-c-の空ポインター定数)、[Q-022](#q-022-c-の空判定の書き方))。C++ は `nullptr` だけを使い、`== nullptr` / `!= nullptr` で比べる。C からも読み込まれるヘッダーは C の規則に従う ([Q-023](#q-023-c-の空ポインター定数))。全 C++ ソースのコード上の `NULL` を `nullptr` に置き換え、C と C++ でポインターをそのまま条件にしている箇所も直す ([Q-024](#q-024-c-の既存コードの移行範囲)、[Q-022](#q-022-c-の空判定の書き方))。
- 状態と手順の説明文は、関数名に `()` を付け、バッククォートを使わない。全 app と `framework/testfw` で直し、文の書き方は変えない ([Q-007](#q-007-状態と手順の説明文の関数名)、[Q-008](#q-008-状態と手順の説明文のバッククォート)、[Q-009](#q-009-状態と手順の説明文の移行範囲))。文の書き方の規則化はバックログにする ([Q-009](#q-009-状態と手順の説明文の移行範囲) の補足)。
- `text_style_jp.py` の C/C++ のコメントで、引用符で囲んだ部分を整形の対象から外す ([Q-010](#q-010-text_style_jp-の誤検出への対応))。機械的に判定できる規則を `test-comment-*` の診断として常設する ([Q-011](#q-011-新しい規則の機械点検の常設))。
- `gzopen()` の判定は `[確認_正常系]` のまま ([Q-012](#q-012-gzopen-の判定のタグ))。calc.net の確認タグの削除を承認 ([Q-013](#q-013-calcnet-の確認タグの削除))。
- `framework/testfw` に成否を戻り値で返す待機関数を加え、porter の Act の 72 件と Arrange の 43 件を書き換える ([Q-014](#q-014-act-に残した-assert_no_throw-の直し方)、[Q-015](#q-015-arrange-の-assert_no_throw-もそろえるか))。
- Act の途中の `EXPECT_CALL` はマルチ フェーズにする ([Q-016](#q-016-act-の途中の-expect_call-の直し方))。
- 欠けている `[Pre-Assert手順]` 276 件を加える ([Q-017](#q-017-pre-assert-手順の欠落への対応))。`DoDefault()` だけの `WillOnce` / `WillRepeatedly` は `.Times(n)` で書く ([Q-019](#q-019-dodefault-の扱い))。
- 完了後、コミット、プッシュ、CI の追跡まで行う ([Q-018](#q-018-コミットとプッシュの扱い))。
- 広範囲の書き換えは慎重に調査して進める ([Q-005](#q-005-引数条件付きで呼び出されないことの文型) の補足)。

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | 残課題の解消。対象は全 11 件 ([Q-001](#q-001-今回の対象とする残課題)) |
| 範囲・境界 | 確認済み | [Q-001](#q-001-今回の対象とする残課題)、[Q-003](#q-003-argparser-の空の-act-の直し方)、[Q-009](#q-009-状態と手順の説明文の移行範囲)、[Q-015](#q-015-arrange-の-assert_no_throw-もそろえるか)、[Q-024](#q-024-c-の既存コードの移行範囲) |
| 入力・根拠 | 確認済み | [調査結果](#調査結果) |
| 整合性・依存関係 | 確認済み | [Q-002](#q-002-assert-に置いてよい文の範囲)、[Q-014](#q-014-act-に残した-assert_no_throw-の直し方)、[Q-017](#q-017-pre-assert-手順の欠落への対応)、[Q-020](#q-020-空ポインターの規範の置き場所)〜[Q-023](#q-023-c-の空ポインター定数) |
| 実現性・例外 | 確認済み | [Q-010](#q-010-text_style_jp-の誤検出への対応)、[Q-016](#q-016-act-の途中の-expect_call-の直し方)、[Q-019](#q-019-dodefault-の扱い) |
| 受入条件・検証 | 確認済み | [Q-011](#q-011-新しい規則の機械点検の常設)、[Q-018](#q-018-コミットとプッシュの扱い)。広範囲の書き換えは確認件数の照合と差分の抽出点検で確かめる ([Q-005](#q-005-引数条件付きで呼び出されないことの文型) の補足) |

## 質問一覧

| Q-ID | 論点 | 対象 | 状態 | 依存・関連 |
| --- | --- | --- | --- | --- |
| [Q-001](#q-001-今回の対象とする残課題) | 今回の対象とする残課題 | 全体 | 解決済み | なし |
| [Q-002](#q-002-assert-に置いてよい文の範囲) | Assert に置いてよい文の範囲 | BL-001 | 解決済み | [Q-003](#q-003-argparser-の空の-act-の直し方) |
| [Q-003](#q-003-argparser-の空の-act-の直し方) | argparser の空の Act の直し方 | BL-001 | 解決済み | [Q-002](#q-002-assert-に置いてよい文の範囲) |
| [Q-004](#q-004-確認コメントのキャスト式) | 確認コメントのキャスト式 | BL-002 | 解決済み | [Q-010](#q-010-text_style_jp-の誤検出への対応) |
| [Q-005](#q-005-引数条件付きで呼び出されないことの文型) | 引数条件付きで呼び出されないことの文型 | BL-003 | 解決済み | なし |
| [Q-006](#q-006-nullptr-と-null-の表記) | nullptr と NULL の表記 | BL-004 | 解決済み | [Q-004](#q-004-確認コメントのキャスト式) |
| [Q-007](#q-007-状態と手順の説明文の関数名) | 状態と手順の説明文の関数名 | BL-005 | 解決済み | [Q-009](#q-009-状態と手順の説明文の移行範囲) |
| [Q-008](#q-008-状態と手順の説明文のバッククォート) | 状態と手順の説明文のバッククォート | BL-005 | 解決済み | [Q-009](#q-009-状態と手順の説明文の移行範囲) |
| [Q-009](#q-009-状態と手順の説明文の移行範囲) | 状態と手順の説明文の移行範囲 | BL-005 | 解決済み | [Q-007](#q-007-状態と手順の説明文の関数名)、[Q-008](#q-008-状態と手順の説明文のバッククォート) |
| [Q-010](#q-010-text_style_jp-の誤検出への対応) | text_style_jp の誤検出への対応 | BL-006 | 解決済み | [Q-004](#q-004-確認コメントのキャスト式) |
| [Q-011](#q-011-新しい規則の機械点検の常設) | 新しい規則の機械点検の常設 | BL-006 | 解決済み | [Q-002](#q-002-assert-に置いてよい文の範囲)、[Q-008](#q-008-状態と手順の説明文のバッククォート) |
| [Q-012](#q-012-gzopen-の判定のタグ) | gzopen の判定のタグ | BL-007 | 解決済み | なし |
| [Q-013](#q-013-calcnet-の確認タグの削除) | calc.net の確認タグの削除 | BL-008 | 解決済み | なし |
| [Q-014](#q-014-act-に残した-assert_no_throw-の直し方) | Act に残した ASSERT_NO_THROW の直し方 | BL-009 | 解決済み | [Q-015](#q-015-arrange-の-assert_no_throw-もそろえるか) |
| [Q-015](#q-015-arrange-の-assert_no_throw-もそろえるか) | Arrange の ASSERT_NO_THROW もそろえるか | BL-009 | 解決済み | [Q-014](#q-014-act-に残した-assert_no_throw-の直し方) |
| [Q-016](#q-016-act-の途中の-expect_call-の直し方) | Act の途中の EXPECT_CALL の直し方 | BL-010 | 解決済み | [Q-003](#q-003-argparser-の空の-act-の直し方) |
| [Q-017](#q-017-pre-assert-手順の欠落への対応) | Pre-Assert 手順の欠落への対応 | BL-011 | 解決済み | [Q-019](#q-019-dodefault-の扱い) |
| [Q-018](#q-018-コミットとプッシュの扱い) | コミットとプッシュの扱い | 全体 | 解決済み | なし |
| [Q-019](#q-019-dodefault-の扱い) | DoDefault の扱い | BL-011 | 解決済み | [Q-017](#q-017-pre-assert-手順の欠落への対応) |
| [Q-020](#q-020-空ポインターの規範の置き場所) | 空ポインターの規範の置き場所 | BL-004 | 解決済み | [Q-006](#q-006-nullptr-と-null-の表記) |
| [Q-021](#q-021-c-の空ポインター定数) | C の空ポインター定数 | BL-004 | 解決済み | [Q-020](#q-020-空ポインターの規範の置き場所) |
| [Q-022](#q-022-c-の空判定の書き方) | C の空判定の書き方 | BL-004 | 解決済み | [Q-021](#q-021-c-の空ポインター定数) |
| [Q-023](#q-023-c-の空ポインター定数) | C++ の空ポインター定数 | BL-004 | 解決済み | [Q-020](#q-020-空ポインターの規範の置き場所) |
| [Q-024](#q-024-c-の既存コードの移行範囲) | C++ の既存コードの移行範囲 | BL-004 | 解決済み | [Q-023](#q-023-c-の空ポインター定数) |

## 明確化のための確認項目

### Q-001 今回の対象とする残課題

**Status**

解決済み

**Question**

`prompt/test-assert-comment-backlog.md` の残課題のうち、今回の案件で対応するものはどれですか。

**Context**

バックログの 11 件は、規則の追加を伴うもの (BL-001〜BL-005、BL-011)、点検ツールに関するもの (BL-006)、ユーザーの判断待ちのもの (BL-007、BL-008)、テストの構造の変更を伴うもの (BL-009、BL-010) に分かれます。規模は [調査結果](#調査結果) のとおりで、BL-005 (約 1,000 行) と BL-002 (246 行)、BL-004 (コード 186 件) が大きく、ほかは数件から数十件です。

対象外にした課題は、今回のバックログ (`prompt/test-assert-comment-followup-backlog.md`) に引き継ぎます。各課題の直し方は Q-002 以降で個別に確認します。対象外にした課題の Q は回答不要です。

**Answer**

選択方式: 複数選択

- [x] A: (推奨) BL-001 Assert フェーズの中にある操作
- [x] B: (推奨) BL-002 確認コメントのキャスト式
- [x] C: (推奨) BL-003 「呼び出されないこと」の係り受け
- [x] D: (推奨) BL-004 nullptr と NULL の表記の揺れ
- [x] E: (推奨) BL-005 状態と手順の説明文の表記
- [x] F: (推奨) BL-006 点検での誤検出
- [x] G: (推奨) BL-007 gzopen の判定のタグ
- [x] H: (推奨) BL-008 判定のない確認タグの削除
- [x] I: (推奨) BL-009 Act に残した ASSERT_NO_THROW
- [x] J: (推奨) BL-010 Act の途中で設定する EXPECT_CALL
- [x] K: (推奨) BL-011 DoDefault() などの Pre-Assert 手順の欠落
- [ ] L: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A〜K (すべて)  
理由: いずれも前案件で定めた確認コメントの規則の続きで、同じ照合の仕組み (確認件数の照合、フェーズとタグの点検) で検証できます。分けて実施すると、同じファイルを何度も書き換えることになります。BL-007 と BL-008 は判断だけで、作業はほぼありません。

**Assessment**

A〜K (全 11 件) が選ばれ、推奨と一致します。全課題を今回の対象とします。

### Q-002 Assert に置いてよい文の範囲

**Status**

解決済み

**Question**

Assert フェーズに、判定のマクロ以外にどの文を置いてよいとしますか。

**Context**

現行の `about-test-phase.md` は、Act の中の判定の扱いを定めていますが、Assert の中の操作の扱いを定めていません。Assert の中にある判定以外の文は 352 件です。多くは判定のための値の取り出しや集計 (ログから該当の行を探すループ、フラグ変数、値のコピー) です。一方、`allocTest.cc` の `memset()` や、`argparserTest.cc` の `[手順]` 付きの呼び出し (50 件) は、被テスト対象への操作です。

```cpp
// allocTest.cc (現状): 書き込みという操作が Assert にある
// Assert
ASSERT_NE((uint8_t *)NULL, buf); // [確認_正常系] - ...
memset(buf, 0xAB, 16U);
SUCCEED(); // [確認_正常系] - SUCCEED() に到達すること。(確保した 16 バイト全体へ書き込めたことの確認)
```

```cpp
// etwSessionIntegrationTest.cc (現状): 判定のための検索が Assert にある
// Assert
bool found = false;
for (...) { if (...) { found = true; break; } }
EXPECT_TRUE(found); // [確認_正常系] - ...
```

案 A の場合、`allocTest.cc` はマルチ フェーズ (Act_1 で確保、Assert_1 で判定、Act_2 で書き込み、Assert_2 で判定) にし、`etwSessionIntegrationTest.cc` は変えません。案 B の場合、検索のループも Act へ移し、Assert には `EXPECT_TRUE(found);` だけを残します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 判定と、判定のための値の取り出しや集計 (被テスト対象の状態を変えない宣言、代入、ループ、検索) を置いてよい。被テスト対象への操作 (`[手順]` を付ける呼び出し、書き込み、状態の変更) は置かず、Act へ移す。
- [ ] B: 判定のマクロだけを置く。値の取り出しや集計も Act へ移す。
- [ ] C: 規則は加えず、`allocTest.cc` と `argparserTest.cc` だけを直す。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 判定のための集計は、判定の一部として Assert にあるほうが読みやすく、Act に移すと手順節に検索の手順が並びます。被テスト対象への操作だけを禁じれば、手順節と確認節の役割が保てます。B は 352 件の大半を動かすことになり、判定と集計が離れます。

**Related**

[Q-003](#q-003-argparser-の空の-act-の直し方)、[Q-011](#q-011-新しい規則の機械点検の常設)

**Assessment**

推奨案 A が選ばれました。Assert には判定と、被テスト対象の状態を変えない値の取り出しや集計を置き、被テスト対象への操作は Act へ移します。`allocTest.cc` の `memset()` と `argparserTest.cc` の `[手順]` 50 件が移動の対象です。

### Q-003 argparser の空の Act の直し方

**Status**

解決済み

**Question**

`argparserTest.cc` の 24 テストにある「空の Act の直後に Assert を置き、Assert の中にブロックを並べる」形を、どう直しますか。

**Context**

[Q-002](#q-002-assert-に置いてよい文の範囲) で操作を Assert に置かないと決めた場合に必要になる判断です。現状は、1 つのテストの中で引数を差し替えて解析し、判定することを、ブロックごとに繰り返しています。

```cpp
// 現状
// Act
// Assert
{
    ARGV(cstr("prog"), cstr("-v"), cstr("--verbose"), cstr("-v"));
    test_argparser_apply_args(parser, argc, argv); // [手順] - 解析対象の引数を差し替える。
    int actual_ret_argparser_parse =
        cplat_argparser_handle_parse(parser); // [手順] - フラグが 3 回出現する引数を解析する。
    EXPECT_EQ(CPLAT_OK, actual_ret_argparser_parse); // [確認_正常系] - ...
    EXPECT_EQ(3, verbose); // [確認_正常系] - verbose が 3 であること。...
}
{
    ARGV(cstr("prog"));
    ...
}
```

```cpp
// 案 A: マルチ フェーズ
// Act_1
ARGV(...);
test_argparser_apply_args(parser, argc, argv); // [手順] - ...
int actual_ret_parse_1 = cplat_argparser_handle_parse(parser); // [手順] - ...

// Assert_1
EXPECT_EQ(CPLAT_OK, actual_ret_parse_1); // [確認_正常系] - ...
EXPECT_EQ(3, verbose); // [確認_正常系] - ...

// Act_2
...
```

```cpp
// 案 B: ブロックごとに別のテストへ分ける
TEST_F(argparserTest, flag_counts_three_occurrences) { ... }
TEST_F(argparserTest, flag_counts_zero_occurrences) { ... }
```

案 A は確認件数とテスト数が変わりません。案 B はテスト数が増え、各テストの確認件数が減ります (合計は変わりません)。`ARGV` マクロはブロックの中で変数を宣言するため、案 A では同じテストの中で名前が衝突しないよう、ブロックを残すか変数名を分ける必要があります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) マルチ フェーズ (Act_1/Assert_1、Act_2/Assert_2 …) にする。
- [ ] B: ブロックごとに別のテストへ分ける。
- [ ] C: 現状の形を規則上の例外として明記し、変えない。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: `about-test-phase.md` のマルチ フェーズの形にそのまま合い、テスト数と確認件数が変わらないため、照合で書き換えの誤りを確かめられます。テスト名の追加も不要です。

**Related**

[Q-002](#q-002-assert-に置いてよい文の範囲)、[Q-016](#q-016-act-の途中の-expect_call-の直し方)

**Assessment**

推奨案 A が選ばれました。`argparserTest.cc` の 24 テストをマルチ フェーズにします。テスト数と確認件数は変わりません。

### Q-004 確認コメントのキャスト式

**Status**

解決済み

**Question**

確認コメントの期待値や引数条件にキャスト式が含まれる場合、どう書きますか。

**Context**

前案件の規則は「判定の引数をそのまま書く」ため、キャストがそのまま説明文と `results.md` に出ます。該当は 246 行で、`[Pre-Assert確認_*]` と `[確認_*]` の両方にあります。

```text
現状:
- fgets() が buf、static_cast<int>(sizeof(buf))、kStream を指定して 2 回呼び出されること。
- cplat_malloc() の戻り値が (uint8_t *)NULL 以外であること。
- buf[0] が (uint8_t)0xAB であること。

案 A:
- fgets() が buf、sizeof(buf)、kStream を指定して 2 回呼び出されること。
- cplat_malloc() の戻り値が NULL 以外であること。
- buf[0] が 0xAB であること。

案 C:
- fgets() が buf、buf の大きさ、kStream を指定して 2 回呼び出されること。
```

キャストに意味がある場合 (符号の扱いを変える `(uint32_t)-1` や、ポインターの同一性を比べる `(void *)` など) は、案 A でも残します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 型をそろえるためだけのキャストは省いて書く。値の意味を変えるキャストは残す。
- [ ] B: キャストを含めてコードどおりに書く (現状を維持し、規則に明記する)。
- [ ] C: キャストを含む式は、意味を日本語で書く。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 読み手が知りたいのは値であり、型合わせのキャストは判定の意味を変えません。C は書き手ごとに表現が揺れます。A の「意味を変えるキャスト」の判断は書き換えの際に個別に確認し、迷ったものは一覧で提示します。

**Related**

[Q-006](#q-006-nullptr-と-null-の表記)、[Q-010](#q-010-text_style_jp-の誤検出への対応)

**Assessment**

推奨案 A が選ばれました。型をそろえるだけのキャストは省き、値の意味を変えるキャストは残します。判断に迷った箇所は作業後に一覧で提示します。

### Q-005 引数条件付きで呼び出されないことの文型

**Status**

解決済み

**Question**

引数条件付きの `.Times(0)` の確認コメントを、どの文型で書きますか。

**Context**

現行の規則の文型では、否定がどこまでかかるかが読みにくくなります。該当は 10 件です。引数条件のない「f() が呼び出されないこと。」(128 件) は対象外です。

```text
現状 (案 B):
- cplat_fopen() が "/tmp/output.bin" と等しい文字列、"wb" と等しい文字列を指定して呼び出されないこと。

案 A:
- "/tmp/output.bin" と等しい文字列、"wb" と等しい文字列を指定した cplat_fopen() の呼び出しがないこと。

案 C:
- cplat_fopen() の呼び出しのうち、"/tmp/output.bin" と等しい文字列、"wb" と等しい文字列を指定したものが 0 回であること。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 「X を指定した f() の呼び出しがないこと。」
- [ ] B: 「f() が X を指定して呼び出されないこと。」(現状を維持する)
- [ ] C: 「f() の呼び出しのうち、X を指定したものが 0 回であること。」
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: 比較的広範囲に修正が及ぶので、慎重に調査し進めること。(コメント)

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 否定が「呼び出し」にだけかかり、引数条件は呼び出しを限定する修飾として読めます。条件を主語の前に置く前案件の規則 (呼び出し条件は主語の前に置く) とも合います。

**Assessment**

推奨案 A が選ばれました。補足の「比較的広範囲に修正が及ぶので、慎重に調査し進めること」は、作業全体への注意として扱います。この Q の文型の変更自体は 10 件ですが、Q-004、Q-006、Q-007、Q-008、Q-017 は数百件から数千件に及ぶため、各作業で書き換え前後の確認件数の照合と、変更の種類ごとの差分の抽出点検を行います (作業 WBS 4.7)。

### Q-006 nullptr と NULL の表記

**Status**

解決済み

**Question**

ポインターの空判定の表記を、どうそろえますか。

**Context**

確認コメントはコードの判定をそのまま書くため、コードが `nullptr` と比べるものは「nullptr 以外」、`NULL` や `(uint8_t *)NULL` と比べるものは「NULL 以外」になります。コード側は `nullptr` が 352 件、`NULL` が 186 件です。「NULL 以外」は `app/cplat` にだけあります。テスト コードは C++ で、コーディング規範に `nullptr` と `NULL` の使い分けの定めはありません。

```cpp
// 現状
ASSERT_NE((uint8_t *)NULL, buf); // [確認_正常系] - cplat_malloc() の戻り値が (uint8_t *)NULL 以外であること。

// 案 A: コードを nullptr にそろえる
ASSERT_NE(nullptr, buf); // [確認_正常系] - cplat_malloc() の戻り値が nullptr 以外であること。

// 案 B: コードは変えず、説明文だけ NULL にそろえる
ASSERT_NE(nullptr, buf); // [確認_正常系] - cplat_malloc() の戻り値が NULL 以外であること。
```

案 A はテスト コードの判定式を 186 件書き換えますが、判定の結果は変わりません。被テスト対象の C コードは対象外です。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) C++ のテスト コードの空判定を `nullptr` にそろえ、説明文も「nullptr 以外」「nullptr であること」にする。
- [ ] B: コードは変えず、説明文だけ「NULL 以外」にそろえる。
- [ ] C: コードの表記どおりに書く方針を規則に明記し、変えない。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: NULL / nullptr の規範を充実させること。C の場合、C++の場合で推奨が変わる場合もあると思うので、規範充実も本作業のスコープにする。

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: C++ では `nullptr` がポインターの空値を表す型付きの値で、`(uint8_t *)NULL` のようなキャストが不要になります ([Q-004](#q-004-確認コメントのキャスト式) の対象も減ります)。B は「コードの判定をそのまま書く」前案件の規則と食い違います。

**Related**

[Q-004](#q-004-確認コメントのキャスト式)、[Q-020](#q-020-空ポインターの規範の置き場所)、[Q-024](#q-024-c-の既存コードの移行範囲)

**Assessment**

推奨案 A が選ばれ、テスト コードの空判定を `nullptr` にそろえることは確定しました。補足で「NULL / nullptr の規範を充実させること。C の場合と C++ の場合で推奨が変わる場合もあるので、規範の充実も本作業のスコープにする」と指示がありました。これは新しい範囲 (コーディング規範の追加) で、置き場所、C と C++ それぞれの規則、既存コードの移行範囲の判断が必要です。[Q-020](#q-020-空ポインターの規範の置き場所)〜[Q-024](#q-024-c-の既存コードの移行範囲) で確認します。

**History**

- 2026-10-16: 補足の規範の充実を [Q-020](#q-020-空ポインターの規範の置き場所)〜[Q-024](#q-024-c-の既存コードの移行範囲) で確認し、すべて解決したため解決済みとしました。

### Q-007 状態と手順の説明文の関数名

**Status**

解決済み

**Question**

`[状態]`、`[手順]`、`[Pre-Assert手順]` の説明文で、関数名に `()` を付けますか。

**Context**

確認タグの説明文は、前案件で関数名に `()` を付けました。状態と手順の説明文は対象外だったため、`()` のない関数名が約 848 行あります。`results.md` では、状態節と手順節が `()` なし、確認節が `()` 付きと混在します。`about-test-phase.md` の例 (「TestedMethod を呼び出す。」) も `()` がありません。

```text
現状:
### 手順
- 16 バイトを指定して cplat_malloc を呼び出す。
### 確認
- cplat_malloc() の戻り値が NULL 以外であること。

案 A:
### 手順
- 16 バイトを指定して cplat_malloc() を呼び出す。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 付ける (確認タグと同じく、引数なしの `()` を付ける)。
- [ ] B: 付けない (現状を維持し、規則に明記する)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 同じ `results.md` の中で関数の表記がそろい、関数と変数の区別が付きます。

**Related**

[Q-009](#q-009-状態と手順の説明文の移行範囲)

**Assessment**

推奨案 A が選ばれました。状態と手順の説明文の関数名に `()` を付けます。

### Q-008 状態と手順の説明文のバッククォート

**Status**

解決済み

**Question**

`[状態]`、`[手順]`、`[Pre-Assert手順]` の説明文で、識別子をバッククォートで囲むことをやめますか。

**Context**

確認タグの説明文は、前案件でバッククォートを使わないことにしました。状態と手順の説明文には、バッククォートで囲んだ識別子が 139 行あります (例: 「`cplat_strncpy` の既定動作を設定する。」)。`results.md` では、状態節と手順節でバッククォートがコードの書式として表示されます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) やめる (確認タグと同じく、バッククォートを使わない)。
- [ ] B: やめない (状態と手順の説明文ではバッククォートを許す)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 確認節と表記がそろいます。8,540 行のうち 139 行だけが使っており、使わない書き方がすでに大半です。

**Related**

[Q-009](#q-009-状態と手順の説明文の移行範囲)、[Q-011](#q-011-新しい規則の機械点検の常設)

**Assessment**

推奨案 A が選ばれました。状態と手順の説明文でバッククォートを使いません。

### Q-009 状態と手順の説明文の移行範囲

**Status**

解決済み

**Question**

[Q-007](#q-007-状態と手順の説明文の関数名) と [Q-008](#q-008-状態と手順の説明文のバッククォート) で決めた表記を、どの範囲の状態と手順の説明文に適用しますか。

**Context**

規則を新設した場合は、既存のコードも同じ変更で全 app を移行するのがこのリポジトリの進め方です。状態と手順の説明文は 8,540 行あり、表記の修正だけでも約 1,000 行が対象です。前案件の確認タグの規則 (主語、文型) は状態と手順には及ぼさず、今回は関数名とバッククォートだけを直す想定です。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 関数名の `()` とバッククォートだけを、全 app と `framework/testfw` のテスト コードで直す。文の書き方は変えない。
- [ ] B: 関数名とバッククォートに加えて、状態と手順の説明文の書き方 (主語や文型) も規則に定め、全体を書き換える。
- [ ] C: 規則だけを定め、既存のコードは変更した箇所から順次直す。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: B はバックログにする。

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 表記は機械的に直せて、確認件数も変わらないため、照合で誤りを確かめられます。B は状態と手順の文型という新しい論点を開くため、必要なら別の案件にします。

**Related**

[Q-007](#q-007-状態と手順の説明文の関数名)、[Q-008](#q-008-状態と手順の説明文のバッククォート)

**Assessment**

推奨案 A が選ばれました。補足の「B はバックログにする」により、状態と手順の説明文の書き方 (主語や文型) を規則に定めることは、今回のバックログ (`prompt/test-assert-comment-followup-backlog.md`) に記録します (作業 WBS 4.9)。

### Q-010 text_style_jp の誤検出への対応

**Status**

解決済み

**Question**

`text_style_jp.py --mode cpp` が確認コメント中の文字列やキャスト式を指摘する誤検出に、どう対応しますか。

**Context**

確認コメントからバッククォートをなくしたため、コメント中の文字列リテラル (`"value={ 123 }"`) や波括弧の文字 (`}`) が保護されず、`space-inside-brackets` (括弧内の空白の削除) で指摘されます。キャスト式 (`static_cast<__int64>(0)`) は `supplemental-bracket` (補足括弧の前に空白を入れる) で指摘されます。`--in-place` で直すと、コメントの内容がコードと食い違います。

`text_style_jp.py` は `framework/docsfw` の共通ツールで、Markdown を含む全リポジトリの整形に使います。キャスト式の指摘は、[Q-004](#q-004-確認コメントのキャスト式) で案 A を選ぶと大半がなくなります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `text_style_jp.py` の C/C++ のコメントの処理で、二重引用符と一重引用符で囲んだ部分を文字列として整形の対象から外す。`framework/docsfw` のテストを追加する。
- [ ] B: ツールは変えず、指摘される確認コメントの書き方を変える (文字列を言い換えるなど)。
- [ ] C: 誤検出として容認し、何もしない。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 確認コメントは文字列リテラルを省略せずに書く規則のため、今後も同じ指摘が出ます。共通ツール側で直せば、全リポジトリのコメントで同じ誤検出がなくなります。B は文字列を正確に書く規則と相反します。なお、Markdown の処理は変えません (C/C++ のコメントに限ります)。

**Related**

[Q-004](#q-004-確認コメントのキャスト式)、[Q-011](#q-011-新しい規則の機械点検の常設)

**Assessment**

推奨案 A が選ばれました。`framework/docsfw` の `text_style_jp.py` を変更するため、`framework/docsfw` のテストも実行します (作業 WBS 4.7)。

### Q-011 新しい規則の機械点検の常設

**Status**

解決済み

**Question**

前案件と今回の規則のうち、機械的に判定できるものを `text_style_jp.py` の点検に常設しますか。

**Context**

前案件では、フェーズとタグの対応や、確認タグの説明文の表記を、作業用の一時スクリプトで点検しました。一時スクリプトはリポジトリに残っていないため、今後のテストの追加や変更では同じ逸脱を検出できません。`text_style_jp.py` には、すでに `test-comment-*` の点検 (タグの不足の検出) があります。

機械的に判定できる候補は次のとおりです。

- Act の中の `EXPECT_*`
- Assert の中の `[手順]`、Pre-Assert 以外の `[Pre-Assert確認_*]`
- タグの説明文のバッククォート ([Q-008](#q-008-状態と手順の説明文のバッククォート) の結果による)
- `[状態確認]` の総合期待

主語の書き方や総合期待の要否など、意味の判断が必要な規則は対象外です。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 上記の候補を `text_style_jp.py` の `test-comment-*` の点検として追加する (診断だけで、自動修正はしない)。
- [ ] B: 常設しない。今回の照合は一時スクリプトで行う。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 既存の `test-comment-*` の点検と同じ仕組みに載せられ、規則の逸脱を変更のたびに検出できます。意味の判断が要るものは誤検出の原因になるため含めません。

**Related**

[Q-002](#q-002-assert-に置いてよい文の範囲)、[Q-008](#q-008-状態と手順の説明文のバッククォート)、[Q-010](#q-010-text_style_jp-の誤検出への対応)、[Q-017](#q-017-pre-assert-手順の欠落への対応)

**Assessment**

推奨案 A が選ばれました。候補 4 種類を `test-comment-*` の診断として追加します。[Q-002](#q-002-assert-に置いてよい文の範囲) の回答により、Assert の中の点検は `[手順]` の有無で判定します。

### Q-012 gzopen の判定のタグ

**Status**

解決済み

**Question**

`app/zlib/test/src/cmd/zlibTest/zlibTest.cc` の `gzip_file_roundtrip` にある `gzopen()` の判定 2 件のタグをどうしますか。

**Context**

テストの説明は「gzip ファイルの圧縮書き込みおよび展開読み込みのラウンド トリップ」です。agy は「`gzopen()` は準備の関数なので `[状態確認]` にすべき」と指摘しました。前案件では、`gzopen()` もラウンド トリップの一部と判断して `[確認_正常系]` のまま残しました。`[状態確認]` にすると、このテストの確認件数が 2 件減ります。

```cpp
gzFile writer = gzopen("zlib_roundtrip_test.gz", "wb"); // [手順] - gzip 出力を開く。
ASSERT_NE(nullptr, writer);
// [確認_正常系] - 書き込み用の gzopen() の戻り値が nullptr 以外であること。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `[確認_正常系]` のままにする。
- [ ] B: `[状態確認]` にする。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: ラウンド トリップは開く、書く、閉じる、開く、読むの一連の操作で、`gzopen()` の成否もその結果です。テストの説明とも合います。

**Assessment**

推奨案 A が選ばれました。変更はありません。

### Q-013 calc.net の確認タグの削除

**Status**

解決済み

**Question**

`app/calc.net` の `Divide_ByZero_ShouldReturnError` と `Divide_ByZero_WithZeroDividend_ShouldReturnError` から、判定のない確認タグ (「ゼロ除算で CalcException が発生すること。」) を削除したことを承認しますか。

**Context**

前案件で削除し、2 テストの確認件数が 1 件ずつ減りました。`Divide()` は例外を投げず、結果の `IsSuccess` と `ErrorCode` で失敗を返す API です。例外を投げる経路は、別のテスト `CalculateOrThrow_DivideByZero_ShouldThrowException` が `Assert.Throws<CalcException>` で確認しています。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 削除を承認する (追加の変更なし)。
- [ ] B: 2 テストに例外の判定を追加し、確認件数を戻す。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 例外の経路は別のテストで確認済みで、`Divide()` のテストに例外の判定を加えると、テストの対象 (`Divide()`) と判定がずれます。

**Assessment**

推奨案 A が選ばれました。変更はありません。

### Q-014 Act に残した ASSERT_NO_THROW の直し方

**Status**

解決済み

**Question**

`app/porter` の Act の中にある `ASSERT_NO_THROW(waitForOutput(...))` (72 件) を、どう直しますか。

**Context**

前案件の規則は「Act の中の `ASSERT_*` は呼び出しと判定に分ける」ですが、`ASSERT_NO_THROW` は呼び出しを包むマクロのため分けられません。`waitForOutput()` は `framework/testfw/include/processController.h` の共通関数で、タイムアウト時に `std::runtime_error` を送出し、受信した出力を返します。現在の利用者は `app/porter` だけです。

```cpp
// 現状
ASSERT_NO_THROW(waitForOutput(recv_h_, "Hello Porter", 3000)); // [手順] - RECIEVER が "Hello Porter" を出力するまで待機する。
// [確認_正常系] - RECIEVER の "Hello Porter" を待つ waitForOutput() が例外を送出しないこと。

// 案 A: testfw に例外を送出しない版を加える
bool actual_ret_wait_hello =
    tryWaitForOutput(recv_h_, "Hello Porter", 3000); // [手順] - RECIEVER が "Hello Porter" を出力するまで待機する。
ASSERT_TRUE(actual_ret_wait_hello); // [確認_正常系] - tryWaitForOutput() の戻り値が true であること。(RECIEVER が "Hello Porter" を出力したことの確認)

// 案 B: 規則に例外を設ける (コードは変えない)
// 「呼び出しを包む ASSERT_NO_THROW / ASSERT_THROW は、分けずに Act に置いてよい」
```

案 A の関数名 (`tryWaitForOutput`) と引数 (受信した出力を受け取る出力引数の有無) は、最終仕様で確定します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `framework/testfw` に、成否を戻り値で返す待機関数を加え、porter の Act の 72 件を呼び出しと判定に分ける。
- [ ] B: 規則に「呼び出しを包む `ASSERT_NO_THROW` / `ASSERT_THROW` は Act に置いてよい」という例外を加え、コードは変えない。
- [ ] C: `app/porter` のテストの中に局所的な補助関数を作り、分ける。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 待機は testfw の共通機能で、成否を返す版は今後のプロセス制御のテストでも使えます。C は同じ機能を app ごとに作ることになります。B は規則の例外を増やし、Act の中の判定の書き方が 2 通りになります。testfw の変更は追加だけで、既存の `waitForOutput()` は変えません。

**Related**

[Q-015](#q-015-arrange-の-assert_no_throw-もそろえるか)

**Assessment**

推奨案 A が選ばれました。待機関数の名前と引数は最終仕様で確定します。既存の `waitForOutput()` は変えません。

### Q-015 Arrange の ASSERT_NO_THROW もそろえるか

**Status**

解決済み

**Question**

[Q-014](#q-014-act-に残した-assert_no_throw-の直し方) で案 A を選んだ場合、Arrange の `[状態確認]` に使っている `ASSERT_NO_THROW(waitForOutput(...))` (43 件) も新しい待機関数へ書き換えますか。

**Context**

Arrange の `ASSERT_NO_THROW` は、`about-test-phase.md` の「1 行の `ASSERT_*` が準備と確認を兼ねるときは `[状態]` と `[状態確認]` の両方を付ける」に合っており、規則には反していません。Act だけを書き換えると、同じファイルの中で待機の書き方が 2 通りになります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 書き換える (同じファイルの待機の書き方をそろえる)。
- [ ] B: 書き換えない (Act の 72 件だけを直す)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 読み手が 2 通りの待機の書き方を見比べずに済みます。確認件数は変わりません。

**Related**

[Q-014](#q-014-act-に残した-assert_no_throw-の直し方)

**Assessment**

推奨案 A が選ばれました。Arrange の 43 件も新しい待機関数へ書き換えます。

### Q-016 Act の途中の EXPECT_CALL の直し方

**Status**

解決済み

**Question**

`stringCatalogFilterSourceTest.cc` で Act の途中に置いている `EXPECT_CALL(...).Times(0)` (2 件) を、どう直しますか。

**Context**

1 回目の組み立てでは `cplat_local_lock_try_lock()` が呼ばれてよく、2 回目では呼ばれないことを確かめるため、Act の途中で期待を設定しています。Pre-Assert へ移すと、1 回目の呼び出しも禁じることになり、判定の意味が変わります。

```cpp
// 現状
// Act
(void)format_job_failed(&actual_matched); // [手順] - 壊れた公開内容で組み立てる。
cplat_string_catalog_filter_source_status status = source_status();
EXPECT_CALL(mock_cplat, cplat_local_lock_try_lock(_)).Times(0); // [Pre-Assert確認_異常系] - ...
(void)format_job_failed(&actual_matched); // [手順] - もう一度組み立てる。

// 案 A: マルチ フェーズ
// Act_1
(void)format_job_failed(&actual_matched); // [手順] - 壊れた公開内容で組み立てる。
cplat_string_catalog_filter_source_status status = source_status();

// Assert_1
(1 回目の結果の判定があれば置く)

// Pre-Assert_2
EXPECT_CALL(mock_cplat, cplat_local_lock_try_lock(_)).Times(0); // [Pre-Assert確認_異常系] - ...

// Act_2
(void)format_job_failed(&actual_matched); // [手順] - もう一度組み立てる。

// Assert_2
EXPECT_NE(0, actual_matched); // [確認_異常系] - ...
```

1 回目の後に判定するものがない場合、Assert_1 は空になります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) マルチ フェーズ (Act_1、Pre-Assert_2、Act_2、Assert_2) にする。Assert_1 は判定がなければ空のまま置く。
- [ ] B: 規則に「呼び出しの順序に依存する期待は Act の途中に置いてよい」という例外を加え、コードは変えない。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: マルチ フェーズの形に合い、`[Pre-Assert確認_*]` が Pre-Assert に置かれます。規則の例外を増やさずに済みます。

**Related**

[Q-003](#q-003-argparser-の空の-act-の直し方)

**Assessment**

推奨案 A が選ばれました。

### Q-017 Pre-Assert 手順の欠落への対応

**Status**

解決済み

**Question**

`WillOnce` / `WillRepeatedly` で動作を注入しているのに `[Pre-Assert手順]` がない `EXPECT_CALL` (276 件) を、どう扱いますか。

**Context**

`about-test-phase.md` は「呼び出された場合に戻り値を返す、出力引数や変数を操作する動作 (`WillOnce` / `WillRepeatedly` の中身) は `[Pre-Assert手順]` に記載します」と定め、`text_style_jp.py` の `test-comment-expect-call-step` が不足を指摘します。指摘は 276 件で、大半は `WillOnce(Return(...))` です。いずれも前案件より前からあり、前案件では確認コメントが動作を述べていた箇所だけに `[Pre-Assert手順]` を加えました。

```cpp
// 現状 (点検で指摘される)
EXPECT_CALL(mock_stdio_, printf(_, _, _, StrEq(kRcSuccessTty)))
    .WillOnce(Return(0)); // [Pre-Assert確認_正常系] - printf() が kRcSuccessTty と等しい文字列を指定して 1 回呼び出されること。

// 案 A: 規範どおり [Pre-Assert手順] を加える
EXPECT_CALL(mock_stdio_, printf(_, _, _, StrEq(kRcSuccessTty)))
    .WillOnce(Return(0)); // [Pre-Assert確認_正常系] - printf() が kRcSuccessTty と等しい文字列を指定して 1 回呼び出されること。
                          // [Pre-Assert手順] - printf() にて 0 を返す。

// 案 B: 判定に関わらない戻り値は省略を許す規則にする (コードは変えず、点検を合わせる)
```

案 A では、手順節に注入した戻り値が並び、テストの前提が `results.md` から読めるようになります。手順節の行数は増えます。確認件数は変わりません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 現行の規範どおり、276 件すべてに `[Pre-Assert手順]` を加える (`DoDefault()` は [Q-019](#q-019-dodefault-の扱い) による)。
- [ ] B: 「判定に関わらない戻り値 (例: `printf()` の 0) は省略してよい」と規則を緩め、点検もそれに合わせる。
- [ ] C: 今回は対応せず、残課題として残す。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 規範と点検はすでにそろっており、コードを規範に合わせれば点検の指摘がなくなって、新しい指摘が埋もれなくなります。B の「判定に関わらない」は機械的に判定できず、点検に載せられません。

**Related**

[Q-011](#q-011-新しい規則の機械点検の常設)、[Q-019](#q-019-dodefault-の扱い)

**Assessment**

推奨案 A が選ばれました。276 件に `[Pre-Assert手順]` を加えます。

### Q-018 コミットとプッシュの扱い

**Status**

解決済み

**Question**

作業の完了後、コミット、プッシュ、CI の追跡までを今回の作業に含めますか。

**Context**

前案件では、ユーザーの指示によりサブモジュールごとのコミットとプッシュ、リポジトリ本体のサブモジュール参照の更新、CI の追跡と修正までを行いました。`AGENTS.md` は、Git の書き込み操作をユーザーの個別の指示があるまで行わないことを求めています。

**Answer**

選択方式: 単一選択

- [x] A: 含める (前案件と同じく、サブモジュール、リポジトリ本体の順にコミットとプッシュを行い、CI を追跡して必要なら追加のコミットとプッシュを行う)。
- [ ] B: 含めない (局所テストと機械的検証までとし、Git の操作はユーザーが行うか、改めて指示する)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: なし  
理由: Git の書き込み操作の許可はユーザーが決めることで、AI が推奨する根拠がありません。

**Assessment**

A が選ばれました。この案件に限り、サブモジュール、リポジトリ本体の順のコミットとプッシュ、CI の追跡と追加のコミットとプッシュを行います。

### Q-019 DoDefault の扱い

**Status**

解決済み

**Question**

`.WillOnce(DoDefault())` のように既定の動作に任せるだけの `EXPECT_CALL` (7 件程度) を、どう扱いますか。

**Context**

現行の規範は、`WillOnce` / `WillRepeatedly` の中身を `[Pre-Assert手順]` に書くことを求め、`text_style_jp.py` の `test-comment-expect-call-step` もこれを点検しています。一方、`DoDefault()` は動作を注入せず、`ON_CALL` で設定した既定の動作 (または本物の関数への委譲) をそのまま行います。`about-test-phase.md` 35 行目は、既定動作を `[Pre-Assert手順]` に書くと注入値と矛盾して解釈されるおそれがあるとして、`ON_CALL` での使用を禁じています。

`.WillOnce(DoDefault())` を 1 つだけ書いた `EXPECT_CALL` は、`.Times(1)` と同じ意味です (Google Mock は、`WillOnce` の数を呼び出し回数とし、動作を指定しない呼び出しには既定の動作を使います)。

```cpp
// 現状 (点検で指摘される)
EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
    .WillOnce(DoDefault()); // [Pre-Assert確認_正常系] - printf() が "3\n" と等しい文字列を指定して 1 回呼び出されること。

// 案 A: 動作の指定をやめ、回数で書く
EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
    .Times(1); // [Pre-Assert確認_正常系] - printf() が "3\n" と等しい文字列を指定して 1 回呼び出されること。

// 案 B: [Pre-Assert手順] を書く
EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
    .WillOnce(DoDefault()); // [Pre-Assert確認_正常系] - printf() が "3\n" と等しい文字列を指定して 1 回呼び出されること。
                            // [Pre-Assert手順] - printf() にて既定の動作を行う。

// 案 C: DoDefault() だけの WillOnce を点検の対象から外す (コードは変えない)
```

`DoDefault()` と別の動作を混ぜている `EXPECT_CALL` (例: 1 回目は失敗を返し、2 回目は既定の動作) は、注入する動作があるため `[Pre-Assert手順]` を書きます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `DoDefault()` だけの `WillOnce` / `WillRepeatedly` は、動作の指定をやめて `.Times(n)` で書く。規則に明記する。
- [ ] B: `[Pre-Assert手順]` に「既定の動作を行う」と書く。
- [ ] C: `DoDefault()` だけの `WillOnce` を点検の対象から外し、コードは変えない。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:35:20 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 動作を注入しない期待は回数の判定だけで、`[Pre-Assert手順]` が要らない形に直せます。B は `ON_CALL` で禁じている既定動作の記載を `EXPECT_CALL` では許すことになり、規範の理由と食い違います。C は点検の例外を増やします。

**Related**

[Q-011](#q-011-新しい規則の機械点検の常設)、[Q-017](#q-017-pre-assert-手順の欠落への対応)

**Assessment**

推奨案 A が選ばれました。

### Q-020 空ポインターの規範の置き場所

**Status**

解決済み

**Question**

[Q-006](#q-006-nullptr-と-null-の表記) の補足で求められた NULL と nullptr の規範を、どの文書に置きますか。

**Context**

現行の規範には、空ポインターの表記 (`NULL`、`nullptr`、`0`) の定めがありません。`app/general/docs/coding-guideline.md` は C と C++ の両方を対象とし、言語標準を C17 と C++17 と定めています。C17 には `nullptr` がなく (C23 で追加)、C では `NULL` を使うほかありません。同じ文書の「ゼロ初期化」の章は、C と C++ で書き方が違う規則を 1 つの章に並べています。

`framework/testfw/docs/about-test-phase.md` は、確認コメントの書き方の正本です。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `coding-guideline.md` に「空ポインターの表記」の章を設け、C と C++ の規則を並べて書く。`about-test-phase.md` には、確認コメントの空判定の表記だけを書く。
- [ ] B: `about-test-phase.md` に、C++ のテスト コードの規則だけを書く。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:41:10 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 空ポインターの表記はテストに限らず C と C++ の全コードに関わり、言語ごとの違いを 1 か所で比べられます。「ゼロ初期化」の章と同じ書き分けになります。

**Related**

[Q-006](#q-006-nullptr-と-null-の表記)、[Q-021](#q-021-c-の空ポインター定数)、[Q-023](#q-023-c-の空ポインター定数)

**Assessment**

推奨案 A が選ばれました。`app/general/docs/coding-guideline.md` に「空ポインターの表記」の章を設け、`about-test-phase.md` には確認コメントの空判定の表記だけを書きます。

### Q-021 C の空ポインター定数

**Status**

解決済み

**Question**

C のコードで、空ポインターを表す値に何を使いますか。

**Context**

C17 で空ポインターを表せるのは `NULL`、`0`、`(void *)0` です。現在の C のコード (テストと外部由来のものを除く 217 ファイル) は `NULL` を使っており、`(void *)0` は 0 件です。ポインターへ `0` を代入する箇所は機械的な抽出で約 30 件の候補がありますが、整数の変数を含むため個別の確認が必要です。

```c
/* 案 A */
char *buf = NULL;
return NULL;

/* 案 B で許す書き方 */
char *buf = 0;
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `NULL` だけを使う。`0` と `(void *)0` は使わない。
- [ ] B: `NULL` を推奨し、`0` も許す。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:41:10 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: ポインターであることがコードから読み取れ、現状のコードもほぼこの形です。

**Related**

[Q-020](#q-020-空ポインターの規範の置き場所)、[Q-022](#q-022-c-の空判定の書き方)

**Assessment**

推奨案 A が選ばれました。ポインターへ `0` を代入している候補 (約 30 件) は、整数の変数を除くよう個別に確認して `NULL` に直します。

### Q-022 C の空判定の書き方

**Status**

解決済み

**Question**

C のコードで、ポインターが空かどうかの判定をどう書きますか。

**Context**

現在の C のコードは `ptr == NULL` / `ptr != NULL` の明示的な比較が 2,124 件です。`if (!x)` の形は 64 件ありますが、上位はいずれも真偽値の変数 (`is_tty`、`is_open` など) で、ポインターに使っている箇所はほとんど見当たりません。同じ規範の「判定慣用句」は、戻り値の判定にコード名との明示的な比較を求めています。

```c
/* 案 A */
if (buf == NULL)
{
    return SAMPLE_ERR_NO_MEMORY;
}

/* 案 B で許す書き方 */
if (!buf)
{
    return SAMPLE_ERR_NO_MEMORY;
}
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `== NULL` / `!= NULL` で明示的に比べる。`!ptr` やポインターをそのまま条件にする書き方は使わない。
- [ ] B: どちらも許す。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:41:10 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 判定の対象がポインターであることが読み取れ、「判定慣用句」の明示的な比較と方針がそろいます。現状のコードもほぼこの形で、移行はわずかです。

**Related**

[Q-021](#q-021-c-の空ポインター定数)、[Q-023](#q-023-c-の空ポインター定数)

**Assessment**

推奨案 A が選ばれました。Q-022 の推奨理由 (「移行はわずか」) のとおり、既存の C のコードでポインターをそのまま条件にしている箇所も直します。版 2 の回答後の横断確認で、ポインターと推定される変数をそのまま条件にしている箇所は C で約 10 件 (真偽値の誤抽出を含む) でした。作業時に型を確かめて直します。

### Q-023 C++ の空ポインター定数

**Status**

解決済み

**Question**

C++ のコードで、空ポインターを表す値と判定の書き方をどうしますか。

**Context**

C++ のコードは、テスト コード、mock、`framework/testfw` のライブラリです。`nullptr` は C++11 で加わった空ポインターの値で、整数の `0` と区別され、どのポインター型にもキャストなしで変換されます。`NULL` は処理系によって `0` や `0L` と定義され、関数の多重定義やテンプレートの型推論で整数として扱われることがあります。

ヘッダーのうち C からも読み込まれるもの (`prod/include/` の公開ヘッダーなど) は C として翻訳されるため、`nullptr` を使えません。

```cpp
// 案 A
ASSERT_NE(nullptr, buf);
EXPECT_CALL(mock, fopen(_, _)).WillOnce(Return(nullptr));
if (handle == nullptr) { ... }

// 現状の例
ASSERT_NE((uint8_t *)NULL, buf);
EXPECT_CALL(mock, fopen(_, _)).WillOnce(Return((FILE *)NULL));
```

判定は C と同じく、`== nullptr` / `!= nullptr` の明示的な比較とします (Google Test の `ASSERT_NE(nullptr, p)` などのマクロを含みます)。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `nullptr` だけを使い、`NULL` と `0` は使わない。C からも読み込まれるヘッダーは C の規則 (`NULL`) に従う。判定は `== nullptr` / `!= nullptr` の明示的な比較とする。
- [ ] B: `nullptr` を推奨し、C の API に渡す引数と C の API の戻り値との比較では `NULL` も許す。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:41:10 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 型付きの空ポインターでキャストが要らなくなり ([Q-004](#q-004-確認コメントのキャスト式) の対象も減ります)、多重定義の取り違えを防げます。B は「C の API かどうか」の判断が書き手に委ねられ、表記が揺れます。

**Related**

[Q-006](#q-006-nullptr-と-null-の表記)、[Q-020](#q-020-空ポインターの規範の置き場所)、[Q-024](#q-024-c-の既存コードの移行範囲)

**Assessment**

推奨案 A が選ばれました。C からも読み込まれるヘッダーの判定基準 (どのヘッダーが該当するか) は、最終仕様で `.c` から読み込まれるかどうかで定めます。

### Q-024 C++ の既存コードの移行範囲

**Status**

解決済み

**Question**

[Q-023](#q-023-c-の空ポインター定数) の規則を、既存の C++ のコードのどこまでに適用しますか。

**Context**

[Q-006](#q-006-nullptr-と-null-の表記) の案 A で確定したのは、テスト コードの空判定 (約 186 件) です。C++ のソースのコード上の `NULL` は、コメントと文字列を除いて約 3,200 件あり、`app/cplat/test` が約 3,100 件、`framework/testfw/libsrc` が 75 件、ほかの app が合わせて約 20 件です。大半は、C の API に渡す引数 (`cplat_xxx(NULL, ...)`) や mock の戻り値 (`Return((FILE *)NULL)`) です。

このリポジトリでは、規範を新設したら既存のコードも同じ変更で全体を移行する進め方をとっています。`NULL` から `nullptr` への置き換えは動作を変えませんが、可変長引数の関数へ渡す箇所や、テンプレートの型推論に関わる箇所はビルドとテストで確認します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) すべての C++ のソース (テスト コード、mock、`framework/testfw/libsrc`) のコード上の `NULL` を `nullptr` に置き換える。コメントと文字列は変えない (確認コメントは [Q-006](#q-006-nullptr-と-null-の表記) による)。
- [ ] B: テスト コードの空判定 (約 186 件) だけを置き換え、ほかは変更した箇所から順次直す。
- [ ] C: `test/` 配下のテスト コードと mock だけを置き換え、`framework/testfw/libsrc` は除く。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-16 08:41:10 JST (検討用 Markdown の保存日時)

**Recommendation**

推奨: A  
理由: 規範と既存コードが一致し、新しいコードを書くときに手本が揺れません。置き換えは機械的で、ビルドとテストで確かめられます。C の対象外とする理由 (testfw のライブラリだけ残す理由) は見当たりません。

**Related**

[Q-006](#q-006-nullptr-と-null-の表記)、[Q-023](#q-023-c-の空ポインター定数)

**Assessment**

推奨案 A が選ばれました。[Q-023](#q-023-c-の空ポインター定数) の明示的な比較の規則により、C++ のコードでポインターをそのまま条件にしている箇所 (横断確認で 1 件) も同じ変更で直します。外部由来の `framework/testfw/gtest/` は対象外です (`AGENTS.md` の外部由来の配布物の規定)。

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `prompt/test-assert-comment-followup-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 3 (2026-10-16)

最終仕様 (design.md) の合意版 (第 2 ゲート): 版 1 (2026-10-16 Tetsuo Honda 承認)

更新予定の成果物: 未確定

受入条件: 未確定

## 反映と検証の記録

未着手。
