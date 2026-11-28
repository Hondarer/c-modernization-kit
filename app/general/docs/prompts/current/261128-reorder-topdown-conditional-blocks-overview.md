<!--
生成元: agree-before-implement スキル
文書種別: 全体管理文書 (overview)
最終更新日時: 2026-11-28 08:38:00 JST
最終更新エージェント: agy (セッション ID: 34911232-f261-4dd8-bdce-4b9a89670d15)
稼働 OS: Linux (WSL)
上流文書: `261125-topdown-function-order-backlog.md` (対策元の残課題一覧: BL-006、関連 BL-004)。削除されている場合があります。
下流文書: `261128-reorder-topdown-conditional-blocks-discussion.md` (検討用 Markdown)、`261128-reorder-topdown-conditional-blocks-lightadr.md` (簡易 ADR)、`261128-reorder-topdown-conditional-blocks-design.md` (最終仕様)、`261128-reorder-topdown-conditional-blocks-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。この文書と、ユーザーが編集した最新の検討用 Markdown を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# reorder_c_topdown.py の条件付きコンパイル分岐の扱い修正 全体管理

ファイル名: `261128-reorder-topdown-conditional-blocks-overview.md`

この文書は、案件の識別、作業 WBS、対象範囲、確認状況、経緯を管理します。質問と回答の本文は検討用 Markdown (`261128-reorder-topdown-conditional-blocks-discussion.md`) に、決定事項は簡易 ADR (`261128-reorder-topdown-conditional-blocks-lightadr.md`) に記載し、この文書には重複して記載しません。

## 案件の識別

検討 ID: `261128-reorder-topdown-conditional-blocks`

検討用 Markdown の版: 1

状態: 検討中

## 作業 WBS と進捗管理

この案件の作業 WBS です。対策元の残課題一覧 (`261125-topdown-function-order-backlog.md`) の BL-006 (優先度 高) を主対象とし、関連する BL-004 (優先度 中) の同時対応要否を含めて前提条件を合意したうえで、最終仕様の作成、実装、検証、および成果物の承認へ進みます。  
小項目 (末端項目) 1 件ごとに、着手直前 (🔄 への更新と `← **CURRENT**` の付け替え) と完了直後 (✅ への更新と親項目の状態再判定) に都度この文書を更新します。決定事項は簡易 ADR (`261128-reorder-topdown-conditional-blocks-lightadr.md`) に記録し、見送り事項や範囲外の気付きは残課題一覧 (`261128-reorder-topdown-conditional-blocks-backlog.md`) に追記します。あわせて、対策元 `261125-topdown-function-order-backlog.md` の状態と経緯も進行に応じて随時更新します。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- 🔄 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 前提条件と要求事項の調査 (`app/general/bin/reorder_c_topdown.py`・`app/general/bin/check_c_topdown.py`・`app/general/bin_test/` の実装確認、一時ディレクトリでの最小再現と GCC 構文確認、現行 `app/*/prod/` 259 ファイルの混在分岐件数の実測、および対策元 `261125-topdown-function-order-backlog.md` の BL-006・BL-004 の状態更新)
    - ✅ 1.2. 課題・論点の抽出と明確化のための確認項目 ([Q-001](261128-reorder-topdown-conditional-blocks-discussion.md#q-001-混在する条件付きコンパイル分岐を検出したときの-reorder_c_topdownpy-の基本動作) 〜 [Q-007](261128-reorder-topdown-conditional-blocks-discussion.md#q-007-windows-側の検証方法と完了条件))、初期の簡易 ADR (`261128-reorder-topdown-conditional-blocks-lightadr.md`)、および残課題一覧 (`261128-reorder-topdown-conditional-blocks-backlog.md`) の作成
    - 🔄 1.3. Q&A ターンごとのユーザー回答の受付と記録 (ターン 1: Q-001 〜 Q-007 の回答待ち。JST 日時、Git ユーザー名または発言者を記録) ← **CURRENT**
    - ⏸️ 1.4. 回答に基づく前提・制約の検証、簡易 ADR (`261128-reorder-topdown-conditional-blocks-lightadr.md`) への決定事項の反映、WBS の見直し (対策元 `261125-topdown-function-order-backlog.md` への合意方針や見送り決定の随時反映)
- ⏸️ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ⏸️ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ⏸️ 2.2. 方針・前提条件の合意確定 (簡易 ADR の決定事項と、根拠となった解決済み Q の回答またはユーザーの明示的な指示との一致を確認し、懸念がないことをもってフェーズ 2 へ着手)
- ⏸️ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ⏸️ 3.1. `261128-reorder-topdown-conditional-blocks-design.md` の作成 (対策元 `261125-topdown-function-order-backlog.md` への確定方針・状態の反映)
    - ⏸️ 3.2. 最終仕様における受入条件と整合性の確認
    - ⏸️ 3.3. 対話セッションにおける最終仕様 (`261128-reorder-topdown-conditional-blocks-design.md`) の承認の受付 (第 2 ゲート: 作業着手の合意)
- ⏸️ 4. 承認された最終仕様に従った作業の実施 (具体的な更新対象は Q&A の合意と最終仕様で確定)
    - ⏸️ 4.1. `app/general/bin/reorder_c_topdown.py` の改修 (条件付きコンパイル分岐の混在検出・扱い、および `--verify` の順序検査)
    - ⏸️ 4.2. `app/general/bin/check_c_topdown.py` の改修 (BL-004 を本案件で同時対応する場合。分岐木構築ロジックの共有と複数分岐評価の対応)
    - ⏸️ 4.3. 単体テスト (`app/general/bin_test/test_reorder_c_topdown.py`、および BL-004 対応時は `app/general/bin_test/test_check_c_topdown.py`) の追加・更新
    - ⏸️ 4.4. 正本ドキュメント (`app/general/docs/coding-guideline.md`) およびコード コメントへの永続的知見の反映
    - ⏸️ 4.5. 局所テスト (`make bin-test BIN_TEST_ARGS=app/general`、全対象 `check_c_topdown.py` 実行)、一時ディレクトリでの再現ケース確認、および機械的検証 (`text_style_jp.py`、Windows 側検証) の実施
    - ⏸️ 4.6. 受入条件への適合確認 (全作業の完了確認)
    - ⏸️ 4.7. 簡易 ADR (`261128-reorder-topdown-conditional-blocks-lightadr.md`) の最終確認
    - ⏸️ 4.8. 残課題一覧 (`261128-reorder-topdown-conditional-blocks-backlog.md`) の再構築と整理、および対策元 `261125-topdown-function-order-backlog.md` の最終状態・経緯の確定
- ⏸️ 5. 成果物の承認の受付 (第 3 ゲート)
    - ⏸️ 5.1. 成果物・検証結果・残課題の提示
    - ⏸️ 5.2. 対話セッションにおける成果物の承認の受付 (「承認」または「完了」の受領によるスキル完了)

## 対象と参照資料

対象成果物 (予定・Q&A で最終確定):

- `app/general/bin/reorder_c_topdown.py`
- `app/general/bin_test/test_reorder_c_topdown.py`
- `app/general/bin/check_c_topdown.py` (BL-004 を同時対応する場合、[Q-004](261128-reorder-topdown-conditional-blocks-discussion.md#q-004-bl-004-を本案件で同時に扱うか) で確認)
- `app/general/bin_test/test_check_c_topdown.py` (BL-004 を同時対応する場合)
- `app/general/docs/coding-guideline.md` ([Q-005](261128-reorder-topdown-conditional-blocks-discussion.md#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い) で確認)

今回の対象範囲 (仮):

- `261125-topdown-function-order-backlog.md` の BL-006 (`reorder_c_topdown.py` が条件付きコンパイル分岐内の `#include`、マクロ、型定義、ファイル内変数を主たる関数の後ろへ移動する問題、および `--verify` による検出不足) の解消
- 関連する BL-004 (複数の分岐にそれぞれ完全な実装を持つ 2 ファイルと `check_c_topdown.py` の判定) の扱いは [Q-004](261128-reorder-topdown-conditional-blocks-discussion.md#q-004-bl-004-を本案件で同時に扱うか) で確定する

書き込み可能な検討文書 (フェーズ 1 の範囲):

- この文書 (`261128-reorder-topdown-conditional-blocks-overview.md`)
- 検討用 Markdown (`261128-reorder-topdown-conditional-blocks-discussion.md`)
- 簡易 ADR (`261128-reorder-topdown-conditional-blocks-lightadr.md`)
- 残課題一覧 (`261128-reorder-topdown-conditional-blocks-backlog.md`)
- 対策元の残課題一覧 (`261125-topdown-function-order-backlog.md` のメタ情報と BL-006・BL-004 の状態および継続作業での経緯と対処のみ)

参照資料と版:

- `AGENTS.md` (リポジトリ ルート、2026-11-28 時点のワーク ツリー)
- `app/general/AGENTS.md` (2026-11-28 時点のワーク ツリー)
- `.agents/skills/agree-before-implement/SKILL.md` (2026-11-24 更新版)
- `framework/docsfw/docs/japanese-technical-writing-guideline.md` (2026-11-28 時点のワーク ツリー)
- `app/general/docs/coding-guideline.md` (`## 関数の記載順と前方宣言` 節、441〜633 行目、2026-11-28 時点のワーク ツリー)
- `app/general/docs/prompts/current/261125-topdown-function-order-backlog.md` (BL-004、BL-006、2026-11-27 整理済み版)
- `app/general/docs/prompts/current/261125-topdown-function-order-design.md` (2026-11-25 改訂版)
- `app/general/bin/reorder_c_topdown.py` (全 613 行、2026-11-28 時点)
- `app/general/bin/check_c_topdown.py` (全 483 行、2026-11-28 時点)
- `app/general/bin_test/test_reorder_c_topdown.py` (全 566 行、18 テスト、2026-11-28 時点)
- `app/general/bin_test/test_check_c_topdown.py` (全 520 行、23 テスト、2026-11-28 時点)
- `.github/workflows/ci.yml` (`build-and-test-windows` ジョブの `make bin-test` 定義、604〜616 行目)

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 未解決 | 初期依頼 ([LADR-001](261128-reorder-topdown-conditional-blocks-lightadr.md#ladr-001-継続案件の立ち上げと前提合意による進行方針))、[Q-001](261128-reorder-topdown-conditional-blocks-discussion.md#q-001-混在する条件付きコンパイル分岐を検出したときの-reorder_c_topdownpy-の基本動作)、[Q-004](261128-reorder-topdown-conditional-blocks-discussion.md#q-004-bl-004-を本案件で同時に扱うか) |
| 範囲・境界 | 未解決 | [Q-002](261128-reorder-topdown-conditional-blocks-discussion.md#q-002-混在と判定する非関数要素と入れ子分岐の境界)、[Q-004](261128-reorder-topdown-conditional-blocks-discussion.md#q-004-bl-004-を本案件で同時に扱うか)、[Q-005](261128-reorder-topdown-conditional-blocks-discussion.md#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い) |
| 入力・根拠 | 確認済み | `reorder_c_topdown.py`、`check_c_topdown.py`、両単体テスト、`coding-guideline.md`、`261125-topdown-function-order-backlog.md`、一時ディレクトリでの最小再現結果、現行 `app/*/prod/` 259 ファイルの実測結果を確認済み |
| 整合性・依存関係 | 未解決 | [Q-003](261128-reorder-topdown-conditional-blocks-discussion.md#q-003---verify-への順序検査の追加と書き換え後コンパイル確認の役割分担)、[Q-004](261128-reorder-topdown-conditional-blocks-discussion.md#q-004-bl-004-を本案件で同時に扱うか)、[Q-005](261128-reorder-topdown-conditional-blocks-discussion.md#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い) |
| 実現性・例外 | 未解決 | [Q-001](261128-reorder-topdown-conditional-blocks-discussion.md#q-001-混在する条件付きコンパイル分岐を検出したときの-reorder_c_topdownpy-の基本動作)、[Q-002](261128-reorder-topdown-conditional-blocks-discussion.md#q-002-混在と判定する非関数要素と入れ子分岐の境界)、[Q-003](261128-reorder-topdown-conditional-blocks-discussion.md#q-003---verify-への順序検査の追加と書き換え後コンパイル確認の役割分担) |
| 受入条件・検証 | 未解決 | [Q-003](261128-reorder-topdown-conditional-blocks-discussion.md#q-003---verify-への順序検査の追加と書き換え後コンパイル確認の役割分担)、[Q-006](261128-reorder-topdown-conditional-blocks-discussion.md#q-006-単体テストの追加範囲とコンパイル確認の扱い)、[Q-007](261128-reorder-topdown-conditional-blocks-discussion.md#q-007-windows-側の検証方法と完了条件) |

## 質問一覧

各 Q の本文は検討用 Markdown にあります。Q-ID には検討用 Markdown の該当見出しへのリンクを設けます。

| Q-ID | 論点 | 状態 | 依存・関連 | 反映先の決定 |
| --- | --- | --- | --- | --- |
| [Q-001](261128-reorder-topdown-conditional-blocks-discussion.md#q-001-混在する条件付きコンパイル分岐を検出したときの-reorder_c_topdownpy-の基本動作) | 混在する条件付きコンパイル分岐を検出したときの `reorder_c_topdown.py` の基本動作 | 未回答 | [Q-002](261128-reorder-topdown-conditional-blocks-discussion.md#q-002-混在と判定する非関数要素と入れ子分岐の境界)、[Q-003](261128-reorder-topdown-conditional-blocks-discussion.md#q-003---verify-への順序検査の追加と書き換え後コンパイル確認の役割分担) | 未反映 |
| [Q-002](261128-reorder-topdown-conditional-blocks-discussion.md#q-002-混在と判定する非関数要素と入れ子分岐の境界) | 混在と判定する非関数要素と入れ子分岐の境界 | 未回答 | [Q-001](261128-reorder-topdown-conditional-blocks-discussion.md#q-001-混在する条件付きコンパイル分岐を検出したときの-reorder_c_topdownpy-の基本動作)、[Q-005](261128-reorder-topdown-conditional-blocks-discussion.md#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い) | 未反映 |
| [Q-003](261128-reorder-topdown-conditional-blocks-discussion.md#q-003---verify-への順序検査の追加と書き換え後コンパイル確認の役割分担) | `--verify` への順序検査の追加と書き換え後コンパイル確認の役割分担 | 未回答 | [Q-001](261128-reorder-topdown-conditional-blocks-discussion.md#q-001-混在する条件付きコンパイル分岐を検出したときの-reorder_c_topdownpy-の基本動作)、[Q-006](261128-reorder-topdown-conditional-blocks-discussion.md#q-006-単体テストの追加範囲とコンパイル確認の扱い) | 未反映 |
| [Q-004](261128-reorder-topdown-conditional-blocks-discussion.md#q-004-bl-004-を本案件で同時に扱うか) | BL-004 を本案件で同時に扱うか | 未回答 | [Q-005](261128-reorder-topdown-conditional-blocks-discussion.md#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い)、[Q-006](261128-reorder-topdown-conditional-blocks-discussion.md#q-006-単体テストの追加範囲とコンパイル確認の扱い)、[Q-007](261128-reorder-topdown-conditional-blocks-discussion.md#q-007-windows-側の検証方法と完了条件) | 未反映 |
| [Q-005](261128-reorder-topdown-conditional-blocks-discussion.md#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い) | コーディング規範の更新範囲と現行 `app/cplat` 残存 2 ファイルの扱い | 未回答 | [Q-002](261128-reorder-topdown-conditional-blocks-discussion.md#q-002-混在と判定する非関数要素と入れ子分岐の境界)、[Q-004](261128-reorder-topdown-conditional-blocks-discussion.md#q-004-bl-004-を本案件で同時に扱うか) | 未反映 |
| [Q-006](261128-reorder-topdown-conditional-blocks-discussion.md#q-006-単体テストの追加範囲とコンパイル確認の扱い) | 単体テストの追加範囲とコンパイル確認の扱い | 未回答 | [Q-003](261128-reorder-topdown-conditional-blocks-discussion.md#q-003---verify-への順序検査の追加と書き換え後コンパイル確認の役割分担)、[Q-004](261128-reorder-topdown-conditional-blocks-discussion.md#q-004-bl-004-を本案件で同時に扱うか) | 未反映 |
| [Q-007](261128-reorder-topdown-conditional-blocks-discussion.md#q-007-windows-側の検証方法と完了条件) | Windows 側の検証方法と完了条件 | 未回答 | [Q-004](261128-reorder-topdown-conditional-blocks-discussion.md#q-004-bl-004-を本案件で同時に扱うか)、[Q-005](261128-reorder-topdown-conditional-blocks-discussion.md#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い) | 未反映 |

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261128-reorder-topdown-conditional-blocks-design.md` (未作成)

