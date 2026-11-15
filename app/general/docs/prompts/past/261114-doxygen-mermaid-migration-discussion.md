<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新エージェント: claude code / セッション ID: e1665a31-f98a-4cbe-aff5-beff19ab41c0
下流文書: `261114-doxygen-mermaid-migration-design.md` (最終仕様)、`261114-doxygen-mermaid-migration-backlog.md` (残課題一覧)。削除済みの場合があります。
再開手順: agree-before-implement スキルを読み、本書の最新版と作業 WBS の `← **CURRENT**` から再開してください。
-->

# Doxygen 由来の PlantUML 図を Mermaid へ移行する検討

Doxygen と Doxybook2 を経由して Markdown へ出力している PlantUML 図を Mermaid へ移行するため、対象範囲、互換方針、検証基準の合意を得ます。

検討 ID: `doxygen-mermaid-migration-20261114` / 版: 9 / 状態: 完了 (第 3 ゲートで成果物承認済み)

## 作業 WBS と進捗管理

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 前提収集と Q&A 実施
    - ✅ 1.1. 現行の図生成経路と正本文書の確認
    - ✅ 1.2. 論点抽出と質問 ([Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) 〜 [Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか)) の作成
    - ✅ 1.3. 未検証技術事項の実地調査と過去 27 ファイルの実態調査 (Doxygen 1.17.0 XML・`mmdc` なし・Doxybook2 `v1.6.1`/現行前後処理・`mmdc` 単体および構文別描画・27 ファイル突合を完了。`bin_test` 基準値と最終出力の描画比較は [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) (A)・[Q-005](#q-005-検証する-app-の範囲をどうするか) (A) に従い第 2 ゲート後に実施)
    - ✅ 1.4. ユーザー回答の受付と観点別確認 ([Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) 〜 [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) 全問回答受領・整合確認完了)
    - ✅ 1.5. 最終仕様書 (版 1) のレビューと差し戻し事項の記録 ([第 2 ゲート レビューによる訂正事項](#第-2-ゲート-レビューによる訂正事項-確認日-2026-11-14))
    - ✅ 1.6. 差し戻しに伴う追加確認 ([Q-011](#q-011-figure-行のキャプションに含まれる記号をどう扱うか) 〜 [Q-017](#q-017-継承グラフの矢印をどの記法で表すか)) の回答受付と試作検証
    - ✅ 1.7. 試作検証で生じた追加確認 ([Q-018](#q-018-キャプションの記号をどの手段でエスケープするか)、[Q-019](#q-019-手動コメントの図ブロックをどの桁へ字下げするか)) の回答受付
    - ✅ 1.8. 補足回答で生じた追加確認 ([Q-020](#q-020-図の規範の強化をどの範囲で行うか)) の回答受付と観点別確認
- ✅ 2. 第 1 ゲート: 残懸念の解消確認 (最終仕様作成へ。差し戻しにより再確認)
- ✅ 3. 最終仕様書の作成と承認
    - ✅ 3.1. 最終仕様書 ([261114-doxygen-mermaid-migration-design.md](261114-doxygen-mermaid-migration-design.md)) の版 2 への作り直しと受入条件の確定
    - ✅ 3.2. 第 2 ゲート: 最終仕様 (版 2) の承認受付 (承認者: Tetsuo Honda / 承認日時: 2026-11-14 20:11:15 JST)
- ✅ 4. 承認された仕様に基づく移行作業
    - ✅ 4.1. 変更前の基準値取得 (`bin_test` の Python 4 ファイル、`app/cplat` の `make doxy` 出力と警告ファイル (`doxy*.warn`) の退避)
    - ✅ 4.2. 変換パイプラインの更新 (`convert-mermaid-tags.py` の新設、`extract-graphs.py`、`preprocess.sh`、`postprocess.sh`、`makefile` のコメント)
    - ✅ 4.3. 図ブロックの字下げ検査の追加 (`check-doxygen-indent.py`)
    - ✅ 4.4. 手動コメントの更新 (`memory_lock.h`)
    - ✅ 4.5. 単体テストの更新・追加 (`test_preprocess.py`、`test_postprocess.py`、`test_extract_graphs.py`、`test_check_doxygen_indent.py`)
    - ✅ 4.6. 正本文書の更新 (`framework/doxyfw/README.md` と `framework/doxyfw/docs/` の 8 ファイル)
    - ✅ 4.7. 既存 Markdown 3 箇所の修正 (`cplat-account-session-discussion.md`、`architecture.md`、`sequence-data.md`)
    - ✅ 4.8. 局所検証 (単体テスト 6 ファイル、`app/cplat` の再生成比較と警告確認、代表図の描画と意味一致の確認、`text_style_jp.py`)
    - ✅ 4.9. 受入条件の適合確認と残課題の整理
- ✅ 5. 第 3 ゲート: 成果物・検証結果の承認受付

## 目的

リポジトリ内の既定 Markdown 文書では Mermaid への統一を進めており、Doxygen 経由で生成される Markdown に残る PlantUML 図も Mermaid へ移行して図記法と描画基盤を一本化します。  
本検討では、移行前に技術調査と方針の合意を済ませ、影響範囲と検証条件を明確にします。

## 現状の仕組み

Doxygen から Markdown へ出力される PlantUML 図には、生成元の異なる 2 系統があります。  
第 1 に、[memory_lock.h](../../../../cplat/prod/include/cplat/runtime/memory_lock.h) の関数コメント内に `@startuml` / `@enduml` で書かれた手動コメント 2 図があります。  
第 2 に、[extract-graphs.py](../../../../../framework/doxyfw/templates/extract-graphs.py) が Doxygen XML の依存関係や呼び出し関係を解析し、`<plantuml>` タグとして挿入する自動生成グラフがあります。

どちらも [preprocess.sh](../../../../../framework/doxyfw/templates/preprocess.sh) で ```` ```plantuml ```` フェンスへ変換され、[postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) が PlantUML 用の下線回避 (`__` の `~_~_` 置換) を施して Markdown へ出力します。  
なお、図のキャプションについては [codeblock-caption.md](../../../../../framework/docsfw/docs/codeblock-caption.md) の現行規範が `Figure:` を推奨し、`CodeBlock:` を移行互換の非推奨としています (付録の過去報告では `CodeBlock:` 適用と記されています)。

## 移行対象候補

| 対象ファイル | 現在の役割 |
| --- | --- |
| [memory_lock.h](../../../../cplat/prod/include/cplat/runtime/memory_lock.h) | 手動 `@startuml` コメント 2 図の記述元 |
| [Doxyfile](../../../../../framework/doxyfw/Doxyfile) | `PLANTUML_JAR_PATH` 等の定義元 (`MERMAID_*` 設定なし) |
| [extract-graphs.py](../../../../../framework/doxyfw/templates/extract-graphs.py) | 自動生成グラフの `<plantuml>` タグ挿入 |
| [preprocess.sh](../../../../../framework/doxyfw/templates/preprocess.sh) | `<plantuml>` タグの ```` ```plantuml ```` フェンス変換 |
| [postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) | ```` ```plantuml ```` 内の `!dunder!` / `__` エスケープ、`fix_ptr`、`<!--details:-->` 終端処理 |
| [input-filter.py](../../../../../framework/doxyfw/bin_internal/input-filter.py) | Doxygen 入力時の Markdown 内 ```` ```plantuml ```` ブロック除去 |
| [test_preprocess.py](../../../../../framework/doxyfw/bin_test/test_preprocess.py)、[test_postprocess.py](../../../../../framework/doxyfw/bin_test/test_postprocess.py)、[test_doxybook2_templates.py](../../../../../framework/doxyfw/bin_test/test_doxybook2_templates.py)、[test_materialize_group_members.py](../../../../../framework/doxyfw/bin_test/test_materialize_group_members.py) | 前処理・後処理・テンプレート・グループ実体化の単体テスト |
| [maintenance-verification.md](../../../../../framework/doxyfw/docs/maintenance-verification.md)、[commands.md](../../../../../framework/doxyfw/docs/commands.md)、[cheatsheet.md](../../../../../framework/doxyfw/docs/cheatsheet.md)、[extract-graphs.md](../../../../../framework/doxyfw/docs/extract-graphs.md)、[doxybook2-plantuml.md](../../../../../framework/doxyfw/docs/doxybook2-plantuml.md) | 図記法方針、コマンド解説、生成例などの正本文書 |

Table: 移行対象候補と現在の役割

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) | 移行対象 2 系統の範囲 | 回答済み (解決) | なし |
| [Q-002](#q-002-既存の-plantuml-変換経路を残すか) | 既存 PlantUML 経路の互換維持 | 回答済み (解決) | [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) |
| [Q-003](#q-003-今後の図記法方針をどう改訂するか) | 正本文書の推奨図記法 | 回答済み (解決) | [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか)、[Q-002](#q-002-既存の-plantuml-変換経路を残すか)、[Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか) |
| [Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) | 過去 27 ファイルの再検証要否 | 回答済み (解決) | [Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか)、[Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか) |
| [Q-005](#q-005-検証する-app-の範囲をどうするか) | 検証する `app` の範囲 | 回答済み (解決) | [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか)、[Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) |
| [Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか) | 描画と意味一致の受入基準 | 回答済み (解決) | [Q-005](#q-005-検証する-app-の範囲をどうするか) |
| [Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか) | 残存 `CodeBlock:` 1 箇所の扱い | 回答済み (解決) | [Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) |
| [Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか) | キャプションなし Mermaid 図 2 箇所の扱い | 回答済み (解決) | [Q-003](#q-003-今後の図記法方針をどう改訂するか)、[Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) |
| [Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか) | Doxygen 由来 Mermaid 図の `Figure:` 出力方針 | 回答済み (解決) | [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか)、[Q-003](#q-003-今後の図記法方針をどう改訂するか) |
| [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) | `doxyfw` 単体テストの実行範囲と基準値確認 | 回答済み (解決) | [Q-005](#q-005-検証する-app-の範囲をどうするか)、[Q-016](#q-016-自動生成グラフの単体テストをどのファイルへ置くか) |
| [Q-011](#q-011-figure-行のキャプションに含まれる記号をどう扱うか) | `Figure:` 行のキャプションに含まれる記号の扱い | 回答済み (解決) | [Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか)、[Q-018](#q-018-キャプションの記号をどの手段でエスケープするか) |
| [Q-012](#q-012-mermaid-タグ変換ヘルパーをどの位置で呼び出すか) | `<mermaid>` 変換ヘルパーの呼び出し位置 | 回答済み (解決) | [Q-002](#q-002-既存の-plantuml-変換経路を残すか) |
| [Q-013](#q-013-mermaid-変換経路の仕様をどの正本文書へ記載するか) | Mermaid 変換経路を記載する正本文書 | 回答済み (解決) | [Q-003](#q-003-今後の図記法方針をどう改訂するか) |
| [Q-014](#q-014-doxyfw-の-readme-の必要なツールを更新するか) | `framework/doxyfw/README.md` の必要なツールの更新 | 回答済み (解決) | [Q-002](#q-002-既存の-plantuml-変換経路を残すか)、[Q-003](#q-003-今後の図記法方針をどう改訂するか) |
| [Q-015](#q-015-doxygen-html-の-mermaid-描画方式をどうするか) | Doxygen HTML の Mermaid 描画方式 | 回答済み (解決) | [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) |
| [Q-016](#q-016-自動生成グラフの単体テストをどのファイルへ置くか) | 自動生成グラフの単体テストの配置先 | 回答済み (解決) | [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) |
| [Q-017](#q-017-継承グラフの矢印をどの記法で表すか) | 継承グラフの矢印の記法 | 回答済み (解決) | [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか)、[Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか) |
| [Q-018](#q-018-キャプションの記号をどの手段でエスケープするか) | キャプションの記号をエスケープする手段 | 回答済み (解決) | [Q-011](#q-011-figure-行のキャプションに含まれる記号をどう扱うか) |
| [Q-019](#q-019-手動コメントの図ブロックをどの桁へ字下げするか) | 手動コメントの図ブロックの字下げ位置 | 回答済み (解決) | [Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか)、[Q-020](#q-020-図の規範の強化をどの範囲で行うか) |
| [Q-020](#q-020-図の規範の強化をどの範囲で行うか) | 図の規範を強化する範囲 | 回答済み (解決) | [Q-003](#q-003-今後の図記法方針をどう改訂するか)、[Q-019](#q-019-手動コメントの図ブロックをどの桁へ字下げするか) |

Table: 明確化のための確認項目一覧

## 明確化のための確認項目

### Q-001 手動コメントと自動生成グラフの双方を対象とするか

**Status**: 回答済み (解決)

**Question**: 移行対象に、手動コメント 2 図と自動生成グラフの双方を含めますか。

**Context**: 手動コメントだけを移行しても、`extract-graphs.py` が `<plantuml>` タグを生成し続けるため出力に PlantUML が残ります。双方を移行する場合は共通パイプラインを変更するため Doxygen を利用する全 `app` に影響します。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 14:18:31 JST  
選択方式: 単一選択

- [x] A: (推奨) 手動コメント 2 図と自動生成グラフの双方を対象とする。
- [ ] B: 手動コメント 2 図だけを対象とし、自動生成グラフは PlantUML のまま維持する。
- [ ] C: 自動生成グラフだけを対象とし、手動コメント 2 図は据え置く。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — どちらか一方を残すと生成 Markdown 内に PlantUML と Mermaid が混在し、描画基盤を統一できないためです。

**Assessment**: 推奨 A の明確な単一選択を確認しました。手動コメント ([memory_lock.h](../../../../cplat/prod/include/cplat/runtime/memory_lock.h) 内の 2 図) と自動生成グラフ ([extract-graphs.py](../../../../../framework/doxyfw/templates/extract-graphs.py)) の双方を Mermaid へ移行する前提と、全 `app` の生成経路への影響が明確に合意されています。

### Q-002 既存の PlantUML 変換経路を残すか

**Status**: 回答済み (解決)

**Question**: 移行後の `framework/doxyfw` に、既存の PlantUML 変換経路 (`@startuml` / `<plantuml>` / ```` ```plantuml ````) を互換維持として残しますか。

**Context**: 既存経路を残せば未移行コメントや外部由来コードの `@startuml` が壊れません。廃止して Mermaid 専用にすると処理は簡素になりますが、`@startuml` を含む既存コメントで警告や出力崩れが起きます。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 14:18:31 JST  
選択方式: 単一選択

- [x] A: (推奨) 既存の PlantUML 変換経路を互換維持し、Mermaid 対応と共存させる。
- [ ] B: 既存の PlantUML 変換経路を廃止し、Mermaid 専用へ切り替える。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 両経路は互いに競合せず、既存コメントや外部コードへの後方互換を保てるためです。B を選ぶ場合は全 `app` に `@startuml` が残らない確認が必要です。

**Assessment**: 推奨 A の明確な単一選択を確認しました。[Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) (A) および [Q-003](#q-003-今後の図記法方針をどう改訂するか) (A) と整合しており、既存 `plantuml` 経路 (`<plantuml>`、`@startuml`、`postprocess.sh` の `~_~_` 退避、`input-filter.py` の ```` ```plantuml ```` 除去) を維持したまま Mermaid 経路を追加・共存させます。

**Related**: [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか)、[Q-003](#q-003-今後の図記法方針をどう改訂するか)

### Q-003 今後の図記法方針をどう改訂するか

**Status**: 回答済み (解決)

**Question**: `framework/doxyfw/docs/` の正本文書に記載されている図の推奨記法 (現行は PlantUML 基本) をどう改訂しますか。

**Context**: 現行の [maintenance-verification.md](../../../../../framework/doxyfw/docs/maintenance-verification.md) は「PlantUML を基本とし、表現しにくい場合に Mermaid を利用」と定めています。また、図のキャプションは [codeblock-caption.md](../../../../../framework/docsfw/docs/codeblock-caption.md) で `Figure:` が推奨されています。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 14:18:31 JST  
選択方式: 単一選択

- [x] A: (推奨) 新規・標準記法を Mermaid (`Figure:` キャプション推奨) へ改訂し、PlantUML は互換・補足として併記する。
- [ ] B: 実装例の更新だけにとどめ、PlantUML を基本とする現行方針を維持する (移行意図との乖離を許容する理由の明記が必要)。
- [ ] C: PlantUML の記述を正本文書から全廃する ([Q-002](#q-002-既存の-plantuml-変換経路を残すか) で互換廃止を選んだ場合のみ選択可)。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 実装を Mermaid へ移した後に正本が PlantUML 基本のままだと、新規記述と実装方針が乖離するためです。

**Assessment**: 推奨 A の選択を確認しました。[Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) (A)・[Q-002](#q-002-既存の-plantuml-変換経路を残すか) (A) と整合しています。なお、Doxygen 由来の生成 Markdown に `Figure:` 行を出力する場合、[postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) の `<!--details:-->` 終端位置や `@mermaid` のキャプション指定形式と干渉するため、細部を [Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか) で確認します。

**Related**: [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか)、[Q-002](#q-002-既存の-plantuml-変換経路を残すか)、[Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか)

### Q-004 過去 27 ファイルの完了報告を今回再検証するか

**Status**: 回答済み (解決)

**Question**: 付録にある過去の手書き Markdown 27 ファイルの完了報告 (総数 57 対内訳 68 の不一致および `CodeBlock:` 記載) を今回の作業で再検証しますか。

**Context**: 前版の記録には 27 ファイルで完了とありますが、見出しの 57 箇所と各ファイル件数の合計 68 箇所が一致せず、現行非推奨の `CodeBlock:` 適用と記されています。本再生成では実ファイルとの突合を行っていません。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 14:18:31 JST  
選択方式: 単一選択

- [ ] A: (推奨) 歴史記録として保持し、今回の作業範囲外とする (不一致と未再検証の事実は記録に残す)。
- [x] B: 27 ファイルの件数精査・実態調査と `Figure:` への修正確認まで今回の作業に含める。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 今回の目的は Doxygen 由来図の移行であり、27 ファイルの再点検を混ぜると全リポジトリ規模へ作業が肥大化するためです。

**Assessment**: B の選択を確認しました。この選択に基づき 27 ファイルの実態を read-only で全数調査した結果、対象 27 ファイルはすべて実在し、Mermaid フェンス合計 70 箇所、PlantUML フェンス合計 4 箇所 (`extract-graphs.md` の生成例)、`Figure:` キャプション 67 箇所、`CodeBlock:` キャプション 1 箇所、キャプションなし Mermaid 図 2 箇所であることが判明しました。この実態から生じた個別判断 2 点 (残存する `CodeBlock:` 1 箇所の扱いと、キャプションなし 2 箇所への `Figure:` 付与要否) を [Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか)・[Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか) で確認します。なお、`extract-graphs.md` の 4 箇所の PlantUML 生成例は [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) (A) および [Q-003](#q-003-今後の図記法方針をどう改訂するか) (A) により本件の正本更新対象に含まれます。

**Related**: [Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか)、[Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか)

### Q-005 検証する app の範囲をどうするか

**Status**: 回答済み (解決)

**Question**: 移行後の再生成比較と警告確認を行う `app` の範囲をどうしますか。

**Context**: [maintenance-verification.md](../../../../../framework/doxyfw/docs/maintenance-verification.md) は単体テストに加え生成前後の出力比較と警告確認を求めています。一方、ルート `AGENTS.md` により複数 `app` や全体テストは対象とコストを提示したうえで事前確認が必要です。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 14:18:31 JST  
選択方式: 単一選択

- [x] A: (推奨) 単体テストに加え、代表の単一 `app` (`app/cplat`) で `make doxy` の前後比較と警告確認を行う。
- [ ] B: 技術調査後に対象 `app` とコストを提示し、実施確認を得たうえで複数 `app` の比較を行う。
- [ ] C: 単体テストのみとし、`app` での比較は行わない (`doxyfw` 検証方針から外れる理由の明記が必要)。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — `app/cplat` には手動コメント 2 図と自動生成グラフの双方が含まれるため、最小コストで両系統を確認できるためです。

**Assessment**: 推奨 A の選択を確認しました。[Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) (A) の両系統を `app/cplat` の単一 `app` 範囲で確認する方針として整合しています。一方で、`framework/doxyfw/bin_test/` には Python 10 ファイル・JavaScript 13 ファイルが存在し、ルート `AGENTS.md` により `framework` 全体テストの無許可実行が制限されているため、実行対象の局所単体テスト範囲と基準値確認の要否を [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) で確認します。

**Related**: [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか)、[Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか)、[Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか)

### Q-006 描画エラーなしと意味一致を受入基準に含めるか

**Status**: 回答済み (解決)

**Question**: Markdown 差分や警告確認に加えて、変換後の Mermaid 図がエラーなく描画され元の図と関係・意味が一致することを受入基準に含めますか。

**Context**: フェンス構文や特殊文字 (`__`、記号、改行) の変換ミスは Markdown テキストの目視や `diff` だけでは見落としやすく、レンダリング時に初めて描画崩れとして現れます。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 14:18:31 JST  
選択方式: 単一選択

- [x] A: (推奨) 代表図についてエラーなく描画され、元の PlantUML 図と関係・意味が一致することを受入基準に含める。
- [ ] B: Markdown のテキスト確認、警告ゼロ、単体テスト合格までを受入基準とし、描画比較は行わない。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 代表的な図種別に絞って描画と関係の意味一致を確認すれば、過度な負荷をかけずに描画品質を担保できるためです。

**Assessment**: 推奨 A の選択を確認しました。[Q-005](#q-005-検証する-app-の範囲をどうするか) (A) と整合しており、ローカル環境の `mmdc` (`11.17.0`) で最小 Mermaid 図の SVG 描画が成功することも実地確認済みです。実装完了後の代表図には、手動コメント 2 図 (`cplat_memory_lock_self`、`cplat_memory_lock_scope_release`) と自動生成グラフ種別 (コールグラフ、呼び出し元グラフ、インクルード依存グラフ、コラボレーション図) の代表例を含めます。

**Related**: [Q-005](#q-005-検証する-app-の範囲をどうするか)

### Q-007 残存する CodeBlock 記載 1 箇所を Figure へ修正するか

**Status**: 回答済み (解決)

**Question**: 27 ファイルの実態調査で唯一 `CodeBlock:` が残っていた [cplat-account-session-discussion.md](../../../../cplat/docs/prompts/current/cplat-account-session-discussion.md) (L295) の Mermaid 図キャプションを、`Figure:` へ修正しますか。

**Context**: [Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) (B) に基づく調査の結果、27 ファイルのうち 25 ファイルはすでに `Figure:` へ統一されており、`CodeBlock:` が残っていたのは `docs/prompts/current/` 配下の検討用文書 [cplat-account-session-discussion.md](../../../../cplat/docs/prompts/current/cplat-account-session-discussion.md) L295 (`CodeBlock: セッション上でプロセスを起動する処理の流れ (試案)`) の 1 箇所だけでした。なお、同ファイル内の C コード ブロック 5 箇所には `{.c caption="..."}` が使われています (Mermaid 図ではありません)。

**Answer**

回答者: ユーザー / 回答日: 2026-11-14  
選択方式: 単一選択

- [ ] A: (推奨) `docs/prompts/current/` 配下の他案件検討文書であるため本文は書き換えず、残存 1 箇所の調査事実のみ記録して対象外とする。
- [x] B: L295 の `CodeBlock:` を `Figure:` へ修正し、同ファイル内の C コード ブロック 5 箇所 (`{.c caption="..."}`) はそのまま維持する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 検討用 (`prompts/current/`) の文書は他タスクの検討履歴そのものであり、図の移行作業と無関係に過去検討文書を改変しない方が安全なためです。ただし、`CodeBlock:` の残存をゼロにする意図で B を選ぶ場合も、修正箇所は 1 行だけに限定できます。

**Assessment**: B の選択を確認しました。[Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) (B) と整合しており、修正範囲を [cplat-account-session-discussion.md](../../../../cplat/docs/prompts/current/cplat-account-session-discussion.md) L295 の `CodeBlock:` から `Figure:` への 1 行変更だけに限定し、同ファイル内の `{.c caption="..."}` 5 箇所は変更しない方針で最終仕様へ反映します。

**Related**: [Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか)

### Q-008 キャプションなしの Mermaid 図 2 箇所へ Figure を付与するか

**Status**: 回答済み (解決)

**Question**: 27 ファイルの実態調査でキャプション行を持たないまま存在していた Mermaid 図 2 箇所 ([architecture.md](../../../../porter/docs/architecture.md) L28-36 および [sequence-data.md](../../../../porter/docs/sequence-data.md) L408-421) に、`Figure:` キャプションを付与しますか。

**Context**: [Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) (B) に基づく調査の結果、[architecture.md](../../../../porter/docs/architecture.md) は付録記載 6 箇所に対し実在 Mermaid 7 箇所 (`Figure:` 6 箇所 + キャプションなし 1 箇所)、[sequence-data.md](../../../../porter/docs/sequence-data.md) は付録記載 9 箇所に対し実在 Mermaid 10 箇所 (`Figure:` 9 箇所 + キャプションなし 1 箇所) でした。

**Answer**

回答者: ユーザー / 回答日: 2026-11-14  
選択方式: 単一選択

- [x] A: (推奨) 現行規範 ([Q-003](#q-003-今後の図記法方針をどう改訂するか) A の `Figure:` 推奨) に合わせ、これら 2 箇所にも内容に沿った `Figure:` キャプションを付与する。
- [ ] B: これら 2 箇所はキャプションなしのままとし、付録側の件数実態 (`7 箇所` / `10 箇所`) の訂正だけを記録する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 同一ファイル内の他の図がすべて `Figure:` を持っているため、残る 2 箇所にも `Figure:` を揃えると表記と図番号の一貫性が保てるためです。

**Assessment**: 推奨 A の選択を確認しました。[Q-003](#q-003-今後の図記法方針をどう改訂するか) (A) および [Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) (B) と整合しており、[architecture.md](../../../../porter/docs/architecture.md) L28-36 と [sequence-data.md](../../../../porter/docs/sequence-data.md) L408-421 に内容に沿った `Figure:` キャプションを付与する計画を最終仕様へ組み込みます。

**Related**: [Q-003](#q-003-今後の図記法方針をどう改訂するか)、[Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか)

### Q-009 Doxygen 由来の Mermaid 図へ Figure キャプションを出力するか

**Status**: 回答済み (解決)

**Question**: Doxygen 経由で出力する Mermaid 図 (手動 `@mermaid` コメントおよび [extract-graphs.py](../../../../../framework/doxyfw/templates/extract-graphs.py) の自動生成グラフ) の Markdown 出力に、`Figure:` キャプション行を出力しますか。

**Context**: 現行の自動生成グラフは `<simplesect kind="par"><title>DOXYFW_DETAILS_ONLY: ...</title>` により小見出し/折り畳み見出しを持ち、PlantUML 内に `caption ...` を出力しています。一方、[postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) L409 は閉じコード フェンス ```` ``` ```` の直後で `<!--:details-->` を閉じるため、自動生成グラフへ `Figure:` 行を出力すると折り畳み構造の調整が必要です。また、手動コメント ([memory_lock.h](../../../../cplat/prod/include/cplat/runtime/memory_lock.h) L130, L175) は `@startuml <title>` と図内 `caption` を併用していますが、Doxygen 1.17.0 では `@mermaid{caption="..."}` により `<mermaid caption="{caption=&quot;...&quot;}">` が出力されます。

**Answer**

回答者: ユーザー / 回答日: 2026-11-14  
選択方式: 単一選択

- [x] A: (推奨) 手動コメント (`@mermaid` でキャプション指定時) と自動生成グラフの双方で ```` ```mermaid ```` 直後に `Figure:` 行を出力し、[postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) が `Figure:` 行を含めて `<!--:details-->` を閉じるよう調整する。
- [ ] B: 自動生成グラフは小見出し・折り畳みタイトルで代替するため `Figure:` 行を出力せず、手動コメントでキャプション指定がある場合だけ ```` ```mermaid ```` 直後に `Figure:` 行を出力する。
- [ ] C: Doxygen 由来の図では見出しや折り畳みタイトル、または図内 `title` のみを用い、`Figure:` 行は生成しない。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 手動図と自動生成グラフの双方で `docsfw` の `Figure:` 規約 ([codeblock-caption.md](../../../../../framework/docsfw/docs/codeblock-caption.md)) と統一でき、折り畳み表示内でのキャプション欠落も防げるためです。

**Assessment**: 推奨 A の選択を確認しました。[Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) (A) および [Q-003](#q-003-今後の図記法方針をどう改訂するか) (A) と整合しており、手動コメント (`@mermaid{caption="..."}`) と [extract-graphs.py](../../../../../framework/doxyfw/templates/extract-graphs.py) の自動生成グラフ双方で ```` ```mermaid ```` 直後に `Figure:` 行を出力し、[postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) が `Figure:` 行を含めて `<!--:details-->` を閉じるよう設計します。

**Related**: [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか)、[Q-003](#q-003-今後の図記法方針をどう改訂するか)

### Q-010 doxyfw 単体テストの実行範囲と基準値確認をどう扱うか

**Status**: 回答済み (解決)

**Question**: 技術調査および実装検証で実行する [framework/doxyfw/bin_test/](../../../../../framework/doxyfw/bin_test) のテスト範囲と、事前基準値確認の要否をどうしますか。

**Context**: ルート `AGENTS.md` により、`framework` 全体テストはコストを説明したうえで事前確認が必要です。[framework/doxyfw/bin_test/](../../../../../framework/doxyfw/bin_test) には Python 10 ファイルと JavaScript 13 ファイル (うち `resolve-puppeteer.js` + 12 本のブラウザー/UI 検証スクリプト) の計 23 ファイルがあります。一方、本件の XML・前後処理・テンプレート変更に直接関係する局所単体テストは Python 4 ファイル ([test_preprocess.py](../../../../../framework/doxyfw/bin_test/test_preprocess.py)、[test_postprocess.py](../../../../../framework/doxyfw/bin_test/test_postprocess.py)、[test_doxybook2_templates.py](../../../../../framework/doxyfw/bin_test/test_doxybook2_templates.py)、[test_materialize_group_members.py](../../../../../framework/doxyfw/bin_test/test_materialize_group_members.py)) です。依存関係レポートやパス空白検査、ブラウザー probe 群は本件の変更範囲外です。

**Answer**

回答者: ユーザー / 回答日: 2026-11-14  
選択方式: 単一選択

- [x] A: (推奨) 実装前 (第 2 ゲート後の作業時) および実装後の双方で、直接関連する局所単体テスト Python 4 ファイル (`test_preprocess.py`、`test_postprocess.py`、`test_doxybook2_templates.py`、`test_materialize_group_members.py`) のみを実行する。
- [ ] B: Python 10 ファイル全件 (依存関係レポート等を含む) の実行を許可し、JavaScript ブラウザー probe 13 ファイルは除外する。
- [ ] C: Python 10 ファイル + JavaScript 13 ファイルの `framework/doxyfw/bin_test/` 全件実行を許可する (Puppeteer / Chrome 起動を含むため実行時間・負荷が大きい)。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 変更対象 (`preprocess.sh`、`postprocess.sh`、テンプレート、`extract-graphs.py` 周辺) を網羅しつつ、無関係な大規模テストや Puppeteer 群のコストを回避できるためです。

**Assessment**: 推奨 A の選択を確認しました。[Q-005](#q-005-検証する-app-の範囲をどうするか) (A) と整合しており、第 2 ゲート承認後の実装フェーズ冒頭で Python 4 ファイルの事前基準値確認を行い、変更完了後に同 4 ファイルと新規・更新テストを再実行する手順として最終仕様へ組み込みます (JavaScript 13 ファイルおよび残り Python 6 ファイルは本件スコープ外)。

**Related**: [Q-005](#q-005-検証する-app-の範囲をどうするか)

### Q-011 Figure 行のキャプションに含まれる記号をどう扱うか

**Status**: 回答済み (解決)

**Question**: Doxygen 由来の図へ出力する `Figure:` 行のキャプションに、Markdown で特別な意味を持つ記号 (`<`、`>`、`*`、`_`、`~`、`[`、`]` など) が含まれる場合、どう扱いますか。

**Context**: 現行の PlantUML 図では、キャプション (`caption divide の呼び出し先` など) がコード フェンスの内側にあるため、記号はそのまま表示されます。[Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか) (A) により `Figure:` 行はコード フェンスの外側へ出るため、Markdown の本文として解釈されます。自動生成グラフのキャプションは `<名前> の<グラフ種別>` の形式で、名前には C++ の `Foo<T>`、`operator*`、`~Foo` などが入る場合があります。最終仕様書 (版 1) はこの扱いを定めておらず、単体テストの項目にも含めていませんでした。

なお、`__` と `$` は [postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) の既存処理がコード フェンス外の本文に対して `&#95;&#95;` と `\$` へ変換するため、どの選択肢でも追加の対処は不要です。

名前が `Foo<T>::operator*` の場合、各案の Markdown 出力は次のようになります。

```markdown
<!-- A: 記号をバックスラッシュでエスケープする -->
Figure: Foo\<T\>::operator\* の呼び出しグラフ

<!-- B: 名前をインライン コードにする -->
Figure: `Foo<T>::operator*` の呼び出しグラフ

<!-- C: 何もしない (<T> が HTML タグ、* が強調の開始として解釈されうる) -->
Figure: Foo<T>::operator* の呼び出しグラフ
```

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 19:35:24 JST  
選択方式: 単一選択

- [x] A: (推奨) 変換ヘルパーが `Figure:` 行のキャプション内の記号をバックスラッシュでエスケープし、名前を文字どおりに表示する。手動コメントのキャプションと自動生成グラフのキャプションに同じ規則を適用する。
- [ ] B: 自動生成グラフのキャプションは名前の部分をインライン コードにする。手動コメントのキャプションは記述されたまま出力する。
- [ ] C: 記号を加工せずに出力し、記号を含む名前でキャプションの表示が崩れる場合があることを容認する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 手動コメントと自動生成グラフの双方へ 1 つの規則を適用でき、現行の PlantUML 図と同じく名前が文字どおりに表示されるためです。B は名前の位置が分かる自動生成グラフにしか適用できず、キャプションの見た目が系統ごとに変わります。

**Assessment**: 推奨 A の選択を確認しました。記号を含む名前を文字どおりに表示し、手動コメントと自動生成グラフへ同じ規則を適用する方針が確定しました。ただし、試作検証 ([版 7 の試作検証で確認した事項](#版-7-の試作検証で確認した事項-確認日-2026-11-14)) で、バックスラッシュによるエスケープは `[` で出力を壊すことが判明しました。選択肢 A の文面にある手段 (バックスラッシュ) をそのまま採用できないため、エスケープの手段を [Q-018](#q-018-キャプションの記号をどの手段でエスケープするか) で確認します。

**History**: [Q-018](#q-018-キャプションの記号をどの手段でエスケープするか) (A) の回答 (2026-11-14 19:57:33 JST) により、エスケープの手段が数値文字参照に確定したため、状態を解決へ更新しました。

**Related**: [Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか)、[Q-018](#q-018-キャプションの記号をどの手段でエスケープするか)

### Q-012 Mermaid タグ変換ヘルパーをどの位置で呼び出すか

**Status**: 回答済み (解決)

**Question**: `<mermaid>` タグをコード フェンスへ変換する新設ヘルパー (`convert-mermaid-tags.py`) を、[preprocess.sh](../../../../../framework/doxyfw/templates/preprocess.sh) のどの位置で呼び出しますか。

**Context**: 最終仕様書 (版 1) は呼び出し位置を「既存の `<plantuml>` 変換と同時期、`</para>` や `<bold>` 処理の前または並列段」と記載しており、位置が 1 つに定まっていませんでした。`preprocess.sh` には 2 種類の呼び出し方があります。第 1 は、XML ファイルのループより前に XML フォルダーを引数として 1 回だけ呼ぶ方式で、`strip-anonymous-namespaces.py`、`mark-admonitions.py`、`fix-anonymous-enums.py` が該当します。第 2 は、XML 1 ファイルごとの `sed` パイプで、既存の `<plantuml>` 変換が該当します。Python をこのパイプへ入れると、XML ファイルの数だけ Python が起動します。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 19:35:24 JST  
選択方式: 単一選択

- [x] A: (推奨) XML ファイルのループより前に、XML フォルダーを引数として 1 回だけ呼び出す (`mark-admonitions.py` と同じ方式)。
- [ ] B: XML 1 ファイルごとの `sed` パイプへ、標準入力から標準出力へ変換する段として追加する (`<plantuml>` 変換の直後)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 既存の Python ヘルパーと呼び出し方がそろい、Python の起動が 1 回で済むためです。変換後のコード フェンスは `<bold>`、`<name>`、`<title>` を含まないため、後続の `sed` 処理とは干渉しません。

**Assessment**: 推奨 A の選択を確認しました。試作検証 ([版 7 の試作検証で確認した事項](#版-7-の試作検証で確認した事項-確認日-2026-11-14)) で、XML ファイルのループより前に変換した XML が、`preprocess.sh` の後続処理、Doxybook2、`postprocess.sh` を終了コード `0`・警告なしで通過することを確認しました。

**Related**: [Q-002](#q-002-既存の-plantuml-変換経路を残すか)

### Q-013 Mermaid 変換経路の仕様をどの正本文書へ記載するか

**Status**: 回答済み (解決)

**Question**: `<mermaid>` タグの変換、`Figure:` 行の出力、`Figure:` 行を含めた折り畳みの終端という Mermaid 変換経路の仕様を、`framework/doxyfw/docs/` のどの文書へ記載しますか。

**Context**: 最終仕様書 (版 1) は [doxybook2-plantuml.md](../../../../../framework/doxyfw/docs/doxybook2-plantuml.md) を「PlantUML 互換経路」と位置づけるだけで、新設する Mermaid 変換経路そのものを記載する文書を定めていませんでした。[extract-graphs.md](../../../../../framework/doxyfw/docs/extract-graphs.md) は自動生成グラフの文書であり、手動コメントの `@mermaid` の変換は対象に含みません。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 19:35:24 JST  
選択方式: 単一選択

- [x] A: (推奨) `framework/doxyfw/docs/doxybook2-mermaid.md` を新設して Mermaid 変換経路を記載し、[文書一覧](../../../../../framework/doxyfw/docs/README.md) へ追加する。`doxybook2-plantuml.md` は PlantUML 変換の文書として残す。
- [ ] B: `doxybook2-plantuml.md` を図変換全体の文書 (`doxybook2-diagrams.md` など) へ改名し、Mermaid と PlantUML の双方を 1 つの文書に記載する。
- [ ] C: 文書は新設せず、`extract-graphs.md` へ Mermaid 変換経路の節を追加する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 変換経路ごとに 1 文書となり、既存文書への参照リンクを変更せずに済むためです。B は文書が 1 つにまとまりますが、改名に伴い既存の参照リンクの更新が必要です。

**Assessment**: 推奨 A の選択を確認しました。`framework/doxyfw/docs/doxybook2-mermaid.md` の新設と文書一覧への追加を、正本文書の更新対象へ加えます。

**Related**: [Q-003](#q-003-今後の図記法方針をどう改訂するか)

### Q-014 doxyfw の README の必要なツールを更新するか

**Status**: 回答済み (解決)

**Question**: [framework/doxyfw/README.md](../../../../../framework/doxyfw/README.md) の「必要なツール」を、今回の移行に合わせて更新しますか。

**Context**: 現行の「必要なツール」は Doxygen、Doxybook2、Python 3、PlantUML の 4 項目で、版数の記載はありません。`@mermaid` は Doxygen 1.17.0 で追加されたコマンドであり、それより古い Doxygen では未知のコマンドとして扱われます。最終仕様書 (版 1) の正本文書の更新対象 5 件に、この README は含まれていませんでした。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 19:35:24 JST  
選択方式: 単一選択

- [x] A: (推奨) Doxygen の項目へ「1.17.0 以上」を明記し、PlantUML の項目は互換経路で使用するツールとして残す。
- [ ] B: Doxygen の項目へ「1.17.0 以上」を明記し、PlantUML の項目は変更しない。
- [ ] C: README は更新しない。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: この環境は過去に未対応であったが現在は 1.17.0 としている。

**Recommendation**: 推奨 A — Doxygen の版数条件が新たに生じるため明記が必要であり、PlantUML が既定の図記法でなくなった後も互換経路で必要な理由を読み取れるためです。

**Assessment**: 推奨 A の選択を確認しました。補足の「現在は 1.17.0 としている」は、ローカルの Doxygen `1.17.0` および CI の `1.18.0` と整合します。`framework/doxyfw/README.md` を正本文書の更新対象へ加えます。

**Related**: [Q-002](#q-002-既存の-plantuml-変換経路を残すか)、[Q-003](#q-003-今後の図記法方針をどう改訂するか)

### Q-015 Doxygen HTML の Mermaid 描画方式をどうするか

**Status**: 回答済み (解決)

**Question**: Doxygen が生成する HTML で、手動コメントの Mermaid 図をどの方式で描画しますか。

**Context**: [Doxyfile](../../../../../framework/doxyfw/Doxyfile) は `GENERATE_HTML = YES` を指定しています。既定の `MERMAID_RENDER_MODE = AUTO` で HTML を生成すると、Doxygen は図を画像へ変換せず、HTML へ次の読み込み処理を埋め込みます (ローカルの Doxygen 1.17.0 で確認)。

```html
import mermaid from 'https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.esm.min.mjs';
```

このため、HTML を開いた端末が `cdn.jsdelivr.net` へ接続できない場合、図は描画されず Mermaid のソース テキストが表示されます。影響を受けるのは Doxygen がコメントから直接処理する手動コメントの図 (現時点では `memory_lock.h` の 2 図) だけです。自動生成グラフは Doxygen の実行後に XML へ挿入するため、Doxygen HTML には現れません。Doxybook2 経由の Markdown は docsfw が描画するため、どの選択肢でも影響を受けません。

`MERMAID_RENDER_MODE = CLI` を指定すると、Doxygen が `mmdc` を起動して画像を生成します。`mmdc` がない環境では、警告ログへ `error: Problems running Mermaid (mmdc)` が記録されます (終了コードは `0`)。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 19:35:24 JST  
選択方式: 単一選択

- [x] A: (推奨) 既定の `AUTO` のままとし、Doxygen HTML の Mermaid 図は閲覧時に CDN へ接続できる場合だけ描画されることを容認する。`MERMAID_*` の設定は追加しない。
- [ ] B: `MERMAID_RENDER_MODE = CLI` を指定し、Doxygen を実行するすべての環境 (ローカル、CI の Linux と Windows) へ `mmdc` の導入を必須とする。
- [ ] C: `MERMAID_JS_URL` でリポジトリまたは生成物に同梱した mermaid を参照させ、CDN へ接続せずに描画する (実現性は未検証のため、選択された場合に調査する)。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 対象が手動コメントの 2 図に限られ、Markdown 側の描画には影響しないためです。B は `mmdc` と Chrome をすべての実行環境の必須条件に加えるため、導入と CI の負担が増えます。

**Assessment**: 推奨 A の選択を確認しました。`MERMAID_*` の設定は追加せず、Doxygen HTML の Mermaid 図が閲覧時の CDN 接続に依存することを仕様として記載します。

**Related**: [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか)

### Q-016 自動生成グラフの単体テストをどのファイルへ置くか

**Status**: 回答済み (解決)

**Question**: [extract-graphs.py](../../../../../framework/doxyfw/templates/extract-graphs.py) の Mermaid 出力を検証する単体テストを、どのファイルへ置きますか。

**Context**: 最終仕様書 (版 1) は、このテストを [test_doxybook2_templates.py](../../../../../framework/doxyfw/bin_test/test_doxybook2_templates.py) へ追加する計画でした。同ファイルは Doxybook2 のテンプレート (`.tmpl`) を検証するテストであり、`extract-graphs.py` を対象とするテストは現在 `framework/doxyfw/bin_test/` に存在しません。テスト ファイルを新設する場合、[Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) (A) で承認された実行範囲 (Python 4 ファイル) に、実装後の実行対象として新設ファイルが加わります。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 19:35:24 JST  
選択方式: 単一選択

- [x] A: (推奨) `framework/doxyfw/bin_test/test_extract_graphs.py` を新設し、実装後の実行対象を承認済みの Python 4 ファイルと新設ファイルの計 5 ファイルとする。
- [ ] B: 最終仕様書 (版 1) のとおり `test_doxybook2_templates.py` へ追加し、実行対象を Python 4 ファイルのままとする。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — テスト ファイル名と検証対象が対応し、テンプレートのテストと責務が混ざらないためです。

**Assessment**: 推奨 A の選択を確認しました。[Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) (A) の実行範囲は、実装前が承認済みの Python 4 ファイル、実装後が新設の `test_extract_graphs.py` を加えた 5 ファイルとなります。

**Related**: [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか)

### Q-017 継承グラフの矢印をどの記法で表すか

**Status**: 回答済み (解決)

**Question**: 自動生成する継承グラフの矢印を、Mermaid のどの記法で表しますか。

**Context**: 現行の PlantUML は継承関係を `-u-|>` で出力し、派生から基底へ向かう白抜き三角の矢印 (UML の汎化) として描画します。Mermaid の `flowchart` には白抜き三角の矢印がないため、最終仕様書 (版 1) の `flowchart BT` と `-->` では、向きと上下の配置は保たれますが、矢印の形が通常の矢印へ変わります。白抜き三角を保つには `classDiagram` を使う必要があります。

```text
A: flowchart BT             B: classDiagram
   n1["Derived"] --> n0["Base"]   class n0["Base"]
                                  class n1["Derived"]
                                  n0 <|-- n1
```

`classDiagram` は、対象ノードの強調 (`style nX fill:...`) とラベルのエスケープの書き方が `flowchart` と異なります。`classDiagram` での強調と記号を含むラベルの描画は未検証です。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 19:35:24 JST  
選択方式: 単一選択

- [x] A: (推奨) `flowchart BT` と `-->` で出力し、矢印の形が白抜き三角から通常の矢印へ変わることを容認する。
- [ ] B: 継承グラフだけ `classDiagram` と `<|--` で出力し、白抜き三角の矢印を保つ (強調とエスケープの実現性は、選択された場合に調査する)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 4 種別のグラフでノード ID、ラベルのエスケープ、強調の生成処理を共通にでき、派生から基底への向きと配置は保たれるためです。

**Assessment**: 推奨 A の選択を確認しました。4 種別のグラフを `flowchart` で統一し、継承グラフの矢印の形が通常の矢印へ変わることを仕様として記載します。

**Related**: [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか)、[Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか)

### Q-018 キャプションの記号をどの手段でエスケープするか

**Status**: 回答済み (解決)

**Question**: [Q-011](#q-011-figure-行のキャプションに含まれる記号をどう扱うか) (A) で決定した「`Figure:` 行のキャプション内の記号を文字どおりに表示する」ためのエスケープを、どの手段で行いますか。

**Context**: [Q-011](#q-011-figure-行のキャプションに含まれる記号をどう扱うか) の選択肢 A は、手段をバックスラッシュと記載していました。試作検証 ([版 7 の試作検証で確認した事項](#版-7-の試作検証で確認した事項-確認日-2026-11-14)) で、同じキャプションを 2 つの手段でエスケープし、Doxybook2 と [postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) へ通しました。バックスラッシュでは、`\[` が `postprocess.sh` の LaTeX ブロック数式の分離処理 (行頭以外にある `\[` の前後へ改行を挿入する処理) に一致し、キャプション行が 3 行へ分断されました。数値文字参照では、すべての記号が保持されました。

```markdown
<!-- 入力のキャプション -->
Foo<T>::operator* の図 operator[] ~Foo

<!-- バックスラッシュでエスケープした場合の最終出力 (行が分断される) -->
Figure: Foo\<T\>::operator\* の図 operator

\[
\] \~Foo

<!-- 数値文字参照でエスケープした場合の最終出力 -->
Figure: Foo&#60;T&#62;::operator&#42; の図 operator&#91;&#93; &#126;Foo
```

数値文字参照は、`postprocess.sh` がコード フェンス外の `__` を `&#95;&#95;` へ変換する既存の方式と同じ手段です。Markdown を表示した際は、どの手段でも記号そのものが表示されます。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 19:57:33 JST  
選択方式: 単一選択

- [x] A: (推奨) 対象の記号 (`\`、`` ` ``、`*`、`<`、`>`、`[`、`]`、`~`) をすべて数値文字参照 (`&#42;` など) へ変換する。`__` と `$` は `postprocess.sh` の既存処理に任せる。
- [ ] B: `[` と `]` だけを数値文字参照へ変換し、それ以外の記号はバックスラッシュでエスケープする。
- [ ] C: すべての記号をバックスラッシュでエスケープし、`postprocess.sh` の LaTeX ブロック数式の分離処理が `Figure:` 行を対象外とするよう変更する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — 試作検証ですべての記号が保持されることを確認済みであり、`postprocess.sh` の既存の変換方式と手段がそろうためです。B は 2 つの手段が混在します。C は `postprocess.sh` の数式処理へ例外を追加するため、変更範囲が広がります。

**Assessment**: 推奨 A の選択を確認しました。[Q-011](#q-011-figure-行のキャプションに含まれる記号をどう扱うか) (A) の方針を、対象の記号 (`\`、`` ` ``、`*`、`<`、`>`、`[`、`]`、`~`) の数値文字参照への変換で実現します。`__` と `$` は `postprocess.sh` の既存処理に任せます。

**Related**: [Q-011](#q-011-figure-行のキャプションに含まれる記号をどう扱うか)

### Q-019 手動コメントの図ブロックをどの桁へ字下げするか

**Status**: 回答済み (解決)

**Question**: [memory_lock.h](../../../../cplat/prod/include/cplat/runtime/memory_lock.h) の手動コメントで、行頭の `*` を置かない図ブロック (`@mermaid` 〜 `@endmermaid`) を、どの桁へ字下げしますか。

**Context**: `memory_lock.h` のコメントは `extern "C"` の内側にあり、本文は `     *  ` (8 桁) の後から始まります。現行の `@startuml` は 4 桁目に置かれています。試作検証 ([版 7 の試作検証で確認した事項](#版-7-の試作検証で確認した事項-確認日-2026-11-14)) で、現行の `memory_lock.h` を Doxygen 1.17.0 へ入力すると、図ブロックの前後にある `@par` の節 (「呼び出しタイミング」「スレッド セーフ」など) が XML の `<verbatim>` へ変換されることを確認しました。Doxygen が、図ブロックより深く字下げされた前後の行を Markdown の字下げコード ブロックと解釈するためです。図ブロックを `@mermaid` へ置き換えても、4 桁目に置く限り同じ結果になりました。図ブロックを本文と同じ 8 桁目に置くと、`@par` の節は正しく `<simplesect kind="par">` へ変換されました。

```c
    /**
     *  @par            呼び出しタイミング
     *  本文
     *
    @mermaid "A: 現行と同じ 4 桁目 (前後の @par が verbatim になる)"
        sequenceDiagram
    @endmermaid
     *
        @mermaid "B: 本文と同じ 8 桁目 (前後の @par が正しく変換される)"
            sequenceDiagram
        @endmermaid
     *
     *  @par            スレッド セーフ
     */
```

8 桁目へ置く場合、該当する 2 つの関数では、図以外の本文 (`@par` の節) の生成結果も変更前から変わります。どちらの位置でも [check-doxygen-indent.py](../../../../../framework/doxyfw/bin/check-doxygen-indent.py) は問題なしと判定します。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 19:57:33 JST  
選択方式: 単一選択

- [x] A: (推奨) 図ブロックを本文と同じ桁 (`memory_lock.h` では 8 桁目) へ字下げし、前後の `@par` の節が `<verbatim>` になる現行の不具合も同じ変更で解消する。`commands.md` と `cheatsheet.md` へ「行頭の `*` を置かない図ブロックは、コメント本文と同じ桁へ字下げする」規則を記載する。
- [ ] B: 現行と同じ 4 桁目のままとし、前後の `@par` の節が `<verbatim>` になる事象は今回の対象外として残課題一覧へ記録する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: plantuml も含めて、正しく図を扱うための規範を強化すること。

**Recommendation**: 推奨 A — 図を置き換える行と同じ箇所の修正で不具合を解消でき、変更後の生成結果を正しい状態で比較できるためです。B を選ぶと、移行後も該当関数の説明がコード ブロックとして出力され続けます。

**Assessment**: 推奨 A の選択を確認しました。`memory_lock.h` の図ブロックを本文と同じ 8 桁目へ字下げし、`commands.md` と `cheatsheet.md` へ字下げの規則を記載します。補足の「plantuml も含めて、正しく図を扱うための規範を強化すること」により、字下げの規則は `@mermaid` だけでなく `@startuml` にも適用する方針が確定しました。一方、「規範を強化」の範囲 (文書への記載だけか、機械検査の追加まで含むか) は補足から一意に定まらないため、[Q-020](#q-020-図の規範の強化をどの範囲で行うか) で確認します。

**History**: [Q-020](#q-020-図の規範の強化をどの範囲で行うか) (A) の回答 (2026-11-14 20:04:35 JST) により、規範の強化の範囲が確定したため、状態を解決へ更新しました。

**Related**: [Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか)、[Q-020](#q-020-図の規範の強化をどの範囲で行うか)

### Q-020 図の規範の強化をどの範囲で行うか

**Status**: 回答済み (解決)

**Question**: [Q-019](#q-019-手動コメントの図ブロックをどの桁へ字下げするか) の補足「plantuml も含めて、正しく図を扱うための規範を強化すること」を、どの範囲で実現しますか。

**Context**: [Q-019](#q-019-手動コメントの図ブロックをどの桁へ字下げするか) (A) により、行頭の `*` を置かない図ブロックをコメント本文と同じ桁へ字下げする規則を、`@mermaid` と `@startuml` の双方について正本文書へ記載します。字下げが浅い場合、Doxygen は警告を出さずに前後の `@par` の節を `<verbatim>` へ変換するため、生成結果を目視しない限り誤りに気付けません。現行の `memory_lock.h` もこの状態でした。

[check-doxygen-indent.py](../../../../../framework/doxyfw/bin/check-doxygen-indent.py) は、Doxygen コメントの字下げレベルを検査・修正する手動実行のコマンドです。現在は、コメント内で行頭が `*` でない行を検査の対象外としているため、図ブロックの字下げの誤りを検出しません。このコマンドへ検査を追加すれば、規則への違反を機械的に検出できます。追加する場合は、検査の単体テスト (`framework/doxyfw/bin_test/` への新設) と [check-doxygen-indent.md](../../../../../framework/doxyfw/docs/check-doxygen-indent.md) の更新が今回の変更対象へ加わります。

正本文書へ記載する規範の内容は、どの選択肢でも次のとおりです。

- 図ブロック (`@mermaid` 〜 `@endmermaid`、`@startuml` 〜 `@enduml`) は、行頭の `*` を置かない場合、コメント本文と同じ桁へ字下げする。
- Mermaid のキャプションは `@mermaid "キャプション"` の形式で指定する。
- 新規の図は Mermaid で記述し、PlantUML は互換の記法として扱う ([Q-003](#q-003-今後の図記法方針をどう改訂するか) (A))。

**Answer**

回答者: Tetsuo Honda / 回答日時: 2026-11-14 20:04:35 JST (対話セッションの選択ダイアログで「文書 + 検出の検査」を選択。AI が転記)  
選択方式: 単一選択

- [x] A: (推奨) 正本文書 (`maintenance-verification.md`、`commands.md`、`cheatsheet.md`) へ規範を記載し、`check-doxygen-indent.py` へ図ブロックの字下げ検査 (検出のみ。自動修正は行わない) と単体テストを追加する。
- [ ] B: 正本文書へ規範を記載するだけとし、機械検査は追加しない。
- [ ] C: A に加えて、`check-doxygen-indent.py` の `--fix` で図ブロックの字下げを自動修正できるようにする。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**: 推奨 A — この誤りは Doxygen が警告を出さないため、文書への記載だけでは再発を検出できないためです。自動修正 (C) は、図の本文の相対的な字下げを保ったまま移動する処理が必要になり、現時点で対象が 1 ファイルだけのため、費用に見合いません。

**Assessment**: 推奨 A の選択を確認しました。正本文書へ規範を記載し、`check-doxygen-indent.py` へ図ブロックの字下げ検査 (検出のみ) と単体テスト (`test_check_doxygen_indent.py` の新設) を追加します。これにより、[Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) (A) と [Q-016](#q-016-自動生成グラフの単体テストをどのファイルへ置くか) (A) の実行範囲は、実装前が承認済みの Python 4 ファイル、実装後が新設の 2 ファイルを加えた 6 ファイルとなります。追加の字下げパターンの検証で、検査の条件 (開始行と終了行は本文と同じ桁、図の本文は開始行より浅くしない) も確定しました。

**Related**: [Q-003](#q-003-今後の図記法方針をどう改訂するか)、[Q-019](#q-019-手動コメントの図ブロックをどの桁へ字下げするか)

## AI が確認する技術事項

作業 WBS 1.3 の技術調査として、ローカルの Doxygen `1.17.0`、Doxybook2 `v1.6.1`、既存の前後処理、`mmdc` `11.17.0`、および関連スクリプトを確認しました。  
実証できた事項と未実証・設計待ちの事項を分離して記録します。いずれも第 2 ゲート通過前であるため、本番ファイルへの確定実装としては扱いません。

### 確認済みの技術事実 (確認日: 2026-11-14)

- **Doxygen Release 1.17.0 (2026-04-30) の公式対応**: [Doxygen Changelog](https://www.doxygen.nl/manual/changelog.html) に Mermaid 対応の追加が明記されており、コメント内コマンドとして `@mermaid`、`@endmermaid`、`@mermaidfile` が追加され、設定項目として `MERMAID_PATH`、`MERMAID_CONFIG_FILE`、`MERMAID_RENDER_MODE`、`MERMAID_JS_URL`、`MERMAIDFILE_DIRS` が追加されています。
- **コメント内構文の公式説明**: [Doxygen Manual (`@mermaid`)](https://www.doxygen.nl/manual/commands.html#cmdmermaid) がコメント内構文の公式説明として公開されています。
- **ローカル環境の Doxygen 版数と既定設定**: ローカルの `doxygen --version` は `1.17.0 (65a43c0aba45cc23b3ca11b6b5334d4eea931726)` です。また、`doxygen -g -` の出力で上記 5 つの `MERMAID_*` 設定項目の存在と、`MERMAID_RENDER_MODE = AUTO` (選択値: `AUTO`、`CLI`、`CLIENT_SIDE`) が既定値であることを確認済みです。
- **Doxygen `1.17.0` の実 XML 出力と `mmdc` なし動作 (一時ディレクトリ最小サンプル検証)**:
    - `@mermaid` 〜 `@endmermaid` は XML に `<para><mermaid>...</mermaid></para>` として出力され、図内の `__flag__` は `<bold>` へ変換されず生文字のまま保持 (`>` は `&gt;`、`"` は `&quot;` へ XML エスケープ) されました。
    - `@mermaid{caption="Attr caption"}` は `<mermaid caption="{caption=&quot;Attr caption&quot;}">...</mermaid>` として出力されました。
    - `@mermaidfile flow.mmd "Flow caption"` は `<mermaidfile name="flow.mmd">Flow caption</mermaidfile>` として出力されました。
    - `MERMAID_RENDER_MODE = AUTO` (既定) かつ存在しない `MERMAID_PATH` (`mmdc` なし相当) の条件では、`GENERATE_HTML = NO` / `YES` ともに終了コード `0` で、Mermaid に関するエラー・警告は `WARN_LOGFILE` および標準エラー出力に出ませんでした。
    - 参考として、`MERMAID_RENDER_MODE = CLI` かつ存在しない `MERMAID_PATH` では終了コード `0` で XML は生成されるものの、`WARN_LOGFILE` に `error: Problems running Mermaid (mmdc)... Exit code: 1.` が記録されました。
- **Doxybook2 `v1.6.1` と既存前後処理の未対応実態 (一時ディレクトリ検証)**:
    - 現行の [preprocess.sh](../../../../../framework/doxyfw/templates/preprocess.sh) は `<plantuml>` のみを対象とし、`<mermaid>` と `<mermaidfile>` を変換しません (終了コード `0` で素通り)。
    - Doxybook2 (`v1.6.1`) は `<mermaid>` と `<mermaidfile>` に未対応であり、標準出力へ `[warning] Text tag "mermaid" not recognised, please contact the author` および `[warning] Text tag "mermaidfile" not recognised, please contact the author` を出力し、タグを除去して図本文をコード フェンスなしの生テキストとして出力しました (`<mermaidfile>` は外部 `.mmd` 本文を取り込めずキャプション文字列のみ残存)。また、隣接する図テキストが同一行へ連結される事象を確認しました。
    - そのまま現行の [postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) へ渡すと、フェンス外の通常本文として扱われるため、Mermaid 図内の `__flag__` が `&#95;&#95;flag&#95;&#95;` へ破壊的に置換されることを確認しました。
    - なお、`<mermaid>` タグを前処理相当で ```` ```mermaid ```` 〜 ```` ``` ```` へ変換して Doxybook2・[postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) へ通した仮検証では、`call(__flag__)` を壊さず ```` ```mermaid ```` ブロックとして保存でき、終了コード `0` で完了しました (`in_code_block && !is_plantuml` 分岐により `~_~_` 退避と `&#95;&#95;` 変換の双方が回避される)。
- **前後処理・入力フィルターのコード確認で判明した設計上の留意点**:
    - [postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) (L504-549) の `fix_ptr` 処理は `in_code_block` 全体に対して `"..."` の外側にある `foo* bar` / `\* ident` を書き換える仕様です。引用符内の `"foo* bar"` や `"* ptr"` は保持される一方、引用符外の `*` を含む Mermaid 構文は影響を受けるため、Mermaid ブロックの除外またはラベル二重引用符化の設計が必要です。
    - [postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) (L368-423) の `DOXYFW_DETAILS_ONLY` 折り畳み処理は、閉じコード フェンス ```` ``` ```` の直後で `<!--:details-->` を閉じるため、図直後に `Figure:` 行を出力する場合は整合が必要です ([Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか))。
    - [input-filter.py](../../../../../framework/doxyfw/bin_internal/input-filter.py) (L1-28) は Doxygen 入力 Markdown から ```` ```plantuml ```` ブロックだけを除去しており、```` ```mermaid ```` は除去対象に含まれていません。
- **ローカル環境の `mmdc` 単体描画確認**: ローカルの `mmdc --version` は `11.17.0` であり、最小 Mermaid ファイルからの SVG 生成 (`mmdc -i ... -o ...`) が終了コード `0` (10,955 バイト出力) で成立することを確認しました。

`@mermaid` と `@endmermaid` を用いたコメント内記述の簡潔な例は次のとおりです。

```c
/**
 * @mermaid
 * flowchart LR
 *     A --> B
 * @endmermaid
 */
```

- **CI の Doxygen 版数と Doxyfile 設定の実態**: [ci.yml](../../../../../.github/workflows/ci.yml) は `DOXYGEN_VERSION: "1.18.0"` を使用しており、[Doxyfile](../../../../../framework/doxyfw/Doxyfile) は `GENERATE_HTML = YES` および `GENERATE_XML = YES` を指定しています (`MERMAID_*` 設定は未指定のため既定の `MERMAID_RENDER_MODE = AUTO` が適用されます)。
- **Mermaid 構文・エスケープ・`mmdc` (`11.17.0`) 構文別描画の一時ディレクトリ検証**:
    - `flowchart TD` (`style n0 fill:#ADD8E6,stroke:#333,font-weight:bold`、`-->`、`-.->`)、`flowchart BT` (`n1 -->|"inheritance"| n0`)、`classDiagram`、`sequenceDiagram` (`memory_lock.h` 2 図相当)、および特殊文字 (`#quot;`、`&amp;`、`&lt;`、`&gt;`、`<br/>`、`__dunder__`、`[brack]`、`{brace}`、`#hash` を `"..."` ラベル内に格納) の全 5 ケースで `mmdc` SVG 出力が終了コード `0` で成功しました。
    - `<mermaid>` 内へ生の `<b>` タグを出力すると Doxybook2 が `[warning] Text tag "b" not recognised, please contact the author` を出す一方、XML エスケープした `&lt;br/&gt;`・`&amp;`・`&gt;`・`&quot;` は Doxybook2 で無警告のまま Markdown へ復号されることを確認しました。
    - `check-doxygen-indent.py` は `/** ... */` 内で `^\s*\*` で始まらない行をスキップするため、4 スペース基準の `@mermaid{caption="..."}` 〜 `@endmermaid` と整合します。
- **`@mermaidfile` の扱い**: 移行対象の手動 2 図 ([memory_lock.h](../../../../cplat/prod/include/cplat/runtime/memory_lock.h)) と [extract-graphs.py](../../../../../framework/doxyfw/templates/extract-graphs.py) の自動生成グラフ 4 種別のいずれも `@mermaidfile` / 外部図ファイル参照を使用していないため、本移行の必須要件には含めません。

### 第 2 ゲート レビューによる訂正事項 (確認日: 2026-11-14)

最終仕様書 (版 1) のレビューで、次の事実誤りと記載不足が見つかりました。いずれも回答済みの質問の決定を変更するものではなく、最終仕様書の版 2 で訂正します。

- **`memory_lock.h` の置き換え図が現行図と一致しない**: 最終仕様書 (版 1) の Mermaid 2 図は、現行の PlantUML 2 図 ([memory_lock.h](../../../../cplat/prod/include/cplat/runtime/memory_lock.h) L130-153、L175-204) の変換になっていませんでした。
    - `memory_lock(min_bytes)`、`memory_unlock(base, size)`、`CPLAT_MEMORY_LOCK_STATE_LOCKED` は `app/cplat/prod` に存在しません。
    - 現行図の参加者 (呼び出し側、API、内部 lock、OS)、Linux と Windows の分岐、`stack_prefault_bytes` の条件、ref-count の加減算、内部 lock の取得と解放が欠落していました。
    - 2 図目の題名が「ロック解放」から「解放フロー」へ変わり、現行にない `@par 動作フロー` と `@par 解放フロー` が追加されていました。
    - 版 2 では、現行図の参加者、メッセージ、分岐、題名を 1 対 1 で Mermaid へ書き直し、`@par` の追加は行いません ([Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか) (A) の意味一致に従います)。
- **`@mermaid{caption="..."}` はキャプション指定の構文ではない**: ローカルの Doxygen 1.17.0 で 2 つの記述を比較しました。
    - `@mermaid{caption="Brace caption"}` は、XML へ `caption="{caption=&quot;Brace caption&quot;}"` を出力し、Doxygen HTML のキャプションに `{caption="Brace caption"}` と波括弧ごと表示されました。
    - `@mermaid "Quoted caption"` は、XML へ `caption="Quoted caption"` を出力し、Doxygen HTML のキャプションに `Quoted caption` と表示されました。
    - 「確認済みの技術事実」と [Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか) の **Context** にある `@mermaid{caption="..."}` の記載は、波括弧全体がキャプション文字列として扱われた結果であり、キャプション指定の正しい構文は `@mermaid "キャプション"` です。
    - 版 2 では、手動コメントの記述、変換ヘルパーのキャプション取得、自動生成グラフの `<mermaid caption="...">`、`commands.md` と `cheatsheet.md` の構文例を `@mermaid "キャプション"` の形式へそろえます。
- **`extract-graphs.md` の更新範囲**: 生成例の 4 箇所のほかに、冒頭の説明、パイプライン図の注記、`relation` と矢印の対応表、XML 挿入例と変換の流れ (L251-275) も PlantUML を前提としています。[Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) (A) と [Q-003](#q-003-今後の図記法方針をどう改訂するか) (A) により、これらも版 2 の更新対象へ含めます。
- **自動生成グラフの見出しとキャプションの区別**: `<simplesect>` の `<title>` へ入れる見出し (「呼び出しグラフ」など) と、`<mermaid>` の `caption` へ入れるキャプション (`<名前> の<グラフ種別>`) は別の文字列です。版 2 では両者を区別して記載します。
- **Markdown 入力の Mermaid コード フェンスという第 3 の発生源**: Doxygen 1.17.0 は、入力 Markdown の ```` ```mermaid ```` コード フェンスを XML の `<para><mermaid>...</mermaid></para>` (`caption` 属性なし) へ変換します (一時ディレクトリの最小サンプルで確認)。直後の `Figure:` 行は、別の `<para>Figure: ...</para>` として出力されます。
    - Doxygen の入力となる Markdown には、現時点で ```` ```mermaid ```` が 6 箇所あります (`app/cplat/prod/libsrc/cplat/runtime/README.md` 1、`app/cplat/prod/libsrc/cplat/console/README.md` 1、`app/doxygen-sample/prod/src/markdown_sample.md` 1、`app/service-sample/prod/src/cmd/service-sample/README.md` 3)。
    - この件数は、CI の `docs-warns` に記録された Doxybook2 の警告 `Text tag "mermaid" not recognised` 6 件と一致します。現行の前処理は `<mermaid>` を変換しないため、Doxybook2 が警告を出します。ただし、`postprocess.sh` は Doxybook2 が出力した `Pages/` を削除し、`copy-markdown-from-input.sh` で入力の Markdown を複製するため、該当ページの最終出力では図は崩れていません。影響は警告の記録だけです。
    - 最終仕様書 (版 1) は移行対象を手動コメントと自動生成グラフの 2 系統としており、この発生源を記載していませんでした。新設する変換ヘルパーは発生源を問わず `<mermaid>` を変換するため、版 2 では Markdown 入力の図も変換対象として明記し、単体テスト (`caption` 属性なしの `<mermaid>` と後続の `Figure:` 段落) と受入条件 (`app/cplat` の警告ファイル (`doxy*.warn`) から `Text tag "mermaid" not recognised` が解消すること) へ加えます。
- **前後処理の記載は実コードと一致**: `is_plantuml` 分岐による `__` の保護、`fix_ptr` がコード フェンス全体へ適用される点、閉じコード フェンスの直後で `<!--:details-->` を出力する点は、最終仕様書 (版 1) の記載と [postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) の実装が一致していました。既存 Markdown 3 箇所の行番号と節名も実ファイルと一致していました。

### 版 7 の試作検証で確認した事項 (確認日: 2026-11-14)

最終仕様書の版 2 へ記載する内容を、一時ディレクトリの試作で検証しました。リポジトリ内のファイルは変更していません。

- **`memory_lock.h` の 2 図の 1 対 1 変換**: 現行の PlantUML 2 図の参加者、メッセージ、`opt` / `alt` / `loop` の入れ子、題名を保ったまま `sequenceDiagram` へ書き直し、`@mermaid "キャプション"` 〜 `@endmermaid` で記述しました。
    - Doxygen `1.17.0` は終了コード `0`・警告なしで、XML へ `<mermaid caption="cplat_memory_lock_self のロック取得">` と `<mermaid caption="cplat_memory_lock_scope_release のロック解放">` を出力しました。Doxygen HTML のキャプションも題名どおりでした。
    - 2 図とも `mmdc` (`11.17.0`) で SVG を生成でき、終了コード `0`・構文エラーなしでした。
    - `check-doxygen-indent.py` は問題なしと判定しました。
- **変換経路の通し確認**: `<mermaid>` をコード フェンスと `Figure:` 行へ置き換える試作スクリプトを XML ファイルのループより前に実行し、[preprocess.sh](../../../../../framework/doxyfw/templates/preprocess.sh)、Doxybook2 `v1.6.1`、[postprocess.sh](../../../../../framework/doxyfw/templates/postprocess.sh) へ通しました。
    - すべて終了コード `0` で、Doxybook2 の警告はありませんでした。
    - 最終出力では、```` ```mermaid ```` の閉じコード フェンス、空行、`Figure:` 行の順に出力されました。コード フェンス内の `__x__` と `a* b` は変更されませんでした (`"..."` で囲んだラベルの内側)。
- **キャプションの記号のエスケープ**: 加工なし、バックスラッシュ、数値文字参照の 3 通りを比較しました。
    - 加工なしでは、`<T>`、`*`、`` ` `` が Markdown の記法としてそのまま出力されました。`__` は `&#95;&#95;` へ、`$` は `\$` へ `postprocess.sh` が変換しました。
    - バックスラッシュでは、`\[` が LaTeX ブロック数式の分離処理に一致し、キャプション行が分断されました。
    - 数値文字参照 (XML 上は `&amp;#42;` の形式) では、すべての記号が `&#42;` などの形で最終出力まで保持されました。
    - 扱いを [Q-018](#q-018-キャプションの記号をどの手段でエスケープするか) で確認します。
- **図ブロックの字下げ位置と `<verbatim>`**: コメント本文が 8 桁目から始まる `memory_lock.h` の形式で、図ブロックの位置を変えて Doxygen `1.17.0` の XML を比較しました。
    - 図ブロックを 4 桁目に置くと、前後の `@par` の節が `<verbatim>` になりました。行頭に `*` を置かない場合も、本文より浅い位置にある限り同じでした。
    - 図ブロックを 8 桁目に置く場合と、行頭へ `     *  ` を付ける場合は、`@par` の節が `<simplesect kind="par">` になりました。
    - 現行の `memory_lock.h` (4 桁目の `@startuml`) でも、該当する 2 関数で `<verbatim>` が 4 箇所出力されました。この確認は既定値の Doxyfile で行っており、`framework/doxyfw/Doxyfile` を使う実際の生成結果は、実装前の基準値取得で確認します。
    - 扱いを [Q-019](#q-019-手動コメントの図ブロックをどの桁へ字下げするか) で確認します。
- **図ブロックの字下げ条件の追加確認 (版 9)**: 本文が 8 桁目から始まるコメントで、次の結果を確認しました。
    - 開始行と終了行を本文より深い 12 桁目に置くと、図ブロック自体が `<verbatim>` になり、`<mermaid>` が出力されませんでした。
    - 開始行と終了行を 8 桁目に置いても、図の本文を 4 桁目に置くと、図ブロックが壊れて `unexpected command endmermaid` の警告が出ました。
    - `@startuml` 〜 `@enduml` も、4 桁目では前後の `@par` の節が `<verbatim>` になり、8 桁目では正しく変換されました。
    - 以上から、規範と検査の条件は「開始行と終了行をコメント本文と同じ桁に置き、図の本文を開始行より浅くしない」となります。
- **自動生成グラフのラベル記法の再確認 (版 9)**: `flowchart BT`、`&lt;` / `&gt;` / `&amp;` / `#quot;` / `<br/>` と `__x__`、`*`、`[`、`{` を含む二重引用符付きのラベル、ラベル付きの `-.->`、`style` による強調を 1 つの図へまとめ、`mmdc` (`11.17.0`) で終了コード `0`・構文エラーなしを確認しました。

### 未実証の技術事項・今後の設計検証項目

方針決定に必要な事前調査は完了しました。以下の実行検証は、[Q-005](#q-005-検証する-app-の範囲をどうするか) (A)・[Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか) (A)・[Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) (A) に従い第 2 ゲート承認後の実装フェーズで順次実施します。

1. [framework/doxyfw/bin_test/](../../../../../framework/doxyfw/bin_test) の変更前基準値および実装後検証 (承認済み Python 4 ファイル: `test_preprocess.py`、`test_postprocess.py`、`test_doxybook2_templates.py`、`test_materialize_group_members.py`)。
2. `app/cplat` での `make doxy` 生成前後比較、`doxyfw` `.warn` 確認、および代表図 (手動 2 図 + 自動生成グラフ 4 種別) の `mmdc` 描画エラーなし・意味一致確認。

## 観点別の確認

| 観点 | 状態 | 根拠・関連項目 |
| --- | --- | --- |
| 目的・利用者 | 確定 | [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) (A)、[Q-003](#q-003-今後の図記法方針をどう改訂するか) (A) |
| 範囲・境界 | 確定 | [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) (A)、[Q-002](#q-002-既存の-plantuml-変換経路を残すか) (A)、[Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) (B)、[Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか) (B)、[Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか) (A)。`@mermaidfile` は対象外 |
| 入力・根拠 | 確定 (技術調査完了) | 作業 WBS 1.3 (Doxygen `1.17.0` XML・Doxybook2 `v1.6.1`/前後処理・`mmdc` `11.17.0` 構文別描画・27 ファイル突合を 2026-11-14 に完了。[Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) 参照) |
| 整合性・依存関係 | 確定 | [Q-002](#q-002-既存の-plantuml-変換経路を残すか) (A)、[Q-003](#q-003-今後の図記法方針をどう改訂するか) (A)、[Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか) (A) により `Figure:` 出力と `DOXYFW_DETAILS_ONLY` 折り畳みを整合 |
| 実現性・例外 | 確定 | 作業 WBS 1.3 (`<mermaid>` XML 変換・`fix_ptr` の `is_mermaid` 除外・`<!--:details-->` 終端調整・XML/`#quot;` エスケープを一時ディレクトリ検証で確認済み) |
| 受入条件・検証 | 確定 | [Q-005](#q-005-検証する-app-の範囲をどうするか) (A)、[Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか) (A)、[Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) (A) (`bin_test` 4 本の基準値/再検証は第 2 ゲート後に実施) |

Table: 多角的な確認の観点別状態

## 付録: 過去の完了報告 (手書き Markdown 27 ファイル)

前版には、手書き管理 Markdown 全 57 箇所について Mermaid 置換、描画比較、`CodeBlock: <caption-text>` 適用、`text_style_jp.py` 検証を完了した旨の報告が記録されていました。  
ただし、以下 27 ファイルに記載された件数を合計すると 68 箇所となり見出しの 57 箇所と一致せず、`CodeBlock:` は現行規範では非推奨です。[Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) (B) の回答に基づき、2026-11-14 に read-only で全 27 ファイルの実態を突合しました。

### 過去 27 ファイルの実態調査結果と既存報告との差異 (2026-11-14 確認)

- **対象ファイルの存在**: 27 ファイルすべてがリポジトリ内に実在します。
- **全体の集計結果**: `mermaid` フェンス **70 箇所**、`plantuml` フェンス **4 箇所** (`extract-graphs.md` の生成例)、`Figure:` キャプション **67 箇所**、`CodeBlock:` キャプション **1 箇所**、キャプションなし `mermaid` 図 **2 箇所**。
- **既存報告との主な差異**:
    1. **`CodeBlock:` 適用という記録と実態の差**: 27 ファイルのうち **25 ファイルはすでに全 Mermaid 図で `Figure:` へ移行済み** でした (`CodeBlock:` は 1 箇所のみ残存)。
    2. **見出し 57 / 内訳 68 と実在 70 の差**: 27 ファイルのうち 25 ファイルは過去報告の各ファイル件数と Mermaid 図数が一致し、No.13 (`architecture.md`: 報告 6 に対し実在 7) と No.19 (`sequence-data.md`: 報告 9 に対し実在 10) に各 1 箇所のキャプションなし Mermaid 図が存在したため合計が 70 箇所 (68 + 2) となっています。
    3. **差異のある個別ファイル**:
        - **No.12 `app/cplat/docs/prompts/current/cplat-account-session-discussion.md`**: 報告 `1 箇所` に対し `mermaid` 1 箇所、`Figure:` 0 箇所、`CodeBlock:` **1 箇所** (L295)。同ファイルには別途 `{.c caption="..."}` の C コード ブロック 5 箇所があります。扱いを [Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか) で確認します。
        - **No.13 `app/porter/docs/architecture.md`**: 報告 `6 箇所` に対し `mermaid` **7 箇所**、`Figure:` 6 箇所。先頭の `flowchart LR` (L28-36) にキャプションがありません。扱いを [Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか) で確認します。
        - **No.19 `app/porter/docs/sequence-data.md`**: 報告 `9 箇所` に対し `mermaid` **10 箇所**、`Figure:` 9 箇所。末尾側の `sequenceDiagram` (L408-421) にキャプションがありません。扱いを [Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか) で確認します。
        - **No.20 `framework/doxyfw/docs/extract-graphs.md`**: 報告 `1 箇所` と一致する `mermaid` 1 箇所 (`Figure:` 1 箇所) のほか、`extract-graphs.py` の現行出力例を示す `plantuml` コード ブロック **4 箇所** (L139、L154、L169、L184) が含まれます (本件の正本更新対象)。

1. `README.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)
2. `app/c-modernization-kit/docs/github-actions.md` (報告 2 箇所 / 実態 `mermaid` 2、`Figure:` 2)
3. `app/general/docs/skill-guide/01-modern-development/about-modern-development.md` (報告 2 箇所 / 実態 `mermaid` 2、`Figure:` 2)
4. `app/general/docs/skill-guide/README.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)
5. `app/service-sample/docs/service-events.md` (報告 2 箇所 / 実態 `mermaid` 2、`Figure:` 2)
6. `app/service-sample/prod/src/cmd/service-sample/README.md` (報告 3 箇所 / 実態 `mermaid` 3、`Figure:` 3)
7. `framework/makefw/docs/template-auto-selection.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)
8. `framework/testfw/docs/about-shared-lib-static-linking.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)
9. `app/cplat/docs/memory-lock.md` (報告 2 箇所 / 実態 `mermaid` 2、`Figure:` 2)
10. `app/cplat/prod/libsrc/cplat/console/README.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)
11. `app/cplat/prod/libsrc/cplat/runtime/README.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)
12. `app/cplat/docs/prompts/current/cplat-account-session-discussion.md` (報告 1 箇所 / 実態 `mermaid` 1、`CodeBlock:` 1 — [Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか))
13. `app/porter/docs/architecture.md` (報告 6 箇所 / 実態 `mermaid` 7、`Figure:` 6、キャプションなし 1 — [Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか))
14. `app/porter/docs/config.md` (報告 5 箇所 / 実態 `mermaid` 5、`Figure:` 5)
15. `app/porter/docs/sequence-bidir.md` (報告 4 箇所 / 実態 `mermaid` 4、`Figure:` 4)
16. `app/porter/docs/sequence-health.md` (報告 2 箇所 / 実態 `mermaid` 2、`Figure:` 2)
17. `app/porter/docs/sequence-lifecycle.md` (報告 6 箇所 / 実態 `mermaid` 6、`Figure:` 6)
18. `app/porter/docs/sequence-tcp.md` (報告 9 箇所 / 実態 `mermaid` 9、`Figure:` 9)
19. `app/porter/docs/sequence-data.md` (報告 9 箇所 / 実態 `mermaid` 10、`Figure:` 9、キャプションなし 1 — [Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか))
20. `framework/doxyfw/docs/extract-graphs.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1 + 生成例 `plantuml` 4)
21. `framework/doxyfw/docs/commands.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)
22. `framework/docsfw/docs/browser-reuse.md` (報告 2 箇所 / 実態 `mermaid` 2、`Figure:` 2)
23. `framework/docsfw/docs/chrome-wrapper.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)
24. `framework/docsfw/docs/rsvg-convert.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)
25. `framework/doxyfw/docs/doxybook2-decolorize-output-spec.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)
26. `framework/doxyfw/docs/doxygen-colorize-output-spec.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)
27. `app/doxygen-sample/prod/src/markdown_sample.md` (報告 1 箇所 / 実態 `mermaid` 1、`Figure:` 1)

## 合意・検証の記録

全質問 ([Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) 〜 [Q-020](#q-020-図の規範の強化をどの範囲で行うか)) の回答を受領し、残懸念が解消したため第 1 ゲートを再通過しました。最終仕様書 ([261114-doxygen-mermaid-migration-design.md](261114-doxygen-mermaid-migration-design.md)) は版 1 を差し戻して版 2 として作り直しており、現在は第 2 ゲートの承認待ちです。  
確認済みの事実と、第 2 ゲート後に実施する検証項目は明確に区別しています。

- **版 2 の作業履歴 (過去記録)**: 対象文書 (`app/general/docs/prompts/current/261114-doxygen-mermaid-migration-discussion.md`) の再生成 (`版: 2`) だけを実施し、現行ファイルと `doxygen --version` (`1.17.0`) の事実を確認した一方で、技術調査、コード・他文書の変更、最終仕様の作成、および Git 書き込み操作は行いませんでした。
- **版 3 の補強記録 (2026-11-14)**: 直前の調査結果を反映し、Doxygen Release 1.17.0 (2026-04-30) の公式変更履歴と `@mermaid` コマンド解説、ローカル `doxygen --version` (`1.17.0 (65a43c0aba45cc23b3ca11b6b5334d4eea931726)`)、`doxygen -g -` の `MERMAID_*` 設定および `MERMAID_RENDER_MODE = AUTO` の確認結果と簡潔なコメント例を追記しました。あわせて、実コメント解析、XML の要素と内容、`mmdc` なしの XML 出力、Doxybook2 `v1.6.1` と前後処理経由の Markdown 保存、および描画結果を未実証として分離し、作業 WBS 1.3 を部分進捗へ更新しました。本更新でも対象文書以外への変更、実ビルド・全体テスト、最終仕様の作成、および Git 書き込み操作は行っていません。
- **版 4 の調査・回答反映記録 (2026-11-14)**: [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) 〜 [Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか) に対するユーザー回答 (回答者: `Tetsuo Honda` / 回答日時: `2026-11-14 14:18:31 JST`) を受け付けて整合性検証 (**Assessment**) を記録し、残る技術調査および [Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) (B) に基づく 27 ファイル実態調査を実施しました。一時ディレクトリでの最小サンプル検証により、Doxygen `1.17.0` による `<mermaid>` / `<mermaidfile>` の XML 出力、`mmdc` なし (`MERMAID_RENDER_MODE = AUTO`) での無警告 XML 生成、および現行 `preprocess.sh` / Doxybook2 `v1.6.1` / `postprocess.sh` が `<mermaid>` と `<mermaidfile>` に未対応でタグ除去・図テキスト連結・`__flag__` の `&#95;&#95;flag&#95;&#95;` 化を起こす失敗事象を確認しました。あわせて `mmdc` `11.17.0` 単体 SVG 描画、および 27 ファイル中 25 ファイルがすでに `Figure:` 適用済みで `CodeBlock:` 残存は 1 箇所・キャプションなし Mermaid 図が 2 箇所・`extract-graphs.md` に `plantuml` 例 4 箇所がある事実を記録し、追加確認として [Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか) 〜 [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) を提示しました。`framework/doxyfw/bin_test/` 全件や複数 `app` ビルド・最終仕様作成・既存コード/他文書変更・Git 書き込み操作は行っていません。
- **版 5 の回答反映・第 1 ゲート通過記録 (2026-11-14)**: [Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか) (B)、[Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか) (A)、[Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか) (A)、[Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) (A) のユーザー回答 (`回答者: ユーザー / 回答日: 2026-11-14`) を受領・評価し、未回答・未確定論点をすべて解消して第 1 ゲートを通過しました。構文別の `mmdc` (`11.17.0`) 描画・Doxybook2 エスケープ挙動・CI/Doxyfile 設定を確認し、最終仕様書 ([261114-doxygen-mermaid-migration-design.md](261114-doxygen-mermaid-migration-design.md)) を作成して第 2 ゲート承認待ちへ移行しました。実装・既存正本・既存 Markdown の変更、単体テスト実行 (`bin_test` 基準値含む)、および Git 書き込み操作はまだ行っていません。
- **版 6 の第 2 ゲート差し戻し記録 (2026-11-14)**: 最終仕様書 (版 1) を対象コード (`preprocess.sh`、`postprocess.sh`、`extract-graphs.py`、`memory_lock.h`) と突き合わせてレビューし、ユーザーの指示 (指示者: `Tetsuo Honda` / 指示日時: `2026-11-14 19:22:59 JST` 以前の対話) により差し戻しました。`memory_lock.h` の置き換え図が現行図と一致しない点と、`@mermaid{caption="..."}` がキャプション指定の構文ではない点を、一時ディレクトリでの Doxygen `1.17.0` の実行結果とともに [第 2 ゲート レビューによる訂正事項](#第-2-ゲート-レビューによる訂正事項-確認日-2026-11-14) へ記録しました。判断が必要な 7 項目を [Q-011](#q-011-figure-行のキャプションに含まれる記号をどう扱うか) 〜 [Q-017](#q-017-継承グラフの矢印をどの記法で表すか) として提示し、作業 WBS と観点別の確認を回答待ちの状態へ戻しました。最終仕様書 (版 1) は変更しておらず、回答の受領と第 1 ゲートの再通過後に版 2 として作り直します。対象成果物の変更、単体テストの実行、Git 書き込み操作は行っていません。
- **版 7 の回答反映・試作検証記録 (2026-11-14)**: [Q-011](#q-011-figure-行のキャプションに含まれる記号をどう扱うか) 〜 [Q-017](#q-017-継承グラフの矢印をどの記法で表すか) のユーザー回答 (回答者: `Tetsuo Honda` / 回答日時: `2026-11-14 19:35:24 JST`、全問 A) を受領して **Assessment** を記録しました。最終仕様書の版 2 へ記載する内容を一時ディレクトリの試作で検証し、結果を [版 7 の試作検証で確認した事項](#版-7-の試作検証で確認した事項-確認日-2026-11-14) へ記録しました。この検証で、バックスラッシュによるキャプションのエスケープが `[` で出力を壊すこと、および図ブロックを 4 桁目に置くと前後の `@par` の節が `<verbatim>` になること (現行の `memory_lock.h` にも存在) が判明したため、[Q-018](#q-018-キャプションの記号をどの手段でエスケープするか) と [Q-019](#q-019-手動コメントの図ブロックをどの桁へ字下げするか) を追加しました。あわせて、Markdown 入力の Mermaid コード フェンスによる警告について、最終出力の図は崩れておらず影響が警告の記録だけである点を訂正しました。最終仕様書 (版 1) と対象成果物は変更しておらず、単体テストの実行と Git 書き込み操作も行っていません。
- **版 8 の回答反映記録 (2026-11-14)**: [Q-018](#q-018-キャプションの記号をどの手段でエスケープするか) (A) と [Q-019](#q-019-手動コメントの図ブロックをどの桁へ字下げするか) (A) のユーザー回答 (回答者: `Tetsuo Honda` / 回答日時: `2026-11-14 19:57:33 JST`) を受領して **Assessment** を記録し、[Q-011](#q-011-figure-行のキャプションに含まれる記号をどう扱うか) を解決へ更新しました。[Q-019](#q-019-手動コメントの図ブロックをどの桁へ字下げするか) の補足「plantuml も含めて、正しく図を扱うための規範を強化すること」から、字下げの規則を `@startuml` にも適用する方針を確定し、強化の範囲 (機械検査の追加の要否) を [Q-020](#q-020-図の規範の強化をどの範囲で行うか) として追加しました。最終仕様書 (版 1) と対象成果物は変更しておらず、単体テストの実行と Git 書き込み操作も行っていません。
- **版 9 の回答反映・第 1 ゲート再通過記録 (2026-11-14)**: [Q-020](#q-020-図の規範の強化をどの範囲で行うか) のユーザー回答 (回答者: `Tetsuo Honda` / 回答日時: `2026-11-14 20:04:35 JST`、対話セッションの選択ダイアログで A を選択) を転記して **Assessment** を記録し、[Q-019](#q-019-手動コメントの図ブロックをどの桁へ字下げするか) を解決へ更新しました。図ブロックの字下げ条件と自動生成グラフのラベル記法を一時ディレクトリで追加検証し、未回答・未解決の論点がないことを確認して第 1 ゲートを再通過しました。最終仕様書 ([261114-doxygen-mermaid-migration-design.md](261114-doxygen-mermaid-migration-design.md)) を版 2 として作り直し、第 2 ゲートの承認待ちへ移行しました。対象成果物の変更、単体テストの実行、Git 書き込み操作は行っていません。
- **作業実施・検証の記録 (2026-11-15 00:32:20 JST)**: 最終仕様書 (版 2) の第 2 ゲート承認 (承認者: `Tetsuo Honda`) を受けて作業を実施しました。
    - **基準値**: 変更前に `bin_test` の Python 4 ファイル (5、1、1、7 件) が全件成功することを確認し、`app/cplat` の `make doxy` の生成物と警告ファイル (`doxy_internal.warn` に `Text tag "mermaid" not recognised` が 2 件) を退避しました。実際の Doxyfile では、`memory_lock.h` の 2 関数の説明全体 (`@brief`、`@param` を含む) がコード ブロックとして出力されていました。
    - **実装**: 変換パイプライン (`extract-graphs.py`、`convert-mermaid-tags.py`、`preprocess.sh`、`postprocess.sh`、`makefile` のコメント)、`memory_lock.h`、`test_preprocess.py`、`test_postprocess.py` は主エージェントが実施しました。ユーザーの指示により、`check-doxygen-indent.py` と `test_check_doxygen_indent.py` と `check-doxygen-indent.md` (Opus)、`test_extract_graphs.py` (Sonnet)、図の規範と既存文書 (Sonnet)、`extract-graphs.md` と `doxybook2-mermaid.md` (Opus) をサブエージェントへ委譲しました。
    - **単体テスト**: 6 ファイル (`test_preprocess.py` 11 件、`test_postprocess.py` 7 件、`test_doxybook2_templates.py` 1 件、`test_materialize_group_members.py` 7 件、`test_extract_graphs.py` 37 件、`test_check_doxygen_indent.py` 11 件) が全件成功しました。
    - **再生成比較**: `app/cplat` の `make doxy` は終了コード `0` で、警告ファイルは出力されませんでした。```` ```plantuml ```` は 0 件、```` ```mermaid ```` は 3007 件で、すべてに `Figure:` 行があります。折り畳み 2995 件はすべて `Figure:` 行の後で閉じています。図のブロックを除いた本文の差分は、`memory_lock` に関する 5 ファイルだけでした。
    - **意味一致**: 自動生成グラフ 2995 件について、変更前の PlantUML と変更後の Mermaid のノード、エッジ、強調、キャプションを機械的に照合し、2980 件が一致しました。差が出た 15 件は、基準値の取得後に入ったコミット `714a8f29` (`eventlog.h` と `tracer.h` のインクルードを `inttypes.h` から `stdint.h` へ変更) によるもので、この 2 つのノードとエッジを除くとすべて一致しました。
    - **描画**: 代表図 8 件 (手動 2 図、インクルード元、インクルード先、コラボレーション図、呼び出し元、呼び出し先、関数の出力から生成した継承関係) が `mmdc` で終了コード `0`・構文エラーなしでした。
    - **検査**: `check-doxygen-indent.py` は `app/cplat/prod/include` で指摘なし、Python の import 経路の検査は変更した 7 ファイルで問題なしでした。
    - **最終仕様書との差**: `cplat-account-session-discussion.md` の `Figure:` への変更は、コミット `8b0ac003` で取り込み済みだったため差分がありません。`framework/doxyfw/docs/README.md` は文書一覧が自動生成のため変更していません。最終仕様書の例にあった見出し名「呼び出しグラフ」を、実装の名称「呼び出し先」へ訂正しました。
    - **残課題**: 4 件を [261114-doxygen-mermaid-migration-backlog.md](261114-doxygen-mermaid-migration-backlog.md) へ記録しました。BL-004 (インクルード グラフの見出し名) は、第 3 ゲートでのユーザーの指示により追記しました。Git 書き込み操作は行っていません。
