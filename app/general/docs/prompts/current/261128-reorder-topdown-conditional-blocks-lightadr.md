<!--
生成元: agree-before-implement スキル
文書種別: 簡易 ADR (lightadr)
最終更新日時: 2026-11-28 08:38:00 JST
最終更新エージェント: agy (セッション ID: 34911232-f261-4dd8-bdce-4b9a89670d15)
稼働 OS: Linux (WSL)
上流文書: `261128-reorder-topdown-conditional-blocks-overview.md` (全体管理文書)、`261128-reorder-topdown-conditional-blocks-discussion.md` (検討用 Markdown)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書の全体管理文書が存在する場合は、その再開手順に従って進行位置を確認します。存在しない場合は、この文書を単体の決定事項の記録として扱います。この文書は正式な ADR ではありません。正式な ADR への転記はユーザーが必要時に行うため、エージェントは依頼なしに転記や正式な ADR の作成・更新を行わないでください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# reorder_c_topdown.py の条件付きコンパイル分岐の扱い修正 簡易 ADR

ファイル名: `261128-reorder-topdown-conditional-blocks-lightadr.md`

この文書は、検討で得られた事実、ユーザーの選択とコメント、およびそれらに基づく決定事項を、決定 1 件ごとにまとめた簡易 ADR (Architecture Decision Record: 設計判断の記録) です。正式な ADR ではなく、対案の比較や決定の影響の評価は記載していません。  
各決定の「決定」は、方針が変わるたびに書き換え、常に最新の決定だけを示します。変更の推移は「経緯」に時系列で記録します。

## 決定事項一覧

| 決定 ID | 題名 | 最終更新日 | 源泉 |
| --- | --- | --- | --- |
| [LADR-001](#ladr-001-継続案件の立ち上げと前提合意による進行方針) | 継続案件の立ち上げと前提合意による進行方針 | 2026-11-28 | 初期依頼 (2026-11-28 対話) |

## LADR-001 継続案件の立ち上げと前提合意による進行方針

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

`261125-topdown-function-order-backlog.md` の BL-006 (優先度 高) として記録された、`app/general/bin/reorder_c_topdown.py` による条件付きコンパイル分岐の誤移動と `--verify` の検出漏れを解消する必要があります。あわせて、関連する BL-004 (複数分岐に完全な実装を持つ 2 ファイルと `check_c_topdown.py` の判定) の扱いも含め、実装前に設計方針を合意する進め方を確定します。

### 得られた事実

- `app/general/bin/reorder_c_topdown.py` は、`find_regions` (139〜154 行目) でファイル全体を 1 つの `#if` で囲む場合だけ分岐内へ降り、分岐外にも関数定義がある場合はファイル全体を 1 つの範囲として処理する。
- 同ツールの `classify` (313〜332 行目) は、関数定義を含む部分的な `Group` に非関数要素 (`#include`、`#define`、型定義、ファイル内変数、`#pragma`、プロトタイプ宣言) が混在していても、定義がすべて `static` なら `Unit("helper")`、すべて非 `static` なら `Unit("main")` として分岐全体を 1 単位で移動する。
- 同ツールの `make_units` (260〜261 行目) と `split_group` (268〜308 行目) は、`static` 関数と非 `static` 関数が混在する部分的な `Group` について、`static` 関数だけを `Unit("helper")` へ分け、非関数要素を非 `static` 関数とともに `Unit("main")` 側へ残す。このため、生成済みプロトタイプ宣言や入口関数 (`main`、`wmain`) の後ろへ非関数要素が移動する。
- 同ツールの `verify_texts` (509〜548 行目) は `Counter((element.context, element.tokens))` によるマルチ セット比較であり、要素の前後関係を検査しない。
- 作業用一時ディレクトリに作成した最小再現 C ファイル 2 件 (`repro_helper_mixed.c`、`repro_split_mixed.c`) では、変換前は `gcc -std=c17 -Wall -Wextra -Werror -fsyntax-only` に合格し、`reorder_c_topdown.py --in-place`、`--verify`、`check_c_topdown.py` もいずれも終了コード 0 で合格したにもかかわらず、変換後は型未定義・暗黙宣言・未宣言識別子・型競合でコンパイルに失敗した。
- 2026-11-28 時点の `app/*/prod/` 対象 259 ファイル (第一者管理 `.c`) を走査した結果、ファイル全体が 1 つの `#if` で囲まれるファイルは 41 ファイル (うち複数分岐に関数定義を持つ BL-004 該当は `console.c` と `etw-viewer.c` の 2 ファイル)、部分的な `#if` 分岐に関数定義を含むファイルは 18 ファイル、そのうち現在も部分分岐内に関数定義と非関数要素が混在するファイルは 2 ファイル (`app/cplat/prod/libsrc/cplat/runtime/process.c:688`、`app/cplat/prod/libsrc/cplat/runtime/sym_loader_resolve.c:269`) であった。
- 2026-11-28 時点の `python3 app/general/bin/check_c_topdown.py` の全対象実行結果は、対象 259 ファイル、エラー 7 件 (`console.c` 5 件、`etw-viewer.c` 2 件)、警告 0 件、終了コード 1 であった。

### 決定

`261125-topdown-function-order-backlog.md` の BL-006 (および関連する BL-004 の扱い確認) について、案件名 `261128-reorder-topdown-conditional-blocks` として `app/general/docs/prompts/current/` に検討文書一式を作成し、`agree-before-implement` スキルの手順で進める。フェーズ 1 ではソース コード、ツール、既存文書 (対策元 backlog の BL-006・BL-004 への継続作業開始の追記を除く) を変更せず、`make` および Git の書き込み操作も行わず、Q&A の提示後にユーザー回答の受付で停止する。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 08:33 | 初期依頼 (対話) | 新案件 `261128-reorder-topdown-conditional-blocks` を検討段階 (フェーズ 1) まで作成 | 元の backlog の BL-006 (関連 BL-004) について、`reorder_c_topdown.py` の現状実装の特定、scratchpad での最小再現、`app/` 配下の概数調査を行い、overview・discussion・lightadr・backlog を作成してユーザー回答の受付を `← **CURRENT**` として停止する。対策元 backlog の BL-006 (扱うなら BL-004) の継続作業での経緯と対処への追記のみ許可し、Git 書き込みと `make` は禁止 | 新規に決定 |