検討用 Markdown の合意版 (第 1 ゲート): 未確定 (残懸念なし確認時に確定)

最終仕様 (design.md) の合意版 (第 2 ゲート): 未承認

更新予定の成果物: 未確定 (Q-001 〜 Q-007 の回答後に確定)

受入条件: 未確定 (Q-003、Q-004、Q-006、Q-007 の回答後に確定)

## 経緯

案件の進行上の出来事を、発生順に末尾へ追記します。個々の決定の内容は簡易 ADR に記載し、ここには出来事と参照先だけを記載します。

| 日時 (JST) | 出来事 | 内容・参照先 |
| --- | --- | --- |
| 2026-11-28 08:38 | 案件の開始 | `261125-topdown-function-order-backlog.md` の BL-006 (および関連 BL-004) の継続案件として事前調査と最小再現を実施し、全体管理文書、検討用 Markdown (版 1、Q-001 〜 Q-007)、初期の簡易 ADR ([LADR-001](261128-reorder-topdown-conditional-blocks-lightadr.md#ladr-001-継続案件の立ち上げと前提合意による進行方針))、残課題一覧 (`261128-reorder-topdown-conditional-blocks-backlog.md`) を作成。対策元 `261125-topdown-function-order-backlog.md` の BL-006・BL-004 を検討中へ更新し、WBS 1.3 (ユーザー回答の受付) で停止 |

## 反映と検証の記録

未着手。作業後の対応箇所、検証結果、未完了項目を記録します。
