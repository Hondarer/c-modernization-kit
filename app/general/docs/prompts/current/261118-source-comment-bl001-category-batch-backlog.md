<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新日時: 2026-11-20 04:50:00 JST
最終更新エージェント: claude code (Opus 5.5) / セッション ID: session_01KJc53SNPk1ZBvJzF3wRqtE (BL-001 と BL-002 の「継続作業での経緯と対処」と下流文書は、継続案件のセッション session_01KJc53Rf57YTu7va3r3rvUU と session_01KJc53TxDHvQnb9NZpGEr1Q が更新)
稼働 OS: Windows
上流文書: `261118-source-comment-bl001-category-batch-discussion.md` (検討用 Markdown)、`261118-source-comment-bl001-category-batch-design.md` (最終仕様)、`261115-source-comment-bl001-deep-dive-backlog.md` (対策元の残課題一覧)。いずれも削除されている場合があります。
下流文書: この残課題一覧を対策する継続作業が開始された場合は、その検討用 Markdown・最終仕様・残課題一覧のパスを追記します。未作成の場合や、削除されている場合があります。BL-001 と BL-002 を対策する継続案件: `261119-void-discard-reason-and-failure-injection-overview.md` (全体管理文書)、`261119-void-discard-reason-and-failure-injection-discussion.md` (検討用 Markdown)、`261119-void-discard-reason-and-failure-injection-lightadr.md` (簡易 ADR)、`261119-void-discard-reason-and-failure-injection-design.md` (最終仕様)、`261119-void-discard-reason-and-failure-injection-backlog.md` (残課題一覧)。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱い、agree-before-implement の新しい検討用 Markdown から開始してください。継続作業でこのファイルの残課題を対策する場合は、このファイルや各項目の記載を削除せず、継続作業の開始時から各項目の状態と経緯を随時更新してください。
-->

# 残課題一覧 (バックログ)

## 案件の識別

案件名: `source-comment-bl001-category-batch`  
作成日: `2026-11-19`  
整理状態: 整理済み

この文書は、見送る決定がなされた問題点や、作業中に判明したが今回は対策しない問題点を記録します。  
第 3 ゲートの前に整理し、今回対策しなかった残課題だけを記録しています。  
対策元 backlog の BL-001 のうち、今回対処しない 57 件は、BL-003 へ転記しました (検討用 Markdown の作業 WBS 4.13)。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | 既存の `(void)` による戻り値の破棄に理由コメントを付ける | 検討時見送り | 中 | 対応済み |
| BL-002 | 失敗を注入できない呼び出しに、テストで失敗を起こす仕組みを追加する | 検討時見送り | 中 | 対応済み |
| BL-003 | 対策元 BL-001 のうち今回対処しない 57 件 | 過去 backlog からの転記 | 中 (項目により失敗処理・資源寿命への影響あり) | 未着手 |

Table: 残課題の一覧

## BL-001 既存の (void) による戻り値の破棄に理由コメントを付ける

### 概要と問題点

- **現状の事象**: `app/` 配下の `prod` の C ソースに、行の先頭が `(void)` の関数呼び出しが 582 箇所あります (2026-11-19 に検索。外部由来の `zlib` と `sqlite` の 56 箇所を含む)。理由コメントがない箇所が含まれますが、その数は数えていません。
- **本来あるべき姿**: `(void)` で戻り値を破棄する第一者のソースの呼び出しのすべてに、破棄する理由のコメントがある状態です。理由が短く、呼び出しの内容から読み取れる可能性が高い場合は後置コメント、説明が必要な場合は前置コメントとします。

### 経緯と見送り理由

