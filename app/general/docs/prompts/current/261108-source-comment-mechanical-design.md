<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: codex
上流文書: `261108-source-comment-mechanical-discussion.md` (検討用 Markdown)。削除されている場合があります。
下流文書: `261108-source-comment-mechanical-backlog.md` (残課題一覧、未作成)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# ソース コメント残課題の機械的整形とタグ桁揃え 最終仕様

ファイル名: `261108-source-comment-mechanical-design.md`  
検討 ID: `source-comment-mechanical-20261108`  
合意版: 版 2 (第 1 ゲート完了)  
最終仕様の版: 版 1  
作成日: 2026-11-08 (JST)  
承認状態: 承認済み (ユーザーのチャット「承認」、2026-11-08 JST)

## 先行対処の範囲を意味不変の機械的整形に限定する

過去のソース コメント案件の残課題一覧 ([261107-source-comment-backlog.md](../past/261107-source-comment-backlog.md)) に記録された全 22 項目のうち、仕様判断を伴わず既存の書式規則だけで完結する BL-007 と BL-005 を先行対処の対象とします。  
契約、識別子、型、式、条件分岐、C のトークン列、文字列リテラル、およびコメント本文の文面を一切変更せず、差分を空白と改行および Doxygen タグの桁揃えに限定します。

| 対象項目 | 対処内容 | 変更の境界 |
| --- | --- | --- |
| BL-007 | 指定 10 ファイルへの既存 `.clang-format` 設定の適用 | 空白と改行の整形のみとし、`.clang-format` の設定変更や対象ファイルの拡大は行わない |
| BL-005 | `app/porter/prod/libsrc/porter/thread/thread_recv_window.h` の Doxygen タグと継続行の整列 | タグ開始位置から 16 桁幅の空白調整と継続行の字下げ整列のみとし、コメント本文・タグ種別・引数名は変更しない |

Table: 今回の先行対処項目と変更の境界

範囲外とする項目は次のとおりです。

- BL-001〜BL-004、BL-006、BL-008〜BL-022 の計 20 項目 (意図の確認、範囲検査や不具合の事実確認、命名変更、修飾子追加、共通化、生成元の改修を要するため、未対処項目として最新作業の残課題一覧へ引き継ぎます)
- `.clang-format`、`.editorconfig`、`.gitattributes` などのスタイル設定ファイルの変更
- Doxygen 生成物 (`docs/doxybook2*/` など) の直接編集
- 動作テストの新設、および framework や複数 app にまたがる全体テストの実行

> [!IMPORTANT]
> 本仕様は第 2 ゲートで承認済みです。対象ファイルの変更と検証は、本仕様の範囲で実施します。

## BL-007: 既存設定による 10 ファイルの clang-format 整形

[ソース スタイル維持ガイドライン](../../source-style-guideline.md) に従い、元の残課題一覧の BL-007 に列挙された次の 10 ファイルへ既存の `.clang-format` 設定を適用します。

| 通番 | 対象ファイル パス | 所属 app |
| --- | --- | --- |
| 1 | `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c` | `app/struct-meta` |
| 2 | `app/cplat/prod/include/cplat/runtime/process_priority_linux.h` | `app/cplat` |
| 3 | `app/cplat/prod/include/cplat/runtime/process_priority_windows.h` | `app/cplat` |
| 4 | `app/cplat/prod/include/cplat/trace/tracer.h` | `app/cplat` |
| 5 | `app/cplat/prod/libsrc/cplat/runtime/abort.c` | `app/cplat` |
| 6 | `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c` | `app/cplat` |
| 7 | `app/cplat/prod/libsrc/cplat/runtime/process_priority_windows.c` | `app/cplat` |
| 8 | `app/cplat/prod/libsrc/cplat/runtime/stack_limit_linux.c` | `app/cplat` |
| 9 | `app/porter/prod/include_internal/porter/potr_context.h` | `app/porter` |
| 10 | `app/porter/prod/libsrc/porter/api/potr_peer_disconnect.c` | `app/porter` |

Table: BL-007 の整形対象 10 ファイル

実施手順と制約は次のとおりです。

1. 作業開始時に、使用する `clang-format` の版と参照する `.clang-format` 設定を確認して記録します。
2. 上記 10 ファイルに対して `clang-format --dry-run -Werror` を実行し、現時点で残存している整形差分を検査します。差分が存在しないファイルは変更不要として記録し、ファイルを更新しません。
3. 差分が存在するファイルは、直接上書きせずセッション用の一時コピーへ `clang-format` を適用し、変更内容が空白と改行のみであり、C のトークン列、文字列リテラル、コメント本文の意味が不変であることを事前に確認します。
4. 事前確認で問題がなければ、対象ファイルへ `clang-format -i` を適用します。`.clang-format` の設定変更や `// clang-format off` などの除外指定は追加せず、整形結果を手作業で変更したり元の記述へ戻したりしません。

## BL-005: thread_recv_window.h の Doxygen タグ 16 桁揃え

