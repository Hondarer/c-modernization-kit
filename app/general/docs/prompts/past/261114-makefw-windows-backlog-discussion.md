<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新日時: 2026-11-15 13:38:55 JST
稼働 OS: Windows
最終更新エージェント: codex (セッション ID は取得できません。前担当: agy / 2a9bad6c-0328-438b-aea8-8c3f8d306f6d)
上流文書: `../past/261114-windows-real-machine-verification-backlog.md` (過去 backlog。継続作業の元)。
下流文書: `261114-makefw-windows-backlog-design.md` (最終仕様)、`261114-makefw-windows-backlog-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# Windows 実機検証残課題 (BL-002〜BL-004) 対策の検討

ファイル名: `261114-makefw-windows-backlog-discussion.md`

## 検討の識別

検討 ID: makefw-windows-backlog-20261114

版: 3

状態: 完了

## 作業 WBS と進捗管理

この検討用 Markdown が管理する作業 WBS です。  
具体的な作業対象を明記し、Q&A のターンごとに必要に応じて見直します。各ゲートの通過・承認および作業の進行に従って状態を更新します。見送りの決定や範囲外の気付きは、どの段階でも生じた時点で `261114-makefw-windows-backlog-backlog.md` に追記します。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 前提条件と要求事項の調査 (`../past/261114-windows-real-machine-verification-backlog.md` の BL-002〜BL-004、および `framework/makefw` の該当実装・テストの調査)
    - ✅ 1.2. 課題・論点の抽出と明確化のための確認項目 ([Q-001](#q-001-今回の対策対象範囲)〜[Q-005](#q-005-検証範囲と実機確認の境界)) の作成
    - ✅ 1.3. 保存済みの [Q-001](#q-001-今回の対策対象範囲)〜[Q-005](#q-005-検証範囲と実機確認の境界) の A 回答を受付 (元の選択は保持)
    - ✅ 1.4. 本文・回答と実装を照合し、追加論点と複合質問を検出
    - ✅ 1.5. 追加質問 [Q-006](#q-006-リンク構成の記録方式)〜[Q-010](#q-010-ソース分類に残る長いシェル展開) への回答受付と再検証
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定 (懸念がないことをもってフェーズ 2 へ着手)
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `261114-makefw-windows-backlog-design.md` の作成
    - ✅ 3.2. 最終仕様における受入条件と整合性の確認
    - ✅ 3.3. 対話セッションにおける最終仕様 (`261114-makefw-windows-backlog-design.md`) の承認の受付 (第 2 ゲート: 作業着手の合意)
- ✅ 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. BL-002 対策: `framework/makefw/makefiles/_link_objects.mk`、`makesrc_c_cpp.mk`、`makelibsrc_c_cpp.mk` のリンク状態追跡の更新
    - ✅ 4.2. BL-003 対策: `framework/makefw/bin_internal/msvc_compile.ps1` の診断パス照合による `.warn` 割り当てと陳腐化 `.warn` 清掃の実装
    - ✅ 4.3. BL-004 対策: `framework/makefw/makefiles/_collect_srcs.mk`、`makesrc_c_cpp.mk`、`makelibsrc_c_cpp.mk` の外部ソース判定の Make 関数化
    - ✅ 4.4. 回帰テストの追加・更新 (`framework/makefw/bin_test/link_objects_selftest.py`、`test_path_spaces.py`、および MSVC `.warn` 割り当てセルフ テスト)
    - ✅ 4.5. 正本ドキュメントおよびコード コメントへの永続的知見の反映
    - ✅ 4.6. 局所テスト (`framework/makefw/bin_test/` と `app/porter` 摘出箇所) および機械的検証 (`text_style_jp.py`、内容のある `.warn` 確認) の実施
    - ✅ 4.7. 受入条件への適合確認 (全完確認)
    - ✅ 4.8. 過去 backlog (`../past/261114-windows-real-machine-verification-backlog.md`) の状態・経緯・対処の更新、および必要に応じた本案件 backlog (`261114-makefw-windows-backlog-backlog.md`) の整理
- ✅ 5. 成果物の承認の受付 (第 3 ゲート)
    - ✅ 5.1. 成果物・検証結果・残課題の提示
    - ✅ 5.2. 対話セッションにおける成果物の承認の受付 (「承認」または「完了」の受領によるスキル完了)

## 対象と参照資料

対象成果物:

- `framework/makefw/makefiles/_link_objects.mk`
- `framework/makefw/makefiles/makesrc_c_cpp.mk`
- `framework/makefw/makefiles/makelibsrc_c_cpp.mk`
- `framework/makefw/makefiles/_collect_srcs.mk`
- `framework/makefw/bin_internal/msvc_compile.ps1`
- `framework/makefw/bin_test/` 配下のテスト スクリプト群
- `app/general/docs/prompts/past/261114-windows-real-machine-verification-backlog.md` (経緯と対処の更新)

今回の対象範囲: BL-002〜BL-004 の一括対策。方式の追加論点は解消済み。採用済み方針は ([Q-001](#q-001-今回の対策対象範囲)〜[Q-005](#q-005-検証範囲と実機確認の境界) で確認済み)。

書き込み可能な検討用 Markdown: この文書 (`app/general/docs/prompts/past/261114-makefw-windows-backlog-discussion.md`)

参照資料と版:

- [../past/261114-windows-real-machine-verification-backlog.md](../past/261114-windows-real-machine-verification-backlog.md) (BL-002、BL-003、BL-004)
- `framework/makefw` HEAD: `49a9ed24c3e2b55dea1263cf1e995d457919910f`
    - [_link_objects.mk](../../../../../framework/makefw/makefiles/_link_objects.mk)
    - [makesrc_c_cpp.mk](../../../../../framework/makefw/makefiles/makesrc_c_cpp.mk)
    - [makelibsrc_c_cpp.mk](../../../../../framework/makefw/makefiles/makelibsrc_c_cpp.mk)
    - [_collect_srcs.mk](../../../../../framework/makefw/makefiles/_collect_srcs.mk)
    - [_path_functions.mk](../../../../../framework/makefw/makefiles/_path_functions.mk)
    - [msvc_compile.ps1](../../../../../framework/makefw/bin_internal/msvc_compile.ps1)
    - [_msvc_utils.ps1](../../../../../framework/makefw/bin_internal/_msvc_utils.ps1)
    - [link_objects_selftest.py](../../../../../framework/makefw/bin_test/link_objects_selftest.py)
    - [test_path_spaces.py](../../../../../framework/makefw/bin_test/test_path_spaces.py)

## 現行の合意内容

全 10 問で A の単一選択を受領しました。  
BL-002〜BL-004 を一括対策し、リンク構成の差異を保持する記録方式、診断パス・依存情報による警告保存、互換性を維持したソース分類を採用します。  
共有ヘッダーの警告は今回の依存する全翻訳単位へ、照合不能な警告は今回の全翻訳単位へ保存し、重複を許容します。  
既存警告の清掃は、今回再コンパイルしたソースのパスに一致する診断行へ限定します。  
ソース分類と clean は既存のローカル実体優先・除外条件を維持し、_DIRECT_SYMLINKS を含む入力数依存の長いシェル展開を解消します。  
関連セルフ テストと摘出元の Windows / MSVC、Linux / GCC の局所検証を実施し、全体テストは実施しません。

保存済み回答と実装を横断して確認し、未回答・矛盾・残懸念がないため、第 1 ゲートを通過しました。  
本版 (版 3) を最終仕様作成の根拠とします。

## 本文レビューの記録

2026-11-15 に現行 makefw HEAD と参照コードを照合しました。  
HEAD は記録済みの `49a9ed24c3e2b55dea1263cf1e995d457919910f` と一致します。  
本文には、複合質問、実装の試案を確定的に評価した説明、再現条件が記録されていない原因断定があり、無条件で最終仕様へ移せる状態ではありません。  
[Q-002](#q-002-bl-002-におけるリンク状態追跡の対象範囲と記録方式) と [Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲) の採用範囲は保存済み回答として保持し、方式の不足は追加質問で確認します。  
元の質問本文や数値は判断履歴として残しますが、そのまま検証済み事実とは扱いません。  
日本語の補助レビューは agy の `gemini-3.8-flash-medium` で実施しました。  
既存 Q&A への表現修正案は、回答後の定義を保持するため適用していません。

スキル上の不足は、回答と未回答表示の不一致、Assessment の欠落、追加質問と WBS の未反映です。  
本版で回答の出典と検証結果を分離し、状態表示・相互参照を更新しました。  
実際の回答日時は記録がなく、ファイル更新日時から推定していません。

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | [Q-001](#q-001-今回の対策対象範囲) |
| 範囲・境界 | 確認済み | [Q-001](#q-001-今回の対策対象範囲)、[Q-002](#q-002-bl-002-におけるリンク状態追跡の対象範囲と記録方式)、[Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲) |
| 入力・根拠 | 確認済み | `../past/261114-windows-real-machine-verification-backlog.md`、`_link_objects.mk`、`msvc_compile.ps1`、`_collect_srcs.mk` のコード読解 (前担当の再現確認の詳細条件は今回未確認) |
| 整合性・依存関係 | 確認済み | [Q-002](#q-002-bl-002-におけるリンク状態追跡の対象範囲と記録方式)、[Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式) |
| 実現性・例外 | 確認済み | [Q-002](#q-002-bl-002-におけるリンク状態追跡の対象範囲と記録方式)、[Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲)、[Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式) |
| 受入条件・検証 | 確認済み | [Q-005](#q-005-検証範囲と実機確認の境界) |

Table: 観点別の確認状況

解消済みの追加確認事項: [Q-006](#q-006-リンク構成の記録方式)、[Q-007](#q-007-共有ヘッダー診断の割り当て)、[Q-008](#q-008-依存情報が不完全な場合の警告保存)、[Q-009](#q-009-外部ソース判定の互換性)、[Q-010](#q-010-ソース分類に残る長いシェル展開)。

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-今回の対策対象範囲) | 今回の対策対象範囲 (BL-002〜BL-004 の一括対処か分割対処か) | 解決済み | なし |
| [Q-002](#q-002-bl-002-におけるリンク状態追跡の対象範囲と記録方式) | BL-002 におけるリンク状態追跡の対象範囲と記録方式 | 解決済み | [Q-001](#q-001-今回の対策対象範囲) |
| [Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲) | BL-003 における MSVC 警告の割り当て方式と既存 `.warn` の整理範囲 | 解決済み | [Q-001](#q-001-今回の対策対象範囲) |
| [Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式) | BL-004 の外部ソース判定におけるコマンド ライン長制限の解消方式 | 解決済み | [Q-001](#q-001-今回の対策対象範囲) |
| [Q-005](#q-005-検証範囲と実機確認の境界) | 検証範囲と実機確認の境界 | 解決済み | [Q-001](#q-001-今回の対策対象範囲)〜[Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式) |
| [Q-006](#q-006-リンク構成の記録方式) | リンク構成の記録方式 | 解決済み | [Q-002](#q-002-bl-002-におけるリンク状態追跡の対象範囲と記録方式) |
| [Q-007](#q-007-共有ヘッダー診断の割り当て) | 共有ヘッダー診断の割り当て | 解決済み | [Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲) |
| [Q-008](#q-008-依存情報が不完全な場合の警告保存) | 依存情報が不完全な場合の警告保存 | 解決済み | [Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲) |
| [Q-009](#q-009-外部ソース判定の互換性) | 外部ソース判定の互換性 | 解決済み | [Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式) |
| [Q-010](#q-010-ソース分類に残る長いシェル展開) | ソース分類に残る長いシェル展開 | 解決済み | [Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式) |

Table: 質問一覧

## 明確化のための確認項目

### Q-001 今回の対策対象範囲

**Status**

解決済み

**Question**

`../past/261114-windows-real-machine-verification-backlog.md` に記載された残課題 3 件 (BL-002、BL-003、BL-004) を、本案件で一括して対策しますか。

**Context**

- **経緯と背景**: [../past/261114-windows-real-machine-verification-backlog.md](../past/261114-windows-real-machine-verification-backlog.md#L26-L32) には、前案件の作業中に摘出された `framework/makefw` の残課題 3 件が記録されています。
    1. **BL-002**: `makepart.mk` の `LIBS` 変更時に生成済み実行体が再リンクされない (優先度: 中)。
    2. **BL-003**: MSVC `/MP` バッチ コンパイルで別ソース名へ割り当てられた `.warn` が元コード修正後も残存する (優先度: 中)。
    3. **BL-004**: Windows で `certificateTest` を直接 `make` した際にシェルの引用符不一致メッセージが出力される (優先度: 低。ただし後述する [Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式) の調査により、2 回目以降のビルドで `CP_SRCS` の一部が脱落する不具合を伴うことが判明しました)。
- **判断の影響**: 3 件とも変更対象が `framework/makefw` (`makefiles/` および `bin_internal/`) に閉じており、原因箇所もすべて特定できています。一括で対策すれば、本案件の完了をもって `../past/261114-windows-real-machine-verification-backlog.md` の残課題をすべて解消でき、後続案件への転記も不要になります。

**Answer**

回答者: Tetsuo Honda (現在の Git ユーザー名。保存済み選択の実際の編集者は未記録)  
回答日時: 不明 (保存済みファイルから受領。過去の日時は推測しません)  
回答の出典: 本レビュー開始時に読み込んだ、この文書の A のチェック。

選択方式: 単一選択

- [x] A: (推奨) BL-002、BL-003、BL-004 の 3 件すべてを本案件で一括対策する。
- [ ] B: 優先度「中」の BL-002 と BL-003 のみを本案件で対策し、BL-004 は次フェーズへ見送る (本案件の backlog へ転記する)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 3 件とも `framework/makefw` 内の独立した小規模改修で解決でき、とくに BL-004 は単なる表示ノイズではなく Windows での増分ビルド時にコピー対象ソース (`CP_SRCS`) が脱落する実害を伴うため、一括で恒久対策するのが合理的です。

**Related**

- [Q-002](#q-002-bl-002-におけるリンク状態追跡の対象範囲と記録方式)
- [Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲)
- [Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式)
- [Q-005](#q-005-検証範囲と実機確認の境界)

**Assessment**

A の単一選択を確認しました。3 件の一括対策という範囲は整合しています。

**History**

2026-11-15: codex が本文の技術的前提を先にレビューし、保存済み回答を検証しました。元の質問・選択肢・チェックは維持しています。

---

### Q-002 BL-002 におけるリンク状態追跡の対象範囲と記録方式

**Status**

解決済み

**Question**

BL-002 の対策として、`$(OBJDIR)/$(notdir $(1)).link.mk` によるリンク状態の追跡を、実行体 (`makesrc_c_cpp.mk`) だけでなくライブラリ (`makelibsrc_c_cpp.mk`) も含む全リンク・アーカイブ ターゲットの実効リンク構成 (`LD`/`CC`/`AR`、`LDFLAGS`/`LIB_LTCG`、`LIBS`/`LIBSFILES`/`STATIC_LIBS`/`DYNAMIC_LIBS`) へ拡張しますか。

**Context**

- **経緯と背景**: 現状の [_link_objects.mk](../../../../../framework/makefw/makefiles/_link_objects.mk#L10-L71) は、オブジェクト ファイルやリソースの削除時に再リンクを行うため、`$(OBJDIR)/$(notdir $(1)).link.mk` に出力先パス (`_MAKEFW_LINKED_OUTPUT_<target>`) とオブジェクト入力一覧 (`_MAKEFW_LINKED_INPUTS_<target>`) を記録しています。しかし、`LIBS`、`LIBSFILES`、`STATIC_LIBS`、`DYNAMIC_LIBS`、`LDFLAGS`、`LIB_LTCG`、およびツール変数 (`LD`、`CC`、`AR`) は `.link.mk` に記録されていません。そのため、`makepart.mk` からライブラリを削除した場合や、既存の古い `.lib`/`.a` を `LIBS` に追加した場合、あるいは `LDFLAGS` を変更した場合に、Linux ではリンク レシピ自体が起動せず、Windows でもレシピ内の `rebuild` が `0` のままとなって再リンクが行われません。
- **根拠資料**: [_link_objects.mk](../../../../../framework/makefw/makefiles/_link_objects.mk#L10-L71)、[makesrc_c_cpp.mk](../../../../../framework/makefw/makefiles/makesrc_c_cpp.mk#L379-L427)、[makelibsrc_c_cpp.mk](../../../../../framework/makefw/makefiles/makelibsrc_c_cpp.mk#L316-L540)
- **現状のコード引用**:

```makefile
# framework/makefw/makefiles/_link_objects.mk (現状)
_makefw_link_state = $(OBJDIR)/$(notdir $(1)).link.mk
_makefw_link_extra_inputs = $(strip $(MAKEFW_EXTRA_OBJS) $(if $(filter $(notdir $(1)),$(MAKEFW_STATIC_TARGETS)),$(RESOURCE_OBJS),$(LINK_INPUTS)))
_makefw_link_expected = $(sort $(patsubst ./%,%,$(OBJS) $(call _makefw_link_extra_inputs,$(1))))
```

- **各案の具体例・コード比較**:
    - **案 A (推奨: 実行体・共有ライブラリ・静的ライブラリの全ターゲットで `.link.mk` に実効リンク構成を記録)**:
        - ターゲット種別ごとの実効リンク構成 (実行体は `$(LD) $(LDFLAGS) $(LIBS) $(LIBSFILES)`、共有ライブラリは `$(CCまたはLD) $(STATIC_LIBS) $(DYNAMIC_LIBS) $(LDFLAGS)`、静的ライブラリは `$(AR) $(LIB_LTCG)`) を、Make 関数で空白・引用符を安全に符号化した文字列 `_makefw_link_args` として計算し、`.link.mk` の `_MAKEFW_LINKED_ARGS_<target>` に保存します。
        - Linux の `_makefw_check_link_objects` (Make パース時、変更なし時はシェル起動 0 回) と、Linux/Windows 共通の `_MAKEFW_OBJLIST` (レシピ実行時) の双方で `_MAKEFW_LINKED_ARGS_<target>` の不一致を検出して再リンクします。

```makefile
# 案 A (推奨) のイメージ (_link_objects.mk)
_makefw_encode_link_args = $(subst ",__MAKEFW_DQUOTE__,$(subst ',__MAKEFW_SQUOTE__,$(strip $(1))))
_makefw_link_args = $(call _makefw_encode_link_args,$(if $(filter $(notdir $(1)),$(MAKEFW_STATIC_TARGETS)),$(AR) $(LIB_LTCG),$(if $(findstring /libsrc/,$(CURDIR)),$(if $(PLATFORM_LINUX),$(CC),$(LD)) $(STATIC_LIBS) $(DYNAMIC_LIBS) $(LDFLAGS),$(LD) $(LDFLAGS) $(LIBS) $(LIBSFILES))))
```

    - **案 B (実行体 `makesrc_c_cpp.mk` の `LIBS` / `LIBSFILES` のみを追跡)**:
        - `makesrc_c_cpp.mk` の実行体ターゲットでのみ `$(LIBS)` と `$(LIBSFILES)` を記録し、`makelibsrc_c_cpp.mk` (DLL / SO / 静的ライブラリ) や `LDFLAGS` の変更は対象外とします。

```makefile
# 案 B のイメージ (makesrc_c_cpp.mk のみ)
_makefw_link_extra_inputs = $(strip $(MAKEFW_EXTRA_OBJS) $(LINK_INPUTS) $(LIBSFILES) $(LIBS))
```

**Answer**

回答者: Tetsuo Honda (現在の Git ユーザー名。保存済み選択の実際の編集者は未記録)  
回答日時: 不明 (保存済みファイルから受領。過去の日時は推測しません)  
回答の出典: 本レビュー開始時に読み込んだ、この文書の A のチェック。

選択方式: 単一選択

- [x] A: (推奨) `_link_objects.mk` を拡張し、実行体 (`makesrc_c_cpp.mk`) とライブラリ (`makelibsrc_c_cpp.mk`) の全リンク・アーカイブ ターゲットで実効リンク構成 (`LD`/`CC`/`AR`、`LDFLAGS`/`LIB_LTCG`、`LIBS`/`LIBSFILES`/`STATIC_LIBS`/`DYNAMIC_LIBS`) を `.link.mk` に記録して再リンク判定に用いる。
- [ ] B: `makesrc_c_cpp.mk` の実行体ターゲットにおける `LIBS` と `LIBSFILES` の変化のみを追跡する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: `_link_objects.mk` はすでに `makesrc_c_cpp.mk` と `makelibsrc_c_cpp.mk` の双方から共通 include されており、同じ `.link.mk` ファイルに `_MAKEFW_LINKED_ARGS_<target>` を 1 行追加するだけで、追加のファイルや変更なしビルド時のシェル起動を増やさずに、実行体・共有ライブラリ・静的ライブラリすべてのリンク構成変更を取りこぼしなく検出できるためです。

**Related**

- [Q-006](#q-006-リンク構成の記録方式)
- [Q-001](#q-001-今回の対策対象範囲)
- [Q-005](#q-005-検証範囲と実機確認の境界)

**Assessment**

A の単一選択を確認しました。全ターゲットを追跡する範囲は明確ですが、記録方式に検出漏れがあるため [Q-006](#q-006-リンク構成の記録方式) で確認します。

追加検証 (版 3): 関連する追加質問の A 回答と整合し、記録方式・例外・互換性の残懸念を解消しました。

**History**

2026-11-15: codex が本文の技術的前提を先にレビューし、保存済み回答を検証しました。元の質問・選択肢・チェックは維持しています。

---

### Q-003 BL-003 における MSVC 警告の割り当て方式と既存 .warn の整理範囲

**Status**

解決済み

**Question**

BL-003 の対策として、`msvc_compile.ps1` において警告行先頭のパスと `/sourceDependencies` の JSON 依存関係を照合して正しい翻訳単位へ `.warn` を割り当てることに加え、同一ディレクトリ内の既存 `*.warn` に残っている「今回再コンパイルしたソース由来の古い警告行」も合わせて除去しますか。

**Context**

- **経緯と背景**: [msvc_compile.ps1](../../../../../framework/makefw/bin_internal/msvc_compile.ps1#L191-L230) は現在、`cl.exe` の標準出力に現れるソース名行を `$currentSource` に保持し、続く警告行を無条件に `$warnings[$currentSource]` へ追加しています。しかし `/MP` 並列コンパイル時は複数ワーカーの標準出力が交錯するため、`receiver_report.c` のソース名行の直後に別ワーカーがコンパイル中の `packet_seal.c(232)` の警告行が出力されると、`receiver_report.c.warn` に `packet_seal.c(232)` の警告が書き込まれます。その後 `packet_seal.c` を修正して増分ビルドすると、`packet_seal.c` だけが再コンパイルされて `receiver_report.c` は再コンパイルされないため、`receiver_report.c.warn` に古い警告が残り続けます。
- **根拠資料**: [msvc_compile.ps1](../../../../../framework/makefw/bin_internal/msvc_compile.ps1#L188-L342)、[_msvc_utils.ps1](../../../../../framework/makefw/bin_internal/_msvc_utils.ps1#L253-L268)
- **現状のコード引用**:

```powershell
# framework/makefw/bin_internal/msvc_compile.ps1 (191〜230 行目抜粋)
$currentSource = $null
$warnings = @{}  # source -> warning lines のハッシュテーブル

foreach ($line in $output -split "`r?`n") {
    $trimmedLine = $line.Trim()
    foreach ($src in $sourceList) {
        if ($trimmedLine -eq $src -or $trimmedLine -eq [System.IO.Path]::GetFileName($src)) {
            $currentSource = $src
            break
        }
    }
    ...
    if ($kind -eq 'warning' -and $null -ne $currentSource) {
        if (-not $warnings.ContainsKey($currentSource)) {
            $warnings[$currentSource] = @()
        }
        $warnings[$currentSource] += $outputLine
    }
}
```

- **各案の具体例・コード比較**:
    - **案 A (推奨: 診断パス・JSON 依存関係の照合による正確な割り当て ＋ 再コンパイル対象ソースに起因する既存 `*.warn` 内の陳腐化行の清掃)**:
        1. `msvc_compile.ps1` で `/sourceDependencies` の JSON (`$json.Data.Includes` および PCH 依存 JSON) を先に読み取り、各 `$src` の正規化済みの完全なパスとインクルード集合を構築します。
        2. 各警告行の先頭ファイル パスを抽出し、`$sourceList` 内のソースと一致すればその `$src` のみに割り当てます。ヘッダーや `.inject` 等のインクルード ファイルと一致する場合は、そのファイルをインクルードしている `$sourceList` 内の `$src` に割り当てます (ファイル パスを持たない `cl : Command line warning ...` 等のみ `$currentSource`、未設定時は `$sourceList[0]` にフォールバックします)。
        3. さらに、今回コンパイルした `$src` 以外の同ディレクトリ内既存 `*.warn` ファイルに、今回コンパイルした `$sourceList` のソースの完全なパスを先頭に持つ古い警告行が残っていた場合は、その行を除去し、空になった `.warn` ファイルを削除します。
    - **案 B (診断パス・JSON 依存関係の照合による割り当てのみ)**:
        - 上記 1・2 の割り当て修正のみを行い、今回コンパイル対象外の既存 `*.warn` ファイルの検査・清掃は行いません。

**Answer**

回答者: Tetsuo Honda (現在の Git ユーザー名。保存済み選択の実際の編集者は未記録)  
回答日時: 不明 (保存済みファイルから受領。過去の日時は推測しません)  
回答の出典: 本レビュー開始時に読み込んだ、この文書の A のチェック。

選択方式: 単一選択

- [x] A: (推奨) 警告行先頭のパスと `$sourceList` / `/sourceDependencies` JSON の照合によって正しい翻訳単位へ割り当てるとともに、同ディレクトリ内の既存 `*.warn` に残っていた「今回再コンパイルしたソースの古い警告行」も除去する。
- [ ] B: 警告行先頭のパスと `$sourceList` / `/sourceDependencies` JSON の照合による割り当てのみを行い、今回コンパイル対象外の既存 `*.warn` は触らない。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 割り当てロジックの修正で今後の誤割り当ては防げますが、過去のビルドで誤割り当てされた `.warn` がワークスペースに残っていた場合や、ヘッダー構成変更前後での部分再ビルド時にも、再コンパイルされたソースの古い警告行を既存 `*.warn` から除去しておくことで、`make clean` なしでも確実に整合性を回復できるためです。

**Related**

- [Q-008](#q-008-依存情報が不完全な場合の警告保存)
- [Q-007](#q-007-共有ヘッダー診断の割り当て)
- [Q-001](#q-001-今回の対策対象範囲)
- [Q-005](#q-005-検証範囲と実機確認の境界)

**Assessment**

A の単一選択を確認しました。割り当てと既存行清掃をまとめた複合質問でした。選択は保持し、共有ヘッダーと不完全な依存情報の扱いを [Q-007](#q-007-共有ヘッダー診断の割り当て)、[Q-008](#q-008-依存情報が不完全な場合の警告保存) へ分離しました。

追加検証 (版 3): 関連する追加質問の A 回答と整合し、記録方式・例外・互換性の残懸念を解消しました。

**History**

2026-11-15: codex が本文の技術的前提を先にレビューし、保存済み回答を検証しました。元の質問・選択肢・チェックは維持しています。

---

### Q-004 BL-004 の外部ソース判定におけるコマンド ライン長制限の解消方式

**Status**

解決済み

**Question**

BL-004 の原因である `_collect_srcs.mk` (および `makesrc_c_cpp.mk` / `makelibsrc_c_cpp.mk` の `clean` 処理) の「カレント ディレクトリ外のソース判定」を、`$(shell ...)` によるシェル ループから Make 組み込みのパス正規化関数 (`_makefw_path_abspath`) によるインプロセス判定へ置き換えますか。

**Context**

- **経緯と背景**: BL-004 (`app/porter/test/src/integration/certificateTest` での ``/usr/bin/sh: -c: line 1: unexpected EOF while looking for matching `"'`` 出力) の原因を調査した結果、以下のメカニズムで発生していることを特定しました。
    1. `certificateTest/makepart.mk` は `ADD_SRCS` に 46 個の製品 `.c` ファイルを指定しています。Windows では初回ビルド時にこれら 46 個が `certificateTest/` 直下へコピーされます。
    2. 2 回目以降の `make` 実行時、[_collect_srcs.mk](../../../../../framework/makefw/makefiles/_collect_srcs.mk#L45-L56) の `_DIRECT_EXISTS` は `$(wildcard ./$(notdir $(f)))` でカレント ディレクトリ内の同名ファイルの存在のみを見るため、コピー済みの 46 個すべてが一旦 `DIRECT_SRCS` に入ります。
    3. 続いて Windows 専用の [_collect_srcs.mk 101〜106 行目](../../../../../framework/makefw/makefiles/_collect_srcs.mk#L101-L106) で、`EXTERNAL_SRCS` が `$(foreach f,$(DIRECT_SRCS), real_f=$$(cd "$(dir $(f))." 2>/dev/null && pwd)/$(notdir $(f)); if [ "$$real_f" != "$$cur/$(notdir $(f))" ]; then echo "$(f)"; fi; )` を単一の `$(shell ...)` として展開します。
    4. 46 ファイル分の展開文字列は約 10,120 文字に達し、Windows 版 GNU Make が `/usr/bin/sh -c` を起動する際のコマンド ライン長上限 (約 8,191 文字) で二重引用符の途中で切り詰められます。これにより ``/usr/bin/sh: -c: line 1: unexpected EOF while looking for matching `"'`` が標準エラーに出力されます。
    5. さらに重大な副作用として、シェルの構文エラーでループが途中終了するため、後半のソース (約 10 ファイル) が `EXTERNAL_SRCS` から脱落し、2 回目以降のビルドで `CP_SRCS` に入らなくなります。その結果、製品側の元ソースを更新しても `certificateTest/` 内のコピーが更新されない不具合が生じていました。
    6. 同種の `cd ... && pwd` ループは [makesrc_c_cpp.mk 575〜583 行目](../../../../../framework/makefw/makefiles/makesrc_c_cpp.mk#L575-L583) および [makelibsrc_c_cpp.mk 679〜687 行目](../../../../../framework/makefw/makefiles/makelibsrc_c_cpp.mk#L679-L687) の `MAKEFW_CLEAN_GITIGNORE_SRCS` にも存在します。
- **根拠資料**: [_collect_srcs.mk](../../../../../framework/makefw/makefiles/_collect_srcs.mk#L42-L108)、[_path_functions.mk](../../../../../framework/makefw/makefiles/_path_functions.mk#L10-L13)、[makesrc_c_cpp.mk](../../../../../framework/makefw/makefiles/makesrc_c_cpp.mk#L574-L588)、[makelibsrc_c_cpp.mk](../../../../../framework/makefw/makefiles/makelibsrc_c_cpp.mk#L678-L692)
- **現状のコード引用**:

```makefile
# framework/makefw/makefiles/_collect_srcs.mk (45〜56 行目、94〜108 行目)
_DIRECT_CANDIDATES := $(filter-out $(CP_SRCS),$(TEST_SRCS) $(ADD_SRCS))
_DIRECT_EXISTS := $(foreach f,$(_DIRECT_CANDIDATES),$(if $(wildcard ./$(notdir $(f))),$(f)))
ifneq ($(_DIRECT_EXISTS),)
    _DIRECT_SYMLINKS := $(shell \
        $(foreach f,$(_DIRECT_EXISTS),if [ -L "./$(notdir $(f))" ]; then echo "$(f)"; fi; ))
    DIRECT_SRCS := $(filter-out $(_DIRECT_SYMLINKS),$(_DIRECT_EXISTS))
else
    DIRECT_SRCS :=
endif
...
ifdef PLATFORM_WINDOWS
    CP_SRCS += $(LINK_SRCS)
    LINK_SRCS :=
    EXTERNAL_SRCS := $(if $(strip $(DIRECT_SRCS)),$(shell \
        cur=$$(pwd); \
        $(foreach f,$(DIRECT_SRCS),\
            real_f=$$(cd "$(dir $(f))." 2>/dev/null && pwd)/$(notdir $(f)); \
            if [ "$$real_f" != "$$cur/$(notdir $(f))" ]; then echo "$(f)"; fi; )))
    CP_SRCS += $(EXTERNAL_SRCS)
    DIRECT_SRCS := $(filter-out $(EXTERNAL_SRCS),$(DIRECT_SRCS))
endif
```

- **各案の具体例・コード比較**:
    - **案 A (推奨: Make 組み込み関数 `_makefw_path_abspath` でカレント ディレクトリ内外をインプロセス判定)**:
        - GNU Make の `$(abspath ...)` (およびそれをラップした `_makefw_path_abspath`) はシンボリック リンクを解決せず字句的に `.` と `..` を正規化します。したがって、`$(call _makefw_path_abspath,$(f))` と `$(call _makefw_path_abspath,$(CURDIR)/$(notdir $(f)))` が一致するかどうかを Make 関数だけで判定すれば、シェルを一切起動せずに「`f` がカレント ディレクトリ直下のパスとして指定されたか、外部ディレクトリのパス (`../../../../prod/...` 等) として指定されたか」を判別できます。
        - `_collect_srcs.mk` では、そもそもカレント ディレクトリ直下を指す要素だけを `_DIRECT_CANDIDATES` とすることで、外部パス (`../../../../prod/...`) が `DIRECT_SRCS` へ混入すること自体を防ぎ、Windows の `EXTERNAL_SRCS` シェル呼び出しを不要にします (`_DIRECT_SYMLINKS` もカレント ディレクトリ直下を指す候補に対してのみ実行されます)。
        - `makesrc_c_cpp.mk` と `makelibsrc_c_cpp.mk` の `MAKEFW_CLEAN_GITIGNORE_SRCS` も同じ Make 関数判定に統一します。

```makefile
# 案 A (推奨) のイメージ (_collect_srcs.mk)
_makefw_is_in_curdir = $(filter $(call _makefw_encode_path,$(call _makefw_path_abspath,$(CURDIR)/$(call _makefw_path_notdir,$(1)))),$(call _makefw_encode_path,$(call _makefw_path_abspath,$(1))))

_DIRECT_CANDIDATES := $(foreach f,$(filter-out $(CP_SRCS),$(TEST_SRCS) $(ADD_SRCS)),$(if $(call _makefw_is_in_curdir,$(f)),$(f)))
_DIRECT_EXISTS := $(foreach f,$(_DIRECT_CANDIDATES),$(if $(wildcard ./$(notdir $(f))),$(f)))
```

    - **案 B (外部シェル スクリプトへの切り出し・バッチ分割)**:
        - 従来の `cd ... && pwd` によるシェル判定を維持し、`bin_internal/` に外部スクリプトを追加するか、チャンク分割して呼び出すことで 8,191 文字制限を回避します (ただし毎回のサブシェル起動コストは残ります)。

**Answer**

回答者: Tetsuo Honda (現在の Git ユーザー名。保存済み選択の実際の編集者は未記録)  
回答日時: 不明 (保存済みファイルから受領。過去の日時は推測しません)  
回答の出典: 本レビュー開始時に読み込んだ、この文書の A のチェック。

選択方式: 単一選択

- [x] A: (推奨) `_collect_srcs.mk` および `makesrc_c_cpp.mk` / `makelibsrc_c_cpp.mk` の外部ソース判定を、Make の `_makefw_path_abspath` を用いたインプロセス判定に置き換え、シェルのコマンド ライン長超過と `CP_SRCS` 脱落を解消する。
- [ ] B: 従来のシェル判定ロジックを維持し、外部スクリプト化または分割呼び出しでコマンド ライン長制限を回避する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: `prepare.mk` で `_path_functions.mk` がすでに読み込まれており、`_makefw_path_abspath` による比較で外部プロセス起動を 0 回にできるため、コマンド ライン長制限・引用符エラー・`CP_SRCS` 脱落・Windows でのプロセス起動オーバーヘッドを同時に解決できるためです。

**Related**

- [Q-010](#q-010-ソース分類に残る長いシェル展開)
- [Q-009](#q-009-外部ソース判定の互換性)
- [Q-001](#q-001-今回の対策対象範囲)
- [Q-005](#q-005-検証範囲と実機確認の境界)

**Assessment**

A の単一選択を確認しました。Linux と clean の意味が変わる可能性、および残存する長いシェル展開を [Q-009](#q-009-外部ソース判定の互換性)、[Q-010](#q-010-ソース分類に残る長いシェル展開) で確認します。

追加検証 (版 3): 関連する追加質問の A 回答と整合し、記録方式・例外・互換性の残懸念を解消しました。

**History**

2026-11-15: codex が本文の技術的前提を先にレビューし、保存済み回答を検証しました。元の質問・選択肢・チェックは維持しています。

---

### Q-005 検証範囲と実機確認の境界

**Status**

解決済み

**Question**

本案件の変更後の検証範囲として、`framework/makefw/bin_test/` のセルフ テスト群 (新規追加する回帰テストを含む) と、摘出元である `app/porter` の該当ターゲット群 (`tcpServerWindowsTest`、`certificateTest`、`prod/libsrc/porter/protocol`) の局所実機確認を実施し、全 app 横断の全体テストは省略する方針でよいですか。

**Context**

- **経緯と背景**: ルート [AGENTS.md](../../../../../AGENTS.md) の「変更後の確認」では、「振る舞い、ABI、ビルド構成を変更した場合は、影響範囲の局所テストを実行してください」「framework または複数 app にまたがる全体テストは、コストを説明してからユーザーに確認してください」と定められています。
- **コストの見積もり**:
    - **局所テスト + 摘出元ターゲットの実機確認 (案 A)**: `framework/makefw/bin_test/` の該当テスト (`link_objects_selftest.py`、`test_path_spaces.py`、新規追加する MSVC `.warn` 割り当てテスト等) と、`app/porter` の摘出元 3 箇所 (`tcpServerWindowsTest`、`certificateTest`、`prod/libsrc/porter/protocol`) の Windows / WSL 局所検証であれば、合計 5〜10 分程度で完了します。
    - **全体テスト (案 B)**: `app/porter` 全体の `make test` は Windows 実機で約 30 分、全 app 横断の `make test` は 1 時間以上を要します。

**Answer**

回答者: Tetsuo Honda (現在の Git ユーザー名。保存済み選択の実際の編集者は未記録)  
回答日時: 不明 (保存済みファイルから受領。過去の日時は推測しません)  
回答の出典: 本レビュー開始時に読み込んだ、この文書の A のチェック。

選択方式: 単一選択

- [x] A: (推奨) `framework/makefw/bin_test/` の関連セルフ テスト (新規追加する BL-002 / BL-003 / BL-004 回帰テストを含む) と、摘出元である `app/porter` の該当箇所 (`tcpServerWindowsTest`、`certificateTest`、`prod/libsrc/porter/protocol`) の局所検証を実施し、全体テストは実施しない。
- [ ] B: 案 A に加え、`app/porter` 全体の `make test` (約 30 分) も実施する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: `app/porter` 全体の基準テスト (470 件) は直前の `windows-real-machine-verification-20261114` で成功確認済みであり、今回の変更は `framework/makefw` のセルフ テストと摘出元ターゲットの再現・解消確認で十分かつ高速に回帰検証できるためです。

**Related**

- [Q-001](#q-001-今回の対策対象範囲)
- [Q-002](#q-002-bl-002-におけるリンク状態追跡の対象範囲と記録方式)
- [Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲)
- [Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式)

**Assessment**

A の単一選択を確認しました。関連セルフ テストと摘出元の局所検証を実施し、全体テストを実施しない範囲は明確です。Windows / MSVC と Linux / GCC の確認を受入条件へ含めます。所要時間 5〜10 分などは前担当の見積もりであり、計測根拠は今回未確認です。過去の 470 件成功だけを変更後の十分性の根拠とはしません。

**History**

2026-11-15: codex が本文の技術的前提を先にレビューし、保存済み回答を検証しました。元の質問・選択肢・チェックは維持しています。

### Q-006 リンク構成の記録方式

**Status**

解決済み

**Question**

BL-002 の記録方式を、引用符の置換と strip による案から、実際の引数の差異を保持する方式へ修正しますか。

**Context**

引用例の strip は引用符内の連続空白も圧縮し、例えば `"a  b"` と `"a b"` を同一視します。また、引用符と置換先文字列そのものが衝突します。保存先は Make に include されるため、`$`、`#`、バックスラッシュの安全性も必要です。CURDIR に `/libsrc/` を含むかでターゲット種別を推定する案は、明示的な種別指定との同値性が未確認です。実際のリンク・アーカイブの引数順序を保持し、成功時だけ保存し、旧形式の記録は一度再生成する方式を提案します。検索パスは実効 LDFLAGS に含まれるため、その変更も記録対象になります。

根拠: [リンク状態追跡](../../../../../framework/makefw/makefiles/_link_objects.mk)、[ソース分類](../../../../../framework/makefw/makefiles/_collect_srcs.mk)、[警告収集](../../../../../framework/makefw/bin_internal/msvc_compile.ps1)、[Q-002](#q-002-bl-002-におけるリンク状態追跡の対象範囲と記録方式)。

**Answer**

回答者: Tetsuo Honda (Git ユーザー名)  
回答受付日時: 2026-11-15 07:34:27 JST  
実際の記入日時: 不明  
回答の出典: 本対話の「記入しました」と、再読した保存済みファイルの A のチェック。

選択方式: 単一選択

- [x] A: (推奨) 記録値の衝突を避ける方式へ修正し、ターゲット種別を明示して実効引数を記録する。
- [ ] B: 元の符号化案を維持する (衝突や空白圧縮による検出漏れを許容する)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 異なるリンク構成を同一と判定すると、BL-002 の対策後も再リンクを取りこぼすためです。

**Related**

- [Q-002](#q-002-bl-002-におけるリンク状態追跡の対象範囲と記録方式)

**Assessment**

A の単一選択を確認しました。引数の順序・引用符内の空白を保持し、記録値の衝突を避け、ターゲット種別を明示する方式で確定しました。旧形式は再生成し、成功時のみ保存します。関連する過去回答と矛盾はなく、残懸念はありません。

**History**

2026-11-15 07:34:27 JST: codex が回答を受領し、過去回答と参照実装を横断して再検証しました。

---

### Q-007 共有ヘッダー診断の割り当て

**Status**

解決済み

**Question**

同じヘッダーを複数の翻訳単位が参照していて診断の発生元を一意に特定できない場合、警告を依存する今回の全翻訳単位へ記録しますか。

**Context**

JSON の Includes は参照関係を表し、その警告が各翻訳単位で実際に発生したことまでは示しません。例えば a.c と b.c が共通の h.h を参照しても、マクロ条件によって警告は a.c だけで発生する場合があります。元の案ではこの区別がなく、「正確な割り当て」と断定しています。A は一部重複を許容して診断の保存を優先します。B は曖昧な場合に翻訳単位を個別に再コンパイルするため、追加コストと副作用の検討が必要です。

根拠: [リンク状態追跡](../../../../../framework/makefw/makefiles/_link_objects.mk)、[ソース分類](../../../../../framework/makefw/makefiles/_collect_srcs.mk)、[警告収集](../../../../../framework/makefw/bin_internal/msvc_compile.ps1)、[Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲)。

**Answer**

回答者: Tetsuo Honda (Git ユーザー名)  
回答受付日時: 2026-11-15 07:34:27 JST  
実際の記入日時: 不明  
回答の出典: 本対話の「記入しました」と、再読した保存済みファイルの A のチェック。

選択方式: 単一選択

- [x] A: (推奨) 依存する今回の全翻訳単位へ記録し、重複する可能性を仕様として明記する。
- [ ] B: 曖昧な場合は個別再コンパイルで発生元を特定する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 現在の /MP を維持しつつ、診断の欠落を防ぐ方針を明確にするためです。

**Related**

- [Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲)

**Assessment**

A の単一選択を確認しました。共有ヘッダーは今回の依存する全翻訳単位へ保存し、重複を許容する仕様で確定しました。関連する過去回答と矛盾はなく、残懸念はありません。

**History**

2026-11-15 07:34:27 JST: codex が回答を受領し、過去回答と参照実装を横断して再検証しました。

---

### Q-008 依存情報が不完全な場合の警告保存

**Status**

解決済み

**Question**

依存 JSON の欠落・解析失敗や照合不能の診断がある場合は、警告を今回の全翻訳単位へ保守的に記録しますか。

**Context**

コンパイル失敗時などに依存情報が取得できない場合の動作は未定義です。ファイル名行の直前出力へ戻す方式は /MP の交錯により再び誤割り当てします。パスのないコマンド ライン警告も発生元を特定できません。A でもコンパイラの失敗終了コードは維持します。既存 warn の清掃対象は、[Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲) の回答どおり今回コンパイルしたソースのパスに一致する診断行に限定し、共有ヘッダー由来や照合不能の既存行を一律に削除しません。

根拠: [リンク状態追跡](../../../../../framework/makefw/makefiles/_link_objects.mk)、[ソース分類](../../../../../framework/makefw/makefiles/_collect_srcs.mk)、[警告収集](../../../../../framework/makefw/bin_internal/msvc_compile.ps1)、[Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲)。

**Answer**

回答者: Tetsuo Honda (Git ユーザー名)  
回答受付日時: 2026-11-15 07:34:27 JST  
実際の記入日時: 不明  
回答の出典: 本対話の「記入しました」と、再読した保存済みファイルの A のチェック。

選択方式: 単一選択

- [x] A: (推奨) 照合不能の警告は今回の全翻訳単位へ記録し、警告の欠落を防ぐ。
- [ ] B: 依存情報や診断を照合できない場合は、ラッパーをエラー終了させる。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 不完全な情報から発生元を断定せず、診断を保存するためです。

**Related**

- [Q-003](#q-003-bl-003-における-msvc-警告の割り当て方式と既存-warn-の整理範囲)

**Assessment**

A の単一選択を確認しました。不完全な依存情報や照合不能の警告は今回の全翻訳単位へ保存します。既存行清掃は今回のソースに一致する診断行だけに限定し、コンパイラ終了コードを維持します。関連する過去回答と矛盾はなく、残懸念はありません。

**History**

2026-11-15 07:34:27 JST: codex が回答を受領し、過去回答と参照実装を横断して再検証しました。

---

### Q-009 外部ソース判定の互換性

**Status**

解決済み

**Question**

外部ソース判定の Make 関数化は、既存のローカル実体ファイル優先と clean の除外条件を維持する方針にしますか。

**Context**

現行 _collect_srcs.mk は同名のローカル実体ファイルがあれば DIRECT_SRCS とします。候補を字句パスで事前に絞る案は Linux でもその動作を変えます。また clean の現行処理はディレクトリへ cd できない入力を除外しますが、abspath は存在確認をしません。シンボリック リンク、Windows の大文字小文字・短い名前なども字句比較だけで同一性を判断できるとは限りません。A は外部ソースを Windows のコピー対象から取りこぼさず、これらの既存動作を維持する共通判定を検討します。B は配置や clean の対象が変わることを受け入れます。

根拠: [リンク状態追跡](../../../../../framework/makefw/makefiles/_link_objects.mk)、[ソース分類](../../../../../framework/makefw/makefiles/_collect_srcs.mk)、[警告収集](../../../../../framework/makefw/bin_internal/msvc_compile.ps1)、[Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式)。

**Answer**

回答者: Tetsuo Honda (Git ユーザー名)  
回答受付日時: 2026-11-15 07:34:27 JST  
実際の記入日時: 不明  
回答の出典: 本対話の「記入しました」と、再読した保存済みファイルの A のチェック。

選択方式: 単一選択

- [x] A: (推奨) 既存のローカル実体ファイル優先と clean の除外条件を維持し、単純な字句比較で同値性を保証できない入力を区別する。
- [ ] B: 既存案の字句パスによる分類へ統一し、Linux の分類や clean の対象が変わることを許容する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: Windows の障害修正に伴って Linux のソース選択や削除対象を意図せず変更しないためです。

**Related**

- [Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式)

**Assessment**

A の単一選択を確認しました。Linux の同名ローカル実体優先と clean の存在・到達可能性の条件を維持します。Windows の外部ソースはコピー対象として追跡し、字句比較だけで同値性を保証できない入力を区別します。関連する過去回答と矛盾はなく、残懸念はありません。

**History**

2026-11-15 07:34:27 JST: codex が回答を受領し、過去回答と参照実装を横断して再検証しました。

---

### Q-010 ソース分類に残る長いシェル展開

**Status**

解決済み

**Question**

BL-004 の対策には、残存する _DIRECT_SYMLINKS の単一シェル展開も、入力数に比例してコマンド ラインが長大化しない方式へ変更することを含めますか。

**Context**

元の案は EXTERNAL_SRCS の展開を除去しますが、_DIRECT_SYMLINKS の foreach 展開は残ります。候補数が多い場合の問題を同時に防ぐかは未合意です。元文書にある約 10,120 文字・8,191 文字での切り詰め・約 10 ファイル脱落という説明は前担当の調査記録であり、今回のレビューでは再現ログや計測条件を確認できていません。したがって確定した原因の数値として design へ転記しません。実装時には一時検証環境でソース数を増やし、初回・反復ビルド・元ソース更新・clean について、分類結果と標準エラーを確認する受入条件を設けます。

根拠: [リンク状態追跡](../../../../../framework/makefw/makefiles/_link_objects.mk)、[ソース分類](../../../../../framework/makefw/makefiles/_collect_srcs.mk)、[警告収集](../../../../../framework/makefw/bin_internal/msvc_compile.ps1)、[Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式)。

**Answer**

回答者: Tetsuo Honda (Git ユーザー名)  
回答受付日時: 2026-11-15 07:34:27 JST  
実際の記入日時: 不明  
回答の出典: 本対話の「記入しました」と、再読した保存済みファイルの A のチェック。

選択方式: 単一選択

- [x] A: (推奨) _DIRECT_SYMLINKS も長大化しない方式へ変更する対象に含める。
- [ ] B: EXTERNAL_SRCS と clean の外部判定に限定し、_DIRECT_SYMLINKS は別件として backlog に記録する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 同じ分類処理に残るコマンド ライン長依存を除き、対策を別の入力構成でも有効にするためです。

**Related**

- [Q-004](#q-004-bl-004-の外部ソース判定におけるコマンド-ライン長制限の解消方式)

**Assessment**

A の単一選択を確認しました。_DIRECT_SYMLINKS も対策に含め、ソース一覧を単一の長いコマンドへ展開しない条件と、多数入力・反復・更新・clean の検証を採用します。関連する過去回答と矛盾はなく、残懸念はありません。

**History**

2026-11-15 07:34:27 JST: codex が回答を受領し、過去回答と参照実装を横断して再検証しました。

---

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261114-makefw-windows-backlog-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 3 (全 10 問を再検証済み)

最終仕様 (`design.md`) の合意版 (第 2 ゲート): 仕様版 1。本対話の「承認」を受領。

更新予定の成果物: 上記の対象成果物。全 10 問の保存済み回答に基づき確定しました。

受入条件: 3 件の回帰テスト、Windows / MSVC と Linux / GCC の局所検証、変更なし時の不要な再リンクがないこと、内容のある `.warn` の確認。詳細は最終仕様の受入条件に記載します。検証範囲は [Q-005](#q-005-検証範囲と実機確認の境界) の保存済み回答に基づきます。

## 反映と検証の記録

最終仕様の仕様版 1 を作成しました。全 10 問の回答と受入条件の整合性を確認済みです。  
第 2 ゲートの承認を受領し、リンク構成記録、MSVC 警告の割り当て・清掃、ソース分類の修正を実装しました。  
局所テスト中に検出した Windows の both 構成の起動条件と、長い構成をシェル コマンドへ埋め込む問題は修正・再検証済みです。  
同一ファイルの更新と検証が重なった際の Make の一時的な解析失敗は、コード更新後の安定した状態で再検証しました。  
Windows の摘出元は tcpServerWindowsTest 1 件、certificateTest 31 件、protocol のビルドが成功しました。  
Linux の共通回帰テストと protocol のビルドが成功しました。  
依存生成物を現在の構成で再生成し、現行ライブラリを優先する条件で certificateTest を生成しました。  
共有ディスク上のレポート生成を中断し、同一バイナリと共有ライブラリの複製を SHA-256 で確認したうえで、Linux の一時領域から 31 件を個別プロセスで実行し、全件成功しました。  
ビルドした cjson・openssl・zlib・cplat・porter の内容がある `.warn` は 0 件です。  
元の BL-002〜BL-004 は対応済みとし、過去の記載を保持して対処を追記しました。  
本案件の残課題は、Linux の旧 OpenSSL mock が通常の探索順で優先される既存環境の BL-001 です。  
実装と局所検証を完了しました。

第 3 ゲートの成果物承認: 2026-11-15 13:38:55 JST に、本対話のユーザー発言「承認」を受領しました。  
全 WBS を完了とし、本案件の合意フローを終了しました。
