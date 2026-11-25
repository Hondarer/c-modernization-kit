<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新日時: 2026-11-25 21:41:25 JST
最終更新エージェント: claude code (opus サブエージェント、主セッション session_01KJc53UFydnbt2gLXTJC1ZC)
稼働 OS: Linux (WSL)
上流文書: `261125-consumable-backlog-items-overview.md` (全体管理文書)。削除されている場合があります。
下流文書: `261125-consumable-backlog-items-lightadr.md` (簡易 ADR)、`261125-consumable-backlog-items-design.md` (最終仕様)、`261125-consumable-backlog-items-backlog.md` (残課題一覧、未作成)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、上流文書の全体管理文書が存在する場合は、その再開手順に従って作業 WBS の `← **CURRENT**` の項目から再開します。存在しない場合は、各 Q の **Status** から未回答と未解決の Q を特定し、扱いをユーザーに確認してください。この文書には Q&A の本文だけを置き、新しい Q は末尾へ追記します。作業 WBS、質問一覧、観点別の確認などの管理情報をこの文書へ追加しないでください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 消化できる残課題の対策 成果物実装前の検討

ファイル名: `261125-consumable-backlog-items-discussion.md`

## 明確化のための確認項目

課題 ID (BL-xxx) は、すべて対策元 `261123-remaining-backlog-items-backlog.md` の課題 ID です。事実は、エージェント (claude code、sonnet サブエージェント) が 2026-11-25 20:35 ごろの作業ツリー (HEAD `9d37dce0`) を読んで確かめたものです。