- **摘出契機**: 前提検討時に見送り (検討用 Markdown の Q-019 の回答 B、2026-11-19)。
- **今回の見送り理由**: 今回の対象は、対策元 backlog の 16 件の範囲に限るためです。既存の箇所のすべてへ追加すると、16 件の範囲を大きく超えます。
- **継続作業での経緯と対処**: 2026-11-19 に、継続案件 `void-discard-reason-and-failure-injection` (検討 ID: `void-discard-reason-and-failure-injection-20261119`、全体管理文書: `261119-void-discard-reason-and-failure-injection-overview.md`) で検討を開始しました。継続案件の検索 (2026-11-19 08:26 JST) では、第一者のソースの該当箇所は 525 箇所 (96 ファイル) でした。2026-11-19 08:36 JST に、継続案件の Q-001 から Q-007 の回答で次の方針が決まりました。5 つの app の `prod` 配下をすべて対象とします。ほかの分類に当たる箇所の扱いも変更し、文脈から対処が一意に決まらない箇所は確認項目で合意します。エラー詳細を記録する内部関数の呼び出し (92 箇所) は、戻り値を返さない記録専用の内部関数へ置き換えます。`test` 配下は対象に含めず、規範に理由コメントを求めないことを定めます。ソースの変更は、この案件 (先行案件) の変更がコミットされた後に始めます。同日 08:50 JST に、対象の 540 箇所 (検索の条件に合わない形式を含む) の洗い出しを終えました。記録専用の関数への置き換えが 92 箇所、理由コメントがすでにある箇所が 168 箇所、理由コメントを追加または補う箇所が 224 箇所、扱いを変える候補と判断が要る箇所が 43 箇所、戻り値の破棄ではない箇所が 13 箇所です。扱いを変える箇所の一部は、継続案件の Q-009 から Q-020 で確認中です。 2026-11-19 22:05 JST までに、継続案件の Q-009 から Q-069 の回答で、次の方針が決まりました (継続案件の簡易 ADR の LADR-008 から LADR-015)。理由コメントは文型 `{理由}のため、戻り値は明示的に破棄する` で書き、既存の理由コメントも文型へ書き換え、文型を共通のコーディング規範に定めます。検査も `(void)` もない呼び出し (約 1,830 箇所) も、6 つの app (`override-sample` を含む) の `prod` 配下で 5 分類に当てはめます。戻り値が失敗を表さない関数は 5 分類の対象外とし、`POTR_TRACE` はマクロの定義の中で破棄し、標準出力へ結果を出すコマンドには終了の直前の検査を加えます。`cplat`、`porter`、サンプルの扱いを変える箇所の失敗時の動作は、箇所ごとに決めました。継続案件の第 1 ゲートは、検討用 Markdown の版 14 で通過し、最終仕様を作成しています。 2026-11-19 22:11 JST に継続案件の最終仕様が承認され、2026-11-20 04:50 JST に実装と局所の検証を終えました。6 つの app の `prod` 配下の `(void)` の呼び出し 1,565 箇所のすべてに文型の理由コメントを付け、エラー詳細を記録する内部関数の呼び出し 92 箇所を記録専用の内部関数へ置き換えました。検査も `(void)` もない呼び出しは、走査で検出できる範囲で、戻り値のない関数、マクロ、5 分類の対象外の関数だけです。扱いを変える 105 単位 (`cplat` の C-001 から C-040、`porter` の P-001 から P-048、サンプルの S-001 から S-017) を実装し、S-016 と S-017 を除くすべてに失敗時のテストを付けました。文型と、戻り値が失敗を表さない関数は、共通のコーディング規範に定めました。Windows/MSVC のビルドとテストは成功しており、Linux/GCC は CI で確かめます。S-016 と S-017 のテストがないことは、継続案件の残課題一覧の BL-001 に記録しました。

### 影響範囲とリスク

- **影響対象**: `app/` 配下の第一者の C ソース。行を折り返した呼び出しと、`test` 配下のソースは、上の件数に含まれていません。
- **放置した場合のリスク**: 今回の案件で規範に「理由コメントを常に求める」と定めた後も、既存の箇所が規範に合わない状態で残ります。破棄の理由を確認できない箇所では、失敗の見落としが残っている可能性があります。

### 推奨される対応策

- **恒久対応案**: app ごとに `(void)` の呼び出しを洗い出し、失敗の扱いの 5 分類に当てはめます。破棄のままでよい箇所には理由コメントを付け、ほかの分類に当たる箇所は扱いを変更します。動作が変わる箇所は、局所テストを追加します。
- **参考情報**: 検討用 Markdown の Q-017 (5 分類の定義)、Q-019 (理由コメントの条件)。件数の検索の条件は、`app/**/prod/**/*.c` に対する正規表現 `^\s*\(void\)\s*[A-Za-z_][A-Za-z0-9_]*\(` です。

## BL-002 失敗を注入できない呼び出しに、テストで失敗を起こす仕組みを追加する

### 概要と問題点

- **現状の事象**: 今回の案件で失敗時の動作を変更した 11 件のうち、9 件は失敗時の動作をテストで実行していません。既存の mock やテストの構成で、対象の呼び出しを差し替えられないためです。該当するのは、BCrypt の解放関数 (032)、`setvbuf` と `SetStdHandle` (043、048)、`bench-io` の `main` の `ferror` (121)、`onLoad()` の中の呼び出し (138)、サービスの説明の設定 `ChangeServiceConfig2U` (163)、`SetServiceStatus` (166)、`string-catalog-library-sample` の `main` の `fflush` と `ferror`、トレース出力 (170、172) です。失敗時のテストを追加したのは 110 (`traceFileCoverageTest`) と 184 (`structMetaSampleIoTest`) です。
- **163 の補足**: 最終仕様では `service-sampleTest` で失敗を注入する見込みでした。実装時に確認した結果、`service-sampleTest` は `svc_os_install()` をテスト側で置き換えており、`service-sample_windows.c` を組み込むテストはありません。
- **本来あるべき姿**: 失敗時だけ動作が変わる変更のすべてに、失敗を起こして動作を確認するテストがある状態です。

### 経緯と見送り理由

