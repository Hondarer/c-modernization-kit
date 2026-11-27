<!--
生成元: agree-before-implement スキル
文書種別: 全体管理文書 (overview)
最終更新日時: 2026-11-28 05:56:03 JST
最終更新エージェント: claude code / セッション ID: session_01KJc53PpM9v6GRymmPiAMGv
稼働 OS: Linux (WSL)
上流文書: `261127-process-orchestration-sample-backlog.md` (対策元の残課題一覧)。削除されている場合があります。
下流文書: `261128-process-orchestration-sample-review-fix-discussion.md` (検討用 Markdown)、`261128-process-orchestration-sample-review-fix-lightadr.md` (簡易 ADR)、`261128-process-orchestration-sample-review-fix-design.md` (最終仕様)、`261128-process-orchestration-sample-review-fix-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。この文書と、ユーザーが編集した最新の検討用 Markdown を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# process-orchestration-sample のレビュー指摘の対処 全体管理

ファイル名: `261128-process-orchestration-sample-review-fix-overview.md`

この文書は、案件の識別、作業 WBS、対象範囲、確認状況、経緯を管理します。質問と回答の本文は検討用 Markdown (`261128-process-orchestration-sample-review-fix-discussion.md`) に、決定事項は簡易 ADR (`261128-process-orchestration-sample-review-fix-lightadr.md`) に記載し、この文書には重複して記載しません。

## 案件の識別

検討 ID: `261128-process-orchestration-sample-review-fix`

検討用 Markdown の版: 1

状態: 検討中

## 作業 WBS と進捗管理

この案件の作業 WBS です。検討用 Markdown のための情報収集と質問の実施、残懸念がないことの確認 (第 1 ゲート: 最終仕様作成への着手)、最終決定事項 (最終仕様) を記載した `261128-process-orchestration-sample-review-fix-design.md` の作成、最終仕様の承認の受付 (第 2 ゲート)、承認された最終仕様に従った作業の実施、および作業完了後の成果物の承認の受付 (第 3 ゲート) に至る手順を記載します。  
具体的な作業対象は、Q&A での合意内容に応じて各ターンで見直します。各ゲートの通過・承認および作業の進行に従って、小項目 (末端項目) 1 件ごとに着手直前 (🔄 への更新と `← **CURRENT**` の付け替え) と完了直後 (✅ への更新と親項目の状態再判定) に都度この文書を更新します。決定事項は、Q を解決済みにした時点、またはユーザーが決定を明示した時点で `261128-process-orchestration-sample-review-fix-lightadr.md` へ反映します。見送りの決定や範囲外の気付きは、生じた時点で `261128-process-orchestration-sample-review-fix-backlog.md` に追記します。対策元の `261127-process-orchestration-sample-backlog.md` の状態と経緯も、進行の各段階で随時更新します。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- 🔄 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 前提条件と要求事項の調査 (対策元 `261127-process-orchestration-sample-backlog.md` の BL-003 〜 BL-019 の状態を「検討中」へ更新し継続案件の開始を記録、cplat の既存 API (`cplat_stat`、`cplat_mkdir`、`cplat_remove`、`cplat_strcpy`、`cplat_thread_timed_join`、`cplat_once_flag`) と結果コード、機能仕様の記載規範、CI の Windows のジョブの確認)
    - ✅ 1.2. 課題・論点の抽出と明確化のための確認項目 ([Q-001](261128-process-orchestration-sample-review-fix-discussion.md#q-001-今回の案件で対策する残課題の範囲) 〜 [Q-017](261128-process-orchestration-sample-review-fix-discussion.md#q-017-不具合ごとの再現テストの追加方針)) の作成、および初期決定事項の簡易 ADR ([LADR-001](261128-process-orchestration-sample-review-fix-lightadr.md#ladr-001-レビュー指摘の残課題を継続案件として対策する)) への記録
    - 🚫 1.3. ターン 1 (Q-001 〜 Q-017) のユーザー回答の受付と記録 ← **CURRENT** (停止理由: ユーザーの回答待ち。[Q-001](261128-process-orchestration-sample-review-fix-discussion.md#q-001-今回の案件で対策する残課題の範囲) 〜 [Q-017](261128-process-orchestration-sample-review-fix-discussion.md#q-017-不具合ごとの再現テストの追加方針))
    - ⏸️ 1.4. ターン 1 回答の検証、簡易 ADR への決定事項の反映、WBS の見直し (対策元 `backlog.md` へ合意方針と見送り決定を反映)
- ⏸️ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ⏸️ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ⏸️ 2.2. 方針・前提条件の合意確定 (簡易 ADR の決定事項と、根拠となった解決済み Q の回答またはユーザーの明示的な指示との一致を確認し、懸念がないことをもってフェーズ 2 へ着手)
- ⏸️ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ⏸️ 3.1. `261128-process-orchestration-sample-review-fix-design.md` の作成 (対策元 `backlog.md` の該当課題の状態を「対応中」へ更新)
    - ⏸️ 3.2. 最終仕様における受入条件と整合性の確認
    - ⏸️ 3.3. 対話セッションにおける最終仕様 (design.md) の承認の受付 (第 2 ゲート: 作業着手の合意)
- ⏸️ 4. 承認された最終仕様に従った作業の実施 (対象 app: `app/process-orchestration-sample`。小項目は最終仕様の確定時に課題単位へ詳細化する)
    - ⏸️ 4.1. Windows のビルドの修正 (BL-018、BL-019)
    - ⏸️ 4.2. 汎用 IPC 層の不具合と契約の修正 (BL-003、BL-004、BL-005、BL-008、BL-009、BL-010、BL-011、BL-012、BL-015 のうち対象と決めたもの)
    - ⏸️ 4.3. 親・子プロセス制御の修正 (BL-006、BL-017 のうち対象と決めたもの)
    - ⏸️ 4.4. 再現テストの追加 ([Q-017](261128-process-orchestration-sample-review-fix-discussion.md#q-017-不具合ごとの再現テストの追加方針) の決定に従う)
    - ⏸️ 4.5. リポジトリ規範との差の解消 (BL-013)
    - ⏸️ 4.6. 正本ドキュメント (公開ヘッダーの Doxygen、機能仕様、README) およびコード コメントへの永続的知見の反映
    - ⏸️ 4.7. Linux での局所ビルド、`.warn` 確認、`make test`、および機械的検証 (`check_c_topdown.py`、`check_c_blank_lines.py`、`clang-format`、`text_style_jp.py`)
    - ⏸️ 4.8. Windows での検証 (BL-007。[Q-013](261128-process-orchestration-sample-review-fix-discussion.md#q-013-windows-での検証の実施方法) の決定に従う)
    - ⏸️ 4.9. 受入条件への適合確認 (全作業の完了確認)
    - ⏸️ 4.10. 簡易 ADR (`261128-process-orchestration-sample-review-fix-lightadr.md`) の最終確認
    - ⏸️ 4.11. 残課題一覧 (`261128-process-orchestration-sample-review-fix-backlog.md`) の再構築と整理 (次フェーズとした課題の転記、および対策元 `261127-process-orchestration-sample-backlog.md` の最終状態・転記先の確定)
- ⏸️ 5. 成果物の承認の受付 (第 3 ゲート)
    - ⏸️ 5.1. 成果物・検証結果・残課題の提示
    - ⏸️ 5.2. 対話セッションにおける成果物の承認の受付 (「承認」または「完了」の受領によるスキル完了)

## 対象と参照資料

対象成果物: `app/process-orchestration-sample` のソース (`prod/`)、テスト (`test/src/`)、公開ヘッダーの Doxygen、機能仕様 (`docs/functional-spec/`)。BL-013 の対処に含める場合は `app/c-modernization-kit/docs/demo-app-composition.md`。

今回の対象範囲: 対策元の残課題一覧の BL-003 〜 BL-019 のうち、[Q-001](261128-process-orchestration-sample-review-fix-discussion.md#q-001-今回の案件で対策する残課題の範囲) で決める範囲。BL-001 と BL-002 は対象外 (元の案件で次フェーズとした課題)。

書き込み可能な検討文書: この文書、検討用 Markdown (`261128-process-orchestration-sample-review-fix-discussion.md`)、簡易 ADR (`261128-process-orchestration-sample-review-fix-lightadr.md`)、残課題一覧 (対策元 `261127-process-orchestration-sample-backlog.md` と、当該作業の `261128-process-orchestration-sample-review-fix-backlog.md`)

参照資料と版:

| 資料 | 版 | 用途 |
| --- | --- | --- |
| `261127-process-orchestration-sample-backlog.md` | 2026-11-27 23:47:24 JST 更新版 | 対策元の残課題 |
| `app/process-orchestration-sample/` | 2026-11-28 時点のワーク ツリー (Git 未追跡) | 対象のソースとテスト |
| `app/cplat/prod/include/cplat/` | 2026-11-28 時点のワーク ツリー | 利用する cplat の API と結果コード |
| ルートと app の `AGENTS.md` | 2026-11-28 時点のワーク ツリー | Git 操作の制約、両 OS 対応、Markdown の整形 |
| `app/general/docs/coding-guideline.md`、`doxygen-comment-guideline.md`、`functional-spec-guideline.md` | 2026-11-28 時点のワーク ツリー | 規範との差の確認 |
| `.github/workflows/ci.yml` | 2026-11-28 時点のワーク ツリー | Windows のジョブの内容 |

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | ユーザーの指示 (2026-11-28、「対処計画を進めて」)。成果物のレビューで追加した残課題の対策 ([LADR-001](261128-process-orchestration-sample-review-fix-lightadr.md#ladr-001-レビュー指摘の残課題を継続案件として対策する)) |
| 範囲・境界 | 未解決 | [Q-001](261128-process-orchestration-sample-review-fix-discussion.md#q-001-今回の案件で対策する残課題の範囲)、[Q-002](261128-process-orchestration-sample-review-fix-discussion.md#q-002-windows-のファイル操作の置き換え方法) (cplat を変更するかどうか)、[Q-015](261128-process-orchestration-sample-review-fix-discussion.md#q-015-要件-id-の参照コメントを付けるか) |
| 入力・根拠 | 確認済み | 対策元の残課題一覧と参照資料の表 |
| 整合性・依存関係 | 未解決 | [Q-003](261128-process-orchestration-sample-review-fix-discussion.md#q-003-持続接続で送受信に失敗した後の扱い)、[Q-004](261128-process-orchestration-sample-review-fix-discussion.md#q-004-破損した持続接続で返す結果コード)、[Q-005](261128-process-orchestration-sample-review-fix-discussion.md#q-005-要求と応答の照合方法)、[Q-006](261128-process-orchestration-sample-review-fix-discussion.md#q-006-タイムアウト指定の意味)、[Q-007](261128-process-orchestration-sample-review-fix-discussion.md#q-007-サーバー停止の期限を過ぎた場合の扱い)、[Q-009](261128-process-orchestration-sample-review-fix-discussion.md#q-009-子の正常終了の記録方法)、[Q-012](261128-process-orchestration-sample-review-fix-discussion.md#q-012-名前付きパイプに接続を許可するアカウント) (公開ヘッダーの契約との整合) |
| 実現性・例外 | 未解決 | [Q-008](261128-process-orchestration-sample-review-fix-discussion.md#q-008-起動中または停止中の子への操作の扱い)、[Q-010](261128-process-orchestration-sample-review-fix-discussion.md#q-010-同じ宛先のサーバーの二重起動を防ぐ方式)、[Q-011](261128-process-orchestration-sample-review-fix-discussion.md#q-011-代替配置先の検証に失敗した場合の扱い)、[Q-016](261128-process-orchestration-sample-review-fix-discussion.md#q-016-文字列コピーが収まらない場合の扱い) |
| 受入条件・検証 | 未解決 | [Q-013](261128-process-orchestration-sample-review-fix-discussion.md#q-013-windows-での検証の実施方法)、[Q-014](261128-process-orchestration-sample-review-fix-discussion.md#q-014-空行の規範の修正方法)、[Q-017](261128-process-orchestration-sample-review-fix-discussion.md#q-017-不具合ごとの再現テストの追加方針) |

## 質問一覧

各 Q の本文は検討用 Markdown にあります。Q-ID には検討用 Markdown の該当見出しへのリンクを設けます。

| Q-ID | 論点 | 状態 | 依存・関連 | 反映先の決定 |
| --- | --- | --- | --- | --- |
| [Q-001](261128-process-orchestration-sample-review-fix-discussion.md#q-001-今回の案件で対策する残課題の範囲) | 今回の案件で対策する残課題の範囲 | 未回答 | Q-013 | 未反映 |
| [Q-002](261128-process-orchestration-sample-review-fix-discussion.md#q-002-windows-のファイル操作の置き換え方法) | Windows のファイル操作の置き換え方法 (BL-018) | 未回答 | なし | 未反映 |
| [Q-003](261128-process-orchestration-sample-review-fix-discussion.md#q-003-持続接続で送受信に失敗した後の扱い) | 持続接続で送受信に失敗した後の扱い (BL-003) | 未回答 | Q-004、Q-005 | 未反映 |
| [Q-004](261128-process-orchestration-sample-review-fix-discussion.md#q-004-破損した持続接続で返す結果コード) | 破損した持続接続で返す結果コード (BL-003) | 未回答 | Q-003 | 未反映 |
| [Q-005](261128-process-orchestration-sample-review-fix-discussion.md#q-005-要求と応答の照合方法) | 要求と応答の照合方法 (BL-003) | 未回答 | Q-003 | 未反映 |
| [Q-006](261128-process-orchestration-sample-review-fix-discussion.md#q-006-タイムアウト指定の意味) | タイムアウト指定の意味 (BL-011) | 未回答 | なし | 未反映 |
| [Q-007](261128-process-orchestration-sample-review-fix-discussion.md#q-007-サーバー停止の期限を過ぎた場合の扱い) | サーバー停止の期限を過ぎた場合の扱い (BL-012) | 未回答 | なし | 未反映 |
| [Q-008](261128-process-orchestration-sample-review-fix-discussion.md#q-008-起動中または停止中の子への操作の扱い) | 起動中または停止中の子への操作の扱い (BL-006) | 未回答 | なし | 未反映 |
| [Q-009](261128-process-orchestration-sample-review-fix-discussion.md#q-009-子の正常終了の記録方法) | 子の正常終了の記録方法 (BL-017) | 未回答 | なし | 未反映 |
| [Q-010](261128-process-orchestration-sample-review-fix-discussion.md#q-010-同じ宛先のサーバーの二重起動を防ぐ方式) | 同じ宛先のサーバーの二重起動を防ぐ方式 (BL-010) | 未回答 | なし | 未反映 |
| [Q-011](261128-process-orchestration-sample-review-fix-discussion.md#q-011-代替配置先の検証に失敗した場合の扱い) | 代替配置先の検証に失敗した場合の扱い (BL-008) | 未回答 | なし | 未反映 |
| [Q-012](261128-process-orchestration-sample-review-fix-discussion.md#q-012-名前付きパイプに接続を許可するアカウント) | 名前付きパイプに接続を許可するアカウント (BL-009) | 未回答 | なし | 未反映 |
| [Q-013](261128-process-orchestration-sample-review-fix-discussion.md#q-013-windows-での検証の実施方法) | Windows での検証の実施方法 (BL-007) | 未回答 | Q-001 | 未反映 |
| [Q-014](261128-process-orchestration-sample-review-fix-discussion.md#q-014-空行の規範の修正方法) | 空行の規範の修正方法 (BL-013) | 未回答 | なし | 未反映 |
| [Q-015](261128-process-orchestration-sample-review-fix-discussion.md#q-015-要件-id-の参照コメントを付けるか) | 要件 ID の参照コメントを付けるか (BL-013) | 未回答 | なし | 未反映 |
| [Q-016](261128-process-orchestration-sample-review-fix-discussion.md#q-016-文字列コピーが収まらない場合の扱い) | 文字列コピーが収まらない場合の扱い (BL-019) | 未回答 | なし | 未反映 |
| [Q-017](261128-process-orchestration-sample-review-fix-discussion.md#q-017-不具合ごとの再現テストの追加方針) | 不具合ごとの再現テストの追加方針 | 未回答 | Q-001、Q-013 | 未反映 |

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261128-process-orchestration-sample-review-fix-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 未確定 (残懸念なし確認時に確定)

最終仕様 (design.md) の合意版 (第 2 ゲート): 未承認

更新予定の成果物: 未確定

受入条件: 未確定

## 経緯

| 日時 (JST) | 出来事 | 内容・参照先 |
| --- | --- | --- |
| 2026-11-28 05:56 | 案件の開始 | ユーザーの指示 (「対処計画を進めて」) により、全体管理文書、検討用 Markdown (版 1、Q-001 〜 Q-017)、簡易 ADR (LADR-001) を作成。対策元 `261127-process-orchestration-sample-backlog.md` の BL-003 〜 BL-019 を「検討中」に更新 |

## 反映と検証の記録

未着手。作業後の対応箇所、検証結果、未完了項目を記録します。
