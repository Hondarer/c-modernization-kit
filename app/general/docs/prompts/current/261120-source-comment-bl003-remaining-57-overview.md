<!--
生成元: agree-before-implement スキル
文書種別: 全体管理文書 (overview)
最終更新日時: 2026-11-21 15:45:33 JST
最終更新エージェント: claude code (Fable 5.2) / セッション ID: session_01KJc53TxDHvQnb9NZpGEr1Q
稼働 OS: Windows
上流文書: `261118-source-comment-bl001-category-batch-backlog.md` (対策元の残課題一覧。BL-003 が対象)。削除されている場合があります。
下流文書: `261120-source-comment-bl003-remaining-57-discussion.md` (検討用 Markdown)、`261120-source-comment-bl003-remaining-57-lightadr.md` (簡易 ADR)、`261120-source-comment-bl003-remaining-57-design.md` (最終仕様)、`261120-source-comment-bl003-remaining-57-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。この文書と、ユーザーが編集した最新の検討用 Markdown を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# BL-003 の 57 件の対処 全体管理

この文書は、案件の識別、作業 WBS、対象範囲、57 件の分析結果と対処計画、確認状況、経緯を管理します。  
質問と回答の本文は検討用 Markdown に、決定事項は簡易 ADR に記載し、この文書には重複して記載しません。

## 案件の識別

検討 ID: `source-comment-bl003-remaining-57-20261120`

検討用 Markdown の版: 12

状態: 作業中

## 作業 WBS と進捗管理

この案件の作業 WBS です。  
項目 4 の内訳は、最終仕様の「作業の順序」を、作業の単位へ分けたものです。実装は、ユーザーの指示により `agy` (`gemini-4-hydrogen-high`) に行わせ、利用上限に達した場合は Opus または Sonnet のサブエージェントへ切り替えます。全体の管理と、変更の審査は、このセッション (Fable) が行います。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 対策元 backlog の BL-003 の状態を「検討中」へ更新し、継続案件の開始を記録する。
    - ✅ 1.2. 57 件の対象ソース、呼び出し元、公開ヘッダーの契約、関連文書、テスト、Git 履歴を調べる。
    - ✅ 1.3. OS と外部 API の一次資料 (Microsoft、OpenSSL、zlib、Linux の man ページ、systemd) を照合する。
    - ✅ 1.4. 57 件を対処の類型に分け、確認項目 ([Q-001](261120-source-comment-bl003-remaining-57-discussion.md#q-001-動作を変える修正を今回の案件に含めるか) から [Q-021](261120-source-comment-bl003-remaining-57-discussion.md#q-021-099-ロック-ファイルの共有削除の許可)) を作成する。
    - ✅ 1.5. ユーザーの回答を受け付けて記録する。
    - ✅ 1.6. 回答を検証し、簡易 ADR へ決定を反映し、WBS を見直す。対策元 backlog へ合意した方針を反映する。
    - ✅ 1.7. 最終仕様の作成のために、57 件の対象の行、貼り付ける文面、テストの手段、`see:` に使う URL の内容を調べる。
    - ✅ 1.8. 調査で新しく必要になった判断を、追加の確認項目 ([Q-022](261120-source-comment-bl003-remaining-57-discussion.md#q-022-直値を名前付き定数にするときの表現) から [Q-028](261120-source-comment-bl003-remaining-57-discussion.md#q-028-124-huge-測定を省略したときの通知の文面)) として提示する。
    - ✅ 1.9. 追加の確認項目の回答を受け付けて記録する。ユーザーの指示により、対話の質問ダイアログで回答を受けた。Q-024 は意図不明とされ、[Q-029](261120-source-comment-bl003-remaining-57-discussion.md#q-029-020-fd-番号が一致しない場合に公開-api-でアプリへ伝えるか) として作り直した。Q-029 の回答を受けて、[Q-030](261120-source-comment-bl003-remaining-57-discussion.md#q-030-020-公開-api-が失敗を返す範囲) と [Q-031](261120-source-comment-bl003-remaining-57-discussion.md#q-031-020-接続できなかった場合と標準ストリームが不完全な場合の区別) を提示し、回答を受けた。
    - ✅ 1.10. 追加の回答を検証し、簡易 ADR へ決定を反映し、WBS を見直す。対策元 backlog へ合意した方針を反映する。
    - ✅ 1.11. ユーザーの指示 (対処可能な残課題は対処したい) により、残課題一覧の 22 件について、対処できるかどうかと対処案を調べる。調査の結果は、セッションの一時ファイル (`bl003-backlog-E.md`、`bl003-backlog-F.md`) にある。
    - ✅ 1.12. 残課題の対処で判断が要る項目を、追加の確認項目 ([Q-032](261120-source-comment-bl003-remaining-57-discussion.md#q-032-bl-001-モジュールのパスの取得で対応する長さ) から [Q-044](261120-source-comment-bl003-remaining-57-discussion.md#q-044-bl-022-全ソースの確認の範囲と不適合の扱い)) として提示し、回答を受け付けて記録する。
    - ✅ 1.13. 回答で決まった対処の設計の調査を行う。対象は、名前付きパイプの方式 (BL-006)、`service-sample` の Linux の監視と停止 (BL-007、BL-008、新しい気付き 2 件)、`app/cplat` と `app/struct-meta` の新しい気付き 7 件。利用上限で中断した後、再開して完了した。結果は、セッションの一時ファイル (`bl003-backlog-G.md`、`bl003-backlog-H.md`、`bl003-backlog-I.md`) にある。
    - ✅ 1.14. 新しい規範 (名前付きの整数定数) への適合を、`app/porter` を除く全 app の製品コードで確認する。指摘は 594 行 (機械的に直せるもの 279、動作や公開の契約が変わり得るもの 7、判断が要るもの 308)。結果は、セッションの一時ファイル (`constants-audit-K1` から `K4` の `.tsv` と `.md`) にある。`app/porter` は、フロー制御の後に確認する。
    - ✅ 1.15. 1.13 と 1.14 の結果から、判断が要る項目 (規範の例外の定め方、公開ヘッダーの定数の型、設計の選択) を、確認項目 ([Q-047](261120-source-comment-bl003-remaining-57-discussion.md#q-047-規範の例外-容量を拡張する倍率の-2) から [Q-058](261120-source-comment-bl003-remaining-57-discussion.md#q-058-getmodulefilenameu-が切り詰めた場合の戻り値)) として提示する。調査でさらに見つかった気付きは、残課題一覧へ記録する。
    - ✅ 1.16. Q-047 から Q-058 の回答を受け付けて記録する。12 件とも回答を受けた (Q-051 は A、ほかは推奨案)。
    - ✅ 1.17. 回答を検証し、簡易 ADR へ決定を反映する。規範の案の例外の文面を確定し (セッションの一時ファイル `norm-integer-constants-v2.md`)、適合の確認の指摘を分け直す。`agy` (`gemini-4-hydrogen-high`) が 4 つの範囲で実行し、666 行を、直す 623、例外に当たる 37、判断が要る 6 に分けた (新しく見つけた 72 行を含む。結果は `constants-reclass-K1` から `K4` の `.tsv` と `.md`)。
    - ✅ 1.18. 判断が要る 6 行のうち、設計で決められない 3 件を、確認項目 ([Q-059](261120-source-comment-bl003-remaining-57-discussion.md#q-059-公開ヘッダーの中で使う文字符号の値) から [Q-061](261120-source-comment-bl003-remaining-57-discussion.md#q-061-内訳と合わない余裕を持つバッファーの大きさ)) として提示し、回答を受け付けて記録する。3 件とも推奨案の回答を受けた。
    - ✅ 1.19. ユーザーの指示により、作業ツリーへ最新の変更を取り込み (ルートは `ee1dc888`。`app/porter` はフロー制御の変更を含む)、`make skills` を実行した。変更されたソースと新しいソースについて、最終仕様の再確認と、`app/porter` の規範への適合の確認を `agy` で行った。結果は、セッションの一時ファイル (`recheck-R1.md`、`constants-porter-R2`、`constants-porter-R3`) にある。
    - ✅ 1.20. 再確認で必要になった判断を、確認項目 ([Q-062](261120-source-comment-bl003-remaining-57-discussion.md#q-062-c-の製品コードの定数の表現) から [Q-065](261120-source-comment-bl003-remaining-57-discussion.md#q-065-appporter-のほかのファイルにある重複したゼロの代入)) として提示し、回答を受け付けて記録する。対話の質問ダイアログで、4 件とも推奨案の回答を受けた。
    - ✅ 1.21. `POTR_NACK_DEDUP_SLOTS` の履歴を調べる (Q-063)。移行時は `POTR_MAX_PATH` が 4 で値 8 と一致しており、`a98cef4` で `POTR_MAX_PATH` が 8 へ変わった時に直されなかった。意図した値は `POTR_MAX_PATH` の 2 倍と分かったため、式で定義する。`app/porter` の指摘のうち C++ の定数化は、`constexpr` で直す。
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に未解決の事項と質問が残っていないことを確認する。
    - ✅ 2.2. 簡易 ADR の決定と、根拠となった回答の一致を確認する。
- ⚠️ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `261120-source-comment-bl003-remaining-57-design.md` を作成する。57 件の分へ、残課題の対処、新しい規範、規範への適合の修正を統合した (2936 行)。規範への適合の修正の一覧 (623 行、123 ファイル) は、付録 `261120-source-comment-bl003-remaining-57-design-appendix-constants.md` に置いた。追記の下書きと統合は `agy` (`gemini-4-hydrogen-high`) が行い、このセッションが章の構成、検討用の語の残り、統合の記録 (矛盾 10 件、落とした内容 6 件) を確かめた。
    - ⚠️ 3.2. 最終仕様の受入条件と整合性を確認する。章の構成と、決定との矛盾の解消の記録は確かめた。未確認の事項: 統合後の全文 (2936 行) と付録 (623 行) を、このセッションが 1 行ずつ読んではいない。確認方法: 実装の着手の時に、項目ごとに対象のソースと決定で確かめる。
    - ✅ 3.3. 最終仕様の承認を受け付ける (第 2 ゲート)。2026-11-21 13:43 にユーザーが承認した。
- 🔄 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. 基盤を反映する。共通のコーディング規範へ「名前付きの整数定数」の節を足す。`app/cplat` へ、公開ヘッダー `cplat/base/char_code.h`、公開マクロ `PLATFORM_PATH_UTF8_MAX`、UTF-8 の文字の境界の内部共有の関数を足す。 審査で、共通の規範から `app/cplat` のヘッダー名を除き、定義する場所は `app/cplat` の規範へ書いた (汎用層から個別 app を参照しない規則のため)。Windows のビルドと `utf8Test` は成功。Linux は未確認。
    - ✅ 4.2. `app/cplat` の動作を変える項目 (015、020 と 021 と 022、036 と 037 と 039 と 040 と、プロンプトの結果コード、063 と 084、102、105) を反映する。 すべて実装と審査が済み。015、063、084、102、105 は、単体テスト 177 件と、ETW の結合テスト 9 件が成功。コンソールとプロンプトは、`consoleTest` 25 件、`pinnedPromptTest` 62 件、`promptTest` 78 件が成功。Linux だけのテストと、実際のコンソールでの `cplat_console_attach_parent` の動作は未確認。
    - 🔄 4.3. `app/cplat` の追加の修正 (パスの長さ、引数パーサー、コンソールへの書き込み、名前付きパイプ、同期の結果コード、文書) を反映する。 名前付きパイプ以外は、実装と審査が済み (20 個のテスト ディレクトリの単体テストが成功、内容のある `.warn` なし)。審査で、`cplat_module_get_basename` の作業領域の拡張に上限を足し (Linux で、パスが長すぎる失敗のたびに領域を広げ続ける不具合)、`GetModuleFileNameU` の失敗の経路で最終エラーを保つようにした。名前付きパイプを実装中。 ← **CURRENT**
    - ⏸️ 4.4. `app/cplat` の動作を変えない項目と、コマンドの項目 (117、118、124、129 など) を反映する。
    - ⏸️ 4.5. `app/cplat` の規範への適合の修正 (付録の `app/cplat` の章) を、ディレクトリごとに反映する。
    - 🔄 4.6. `app/service-sample`、`app/calc`、`app/string-catalog-sample`、`app/struct-meta`、`app/override-sample`、`app/subfolder-sample` を反映する (57 件の項目、追加の修正、規範への適合の修正)。 ユーザーの指示 (`もう1並列、Opusを利用して良い。`) により、4.3 から 4.5 と並行して、Opus のサブエージェントで実装中。変更するファイルは `app/cplat` と重ならない。ビルドは `app/cplat` の出力を読むだけのため、`app/cplat` の再ビルドと重なった場合は再実行する。
    - ⏸️ 4.7. `app/porter` を反映する (57 件の項目、追加の修正、`POTR_NACK_DEDUP_SLOTS`、規範への適合の修正)。着手の前に、フロー制御の作業の担当セッションへ連絡する。 2026-11-21 に連絡し、返信を受けた。相手の側に未コミットの変更はなく、着手して差し支えないとのこと。`app/porter` のリモートの `main` は `7468b46` へ進んでいる (差分は `docs/prompts/current/` の検討文書だけ)。作業ツリーは `a16f6ef` のままであり、取り込みはユーザーの指示を受けてから行う。`health-check.md` の文面は、相手が実装と照合し、3 点を訂正した (受信報告の行の条件へ「実効の `health_interval_ms` が 0 でない」を足す、フロー制御の行の条件を「フロー制御パケットを送出する受信側」に直す、TCP の補足へポーリングへ変わる場合を足す)。最終仕様が文面の確認を定めているため、訂正後の文面を使う。相手からの条件は 3 つ (`POTR_FLOW_CONTROL_STATE_*` の値を明示する、`potr_flow_control.c/.h` と `receiver_report.c` は必要な範囲だけを変える、定数の値は変えない)。プッシュした時点で、相手へコミットを知らせる。
    - ⏸️ 4.8. 全体の検証 (変更した app の `make`、`make test`、`make doxy`、`.warn`、`text_style_jp`) と、受入条件への適合を確認する。
    - ⏸️ 4.9. 簡易 ADR を最終確認する。
    - ⏸️ 4.10. 残課題一覧を再構築して整理し、対策元 backlog の BL-003 の最終状態を記録する。
- ⏸️ 5. 成果物の承認の受付 (第 3 ゲート)
    - ⏸️ 5.1. 成果物、検証結果、残課題を提示する。
    - ⏸️ 5.2. 成果物の承認を受け付ける。

## 対象と参照資料

対象成果物: 57 件の対象ソース (下の分析結果の表)、関連する公開ヘッダー、正本文書、テストです。

今回の対象範囲: 対策元 backlog `261118-source-comment-bl001-category-batch-backlog.md` の BL-003 の 57 件すべてです (2026-11-20 のユーザーの依頼)。

書き込み可能な検討文書: この文書、検討用 Markdown、簡易 ADR、残課題一覧です。

参照資料と版:

- ルート リポジトリ `8ad39640`、`app/cplat` `00794975`、`app/porter` `22d2d45` (2026-11-20 の調査時点)。
- `app/general/docs/coding-guideline.md` (実装コメントの規範。確認していない推測の理由は書かず、確認できた事実だけを書く)。
- 一次資料の URL は、分析結果の表と検討用 Markdown の各 Q に記載しています。

調査の限界: 調査はコード、文書、Git 履歴、一次資料の読解で行い、ビルド、テストの実行、実機での確認はしていません。  
「再現条件」と書いた内容は、コードを追って導いたものです。

## 57 件の分析結果

### 全体の結果

57 件のうち、解消済みの項目はありません。  
直前の案件 (`void-discard-reason-and-failure-injection`) の変更は、57 件の疑問点に触れていませんでした。

値や分岐を選んだ理由が、履歴または文書から見つかった項目は 2 件だけです。  
003 は導入コミットのメッセージに理由がありました。153 は統合前のファイルのコメントに目的がありました。  
残る項目は、コミット メッセージの本文がないか、理由の記載がありません。

一方、調査で次の 4 種類の結果が得られました。

- 理由は分からないが、値が満たすべき制約や、値を変えたときの影響を、事実として確認できた項目です。規範に従い、確認できた事実だけを書けます。
- 一次資料と照合して、現在の実装が資料と合っていること、または資料では決まらないことを確認できた項目です。確認できた範囲を `see:` 付きで書けます。
- 現在の動作を仕様として確定すれば、契約として明記できる項目です。
- 現在の実装が、機能仕様、一次資料、または規範と合っていない項目です。11 件あり、実装の修正とテストの追加が要ります。

### 対処の類型

57 件を、推奨する対処の内容で 5 つの類型に分けました。

| 類型 | 内容 | 動作の変化 | 件数 |
| --- | --- | --- | --- |
| 1 | 確認できた事実を、コメント、Doxygen、正本文書へ記載する。 | なし | 26 |
| 2 | 動作を変えないコードの変更 (直値の定数化、重複している処理の削除) と、事実の記載。 | なし | 11 |
| 3 | 現在の動作を契約として公開ヘッダーまたは正本文書へ明記する。必要な場合は現状を固定するテストを足す。 | なし | 8 |
| 4 | 実装を修正し、テストを追加する。 | あり | 11 |
| 5 | 変更しない (記録だけ)。 | なし | 1 |

Table: 対処の類型と件数

類型 1: 003、004、007、012、021、022、030、031、034、041、042、049、059、100、107、116、128、132、135、157、158、169、173、181、182、014。  
類型 2: 039、040、117、118、143、145、146、153、154、155、156。  
類型 3: 008、017、028、029、098、099、134、141。  
類型 4: 015、020、036、037、063、084、102、105、124、129、159。  
類型 5: 013。

012 は、推奨案ではタグを 1 つ削除し、Doxygen の出力に小さなグラフが 1 つ増えます。実行時の動作は変わらないため、類型 1 に入れています。  
084 は、非有限値の検証 (類型 4) と、ロケールの前提の明記 (類型 3 に相当) の 2 つを含みます。

### 容量、件数、しきい値の数値 (16 件)

| 枝番号 | 対象 | 確認した事実 | 推奨する対処 | 類型 | 確認項目 |
| --- | --- | --- | --- | --- | --- |
| 008 | `cplat/crt/path.h` の可変長マクロ | 断片が 17 個以上だと、17 番目の文字列が断片数の位置に入る。GCC 13 以前の C は警告だけで通る。上限 16 は Doxygen にも文書にもなく、製品コードの呼び出しは最大 3 断片。 | 2 つのマクロの Doxygen に、上限 16 と `_n` 版への誘導を書く。 | 3 | Q-002 |
| 013 | `argparser.c` の説明列 28 | 名前付き定数と意味のコメントがすでにある。端末幅を参照する処理はなく、28 を導く式はない。テストが期待文字列で 28 桁を固定している。 | 変更しない。 | 5 | Q-002 |
| 014 | `argparser.c` のバッファー 512 | メッセージは固定文と、長さに上限のない対象文字列の連結で、512 が十分である証明は成り立たない。切り詰めは意図した動作として、コメント、公開ヘッダー、テストに記録がある。登録エラー側の Doxygen には切り詰めの記載がない。 | 定義のコメントに、切り詰めと戻り値を書く。登録エラー側の Doxygen に一文を足す。 | 1 | Q-002 |
| 036 | `pinned_prompt.c` のセンチネル値 9999 | 参照は「前回と今回の小さいほうを採る」比較だけ。行数の型は 9999 を超える値を表せる。端末が 10002 行以上のとき、初回の描画で通常出力の領域を余分に消去する。 | センチネル値を `INT_MAX` に変え、境界のテストを足す。 | 4 | Q-015 |
| 039 | `pinned_prompt.c` の初期容量 4 | 履歴コンテキストは readline の呼び出し箇所ごとに 1 個。満杯で 2 倍に再確保する。4 の意味は「4 個までは確保が 1 回」だけ。 | 名前付き定数にし、事実を書く。 | 2 | Q-002 |
| 040 | `pinned_prompt.c` の初期容量 256 | 初回だけ 256 [byte] を確保し、不足時は必要長 + 1 へ再確保する。端末幅との関係はコードにない。 | 名前付き定数にし、事実を書く。 | 2 | Q-002 |
| 059 | `module.c` の `path_buf[4096]` | Linux の `PLATFORM_PATH_MAX` は `PATH_MAX`。Windows は取得側が `MAX_PATH` (260) で制限するため、4096 が不足する経路はない。ファイル冒頭の注記は現状と合わない。 | コメントに各 OS の上限との関係を書き、冒頭の注記を直す。 | 1 | Q-002 |
| 117 | `bench-hashtable.c` の `lifetime = 5` | ベンチマークは加齢の処理を呼ばないため、3 から 255 のどの値でも測定対象の処理は同じ。 | 名前付き定数にし、`benchmark-method.md` に事実を書く。 | 2 | Q-002 |
| 118 | `bench-hashtable.c` の既定の上限 16384 | 既定では 256、1024、4096、16384 の 4 点を測り、共有している結果表もこの 4 点。時間やメモリの制約から 16384 が導かれる関係はない。 | 名前付き定数にし、`benchmark-method.md` に事実を書く。 | 2 | Q-002 |
| 135 | `compress-cli.c` の 2 倍と下限 256 | zlib の上界 (`deflateBound`) と照合した結果、入力 7 [byte] 以上は 2 倍が上界以上、6 [byte] 以下は下限 256 が上界以上で、不足する条件は見つからない ([zlib Manual](https://zlib.net/manual.html))。 | コメントを照合の結果と `see:` に書き換える。 | 1 | Q-002 |
| 141 | `potr_service_open.c` の下限 2 | 送信キューは `count + inflight >= depth` で満杯を判定する。depth が 1 で壊れる処理は見つからないが、まとめ送りが起きなくなる。`config.md` に下限の記載がなく、下限のテストもない。 | 下限 2 を仕様として `config.md` に書き、境界のテストを足す。 | 3 | Q-002 |
| 146 | `potr_send_thread.c` の抑制回数 10 | 警告の後、書き込み可能の判定が累計 10 回で抑制を解く。`architecture.md` は「ERROR ログ」「10 回連続」と記載しており、コード (WARNING、累計) と合わない。テストは直値に依存する。 | 名前付き定数にし、`architecture.md` の相違を直す。 | 2 | Q-002、Q-003 |
| 153 | `tcpServer_linux.c` の backlog 128 | 統合前のファイルに「全ワーカーがビジー時に新しい接続をキュー イングするため」のコメントがあった。`somaxconn` を超える値は切り詰められる ([listen(2)](https://man7.org/linux/man-pages/man2/listen.2.html))。128 の導出はない。 | 共通の定数にし、目的と切り詰めの事実を `see:` 付きで書く。 | 2 | Q-002 |
| 154 | `tcpServer_windows.c` の backlog 128 | 指定値はプロバイダーが制限し、実際の値を得る手段はない ([listen](https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-listen))。Windows で 128 を選んだ理由はない。 | 153 と定数を共用し、制限の事実を `see:` 付きで書く。 | 2 | Q-002 |
| 169 | `string-catalog-library-sample.c` の 4096 と 1024 | 書式へ渡す表示用の引数で、分岐にもバッファーの大きさにも使われない。ライブラリに 1024 という上限はない。 | 表示例であることをコメントに足す。 | 1 | Q-002 |
| 181 | `struct_meta_gen_emit.c` の `count * 2U` | `capacity` は件数以上が必須で、2 倍は占有率を 50% 以下にする。同じ係数が `catalog.c` と `index.c` にもある。測定や基準の記載はない。 | `architecture.md` に事実を書き、3 か所のコメントから参照する。 | 1 | Q-002 |

Table: 容量、件数、しきい値の数値の分析結果

### 待ち時間と周期の数値 (7 件)

| 枝番号 | 対象 | 確認した事実 | 推奨する対処 | 類型 | 確認項目 |
| --- | --- | --- | --- | --- | --- |
| 041 | `abort.c` の待機 1000 [ms] | 別のスレッドが異常終了を処理中のときだけ入る無限ループ。プロセスは処理中のスレッドの `abort()` で終了するため、1000 は起床の頻度だけを決め、終了の時刻に影響しない。 | 事実と `see:` をコメントに書く。 | 1 | Q-002 |
| 145 | `potr_recv_thread.c` の待ち時間 | `health_timeout_ms` の 3 分の 1、下限 100 [ms]、無効時 500 [ms] がすべて直値。待ち時間は、無受信の検出の遅れと停止の上界を決める。`health-check.md` は 3 分の 1 だけを記載している。 | 3 つの直値を名前付き定数にし、各値が何の上界かをコメントと `health-check.md` に書く。 | 2 | Q-002、Q-003 |
| 155 | `tcpServer_windows.c` の待機 10 [ms] | 接続が 0 本のワーカーが、パイプを再確認するまでの待ち。新しい接続の受け取りの遅れの上界になる。 | 156 と共通の定数にし、役割を書く。 | 2 | Q-002 |
| 156 | `tcpServer_windows.c` の `select` の 10 [ms] | 接続が 1 本以上のワーカーが、パイプの確認へ戻る周期。155 と同じ役割の値である。 | 155 と同じ定数を使う。 | 2 | Q-002 |
| 157 | `service-sample_linux.c` の `RestartSec=5` と `WatchdogSec=30` | 通知は `sd_event_set_watchdog()` が期限の半分 (15 [s]) の周期で送り、systemd の推奨を満たす ([sd_watchdog_enabled](https://www.freedesktop.org/software/systemd/man/latest/sd_watchdog_enabled.html))。Windows 側の定数は `RestartSec=5` と一致させている。値の理由はない。 | `service-events.md` に、例示値であること、通知の周期、通知するスレッドを書く。 | 1 | Q-002 |
| 158 | `service-sample_linux_events.c` の終了待機 5000 [ms] | 名前付き定数になっている。unit は `TimeoutStopSec` を指定せず、既定の 90 [s] が適用される。停止の上界は 5 [s] 強で 90 [s] に収まる。 | 定数の説明に、停止タイムアウトとの関係と超過時の動作を `see:` 付きで書く。 | 1 | Q-002 |
| 159 | `service-sample_windows.c` の `dwWaitHint` | 停止の経路で、2 回目の `STOP_PENDING` が同じ checkpoint 1 のまま通知される。一次資料は、hint の時間内に checkpoint を増やすか状態を変えることを求めている ([SERVICE_STATUS](https://learn.microsoft.com/en-us/windows/win32/api/winsvc/ns-winsvc-service_status))。 | 2 回目の通知で checkpoint を増やす。hint を名前付き定数にし、契約を書く。 | 4 | Q-014 |

Table: 待ち時間と周期の数値の分析結果

### OS や外部 API の仕様との照合 (12 件)

| 枝番号 | 対象 | 確認した事実 | 推奨する対処 | 類型 | 確認項目 |
| --- | --- | --- | --- | --- | --- |
| 020 | `console.c` の `reopen_crt_std_fd` | `_open_osfhandle` は空いている最小の番号を返す (UCRT のソース)。番号がずれるのは、若番の標準 FD が閉じられている場合と、別のスレッドが間にファイルを開いた場合。ずれても診断ログだけで成功を返し、FILE が閉じた FD を指したままになる。 | 不一致を失敗として返し、呼び出し側のコメントを直す。 | 4 | Q-008 |
| 021 | `console.c` の再接続後の書き込み失敗 | 実機の記録は 4 点あるが、ログも再現手順もない。CRT 側の原因は一次資料から特定できない。`console.c`、`console.h`、README の記載が、2 つの現象を区別していない。 | 2 つの現象を分けて記載し、原因が未特定であることを 3 つの文書でそろえる。 | 1 | Q-019 |
| 022 | `console.c` の `stdin` のバッファー | `stdout` と `stderr` だけ `setvbuf(_IONBF)` を呼ぶ。`stdin` を無バッファーにする必要を示す資料はない。理由の記載はない。 | 出力だけを無バッファーにする意図を書き、`see:` を付ける。 | 1 | Q-002 |
| 028 | `sys_stat.c` の `path_root_prefix_len` | `\\?\C:\` と `\\?\Volume{GUID}\` は正しい位置から作る。`\\?\UNC\srv\share\` は `?` をサーバー、`UNC` を共有と誤認する。260 字以上は拒否する。公開契約はデバイス名前空間に触れていない。 | `\\?\` と `\\.\` で始まるパスを保証しないことを、公開ヘッダーへ書く。 | 3 | Q-002 |
| 030 | `crypto_linux.c` の `EVP_DigestUpdate` | OpenSSL の資料は、長さ 0 のときのポインターの扱いを定めていない ([EVP_DigestInit](https://docs.openssl.org/master/man3/EVP_DigestInit/))。置き換えは未規定の動作に依存しない。 | 事実と `see:` を書く。 | 1 | Q-002 |
| 031 | `crypto_windows.c` の `BCryptHashData` | SDK の `bcrypt.h` は `pbInput` を `_In_reads_bytes_(cbInput)` と注釈し、省略可能としていない。現在の置き換えは注釈と合う。 | 事実と `see:` を書く。 | 1 | Q-002 |
| 034 | `endpoint_linux.c`、`endpoint_windows.c` の `ai_socktype` | 種別を 0 にすると、種別ごとに結果が重複する ([getaddrinfo(3)](https://man7.org/linux/man-pages/man3/getaddrinfo.3.html))。`SOCK_DGRAM` と `SOCK_STREAM` でアドレスが変わるかは資料に記載がない。両実装とも `SOCK_DGRAM`。 | 種別を 1 つに絞る理由だけを、両ファイルへ同じ文面で書く。 | 1 | Q-002 |
| 042 | `elevated_process.c` のハンドルの受け渡し | 「runas では継承できない」と直接述べる資料は確認できない。`ShellExecuteExW` には継承を指定する引数がない。整合性レベルによる制限は、低いほうから高いほうへの向きである。 | コメントを 2 文に分け、確認できる表現へ直し、それぞれに `see:` を付ける。README も同じ。 | 1 | Q-002 |
| 049 | `elevated_process.c` のセッション 0 | Doxygen は「おそれがある」と書き資料の範囲に収まるが、短いコメント 3 か所は「UAC ダイアログを表示できないため」と断定している。 | 3 か所を確認できる表現にそろえ、判定の条件を関数コメントに書く。 | 1 | Q-002 |
| 099 | `sync_windows.c` の `FILE_SHARE_DELETE` | cplat はロック ファイルを削除しない。外部の操作でファイルが削除または名前変更されると、2 つのプロセスが同時に排他ロックを取得する経路がある (Linux でも同じ)。公開ヘッダーは identity を「名前文字列」とだけ書いている。 | identity がファイル パスであることと、参加中は削除と名前変更をしないことを、公開ヘッダーへ書く。 | 3 | Q-021 |
| 100 | `sync_windows.c` の先頭 1 [byte] | 終端を超える範囲のロックは許される。長さ 0 の扱いは資料に記載がない ([LockFileEx](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-lockfileex))。全参加者が同じ範囲を使う。 | 事実と `see:` を定数に書く。 | 1 | Q-002 |
| 102 | `trace_etw_session.c` の `PropertyStruct` | 構造体のメンバーのデータを消費せずに次のプロパティを読むため、位置がずれる。配列も考慮していない。cplat 自身が書くイベントは構造体を含まないため、現状の利用では発生しない。 | 構造体と配列で読み取りを打ち切り、理由と `see:` を書く。 | 4 | Q-009 |

Table: OS や外部 API の仕様との照合の分析結果

### 設計者の判断が必要な仕様と分岐 (22 件)

| 枝番号 | 対象 | 確認した事実 | 推奨する対処 | 類型 | 確認項目 |
| --- | --- | --- | --- | --- | --- |
| 003 | `calc.c` の乗算記号 `x` | 導入コミット `f3e853b0` のメッセージに「シェルのワイルドカード展開を回避」とある。コミットの作者は Claude である。`x` を固定するテストはない。 | 理由をコメントに書く。 | 1 | Q-002 |
| 004 | `shared-and-static-calc.c` の乗算記号 `x` | 003 の 2 日後に同じ演算子を取り込んだ。自身のコミットに理由はない。 | 003 と同じ文面のコメントを書く。 | 1 | Q-002 |
| 007 | `dll_exports.h` の `__INTELLISENSE__` | 理由の記載はどのコミットにもない。分岐はこのヘッダー 1 か所だけ。Microsoft の資料はマクロの用途だけを説明している。 | 事実と `see:` だけを書く。 | 1 | Q-018 |
| 012 | `error_message_internal.h` の `@hideincludedbygraph` | 製品のヘッダー 100 件のうち 77 件がタグを持ち、75 件は「多数のソースから参照されるため」の定型コメントを併記する。このヘッダーを取り込むのは 2 ファイルだけで、定型の理由が当てはまらない。 | このヘッダーのタグを削除し、`error.h` に定型コメントを足す。 | 1 | Q-017 |
| 015 | `error.c` の名前解決の失敗の結果コード | `EAI_MEMORY` と `EAI_AGAIN` は、要因では区別するが結果コードは `CPLAT_ERR_UNKNOWN` になる。errno 版と Winsock 版は、同じ状況を `CPLAT_ERR_OUT_OF_MEMORY` と `CPLAT_ERR_BUSY` へ変換する。`endpoint.h` の `@return` に `CPLAT_ERR_NOT_FOUND` が抜けている。 | `EAI_MEMORY` と `EAI_AGAIN` の結果コードを分ける。`@return` を直す。 | 4 | Q-012 |
| 017 | `clock.c` の秒から分への変換 | C の整数除算で、分未満を 0 方向へ切り捨てる。端数が出るのは、Linux で歴史的な時刻を渡した場合に限られる。公開契約は分未満の扱いを書いていない。 | 切り捨てを公開ヘッダーへ明記し、テスト 1 件で固定する。 | 3 | Q-002 |
| 029 | `sys_stat.c` の `mkdir` の 0755 | 導入時のヘッダーにあった契約文が、現在のヘッダーにない。0755 を選んだ理由はない。権限を指定できる API はない。 | 契約文を公開ヘッダーへ戻す。 | 3 | Q-002 |
| 037 | `pinned_prompt.c` の内部関数の 0 と -1 | 共通の規範に、static の関数を結果コードの対象外とする規定はない。同じファイルの別の関数は共通の結果コードを返している。メモリ不足の扱いが、呼び出し元によって `CPLAT_ERR_UNKNOWN` と `CPLAT_ERR_OUT_OF_MEMORY` に分かれている。 | 2 つの関数を共通の結果コードへ統一し、公開 API の扱いもそろえる。 | 4 | Q-011 |
| 063 | `filter_describe.c` の定数の作業配列 | 外部イメージの検証が、文字列の定数の長さを行幅と比べていない。行幅 1024 では、説明文が 4111 [byte] で切れたまま `CPLAT_OK` を返す。範囲外への書き込みは起きない。 | 検証で、行幅を超える文字列の定数を拒否する。 | 4 | Q-006 |
| 084 | `filter_image.c` の浮動小数点の定数 | コンパイルの経路は非有限値を拒否するが、外部イメージの読み取りは検査しない。NaN と Inf は `nan.0`、`inf.0` と出力され、再コンパイルできない。小数点の表記はロケールに依存する。 | 読み取りで非有限値を拒否する。ロケールの前提を公開ヘッダーへ書く。 | 4 | Q-007、Q-002 |
| 098 | `sync_descriptor.c` の予約 8 [byte] | 書き出しは 0 で埋め、読み込みは検査しない。用途の記載はどこにもない。 | 「用途は未定。書き出しは 0、読み込みは検査しない」と内部ヘッダーへ書く。 | 3 | Q-002 |
| 105 | `trace_eventlog.c` のレベルとイベント ID | 強制出力のレベル (負値) が変換されずに渡り、イベント ID 0、カテゴリ 0、Information で記録される。ETW と syslog は変換している。機能仕様の記載と合わない。 | 強制出力のレベルを変換し、テストと対応表を足す。 | 4 | Q-005 |
| 107 | `trace_eventlog.c` の識別子の書式化 | `int64_t` は最大 20 文字で、32 [byte] の作業配列に収まる。切り詰めは起こらない。 | 桁数の検算をコメントに書く。 | 1 | Q-002 |
| 116 | `file_api.c` の `WriteConsoleU` | 現在の実装は比例変換をしておらず、公開契約 (部分書き込みでは 0) と一致する。部分書き込みが起こる条件は、一次資料に記載がない。 | 現在の契約を維持し、実装に事実を書く。 | 1 | Q-020 |
| 124 | `bench-io.c` の huge 測定の省略 | 通常サイズは失敗しても次へ進むが、huge だけが失敗 0 件を条件にする。省略しても通知は出ない。文書に記載はない。 | 省略し、省略したことを通知する。文書へ書く。 | 4 | Q-013 |
| 128 | `bench_mmap.c` の集計式 | 集計値は出力も比較もされない。データは 0 から 65535 で、符号の扱いは結果に影響しない。 | 事実をコメントに書く。 | 1 | Q-002 |
| 129 | `bench_report.c` の CPU 名 | `/proc/cpuinfo` の書式により、値の先頭に空白が 1 個残る。Windows 側には入らない。 | 先頭の空白を除去し、テストを足す。 | 4 | Q-010 |
| 132 | `bench_stdio.c` の集計式 | 128 と同じ式である。 | 128 と同じコメントを書く。 | 1 | Q-002 |
| 134 | `bench_timer.c` の中央値 | 偶数件では、中央の 2 値のうち大きいほうを返す。既定の試行回数は 5 で奇数。 | 偶数件の定義を、ヘッダーと `benchmark-method.md` へ書く。 | 3 | Q-002 |
| 143 | `potr_service_open.c` の原子変数の初期化 | `cplat_atomic_*` は通常の構造体で、契約がゼロ初期化を許可している。`memset` の後の 0 の格納は重複している。 | 重複している格納を削除する。 | 2 | Q-016、Q-003 |
| 173 | `string-catalog-library-sample.c` の DEBUG | サンプルは VERBOSE のトレースを出力するため、VERBOSE 以上のしきい値が必要。DEBUG のトレースはない。 | すべてのレベルを表示することを、コメントと README へ書く。 | 1 | Q-002 |
| 182 | `struct_meta_gen_emit.c` の `lifetime` | 索引は構築後に削除しないため、`lifetime` は動作に影響しない。構成検査が 2 以上を求める。 | 事実を `emit` と `catalog.c` のコメントに書く。 | 1 | Q-002 |

Table: 設計者の判断が必要な仕様と分岐の分析結果

## 実装レベルの調査で分かった事実の訂正

最終仕様の作成のために対象の行を調べた結果、「57 件の分析結果」と検討用 Markdown の Context の記載に、正確でない点が見つかりました。  
検討用 Markdown の回答済みの Q は書き換えず、ここに訂正を記録します。最終仕様は、訂正後の事実で作成します。

| 枝番号 | 元の記載 | 訂正後の事実 | 影響 |
| --- | --- | --- | --- |
| 012 | 製品のヘッダーは 100 件。75 件が定型のコメントを併記する。 | `prod/` 配下の `.h` は 107 件。タグを持つのは 77 件で変わらない。併記のある 75 件のうち、72 件が「多数のソース ファイル」、3 件が「複数のソース ファイル」の文面である。 | `error.h` には、72 件と同じ文面を足す。 |
| 014 | 超過分は切り詰めて出力し、`CPLAT_ERR_BUFFER_TOO_SMALL` を返す。 | 登録エラーの出力は、ループの中で結果を上書きする。戻り値は、最後に出力したメッセージの結果だけを表す。 | コメントと Doxygen は、成り立つ範囲の文面にする。戻り値の動作は、残課題一覧へ記録した。 |
| 015 | `EAI_SYSTEM` の現在の結果コードは `CPLAT_ERR_UNKNOWN`。 | `CPLAT_ERR_NOT_FOUND` は要因で判定している。`EAI_SYSTEM` で errno が `ENOENT` などの場合は、現在も `CPLAT_ERR_NOT_FOUND` が返る。 | Q-023 で確認する。 |
| 021 | CRT のストリーム経由の書き込みが、`EACCES` や `EBADF` で失敗する。 | `EACCES` と `EBADF` は、別の方式 (`freopen`、`_dup2`、複製したハンドル) を試したときの失敗の記録である。現在の方式での失敗の errno は、記録がない。 | 現在の方式の現象には、errno を書かない。 |
| 034 | 種別を 0 にすると、同じアドレスが種別ごとに重複して返る。 | Linux の資料の記載は、「同じサービスが複数のソケット種別で提供される場合に、結果が複数になる」である。Windows の資料は、「0 はどの種別も受け入れる」とだけ記載している。 | コメントの文面を「種別ごとに結果が複数返るのを避ける」とする。Windows 側は Windows の資料を `see:` に使う。 |
| 042 | 整合性レベルが低いプロセスから高いプロセスへのアクセスは制限される。 | 資料が明記しているのは、整合性レベルが低い主体が、高いオブジェクトへ書き込めないことである。 | コメントの文面を、資料の表現に合わせる。 |
| 098 | 予約は offset 12 から 19 の 8 [byte]。 | offset 7 の 1 [byte] も予約で、扱いは同じである。 | 記載に offset 7 を含める。 |
| 116 | UTF-16 の単位数から UTF-8 のバイト数は求められない。 | 書けた範囲を UTF-8 へ変換し直せば、バイト数は求められる。現在の実装は、換算をしていない。 | コメントの文面を「UTF-16 の単位数と UTF-8 のバイト数は比例しないため、換算せず、部分書き込みでは 0 を格納する」とする。 |
| 124 | huge 測定は、通常サイズの測定の失敗が 0 件であることを条件にする。 | 失敗の件数は、`--sizes` の不正な指定でも増える。条件は、「それまでの処理の失敗が 0 件」である。 | コメントと文書の文面を、訂正後の条件にする。 |
| 135 | 上界の根拠は zlib Manual。 | 上界の式は、zlib のソース (`deflate.c` の `deflateBound()`) にある。zlib Manual は、`deflateBound()` が上界を返すことを述べている。 | コメントに式を書き、`see:` は zlib Manual とする。 |
| 145 | 500 [ms] は、ヘルス チェックが無効のときの値。 | `health_timeout_ms` が 0 の場合に加えて、役割が SENDER で種別が `unicast_bidir` 以外の場合も、500 [ms] になる。 | 定数名と説明を、「受信期限を監視しない場合」とする。 |
| 153、154 | backlog は、全ワーカーが処理中のときに接続を待たせるキューの長さ。 | 同じ `listen()` を、ワーカーのない fork モードも使う。「全ワーカーが処理中」は、prefork モードだけの説明である。 | コメントの文面を、prefork モードの説明と分かる形にする。 |
| 155、156 | 10 [ms] は、新しい接続の受け取りの遅れの上界。 | 接続が 1 本以上のときは、待機の後に受信と送信の処理時間が加わる。10 [ms] は、待機による遅れの上界である。 | コメントの文面を「待機による遅れの上界」とする。 |
| 159 | `dwWaitHint` の直値は 3000 と 5000。 | 直値は 4 か所で、3000 は起動の通知にも使われている。`on_run()` が停止要求なしに戻った場合は、checkpoint を 2 にする通知が、最初の `SERVICE_STOP_PENDING` になる。 | 定数は役割ごとに 3 個とする。コメントとテストで、停止要求なしの経路を明記する。 |

Table: 事実の訂正

決定の文面と実装の構造が合わなかった点が、1 つあります。  
105 の決定は、強制出力のレベルを EventLog へ渡す前に変換することです。ETW と syslog の変換は、バックエンドではなく `tracer.c` にあります。決定で定めたテスト (`traceTest`) は EventLog の書き込みの関数を差し替えるため、変換を `trace_eventlog.c` に置くと、`traceTest` で確かめられません。  
最終仕様では、ETW、syslog と同じ `tracer.c` に変換を置きます。

## 対処計画

### 進め方

対処は、1 つの案件で 57 件すべてを扱い、類型の順に進めます。

1. 確認項目の回答で、項目ごとの対処の内容を確定します。動作を変えない 40 件は [Q-002](261120-source-comment-bl003-remaining-57-discussion.md#q-002-動作を変えない-40-件の適用内容) でまとめて確認し、残る 17 件は 1 件ずつ確認します。
2. 最終仕様を作成し、承認を受けます。
3. 類型 1 から 3 (動作を変えない 45 件) を、app ごとにまとめて反映します。コメント、Doxygen、文書、定数化が中心で、公開ヘッダーを変える項目は `api-cheatsheet.md` と機能仕様を同じ変更で確認します。
4. 類型 4 (動作が変わる 11 件) を、1 件ずつ実装とテストの追加で反映します。
5. `app/porter` の 4 件は、フロー制御の作業と変更が重ならない時期に反映します ([Q-003](261120-source-comment-bl003-remaining-57-discussion.md#q-003-porter-の-4-件を変更する時期))。

### 変更の規模の見込み

| app | 件数 | 主な変更先 |
| --- | --- | --- |
| `app/cplat` | 40 | `prod/libsrc/cplat` の各モジュール、公開ヘッダー、`bench-io`、`bench-hashtable`、`compress-cli`、`docs/api-cheatsheet.md`、テスト |
| `app/porter` | 8 | `potr_service_open.c`、`potr_recv_thread.c`、`potr_send_thread.c`、`tcpServer`、`docs/config.md`、`docs/health-check.md`、`docs/architecture.md`、テスト |
| `app/service-sample` | 3 | `service-sample_linux.c`、`service-sample_linux_events.c`、`service-sample_windows.c`、`docs/service-events.md`、テスト |
| `app/calc` | 2 | `calc.c`、`shared-and-static-calc.c` |
| `app/string-catalog-sample` | 2 | `string-catalog-library-sample.c`、README |
| `app/struct-meta` | 2 | `struct_meta_gen_emit.c`、`catalog.c`、`index.c`、`docs/architecture.md` |

Table: app ごとの変更の規模の見込み

テストの追加が要る見込みの項目は、類型 4 の 11 件と、類型 3 の 017、141 です。  
このうち 020、102、105、159 は Windows だけで実行されるテストです。129 は Linux だけで実行されるテストで、CI で確認します。

### 調査で見つけた対象外の気付き

57 件の調査の中で、57 件には含まれない気付きが 10 件ありました。  
今回の対象には含めず、この案件の残課題一覧 (`261120-source-comment-bl003-remaining-57-backlog.md`) に記録しています。

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | 対策元 backlog の BL-003。保守者が、値と分岐を変更できる範囲を判断できる状態にする。 |
| 範囲・境界 | 確認済み | 57 件すべてが対象 (ユーザーの依頼)。動作を変える 11 件の修正も含める ([Q-001](261120-source-comment-bl003-remaining-57-discussion.md#q-001-動作を変える修正を今回の案件に含めるか))。決定の対象と同じ記載と、同じ段落の事実と合わない記載も直す ([Q-027](261120-source-comment-bl003-remaining-57-discussion.md#q-027-決定の対象と同じ記載や同じ段落の事実と合わない記載の扱い))。020 は、公開 API が失敗を返す範囲を、検出できる失敗のすべてへ広げる ([Q-030](261120-source-comment-bl003-remaining-57-discussion.md#q-030-020-公開-api-が失敗を返す範囲))。57 件に含まれない気付きは、残課題一覧へ記録する。 残課題一覧のうち対処できるものも対象に含める (簡易 ADR の LADR-026)。対処する項目と内容は、調査の後に確認項目で決める。 規範の例外の定め方と、enum にする範囲は [Q-047](261120-source-comment-bl003-remaining-57-discussion.md#q-047-規範の例外-容量を拡張する倍率の-2) から [Q-051](261120-source-comment-bl003-remaining-57-discussion.md#q-051-排他的な値の集合を-enum-にする範囲)。 |
| 入力・根拠 | 確認済み | 57 件のコード、履歴、文書、一次資料を調査済み (57 件の分析結果)。対象の行、貼り付ける文面、テストの手段、`see:` の URL の内容を確認済み (実装レベルの調査で分かった事実の訂正)。実機での確認はしていない。021 の実機での再現は行わない ([Q-019](261120-source-comment-bl003-remaining-57-discussion.md#q-019-021-再接続後の書き込み失敗の調査))。 |
| 整合性・依存関係 | 確認済み | `app/porter` の通信ライブラリの 4 件は、フロー制御の変更が main へ入った後に変更する ([Q-003](261120-source-comment-bl003-remaining-57-discussion.md#q-003-porter-の-4-件を変更する時期))。定数は `#define` で定義し、規範との差は残課題とする ([Q-022](261120-source-comment-bl003-remaining-57-discussion.md#q-022-直値を名前付き定数にするときの表現))。公開ヘッダーへ書く契約 3 件は、機能仕様にも書く ([Q-026](261120-source-comment-bl003-remaining-57-discussion.md#q-026-公開ヘッダーへ書く契約を機能仕様にも書くか))。 先の決定 (039、181、Q-040) と新しい規範、設計との食い違いは [Q-047](261120-source-comment-bl003-remaining-57-discussion.md#q-047-規範の例外-容量を拡張する倍率の-2)、[Q-054](261120-source-comment-bl003-remaining-57-discussion.md#q-054-bl-007-on_run-の生存の判定)。 |
| 実現性・例外 | 確認済み | 動作を変える 11 件の修正の内容は、[Q-005](261120-source-comment-bl003-remaining-57-discussion.md#q-005-105-強制出力のレベルを-eventlog-へ渡すときの対応) から [Q-015](261120-source-comment-bl003-remaining-57-discussion.md#q-015-036-未設定の行を表すセンチネル値)、[Q-023](261120-source-comment-bl003-remaining-57-discussion.md#q-023-015-eai_system-のときの結果コード)、[Q-025](261120-source-comment-bl003-remaining-57-discussion.md#q-025-037-書式付きの-api-で書式バッファーを確保できない場合の扱い)、[Q-028](261120-source-comment-bl003-remaining-57-discussion.md#q-028-124-huge-測定を省略したときの通知の文面)、[Q-030](261120-source-comment-bl003-remaining-57-discussion.md#q-030-020-公開-api-が失敗を返す範囲)、[Q-031](261120-source-comment-bl003-remaining-57-discussion.md#q-031-020-接続できなかった場合と標準ストリームが不完全な場合の区別) の回答で確定した。102 と 020 のテストの手段は、調査で確認済みで、最終仕様に記載する。 残課題の対処の設計で、動作や公開の契約に関わる選択は [Q-050](261120-source-comment-bl003-remaining-57-discussion.md#q-050-公開ヘッダーの定数の型)、[Q-052](261120-source-comment-bl003-remaining-57-discussion.md#q-052-bl-006-昇格したプロセスが出力のパイプへ接続しなかった場合の戻り値) から [Q-058](261120-source-comment-bl003-remaining-57-discussion.md#q-058-getmodulefilenameu-が切り詰めた場合の戻り値)。 |
| 受入条件・検証 | 確認済み | エージェントが Windows/MSVC でビルドと局所テストを実行し、Linux/GCC は CI で確認する ([Q-004](261120-source-comment-bl003-remaining-57-discussion.md#q-004-ビルドと局所テストの実行担当))。受入条件は、最終仕様に記載する。 |

Table: 観点別の確認

## 質問一覧

各 Q の本文は検討用 Markdown にあります。

| Q-ID | 論点 | 状態 | 依存・関連 | 反映先の決定 |
| --- | --- | --- | --- | --- |
| [Q-001](261120-source-comment-bl003-remaining-57-discussion.md#q-001-動作を変える修正を今回の案件に含めるか) | 動作を変える修正を今回の案件に含めるか | 解決済み | Q-005 から Q-015 | [LADR-002](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-002-動作を変える修正を案件に含める) |
| [Q-002](261120-source-comment-bl003-remaining-57-discussion.md#q-002-動作を変えない-40-件の適用内容) | 動作を変えない 40 件の適用内容 | 解決済み | なし | [LADR-003](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-003-動作を変えない-40-件の適用内容) |
| [Q-003](261120-source-comment-bl003-remaining-57-discussion.md#q-003-porter-の-4-件を変更する時期) | porter の 4 件を変更する時期 | 解決済み | Q-002、Q-016 | [LADR-004](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-004-porter-の通信ライブラリの-4-件を変更する時期) |
| [Q-004](261120-source-comment-bl003-remaining-57-discussion.md#q-004-ビルドと局所テストの実行担当) | ビルドと局所テストの実行担当 | 解決済み | なし | [LADR-005](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-005-ビルドと局所テストの実行担当) |
| [Q-005](261120-source-comment-bl003-remaining-57-discussion.md#q-005-105-強制出力のレベルを-eventlog-へ渡すときの対応) | 105 強制出力のレベルを EventLog へ渡すときの対応 | 解決済み | Q-001 | [LADR-006](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-006-105-強制出力のレベルの-eventlog-への対応) |
| [Q-006](261120-source-comment-bl003-remaining-57-discussion.md#q-006-063-行幅を超える文字列の定数を持つ外部イメージの扱い) | 063 行幅を超える文字列の定数を持つ外部イメージの扱い | 解決済み | Q-001 | [LADR-007](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-007-063-行幅を超える文字列の定数を持つ外部イメージを拒否する) |
| [Q-007](261120-source-comment-bl003-remaining-57-discussion.md#q-007-084-外部イメージの非有限値の定数の扱い) | 084 外部イメージの非有限値の定数の扱い | 解決済み | Q-001 | [LADR-008](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-008-084-非有限値の定数を持つ外部イメージを拒否する) |
| [Q-008](261120-source-comment-bl003-remaining-57-discussion.md#q-008-020-標準ストリームの-fd-番号が一致しない場合の扱い) | 020 標準ストリームの FD 番号が一致しない場合の扱い | 解決済み | Q-001 | [LADR-009](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-009-020-標準ストリームの-fd-番号の不一致を失敗とする) |
| [Q-009](261120-source-comment-bl003-remaining-57-discussion.md#q-009-102-etw-の構造体と配列のプロパティの扱い) | 102 ETW の構造体と配列のプロパティの扱い | 解決済み | Q-001 | [LADR-010](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-010-102-etw-の構造体と配列のプロパティで読み取りを打ち切る) |
| [Q-010](261120-source-comment-bl003-remaining-57-discussion.md#q-010-129-cpu-名の先頭の空白の扱い) | 129 CPU 名の先頭の空白の扱い | 解決済み | Q-001 | [LADR-011](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-011-129-cpu-名の先頭の空白を除去する) |
| [Q-011](261120-source-comment-bl003-remaining-57-discussion.md#q-011-037-固定プロンプトの内部関数の結果コード) | 037 固定プロンプトの内部関数の結果コード | 解決済み | Q-001 | [LADR-012](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-012-037-固定プロンプトの内部関数を共通の結果コードへ統一する) |
| [Q-012](261120-source-comment-bl003-remaining-57-discussion.md#q-012-015-名前解決の失敗の結果コード) | 015 名前解決の失敗の結果コード | 解決済み | Q-001 | [LADR-013](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-013-015-名前解決の失敗の結果コードを-2-つ分ける) |
| [Q-013](261120-source-comment-bl003-remaining-57-discussion.md#q-013-124-測定の失敗後の-huge-測定の扱い) | 124 測定の失敗後の huge 測定の扱い | 解決済み | Q-001 | [LADR-014](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-014-124-huge-測定の省略を通知する) |
| [Q-014](261120-source-comment-bl003-remaining-57-discussion.md#q-014-159-停止の経路の-2-回目の通知の-checkpoint) | 159 停止の経路の 2 回目の通知の checkpoint | 解決済み | Q-001 | [LADR-015](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-015-159-停止の経路の-2-回目の通知の-checkpoint-を増やす) |
| [Q-015](261120-source-comment-bl003-remaining-57-discussion.md#q-015-036-未設定の行を表すセンチネル値) | 036 未設定の行を表すセンチネル値 | 解決済み | Q-001 | [LADR-016](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-016-036-未設定の行のセンチネル値を-int_max-にする) |
| [Q-016](261120-source-comment-bl003-remaining-57-discussion.md#q-016-143-重複している原子変数の初期化) | 143 重複している原子変数の初期化 | 解決済み | Q-003 | [LADR-017](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-017-143-重複している原子変数の初期化を削除する) |
| [Q-017](261120-source-comment-bl003-remaining-57-discussion.md#q-017-012-参照元が少ないヘッダーの-hideincludedbygraph) | 012 参照元が少ないヘッダーの hideincludedbygraph | 解決済み | なし | [LADR-018](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-018-012-参照元が少ないヘッダーのタグを削除する) |
| [Q-018](261120-source-comment-bl003-remaining-57-discussion.md#q-018-007-intellisense-の解析時に公開修飾を空にする分岐) | 007 IntelliSense の解析時に公開修飾を空にする分岐 | 解決済み | なし | [LADR-019](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-019-007-intellisense-の分岐には事実だけを書く) |
| [Q-019](261120-source-comment-bl003-remaining-57-discussion.md#q-019-021-再接続後の書き込み失敗の調査) | 021 再接続後の書き込み失敗の調査 | 解決済み | Q-008 | [LADR-020](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-020-021-再接続後の書き込み失敗は既知の制限として記載する) |
| [Q-020](261120-source-comment-bl003-remaining-57-discussion.md#q-020-116-コンソールへの部分書き込みの扱い) | 116 コンソールへの部分書き込みの扱い | 解決済み | なし | [LADR-021](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-021-116-コンソールへの部分書き込みの契約を維持する) |
| [Q-021](261120-source-comment-bl003-remaining-57-discussion.md#q-021-099-ロック-ファイルの共有削除の許可) | 099 ロック ファイルの共有削除の許可 | 解決済み | なし | [LADR-022](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-022-099-ロック-ファイルの共有削除の許可を維持する) |
| [Q-022](261120-source-comment-bl003-remaining-57-discussion.md#q-022-直値を名前付き定数にするときの表現) | 直値を名前付き定数にするときの表現 | 解決済み | Q-002、Q-014 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-023](261120-source-comment-bl003-remaining-57-discussion.md#q-023-015-eai_system-のときの結果コード) | 015 EAI_SYSTEM のときの結果コード | 解決済み | Q-012 | [LADR-013](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-013-015-名前解決の失敗の結果コードを-2-つ分ける) |
| [Q-024](261120-source-comment-bl003-remaining-57-discussion.md#q-024-020-fd-番号が一致しない場合の公開-api-の戻り値) | 020 FD 番号が一致しない場合の公開 API の戻り値 | 回答済み (意図不明。Q-029 として作り直し) | Q-008 | [LADR-009](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-009-020-標準ストリームの-fd-番号の不一致を失敗とする) |
| [Q-025](261120-source-comment-bl003-remaining-57-discussion.md#q-025-037-書式付きの-api-で書式バッファーを確保できない場合の扱い) | 037 書式付きの API で書式バッファーを確保できない場合の扱い | 解決済み | Q-011 | [LADR-012](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-012-037-固定プロンプトの内部関数を共通の結果コードへ統一する) |
| [Q-026](261120-source-comment-bl003-remaining-57-discussion.md#q-026-公開ヘッダーへ書く契約を機能仕様にも書くか) | 公開ヘッダーへ書く契約を機能仕様にも書くか | 解決済み | Q-002、Q-021 | [LADR-024](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-024-公開ヘッダーへ書く契約を機能仕様にも書く) |
| [Q-027](261120-source-comment-bl003-remaining-57-discussion.md#q-027-決定の対象と同じ記載や同じ段落の事実と合わない記載の扱い) | 決定の対象と同じ記載や、同じ段落の事実と合わない記載の扱い | 解決済み | Q-002、Q-019、Q-021 | [LADR-025](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-025-決定の対象と同じ記載と同じ段落の相違も直す) |
| [Q-028](261120-source-comment-bl003-remaining-57-discussion.md#q-028-124-huge-測定を省略したときの通知の文面) | 124 huge 測定を省略したときの通知の文面 | 解決済み | Q-013 | [LADR-014](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-014-124-huge-測定の省略を通知する) |
| [Q-029](261120-source-comment-bl003-remaining-57-discussion.md#q-029-020-fd-番号が一致しない場合に公開-api-でアプリへ伝えるか) | 020 FD 番号が一致しない場合に公開 API でアプリへ伝えるか | 回答済み (推奨の根拠の質問。Q-030、Q-031 で確定) | Q-008、Q-024 | [LADR-009](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-009-020-標準ストリームの-fd-番号の不一致を失敗とする) |
| [Q-030](261120-source-comment-bl003-remaining-57-discussion.md#q-030-020-公開-api-が失敗を返す範囲) | 020 公開 API が失敗を返す範囲 | 解決済み | Q-008、Q-029 | [LADR-009](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-009-020-標準ストリームの-fd-番号の不一致を失敗とする) |
| [Q-031](261120-source-comment-bl003-remaining-57-discussion.md#q-031-020-接続できなかった場合と標準ストリームが不完全な場合の区別) | 020 接続できなかった場合と標準ストリームが不完全な場合の区別 | 解決済み | Q-030 | [LADR-009](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-009-020-標準ストリームの-fd-番号の不一致を失敗とする) |
| [Q-032](261120-source-comment-bl003-remaining-57-discussion.md#q-032-bl-001-モジュールのパスの取得で対応する長さ) | BL-001 モジュールのパスの取得で対応する長さ | 解決済み | なし | [LADR-027](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-027-bl-001-モジュールのパスを-os-の上限まで取得できるようにする) |
| [Q-033](261120-source-comment-bl003-remaining-57-discussion.md#q-033-bl-002-書式バッファーの再確保の失敗の扱い) | BL-002 書式バッファーの再確保の失敗の扱い | 解決済み | Q-025 | [LADR-028](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-028-bl-002-書式バッファーの再確保の失敗を初回の確保の失敗と同じ扱いにする) |
| [Q-034](261120-source-comment-bl003-remaining-57-discussion.md#q-034-bl-015-入力履歴の確保の失敗の結果コード) | BL-015 入力履歴の確保の失敗の結果コード | 解決済み | Q-011 | [LADR-029](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-029-bl-015-入力履歴の確保の失敗は-cplat_err_out_of_memory-を返す) |
| [Q-035](261120-source-comment-bl003-remaining-57-discussion.md#q-035-bl-010-測定できない上限の指定の扱い) | BL-010 測定できない上限の指定の扱い | 解決済み | なし | [LADR-030](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-030-bl-010-測定できない上限の指定を失敗にする) |
| [Q-036](261120-source-comment-bl003-remaining-57-discussion.md#q-036-bl-021-ヘルプの文面と定数の連動) | BL-021 ヘルプの文面と定数の連動 | 解決済み | Q-002、Q-035 | [LADR-031](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-031-bl-021-ヘルプの文面を実行時に定数から組み立てる) |
| [Q-037](261120-source-comment-bl003-remaining-57-discussion.md#q-037-bl-022-整数の定数の規範と実態の差の合わせ方) | BL-022 整数の定数の規範と実態の差の合わせ方 | 回答済み (Q-043、Q-044 で確定) | Q-022 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-038](261120-source-comment-bl003-remaining-57-discussion.md#q-038-bl-006-別のアカウントでの昇格への対処の範囲) | BL-006 別のアカウントでの昇格への対処の範囲 | 解決済み | なし | [LADR-032](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-032-bl-006-昇格の出力の受け渡しを名前付きパイプへ変える) |
| [Q-039](261120-source-comment-bl003-remaining-57-discussion.md#q-039-bl-008-イベント監視スレッドが時間内に終わらない場合の扱い) | BL-008 イベント監視スレッドが時間内に終わらない場合の扱い | 回答済み (待機の時間は Q-042 で確定) | Q-042 | [LADR-033](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-033-bl-008-イベント監視スレッドが-30-s-で終わらない場合は異常終了させる) |
| [Q-040](261120-source-comment-bl003-remaining-57-discussion.md#q-040-bl-007-イベント監視スレッドと-watchdog-の扱い) | BL-007 イベント監視スレッドと watchdog の扱い | 解決済み | なし | [LADR-034](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-034-bl-007-起動の失敗をサービスの起動の失敗にしwatchdog-を-on_run-の生存に結び付ける) |
| [Q-041](261120-source-comment-bl003-remaining-57-discussion.md#q-041-残課題の調査で見つかった新しい気付きの扱い) | 残課題の調査で見つかった新しい気付きの扱い | 解決済み | Q-032 | [LADR-035](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-035-残課題の調査で見つかった新しい気付き-10-件も対象に含める) |
| [Q-042](261120-source-comment-bl003-remaining-57-discussion.md#q-042-bl-008-イベント監視スレッドの終了を待つ時間) | BL-008 イベント監視スレッドの終了を待つ時間 | 解決済み | Q-039 | [LADR-033](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-033-bl-008-イベント監視スレッドが-30-s-で終わらない場合は異常終了させる) |
| [Q-043](261120-source-comment-bl003-remaining-57-discussion.md#q-043-bl-022-新しい規範に入れる規則) | BL-022 新しい規範に入れる規則 | 解決済み | Q-037 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-044](261120-source-comment-bl003-remaining-57-discussion.md#q-044-bl-022-全ソースの確認の範囲と不適合の扱い) | BL-022 全ソースの確認の範囲と不適合の扱い | 解決済み | Q-037、Q-043 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-045](261120-source-comment-bl003-remaining-57-discussion.md#q-045-bl-022-全ソースの確認の範囲と不適合の扱いの再確認) | BL-022 全ソースの確認の範囲と不適合の扱いの再確認 | 解決済み (Q-044 の決定を維持) | Q-044 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-047](261120-source-comment-bl003-remaining-57-discussion.md#q-047-規範の例外-容量を拡張する倍率の-2) | 規範の例外 容量を拡張する倍率の 2 | 解決済み | Q-002、Q-043 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-048](261120-source-comment-bl003-remaining-57-discussion.md#q-048-規範の例外-文字列リテラルの長さから決まる値) | 規範の例外 文字列リテラルの長さから決まる値 | 解決済み | Q-043 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-049](261120-source-comment-bl003-remaining-57-discussion.md#q-049-規範の例外-文字符号の規格が定める値) | 規範の例外 文字符号の規格が定める値 | 解決済み | Q-043 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-050](261120-source-comment-bl003-remaining-57-discussion.md#q-050-公開ヘッダーの定数の型) | 公開ヘッダーの定数の型 | 解決済み | Q-043、Q-044 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-051](261120-source-comment-bl003-remaining-57-discussion.md#q-051-排他的な値の集合を-enum-にする範囲) | 排他的な値の集合を enum にする範囲 | 解決済み | Q-043 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-052](261120-source-comment-bl003-remaining-57-discussion.md#q-052-bl-006-昇格したプロセスが出力のパイプへ接続しなかった場合の戻り値) | BL-006 昇格したプロセスが出力のパイプへ接続しなかった場合の戻り値 | 解決済み | Q-038 | [LADR-032](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-032-bl-006-昇格の出力の受け渡しを名前付きパイプへ変える) |
| [Q-053](261120-source-comment-bl003-remaining-57-discussion.md#q-053-bl-006-名前付きパイプの保護) | BL-006 名前付きパイプの保護 | 解決済み | Q-038、Q-052 | [LADR-032](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-032-bl-006-昇格の出力の受け渡しを名前付きパイプへ変える) |
| [Q-054](261120-source-comment-bl003-remaining-57-discussion.md#q-054-bl-007-on_run-の生存の判定) | BL-007 on_run の生存の判定 | 解決済み | Q-040 | [LADR-034](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-034-bl-007-起動の失敗をサービスの起動の失敗にしwatchdog-を-on_run-の生存に結び付ける) |
| [Q-055](261120-source-comment-bl003-remaining-57-discussion.md#q-055-service-sample-の-linux-で-on_reload-がない場合の-sighup) | service-sample の Linux で on_reload がない場合の SIGHUP | 解決済み | Q-040、Q-041 | [LADR-034](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-034-bl-007-起動の失敗をサービスの起動の失敗にしwatchdog-を-on_run-の生存に結び付ける) |
| [Q-056](261120-source-comment-bl003-remaining-57-discussion.md#q-056-service-sample-の-linux-のコールバックの呼び出しの順序) | service-sample の Linux のコールバックの呼び出しの順序 | 解決済み | Q-041 | [LADR-034](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-034-bl-007-起動の失敗をサービスの起動の失敗にしwatchdog-を-on_run-の生存に結び付ける) |
| [Q-057](261120-source-comment-bl003-remaining-57-discussion.md#q-057-windows-で-utf-8-のパスを受けるバッファーの大きさ) | Windows で UTF-8 のパスを受けるバッファーの大きさ | 解決済み | Q-032、Q-041 | [LADR-027](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-027-bl-001-モジュールのパスを-os-の上限まで取得できるようにする) |
| [Q-058](261120-source-comment-bl003-remaining-57-discussion.md#q-058-getmodulefilenameu-が切り詰めた場合の戻り値) | GetModuleFileNameU が切り詰めた場合の戻り値 | 解決済み | Q-041 | [LADR-035](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-035-残課題の調査で見つかった新しい気付き-10-件も対象に含める) |
| [Q-059](261120-source-comment-bl003-remaining-57-discussion.md#q-059-公開ヘッダーの中で使う文字符号の値) | 公開ヘッダーの中で使う文字符号の値 | 解決済み | Q-049 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-060](261120-source-comment-bl003-remaining-57-discussion.md#q-060-guid-の数値の組と文字列の重複) | GUID の数値の組と文字列の重複 | 解決済み | Q-043 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-061](261120-source-comment-bl003-remaining-57-discussion.md#q-061-内訳と合わない余裕を持つバッファーの大きさ) | 内訳と合わない余裕を持つバッファーの大きさ | 解決済み | Q-038、Q-043 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-062](261120-source-comment-bl003-remaining-57-discussion.md#q-062-c-の製品コードの定数の表現) | C++ の製品コードの定数の表現 | 解決済み | Q-037、Q-043 | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-063](261120-source-comment-bl003-remaining-57-discussion.md#q-063-potr_nack_dedup_slots-の値とコメントの食い違い) | POTR_NACK_DEDUP_SLOTS の値とコメントの食い違い | 解決済み | なし | [LADR-023](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-023-直値を名前付き定数にするときの表現) |
| [Q-064](261120-source-comment-bl003-remaining-57-discussion.md#q-064-health-checkmd-の待機の上限の記載の範囲) | health-check.md の待機の上限の記載の範囲 | 解決済み | Q-002 | [LADR-003](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-003-動作を変えない-40-件の適用内容) |
| [Q-065](261120-source-comment-bl003-remaining-57-discussion.md#q-065-appporter-のほかのファイルにある重複したゼロの代入) | app/porter のほかのファイルにある重複したゼロの代入 | 解決済み | Q-016 | [LADR-017](261120-source-comment-bl003-remaining-57-lightadr.md#ladr-017-143-重複している原子変数の初期化を削除する) |

Table: 質問一覧

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261120-source-comment-bl003-remaining-57-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 12 (2026-11-21 13:40 JST に、残懸念がないことを確認)。以前の確認: 版 4 (2026-11-21 00:33 JST に、残懸念がないことを確認。版 2 での確認は、追加の確認項目の提示により取り消した)

最終仕様 (design.md) の合意版 (第 2 ゲート): 2026-11-21 13:43 承認 (取り込みの後の再確認と、Q-062 から Q-065 の決定を反映した版。3055 行)

更新予定の成果物: 最終仕様の「変更するファイル」の節のとおりです。

受入条件: 最終仕様の「受入条件」の節のとおりです。

## 経緯

| 日時 (JST) | 出来事 | 内容・参照先 |
| --- | --- | --- |
| 2026-11-20 08:30 | 案件の開始 | ユーザーの依頼「BL-003 の対処を行う。57 件を詳細に分析し、対処計画を立ててください。」を受け、57 件の調査を開始。対象の決定は簡易 ADR の LADR-001。 |
| 2026-11-20 08:55 | 確認項目の提示 | 57 件の分析を終え、全体管理文書、検討用 Markdown (Q-001 から Q-021)、簡易 ADR、残課題一覧を作成。対策元 backlog の BL-003 を「検討中」へ更新。 |
| 2026-11-20 16:48 | 回答の受付 | Q-001 から Q-021 のすべてに回答があった (すべて推奨案。補足の記入はなし)。回答を検証し、簡易 ADR の LADR-002 から LADR-022 へ決定を記録した。検討用 Markdown を版 2 とした。 |
| 2026-11-20 16:54 | 第 1 ゲートの通過 | 未回答と未解決の Q がなく、6 つの観点がすべて確認済みであることを確認した。簡易 ADR の決定と回答の一致を確認し、最終仕様の作成に着手した。 |
| 2026-11-20 18:11 | 追加の確認項目の提示、第 1 ゲートの取り消し | 最終仕様の作成のために、57 件の対象の行、貼り付ける文面、テストの手段、`see:` に使う URL の内容を調べた。決定だけでは決まらない判断が 7 件見つかったため、Q-022 から Q-028 を提示し、検討用 Markdown を版 3 とした。第 1 ゲートの通過を取り消し、最終仕様の作成は回答の後に行う。調査で分かった事実の訂正は、「実装レベルの調査で分かった事実の訂正」に記録した。 |
| 2026-11-20 19:19 | フロー制御のセッションからの連絡 | フロー制御の変更 (最終仕様の版 5 まで) が、`app/porter` の main へ入った (`app/porter` は `d3abc97`、`framework/makefw` は `cefcba4`、ルートは `e21fab2f`。CI は run 44718213976 で実行中)。フロー制御の作業は続き、版 6 と版 7 で `potr_service_open.c`、`potr_context.h`、`docs/config.md` などを変更する見込みである。この 3 つは、141、143、146 の対象と重なる。相手のセッションは、通信ライブラリの 4 件に着手する前の連絡を求めている。4 件の対象の行は、着手の前に、その時点の main で読み直す。この作業ツリーは、相手のプッシュを取り込んでいない (`app/porter` は `0c0b61d`)。 |
| 2026-11-21 00:33 | 追加の回答の受付 | ユーザーの指示 (`q-022以降を対話のqaで決定したい`) により、Q-022 以降の回答を、対話の質問ダイアログで受けた。Q-022、Q-023、Q-025 から Q-028 は推奨案 (A)。Q-024 は意図不明とされ、Q-029 として作り直した。Q-029 では、推奨の根拠と、堅牢で正しい基盤としての推奨を問われた。エージェントは、A の推奨が変更の範囲を小さく保つ理由によるものだったことを説明し、検出できる失敗のすべてを公開 API の失敗として返す案を推奨し直して、Q-030 と Q-031 を提示した。両方とも A の回答を受けた。決定を簡易 ADR の LADR-009、LADR-012、LADR-013、LADR-014、LADR-023、LADR-024、LADR-025 へ記録し、検討用 Markdown を版 4 とした。 |
| 2026-11-21 00:33 | 第 1 ゲートの通過 | 未回答と未解決の Q がなく、6 つの観点がすべて確認済みであることを確認した。簡易 ADR の決定と回答の一致を確認し、最終仕様の作成に着手した。 |
| 2026-11-21 00:40 | 最終仕様の作成、作業 WBS の見直し | 最終仕様を作成した。作業 WBS の項目 4 を、最終仕様の「作業の順序」に合わせて 10 項目に分けた。第 2 ゲートの承認待ち。 |
| 2026-11-21 06:35 | 範囲の追加、フェーズ 1 への差し戻し | 最終仕様の提示に対し、ユーザーが `対処可能な残課題は対処したい` と指示した。残課題一覧の 22 件のうち、対処できるものをこの案件の対象に含める (簡易 ADR の LADR-026)。第 1 ゲートの通過を取り消し、22 件の対処案の調査を始めた。最終仕様の承認は、残課題の対処を追記した後に受ける。 |
| 2026-11-21 07:09 | 残課題の対処の回答の受付 | 残課題一覧の 22 件の対処案を調べた。`potr_service_close.c` の 0 の格納 (BL-020 の一部) は、問題ではないと分かった。判断が要る項目を Q-032 から Q-044 として対話で確認し、回答を受けた (検討用 Markdown の版 5)。Q-037 では、規範を改め、正しさを強化した規則を加えて、全ソースの適合を確認するよう指示を受けた。Q-038 では、推奨と異なり、方式の変更 (名前付きパイプ) まで行う回答を受けた。Q-039 では、待機の時間について意見を求められ、30 [s] の根拠を示して Q-042 で確定した。決定を簡易 ADR の LADR-023、LADR-027 から LADR-035 へ記録した。 |
| 2026-11-21 07:09 | 作業の中断 | 回答で決まった対処の設計の調査 (3 つ) と、新しい規範への適合の確認 (4 つ) を、サブエージェントで始めた。利用上限 (2026-11-23 06:00 JST に解除) に達し、7 つのうち 6 つが途中で止まった。作業 WBS の 1.13 と 1.14 をブロック中とした。再開のときは、各調査を最初の依頼の内容でやり直すか、止まったサブエージェントを続行させる。 |
| 2026-11-21 07:18 | Q-044 の再確認 | ユーザーが「Q-044 は回答を誤った可能性がある」と述べたため、Q-045 として問い直した。ユーザーは、Q-044 の選択 A を変えず、補足欄へ、テスト コードを対象外とする理由 (テスト コードでは直値が推奨される) を記入した。決定は変わらない。検討用 Markdown を版 6 とした。 |
| 2026-11-21 07:23 | Q-044 の再確認の重複 | ユーザーから、Q-044 をもう一度問い合わせる指示が再び届いたため、対話で問い直した。ユーザーは、`Q-046は回答不要と考えます。(指示の重複) Q-045までで一貫しています` と答えた。指示の重複であり、決定は Q-045 までのとおりである。検討用 Markdown へ Q は追加していない。 |
| 2026-11-21 07:38 | 作業の再開、調査の完了 | 止まっていたサブエージェント 6 つを再開し、設計の調査 3 つと、規範への適合の確認 4 つが完了した。適合の確認の指摘は 594 行で、判断が要る 308 行の大半は、規範の例外の定め方 (2 の扱い、倍率、文字符号の値、文字列の長さから決まる値、API が定める値) で決まる。先の決定のうち 041 と 107 (コメントの追加だけ) は、新しい規範では直値が残るため、定数にする変更へ改める。 |
| 2026-11-21 07:43 | 確認項目の提示 | 残課題の対処の設計の調査と、新しい規範への適合の確認の結果から、判断が要る 12 件を Q-047 から Q-058 として提示した (検討用 Markdown の版 7)。対話の質問ダイアログでの確認を始めたところ、ユーザーから `md方式でq and aしたい` と指示を受けたため、検討用 Markdown への記入で回答を受ける。設計の細部 (テストの方式、確認の周期、mock の追加の範囲など) は、確認項目にせず、最終仕様に書いて第 2 ゲートで確認を受ける。調査で見つかった対象外の気付き 16 件を、残課題一覧の BL-023 から BL-038 へ記録した。 |
| 2026-11-21 08:48 | 回答の受付 | Q-047 から Q-058 の回答を、検討用 Markdown への記入で受けた (版 8)。Q-051 は A (内部も公開ヘッダーも enum にする。推奨は B)、ほかは推奨案。決定を簡易 ADR の LADR-023、LADR-027、LADR-032、LADR-034、LADR-035 へ補足した。ユーザーの指示: 調査は `agy --dangerously-skip-permissions` で `gemini-4-hydrogen-high` を使い、このセッションは管理を行う。 |
| 2026-11-21 09:04 | 指摘の分け直しの完了、確認項目の提示 | `agy` による分け直しが完了した。直す箇所は 623 行 (R1 67、R2 79、R3 45、R4 36、R5 396)。判断が要る 6 行のうち、3 行は設計で決める (`trace_syslog.c` のコメントの RFC の番号は、資料を確かめて事実に合わせる。`bench_case.h` のパディングは `static_assert` を足して直値を残す。`elevated_process.c` の内部フラグの文字数は、Q-061 に含めた)。残る判断を Q-059 から Q-061 として提示した (検討用 Markdown の版 9)。 |
| 2026-11-21 09:14 | 回答の受付、第 1 ゲートの通過 | Q-059 から Q-061 の回答を受けた (3 件とも推奨案。検討用 Markdown の版 10)。未回答と未解決の Q がなく、6 つの観点が確認済みであることを確かめ、最終仕様への追記に着手した。 |
| 2026-11-21 09:35 | 最終仕様の統合 | `agy` が、追記の下書き 3 つ (計 1859 行) と統合を行った。最終仕様は 14 章、2936 行になった。付録に、規範への適合の修正 623 行を置いた。分け直しで判断が要るとされた 6 行のうち、確認項目にしなかった 2 行 (`trace_syslog.c`、`bench_case.h`) の扱いは、このセッションが最終仕様へ書いた。第 2 ゲートの承認待ち。 |
| 2026-11-21 12:27 | 取り込みと方針の再確認 | 最終仕様の提示に対し、ユーザーが、取り込み (pull)、差分と新しいソースの確認、方針の再確認、`make skills` の実行を指示した。取り込みは早送りで完了した。最終仕様の 14 項目を現在のソースで確かめ直し、前提が崩れた項目はなかった (行番号の修正が 4 点)。`app/porter` の規範への適合の確認は、ライブラリが 83 行 (直す 82、判断が要る 1)、コマンドが 181 行 (直す 61、判断が要る 120。大半は C++ の `porter-bench.cc`)。更新されたスキルの手順 (対話で問い合わせる前に、未回答の質問を文書へ書く) に従い、判断が要る 4 件を Q-062 から Q-065 として先に文書へ書いた (検討用 Markdown の版 11)。 |
| 2026-11-21 13:25 | 回答の受付 | ユーザーの指示 (`対話で残課題を評価したい`) により、Q-062 から Q-065 を対話の質問ダイアログで確認し、4 件とも推奨案の回答を受けた。検討用 Markdown への記入は、エージェントが行った (版 12)。 |
| 2026-11-21 13:40 | 最終仕様の改訂、第 1 ゲートの通過 | `agy` が、取り込みの後の行番号、C++ の定め、`health-check.md` の 6 つの条件、重複したゼロの代入の追加の 2 ファイル、`POTR_NACK_DEDUP_SLOTS` の節、`app/porter` の規範への適合の結果を、最終仕様へ反映した。動作を変える項目は 12 件、規範への適合の修正は全 app で 893 行になった。`porter-bench.cc` の時間の定数は、`std::chrono` の型を持つ `constexpr` とする (このセッションが設計で決めた)。付録へ `app/porter` の 264 行を足した。第 2 ゲートの承認待ち。 |
| 2026-11-21 13:43 | 第 2 ゲートの承認 | ユーザーが最終仕様を承認した。ユーザーの指示: `agy` が利用上限に達した場合は、Opus または Sonnet へ切り替える。全体の管理と審査は、このセッション (Fable) が行う。作業 WBS の項目 4 を、作業の単位へ分け直した。項目 3.2 の未確認の事項 (全文を 1 行ずつ読んでいない) は、実装の審査で、項目ごとにソースと決定で確かめる。 |
| 2026-11-21 18:55 | 並行の実装の指示 | ユーザーの指示: `もう1並列、Opusを利用して良い。` サンプルの app 6 個の実装を、Opus のサブエージェントで、`app/cplat` の実装と並行して進める。 |
| 2026-11-21 19:50 | 取り込みとプッシュの指示 | ユーザーの指示: `リモートの取り込みや、mainへのプッシュしてのCI起動と追跡、CIエラーや警告の対処は自走してよい。` この案件の変更のコミット、`main` へのプッシュ、CI の追跡、CI のエラーと警告の対処を、個別の確認なしで行う (コミットはプッシュの前提として含むと解釈した)。指示を受けて、ルートを `b05af0fd`、`app/porter` を `26fa169` へ早送りで取り込んだ (差分は `app/porter` の検討文書だけ)。 |

Table: 経緯

## 反映と検証の記録

未着手。作業後の対応箇所、検証結果、未完了項目を記録します。