- **摘出契機**: 前提検討時に見送り (検討用 Markdown の Q-014 の回答 A、2026-11-19)。
- **今回の見送り理由**: 該当する変更は数行で、差し替えの仕組みを追加する変更のほうが大きくなるためです。仕組みを追加すると、呼び出しを差し替え可能な関数へ包む変更が製品のコードに入ります。
- **継続作業での経緯と対処**: 2026-11-19 に、継続案件 `void-discard-reason-and-failure-injection` (検討 ID: `void-discard-reason-and-failure-injection-20261119`、全体管理文書: `261119-void-discard-reason-and-failure-injection-overview.md`) で検討を開始しました。2026-11-19 08:36 JST に、継続案件の Q-008 の回答で、対象の確定と項目ごとの方法の検討は、この案件 (先行案件) の実装とテストの追加が終わった後に行うことが決まりました。 2026-11-19 11:51 JST に、先行案件のコミット (ルートは 276a4242) の後で 9 件の呼び出しと既存のテストを調べ、9 件すべてを対象としました。項目ごとの失敗を注入する方法は、2026-11-19 18:03 JST に継続案件の Q-021 から Q-027 の回答で決まりました (継続案件の簡易 ADR の LADR-013)。9 件すべてにテストを追加し、必要な mock は `framework/testfw` の `mock_libc` へ追加します。`setvbuf`、`SetStdHandle`、`SetServiceStatus`、BCrypt の解放関数は、`framework/testfw` の `mock_libc` に mock がありません。 2026-11-19 20:17 JST に、継続案件の Q-059 の回答で、継続案件で失敗時の動作を変える箇所のすべてにも、失敗時のテストを追加することが決まりました (継続案件の簡易 ADR の LADR-015)。実装の時点で注入できないと分かった箇所は、理由を付けて報告し、継続案件の残課題として記録します。継続案件の第 1 ゲートは、2026-11-19 22:01 JST に検討用 Markdown の版 14 で通過し、最終仕様を作成しています。 2026-11-19 22:11 JST に継続案件の最終仕様が承認され、2026-11-20 04:50 JST に実装と局所の検証を終えました。9 件すべてに失敗時のテストを追加しました。必要な mock (BCrypt の解放関数 3 つ、`setvbuf`、`SetStdHandle`、`SetServiceStatus` など) は `framework/testfw` へ追加しました。172 (トレース出力の失敗) は、出力の関数がライブラリの中から呼ばれ差し替えられないため、継続案件の Q-071 の回答により、`main` が直接呼ぶ `cplat_tracer_start` を差し替えて同じ失敗の経路を通す方法で確かめました。032、043、048、163、166 のテストは Windows だけで実行されます。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の `elevated_process.c`、`crypto_windows.c`、`bench-io.c`、`app/service-sample` の `service-sample_windows.c` (163、166)、`app/override-sample` の `libbase_lifecycle.c`、`app/string-catalog-sample` の `string-catalog-library-sample.c`。
- **放置した場合のリスク**: 失敗時の経路に誤りがあっても、テストでは検出できません。後の変更で失敗時の経路が壊れた場合も検出できません。

### 推奨される対応策

- **恒久対応案**: 項目ごとに、既存の mock (`mock_libc`、`mock_windows`、`mock_cplat`) へ関数を追加する方法と、`main` の本体を関数へ分けてテストから呼ぶ方法を比べ、製品のコードの変更が小さい方法を選びます。
- **参考情報**: 検討用 Markdown の Q-014。

## BL-003 対策元 BL-001 のうち今回対処しない 57 件

### 概要と問題点

- **現状の事象**: 値の選定理由、OS や外部 API の仕様との照合、分岐条件の選択について、現在の実装がそうなっていることを超えた根拠、または変更後の契約を確定できていない 57 件です。対策元 `261115-source-comment-bl001-deep-dive-backlog.md` の BL-001 (73 件) から、今回対処した失敗の扱いの 16 件 (032、043、044、045、047、048、110、121、137、138、163、166、168、170、172、184) を除いた残りです。
- **本来あるべき姿**: 各項目の根拠をコメントまたは正本文書へ記載するか、根拠がない場合は設計者の判断で値や分岐を見直した状態です。

### 経緯と見送り理由

- **摘出契機**: 過去 backlog からの転記。疑問の種類で 5 つの項目群に分け、今回は失敗の扱いの項目群 (G1) だけを対象とした (検討用 Markdown の Q-001、2026-11-18)。
- **今回の見送り理由**: 依頼の目安 (類似の 10 件から 20 件程度) に合わせて 1 つの項目群に絞ったためです。
- **項目群の内訳**: 容量、件数、しきい値の数値 16 件 (008、013、014、036、039、040、059、117、118、135、141、146、153、154、169、181)、待ち時間と周期の数値 7 件 (041、145、155、156、157、158、159)、OS や外部 API の仕様との照合 12 件 (020、021、022、028、030、031、034、042、049、099、100、102)、設計者の判断が必要な仕様と分岐 22 件 (003、004、007、012、015、017、029、037、063、084、098、105、107、116、124、128、129、132、134、143、173、182) です。

### 影響範囲とリスク

- **影響対象**: 下表の対象パス。
- **放置した場合のリスク**: 数値の選定理由が不明な項目は、保守時に変更できる範囲を判断できません。OS の仕様と照合していない項目は、仕様に合わない分岐が残っている可能性があります。

