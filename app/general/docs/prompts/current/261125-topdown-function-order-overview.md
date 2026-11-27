<!--
生成元: agree-before-implement スキル
文書種別: 全体管理文書 (overview)
最終更新日時: 2026-11-27 12:37:39 JST
最終更新エージェント: agy (gemini-4-neon-medium、主セッション session_01KJc53RTnwmpzd7VMLf1qR9)
稼働 OS: Linux (WSL)
下流文書: `261125-topdown-function-order-discussion.md` (検討用 Markdown)、`261125-topdown-function-order-lightadr.md` (簡易 ADR)、`261125-topdown-function-order-design.md` (最終仕様)、`261125-topdown-function-order-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。この文書と、ユーザーが編集した最新の検討用 Markdown を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# app 配下ソースのトップ ダウン記載統一 全体管理

ファイル名: `261125-topdown-function-order-overview.md`

この文書は、案件の識別、作業 WBS、対象範囲、確認状況、経緯を管理します。質問と回答の本文は検討用 Markdown (`261125-topdown-function-order-discussion.md`) に、決定事項は簡易 ADR (`261125-topdown-function-order-lightadr.md`) に記載し、この文書には重複して記載しません。

## 案件の識別

検討 ID: `261125-topdown-function-order`

検討用 Markdown の版: 4 (合意済み。版 4 は第 2 ゲートでの差し戻しへの対処の版で、新しい Q はない)

状態: 作業実施中 (第 2 段)

## 作業 WBS と進捗管理

この案件の作業 WBS です。最終仕様 (`261125-topdown-function-order-design.md`) は、Opus のレビュー指摘に基づく差し戻しの後、ユーザーの指示により改訂し、ユーザーの自走の指示により第 2 ゲートを承認済みとして扱います。第 4 章の作業は、主エージェントの指示で着手します。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 前提条件と要求事項の調査 (`app/makepart.mk`、`coding-guideline.md`、GCC/MSVC の警告仕様、既存 `app/` 配下第一者管理ソースの調査)
    - ✅ 1.2. 課題・論点の抽出と明確化のための確認項目 ([Q-001](261125-topdown-function-order-discussion.md#q-001-適用対象とするソース-ファイルの範囲) 〜 [Q-010](261125-topdown-function-order-discussion.md#q-010-既存ファイル一括改修後のビルド・テスト検証範囲)) の作成および簡易 ADR 記録
    - ✅ 1.3. Q&A ターンごとのユーザー回答の受付と記録 (ターン 1: Q-001 〜 Q-008、ターン 2: Q-009 〜 Q-010)
    - ✅ 1.4. 回答に基づく前提・制約の検証、簡易 ADR (`261125-topdown-function-order-lightadr.md`) への決定事項の反映、WBS の見直し
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点 (Q-001 〜 Q-010) に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定 (簡易 ADR LADR-001 〜 LADR-006 と回答の一致を確認し、フェーズ 2 へ着手)
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `261125-topdown-function-order-design.md` の作成と、レビュー指摘に基づく改訂 (適用範囲のファイル単位化、構成順の変更、`inline` の記述の削除、並べ替えツールと等価性の検証、Git と CI の手順、段階分け、判定基準の一意化、件数の実測)
    - ✅ 3.2. 最終仕様における受入条件と整合性の確認 (簡易 ADR LADR-001 〜 LADR-008 との一致)
    - ✅ 3.3. 対話セッションにおける最終仕様 (`261125-topdown-function-order-design.md`) の承認の受付 (第 2 ゲート: 作業着手の合意。2026-11-25 21:48 の対話の「ユーザーの確認なく継続的に自走すること」の指示により承認済みとして扱う。承認を、最終仕様の「Git の運用と書き込み操作の許可の範囲」の許可とする)
- 🔄 4. 承認された最終仕様に従った作業の実施 (`make` は同時に 1 本、`make clean` は禁止。Git の書き込み操作は主エージェントだけが行う)
    - ✅ 4.1. 第 1 段 (`app/general` の規範とツール、`cplat` と `porter` 以外の app)
        - ✅ 4.1.1. コーディング規範 (`app/general/docs/coding-guideline.md`) への `## 関数の記載順と前方宣言` 節の追加と、概要・`## 実装コメント` 節の整合更新、`text_style_jp.py --dry-run` / `--in-place`
        - ✅ 4.1.2. チェッカー (`app/general/bin/check_c_topdown.py`) と単体テスト (`app/general/bin_test/test_check_c_topdown.py`) の作成
        - ✅ 4.1.3. 並べ替えツール (`app/general/bin/reorder_c_topdown.py`、等価性の検証を含む) と単体テスト (`app/general/bin_test/test_reorder_c_topdown.py`) の作成
        - ✅ 4.1.4. `make bin-test BIN_TEST_ARGS=app/general` の実行
        - ✅ 4.1.5. `app/general` の単位のコミット、main への push、CI の確認 (親リポジトリの 76b19dfc を main へ push 済み。CI 45692020891 は成功)
        - ✅ 4.1.6. `service-sample` (違反 5 ファイル) の書き換え、`--verify`、`make`、`.warn` の確認、`make test` (チェッカーのエラー 94 件を 0 件にした。`check_c_blank_lines.py` の検出は 23 件のまま。`.warn` は空。`make test` は合格)
        - ✅ 4.1.7. `service-sample` のコミット、push、CI の確認 (親リポジトリの 9ab53061 を main へ push 済み。CI 45692020891 は成功)
        - ✅ 4.1.8. `string-catalog-sample` (違反 7 ファイル) の書き換え、`--verify`、`app/string-catalog-sample/docs/architecture.md` の 240 行ほかの行番号の追従、`make`、`.warn` の確認、`make test` (チェッカーのエラー 91 件を 0 件にした。`architecture.md` の 240 行の `string-catalog-command-sample.c:273` を、書き換え後の `sample_trace_key_state_dump` の呼び出し行の 355 へ直した。`.warn` は空。`make test` は合格)
        - ✅ 4.1.9. `string-catalog-sample` のコミット、push、CI の確認 (親リポジトリの f2d5d837 を main へ push 済み。CI 45692020891 は成功)
        - ✅ 4.1.10. `cjson` の第一者管理の拡張 (`integer/cJSON_Integer.c`、`jsonc/cJSON_JSONC.c`) の書き換え、`--verify`、`app/cjson` の `make`、`.warn` の確認、`make test` (`app/cjson` はサブモジュール (`Hondarer/app_cjson`)。チェッカーのエラー 39 件を 0 件にした。`check_c_blank_lines.py` の検出は 79 件のまま。`.warn` は空。`make test` は合格)
        - ✅ 4.1.11. `cjson` リポジトリのコミットと push、親リポジトリの参照の更新のコミットと push、CI の確認 (cjson の main 3317d91 と、親リポジトリの参照の更新 42f4f0f2 を push 済み。CI 45692020891 は成功)
        - ✅ 4.1.12. 違反のない app (`calc`、`doxygen-sample`、`override-sample`、`subfolder-sample`、`tutorial`、`zlib_sample.c`、`cjson_sample.c`) の `check_c_topdown.py` の確認 (`calc` 8、`doxygen-sample` 7、`override-sample` 6、`subfolder-sample` 6、`tutorial` 2、`zlib` 1、`cjson_sample.c` を含む `cjson` 3 ファイルで、エラー 0 件、警告 0 件)
        - ✅ 4.1.13. `struct-meta` (違反 17 ファイル) の書き換え、`--verify`、`make`、`.warn` の確認、`make test` (開始条件の U1 (BL-031) は main (937b6ab0) へ統合済みのため、作業ブランチ `work/261125-consumable-backlog-items` (839387f4) の作業ツリーで実施した。チェッカーのエラー 161 件を 0 件にした (対象 23 ファイル、変更 17 ファイル、警告 0 件)。17 ファイルはすべて手書きで、生成物の違反はない。`--verify` は全 17 ファイルに合格。`check_c_blank_lines.py` の検出は対象 17 ファイルで書き換え前後とも 17 件のまま。`make` 後の `.warn` は空。`make test` は合格 (18 テスト ディレクトリで Passed 129 件、Failed 0 件)。`architecture.md` に行番号の参照はなく、追従は不要)
        - ✅ 4.1.14. `struct-meta` のコミット、push、CI の確認 (作業ブランチの 803b8a3a を、並行案件の U4〜U7 とともに main の 712b55ff へ統合して push。Windows の検証は 128 件成功 (Windows で除外の 1 件を除き Linux の 129 件と一致)。CI 45751634321 は成功)
    - 🔄 4.2. 第 2 段 (`porter`、`cplat`、並行案件が変更したファイルの再実行)
        - ✅ 4.2.1. 開始条件の確認 (並行案件 `261125-consumable-backlog-items` と `261123-porter-remaining-backlog-phase2` の main への統合、または統合なしの終了。満たせない場合は残りを残課題一覧へ送る) (`261125-consumable-backlog-items` は main へ統合済みで、第 3 ゲートの承認を受けて完了した。`261123-porter-remaining-backlog-phase2` は各サブモジュールの main への統合と、親リポジトリの参照の更新 (親の main の dbc28819) を終えた。統合後の CI 45888369745 は実行中だが、開始条件は満たされたものとして扱う)
        - ✅ 4.2.2. `porter` (違反 42 ファイル) の書き換え、`--verify`、`make`、`.warn` の確認、`make test` (並行案件の統合後の数え直しで違反は 41 ファイル、エラー 551 件 (`missing-prototype` 336 件)。生成物と外部由来のファイルはない。40 ファイルを `reorder_c_topdown.py` で、`#pragma warning` で `main` を囲む `tcpServer.c` を手作業で書き換え、41 ファイルすべてが `--verify` に合格した。チェッカーのエラーは 0 件、警告は 0 件 (対象 58 ファイル)。`check_c_blank_lines.py` の検出は 1,059 件から 1,058 件で、増えていない。`make` 後の `.warn` は空。`make test` は合格 (実行した 50 テスト ディレクトリで Passed 895 件、Failed 0 件)。`test.stamp` で実行を省いた 7 ディレクトリは、書き換えたソースを含まないため流し直していない。Windows 専用の `tcpServerWindowsTest` は Linux では実行されない。`app/porter/docs/` の `prompts/` の外に行番号の参照はなく、追従は不要)
        - 🔄 4.2.3. `porter` リポジトリのコミットと push、親リポジトリの参照の更新のコミットと push、CI の確認 (porter の main 91fa196 へコミットと push。Windows の検証 (依頼 20) は成功し、cplat の全体 2,172 件、porter の全体 996 件が合格 (test.stamp で省かれた 32 ディレクトリも流し直して合格)。#if の分岐の中の宣言の崩れ (BL-006) は Windows でも見つからず、MSVC の警告と内容のある .warn はなし。親リポジトリの参照の更新を push し、CI の結果待ち)
        - ✅ 4.2.4. `cplat` (違反 81 ファイル) の書き換え、`--verify`、`make`、`.warn` の確認、`make test` (対象 117 ファイル、違反 81 ファイル、エラー 1,485 件 (`missing-prototype` 746 件、`non-topdown-order` 739 件)、警告 1 件 (`console.c` の `order-mismatch`)。生成物と外部由来のファイルは対象外。80 ファイルを `reorder_c_topdown.py` で、`prompt_windows.c` を手作業で書き換え、さらに `#if` 分岐内の `#include`・マクロ・型定義・静的変数が関数より後へ移った `elevated_process.c`・`process.c`・`eventlog-register.c` を手作業で整え、全 81 ファイルが `--verify` に合格した。チェッカーは BL-004 の 2 ファイル (`console.c` 5 件、`etw-viewer.c` 2 件の計 7 件の `non-topdown-order`) を除きエラー 0 件、警告 0 件。`check_c_blank_lines.py` の検出は 456 件のまま増えていない。`make` 後の `.warn` は空。`make test` は合格 (実行した 84 テスト ディレクトリで Passed 2,202 件、Warning 0 件、Failed 0 件、所要 02:04:01)。`test.stamp` で実行を省いた 17 ディレクトリは、いずれも書き換えたソースをビルドに含まないため流し直していない。`app/cplat/docs/` (`prompts/` を除く) とソース コメントに行番号の参照はなく、追従は不要)
        - 🔄 4.2.5. `cplat` リポジトリのコミットと push、親リポジトリの参照の更新のコミットと push、CI の確認 (cplat の main 1780861a へコミットと push。Windows の検証は 4.2.3 と同じ。親リポジトリの参照の更新を push し、CI の結果待ち) ← **CURRENT**
        - ⚠️ 4.2.6. 第 1 段の後に並行案件が変更したファイルへの `reorder_c_topdown.py` の再実行、検証、コミット、push、CI の確認 (第 1 段コミット後に変更された `.c` は 9 ファイル (`service-sample.c`、`add.c`、`calc.c`、`shared-and-static-calc.c`、`override-sample.c`、`sample-app/main.c`、`hello.c`、`hello_with_args.c`、`cJSON_JSONC.c`) で、`struct-meta` の `9376a220` は生成元の `struct_meta_parse.y` のみ。9 ファイルすべて `check_c_topdown.py` でエラー 0 件・警告 0 件のため、書き換え・再ビルド・再テストは不要。コミット・push・CI の確認は主エージェントが行う)
    - ⚠️ 4.3. 全対象 (2026-11-25 時点で 244 ファイル) への `check_c_topdown.py` の実行 (エラーと警告 0 件) と実行時間の計測 (2026-11-27 時点の全対象は 246 ファイル (`cplat` が 115 → 117 ファイルへ増加)。結果はエラー 7 件、警告 0 件で、残ったのは BL-004 の 2 ファイル (`cplat` の `console.c` 5 件、`etw-viewer.c` 2 件) のみ。実行時間は約 1.67 [s] (3 回計測: 1.659 [s]、1.677 [s]、1.672 [s])。BL-004 のため受入条件 4 を満たせない扱いは主エージェントが決める)
    - ⏸️ 4.4. 受入条件への適合確認
    - ⏸️ 4.5. 簡易 ADR (`261125-topdown-function-order-lightadr.md`) の最終確認
    - ⏸️ 4.6. 残課題一覧 (`261125-topdown-function-order-backlog.md`) の再構築と整理
- ⏸️ 5. 成果物の承認の受付 (第 3 ゲート)
    - ⏸️ 5.1. 成果物・検証結果・残課題の提示
    - ⏸️ 5.2. 対話セッションにおける成果物の承認の受付 (「承認」または「完了」の受領によるスキル完了)

## 対象と参照資料

対象成果物 (確定):

- `app/general/docs/coding-guideline.md`
- `app/general/bin/check_c_topdown.py`
- `app/general/bin_test/test_check_c_topdown.py`
- `app/general/bin/reorder_c_topdown.py`
- `app/general/bin_test/test_reorder_c_topdown.py`
- 対象の `.c` のうち違反のあるファイル (2026-11-25 時点で 154 ファイル。親リポジトリ、`cjson`、`porter`、`cplat`)
- 行番号を引用する文書 (`app/string-catalog-sample/docs/architecture.md` ほか)

今回の対象範囲: `app/*/prod/` 配下の `.c` のうち、各リポジトリで追跡され、ignore 対象でもシンボリック リンクでもないファイル (2026-11-25 時点で 244 ファイル) におけるトップ ダウン記載統一の規範策定、チェッカーと並べ替えツールの整備、既存の違反ファイルの書き換え。`packages/` からの展開物、自動生成物、シンボリック リンク、`test/`、C++ ソース、ヘッダー、`app/makepart.mk`、`framework/makefw`、CI ワークフロー設定は対象外

書き込み可能な検討文書: この文書、`261125-topdown-function-order-discussion.md`、`261125-topdown-function-order-lightadr.md`、`261125-topdown-function-order-design.md`、`261125-topdown-function-order-backlog.md`

参照資料と版:

- `AGENTS.md` (リポジトリ ルート、2026-11-25 時点のワーク ツリー)
- `app/general/AGENTS.md` (2026-11-25 時点のワーク ツリー)
- `app/general/docs/coding-guideline.md` (2026-11-25 時点のワーク ツリー、特に「実装コメント」「宣言と定義の関係」節)
- `app/makepart.mk` (2026-11-25 時点のワーク ツリー)
- `framework/makefw/makefiles/_flags.mk` (2026-11-25 時点のワーク ツリー)
- `app/general/bin/check_c_blank_lines.py` (既存の C コード機械チェックの参考実装)
- 並行案件の最終仕様と全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-design.md`、`app/porter/docs/prompts/current/261123-porter-remaining-backlog-phase2-overview.md` (2026-11-25 時点のワーク ツリー)
- 最終仕様のレビュー (Opus、2026-11-25。主セッションの一時ディレクトリの `topdown-review/` に走査スクリプトと結果)

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | 依頼文 ([LADR-001](261125-topdown-function-order-lightadr.md#ladr-001-トップ-ダウン記載スタイルへの統一方針)、[LADR-002](261125-topdown-function-order-lightadr.md#ladr-002-関数記載順の非決定論的指針と宣言・定義の順序一致))、[Q-006](261125-topdown-function-order-discussion.md#q-006-トップ-ダウン記載における関数記載順の指針) |
| 範囲・境界 | 確認済み | 2026-11-25 21:48 の対話の指示 ([LADR-003](261125-topdown-function-order-lightadr.md#ladr-003-適用対象の範囲と既存コードの一括改修) のファイル単位の境界、[LADR-007](261125-topdown-function-order-lightadr.md#ladr-007-段階分けと並行案件との順序) の段階分け)、[Q-001](261125-topdown-function-order-discussion.md#q-001-適用対象とするソース-ファイルの範囲)、[Q-002](261125-topdown-function-order-discussion.md#q-002-既存コードへの一括適用を本案件に含めるか)、[Q-007](261125-topdown-function-order-discussion.md#q-007-c-ソース内の-static-inline-関数の扱い) ([LADR-003](261125-topdown-function-order-lightadr.md#ladr-003-適用対象の範囲と既存コードの一括改修)) |
| 入力・根拠 | 確認済み | `app/makepart.mk`、GCC 8.5.0 実機検証、各リポジトリの `git ls-files` による対象 244 ファイルと違反 154 ファイルの実測、並行案件の最終仕様 |
| 整合性・依存関係 | 確認済み | 2026-11-25 21:48 の対話の指示 (`coding-guideline.md` の「inline 関数」節と `app/general/AGENTS.md` への整合、並行案件との順序: [LADR-003](261125-topdown-function-order-lightadr.md#ladr-003-適用対象の範囲と既存コードの一括改修)、[LADR-007](261125-topdown-function-order-lightadr.md#ladr-007-段階分けと並行案件との順序)、Git の許可: [LADR-008](261125-topdown-function-order-lightadr.md#ladr-008-git-の書き込み操作と-ci-の確認))、[Q-003](261125-topdown-function-order-discussion.md#q-003-機械的チェックの実現方式)、[Q-004](261125-topdown-function-order-discussion.md#q-004-機械的チェックの検査項目と厳格度)、[Q-008](261125-topdown-function-order-discussion.md#q-008-コーディング規範の記載場所) ([LADR-004](261125-topdown-function-order-lightadr.md#ladr-004-機械的チェックの方式・厳格度・実行タイミング)、[LADR-005](261125-topdown-function-order-lightadr.md#ladr-005-コーディング規範の記載場所)) |
| 実現性・例外 | 確認済み | 2026-11-25 21:48 の対話の指示 (構成順の変更による関数表の初期化のコンパイル、並べ替えツール、分岐内の重複定義: [LADR-002](261125-topdown-function-order-lightadr.md#ladr-002-関数記載順の非決定論的指針と宣言・定義の順序一致)、[LADR-004](261125-topdown-function-order-lightadr.md#ladr-004-機械的チェックの方式・厳格度・実行タイミング)。未確定の細部は残課題一覧の BL-001 〜 BL-003)、[Q-003](261125-topdown-function-order-discussion.md#q-003-機械的チェックの実現方式)、[Q-004](261125-topdown-function-order-discussion.md#q-004-機械的チェックの検査項目と厳格度)、[Q-007](261125-topdown-function-order-discussion.md#q-007-c-ソース内の-static-inline-関数の扱い)、[Q-009](261125-topdown-function-order-discussion.md#q-009-宣言と定義の順序不一致に対するチェッカーの振る舞い) ([LADR-004](261125-topdown-function-order-lightadr.md#ladr-004-機械的チェックの方式・厳格度・実行タイミング)) |
| 受入条件・検証 | 確認済み | 2026-11-25 21:48 の対話の指示 (等価性の検証、段ごとの CI の確認: [LADR-006](261125-topdown-function-order-lightadr.md#ladr-006-一括改修後の検証範囲と完了条件)、[LADR-008](261125-topdown-function-order-lightadr.md#ladr-008-git-の書き込み操作と-ci-の確認))、[Q-005](261125-topdown-function-order-discussion.md#q-005-機械的チェックの実行タイミング)、[Q-009](261125-topdown-function-order-discussion.md#q-009-宣言と定義の順序不一致に対するチェッカーの振る舞い)、[Q-010](261125-topdown-function-order-discussion.md#q-010-既存ファイル一括改修後のビルド・テスト検証範囲) ([LADR-004](261125-topdown-function-order-lightadr.md#ladr-004-機械的チェックの方式・厳格度・実行タイミング)、[LADR-006](261125-topdown-function-order-lightadr.md#ladr-006-一括改修後の検証範囲と完了条件)) |

## 質問一覧

各 Q の本文は検討用 Markdown にあります。Q-ID には検討用 Markdown の該当見出しへのリンクを設けます。

| Q-ID | 論点 | 状態 | 依存・関連 | 反映先の決定 |
| --- | --- | --- | --- | --- |
| [Q-001](261125-topdown-function-order-discussion.md#q-001-適用対象とするソース-ファイルの範囲) | 適用対象とするソース ファイルの範囲 | 解決済み | なし | [LADR-001](261125-topdown-function-order-lightadr.md#ladr-001-トップ-ダウン記載スタイルへの統一方針)、[LADR-003](261125-topdown-function-order-lightadr.md#ladr-003-適用対象の範囲と既存コードの一括改修) |
| [Q-002](261125-topdown-function-order-discussion.md#q-002-既存コードへの一括適用を本案件に含めるか) | 既存コードへの一括適用を本案件に含めるか | 解決済み | [Q-001](261125-topdown-function-order-discussion.md#q-001-適用対象とするソース-ファイルの範囲)、[Q-010](261125-topdown-function-order-discussion.md#q-010-既存ファイル一括改修後のビルド・テスト検証範囲) | [LADR-003](261125-topdown-function-order-lightadr.md#ladr-003-適用対象の範囲と既存コードの一括改修) |
| [Q-003](261125-topdown-function-order-discussion.md#q-003-機械的チェックの実現方式) | 機械的チェックの実現方式 | 解決済み | なし | [LADR-004](261125-topdown-function-order-lightadr.md#ladr-004-機械的チェックの方式・厳格度・実行タイミング) |
| [Q-004](261125-topdown-function-order-discussion.md#q-004-機械的チェックの検査項目と厳格度) | 機械的チェックの検査項目と厳格度 | 解決済み | [Q-003](261125-topdown-function-order-discussion.md#q-003-機械的チェックの実現方式)、[Q-009](261125-topdown-function-order-discussion.md#q-009-宣言と定義の順序不一致に対するチェッカーの振る舞い) | [LADR-004](261125-topdown-function-order-lightadr.md#ladr-004-機械的チェックの方式・厳格度・実行タイミング) |
| [Q-005](261125-topdown-function-order-discussion.md#q-005-機械的チェックの実行タイミング) | 機械的チェックの実行タイミング | 解決済み | [Q-003](261125-topdown-function-order-discussion.md#q-003-機械的チェックの実現方式) | [LADR-004](261125-topdown-function-order-lightadr.md#ladr-004-機械的チェックの方式・厳格度・実行タイミング) |
| [Q-006](261125-topdown-function-order-discussion.md#q-006-トップ-ダウン記載における関数記載順の指針) | トップ ダウン記載における関数記載順の指針 | 解決済み | なし | [LADR-002](261125-topdown-function-order-lightadr.md#ladr-002-関数記載順の非決定論的指針と宣言・定義の順序一致) |
| [Q-007](261125-topdown-function-order-discussion.md#q-007-c-ソース内の-static-inline-関数の扱い) | `.c` ソース内の `static inline` 関数の扱い | 解決済み | [Q-001](261125-topdown-function-order-discussion.md#q-001-適用対象とするソース-ファイルの範囲)、[Q-004](261125-topdown-function-order-discussion.md#q-004-機械的チェックの検査項目と厳格度) | [LADR-003](261125-topdown-function-order-lightadr.md#ladr-003-適用対象の範囲と既存コードの一括改修) |
| [Q-008](261125-topdown-function-order-discussion.md#q-008-コーディング規範の記載場所) | コーディング規範の記載場所 | 解決済み | なし | [LADR-005](261125-topdown-function-order-lightadr.md#ladr-005-コーディング規範の記載場所) |
| [Q-009](261125-topdown-function-order-discussion.md#q-009-宣言と定義の順序不一致に対するチェッカーの振る舞い) | 宣言と定義の順序不一致に対するチェッカーの振る舞い | 解決済み | [Q-004](261125-topdown-function-order-discussion.md#q-004-機械的チェックの検査項目と厳格度) | [LADR-004](261125-topdown-function-order-lightadr.md#ladr-004-機械的チェックの方式・厳格度・実行タイミング) |
| [Q-010](261125-topdown-function-order-discussion.md#q-010-既存ファイル一括改修後のビルド・テスト検証範囲) | 既存ファイル一括改修後のビルド・テスト検証範囲 | 解決済み | [Q-002](261125-topdown-function-order-discussion.md#q-002-既存コードへの一括適用を本案件に含めるか) | [LADR-006](261125-topdown-function-order-lightadr.md#ladr-006-一括改修後の検証範囲と完了条件) |

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261125-topdown-function-order-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 4 (2026-11-25 21:57 JST 確定。版 3 の Q&A に、第 2 ゲートでの差し戻しへのユーザーの指示を加えたもの)

最終仕様 (design.md) の合意版 (第 2 ゲート): 2026-11-25 21:57 JST の改訂版。2026-11-25 21:48 の対話の自走の指示により承認済みとして扱う

更新予定の成果物: 「対象と参照資料」の対象成果物のとおり

受入条件 (最終仕様の「受入条件」と同じ):

1. `coding-guideline.md` に `## 関数の記載順と前方宣言` 節があり、概要、`## 実装コメント` 節、「inline 関数」節と矛盾せず、個別の app 名を規範として記載していない。
2. `check_c_topdown.py` と `reorder_c_topdown.py` の単体テストが `make bin-test BIN_TEST_ARGS=app/general` で全件合格する。
3. 書き換えたすべてのファイルが `reorder_c_topdown.py --verify` に合格している。
4. 対象のすべてのファイルに対して `check_c_topdown.py` のエラーと警告が 0 件で、全件の走査の実行時間を記録している。
5. 書き換えた各 app で、Linux (WSL) の `make` が成功し、`.warn` に内容がなく、`make test` が全件合格する。
6. 行番号の引用が書き換え後のソースと一致している。
7. 各段の最後の push の後の CI (Windows/MSVC を含む) が成功し、警告がない。
8. 変更した Markdown に `text_style_jp.py --dry-run` の指摘がない。

## 経緯

案件の進行上の出来事を、発生順に末尾へ追記します。個々の決定の内容は簡易 ADR に記載し、ここには出来事と参照先だけを記載します。

| 日時 (JST) | 出来事 | 内容・参照先 |
| --- | --- | --- |
| 2026-11-25 21:16 | 案件の開始 | 事前調査を実施し、全体管理文書、検討用 Markdown (版 1、Q-001 〜 Q-008)、簡易 ADR (LADR-001、LADR-002) を作成 |
| 2026-11-25 21:26 | ターン 1 回答反映と追加確認の提示 | Q-001 〜 Q-008 の回答を検証・解決済みにし、簡易 ADR (LADR-001 〜 LADR-005) と WBS を更新。版 2 として Q-009・Q-010 を追記 |
| 2026-11-25 21:30 | 第 1 ゲート通過・最終仕様の作成 | Q-009・Q-010 の回答を反映し、全 10 問・全 6 観点が確定。簡易 ADR (LADR-004 更新、LADR-006 追加) を更新し、`261125-topdown-function-order-design.md` を作成して第 2 ゲート承認待ちへ移行 |
| 2026-11-25 21:36 | 第 2 ゲート補足指示反映・最終仕様改版 | 「CI や make では行わないが、ソース コードの新規作成・編集時には対象ファイルに対してチェッカーを実施する」ルールを `discussion.md` (版 3)、`lightadr.md` (LADR-004)、`design.md` に反映し、改めて第 2 ゲート承認待ちへ移行 |
| 2026-11-25 21:48 | 最終仕様の差し戻しとユーザーの指示 | Opus のレビューが最終仕様に 12 件の指摘 (母数への生成物とリンクの混入、app 単位の境界、構成順によるコンパイル不能、`inline` の規範との矛盾、並べ替えの道具と等価性の検証の欠如、Git と CI、Windows 専用コード、編集時の実施の範囲、主たる関数の定義、並び順の例外、判定基準の一意化、並行案件との順序) を挙げた。ユーザーは対話で、1 〜 5 の Opus の指摘での修正、7 の CI での確認、6 の main へのコミットの事前許可、自走、実現できない仕様の backlog への送付、順序性のある作業、この案件の先行を指示した。新しい Q は作らない |
| 2026-11-25 21:57 | 最終仕様の改訂と第 2 ゲートの承認扱い | 検討用 Markdown を版 4 とし (Q の追加なし)、簡易 ADR (LADR-001 〜 LADR-004、LADR-006 の書き換え、LADR-007、LADR-008 の追加)、最終仕様、作業 WBS を改訂。未確定の細部を残課題一覧 (`261125-topdown-function-order-backlog.md`、BL-001 〜 BL-003) に送った。自走の指示により第 2 ゲートを承認済みとして扱い、作業 WBS 4.1.1 で停止 (着手は主エージェントの指示による) |
| 2026-11-26 04:02 | 第 1 段の作業の開始 | 主エージェントの指示により、作業 WBS 4.1.1 〜 4.1.4 (規範の節、チェッカー、並べ替えツール、単体テスト) に着手 |
| 2026-11-26 04:19 | 第 1 段の規範とツールの作成の完了 | 作業 WBS 4.1.1 〜 4.1.4 を完了。チェッカーで対象 244 ファイル、違反 154 ファイルを数え直し、反映と検証の記録に記載。並べ替えツールの試行で判明した、複数の分岐に完全な実装を持つファイルの判定の問題を残課題一覧の BL-004 に送った。作業 WBS 4.1.5 (主エージェントによるコミット、push、CI の確認) で停止 |
| 2026-11-26 04:36 | 第 1 段の app の書き換えの完了 | 作業 WBS 4.1.6、4.1.8、4.1.10、4.1.12 を完了。`service-sample` (5 ファイル)、`string-catalog-sample` (7 ファイル)、`cjson` の拡張 (2 ファイル、サブモジュール) を `reorder_c_topdown.py` で書き換え、`--verify` に合格し、チェッカーのエラーを 0 件にした。`check_c_blank_lines.py` の検出は増えていない。各 app の `make` で `.warn` は空、`make test` は合格。`architecture.md` の行番号を追従。4.1.5、4.1.7、4.1.9、4.1.11 は主エージェントのコミット項目のため要注意とした。4.1.13 (`struct-meta`) は開始条件の待ちのため、ブロック中として停止 |
| 2026-11-26 09:03 | `struct-meta` の書き換えの完了 | 開始条件の U1 (BL-031) の main (937b6ab0) への統合を受け、作業 WBS 4.1.13 を完了。`struct-meta` の 17 ファイルを `reorder_c_topdown.py` で書き換え、`--verify` に合格し、チェッカーのエラー 161 件を 0 件にした。`check_c_blank_lines.py` の検出は増えていない。`make` で `.warn` は空、`make test` は合格 (Passed 129 件)。4.1.14 は主エージェントのコミット項目のため要注意とした |
| 2026-11-27 06:08 | 第 2 段の開始と `porter` の書き換えの完了 | 並行案件 `261125-consumable-backlog-items` の main への統合と完了、`261123-porter-remaining-backlog-phase2` の各サブモジュールの main への統合と親リポジトリの参照の更新 (dbc28819) を受け、作業 WBS 4.2.1 を完了 (統合後の CI 45888369745 は実行中)。作業 WBS 4.2.2 で `porter` の 41 ファイルを書き換え、`--verify` に合格し、チェッカーのエラー 551 件を 0 件にした。`make` で `.warn` は空、`make test` は合格 (Passed 895 件)。4.2.3 は主エージェントのコミット項目のため要注意とした |
| 2026-11-27 08:36 | `cplat` の書き換えの完了 | 作業 WBS 4.2.4 を完了。`cplat` の 81 ファイルを書き換え、全 81 ファイルが `--verify` に合格し、チェッカーのエラー 1,485 件・警告 1 件を、BL-004 の 2 ファイル (`console.c` 5 件、`etw-viewer.c` 2 件の計 7 件) を除き 0 件にした。`check_c_blank_lines.py` の検出は 456 件のまま増えていない。`make` で `.warn` は空、`make test` は合格 (84 テスト ディレクトリで Passed 2,202 件、Failed 0 件)。4.2.5 は主エージェントのコミット項目のため要注意とした |
| 2026-11-27 12:37 | 第 1 段以降の変更確認と全対象チェッカー実行 | 作業 WBS 4.2.6 と 4.3 を実施。4.2.6 は第 1 段コミット後に変更された 9 ファイルで `check_c_topdown.py` のエラー 0 件・警告 0 件を確認し、書き換え不要 (コミット・push・CI の確認は主エージェントが行うため要注意)。4.3 は全対象 246 ファイルに実行し、BL-004 の 2 ファイル (`console.c` 5 件、`etw-viewer.c` 2 件) の計 7 件を除きエラー 0 件・警告 0 件、実行時間は約 1.67 [s] (BL-004 の残存のため要注意) |

## 反映と検証の記録

作業後の対応箇所、検証結果、未完了項目を記録します。

事前の実測 (2026-11-25、一時ディレクトリの走査スクリプト): 対象 244 ファイル、違反 154 ファイル、前方宣言のない `static` 関数 1,341 個。

### 第 1 段の規範とツール (作業 WBS 4.1.1 〜 4.1.4)

- 規範: `app/general/docs/coding-guideline.md` に `## 関数の記載順と前方宣言` 節 (適用範囲、用語、ファイル全体の構成、補助関数の記載順の 4 原則と違反としない事項、コメントの配置、条件付きコンパイル、望ましい例と望ましくない例、実施の規則、検証) を `## 処理を区切る空行` と `## 命名規則` の間に新設した。概要の記載一覧に追加し、`## 実装コメント` の「書かない内容」に、前方プロトタイプ宣言への実装コメントの重複を加えた。`text_style_jp.py --dry-run` の指摘なし。
- チェッカー: `app/general/bin/check_c_topdown.py`。字句と構文の解析処理を持ち、並べ替えツールが同じディレクトリのモジュールとして import する。最終仕様の 3 種別に加え、波括弧や丸括弧の対応がとれないファイルを見逃さないよう `parse-error` (error) を報告する (全対象で 0 件)。
- 並べ替えツール: `app/general/bin/reorder_c_topdown.py`。既存の `static` 関数のプロトタイプ宣言は文字列を変えずに定義の順へ並べ直し、ないものだけを生成する。`static` 関数と `static` でない関数の定義が混在する部分的な分岐は、同じ条件の 2 つの分岐 (前に `static` でない関数と関数定義以外の要素、後に `static` 関数) に分けて移す。すでに規則を満たすファイルは変更しない。
- 単体テスト: `test_check_c_topdown.py` 23 件、`test_reorder_c_topdown.py` 18 件。`make bin-test BIN_TEST_ARGS=app/general` で `app/general` の 99 件が全件合格 (2026-11-26、Linux (WSL))。
- 正式な件数 (`check_c_topdown.py`、2026-11-26、引数なしでワークスペースの `app/*/prod/` 全体を走査): 対象 244 ファイル、違反 154 ファイル、エラー 2,425 件、警告 1 件、`missing-prototype` 1,344 件。全件の走査の実行時間は約 1.4 [s]。

Table: app ごとの対象ファイル数と違反ファイル数 (`check_c_topdown.py` による数え直し)

| app | 対象ファイル数 | 違反ファイル数 | `missing-prototype` | エラー | 警告 |
| --- | ---: | ---: | ---: | ---: | ---: |
| `service-sample` | 5 | 5 | 57 | 94 | 0 |
| `string-catalog-sample` | 10 | 7 | 69 | 91 | 0 |
| `cjson` | 3 | 2 | 16 | 39 | 0 |
| `struct-meta` | 23 | 17 | 117 | 161 | 0 |
| `calc`、`doxygen-sample`、`override-sample`、`subfolder-sample`、`tutorial`、`zlib` | 30 | 0 | 0 | 0 | 0 |
| `porter` | 58 | 42 | 339 | 557 | 0 |
| `cplat` | 115 | 81 | 746 | 1,483 | 1 |
| **合計** | **244** | **154** | **1,344** | **2,425** | **1** |

`missing-prototype` の件数が事前の実測より 3 件多い (`service-sample`、`struct-meta`、`porter` で各 1 件) のは、事前の走査スクリプトとの関数定義の抽出の差による。違反ファイル数は一致した。

並べ替えツールの試行 (リポジトリの `.c` は書き換えず、メモリ上と一時ディレクトリの複製で実施):

- 違反 154 ファイル全件にメモリ上で適用し、150 ファイルが等価性の検証に合格し、結果のチェッカーのエラーと警告が 0 件、`check_c_blank_lines.py` の検出の増加なし、再実行で変化なしだった。第 1 段の対象 (`service-sample`、`string-catalog-sample`、`cjson`、`struct-meta`) はすべて含まれる。
- 残る 4 ファイルはすべて第 2 段の対象。`cplat` の `prompt_windows.c` (`static` 関数を参照する変数がマクロの定義より前にある) と `porter` の `tcpServer.c` (`main` を `#pragma warning(push)` と `pop` で囲む) は自動で並べ替えられないと判定され、手作業での書き換えになる。`cplat` の `console.c` と `etw-viewer.c` は、並べ替えても `non-topdown-order` が残る ([残課題一覧](261125-topdown-function-order-backlog.md) の BL-004)。
- `service-sample` の 5 ファイルを一時ディレクトリの Git リポジトリへ複製し、`reorder_c_topdown.py --in-place` で書き換えた。5 ファイルとも `--verify` に合格し、`check_c_topdown.py` のエラーと警告が 0 件、`check_c_blank_lines.py` の検出が書き換え前と同じだった。

### 第 1 段以降の変更確認と全対象チェッカー実行 (作業 WBS 4.2.6、4.3)

- 4.2.6 (第 1 段のコミット以降に変更された `.c` の確認、2026-11-27):
    - 対象 (`cplat` と `porter` 以外の app) で第 1 段のコミット後に変更された `.c` は 9 ファイル (`839387f4` による `service-sample.c`、`add.c`、`calc.c`、`shared-and-static-calc.c`、`override-sample.c`、`sample-app/main.c`、`hello.c`、`hello_with_args.c` と、`cjson` サブモジュールの `77c9c3a` / `57db801` による `cJSON_JSONC.c`)。`struct-meta` の `9376a220` は生成元の `struct_meta_parse.y` のみの変更であり、対象 `.c` の変更はなし。
    - 9 ファイルすべてに対して `check_c_topdown.py` を実行し、エラー 0 件・警告 0 件を確認した。書き換え対象がないため、`reorder_c_topdown.py` の再実行、`make`、`make test` は不要。
- 4.3 (全対象への `check_c_topdown.py` の実行と計測、2026-11-27、Linux (WSL)):
    - 引数なしで `python3 app/general/bin/check_c_topdown.py` を実行した。
    - 対象ファイル数: 246 ファイル (2026-11-25 時点の 244 ファイルから +2。`cplat` が 115 → 117 ファイルに増加。内訳: `calc` 8、`cjson` 3、`cplat` 117、`doxygen-sample` 7、`override-sample` 6、`porter` 58、`service-sample` 5、`string-catalog-sample` 10、`struct-meta` 23、`subfolder-sample` 6、`tutorial` 2、`zlib` 1)。
    - 結果: エラー 7 件、警告 0 件 (終了コード 1)。残ったエラーはすべて BL-004 の 2 ファイル (`app/cplat/prod/libsrc/cplat/console/console.c` の `non-topdown-order` 5 件と、`app/cplat/prod/src/cmd/etw-viewer/etw-viewer.c` の `non-topdown-order` 2 件) であり、それ以外のエラー・警告は 0 件。
    - 実行時間: 約 1.67 [s] (3 回計測: 1.659 [s]、1.677 [s]、1.672 [s]。初回ウォーム アップ時 1.764 [s])。
