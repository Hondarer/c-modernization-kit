<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新日時: 2026-11-28 08:38:00 JST
最終更新エージェント: agy (セッション ID: 34911232-f261-4dd8-bdce-4b9a89670d15)
稼働 OS: Linux (WSL)
上流文書: `261128-reorder-topdown-conditional-blocks-overview.md` (全体管理文書)、`261128-reorder-topdown-conditional-blocks-design.md` (最終仕様)、`261128-reorder-topdown-conditional-blocks-discussion.md` (検討用 Markdown)、`261128-reorder-topdown-conditional-blocks-lightadr.md` (簡易 ADR)。いずれも削除されている場合があります。
下流文書: この残課題一覧を対策する継続作業が開始された場合は、その全体管理文書・検討用 Markdown・最終仕様・残課題一覧のパスを追記します。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい全体管理文書と検討用 Markdown から開始してください。継続作業によってこのファイルの残課題を対策する場合は、このファイルや各項目の記載を削除することなく、継続作業の開始時 (全体管理文書と検討用 Markdown の作成時) から検討・設計・実装・整理の各段階において、このファイルのメタ情報、各項目の状態、および経緯と対処を随時更新することを原則とします。また、フェーズ分けした対策で対処できていない項目がある場合は、当該継続作業の backlog にも必ず転記し、最新作業の backlog を見れば残件が把握できるようにしてください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 残課題一覧 (バックログ)

ファイル名: `261128-reorder-topdown-conditional-blocks-backlog.md`

## 案件の識別

案件名: `261128-reorder-topdown-conditional-blocks`  
作成日: `2026-11-28`  
整理状態: 随時追記中

この文書は、本案件の検討・設計・実装の過程で見送る決定がなされた問題点や、作業中に判明した範囲外の気付きを随時記録する残課題一覧です。第 3 ゲートの前に再構築して整理し、当該作業内で解消・容認した項目を除外します。また、対策元 `261125-topdown-function-order-backlog.md` のうち本案件で未対処のまま残る項目 (BL-001、BL-002、BL-003、BL-005、および Q-004 で見送りとなった場合の BL-004) は、整理時にこの文書へ転記して最新の残件一覧とします。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | 現行 `app/cplat` に残存する部分条件分岐内の非関数要素混在 2 ファイルの分離検討 | 作業中摘出 | 低 | 未着手 (Q-005 で扱い確認中) |
| BL-002 | 部分条件分岐内の既存 `static` プロトタイプ宣言に対する重複宣言生成の潜在問題 | 作業中摘出 | 低 | 未着手 (Q-001・Q-002 で扱い確認中) |

## BL-001 現行 app/cplat に残存する部分条件分岐内の非関数要素混在 2 ファイルの分離検討

### 概要と問題点

- **現状の事象**: 2026-11-28 の全対象走査 (259 ファイル) で、すでにトップ ダウン規則に適合している `app/cplat` の 2 ファイルに、部分的な `#if` 分岐内での関数定義と非関数要素の混在が残存していることを確認した。
    - `app/cplat/prod/libsrc/cplat/runtime/process.c` (688 行目の `#if defined(PLATFORM_LINUX)` 内に 32 個の `static` 関数と、983〜985 行目の `#ifndef NAME_MAX / #define NAME_MAX 255 / #endif`)
    - `app/cplat/prod/libsrc/cplat/runtime/sym_loader_resolve.c` (269 行目の `#if defined(PLATFORM_LINUX)` 内に 1 個の `static` 関数 `wait_for_entry_lock_initialization` と、283 行目の関数ポインター変数 `s_entry_lock_wait_hook`)
- **本来あるべき姿**: 条件付きコンパイル分岐内のマクロ定義やファイル内変数が関数定義と分離されており、将来これらのファイルへ `reorder_c_topdown.py` を適用する場面でも手動分離が不要になっている。

### 経緯と見送り理由