[app 向け Doxygen コメント](../../doxygen-comment-guideline.md) の「タグの桁揃え」に従い、`app/porter/prod/libsrc/porter/thread/thread_recv_window.h` 内の関数 Doxygen コメントのタグ幅と継続行を整列します。

| 関数名 | 現状の整列状態 | 今回の処置 |
| --- | --- | --- |
| `thread_recv_window_accept_outer()` | タグ開始位置から 16 桁幅に整列済み | 既存の整列状態を維持する |
| `thread_recv_window_on_reject()` | タグ直後が 1 空白のみで未整列 | タグ開始位置から 16 桁幅に揃え、継続行の開始位置を整列する |
| `thread_recv_window_on_nack()` | タグ直後が 1 空白のみで未整列 | タグ開始位置から 16 桁幅に揃え、継続行の開始位置を整列する |
| `thread_recv_window_scan_ping_gap()` | タグ直後が 1 空白のみで未整列 | タグ開始位置から 16 桁幅に揃え、継続行の開始位置を整列する |
| `thread_recv_window_check_reorder_timeout()` | タグ直後が 1 空白のみで未整列 | タグ開始位置から 16 桁幅に揃え、継続行の開始位置を整列する |

Table: `thread_recv_window.h` 内の関数別 Doxygen コメント整列内容

変更は Doxygen コメント内の `@brief`、`@param[in]`、`@param[in,out]`、`@note` タグの開始位置から説明文または引数名までの空白幅 (16 桁幅) と、複数行にわたる継続行の空白整列だけに限定します。  
コメント本文の文面、タグ種別、方向指定、引数名、および関数の宣言構文は一切変更しません。

## 検証手順と異常時の再質問条件

対象ファイルの更新後は、次の手順で機械的検証と目視確認を実施します。

1. **整形の完了とべき等性の確認**: BL-007 の 10 ファイルおよび BL-005 の `thread_recv_window.h` に対して `clang-format --dry-run -Werror` を実行して警告と差分が 0 件であることを確認し、さらに `clang-format` を再適用しても新たな差分が発生しないこと (べき等性) を確認します。
2. **本文・トークン・文字列の保持確認**: 変更前後の差分を確認し、C のトークン列、文字列リテラル、コメント本文、Doxygen タグ種別、引数名が変更前と完全に一致していることを確認します。空白を無視する比較だけでは文字列リテラルやコメント内の意図しない変更を検出できないため、コメントと文字列を含む通常の差分全体を目視でも確認します。
3. **局所 Doxygen 生成と警告の確認**: `app/porter` で局所的に `make doxy` を実行し、内容のある `.warn` ファイルが存在しないこと、および `thread_recv_window.h` に対応する Doxygen 生成結果に崩れがないことを確認します。生成されたファイルは直接編集しません。
4. **変更範囲の境界確認**: BL-007 の 10 ファイル、BL-005 の 1 ヘッダー、および残課題一覧の更新以外に、今回のソース修正が広がっていないことを確認します。

> [!NOTE]
> 今回の変更は空白・改行の整形と Doxygen コメント内の空白整列に限定され、実行時の振る舞いや ABI、ビルド構成を変更しないため、動作テストの新設および framework や複数 app にまたがる全体テストは実施しません。

作業中に次のいずれかの異常や不整合が判明した場合は、独断で補完や妥協を行わず、その時点で作業を中断してユーザーへ再質問します。

- `clang-format` の適用により、空白や改行以外にトークン、文字列リテラル、または意味を変える差分が発生する場合
- `thread_recv_window.h` における Doxygen タグ 16 桁幅・継続行の整列と、`clang-format` の整形規則が、コメント本文や設定を変えずに両立できない場合
- 意味を変える修正が必要となり、局所テストの追加や範囲の再定義が必要になった場合 (全体テストが必要になった場合は、コストを説明して別途ユーザーに確認します)

## 残課題一覧の経緯更新と未対処 20 項目の引き継ぎ

ソース ファイルの対処と検証が完了した後、第 3 ゲートの承認を求める前に残課題一覧を次のとおり更新・作成します。

1. **過去の残課題一覧の保持と経緯更新**: 元の残課題一覧 (`app/general/docs/prompts/past/261107-source-comment-backlog.md`) のファイルおよび全 22 項目の記載は削除せず保持します。対処を完了した BL-005 と BL-007 の状態を「対応済み」に更新し、本案件 (`source-comment-mechanical-20261108`) で対処した経緯と検証結果を追記します。また、今回未対処の 20 項目には、最新作業の残課題一覧へ転記して引き継いだ旨 (転記先ファイル パスと課題 ID) を経緯として追記します。
2. **最新作業の残課題一覧への転記と整理**: 最新作業の残課題一覧 (`app/general/docs/prompts/current/261108-source-comment-mechanical-backlog.md`) を作成し、今回対処しなかった 20 項目 (BL-001〜BL-004、BL-006、BL-008〜BL-022) を、チケット管理システムへの起票にそのまま利用できる詳細度・具体性を維持して転記します。先行対象の BL-005・BL-007 に未解決の残渣が生じた場合は、その残渣もあわせて記録し、最新作業の残課題一覧だけで現時点の残件をすべて把握できるように整理します。
3. **Markdown の機械整形確認**: 更新・作成した残課題一覧の Markdown ファイルに対して `text_style_jp.py --dry-run` を先に実行し、不自然な変換がないことを確認したうえで `--in-place` を適用し、再度 `--dry-run` で差分がないことを確認します。