### 推奨される対応策

- **恒久対応案**: 項目群ごとに案件を分け、同じ判断の軸で対処します。数値の項目は履歴と仕様から根拠を探し、見つからない場合は維持、定数化、見直しのいずれかを設計者が決めます。OS や外部 API の項目は一次資料を確認し、ソースへ `see: <URL>` を残します。
- **参考情報**: 対策元の記載 (確認した事実・未確定点、次の調査・対策方針) を下表へそのまま転記しています。行番号は変更で増減するため、対象と関数名で位置を特定します。

| 枝番号 | 対象パス | 確認した事実・未確定点 | 次の調査・対策方針 |
| --- | --- | --- | --- |
| 003 | `app/calc/prod/src/cmd/calc/calc.c` | 乗算の x は現行仕様です。シェルのワイルドカード展開を避ける効果はありますが、それを元の採用理由と断定する根拠は未確定です。 | `calc` コマンドの使用例・ドキュメントと導入時の変更履歴を調べ、乗算記号に `*` ではなく `x` を採用した経緯 (シェル展開回避や既存仕様との互換) を確認し、現状維持と `*` 併用・変更の要否を判断します。 |
| 004 | `app/calc/prod/src/cmd/shared-and-static-calc/shared-and-static-calc.c` | 乗算の x は現行仕様です。シェルのワイルドカード展開を避ける効果はありますが、それを元の採用理由と断定する根拠は未確定です。 | 003 とあわせて `shared-and-static-calc` の利用手順と変更履歴を確認し、両コマンドで乗算演算子 `x` を統一して維持するか、`*` の受理や説明追記を行うか判断します。 |
| 007 | `app/cplat/prod/include/cplat/base/dll_exports.h` | IntelliSense 時だけ公開修飾を空にしています。解析負荷や誤診断を回避したという根拠は確認できておらず、推測でコメントを追加しません。 | `dll_exports.h` の導入・変更履歴と Visual Studio の IntelliSense 構成を確認し、`__INTELLISENSE__` 分岐を置いた対象エラーや診断の有無を特定して維持要否を判断します。 |
| 008 | `app/cplat/prod/include/cplat/crt/path.h` | 可変長マクロの数え上げリストが 16 であることは確認できます。16 という容量の選定理由は追加の仕様や履歴が必要です。 | `CPLAT_PATH_CONCAT` の呼び出し箇所と変更履歴を調べ、最大 16 引数を上限とした前提や引数超過時のコンパイル挙動を確認し、公開契約への上限明記または拡張要否を判断します。 |
| 012 | `app/cplat/prod/include_internal/cplat/base/error_message_internal.h` | 表示抑制の効果は分かりますが、このヘッダー固有の採用理由は履歴で追加確認します。 | `framework/doxyfw` の生成結果と `error_message_internal.h` の導入履歴を確認し、`@hideincludedbygraph` によるグラフ抑止の対象範囲と他ヘッダーとの方針統一を確認します。 |
| 013 | `app/cplat/prod/libsrc/cplat/argparser/argparser.c` | 28 はラベルが長い場合の折り返し後にも使用します。表示上の選択であり、28 を一意に導く式は確認できません。 | `argparser` のヘルプ整形テスト・仕様と変更履歴を確認し、表示開始列 28 の基準 (端末幅 80 桁との想定等) を特定して定数の維持または調整余地を判断します。 |
| 014 | `app/cplat/prod/libsrc/cplat/argparser/argparser.c` | 512 を超える場合に切り詰めを許す経路があります。容量を十分とする証明と、切り詰めを許す方針は別に必要です。 | `ARGPARSER_ERROR_MESSAGE_BYTES` の全利用箇所と変更履歴を調べ、最大メッセージ長の見積もり条件と 512 超過時に切り詰めを許容する出力契約を確認します。 |
| 015 | `app/cplat/prod/libsrc/cplat/base/error.c` | EAI_MEMORY 等を要因として区別しながら結果コードでは UNKNOWN に集約しています。詳細を保持する事実だけでは集約範囲の選択理由になりません。 | `cplat_internal_error_report_gai_error` の呼び出し元と `CPLAT_CAUSE_*` の定義契約を照合し、`EAI_MEMORY` や `EAI_AGAIN` 等を個別に分類する方式と `UNKNOWN` 集約を維持する方式を比較します。 |
| 017 | `app/cplat/prod/libsrc/cplat/clock/clock.c` | 秒差を分単位へ変換しています。歴史的な秒単位の UTC オフセットを 0 方向へ丸める方針は別途確認が必要です。 | `clock_utc_offset_minutes` の公開契約・単体テスト・変更履歴を確認し、歴史的タイム ゾーン等で秒単位の端数がある場合の 0 方向切り捨て、四捨五入、エラー化のいずれを仕様とするか確定します。 |
| 020 | `app/cplat/prod/libsrc/cplat/console/console.c` | new_fd が異なると FILE の元 FD と所有するハンドルが一致しません。elevated_process の同種処理は _dup2 で対応しています。Windows の再接続経路と所有権の検証が必要です。 | `reopen_crt_std_fd` と `elevated_process.c` の `_dup2` 実装、呼び出し元契約を比較し、`new_fd != expected_fd` を失敗返却または `_dup2` 再配置にする案と、Win32/CRT の結合局所テストを設計します。 |
| 021 | `app/cplat/prod/libsrc/cplat/console/console.c` | 既存コードに実機回帰の記録があります。FD 不一致の 020 と独立な CRT 原因を確認できていません。 | 既存コメントと変更履歴から Windows 実機環境・CRT バージョンによる `printf`/`fprintf` 失敗条件を再現・特定し、根本原因の記載または回避範囲の限定を行います。 |
| 022 | `app/cplat/prod/libsrc/cplat/console/console.c` | 出力を即時反映するための無バッファー化と入力の先読みは目的が異なります。入力バッファーを残す固有の方針は未確定です。 | `cplat_console_attach_parent` 後の `stdin` 利用経路 (`fgets`・プロンプト API) と変更履歴を確認し、`stdin` に `setvbuf(_IONBF)` を適用した場合と現行のまま維持した場合の挙動差を確認します。 |
| 028 | `app/cplat/prod/libsrc/cplat/crt/sys_stat.c` | ドライブと UNC の根を解析します。デバイス名前空間を入力契約から除外しているか、対応漏れかの判定が必要です。 | `path_root_prefix_len` の呼び出し元と `cplat/crt/path.h` のパス入力契約を確認し、`\\?\` 等のデバイス名前空間を契約上除外するか解析対象に追加するかを判断します。 |
| 029 | `app/cplat/prod/libsrc/cplat/crt/sys_stat.c` | 0755 は umask 適用前の許可です。利用側へ mode を渡させない API と、この既定値を採る設計方針の確認が必要です。 | `cplat_mkdir` の公開ヘッダー契約、利用箇所、Windows 側 `_wmkdir` との差異を照合し、固定値 `0755` とプロセス umask 依存を仕様化するか、権限引数付き API の要否を判断します。 |
| 030 | `app/cplat/prod/libsrc/cplat/crypto/crypto_linux.c` | 公開 API は NULL と長さ 0 を受理します。EVP のゼロ長 NULL の許否と、安全なダミー領域を渡す選択は区別します。 | OpenSSL の `EVP_DigestUpdate` 一次資料と `cplat_passphrase_to_key` の公開契約を照合し、長さ 0 時の NULL 渡しに対する仕様上の可否と、空文字列置換を維持する理由を確認します。 |
| 031 | `app/cplat/prod/libsrc/cplat/crypto/crypto_windows.c` | 公開 API の NULL・長さ 0 を BCrypt 入力へ変換しています。NULL が必須禁止だという断定はせず、文書のポインター契約と照合します。 | Microsoft の `BCryptHashData` 一次資料と `crypto.h` の契約を照合し、長さ 0 での NULL 入力に対する OS API の振る舞いと空配列置換の必要性を確認します。 |
| 034 | `app/cplat/prod/libsrc/cplat/net/endpoint_linux.c` | 種別を一つに固定すると重複結果を避けられます。service=NULL でアドレスだけ使う場合に DGRAM が STREAM より必要な根拠は未確定です。 | `cplat_ipv4_resolve` の `endpoint_linux.c` / `endpoint_windows.c` と変更履歴を確認し、両実装で `SOCK_DGRAM` と `SOCK_STREAM` の差異有無と選択理由を照合して方針を統一します。 |
| 036 | `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c` | 現在は PINNED_PROMPT_ROW_UNSET と命名済みです。9999 が全対応端末の有効行と衝突しない保証は得られていません。 | `PINNED_PROMPT_ROW_UNSET` の利用箇所と端末行数取得の型・上限を照合し、大画面端末との衝突回避のため負値や状態フラグ等へ変更する案と現行値の前提明記を比較します。 |
| 037 | `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c` | メモリ不足しか返さない内部ヘルパーです。共通結果コードの対象外とする根拠は規範との照合が必要です。 | `app/general/docs/coding-guideline.md` の共通結果コード適用外規則と `pinned_prompt_set_prompt` の呼び出し元を照合し、0/-1 維持の理由記載か `CPLAT_OK`/`CPLAT_ERR_OUT_OF_MEMORY` への統一かを判断します。 |
| 039 | `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c` | 初期容量 4 の後は倍増します。動的拡張できる事実は 4 の選定根拠とはなりません。 | 履歴配列の追加・拡張箇所と変更履歴を調べ、初期容量 4U を採った運用想定を確認し、定数化による前提記載または初期容量見直しの要否を判断します。 |
| 040 | `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c` | 256 は書式化バッファーの初期容量です。必要量に応じた拡張と、初期値の選定理由を分けます。 | 書式化バッファーの初期容量 256U を参照する処理と典型的な行長・端末幅を照合し、固定初期値の前提明記か設定・定数化の必要性を判断します。 |
| 041 | `app/cplat/prod/libsrc/cplat/runtime/abort.c` | 異常終了を担当しないスレッドの待機です。ポーリング間隔 1000 ms の必要性・許容遅延の根拠は未確定です。 | `cplat_abort` の異常終了待ちループとテスト・変更履歴を確認し、1000 ms 待機による CPU 負荷とプロセス終了遅延の許容条件を設計者に確認します。 |
| 042 | `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` | ハンドル継承と PROCESS_DUP_HANDLE のアクセス許可は別の制約です。既存の DuplicateHandle へのリンクだけで両者を保証しません。 | Microsoft の CreateProcess/ShellExecuteEx (`runas`) と整合性レベル (MIC) に関する一次資料を確認し、非継承と `PROCESS_DUP_HANDLE` 許可の双方を裏付ける出典と説明を補います。 |
| 049 | `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` | セッション 0 の非対話性は既存資料にあります。すべての runas の失敗を同じ原因と断定せず、対象とする UAC 操作を確認します。 | Microsoft の Session 0 Isolation と UAC プロンプト表示条件の一次資料を調べ、すべての `runas` 失敗に結び付けないよう対象環境とエラー条件を正確に切り分けます。 |
| 059 | `app/cplat/prod/libsrc/cplat/runtime/module.c` | 4096 と PLATFORM_PATH_MAX の相違は残ります。長い実行パスの取得方針と API の必要容量の扱いを確認する必要があります。 | `cplat_module_get_path` と `cplat_module_get_basename` の最大パス長契約、および `PLATFORM_PATH_MAX` との差を比較し、4096 固定の根拠確認または動的確保・`PLATFORM_PATH_MAX` 統一を判断します。 |
| 063 | `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c` | 定数領域は最大で行幅の 8 倍ですが、作業配列は行幅の 4 倍に基づきます。コンパイル入力の長さだけでは外部イメージに含まれる定数長を保証できず、最大長の証明が未完了です。 | `string_catalog_filter_format_constant` のエスケープ倍率と外部バイナリ イメージの最大文字列長検証を照合し、容量不足時の失敗伝播または動的確保・検証強化と局所テストを設計します。 |
| 084 | `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c` | 整数と異なり浮動小数点・ロケールに依存します。外部イメージの非有限値の検証と、書式化失敗を含む扱いを追加確認します。 | `string_catalog_filter_format_constant` における浮動小数点 (NaN/Inf を含む) とロケール依存・`snprintf` 負値戻り値の扱いを調べ、非有限値の検証および失敗伝達方針を確定します。 |
| 098 | `app/cplat/prod/libsrc/cplat/sync/sync_descriptor.c` | 8 byte を予約する形式は確認できますが、具体的に想定する将来機能は確認できません。用途を創作しません。 | 同期ディスクリプタのフォーマット仕様・テスト・変更履歴を確認し、offset 12-19 の予約領域に想定された将来拡張の有無を調査して仕様書へ反映します。 |
| 099 | `app/cplat/prod/libsrc/cplat/sync/sync_windows.c` | FILE_SHARE_DELETE が削除・名前変更を許す効果は明確です。ロック ファイルの削除を許す運用と同一性の維持が必要です。 | `cplat_interprocess_rwlock` のロック ファイル設計と複数プロセス開錠・削除手順を照合し、`FILE_SHARE_DELETE` を許す場合に inode 相当のファイル同一性をどう保つかの契約を確認します。 |
| 100 | `app/cplat/prod/libsrc/cplat/sync/sync_windows.c` | 全参加者が同じ非空領域を共有する必要があります。1 byte 以外との性能差や将来用途を推測せず、最小の競合領域という説明の妥当性を確認します。 | Win32 `LockFileEx` の一次資料と `sync_windows.c` の全ロック呼び出し元を確認し、先頭 1 byte 固定の妥当性 (0 byte 長ロックとの比較や全参加者一致要件) を照合します。 |
| 102 | `app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c` | 構造体のサイズ分を進めず後続プロパティを読むと、cursor とメタ情報がずれます。未対応スキーマとして打ち切るか、TDH でサイズを求めるかの設計が必要です。 | Microsoft TDH (`TdhGetPropertySize` / `EVENT_PROPERTY_INFO`) の一次資料を調べ、`PropertyStruct` を未対応スキーマとして打ち切るかサイズ算出で進めるかを設計し、局所テストを追加します。 |
| 105 | `app/cplat/prod/libsrc/cplat/trace/backends/eventlog/trace_eventlog.c` | 有効レベルには定義済みリソース ID を割り当て、範囲外は 0 です。未定義レベル自体を受理すべきかを含む入力契約の確認が必要です。 | `trace_eventlog.c` とメッセージ リソース定義 (`.mc`)、呼び出し元契約を照合し、定義外トレース レベルを 0 に落とすかエラー・既定値に倒すかの方針を確認します。 |
| 107 | `app/cplat/prod/libsrc/cplat/trace/backends/eventlog/trace_eventlog.c` | 識別子なしと書式化失敗が同じ判定になります。実際の作業配列で整数が収まるかと、異常時の省略方針を分けて確認します。 | 識別子の作業配列サイズと変換対象となる入力値の最大桁数を検証したうえで、書式化失敗時と識別子なし (0) を分離して扱うかを確認します。 |
| 116 | `app/cplat/prod/libsrc/cplat/win32/file_api.c` | UTF-16 単位数から UTF-8 バイト数への単純比例変換はできません。部分成功時の公開戻り値契約と、出力済みデータとの整合を確認する必要があります。 | `win32.h` の UTF-8 コンソール書き込み API 契約と呼び出し元を照合し、部分書き込み発生時に書き込めた UTF-8 バイト数を計算して返すか全失敗扱いにするかを確認します。 |
| 117 | `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c` | 有限寿命 5 を使う測定条件は記載済みですが、別の有効値より 5 を採る根拠は未確定です。 | `benchmark-method.md` と `bench-hashtable` の変更履歴を調べ、ベンチマーク条件 `lifetime = 5` を採った経緯 (再ハッシュ・トゥームス トーン測定条件等) を確認し、設定根拠を文書化します。 |
| 118 | `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c` | 既定の測定範囲 16384 は確認できますが、環境・時間上限からこの値を導く条件はありません。 | `benchmark-method.md` と測定実行時間・メモリ制約を確認し、既定上限 `16384u` の選定基準を文書化するか既定値見直しの要否を判断します。 |
| 124 | `app/cplat/prod/src/cmd/bench-io/bench-io.c` | 通常測定に失敗していれば huge 測定を省略する動作です。失敗後も継続する通常サイズ群との方針の違いは未確定です。 | `bench-io.c` の引数仕様・測定フローと変更履歴を確認し、通常測定で一部失敗した後に huge 測定を省略する設計意図と、他サイズ群とのポリシー一致を確認します。 |
| 128 | `app/cplat/prod/src/cmd/bench-io/bench_mmap.c` | int32_t を一度 uint32_t にすると符号拡張せず 32 bit の非負値として集計します。この集計方式を比較対象の正本が要求するか追加確認が必要です。 | `bench-io` の検査サム・集計仕様と変更履歴を確認し、負値を含む `int32_t` を符号拡張せず 32 bit 非負値として加算する計算要件の正本有無を調査します。 |
| 129 | `app/cplat/prod/src/cmd/bench-io/bench_report.c` | CPU 名のコロン直後をコピーし、末尾しか除去しません。区切りの空白まで値に残す必要性と、整形値としての契約を確認します。 | Linux `/proc/cpuinfo` の書式仕様と `bench_report` の CSV 列仕様を照合し、コロン直後の空白をトリミングする変更と既存出力互換の維持を比較して局所テストを設計します。 |
| 132 | `app/cplat/prod/src/cmd/bench-io/bench_stdio.c` | 128 と同じ集計式です。mmap と stdio の一致は確認できますが、符号なし 32 bit として集計する選択の根拠とは区別します。 | 128 とあわせて `bench_mmap.c`・`bench_stdio.c` の参照仕様を確認し、符号なし 32 bit 変換を集計要件として仕様化するか符号付き集計へそろえるかを確定します。 |
| 134 | `app/cplat/prod/src/cmd/bench-io/bench_timer.c` | 偶数件で上側中央値を返す実装です。測定方法には中央値とだけ記載されており、上側の標本値を採る方針は未確定です。 | `bench-io` の `benchmark-method` 等と統計仕様を照合し、偶数標本数で上側 1 値 (`trial_count / 2U`) を採る仕様か中央 2 値の平均へ変更するかを判断します。 |
| 135 | `app/cplat/prod/src/cmd/compress-cli/compress-cli.c` | 圧縮方式ごとの必要容量の上界と対応していません。2 倍・下限 256 の十分性を仮定せず、容量照会または再確保方式との比較が必要です。 | `cplat_compress` の公開契約 (圧縮方式ごとの最大出力サイズ照会や `BUFFER_TOO_SMALL` 再試行) と `compress-cli.c` の 2 倍・下限 256 確保を比較し、根拠確認または再確保方式への変更を検討します。 |
| 141 | `app/porter/prod/libsrc/porter/api/potr_service_open.c` | 送信キューは count を持ち、head/tail だけで空満を区別する方式ではありません。空きスロットが一つ必要だから下限 2、という説明は採用できません。 | `app/porter` の送信キュー実装 (`count` 管理) と機能仕様・変更履歴を照合し、下限 2 を課す根拠の有無を調べて仕様明記または下限 1 への見直しを判断します。 |
| 143 | `app/porter/prod/libsrc/porter/api/potr_service_open.c` | セッション生成で path ごとの原子変数を格納します。単なる重複ゼロ初期化なのか、原子操作による初期値設定を求めるラッパー契約なのかを追加確認します。 | `cplat_atomic_u64` の型定義・初期化契約 (`cplat_atomic_init` 等) と `potr_service_open.c` の `memset` ゼロ初期化を照合し、型契約上の必須初期化か冗長な再初期化かを確認します。 |
| 145 | `app/porter/prod/libsrc/porter/thread/potr_recv_thread.c` | 現在も health_timeout/3 の下限 100 ms、無効時 500 ms が残っています。reorder_timeout 等でさらに短縮する経路もあり、値の選定根拠は未確定です。 | `potr_recv_thread.c` の各タイムアウト (health/reorder) 短縮条件と porter 機能仕様・変更履歴を照合し、下限 100 ms・規定 500 ms の応答性／CPU 負荷上の基準を確認します。 |
| 146 | `app/porter/prod/libsrc/porter/thread/potr_send_thread.c` | 現在も buf_full_suppress_cnt > 10 が残っています。ログ抑制の目的は分かりますが、10 の選定根拠は未確定です。 | `potr_send_thread.c` のログ抑制カウンターしきい値 10 と変更履歴 (261109 引き継ぎ記録含む) を確認し、しきい値 10 の根拠または定数定義・変更可否の明記を判断します。 |
| 153 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_linux.c` | listen の 128 は保留接続の上限です。想定接続負荷と OS 上限からこの値を導く根拠は未確定です。 | `tcpServer` サンプルの想定クライアント数・Linux `somaxconn` 仕様と変更履歴を確認し、バックログ 128 の根拠明記または `SOMAXCONN` 等の利用可否を判断します。 |
| 154 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` | 153 の Windows 側です。Linux の既定値・過去の SOMAXCONN を、Windows で 128 を選んだ根拠にしません。 | 153 とあわせて Winsock `listen` (`SOMAXCONN`) 仕様と Windows 側サンプルの想定接続数を照合し、128 を採った経緯と OS 差の扱いを整理します。 |
| 155 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` | 10 ms の再試行間隔は残ります。遅延と CPU 負荷の要求または測定根拠が必要です。 | `tcpServer_windows.c` の再試行ループにおける 10 ms スリープの根拠と、CPU 使用率・応答遅延への影響を設計者へ確認し、定数化または方式比較を行います。 |
| 156 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` | select の 10 ms は残ります。155 と同値という事実だけで最適性を説明しません。 | `tcpServer_windows.c` の `select` タイムアウト 10 ms (10000 us) の応答・終了待機要件を確認し、155 の待機と独立した根拠の有無を照合します。 |
| 157 | `app/service-sample/prod/src/cmd/service-sample/service-sample_linux.c` | RestartSec=5 と WatchdogSec=30 は運用値です。監視通知の周期や許容停止時間と対応する要求が必要です。 | `app/service-sample` の systemd 連携仕様と運用要件を確認し、`RestartSec=5`・`WatchdogSec=30` およびウォッチドッグ通知周期との整合性を確認します。 |
| 158 | `app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c` | 終了待機 5000 ms の選定には、監視待機と終了通知を含めた停止上界の確認が必要です。 | イベント監視スレッドの終了待機上界と systemd 停止タイムアウト・`SVC_EVENTS_JOIN_TIMEOUT_MS` の関係を調べ、5000 ms の設定根拠を特定します。 |
| 159 | `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c` | dwWaitHint は完了期限の保証とは異なります。3000/5000 ms を各状態へ設定する運用方針は未確定です。 | Microsoft SCM (`SERVICE_STATUS.dwWaitHint`) の一次資料と Windows サービス停止・起動の実処理時間を照合し、状態別の 3000/5000 ms 設定方針を確認します。 |
| 169 | `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c` | 4096 > 1024 による上限超過メッセージの例です。この値でなければならない根拠はなく、例示値として扱う方針の確認が必要です。 | `string-catalog-library-sample.c` のエラー メッセージ表示例とテスト期待出力を確認し、4096/1024 をデモ用の例示値と明記するかライブラリ上限値と連動させるか判断します。 |
| 173 | `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c` | DEBUG は通常の全レベルを表示するしきい値です。全レベルを見せることがこのサンプルの要件であるかを確認します。 | `string-catalog-library-sample` の仕様・README と出力期待値を確認し、全レベル出力 (`CPLAT_TRACE_LEVEL_DEBUG`) がサンプル要件であるか確定します。 |
| 181 | `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c` | count*2 は占有率を下げますが、検索性能の要求や実測から 2 倍を選ぶ根拠は未確定です。 | `cplat_hashtable` の負荷率・ハッシュ衝突と `struct-meta-gen` の生成規模・検索性能要件を調べ、`count * 2U` を採った実測・基準の有無を確認します。 |
| 182 | `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c` | lifetime は使用中レコードの期限ではなく削除済みレコードの寿命です。型名索引を永続化するから無限、という説明だけでは選択を正当化できません。 | `cplat_hashtable` の `lifetime` 契約 (削除済みレコード寿命と永続化イメージでの意味) を踏まえ、`INFINITE` 指定の妥当性と有限値との差を整理します。 |

Table: 今回対処しない 57 件