- **摘出契機**: フェーズ 1 の事前調査 (2026-11-28、`app/*/prod/` 259 ファイルの走査)
- **今回の見送り理由**: [Q-005](261128-reorder-topdown-conditional-blocks-discussion.md#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い) で本案件のスコープに含めるか確認中。すでに規則適合済みかつビルド・テスト合格済みのため、本案件で `app/cplat` サブモジュールを変更しない場合は残課題として残すか、仕様上問題なしとして整理時に除外する。
- **継続作業での経緯と対処**: なし

### 影響範囲とリスク

- **影響対象**: `app/cplat/prod/libsrc/cplat/runtime/process.c`、`app/cplat/prod/libsrc/cplat/runtime/sym_loader_resolve.c`
- **放置した場合のリスク**: 現時点では両ファイルとも `check_c_topdown.py` に合格しているため直ちに影響はない。将来これらのファイルに未宣言 `static` 関数を追加して `reorder_c_topdown.py` を実行すると、混在検出により自動並べ替えが停止して手作業が必要になる。

### 推奨される対応策

- **恒久対応案**: `app/cplat` を変更する別案件や保守の機会に、`process.c:983-985` の `NAME_MAX` 補完定義をファイル先頭の `#if defined(PLATFORM_LINUX)` 定義部へ移し、`sym_loader_resolve.c:283` の `s_entry_lock_wait_hook` を前方宣言後の変数区分へ移す。
- **参考情報**: [Q-002](261128-reorder-topdown-conditional-blocks-discussion.md#q-002-混在と判定する非関数要素と入れ子分岐の境界)、[Q-005](261128-reorder-topdown-conditional-blocks-discussion.md#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い)

## BL-002 部分条件分岐内の既存 static プロトタイプ宣言に対する重複宣言生成の潜在問題

### 概要と問題点

- **現状の事象**: `app/general/bin/reorder_c_topdown.py` の `classify` (315 行目) は関数定義を含む `Group` を丸ごと `Unit("helper")` または `Unit("main")` に分類し、`declared_names` (334〜342 行目) は `definition_units` 内の要素を既宣言一覧から除外している。このため、関数定義と同じ部分分岐内に既存の `static` プロトタイプ宣言が書かれているファイルへ `reorder_c_topdown.py` を適用すると、元の宣言が分岐内に残ったまま先頭側へ重複したプロトタイプ宣言が生成される。
- **本来あるべき姿**: 部分分岐内の既存 `static` プロトタイプ宣言が誤って重複生成されない (混在分岐として停止するか、既存宣言を適切に認識・移動する)。

### 経緯と見送り理由

- **摘出契機**: フェーズ 1 の実装解析 (2026-11-28、`reorder_c_topdown.py:315, 334-342` の読解)
- **今回の見送り理由**: [Q-001](261128-reorder-topdown-conditional-blocks-discussion.md#q-001-混在する条件付きコンパイル分岐を検出したときの-reorder_c_topdownpy-の基本動作)・[Q-002](261128-reorder-topdown-conditional-blocks-discussion.md#q-002-混在と判定する非関数要素と入れ子分岐の境界) で混在分岐の扱いが確定し、本案件内で解消された場合は第 3 ゲート前の整理時にこの項目を除外する。
- **継続作業での経緯と対処**: なし

### 影響範囲とリスク

- **影響対象**: `app/general/bin/reorder_c_topdown.py`
- **放置した場合のリスク**: 同一翻訳単位内に同じ `static` 関数のプロトタイプ宣言が 2 箇所出力され、保守時に片方だけ修正されて不整合を招くおそれがある。

### 推奨される対応策

- **恒久対応案**: [Q-002](261128-reorder-topdown-conditional-blocks-discussion.md#q-002-混在と判定する非関数要素と入れ子分岐の境界) で関数定義を含む部分分岐内のプロトタイプ宣言も混在と判定して停止するか、自動分割時に既存宣言を `proto` として正しく抽出する。
- **参考情報**: `app/general/bin/reorder_c_topdown.py:313-342`、[Q-001](261128-reorder-topdown-conditional-blocks-discussion.md#q-001-混在する条件付きコンパイル分岐を検出したときの-reorder_c_topdownpy-の基本動作)、[Q-002](261128-reorder-topdown-conditional-blocks-discussion.md#q-002-混在と判定する非関数要素と入れ子分岐の境界)