## 恒久仕様の追加を伴わないため正本規範は改訂しない

本案件は、既存の [ソース スタイル維持ガイドライン](../../source-style-guideline.md) および [app 向け Doxygen コメント](../../doxygen-comment-guideline.md) に定められた規則を既存ファイルへ適用する作業です。  
新しい恒久仕様や設計判断の追加はないため、`coding-guideline.md`、`source-style-guideline.md`、`doxygen-comment-guideline.md` などの正本ドキュメントは改訂しません。

## 受入条件

- BL-007 の対象 10 ファイルについて、使用した `clang-format` の版と設定、および検査時の差分の有無が記録されている。
- BL-007 の対象 10 ファイルと BL-005 の `app/porter/prod/libsrc/porter/thread/thread_recv_window.h` で、`clang-format --dry-run -Werror` の差分が 0 件であり、再適用しても差分が発生しない。
- `app/porter/prod/libsrc/porter/thread/thread_recv_window.h` の全 5 関数の Doxygen コメントで、タグ開始位置から 16 桁幅および継続行の整列が完了しており、コメント本文、タグ種別、引数名が変更前と一致している。
- 変更したすべてのソース・ヘッダー ファイルで、コメントと文字列を含む差分全体を目視および機械比較で確認し、C のトークン列、文字列リテラル、コメント本文の意味に一切変更がない。
- `app/porter` の `make doxy` が成功し、内容のある `.warn` ファイルが存在せず、該当する Doxygen 生成結果に崩れがない (かつ生成物を手動編集していない)。
- 元の残課題一覧 (`app/general/docs/prompts/past/261107-source-comment-backlog.md`) の全 22 項目が削除されずに保持され、BL-005・BL-007 の対処経緯と未対処 20 項目の引き継ぎ先が記録されている。
- 最新作業の残課題一覧 (`app/general/docs/prompts/current/261108-source-comment-mechanical-backlog.md`) に、未対処の 20 項目 (および先行対象に残渣がある場合はその項目) が起票レベルの詳細度で引き継がれ、整理状態が「整理済み」になっている。
- 本案件の合意文書の管理を除き、BL-005、BL-007、および残課題一覧の 2 ファイル以外に、変更が広がっていない。

## 対象ファイル一覧

| ファイル パス | 区分 | 変更内容 |
| --- | --- | --- |
| `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c` | BL-007 | 既存 `.clang-format` 設定による整形 (差分がある場合) |
| `app/cplat/prod/include/cplat/runtime/process_priority_linux.h` | BL-007 | 既存 `.clang-format` 設定による整形 (差分がある場合) |
| `app/cplat/prod/include/cplat/runtime/process_priority_windows.h` | BL-007 | 既存 `.clang-format` 設定による整形 (差分がある場合) |
| `app/cplat/prod/include/cplat/trace/tracer.h` | BL-007 | 既存 `.clang-format` 設定による整形 (差分がある場合) |
| `app/cplat/prod/libsrc/cplat/runtime/abort.c` | BL-007 | 既存 `.clang-format` 設定による整形 (差分がある場合) |
| `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c` | BL-007 | 既存 `.clang-format` 設定による整形 (差分がある場合) |
| `app/cplat/prod/libsrc/cplat/runtime/process_priority_windows.c` | BL-007 | 既存 `.clang-format` 設定による整形 (差分がある場合) |
| `app/cplat/prod/libsrc/cplat/runtime/stack_limit_linux.c` | BL-007 | 既存 `.clang-format` 設定による整形 (差分がある場合) |
| `app/porter/prod/include_internal/porter/potr_context.h` | BL-007 | 既存 `.clang-format` 設定による整形 (差分がある場合) |
| `app/porter/prod/libsrc/porter/api/potr_peer_disconnect.c` | BL-007 | 既存 `.clang-format` 設定による整形 (差分がある場合) |
| `app/porter/prod/libsrc/porter/thread/thread_recv_window.h` | BL-005 | Doxygen コメントのタグ 16 桁幅と継続行の空白整列 |
| `app/general/docs/prompts/past/261107-source-comment-backlog.md` | 残課題管理 | 全項目を保持したまま、BL-005・BL-007 の対処経緯と未対処 20 項目の引き継ぎ先を追記 |
| `app/general/docs/prompts/current/261108-source-comment-mechanical-backlog.md` | 残課題管理 | 新規作成。未対処 20 項目 (BL-001〜BL-004、BL-006、BL-008〜BL-022) の引き継ぎと整理 |

Table: 第 2 ゲート承認後に作成・更新する対象ファイル