範囲 ([Q-001](#q-001-対策する残課題の範囲)) の回答後に、範囲外になった課題の質問を取り下げ、必要な詳細質問を追加します。Q-003 から Q-015 は、範囲に入った場合に回答が要る質問です。

### Q-001 対策する残課題の範囲

**Status**

解決済み

**Question**

この案件で対策する残課題 (BL) を、すべて選んでください。

**Context**

- **経緯と背景**: ユーザーの指示は「消化可能なバックログを消化していきたい」です。`makefw` 系の BL-002、BL-029、BL-030 は別案件で扱い、実機や CI での計測が要る項目は対象外です。残る候補は 14 件で、実装と検証の場所で 3 群に分かれます。
- **根拠資料**: [対策元の残課題一覧](261123-remaining-backlog-items-backlog.md)
- **各候補の規模と依存関係**:

Table: 範囲の候補の規模と依存関係

| 課題 ID | 規模 | 依存関係と備考 |
| --- | --- | --- |
| BL-031 | 小 | `struct_meta_parse.y` の行番号の取得の修正と、期待値を誤った行 (5) で書いた既存テスト 2 件の修正。ほかの BL に依存しません ([Q-008](#q-008-struct-meta-のフィールド診断の行番号の修正方式)、[Q-009](#q-009-フィールド診断で報告する行の定義))。 |
| BL-013 | 小から中 | 5 項目の値域の検査と、待機の関数へ渡る経路の確認。`thread_connect_prepare.c` の `(int)timeout_ms` と `potr_send_thread.c` の `pack_wait_ms` は、2,147,483,647 以上で符号付き整数の範囲を超える経路です ([Q-007](#q-007-待機時間の項目の上限の検査方式))。 |
| BL-012 | 小から中 | cJSON の生成元のパッチを 1 つ追加する案が中心です。Windows の DLL でもビルドが通る必要があります ([Q-006](#q-006-cjson-の直接解析と-global_error-の競合の対策方式))。 |
| BL-006 | 中 | `cplat_argparser_init` を `int` 戻り値にする案では、公開 API、mock、export テスト、文書と、呼び出し元 35 箇所を同じ変更で移行します。mock のヘッダーを変えるため、提供元 app で make してから利用側をテストします ([Q-003](#q-003-cplat_tracer_dispose-の失敗の通知方式)、[Q-004](#q-004-cplat_argparser_init-の失敗の通知方式))。 |
| BL-008 | 中 | `app/porter` を中心にした寿命の契約の追加。cplat の終了処理 (`cplat_shutdown_register` の LIFO 順) に依存します ([Q-005](#q-005-終了後に残るトレーサーのハンドルの扱い))。 |
| BL-001 | 中から大 | `framework/testfw` にヘルパーを新設し、`app/porter/test` の固定ポート約 10 箇所を移行します。Linux と Windows の両方で動く必要があり、Windows の検証が要ります ([Q-010](#q-010-テスト用ポートの払い出しの共有方式)、[Q-011](#q-011-テスト用ポートの払い出し範囲))。 |
| BL-005 | 小から中 | `cplat_thread_timed_join` がすでにあるため、新しい OS API は要りません。Windows のモック単体テストで検証します ([Q-012](#q-012-etw-停止で両方失敗した場合の待機の打ち切り方式))。 |
| BL-007 | 小 | 恒久対応案は「なし」です。BL-005 の対策のモック単体テストで経路を通し、実機の確認が要らないことを整理してクローズします。単独では対策になりません。 |
| BL-004 | 中 | `stdio_temp.c` と `elevated_process.c` の 2 箇所で `GetTempFileNameW` を置き換えます。Windows の検証が要ります ([Q-013](#q-013-長いパスの一時ファイルの作成方式))。 |
| BL-028 | 中 | 未実施の組み合わせの内訳が不明なため、最初に棚卸しをします。Windows の検証が要ります。 |
| BL-003 | 中 | 利用側が `compress-cli` の 1 か所だけです。新しい公開 API が要ります ([Q-014](#q-014-ntfs-の同一性判定の扱い))。 |
| BL-024 | 小 | Windows 実機の確認だけです。対話や管理者権限は、本文の手順からは要らない見込みです (新しいコンソールを作る確認用の実行ファイルで画面バッファーを読む)。 |
| BL-020 | 小から中 | `longPathAware` のマニフェストを持つ確認用の実行ファイルと、OS の長いパスの設定 (`LongPathsEnabled`) が要ります。管理者権限が要る可能性があります ([Q-015](#q-015-windows-実機確認での環境変更の許可))。 |
| BL-009 | 小から中 | BL-020 と同じ前提です。長いパスへ置いた `tcpServer` の `fork` と `prefork` を起動し、通信します ([Q-015](#q-015-windows-実機確認での環境変更の許可))。 |

- **Windows 側の検証の連携**: Windows の MSVC の検証が要る BL-001、BL-003、BL-004、BL-005、BL-007、BL-028 と、実機確認の BL-020、BL-024、BL-009 は、Windows 側の Claude Code に依頼します ([Q-002](#q-002-windows-側での検証の受け渡し方式))。

**Answer**

選択方式: 複数選択

- [x] A: (推奨) BL-006 (`cplat_tracer_dispose` と `cplat_argparser_init` の失敗の通知)
- [x] B: (推奨) BL-008 (cplat の終了後に残るトレーサーのハンドル)
- [x] C: (推奨) BL-012 (JSONC の共通ロックを通らない cJSON の直接解析と `global_error` の競合)
- [x] D: (推奨) BL-013 (待機時間の項目の上限の未検査)
- [x] E: (推奨) BL-031 (struct-meta のフィールド診断の行番号のずれ)
- [x] F: (推奨) BL-001 (通信テストの固定ポート)
- [x] G: BL-003 (NTFS の名前比較)
- [x] H: (推奨) BL-004 (長いパスの一時ファイル)
- [x] I: (推奨) BL-005 (ETW 停止の両方失敗)
- [x] J: (推奨) BL-007 (BL-005 のクローズ整理。I と同時に選ぶ)
- [x] K: (推奨) BL-028 (Windows ソケット待機のイベント競合テスト)
- [x] L: (推奨) BL-024 (`WriteConsoleU` の途中の NUL の実機確認)
- [x] M: (推奨) BL-020 (`longPathAware` の実機確認)。前提 (`LongPathsEnabled`) を満たさない場合は、実施せず報告します
- [x] N: (推奨) BL-009 (長いパスの `tcpServer` の子プロセスの実機確認)。M と同じ前提です
- [ ] O: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 複数選択の質問であり、A から N の 14 件すべての選択は形式として有効です。推奨外の G (BL-003) を含みます。
- G の選択は、[Q-014](#q-014-ntfs-の同一性判定の扱い) で A (新しい公開 API の追加) を選んだことと整合します。
- J (BL-007) は I (BL-005) と同時に選ぶ条件を満たします。
- M と N (BL-020、BL-009) の前提 `LongPathsEnabled` は、Windows 側で `0x1` を確認済みです。[Q-015](#q-015-windows-実機確認での環境変更の許可) の A (環境を変えない) のまま実施できます。
- 範囲は確定です。BL-003 の詳細は、追加の [Q-023](#q-023-同一性の判定で存在しないパスを渡した場合の結果) で確認します。

**History**

- 2026-11-25 20:52:24 JST: ユーザーが A から N を選択しました (opus サブエージェントが検証)。

**Recommendation**

推奨: A、B、C、D、E、F、H、I、J、K、L、M、N (BL-003 を除く 13 件)  
優先した基準: 消化できる件数の最大化を重視し、次に変更の波及の小ささ、設計判断の少なさを重視しました。  
理由: BL-003 だけは、製品内の利用側が `compress-cli` の 1 か所で、新しい公開 API が要るため、利用側が増えるまで見送るのが妥当です。残る 13 件は、いずれも既存の関数の修正、テストの追加、実機の確認のどれかです。13 件は、app と検証の場所で単位に分けて順に進められます (4.1 は WSL、4.2 と 4.3 は Windows 側と連携、4.4 は実機確認)。1 案件として大きい場合は、実機確認の M、N と、H (BL-004) を次の案件へ回せます。その場合でも、他の件への影響はありません。

**Related**

[Q-002](#q-002-windows-側での検証の受け渡し方式)、[Q-015](#q-015-windows-実機確認での環境変更の許可)

### Q-002 Windows 側での検証の受け渡し方式

**Status**

解決済み

**Question**

Windows 側での検証の失敗を、誰がどう直すか (修正の往復の方式) を、どれにしますか。

**Context**

- **経緯と背景**: ユーザーの指示で、Windows の検証は次の方式で行います。WSL で作ったパッチを、Windows 側の別作業ツリー `/mnt/d/Users/tetsuo/Local/repos/c-modernization-kit` (HEAD `9d37dce0`、2026-11-25 20:35 ごろ変更なしを確認) へ渡し、Windows 側 Claude Code が `git apply` (`--index` なし) して MSVC でビルドとテストを行います。主エージェントが仲介します。この方式自体はユーザーの指示です。ここで決めるのは、MSVC のビルド エラーや Windows のテストの失敗が出たときの扱いです。
- **標準の手順 (案の共通部分)**:
    - 両側の HEAD が同じであることを確認します (WSL 側で commit が進んだ場合は、パッチの基点を更新します)。
    - WSL 側で `git diff -- <対象パス>` (tracked の変更) と `git diff --no-index /dev/null <新規ファイル>` (新規ファイル) を連結してパッチを作ります。`git add -N` は使いません。
    - Windows 側で `git apply --check` の後に `git apply` を行います。インデックスは変更しません。
    - MSVC でビルドし、対象のテストを実行し、`.warn` ファイルの内容を確認します。
    - 結果 (成否、失敗のログ、`.warn` の内容) を、主エージェント経由で受け取ります。
    - 検証の後、Windows 側の作業ツリーを元に戻します (`git apply -R`。ステージングやスタッシュではありません)。
- **各案の具体例**:

```text
案 A: Windows 側は検証と報告だけを行う。
  WSL 側のパッチを適用 -> ビルド -> テスト -> 失敗ログを返す。
  修正は WSL 側で行い、新しいパッチを渡し直す。
  作業ツリーの正本が WSL 側だけに保たれる。MSVC 固有のエラーは往復が増える。
```

```text
案 B: Windows 側が MSVC 固有の最小の修正を行い、その差分を返す。
  適用 -> ビルドエラーや失敗 -> Windows 側が最小の修正 -> 再ビルド -> 通ったら、
  修正の差分を git diff で返す。WSL 側が差分を取り込み、WSL 側のテストを再実行する。
  往復は減る。Windows 側の修正は、必ず差分として返し、WSL 側が内容を確認してから取り込む。
```

```text
案 C: 実装の一部 (Windows 専用のソース) を Windows 側が最初から書く。
  WSL では Windows 専用のソースをビルドできないため、書いてから渡すより早い場合がある。
  設計は WSL 側が決め、Windows 側が実装し、差分を返す。
```

**Answer**

選択方式: 単一選択

- [ ] A: Windows 側は検証と報告だけを行い、修正は WSL 側で行う
- [x] B: (推奨) Windows 側が MSVC 固有の最小の修正を行って差分を返し、WSL 側が確認して取り込む
- [ ] C: Windows 専用のソースは、設計を渡して Windows 側が実装し、差分を返す
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: 作業ブランチでコミットしながら2つのOSで作業を進めてもよい。最終的にはmainにマージすること。

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 B (Windows 側が MSVC 固有の最小の修正を行い、差分を WSL 側が確認して取り込む) は、単一選択として明確です。
- 補足は、**Context** の前提 (コミットせず、`git diff` のパッチを `git apply` で渡し、インデックスを変更しない) を、作業ブランチへのコミットによる受け渡しへ広げる指示です。前提と補足が食い違うため、未解決とします。
- `AGENTS.md` は、Git の書き込み操作の許可を、指示に限る単発の許可として扱います。この補足を許可として使うには、範囲を最終仕様に明記する必要があります。補足から次の事項は読み取れると判断し、追加の Q の **Context** に明記しました。
    - この案件の作業で、作業ブランチの作成と、そのためのステージングとコミットを行ってよいこと。
    - WSL 側と Windows 側の両方のエージェントがコミットしてよいこと (「2 つの OS で作業を進めてもよい」)。Windows 側のコミットは、選択 B のとおり、WSL 側が差分を確認してから取り込みます。
    - 作業ブランチは、変更するすべてのリポジトリに作ること。この案件は、親リポジトリに加え、サブモジュール `app/cplat`、`app/porter`、`app/cjson`、`framework/testfw` を変更します (`.gitmodules` で確認)。
- 補足から読み取れない事項は、1 問 1 判断で追加しました。受け渡しの経路は [Q-016](#q-016-作業ブランチのコミットを受け渡す経路)、main へ統合する時点は [Q-017](#q-017-main-へ統合する時点)、統合の方法は [Q-018](#q-018-main-へ統合する方法)、main の push の扱いは [Q-019](#q-019-main-の-push-の扱い) です。
- 4 件の回答で範囲が確定した時点で、この Q を解決済みにします。
- 2026-11-25 21:33:48 JST の回答 (Q-016 A、Q-017 C、Q-018 A、Q-019 A) で範囲が確定しました。作業ブランチを origin で共有し、両側でコミットし、Windows 側のコミットは WSL 側が差分を確認してから取り込みます。BL の単位で検証が済むごとに、マージ コミットでサブモジュール、親リポジトリの順に main へ統合し、push して CI を確認します。補足と選択 B は、この形で整合します。解決済みとします。

**Recommendation**

推奨: B  
優先した基準: 往復回数の削減を重視し、次に作業ツリーの正本の一貫性 (取り込む前に必ず内容を確認すること) を重視しました。  
理由: MSVC 固有のエラー (型の幅、`_WIN32` の分岐、ヘッダーの順序など) は、WSL では再現できず、案 A では 1 回の失敗ごとに往復が要ります。案 B は、Windows 側の修正を差分として返し、WSL 側が内容を確認してから取り込むため、正本は保てます。案 C は、WSL 側で設計の意図を共有した後に Windows 側が書く分、設計と実装の食い違いが起きやすく、推奨しません。

**Related**

[Q-001](#q-001-対策する残課題の範囲)、[Q-016](#q-016-作業ブランチのコミットを受け渡す経路)、[Q-017](#q-017-main-へ統合する時点)、[Q-018](#q-018-main-へ統合する方法)、[Q-019](#q-019-main-の-push-の扱い)

### Q-003 cplat_tracer_dispose の失敗の通知方式

**Status**

解決済み

**Question**

`cplat_tracer_dispose()` の失敗 (停止の失敗、内部の登録簿からの削除の失敗) を、戻り値で通知する形に変えますか、`void` のままにしますか。

**Context**

- **経緯と背景**: `cplat_tracer_dispose(cplat_tracer **handle)` は `void` で、失敗すると状態を元に戻し、`*handle` を `NULL` にせず戻ります (`app/cplat/prod/libsrc/cplat/trace/tracer.c` の 2350 行付近)。呼び出し側は `*handle` が `NULL` かで解放の完了を判定できます。公開ヘッダー `tracer.h` と `docs/api-cheatsheet.md` にその旨を記載済みです。
- **判断の基準**: 共通規範 `app/general/docs/coding-guideline.md` の「失敗の扱いの分類」は、関数の型を「呼び出し元が失敗を受けて取れる行動 (中止、再試行、別の経路) があるか」で決めます。cplat 固有の規範 `app/cplat/docs/coding-guideline.md` も、dispose 系を含む戻り値を持たない関数を `void` と定めています。cplat の dispose 系 (`cplat_regex_dispose`、`cplat_hashtable_dispose` など) は、すべて `void` です。
- **呼び出し元の実態**: 製品の呼び出し元は約 15 箇所 (`trace-cli`、`bench-tracer`、`string-catalog-sample`、`service-sample`)、テストは 100 箇所を超えます。いずれも終了処理で、失敗を受けて取れる行動は見当たりません。ただし、失敗後は状態が `ACTIVE` へ戻るため、再試行はできます。
- **各案の具体例**:

```c
/* 案 A: 戻り値を持たせる (公開 API の変更。ABI は x86_64 で呼び出し側が戻り値を無視すれば互換) */
CPLAT_EXPORT int CPLAT_API cplat_tracer_dispose(cplat_tracer **handle);
/* 呼び出し元 15 箇所は、検査するか、(void) と理由コメントを付ける。mock、exportTest、cheatsheet も移行。 */
```

```c
/* 案 B (推奨): void のまま。失敗は *handle が NULL でないことで判定する契約を、規範に明文化する */
cplat_tracer_dispose(&tracer);
if (tracer != NULL) { /* 解放できなかった */ }
```

**Answer**

選択方式: 単一選択

- [ ] A: 戻り値 (`int`) を持たせ、全呼び出し元を同じ変更で移行する
- [x] B: (推奨) `void` のまま維持し、`*handle` で判定する契約を cplat の規範へ明記して、BL を容認としてクローズする
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: 戻り値を int にしても、得られるものがない。再度 dispose を試みて成功する可能性も低く、リークを許容するのがよいのではないか。

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 B (`void` を維持し、`*handle` で判定する契約を cplat の規範へ明記して、容認でクローズ) は単一選択として明確です。
- 補足の照合: 「戻り値を `int` にしても得られるものがない」は、B の推奨理由 (終了処理では失敗を受けて取れる行動がない) と一致します。「再度 dispose を試みて成功する可能性も低く、リークを許容する」は、B の契約と矛盾しません。B の契約は、失敗を `*handle` が `NULL` でないことで判定できることを定めるもので、呼び出し側に再試行を求めるものではないためです。
- ソースの事実: 停止または登録簿からの削除に失敗すると、状態を `TRACE_HANDLE_ACTIVE` へ戻し、`*handle` を `NULL` にせず戻ります (`app/cplat/prod/libsrc/cplat/trace/tracer.c` の 2365 から 2376 行)。停止に失敗したハンドルは登録簿に残るため、cplat の終了処理 (`cplat_internal_trace_registry_dispose_all_on_shutdown()`、同 2384 行) が回収します。登録簿のロックの失敗や未登録で削除できなかった場合は、回収されずに残ります。
- 規範へ明記する内容は、補足に合わせて次のとおりとします。失敗は `*handle` で判定できること、呼び出し側に再試行を求めないこと、失敗時に残った資源はリークとして許容すること (登録簿に残る場合は終了処理が回収すること)。**Context** の「再試行はできます」は事実として残りますが、規範では再試行を求めません。
- 認識のずれはないため、解決済みとします。

**Recommendation**

推奨: B  
優先した基準: 共通規範との整合を重視し、次に変更量の抑制を重視しました。  
理由: 呼び出し元が失敗を受けて取れる行動は、終了処理では見当たりません。規範の基準では `void` が妥当です。判定の手段 (`*handle` の NULL) もすでにあります。案 A は、約 15 箇所の製品コードと 100 箇所を超えるテストの移行が要り、得られる行動がありません。ただし、規範が「通知なし (void)」と書く dispose に二重ポインターの判定という例外があるため、その位置付けを cplat の規範へ 1 節として足します。

**Related**

[Q-004](#q-004-cplat_argparser_init-の失敗の通知方式)

### Q-004 cplat_argparser_init の失敗の通知方式

**Status**

解決済み

**Question**

`cplat_argparser_init()` の失敗 (既定のパーサーの確保や `description` の複製の失敗) を、戻り値で通知する形に変えますか。

**Context**

- **経緯と背景**: `cplat_argparser_init(int argc, char *const *argv, const char *description)` は `void` です。確保や複製に失敗すると、失敗を通知せず、既定のパーサーは初期化されません。以後にパーサーを操作すると、引数なしのパーサーが新しく作られます (`app/cplat/prod/include/cplat/argparser/argparser.h` の 199 から 201 行、`app/cplat/prod/libsrc/cplat/argparser/argparser.c` の 1356 から 1415 行)。利用側のコマンドは、引数がないものとして動くため、失敗に気付けません。
- **判断の基準**: 共通規範の「失敗の扱いの分類」では、関数の型を、呼び出し元が失敗を受けて取れる行動で決めます。コマンドの `main` は、初期化の失敗を終了コードで失敗にでき、取れる行動があります。Q-003 の dispose (終了処理) とは、この点が違います。
- **呼び出し元の実態**: `cplat_argparser_init` の呼び出しは、テスト以外で 35 箇所です (`app/cplat/prod/src/cmd/` の各コマンド、`app/porter`、`app/service-sample`、`app/string-catalog-sample` ほか)。規範のとおり、案 A では 35 箇所のすべてに、検査 (終了コードで伝達) か `(void)` と理由コメントが要ります。
- **各案の具体例**:

```c
/* 案 A (推奨): int 戻り値。呼び出し元は main の失敗として伝達する */
if (cplat_argparser_init(argc, argv, "sample program") != CPLAT_OK)
{
    return EXIT_FAILURE;
}
```

```c
/* 案 B: void のまま。失敗を、次のオプション登録や解析の失敗として表面化する (記録だけを残す) */
cplat_argparser_init(argc, argv, "sample program");
/* 登録や cplat_argparser_parse() が、初期化の失敗を記録した場合は CPLAT_ERR_OUT_OF_MEMORY を返す。 */
```

```c
/* 案 C: 現状維持。ヘッダーの記載だけで容認し、BL をクローズする */
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `int` 戻り値にして、35 箇所すべてを同じ変更で移行する
- [ ] B: `void` のまま、失敗を記録し、後続の登録と解析の関数が失敗を返す
- [ ] C: 現状維持 (ヘッダーの記載だけ) として、容認でクローズする
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (`int` 戻り値にして、全呼び出しを同じ変更で移行) は単一選択として明確です。
- 呼び出し元の数の訂正: **Context** の「テスト以外で 35 箇所」は、再調査の結果、製品とサンプルの `.c` が 23 箇所でした。内訳は `app/cplat/prod/src/cmd/` の 11 箇所、`app/calc` の 3 箇所、`app/porter` の 3 箇所、`app/tutorial` の 2 箇所、`app/override-sample`、`app/service-sample`、`app/struct-meta`、`app/subfolder-sample` の各 1 箇所です。このほか、テスト (`argparserTest.cc` の 8 箇所、`argparserAllocFailureTest.cc` の 2 箇所)、mock (`mock_cplat.h`、`mock_cplat.cc`、`mock_cplat_argparser.cc`)、`exportTest.cc`、文書 (`app/cplat/prod/libsrc/cplat/argparser/README.md`、`argparser-sample/README.md`、`app/cplat/docs/api-cheatsheet.md`) を同じ変更で移行します。件数の違いは移行の方針に影響しません。
- 戻り値は、`argparser_default_acquire()` が `NULL` を返す場合 (既定のパーサーの確保、名前や説明の複製、内部ロックの生成の失敗) に `CPLAT_ERR_OUT_OF_MEMORY`、それ以外は `CPLAT_OK` とする見込みです (`argparser.c` の 1356 から 1415 行)。詳細は最終仕様で定めます。
- 規範 (新設した規範への既存コードの一括移行、cplat を理想の最終形に保つ方針) と整合します。解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 失敗を呼び出し側が直接受け取れること (BL の「本来あるべき姿」) を重視し、次に規範の判断基準との整合、cplat を理想の最終形に保つ方針 (新しい形へ全呼び出しを同じ変更で移行) を重視しました。  
理由: 初期化の失敗は、コマンドが終了コードで失敗にでき、取れる行動があります。案 B は、失敗が後続の関数の失敗として現れるだけで、原因が見えにくくなります。案 C は BL を解決しません。案 A は、呼び出し側の移行が 35 箇所と多いが、いずれも `main` の冒頭の機械的な修正です。mock のヘッダーを変えるため、`app/cplat` で make してから、利用側の各 app をテストします。

**Related**

[Q-003](#q-003-cplat_tracer_dispose-の失敗の通知方式)

### Q-005 終了後に残るトレーサーのハンドルの扱い

**Status**

解決済み

**Question**

cplat の終了処理がトレーサーを解放した後に、porter が解放済みのハンドルを参照しないようにする方式を、どれにしますか。

**Context**

- **経緯と背景**: cplat は `cplat_shutdown_register()` で終了コールバックを 1 つ登録し、終了時に登録済みのトレーサーをすべて解放します (`app/cplat/prod/libsrc/cplat/trace/tracer.c` の 162 から 177 行、2384 行付近)。解放は `cplat_free(handle)` までを行います。一方、porter の `potr_trace.c` は、静的変数 `s_trace` と状態 `s_trace_state` へハンドルを保持します。終了後も `s_trace` は無効化されず、`potr_internal_trace_get()` が解放済みのハンドルを返します。公開関数 `potr_tracer_get()` の利用側も、解放済みのハンドルを保持し得ます。
- **根拠資料**: [`potr_trace.c`](file:///home/user/c-modernization-kit/app/porter/prod/libsrc/porter/infra/potr_trace.c#L53-L130)、`app/cplat/prod/include/cplat/runtime/shutdown.h` (終了コールバックは LIFO 順に 1 回だけ実行される)。
- **順序の事実**: porter が最初にトレーサーを作るとき、`cplat_tracer_create()` が cplat の終了コールバックを先に登録します。porter が自分の終了コールバックを、その後 (生成の成功後) に登録すれば、LIFO により、porter のコールバックが cplat の解放より先に実行されます。
- **各案の具体例**:

```c
/* 案 A (推奨): porter が終了コールバックを登録し、s_trace を無効化して状態を「終了済み」にする */
static void potr_trace_shutdown(const cplat_shutdown_event *event, void *context)
{
    cplat_atomic_store_ptr(&s_trace, NULL, CPLAT_MEMORY_ORDER_RELEASE);
    cplat_atomic_store_i32(&s_trace_state, 3, CPLAT_MEMORY_ORDER_RELEASE); /* 3: 終了済み。以後の再生成もしない */
}
/* potr_internal_trace_get() は状態 3 で NULL を返す。POTR_TRACE と cplat_tracer_writef_at は、NULL を無視する。 */
/* 終了処理と並行して porter を使うスレッドが残らないことを、porter_spec.h の契約として明記する。 */
```

```c
/* 案 B: cplat 側で、終了時にトレーサーの構造体を解放せず「解放済み」の状態で残す */
/* 外部のすべての保持者が安全になるが、終了時のメモリを意図的に残す。cplat の契約と sanitizer の扱いが変わる。 */
```

```c
/* 案 C: porter が、毎回、cplat の終了状態を照会する新しい API を呼ぶ */
/* cplat へ公開 API を足す。照会と使用の間の競合が残る。 */
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) porter が終了コールバックを登録し、`s_trace` の無効化と再生成の禁止を行う。並行利用の禁止を契約に明記する
- [ ] B: cplat 側で、終了時もトレーサーの構造体を解放せず無効状態で残す
- [ ] C: cplat へ終了状態の照会 API を追加し、porter が毎回確認する
- [ ] D: 今回は見送る (次の案件へ転記する)
- [ ] E: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (porter が終了コールバックを登録し、`s_trace` の無効化と再生成の禁止を行い、並行利用の禁止を契約に明記) は単一選択として明確です。
- 前提の確認: 終了コールバックは LIFO 順に 1 回だけ実行されます (`app/cplat/prod/include/cplat/runtime/shutdown.h` の 9 行)。cplat のトレースの終了コールバックは、最初のトレーサーの生成時に 1 回だけ登録されます (`tracer.c` の 158 から 171 行)。porter が生成の成功後に登録すれば、porter のコールバックが先に実行されます。`cplat_tracer_writef_at()` は `NULL` のハンドルを無視して `CPLAT_OK` を返します (`tracer.c` の 1606 行)。`mock_cplat` に `cplat_shutdown_register` の mock があり (`mock_cplat.h` の 2267 行)、単体テストでコールバックを捕まえて実行できます。
- BL-008 の「進行中の参照も回収します」について、A は参照カウントを持たず、終了処理と並行する porter の利用を契約で禁じる方式です。選択肢 A の文言がこの方式を明示しているため、進行中の参照は契約で扱い、参照カウントは追加しません。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 変更範囲を porter に閉じることを重視し、次に cplat の契約の維持、公開 API の増加の回避を重視しました。  
理由: LIFO の順序により、porter の終了コールバックは、cplat の解放より先に必ず実行されます。案 B は全員を守れる一方で、cplat の終了処理の契約を変え、メモリの扱いが複雑になります。案 C は、公開 API が増える上に、確認と使用の間の競合が残ります。案 A でも、終了コールバックの実行中に別スレッドが porter のトレースを使う場合の競合は残るため、その場合は契約 (終了処理の開始までに porter の利用を止める) として `porter_spec.h` へ明記します。BL の「進行中の参照も回収する」を完全に満たすには、参照カウントの追加が要り、設計の段階で扱いを決めます。

**Related**

[Q-001](#q-001-対策する残課題の範囲)

### Q-006 cJSON の直接解析と global_error の競合の対策方式

**Status**

解決済み

**Question**

cJSON の `global_error` の競合を、どの方式で解消しますか。

**Context**

- **経緯と背景**: cJSON 本体の `global_error` (`app/cjson/prod/libsrc/cjson/cJSON.c` の 95 行) は、プロセス共有の静的変数です。解析の失敗時に書かれ、`cJSON_GetErrorPtr()` が読みます。JSONC のラッパー (`cjson_app_cJSON_ParseJSONCWithLengthOpts`) は、プロセス共有のロックで解析を直列化しますが、`struct_meta_json_file_load()` (`app/struct-meta/prod/libsrc/struct_meta/json/file.c` の 155 行の `cjson_app_cJSON_Parse()`) と、利用側が直接呼ぶ `cJSON_Parse` 系は、そのロックを通りません。現在は、利用側が排他する契約です (`app/cjson/README.md`、`cJSON_JSONC.h`)。
- **制約**: cJSON 本体は生成物で、直接編集できません。変更は `app/cjson/patches/` のパッチ (`0001`、`0002` に続く `0003`) だけで行えます。cjson は cplat に依存しない層なので、パッチは cplat の `THREAD_LOCAL` (`compiler.h`) を使えません。
- **注意点**: ロックは、`global_error` のためだけではありません。JSONC 側の確保失敗の計数 (`jsonc_allocation_failure_count`) も、プロセス共有の変数で、並行する別の解析の確保の失敗を、誤って JSONC の失敗と判定し得ます (`cJSON_JSONC.c` の 415 行以降)。方式によって、この計数も同時に見直す必要があります。
- **各案の具体例**:

```c
/* 案 A (推奨): パッチ 0003 で global_error をスレッド ローカルにする */
-static error global_error = { NULL, 0 };
+static CJSON_THREAD_LOCAL error global_error = { NULL, 0 };  /* C11 _Thread_local / MSVC __declspec(thread) */
/* 全解析経路の書き込み競合が消える。cJSON_GetErrorPtr() は「同じスレッドの直近の失敗」を返す。 */
/* JSONC のロックは、確保失敗の計数の保護に残るが、計数もスレッド ローカル化できれば不要になる。 */
```

```c
/* 案 B: パッチ 0003 で cJSON_ParseWithLengthOpts の中に共通ロックを取る */
/* 直接の解析も直列化される。cJSON_GetErrorPtr() の読み取りの競合は残り、全解析が 1 本に直列化する。 */
```

```c
/* 案 C: 現状維持。利用側の排他の契約のまま、容認としてクローズする */
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) パッチで `global_error` をスレッド ローカルにし、契約と文書を更新する
- [ ] B: パッチで本体の解析にも共通ロックを取り、直接の解析も直列化する
- [ ] C: 現状維持として、容認でクローズする
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (パッチで `global_error` をスレッド ローカルにし、契約と文書を更新) は単一選択として明確です。
- 事実の訂正: **Context** の「確保失敗の計数 (`jsonc_allocation_failure_count`) もプロセス共有の変数」は誤りでした。計数は、すでにスレッド ローカルです (`app/cjson/prod/libsrc/cjson/jsonc/cJSON_JSONC.c` の 62 行)。JSONC の共通ロックは、cJSON 本体の解析の呼び出しだけを直列化しています (`app/cjson/README.md` の 50 行)。このため、A の後はロックを残す理由がなくなります。ロックの扱いは、新しい判断として [Q-020](#q-020-jsonc-の共通ロックの扱い) で確認します。
- 実現性: 同じライブラリの `cJSON_JSONC.c` が、すでに `__declspec(thread)` (MSVC) と `__thread` (GCC) を使っています (同 21 から 27 行)。Windows の DLL と動的読み込みのテスト (`cjsonJsoncDynamicLoadTest`) もあります。cJSON 本体の書き換えできる静的変数は、`global_error` と、ロード後に変えない `global_hooks` だけです (`cJSON.c` の 95 行と 189 行)。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 競合の根絶 (BL の「本来あるべき姿」の「`global_error` 自体を持たない」に近い形) を重視し、次に利用側の契約の簡素化、解析性能 (全解析を直列化しないこと) を重視しました。  
理由: 案 A は、書き込みも読み取りも、スレッドごとに独立します。`cJSON_GetErrorPtr()` の意味は、プロセス共有から呼び出しスレッドの直近の失敗へ変わりますが、porter はすでに使っておらず (`config_document.c` の 306 行)、利用側が増えても同じスレッドで呼ぶのが自然です。案 B は、直接の解析を含む全解析が直列になり、読み取りの競合も残ります。Windows の DLL で `__declspec(thread)` を使えることは、設計の段階で MSVC の版と共に確認します。

**Related**

[Q-001](#q-001-対策する残課題の範囲)、[Q-020](#q-020-jsonc-の共通ロックの扱い)

### Q-007 待機時間の項目の上限の検査方式

**Status**

解決済み

**Question**

`pack_wait_ms`、`connect_timeout_ms`、`reorder_timeout_ms`、`udp_health_timeout_ms`、`health_timeout_ms` の上限を、どの方式で扱いますか。

**Context**

- **経緯と背景**: これら 5 項目の値域は 0 から 4,294,967,294 (`app/porter/prod/libsrc/porter/protocol/config_schema.c` の 64 から 145 行) です。待機の関数へ渡す上限 `POTR_WAIT_MS_MAX` (2,147,483,646。`app/porter/prod/include_internal/porter/potr_context.h` の 55 行。`INT_MAX` は `CPLAT_WAIT_FOREVER` と同じ値で無期限になるため含まれません) との関係を確かめていません。
- **確認できた事実**: `thread_connect_prepare.c` の 262 から 266 行は、`connect_timeout_ms` を `(int)timeout_ms` に変換して `cplat_socket_watch_connect` へ渡します。2,147,483,648 以上では負の値になり、2,147,483,647 では無期限の待機になります。`potr_send_thread.c` の 1343 から 1382 行は、`pack_wait_ms` が `INT_MAX` 以上のとき、`remaining` を `int` へ変換する前提 (コメントは「`pack_wait_ms` 以下で `INT_MAX` に収まる」) が崩れます。`reorder_timeout_ms` (`thread_recv_window.c` の 194 行) と `health_timeout_ms` は、主に時刻との比較で、変換の経路は確認していません (設計の段階で、残る 3 項目の経路を確認します)。同じ設定の別項目 (`tcp_health_interval_ms` など) は、すでに `POTR_WAIT_MS_MAX` を値域の上限にしています。`tcp_health_timeout_ms` は 715,827,882 (別の計算のあふれ防止) です。
- **各案の具体例**:

```c
/* 案 A (推奨): 5 項目の値域の上限を POTR_WAIT_MS_MAX へ狭める (既存の項目と同じ書き方) */
{POTR_CONFIG_SFIELD_CONNECT_TIMEOUT_MS, 0U, "connect_timeout_ms", POTR_CONFIG_VAL_U32, 0U, POTR_WAIT_MS_MAX, ...,
 "0..2147483646"},
/* 2,147,483,647 以上の設定は、設定の検証でエラーになる。 */
```

```c
/* 案 B: 値域は維持し、使う側で POTR_WAIT_MS_MAX へ飽和させる */
connect_timeout_ms = (timeout_ms > POTR_WAIT_MS_MAX) ? (int)POTR_WAIT_MS_MAX : (int)timeout_ms;
/* 大きな値は上限として黙って丸められる。経路ごとに修正が要る。 */
```

```c
/* 案 C: あふれる経路がある connect_timeout_ms と pack_wait_ms だけ値域を狭め、残りは維持する */
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 5 項目すべての値域の上限を `POTR_WAIT_MS_MAX` へ狭め、境界値のテストを加える
- [ ] B: 値域は維持し、使う側で `POTR_WAIT_MS_MAX` へ飽和させる
- [ ] C: あふれる経路が確認できた項目だけ値域を狭める
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (5 項目すべての値域の上限を `POTR_WAIT_MS_MAX` へ狭め、境界値のテストを加える) は単一選択として明確です。
- 残る 3 項目の経路の確認: `reorder_timeout_ms` は時刻との比較だけです (`thread_recv_window.c` の 189 から 194 行)。`health_timeout_ms` (`udp_health_timeout_ms` が既定値) は、64 ビットの比較と、3 で割った待機の上限 (最大 1,431,655,764) に使われ、あふれません (`potr_recv_thread.c` の 307 から 309 行、`thread_recv_health.c`)。値域を狭めると、5 項目のすべての経路が安全になります。
- 事実の訂正: **Recommendation** の「`4294967294` の記載は `config_schema.c` と `config_schema.h` だけ」は誤りでした。`app/porter/docs/config.md` の 79、83、446、447 行に値域の記載があり、同じ変更で更新します。
- 同じ値域の `group_failback_ms` は、時刻の差との比較だけに使われ (`path_group.c` の 137 から 148 行)、待機の関数へ渡りません。BL-013 の対象外のままとします。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 設定の検証の規則の一貫性 (待機の項目は `POTR_WAIT_MS_MAX` 以下) を重視し、次に実装の単純さ、誤った設定の早期の検出を重視しました。  
理由: 約 24 日を超える待機を、実運用で設定する場面は限られ (BL の優先度の根拠)、設定が拒否されても影響は小さい一方、検証で止めるほうが、経路ごとの丸めより単純で漏れません。案 B は、値が黙って丸められ、経路の追加で漏れる余地が残ります。案 C は、確認した経路だけの対症になり、規則が不揃いになります。設定の値域を狭める変更なので、`porter` の機能仕様やドキュメントに値域の記載があれば、同じ変更で更新します (`4294967294` の記載は `config_schema.c` と `config_schema.h` だけであることを確認済み)。

**Related**

[Q-001](#q-001-対策する残課題の範囲)

### Q-008 struct-meta のフィールド診断の行番号の修正方式

**Status**

解決済み

**Question**

構文解析器が、規則の還元時に次の字句を先読みしているために、診断の行がずれる問題を、どの方式で直しますか。

**Context**

- **経緯と背景**: フィールドの規則は、還元時に `yyget_lineno(scanner)` を読みます (`app/struct-meta/prod/libsrc/struct_meta/parse/struct_meta_parse.y` の 138、146、163、172、186 行ほか)。この関数は、字句解析器が今いる行を返すため、`;` の次の字句を先読みした行になり、字面の行より 1 行後ろになります。この行は診断だけでなく、構造体とフィールドの `line` として AST に保存され (`ast.c`)、`catalog/build.c` の 5 箇所の診断 (メンバー名の重複、未知の型、循環など) も使います。
- **既存のテストの前提**: 次の 2 件のテストが、ずれた行 (5) を期待値として書いています。修正に合わせて、4 へ直します。
    - `app/struct-meta/test/src/structMetaBuildTest/structMetaBuildTest.cc` の 388 から 410 行 (コメントに「先読みが次の改行を消費するため」とあります)。
    - `app/struct-meta/test/src/structMetaGenEmitTest/structMetaGenEmitTest.cc` の 301 から 307 行。
- **制約**: bison と flex は `parse/gen/` へビルド時に生成します (生成物は git の管理外)。生成元は `.y` と `.l` です。`yylineno` オプションを使っています (`struct_meta_parse.l` の 28 行)。
- **各案の具体例**:

```text
案 A (推奨): bison の位置情報の機構を使う。
  .y: %locations を追加し、規則内で @3.first_line のように字句の位置を使う。
  .l: %option bison-locations と、YY_USER_ACTION で字句の位置 (yylloc) を設定する。
  yylex のシグネチャが yylex(YYSTYPE *, YYLTYPE *, yyscan_t) に変わる。
  標準の機構で、先読みの影響を受けない。
```

```text
案 B: 字句の値に行番号を持たせる。
  IDENT のセマンティック値を { char *str; int line; } にし、字句解析器が行を設定する。
  規則では $3.line を使う。%union の変更と、IDENT を使う全規則の修正が要る。
```

```text
案 C: 現状維持。フィールド診断の行は「字面の次の行」という仕様にして、容認でクローズする。
  AST の line も同様にずれたまま。build.c の診断の行も、字面と 1 行ずれる。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) bison の `%locations` と flex の位置情報の機構で、字句の位置を取る
- [ ] B: 字句の値に行番号を持たせる
- [ ] C: 現状維持として、容認でクローズする
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (bison の `%locations` と flex の位置情報の機構) は単一選択として明確です。
- 実現性: WSL の bison 3.0.4 と、Windows の CI が使う WinFlexBison 2.5.25 (`.github/workflows/ci.yml` の 24 行) の両方で使えます。flex は、`YY_USER_ACTION` の時点で、字句の中の改行をすでに `yylineno` へ数えています。このため、字句の開始行は `yylineno` から字句の中の改行の数を引いた値です (一時ディレクトリでの flex 2.6.1 の実験で確認)。`yyerror` と `yylex` の引数に位置が加わります。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 標準機構による根本の解決を重視し、次に変更の局所性 (`parse/` 配下だけ) を重視しました。  
理由: `%locations` は、bison が先読みと無関係に、各字句と規則の位置を保持する機構です。案 B は、同じ効果を独自に作ることになり、`%union` と全規則の修正が要ります。案 C は、診断の使い勝手が改善されません。生成物は `parse/gen/` で管理外のため、リポジトリの変更は `.y`、`.l`、テストに限られます。

**Related**

[Q-009](#q-009-フィールド診断で報告する行の定義)、[Q-021](#q-021-フィールドと構造体以外の診断で報告する行)

### Q-009 フィールド診断で報告する行の定義

**Status**

解決済み

**Question**

フィールドの診断で報告する行を、フィールドのどの字句の行にしますか。

**Context**

- **経緯と背景**: BL-031 の恒久対応案は「規則の最初の字句から取る」です。しかし、フィールドの規則は `doc_prefix type_spec IDENT SEMI doc_postfix` で、最初の要素 `doc_prefix` は、フィールドの上にあるドキュメント コメント (`/** ... */` が複数行にわたる場合がある) です。最初の字句の行をそのまま使うと、コメントの先頭の行になり、フィールドの宣言とは別の行を指します。
- **例** (行番号を左に付けます):

```text
3  /**
4   * @brief 値
5   */
6  int value;     <- フィールド名 value は 6 行目。
7  int value;     <- 2 つ目の value は 7 行目。(診断で指すべき行)
```

- **各案の具体例**: 上の 2 つ目の `value` (重複) の診断は、次の行を報告します。
    - 案 A (推奨): フィールド名 (IDENT) の行。7 行目。
    - 案 B: 規則の最初の字句の行。ドキュメント コメントがある場合はコメントの先頭の行 (この例では 3 または 7)。
    - 案 C: 型指定 (`type_spec`) の最初の字句の行。複数行にわたる宣言では、型の行。この例では 7 行目。
- 構造体 (`typedef struct`) の行も同じ規則で決めます。構造体は、案 A に相当するものとして、構造体名 (`IDENT`) の行を使います。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) フィールド名 (IDENT) の行
- [ ] B: 規則の最初の字句の行 (ドキュメント コメントがあればその先頭)
- [ ] C: 型指定の最初の字句の行
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (フィールド名 (IDENT) の行) は単一選択として明確です。
- **Context** の「構造体は構造体名 (`IDENT`) の行」は、AST に記録する名前、すなわち `typedef` の名前 (規則の閉じ波括弧の後の `IDENT`) の行と解釈します。ポインター メンバーの診断もフィールドの診断であり、フィールド名の行とします。
- フィールドと構造体以外の診断 (ドキュメント コメントの誤り、`long` の拒否、構文エラー) の行は、この Q の対象外です。新しい判断として [Q-021](#q-021-フィールドと構造体以外の診断で報告する行) で確認します。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 診断が指す行の一意性 (診断の対象の名前がある行) を重視し、次に診断メッセージとの一致 (メッセージはフィールド名を含みます) を重視しました。  
理由: 診断の文面は「メンバー名が重複しています: value」のように、フィールド名を含むため、名前の行を指すのが利用者にとって自然です。`;` までの宣言を 1 行に書く通常の書き方では、A、B (コメントなし)、C は同じ行になります。複数行にわたる宣言とコメントのある場合だけ差が出ます。

**Related**

[Q-008](#q-008-struct-meta-のフィールド診断の行番号の修正方式)、[Q-021](#q-021-フィールドと構造体以外の診断で報告する行)

### Q-010 テスト用ポートの払い出しの共有方式

**Status**

解決済み

**Question**

テスト用のポートの払い出しを、プロセスの間で共有しますか、プロセス内だけにしますか。

**Context**

- **経緯と背景**: `app/porter` の結合テストは、固定のポートを使い (例: `certificateTest.cc` の 176 行 `38800 + (int)type * 10 + mode * 2`)、2026-11-25 の CI で衝突の疑いのある失敗がありました。ユーザーの方向付け (2026-11-25) で、空いているポートを払い出す共通のヘルパーを `framework/testfw` に用意します。
- **既存の類似の仕組み**: `app/porter/test/src/integration/porterSendRecvTest/nonblockingStopIntegrationTest.cc` の 52 行の `pick_free_port()` が、ポート 0 で bind し `getsockname` で番号を得て閉じます。そのファイル内の局所の関数で、直近の払い出しの記録はなく、TCP の `TIME_WAIT` や、UDP の遅れて届くパケットへの対策はありません。
- **固定ポートを使う箇所**: `certificateTest.cc` (176、335 行)、`pathGroupReportTest.cc` (81、82 行)、`tcpMultipathTest.cc` (224 行)、`tcp_allocation_failure_test.cc` (158 行)、`porterCancelFailureTest.cc` (164 行)、`porterTestOutputTest.cc` (392 行)、`tcpServerTest.cc`。このほか、`libporter` の単体テストの多数のポート (apiOpenPaths 系、potrServiceOpen 系ほか) は、mock の対象で、実際には待ち受けないものが多いと見られます (移行の要否は、設計の段階で 1 件ずつ確認します)。
- **各案の具体例**:

```text
案 A (推奨): プロセスの間でも重ならないようにする。
  払い出した番号を、共有ファイル (一時ディレクトリ配下) に直近 N 件記録し、ファイル ロック
  (Linux: flock、Windows: LockFileEx) で直列化する。N 件の間は再び払い出さない。
  並列に動くテストの実行体の間でも、TCP と UDP の再利用の衝突を防げる。
```

```text
案 B: プロセス内だけ。
  払い出した番号の直近 N 件を、静的な配列に記録する。bind の確認は OS に任せる。
  実装は小さい。並列に動く別のプロセス (別のテストのディレクトリ) とは重なり得る。
```

```text
案 C: 払い出さず、製品側が OS 割り当て (ポート 0) の番号を返す仕組みを使う。
  porter の API (dst_port = 0 の扱いと、割り当て番号の取得) の変更が要り、テストのヘルパーでは解決しない。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) プロセスの間で共有する (共有ファイルとファイル ロックで直近の払い出しを記録する)
- [ ] B: プロセス内だけで記録する
- [ ] C: 製品側の API を変更し、OS 割り当てのポートを使う
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (共有ファイルとファイル ロックで、プロセスの間で直近の払い出しを記録) は単一選択として明確です。
- **Recommendation** のとおり、記録の個数、ファイルの置き場所、古い記録の扱いは最終仕様で定めます。
- 移行の対象の再調査: `app/cplat` の実ソケットのテスト (`socketTest.cc`) は、ポート 0 (OS の割り当て) を使っており、移行は要りません。`app/porter` には、**Context** の一覧のほかに、`porterSendRecvTest.cc` (19010 番台を多数)、`handleConcurrencyTest.cc`、`tcpServerPreforkTest.cc`、`tcpServerPreforkObservationTest.cc` の固定のポートがあります。移行の対象は最終仕様で列挙します。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 並列実行の衝突の確実な排除 (BL の「本来あるべき姿」) を重視し、次に製品側の変更の回避、実装の複雑さの抑制を重視しました。  
理由: BL の根本原因は、複数のテストのディレクトリが並列に動くことです (プロセスの間の衝突)。案 B は、これを防げません。案 C は、製品の API の変更が要り、BL の範囲を超えます。案 A は、共有の記録の寿命 (N の値、ファイルの置き場所、古い記録の掃除) を、設計の段階で決めます。ユーザーの方向付けに沿います。

**Related**

[Q-011](#q-011-テスト用ポートの払い出し範囲)

### Q-011 テスト用ポートの払い出し範囲

**Status**

解決済み

**Question**

テスト用のポートを払い出す範囲を、OS の一時的なポートの範囲の外にしますか、中にしますか。

**Context**

- **経緯と背景**: Linux の一時的なポートの範囲は、既定で 32768 から 60999 (`/proc/sys/net/ipv4/ip_local_port_range`)、Windows の既定は 49152 から 65535 です。現在の固定ポート (38800 番台) は、Linux の一時的な範囲の中にあります。OS が接続の送信元ポートとして自動で割り当てる番号と、テストの待ち受けが重なると、bind に失敗します。
- **各案の具体例**:

```text
案 A (推奨): 範囲の外から選ぶ (Linux の範囲の下限より小さい番号。例: 20000 から 32767)。
  OS の自動割り当てと衝突しない。選んだ番号が使用中かは、bind の確認と直近の記録で防ぐ。
  Linux の一時的な範囲は変更できるため、実行時に /proc/sys/net/ipv4/ip_local_port_range を読み、
  範囲にかかる場合は避ける。
```

```text
案 B: OS に割り当てさせる (ポート 0)。一時的な範囲の中から払い出される。
  bind の確認と直近の記録だけで衝突を防ぐ。OS の自動割り当てとの競合は、払い出しから使用までの間に残る。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) OS の一時的な範囲の外から払い出す (実行時に OS の設定を読んで避ける)
- [ ] B: OS に割り当てさせ (一時的な範囲の中)、bind の確認と直近の記録で衝突を防ぐ
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (OS の一時的な範囲の外から払い出し、実行時に OS の設定を読んで避ける) は単一選択として明確です。
- **Context** が実行時に読むとしたのは Linux の `/proc/sys/net/ipv4/ip_local_port_range` です。Windows には、動的ポートの範囲を返す公開の関数がないため、既定の範囲 (49152 から 65535) の外を候補とし、予約された除外範囲は bind の確認で避けます。Windows の実際の動的ポートの範囲と除外範囲は、Windows 側で読み取りだけの確認を依頼します。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 衝突の確率の低さ (OS の自動割り当てとの競合の排除) を重視し、次に環境差 (Linux と Windows の既定の違い、コンテナーでの設定の変更) への耐性を重視しました。  
理由: 案 B は、払い出しから使用までの短い間に、OS が同じ番号を自動割り当てし得ます。案 A は、その競合を原理的に避けられます。Windows には、予約された除外範囲 (`netsh int ipv4 show excludedportrange`) があるため、範囲の外でも bind の確認は残します。

**Related**

[Q-010](#q-010-テスト用ポートの払い出しの共有方式)

### Q-012 ETW 停止で両方失敗した場合の待機の打ち切り方式

**Status**

解決済み

**Question**

`cplat_etw_session_stop()` で、停止の要求とトレースを閉じる処理の両方が失敗した場合に、スレッドの終了待ちを打ち切って戻る方式を、どれにしますか。

**Context**

- **経緯と背景**: `cplat_etw_session_stop()` (`app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c` の 715 から 759 行) は、`ControlTraceW(STOP)` が失敗すると `close_trace_handle()` で閉じて `ProcessTrace()` を戻らせます。両方が失敗すると、`cplat_thread_join()` が戻りません。公開ヘッダー `etw.h` の 191 から 207 行は、その旨を通知しています。
- **利用できる部品**: `cplat_thread_timed_join(thread, timeout_ms)` がすでにあります (`app/cplat/prod/include/cplat/sync/sync.h` の 415 行)。このため、新しい OS API とその mock は要りません。
- **BL が挙げる注意**: 打ち切った後にスレッドを切り離して構造体を解放すると、コールバックのユーザー データの解放後の使用を招きます。
- **BL-007 との関係**: BL-007 の恒久対応案は「なし (モックの単体テストで経路を通す)」です。この案件では、同じ単体テスト (`trace_etw_sessionTest`、`ControlTraceW`、`CloseTrace`、`ProcessTrace` のモック) で、打ち切りの経路も確認し、BL-007 をクローズします。
- **各案の具体例**:

```c
/* 案 A (推奨): 期限付きで待ち、期限を超えたら構造体を解放せずに戻る (意図的に残す) */
if (cplat_thread_timed_join(session->thread_handle, CPLAT_ETW_STOP_JOIN_TIMEOUT_MS) != CPLAT_OK)
{
    /* スレッドは ProcessTrace() の中。session とコールバックの文脈を、スレッドが参照し続けるため解放しない。 */
    return;
}
```

```c
/* 案 B: 異常終了 (fail-fast) にする。内部状態が壊れ、続行が危険という分類の条件に当てはまる */
CPLAT_ABORT(...);
```

```c
/* 案 C: 現状維持。ヘッダーの記載だけで容認し、BL-005 をクローズする */
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 期限付きの待機にして、期限を超えたら構造体を解放せずに戻る。待機の期限の長さは設計で決める
- [ ] B: 期限付きの待機にして、期限を超えたら異常終了する
- [ ] C: 現状維持として、容認でクローズする
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (期限付きの待機にして、期限を超えたら構造体を解放せずに戻る。期限の長さは設計で決める) は単一選択として明確です。選択肢の文言のとおり、期限の長さは最終仕様で定めます。
- 契約の確認: 公開ヘッダーは「本関数から戻った後は、コールバックは呼び出されません」と定めています (`etw.h` の 197 行)。打ち切って戻ると、処理スレッドが残るため、そのままでは利用側のコールバックとその文脈の解放後の使用が起きえます。この契約を保つため、打ち切る前に以後の利用側のコールバックの呼び出しを止め、実行中のコールバックの終了を待つ仕組みを、最終仕様で定めます。新しい OS の関数は使いません。
- BL-007 は、同じ単体テスト (`trace_etw_sessionTest`) で打ち切りの経路を通し、クローズします。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 解放後のメモリ参照の防止を最優先とし、次に停止の呼び出しが戻ること、変更の小ささを重視しました。  
理由: 案 A は、BL の「本来あるべき姿」(解放後のメモリ参照を起こさずに待機を打ち切って戻れる) を満たします。代償は、異常な環境でのメモリの意図的な残しと、ヘッダーの契約 (「戻った後はコールバックが呼ばれない」) の修正です。案 B は、取れる手段のない失敗でプロセスを落とすため、呼び出し側の終了処理の継続を妨げます。案 C は BL を解決しません。

**Related**

[Q-001](#q-001-対策する残課題の範囲)

### Q-013 長いパスの一時ファイルの作成方式

**Status**

解決済み

**Question**

`GetTempFileNameW` を使う 2 箇所の一時ファイルの作成を、どの方式にしますか。

**Context**

- **経緯と背景**: `cplat_fopen_temp()` (`app/cplat/prod/libsrc/cplat/crt/stdio_temp.c` の 179 行) と、昇格の結果ファイルの作成 (`app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` の 1308 行) は、`GetTempFileNameW` を使います。ディレクトリ部が `MAX_PATH - 14` 文字を超えると `ERROR_BUFFER_OVERFLOW` で失敗し、出力先も `MAX_PATH` 要素の配列です。
- **利用できる部品**: `cplat_random_bytes()` (`app/cplat/prod/include/cplat/crypto/random.h`) と、一時ディレクトリの取得 `cplat_internal_get_temp_dir_w()` (`temp_dir_internal.h`) がすでにあります。
- **既存の制約**: `cplat_fopen_temp()` の公開 API は、UTF-8 の `path_out` (`PLATFORM_PATH_MAX` 系) へ結果を書き、`FILE *` を返します。長いパスを開くには、拡張パスの接頭辞 (`\\?\`) か、`longPathAware` のプロセスが要ります。
- **各案の具体例**:

```c
/* 案 A (推奨): 名前を cplat が作り、CREATE_NEW で排他的に作る内部の共通関数を、2 箇所から使う */
/* 名前: 接頭辞 + cplat_random_bytes() による 16 進文字列 + ".tmp"。CreateFileW(CREATE_NEW) で作成。 */
/* 衝突時は別の名前で再試行。長いパスは、拡張パスの接頭辞を付けて扱う。 */
```

```c
/* 案 B: 現状の GetTempFileNameW のまま、公開ヘッダーの契約へ「ディレクトリ部が MAX_PATH - 14 以下」の制約を明記して容認する */
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 内部の共通関数で、名前を作って排他的に作成する方式へ置き換える
- [ ] B: 制約を契約として明記し、容認でクローズする
- [ ] C: 今回は見送る (次の案件へ転記する)
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (内部の共通関数で名前を作り、排他的に作成する方式へ置き換え) は単一選択として明確です。
- 前提の確認: `CreateFileW` の mock は `framework/testfw/libsrc/mock_libc/mock_CreateFileW.cc` にあります。乱数は `cplat_random_bytes()` を使えます。昇格の結果ファイルのパスは、`PLATFORM_PATH_UTF8_MAX` (Windows は 780 [byte]) と `PLATFORM_PATH_MAX` (260 [wchar_t]) の固定長の配列です (`elevated_process.c` の 1251、1253 行)。
- `MAX_PATH` 以上の完全なパスは、プロセスが `longPathAware` でない限り、`CreateFileW` と `_wfsopen` で開けません。cplat の既存の UTF-8 のラッパー (`CreateFileU` など) は拡張パスの接頭辞 (`\\?\`) を付けません (`app/cplat/prod/libsrc/cplat/win32/file_api.c`)。**Context** の案 A の例は接頭辞を付ける書き方でしたが、選択肢の文言は接頭辞の扱いを定めていません。新しい判断として [Q-022](#q-022-長いパスの一時ファイルでの拡張パスの接頭辞) で確認します。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 制約の根本の解消 (BL の「本来あるべき姿」) を重視し、次に cplat を理想の最終形に保つ方針 (2 箇所を同じ変更で移行) を重視しました。  
理由: 部品が揃っており、新しい OS API は `CreateFileW` だけです (testfw の mock の有無は、設計の段階で確認します)。2 箇所を 1 つの内部関数へまとめられます。案 B は、通常の環境では十分で、変更が最小です。ただし、`FILE *` として開く側 (`_wfsopen`) も長いパスを扱えることの確認が要り、Windows 側の検証に時間が要ります。範囲 ([Q-001](#q-001-対策する残課題の範囲)) で規模が大きすぎる場合は、C が現実的です。

**Related**

[Q-001](#q-001-対策する残課題の範囲)、[Q-022](#q-022-長いパスの一時ファイルでの拡張パスの接頭辞)

### Q-014 NTFS の同一性判定の扱い

**Status**

解決済み

**Question**

`cplat_path_equal()` の限界 (NTFS の大文字の表との違い、リンク、ハード リンク) に対し、ファイルの識別子で同一性を判定する関数を追加しますか。

**Context**

- **経緯と背景**: `cplat_path_equal()` は、文字列として比べるだけです (公開ヘッダー `app/cplat/prod/include/cplat/crt/path.h` の 205 から 235 行に、範囲を明記済み)。Windows ではシンボリック リンクを解決せず、両方の OS でハード リンクを判定しません。製品の呼び出し元は `compress-cli` の 1 か所 (`app/cplat/prod/src/cmd/compress-cli/compress-cli.c` の 247 行、入力と出力が同じファイルかの確認) です。
- **BL の恒久対応案**: 「同じ実体かの判定が要る利用側が現れた場合に、ファイルの識別子 (`GetFileInformationByHandle` のボリュームの番号とファイルの番号) で比べる関数を、別に設けることを検討します。」
- **追加する場合の規模**: 公開 API の追加 (例: `cplat_path_is_same_file`) に伴い、`mock_cplat`、`exportTest.cc`、`api-cheatsheet.md`、機能仕様、`compress-cli` への適用、Linux 側 (`stat` の `st_dev` と `st_ino`) の実装が要ります。出力先のファイルが未存在のときは、同一性を判定できないため、文字列の比較へ戻る必要があります (`compress-cli` の用途では、出力が既存の場合にだけ意味があります)。
- **各案の具体例**:

```c
/* 案 A: 新しい公開 API を追加する */
CPLAT_EXPORT int CPLAT_API cplat_path_is_same_file(const char *lhs, const char *rhs, int *same_out, cplat_error *detail_out);
```

```text
案 B (推奨): 見送りを継続する。利用側が増えた時点で、必要な意味 (存在しない出力先をどう扱うか) と合わせて設計する。
```

**Answer**

選択方式: 単一選択

- [x] A: 新しい公開 API を追加し、`compress-cli` へ適用する
- [ ] B: (推奨) 見送りを継続する (BL-003 を次の案件へ転記する)
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (新しい公開 API を追加し、`compress-cli` へ適用) は単一選択として明確で、[Q-001](#q-001-対策する残課題の範囲) の G と整合します。
- 判定の方式は、**Question** のとおりファイルの識別子 (Windows はボリュームとファイルの識別子、Linux は `stat` の `st_dev` と `st_ino`) です。`CompareStringOrdinal` による文字列の比較 (`cplat_path_equal()`) は変更しません。シンボリック リンクは、開く (`stat` する) 時に解決されるため、リンク先の実体で比べます。
- **Context** のとおり、存在しないパスの扱いが未決です。新しい判断として [Q-023](#q-023-同一性の判定で存在しないパスを渡した場合の結果) で確認します。
- Windows で使う OS の関数 (`GetFileInformationByHandleEx` または `GetFileInformationByHandle`) の mock は testfw にありません。新しい OS の関数として、testfw に mock を加えます。Linux の `stat` の mock はあります (`mock_stat.cc`)。
- 解決済みとします。

**Recommendation**

推奨: B  
優先した基準: 利用側の実需に基づく設計を重視し、次に公開 API の増加の抑制を重視しました。  
理由: 製品内の呼び出し元が 1 か所だけで、その用途は、出力先が未存在の場合が主であり、ファイルの識別子での判定が効く場面が限られます。BL 自身も「利用側が現れた場合」を条件にしています。公開 API を足すと、mock、export、文書の移行が要ります。

**Related**

[Q-001](#q-001-対策する残課題の範囲)、[Q-023](#q-023-同一性の判定で存在しないパスを渡した場合の結果)

### Q-015 Windows 実機確認での環境変更の許可

**Status**

解決済み

**Question**

Windows 実機の確認 (BL-020、BL-009、BL-024) のために、Windows 側が行ってよい環境の変更の範囲を、どれにしますか。

**Context**

- **経緯と背景**: 3 件の確認の前提は、本文の手順から次のように判定できます。
    - BL-024 (`WriteConsoleU` の途中の NUL): 新しいコンソール (`CREATE_NEW_CONSOLE` または `AllocConsole`) を作る確認用の実行ファイルが、`WriteConsoleU` で NUL を含む入力を書き、画面バッファー (`ReadConsoleOutputCharacterW`) を読んで結果を記録します。対話は要らず、管理者権限も要らない見込みです。
    - BL-020 (`longPathAware`): `MAX_PATH` を超える深さのディレクトリへ、`longPathAware` のマニフェストを組み込んだ確認用の実行ファイルとモジュールを置き、`cplat_module_get_path` と `cplat_process_get_executable_path` の結果を記録します。OS の長いパスの設定 (レジストリ `HKLM\SYSTEM\CurrentControlSet\Control\FileSystem` の `LongPathsEnabled`) が 1 であることが前提です。
    - BL-009 (長いパスの `tcpServer`): BL-020 と同じ前提で、`tcpServer` を長いパスへ置き、`fork` と `prefork` を起動して通信します。実行ファイルへマニフェストを組み込む必要があります。
- **確認できた事実**: リポジトリの実行ファイルには、`longPathAware` のマニフェストがありません (`grep` で確認)。このため、確認では、マニフェストを組み込んだ確認用の実行ファイル (または実行ファイルの複製へ `mt.exe` で組み込んだもの) を、一時ディレクトリで作ります。リポジトリのソースは変更しません。
- **管理者権限の要否**: `LongPathsEnabled` がすでに 1 の環境なら、読み取りだけで足り、管理者権限は要りません。0 の環境で 1 に変えるには、管理者権限 (UAC の承認など、ユーザーの対話) と、プロセスの再起動またはサインアウトが要る場合があります。
- **各案の具体例**:

```text
案 A (推奨): 環境を変えない。事前確認で LongPathsEnabled が 1 なら BL-020 と BL-009 を実施し、
  0 なら実施せず「環境が前提を満たさない」と報告して、BL を「検討中」のまま残す。BL-024 は常に実施する。
```

```text
案 B: LongPathsEnabled を管理者権限で一時的に 1 に変更し、確認の後に元の値へ戻す。
  Windows 側で UAC の承認が要る (ユーザーの操作)。変更と復元は、記録として報告する。
```

```text
案 C: BL-020 と BL-009 を今回は見送り、BL-024 だけを実施する。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 環境を変えない。前提を満たす場合だけ実施し、満たさない場合は報告して残す
- [ ] B: 管理者権限で `LongPathsEnabled` を一時的に変更し、確認の後に戻す (ユーザーが UAC を承認する)
- [ ] C: BL-020 と BL-009 は今回見送り、BL-024 だけ実施する
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 20:52:24 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (環境を変えず、前提を満たす場合だけ実施) は単一選択として明確です。
- `LongPathsEnabled` は `0x1` を確認済みであり、BL-020 と BL-009 を実施できます。
- Windows 側のリポジトリの `core.longpaths` は未設定です。確認では、リポジトリを長いパスへ取得し直さず、ビルドした実行ファイルを一時ディレクトリの長いパスへ複製して使うため、Git の設定の変更は要りません。A と整合します。
- `longPathAware` のマニフェストを組み込む `mt.exe` の有無は、Windows 側で読み取りだけの確認を依頼します。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 環境の非変更 (実機の状態を変えないこと) を重視し、次にユーザーの操作が要らない進行 (Windows 側 Claude Code の非対話の実行) を重視しました。  
理由: 環境の設定の変更は、他の作業や他の確認に影響し得ます。事前確認の結果によっては、BL-020 と BL-009 を実施できず、「検討中」のまま次の案件へ引き継ぎます。案 B は確認を完了できますが、UAC の承認というユーザーの対話が要り、設定の復元の確認も要ります。実機の設定を変えてよいかは、ユーザーの判断です。範囲 ([Q-001](#q-001-対策する残課題の範囲)) で M と N を選ばない場合、この質問は、BL-024 に関しては回答が要りません。

**Related**

[Q-001](#q-001-対策する残課題の範囲)

### Q-016 作業ブランチのコミットを受け渡す経路

**Status**

解決済み

**Question**

WSL 側と Windows 側の作業ツリーの間で、作業ブランチのコミットを受け渡す経路を、どれにしますか。

**Context**

- **経緯と背景**: [Q-002](#q-002-windows-側での検証の受け渡し方式) の補足は「作業ブランチでコミットしながら 2 つの OS で作業を進めてもよい。最終的には main にマージすること。」です。[Q-002](#q-002-windows-側での検証の受け渡し方式) の **Context** は、コミットせずにパッチ (`git diff` と `git apply`) で受け渡す前提でした。`AGENTS.md` は Git の書き込み操作の許可を単発として扱うため、補足を許可として使うには、範囲を最終仕様に明記する必要があります。範囲を決める判断を、この Q から [Q-019](#q-019-main-の-push-の扱い) までの 4 問に分けます。
- **補足から読み取った事項** (異なる場合は、「その他」に記入してください):
    - この案件の作業で、作業ブランチの作成と、そのためのステージングとコミットを行ってよい。
    - WSL 側と Windows 側の両方のエージェントがコミットしてよい。Windows 側のコミットは、[Q-002](#q-002-windows-側での検証の受け渡し方式) の B のとおり、WSL 側 (主エージェント) が差分を確認してから取り込む。
    - 作業ブランチは、変更するすべてのリポジトリに、同じ名前 (例: `work/261125-consumable-backlog-items`) で作る。この案件が変更するのは、親リポジトリ (`app/struct-meta`、`app/calc` などの app、検討文書) と、サブモジュール `app/cplat`、`app/porter`、`app/cjson`、`framework/testfw` です。
- **確認できた事実**:
    - 2 つの作業ツリーは別々のクローンです (WSL 側 `/home/user/c-modernization-kit`、Windows 側 `D:\Users\tetsuo\Local\repos\c-modernization-kit`)。`origin` は GitHub の公開リポジトリ (`Hondarer/c-modernization-kit` と、サブモジュールごとのリポジトリ) です。
    - CI (`.github/workflows/ci.yml`) は、main への push と main への pull request だけで起動します。作業ブランチの push では CI は動きません。
    - 親リポジトリが記録するサブモジュールのコミット (gitlink) は、相手の作業ツリーから取得できる場所にないと、相手側で `git submodule update` ができません。
    - 並行する案件 (`app/porter` の `261123-porter-remaining-backlog-phase2` は `framework/testfw`、`app/cplat`、`app/porter` を変更する予定、`framework/makefw` の `261125-makefw-backlog`) が、同じ期間に main を進める可能性があります。
- **各案の具体例**:

```text
案 A (推奨): 作業ブランチを origin へ push し、相手側は origin から fetch する。
  WSL 側:     git -C app/cplat push origin work/261125-consumable-backlog-items  (変更した各リポジトリ)
  Windows 側: git fetch origin; git switch work/261125-consumable-backlog-items; git submodule update
  main へ統合した後、作業ブランチをリモートとローカルから削除する。
  作業ブランチは公開リポジトリ上で見える。CI は動かない。
```

```text
案 B: origin を使わず、2 つのクローンの間で直接 fetch する。
  WSL 側: git remote add windows /mnt/d/Users/tetsuo/Local/repos/c-modernization-kit (サブモジュールごとにも登録)
  Windows 側: \\wsl.localhost\<ディストリビューション>\home\user\c-modernization-kit を remote として登録
  外部へは出ない。リポジトリごとの remote の登録と、OS をまたぐファイル システムの参照 (遅さ、権限の差) が要る。
```

```text
案 C: コミットは WSL 側だけで行う。Windows 側とは、これまでどおりパッチで受け渡す。
  Windows 側の MSVC 固有の修正もパッチで返し、WSL 側がコミットする。
  補足の「2 つの OS で作業を進める」は、Windows 側がコミットしない形で満たす。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 作業ブランチを origin へ push し、相手側は origin から fetch する。統合後に作業ブランチを削除する
- [ ] B: origin を使わず、2 つのクローンの間で直接 fetch する
- [ ] C: コミットは WSL 側だけで行い、Windows 側とはパッチで受け渡す
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 21:33:48 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (作業ブランチを origin へ push し、相手側は origin から fetch する。統合後に作業ブランチを削除する) は単一選択として明確です。補足はありません。
- **Context** の「補足から読み取った事項」(作業ブランチの作成とコミットの許可、両側のエージェントのコミット、変更する全リポジトリでの同名の作業ブランチ) に、「その他」での異議はありません。読み取ったとおりに扱います。
- [Q-017](#q-017-main-へ統合する時点) の C (BL の単位で統合) と組み合わせるため、作業ブランチは案件の全期間で 1 本とし、統合のたびに削除せず、最後の統合の後に削除します。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 2 つの作業ツリーの状態をコミットの単位で確実に一致させることを重視し、次に手順の単純さ (サブモジュールを含めて標準の `fetch` と `submodule update` だけで済むこと)、外部への公開の最小化を重視しました。  
理由: 案 A は、サブモジュールのコミットも origin から取得できるため、親リポジトリの gitlink がそのまま相手側で解決できます。作業ブランチの push では CI は動きません。案 B は外部へ出ませんが、リポジトリごとの remote の登録が要り、Windows の Git から WSL のファイル システムを参照する操作は遅く、ファイルの属性の差が出ることがあります。案 C は、従来の手順のままで済む一方、Windows 側の修正をコミットとして残せず、補足の意図に沿いにくいです。

**Related**

[Q-002](#q-002-windows-側での検証の受け渡し方式)、[Q-017](#q-017-main-へ統合する時点)、[Q-018](#q-018-main-へ統合する方法)、[Q-019](#q-019-main-の-push-の扱い)

### Q-017 main へ統合する時点

**Status**

解決済み

**Question**

作業ブランチを main へ統合する時点を、どれにしますか。

**Context**

- **経緯と背景**: [Q-002](#q-002-windows-側での検証の受け渡し方式) の補足は「最終的には main にマージすること」です。統合する時点は示されていません。
- **事実**: この案件は、最終仕様の承認 (第 2 ゲート) の後に作業を行い、成果物の承認 (第 3 ゲート) で完了します。14 件の BL は、WSL で完結する単位と、Windows の検証が要る単位に分かれます。並行する案件が main を進める場合、統合の時点が遅いほど、衝突の解消をまとめて行うことになります。
- **各案の具体例**:

```text
案 A (推奨): 第 3 ゲートで成果物の承認を受けた後に、全リポジトリをまとめて統合する。
  承認前の成果物は main に入らない。第 3 ゲートでの指摘は、作業ブランチで直してから統合する。
```

```text
案 B: 全作業の完了後、第 3 ゲートの提示の前に統合する。
  提示の時点で main が最終形になる。指摘を受けた場合は、main 上で追加のコミットで直す。
```

```text
案 C: BL の単位で、Linux と Windows の検証が済むごとに統合する。
  統合の回数が増える。並行案件との衝突を小さい単位で解消できる。
```

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) 第 3 ゲートで成果物の承認を受けた後に、まとめて統合する
- [ ] B: 全作業の完了後、第 3 ゲートの提示の前に統合する
- [x] C: BL の単位で、検証が済むごとに統合する
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 21:33:48 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 C (BL の単位で、検証が済むごとに統合する) は単一選択として明確です。推奨外です。補足はありません。
- 第 3 ゲートとの照合: C は、成果物の承認 (第 3 ゲート) の前に、BL の単位で main へ統合することを意味します。選択肢の文言は、統合の契機を「検証が済むごと」と定めており、BL ごとのユーザーの受入確認を契機に含めていません。このため、次のとおり扱います。
    - 統合、push、CI の確認は、第 2 ゲートで承認する最終仕様に、許可の範囲 (リポジトリ、ブランチ、操作、契機) として明記し、その範囲内の作業として行います。最終仕様の承認を、この範囲の Git の書き込み操作の許可とします。
    - 統合の契機は、その単位の受入条件のうち、局所テスト (Linux) と、要る場合は Windows の MSVC のビルドとテストが、エージェントの確認で満たされたことです。BL ごとのユーザーの受入確認は求めません。
    - 第 3 ゲートは、全単位の統合と CI の結果をまとめた成果物の承認として残します。第 3 ゲートでの指摘は、作業ブランチで直し、同じ手順で再び統合します。
- この扱いは選択肢の文言から一意に導けるため、追加の Q は設けません。異なる場合は、最終仕様の承認 (第 2 ゲート) の際に指摘を受けます。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 承認前の成果物を main に入れないこと (合意の手順との整合) を重視し、次に統合の操作の回数の少なさを重視しました。  
理由: 案 A は、第 3 ゲートの承認と main への統合が一致し、承認されなかった変更が main に残りません。案 B は、指摘を受けた場合に main の上で直すことになります。案 C は、並行案件との衝突を早く見つけられますが、統合と CI の回数が BL の単位で増えます。

**Related**

[Q-002](#q-002-windows-側での検証の受け渡し方式)、[Q-016](#q-016-作業ブランチのコミットを受け渡す経路)、[Q-018](#q-018-main-へ統合する方法)、[Q-019](#q-019-main-の-push-の扱い)

### Q-018 main へ統合する方法

**Status**

解決済み

**Question**

作業ブランチを main へ統合する方法を、どれにしますか。

**Context**

- **経緯と背景**: [Q-002](#q-002-windows-側での検証の受け渡し方式) の補足の「マージ」の方法は示されていません。この案件は、親リポジトリと 4 つのサブモジュールを変更するため、サブモジュールの統合の結果を、親リポジトリの gitlink に反映する必要があります。
- **事実**: 親リポジトリと各サブモジュールの履歴には、マージ コミットがあります (例: 親リポジトリの `707a7d3f Merge remote-tracking branch 'origin/main'`、`app/cplat` の `73911d87`)。並行する案件が main を進めた場合、早送り (fast-forward) では統合できません。
- **各案の具体例**:

```text
案 A (推奨): マージ コミット (git merge --no-ff) で統合する。
  1. 各サブモジュール: main へ作業ブランチをマージする (衝突があれば解消する)。
  2. 親リポジトリの作業ブランチ: gitlink を、各サブモジュールの main のマージ コミットへ更新してコミットする。
  3. 親リポジトリ: main へ作業ブランチをマージする。
  作業ブランチのコミットの履歴が残る。
```

```text
案 B: 早送りだけで統合する (git merge --ff-only)。
  main が進んでいる場合は、作業ブランチを main へ rebase してから統合する。
  rebase はコミットを作り直すため、origin へ push 済みの作業ブランチは強制 push が要り、
  親リポジトリの gitlink も作り直したコミットへ付け替える。
```

```text
案 C: squash (1 つのコミットにまとめる) で統合する。
  main の履歴は 1 リポジトリ 1 コミットになる。作業ブランチのコミット (Windows 側の修正を含む) の履歴は main に残らない。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) マージ コミット (`--no-ff`) で、サブモジュール、親リポジトリの順に統合する
- [ ] B: 早送りだけで統合する (main が進んでいれば rebase してから)
- [ ] C: squash で統合する
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 21:33:48 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (マージ コミット (`--no-ff`) で、サブモジュール、親リポジトリの順に統合) は単一選択として明確です。補足はありません。
- [Q-017](#q-017-main-へ統合する時点) の C により、統合は単位ごとに繰り返します。main が進んでいた場合 (並行する案件の統合を含む) は、統合の前に main を作業ブランチへマージして衝突を解消し、その単位の検証をやり直してから統合します。履歴を書き換えないため、A と整合します。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 並行する案件が main を進めた場合にも、履歴を書き換えずに統合できることを重視し、次に既存の履歴の書き方との一致を重視しました。  
理由: 案 A は、作業ブランチのコミットを書き換えないため、Windows 側と共有した作業ブランチをそのまま統合できます。案 B は履歴が一直線になりますが、main が進んでいると rebase と強制 push が要ります。案 C は履歴が簡潔になりますが、Windows 側の修正の取り込みなど、作業ブランチ上の経緯が main から追えなくなります。

**Related**

[Q-002](#q-002-windows-側での検証の受け渡し方式)、[Q-016](#q-016-作業ブランチのコミットを受け渡す経路)、[Q-017](#q-017-main-へ統合する時点)、[Q-019](#q-019-main-の-push-の扱い)

### Q-019 main の push の扱い

**Status**

解決済み

**Question**

main へ統合した後の、origin の main への push を、この案件の作業に含めますか。

**Context**

- **経緯と背景**: [Q-002](#q-002-windows-側での検証の受け渡し方式) の補足の「main にマージ」が、ローカルの main への統合までを指すのか、origin の main への push までを指すのかは示されていません。
- **事実**: origin の main への push は CI を起動します。CI の 1 回の実行には数時間かかります (`app/general/docs/workspace-agent-workflow.md` の「Git の扱い」)。サブモジュールの main を先に push しないと、親リポジトリの main の gitlink を CI が取得できません。
- **各案の具体例**:

```text
案 A (推奨): サブモジュール、親リポジトリの順に origin の main へ push し、CI の結果を確認するまでを作業に含める。
  CI が失敗した場合は、原因を調べて修正する (一過性の失敗は根拠を確かめて再実行する)。
```

```text
案 B: ローカルの main への統合までを作業とし、push はユーザーが行う。
  CI の確認は、ユーザーの push の後に別の依頼として扱う。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) サブモジュール、親リポジトリの順に push し、CI の結果の確認までを作業に含める
- [ ] B: ローカルの main への統合までとし、push はユーザーが行う
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 21:33:48 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (サブモジュール、親リポジトリの順に push し、CI の結果の確認までを作業に含める) は単一選択として明確です。補足はありません。
- [Q-017](#q-017-main-へ統合する時点) の C と組み合わせると、統合の単位ごとに CI (1 回に数時間) が動きます。CI の結果を待つ間に、次の単位の実装を作業ブランチで進めます。CI の失敗は、原因を調べて作業ブランチで直し、同じ手順で統合し直します。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 統合した成果物が CI (Linux と Windows の全体のビルドとテスト) を通ることの確認を、作業の完了条件に含めることを重視しました。  
理由: この案件は、cplat の公開 API (`cplat_argparser_init` の戻り値、新しい同一性の判定の関数) と mock を変え、多くの app に波及します。局所テストだけでは、全体のビルドと Windows の CI の結果を確かめられません。案 B は push の判断をユーザーに残せますが、CI の失敗の修正が別の依頼になります。

**Related**

[Q-002](#q-002-windows-側での検証の受け渡し方式)、[Q-016](#q-016-作業ブランチのコミットを受け渡す経路)、[Q-017](#q-017-main-へ統合する時点)、[Q-018](#q-018-main-へ統合する方法)

### Q-020 JSONC の共通ロックの扱い

**Status**

解決済み

**Question**

`global_error` をスレッド ローカルにした後 ([Q-006](#q-006-cjson-の直接解析と-global_error-の競合の対策方式) の A)、JSONC のラッパーの共通ロックを削除しますか。

**Context**

- **経緯と背景**: [Q-006](#q-006-cjson-の直接解析と-global_error-の競合の対策方式) の **Recommendation** は「JSONC のロックは、確保失敗の計数の保護に残るが、計数もスレッド ローカル化できれば不要になる」としていました。再調査で、計数はすでにスレッド ローカルであることが分かりました。
- **確認できた事実**:
    - 確保失敗の計数 `jsonc_allocation_failure_count` は、`CJSON_APP_JSONC_THREAD_LOCAL` 付きの静的変数です (`app/cjson/prod/libsrc/cjson/jsonc/cJSON_JSONC.c` の 62 行)。
    - 共通ロック (`jsonc_parse_exclusion`、Linux は `pthread_mutex_t`、Windows は `SRWLOCK`) は、cJSON 本体の解析 (`cjson_app_cJSON_ParseWithLengthOpts()`) の呼び出しだけを囲みます (同 414 から 416 行)。`app/cjson/README.md` の 50 行も「cJSON 本体の解析呼び出しだけを直列に実行」と記載しています。
    - cJSON 本体の書き換えできる静的変数は、`global_error` と、ロード後に変えない `global_hooks` だけです (`cJSON.c` の 95 行と 189 行)。`global_error` をスレッド ローカルにすると、解析の間にスレッドの間で共有して書き換える状態は残りません。
    - 直列化を確かめる既存のテストがあります (`app/cjson/test/src/cmd/cjsonTest/cjsonJsoncTest.cc` の `serializes_new_and_existing_parser_calls` と、`cJSON_JSONC.inject.c` の `cjson_jsonc_test_parse_lock_is_available()`)。
    - 利用側の文書は、ロックを前提に「JSONC の解析は共通ロックを通るため、解析どうしは競合しません」と記載しています (`app/porter/prod/include/porter/porter_spec.h` の 461、521、554 行、`app/porter/docs/api.md` の 97 から 104 行)。
- **各案の具体例**:

```c
/* 案 A (推奨): ロックを削除する。解析は並行に実行できる */
end = NULL;
result = cjson_app_cJSON_ParseWithLengthOpts(normalized, buffer_length + 1U, &end, 1);
/* serializes_new_and_existing_parser_calls は、並行する解析の結果と、
   スレッドごとの cJSON_GetErrorPtr() が独立であることを確かめるテストへ置き換える。 */
```

```c
/* 案 B: ロックを維持する。JSONC の解析は、これまでどおりプロセス全体で直列に実行する */
jsonc_parse_exclusion_acquire();
result = cjson_app_cJSON_ParseWithLengthOpts(normalized, buffer_length + 1U, &end, 1);
jsonc_parse_exclusion_release();
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 共通ロックを削除し、直列化のテストを、並行する解析が独立であることを確かめるテストへ置き換える
- [ ] B: 共通ロックを維持する
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 21:33:48 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (共通ロックを削除し、直列化のテストを、並行する解析が独立であることを確かめるテストへ置き換える) は単一選択として明確です。補足はありません。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: 守る対象のない排他を残さないこと (理由のない制約を残さない) を重視し、次に解析の並行性 (設定の検証や列挙をスレッドごとに並行に実行できること) を重視しました。  
理由: A の後は、ロックが守る共有の状態がありません。B はロックを残しても誤りにはなりませんが、守る対象のない直列化がプロセス全体の解析を 1 本にし、利用側の文書も「ロックを通るので競合しない」という、根拠の変わった説明のまま残ります。A では、porter の文書の説明を「解析ごとに独立した状態を使うため競合しない」へ改めます。

**Related**

[Q-006](#q-006-cjson-の直接解析と-global_error-の競合の対策方式)

### Q-021 フィールドと構造体以外の診断で報告する行

**Status**

解決済み

**Question**

フィールドと構造体以外の診断 (ドキュメント コメントの誤り、`long` の拒否、構文エラー) で報告する行を、どの規則で決めますか。

**Context**

- **経緯と背景**: [Q-009](#q-009-フィールド診断で報告する行の定義) で、フィールドの診断はフィールド名の行、構造体の診断は構造体名の行と決まりました。構文解析器 (`app/struct-meta/prod/libsrc/struct_meta/parse/struct_meta_parse.y`) には、ほかにも還元時の `yyget_lineno(scanner)` を使う診断があり、同じく先読みの影響を受けます。BL-031 の「本来あるべき姿」は「誤りのある字句が置かれた実際の行番号が出力される状態」です。
- **対象の診断** (`struct_meta_parse.y` の行番号):
    - ドキュメント コメントの属性の誤り: 前置のコメント (`doc_prefix`、137 行) と後置のコメント (`doc_postfix`、152 行)。複数行のコメントがあります。
    - `long` と `unsigned long` の拒否 (`REJECT_PLATFORM_DEPENDENT_TYPE()`、35 行)。
    - 構文エラー (`yyerror()`、259 行)。
- **例** (行番号を左に付けます):

```text
3  /**
4   * @brief 値
5   * @meta.kind 不明な値      <- 属性の誤り
6   */
7  long value;               <- long の拒否
8  int other int;            <- 構文エラー (2 つ目の int で誤りを検出)
```

- **各案の具体例**: 上の例で報告する行は次のとおりです。
    - 案 A (推奨): 誤りの字句そのものの行。属性の誤りはコメントの開始行 (3)、`long` の拒否は `long` の行 (7)、構文エラーは bison が誤りを検出した字句の行 (8)。
    - 案 B: [Q-009](#q-009-フィールド診断で報告する行の定義) と同じく、対象の名前の行に揃える。属性の誤りはコメントが付くフィールドの名前の行 (7)、`long` の拒否はフィールド名の行 (7)、構文エラーは名前がないため誤りを検出した字句の行 (8)。
    - 案 C: 現状維持 (還元時の行のまま。今回はフィールドと構造体だけを直す)。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 誤りの字句そのものの行 (コメントは開始行、`long` はそのキーワードの行、構文エラーは誤りを検出した字句の行)
- [ ] B: 対象の名前の行に揃える (名前のない構文エラーだけ、誤りを検出した字句の行)
- [ ] C: 現状維持
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 21:33:48 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (誤りの字句そのものの行) は単一選択として明確です。補足はありません。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: BL-031 の「本来あるべき姿」(誤りのある字句の実際の行) との一致を重視し、次に先読みの影響を構文解析器の全体から除くこと (診断ごとの例外を残さないこと) を重視しました。  
理由: 属性の誤りは、コメントの中にあり、フィールドの名前の行にはありません。案 A は、利用者が誤りを直す位置をそのまま示します。案 B は、診断の行がフィールドの単位で揃いますが、複数行のコメントの誤りでは、誤りの位置とずれます。案 C は、`%locations` を導入した後も、一部の診断に先読みのずれが残ります。

**Related**

[Q-008](#q-008-struct-meta-のフィールド診断の行番号の修正方式)、[Q-009](#q-009-フィールド診断で報告する行の定義)

### Q-022 長いパスの一時ファイルでの拡張パスの接頭辞

**Status**

解決済み

**Question**

一時ファイルの完全なパスが `MAX_PATH` 以上になる場合に、拡張パスの接頭辞 (`\\?\`) を付けて作成し、その形のパスを利用側へ返しますか。

**Context**

- **経緯と背景**: [Q-013](#q-013-長いパスの一時ファイルの作成方式) の A で、`GetTempFileNameW` を、cplat が名前を作って `CreateFileW` (`CREATE_NEW`) で作る内部の共通関数へ置き換えると決まりました。`GetTempFileNameW` 固有の制限 (ディレクトリ部が `MAX_PATH - 14` 文字まで、出力先が `MAX_PATH` 要素) はなくなります。しかし、OS の制限として、プロセスが `longPathAware` でない (マニフェストがない) 場合、`MAX_PATH` 以上のパスは、接頭辞を付けない限り `CreateFileW` と `_wfsopen` で開けません。
- **確認できた事実**:
    - cplat の既存の UTF-8 のラッパー (`CreateFileU` など) は、接頭辞を付けずにそのまま OS へ渡します (`app/cplat/prod/libsrc/cplat/win32/file_api.c`)。利用側が接頭辞付きのパスを渡せば、そのまま通ります。
    - リポジトリの実行ファイルには、`longPathAware` のマニフェストがありません。
    - 昇格の結果ファイルのパスは、昇格した子プロセスへコマンド ラインで渡し、子プロセスが書き込みます (`app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` の 1291 から 1360 行)。子プロセスも `longPathAware` とは限りません。
    - `GetTempPath2W` / `GetTempPathW` は、末尾に区切りを持つ完全なパスを返します。接頭辞を付けたパスは OS が正規化しないため、区切りは `\` で、`.` や `..` を含まない完全なパスである必要があります。UNC のパスは `\\?\UNC\` の形にします。
    - see: [Maximum Path Length Limitation](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation)
- **各案の具体例**:

```text
案 A (推奨): 完全なパスが MAX_PATH 以上になる場合だけ、接頭辞を付けて作成し、その形で返す。
  一時ディレクトリ: C:\Users\u\AppData\Local\Temp\<長い階層>\
  作成と返すパス:  \\?\C:\Users\u\AppData\Local\Temp\<長い階層>\cu_0123456789abcdef.tmp
  MAX_PATH 未満の通常の場合は、これまでどおり接頭辞なしのパスを返す。
  longPathAware でないプロセス (昇格した子プロセスを含む) でも作成と書き込みができる。
```

```text
案 B: 接頭辞を付けない (cplat の他の API と同じ方針)。
  MAX_PATH 以上のパスは、longPathAware のプロセス (OS の LongPathsEnabled が 1) でだけ作れる。
  それ以外のプロセスでは、CreateFileW の失敗を結果コードと詳細エラーで返す。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `MAX_PATH` 以上になる場合だけ接頭辞を付けて作成し、その形のパスを返す
- [ ] B: 接頭辞を付けない (長いパスは `longPathAware` のプロセスでだけ作れる)
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 21:33:48 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 A (`MAX_PATH` 以上になる場合だけ接頭辞を付けて作成し、その形のパスを返す) は単一選択として明確です。補足はありません。
- 解決済みとします。

**Recommendation**

推奨: A  
優先した基準: BL-004 の「本来あるべき姿」(一時ディレクトリが長くても一時ファイルを作れる) を、プロセスのマニフェストに依存せずに満たすことを重視し、次に通常の長さのパスの扱いを変えないことを重視しました。  
理由: 案 A は、現在は失敗する場合だけ振る舞いが変わり、通常の場合の返すパスは変わりません。昇格の結果ファイルのように、マニフェストを持たない子プロセスへ渡すパスでも動きます。案 B は cplat の他の API と方針が揃いますが、`longPathAware` でないプロセスでは、長い一時ディレクトリで引き続き失敗し、BL の解消は条件付きになります。

**Related**

[Q-013](#q-013-長いパスの一時ファイルの作成方式)

### Q-023 同一性の判定で存在しないパスを渡した場合の結果

**Status**

解決済み

**Question**

新しい同一性の判定の関数に、存在しないパスを渡した場合の結果を、どれにしますか。

**Context**

- **経緯と背景**: [Q-014](#q-014-ntfs-の同一性判定の扱い) の A で、ファイルの識別子で同じ実体かを判定する公開 API を追加し、`compress-cli` へ適用すると決まりました。**Context** で挙げた未決の点が、存在しないパスの扱いです。ファイルの識別子は、存在するファイルを開いて (Linux は `stat` して) 得るため、存在しないパスには識別子がありません。
- **利用側の実態**: `compress-cli` は、入力と出力が同じファイルかを、出力を開く前に確かめます (`app/cplat/prod/src/cmd/compress-cli/compress-cli.c` の 247 行、現在は `cplat_path_equal()`)。入力は存在するファイルで、出力は通常は存在しません (新しく作る)。
- **関数の形の案** (名前と引数は最終仕様で定めます):

```c
/* lhs と rhs が同じ実体なら *same_out に 1、異なれば 0 を格納する。 */
CPLAT_EXPORT int CPLAT_API cplat_path_is_same_file(const char *lhs, const char *rhs, int *same_out,
                                                    cplat_error *detail_out);
```

- **各案の具体例** (出力が存在しない場合の `compress-cli`):

```c
/* 案 A: 存在しないパスがあれば CPLAT_ERR_NOT_FOUND を返す */
rc = cplat_path_is_same_file(input, output, &same, &error);
if (rc == CPLAT_ERR_NOT_FOUND) { same = 0; }      /* 出力が未作成なら同じ実体ではない */
else if (rc != CPLAT_OK) { /* 比較の失敗として終了 */ }
```

```c
/* 案 B (推奨): 存在しないパスがあれば、同じ実体ではないとして CPLAT_OK と *same_out = 0 を返す */
rc = cplat_path_is_same_file(input, output, &same, &error);
if (rc != CPLAT_OK) { /* 比較の失敗 (権限など) として終了 */ }
```

```c
/* 案 C: 存在しないパスがあれば、文字列の比較 (cplat_path_equal() と同じ結果) へ戻る */
/* 両方とも存在しない同じ文字列のパスは「同じ」になる。 */
```

**Answer**

選択方式: 単一選択

- [ ] A: 存在しないパスがあれば `CPLAT_ERR_NOT_FOUND` を返す
- [x] B: (推奨) 存在しないパスがあれば、同じ実体ではないとして `CPLAT_OK` と `*same_out = 0` を返す
- [ ] C: 存在しないパスがあれば、文字列の比較の結果へ戻る
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-25 21:33:48 JST (検討用 Markdown の保存時刻)

**Assessment**

- 選択 B (存在しないパスがあれば、同じ実体ではないとして `CPLAT_OK` と `*same_out = 0` を返す) は単一選択として明確です。補足はありません。
- 解決済みとします。

**Recommendation**

推奨: B  
優先した基準: 関数の問い (同じ実体か) への直接の答えを返すことを重視し、次に利用側の分岐の少なさを重視しました。  
理由: 存在しないパスは実体を持たないため、存在する実体と同じになることはなく、「同じではない」は問いへの正しい答えです。`compress-cli` の主な用途 (出力が未作成) で、利用側の分岐が要りません。案 A は存在の有無を区別できますが、利用側が毎回 `CPLAT_ERR_NOT_FOUND` を成功として扱い直す必要があります。案 C は、実体の判定と文字列の判定の意味が混ざり、結果の意味が入力によって変わります。存在の確認が要る利用側は、別の関数で確かめられます。

**Related**

[Q-014](#q-014-ntfs-の同一性判定の扱い)
