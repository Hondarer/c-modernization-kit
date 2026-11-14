<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新エージェント: agy / セッション ID: 0781d8d9-344b-4de0-9d84-13f456bc6886
下流文書: `261114-doxygen-mermaid-migration-design.md` (最終仕様)、`261114-doxygen-mermaid-migration-backlog.md` (残課題一覧)。未作成・削除済みの場合があります。
再開手順: agree-before-implement スキルを読み、本書の最新版と作業 WBS の `← **CURRENT**` から再開してください。
-->

# Doxygen 由来の PlantUML 図を Mermaid へ移行する検討

Doxygen と Doxybook2 を経由して Markdown へ出力している PlantUML 図を Mermaid へ移行するため、対象範囲、互換方針、検証基準の合意を得ます。

検討 ID: `doxygen-mermaid-migration-20261114` / 版: 5 / 状態: 第 1 ゲート通過 (第 2 ゲート承認待ち)

## 作業 WBS と進捗管理

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 前提収集と Q&A 実施
    - ✅ 1.1. 現行の図生成経路と正本文書の確認
    - ✅ 1.2. 論点抽出と質問 ([Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) 〜 [Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか)) の作成
    - ✅ 1.3. 未検証技術事項の実地調査と過去 27 ファイルの実態調査 (Doxygen 1.17.0 XML・`mmdc` なし・Doxybook2 `v1.6.1`/現行前後処理・`mmdc` 単体および構文別描画・27 ファイル突合を完了。`bin_test` 基準値と最終出力の描画比較は [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) (A)・[Q-005](#q-005-検証する-app-の範囲をどうするか) (A) に従い第 2 ゲート後に実施)
    - ✅ 1.4. ユーザー回答の受付と観点別確認 ([Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) 〜 [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) 全問回答受領・整合確認完了)
- ✅ 2. 第 1 ゲート: 残懸念の解消確認 (最終仕様作成へ)
- 🔄 3. 最終仕様書の作成と承認
    - ✅ 3.1. 最終仕様書 ([261114-doxygen-mermaid-migration-design.md](261114-doxygen-mermaid-migration-design.md)) の作成と受入条件の確定
    - 🚫 3.2. 第 2 ゲート: 最終仕様の承認受付 (ユーザー承認待ち) ← **CURRENT**
- ⏸️ 4. 承認された仕様に基づく移行作業
    - ⏸️ 4.1. 手動コメントの更新 (`memory_lock.h`)
    - ⏸️ 4.2. 設定と変換パイプラインの更新 (`Doxyfile`、`extract-graphs.py`、`preprocess.sh`、`postprocess.sh`、`input-filter.py`)
    - ⏸️ 4.3. 単体テストの更新・追加 (`framework/doxyfw/bin_test/` の承認範囲)
    - ⏸️ 4.4. 正本文書の図記法更新 (`framework/doxyfw/docs/`)
    - ⏸️ 4.5. [Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) (B) に基づく既存 Markdown の修正確認 ([Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか)・[Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか) の決定範囲)
    - ⏸️ 4.6. 承認範囲での再生成比較 (`app/cplat`)・警告確認・`text_style_jp.py` 検証・描画意味一致確認
    - ⏸️ 4.7. 受入条件の適合確認と残課題の整理
- ⏸️ 5. 第 3 ゲート: 成果物・検証結果の承認受付

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
| [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) | `doxyfw` 単体テストの実行範囲と基準値確認 | 回答済み (解決) | [Q-005](#q-005-検証する-app-の範囲をどうするか) |

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

全質問 ([Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) 〜 [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか)) の回答を受領し、残懸念が解消したため第 1 ゲートを通過しました。最終仕様書 ([261114-doxygen-mermaid-migration-design.md](261114-doxygen-mermaid-migration-design.md)) を作成済みであり、現在は第 2 ゲートの承認待ちです。  
確認済みの事実と、第 2 ゲート後に実施する検証項目は明確に区別しています。

- **版 2 の作業履歴 (過去記録)**: 対象文書 (`app/general/docs/prompts/current/261114-doxygen-mermaid-migration-discussion.md`) の再生成 (`版: 2`) だけを実施し、現行ファイルと `doxygen --version` (`1.17.0`) の事実を確認した一方で、技術調査、コード・他文書の変更、最終仕様の作成、および Git 書き込み操作は行いませんでした。
- **版 3 の補強記録 (2026-11-14)**: 直前の調査結果を反映し、Doxygen Release 1.17.0 (2026-04-30) の公式変更履歴と `@mermaid` コマンド解説、ローカル `doxygen --version` (`1.17.0 (65a43c0aba45cc23b3ca11b6b5334d4eea931726)`)、`doxygen -g -` の `MERMAID_*` 設定および `MERMAID_RENDER_MODE = AUTO` の確認結果と簡潔なコメント例を追記しました。あわせて、実コメント解析、XML の要素と内容、`mmdc` なしの XML 出力、Doxybook2 `v1.6.1` と前後処理経由の Markdown 保存、および描画結果を未実証として分離し、作業 WBS 1.3 を部分進捗へ更新しました。本更新でも対象文書以外への変更、実ビルド・全体テスト、最終仕様の作成、および Git 書き込み操作は行っていません。
- **版 4 の調査・回答反映記録 (2026-11-14)**: [Q-001](#q-001-手動コメントと自動生成グラフの双方を対象とするか) 〜 [Q-006](#q-006-描画エラーなしと意味一致を受入基準に含めるか) に対するユーザー回答 (回答者: `Tetsuo Honda` / 回答日時: `2026-11-14 14:18:31 JST`) を受け付けて整合性検証 (**Assessment**) を記録し、残る技術調査および [Q-004](#q-004-過去-27-ファイルの完了報告を今回再検証するか) (B) に基づく 27 ファイル実態調査を実施しました。一時ディレクトリでの最小サンプル検証により、Doxygen `1.17.0` による `<mermaid>` / `<mermaidfile>` の XML 出力、`mmdc` なし (`MERMAID_RENDER_MODE = AUTO`) での無警告 XML 生成、および現行 `preprocess.sh` / Doxybook2 `v1.6.1` / `postprocess.sh` が `<mermaid>` と `<mermaidfile>` に未対応でタグ除去・図テキスト連結・`__flag__` の `&#95;&#95;flag&#95;&#95;` 化を起こす失敗事象を確認しました。あわせて `mmdc` `11.17.0` 単体 SVG 描画、および 27 ファイル中 25 ファイルがすでに `Figure:` 適用済みで `CodeBlock:` 残存は 1 箇所・キャプションなし Mermaid 図が 2 箇所・`extract-graphs.md` に `plantuml` 例 4 箇所がある事実を記録し、追加確認として [Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか) 〜 [Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) を提示しました。`framework/doxyfw/bin_test/` 全件や複数 `app` ビルド・最終仕様作成・既存コード/他文書変更・Git 書き込み操作は行っていません。
- **版 5 の回答反映・第 1 ゲート通過記録 (2026-11-14)**: [Q-007](#q-007-残存する-codeblock-記載-1-箇所を-figure-へ修正するか) (B)、[Q-008](#q-008-キャプションなしの-mermaid-図-2-箇所へ-figure-を付与するか) (A)、[Q-009](#q-009-doxygen-由来の-mermaid-図へ-figure-キャプションを出力するか) (A)、[Q-010](#q-010-doxyfw-単体テストの実行範囲と基準値確認をどう扱うか) (A) のユーザー回答 (`回答者: ユーザー / 回答日: 2026-11-14`) を受領・評価し、未回答・未確定論点をすべて解消して第 1 ゲートを通過しました。構文別の `mmdc` (`11.17.0`) 描画・Doxybook2 エスケープ挙動・CI/Doxyfile 設定を確認し、最終仕様書 ([261114-doxygen-mermaid-migration-design.md](261114-doxygen-mermaid-migration-design.md)) を作成して第 2 ゲート承認待ちへ移行しました。実装・既存正本・既存 Markdown の変更、単体テスト実行 (`bin_test` 基準値含む)、および Git 書き込み操作はまだ行っていません。
