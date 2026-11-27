<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新エージェント: claude code / セッション ID: session_018UJwBxipLvskQMgge7hiBW
上流文書: `261024-test-assert-comment-followup4-backlog.md` (前案件の残課題一覧)。削除されている場合があります。
下流文書: `261024-test-assert-comment-followup5-design.md` (最終仕様)、`261024-test-assert-comment-followup5-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# ポインターの条件の点検、makefw の省略判定、自己テストの修復の検討

ファイル名: `prompt/current/test-assert-comment-followup5-discussion.md`

## 検討の識別

検討 ID: test-assert-comment-followup5

版: 4

状態: 合意済み

## 作業 WBS と進捗管理

この検討用 Markdown が管理する作業 WBS です。Q&A のターンごとに必要に応じて見直します。フェーズ 3 の項目は、[Q-001](#q-001-本案件で対処する残課題) の回答で対象外になったものを削除し、具体的な対象ファイルを追記します。見送りの決定や範囲外の気付きは、どの段階でも生じた時点で `test-assert-comment-followup5-backlog.md` に追記します。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 前提条件と要求事項の調査 (`prompt/current/test-assert-comment-followup4-backlog.md` の BL-001〜BL-003、CI コンテナーの LLVM の導入、PyPI の libclang、前回の一時点検のスクリプト、makefw のコンパイル規則とスクリプトの用途、`app_paths_selftest.py` の修正の試行、`run-bin-tests.py` の実行規則、CI の警告の扱い)
    - ✅ 1.2. 課題・論点の抽出と明確化のための確認項目 ([Q-001](#q-001-本案件で対処する残課題)〜[Q-017](#q-017-前案件の文書の扱い)) の作成
    - ✅ 1.3. Q&A ターンごとのユーザー回答の受付と記録 (JST 日時、Git ユーザー名)
    - ✅ 1.4. 回答に基づく前提・制約の検証と WBS の見直し (BL-001 の項目を削除)
    - ✅ 1.5. 追加の確認項目 ([Q-018](#q-018-テスト-ディレクトリ単位の省略判定への-makefw-の変更の反映)) の回答の受付と記録
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定 (懸念がないことをもってフェーズ 2 へ着手)
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `prompt/current/test-assert-comment-followup5-design.md` の作成
    - ✅ 3.2. 最終仕様における受入条件と整合性の確認
    - ✅ 3.3. 対話セッションにおける最終仕様 (design.md) の承認の受付 (第 2 ゲート: 作業着手の合意)
- ✅ 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. BL-003: 自己テストの修復 (`framework/makefw/bin_internal/app_paths_selftest.py` の `framework/makefw/bin_test/app_paths_selftest.py` への移動と修正、`framework/makefw/docs/build-configurations.md` の案内)
    - ✅ 4.2. BL-002: 署名への makefw の入力の追加 (新設の `framework/makefw/bin_internal/list_signature_inputs.sh`、`framework/makefw/bin_internal/resolve_app_deps.sh`、`framework/testfw/bin_internal/exec_test_c_cpp.sh`、`framework/testfw/bin_internal/list_signature_scripts.sh`、各 `bin_test/` のテスト、`framework/makefw/docs/build-configurations.md`、`framework/testfw/docs/how-to-test.md`)
    - ✅ 4.3. 整形: `framework/testfw/bin_internal/exec_test_c_cpp.sh` のコメントの整形の指摘 2 件
    - ✅ 4.4. 正本ドキュメントおよびコード コメントへの永続的知見の反映
    - ✅ 4.5. 局所テストおよび機械的検証 ([Q-015](#q-015-受入時のテストの範囲)、`.warn` の確認、変更した Markdown への `text_style_jp.py`)
    - ✅ 4.6. 受入条件への適合確認 (全完確認)
    - ✅ 4.7. 残課題一覧 (`prompt/current/test-assert-comment-followup5-backlog.md`) の再構築と整理 (解消・容認した項目の除外、起票できる詳細度への書き直し)
    - ✅ 4.8. コミットとプッシュ、CI の追跡 ([Q-016](#q-016-コミットとプッシュの扱い))
- ✅ 5. 成果物の承認の受付 (第 3 ゲート)
    - ✅ 5.1. 成果物・検証結果・残課題の提示
    - ✅ 5.2. 対話セッションにおける成果物の承認の受付 (「承認」または「完了」の受領によるスキル完了)
    - ✅ 5.3. 前案件の文書の `prompt/past/` への移動 ([Q-017](#q-017-前案件の文書の扱い))

## 対象と参照資料

対象成果物: `prompt/current/test-assert-comment-followup4-backlog.md` の BL-001〜BL-003 のうち、[Q-001](#q-001-本案件で対処する残課題) で選ばれた残課題の対処

今回の対象範囲: BL-002、BL-003、`exec_test_c_cpp.sh` のコメントの整形の指摘 2 件 ([Q-001](#q-001-本案件で対処する残課題))。BL-001 は対象外です。

書き込み可能な検討用 Markdown: この文書 (`prompt/current/test-assert-comment-followup5-discussion.md`)

参照資料と版: 2026-10-24 時点の作業ツリー (リポジトリ本体 `3b1dabc9`)

## 調査結果

### BL-001 ポインターの条件の常設の点検

- コーディング規範 (`app/general/docs/coding-guideline.md`) は、ポインターとスマート ポインターを `if (p)` や `if (!p)` のようにそのまま条件にせず、`NULL` や `nullptr` と比較することを求めています。
- 前回の一時点検は、libclang (clang の構文解析を Python から使うライブラリ) で書いた 37 行のスクリプトでした。`if`、`while`、`do`、`for`、`?:`、`!`、`&&`、`||` の被演算子の型を調べ、ポインター型と `std::unique_ptr` / `std::shared_ptr` を報告します。
- libclang は、PyPI のホイール (`pip install libclang`、最新 18.1.1) で、Linux と Windows のどちらにも導入できます。CI コンテナーの変更は不要です。
- clang-tidy の `readability-implicit-bool-conversion` は、生のポインターの暗黙の変換を報告します。スマート ポインターの `if (p)` は `explicit operator bool` の呼び出しになるため、この検査では報告されません。整数や浮動小数点の変換も報告するため、規範にない指摘を除く絞り込みが必要です。clang-tidy は CI コンテナーにありません。コンテナーは LLVM 23.1.1 のソースから clang-format だけをビルドしています。
- 正しい解析には、ビルドと同じインクルード パスとマクロ定義が必要です。前回は、ビルド時だけに作られるシンボリック リンクやマクロに依存する 12 ファイルで解析エラーが出ました。makefw には、コンパイル オプションをファイルごとに書き出す仕組み (`compile_commands.json`) がありません。Linux のコンパイル規則は `makesrc_c_cpp.mk` の `compile_rule_template`、Windows は `_msvc_compile.mk` と `msvc_compile.ps1` です。
- Windows だけでコンパイルされるコード (`PLATFORM_WINDOWS` の分岐、`*_windows.c`) は、Windows のヘッダーが必要なため、Linux では解析できません。
- CI の警告の扱い: 各ジョブの `.warn` を成果物として集め、`warnings-summary` ジョブが `core.warning` で警告として表示します。CI は失敗にしません。CI には GitHub Actions のほかに `.jenkins/build.sh` があります。

### BL-002 makefw の変更の省略判定への反映

- app 直下の署名は、`framework/makefw` を含みません (前案件で testfw は追加済み)。
- `framework/makefw/makefiles/` には、テンプレート (`__template.mk` など) と、テンプレートが読み込む make ファイル (`makesrc_c_cpp.mk`、`_flags.mk` など)、Windows のリンクに使う `utf8_manifest.manifest` があります。app のテンプレートの複製 (`app/<name>/makefile` など) は、すでに app 側の入力として署名に含まれています。
- `framework/makefw/bin_internal/` の 20 個のスクリプトは、ビルドとテストの手順で使うもの (`resolve_app_deps.sh`、`capture_warnings.sh`、`msvc_compile.ps1` など)、Doxygen と Coverity で使うもの (`doxy_signature.py`、`cov-build-app.sh`)、および自己テスト (`app_paths_selftest.py`、BL-003) です。`framework/makefw/bin/` は開発環境の補助 (`sync_c_cpp_properties.sh`、`update_template_makefiles.py`) です。

### BL-003 依存パス解決の自己テストの修復

- 一時フォルダーに複製した自己テストに、`makefiles/` 全体を一時ワークスペースへコピーする 1 行を加えると、Linux で 2 つの確認がすべて合格しました。
- `make bin-test` (`app/general/bin_internal/run-bin-tests.py`) は、`bin_test/` の `test_*.py` を unittest で、`*_selftest.py` を直接実行します。`*_selftest.py` は `BIN_TEST_PLATFORM = "linux"` などで実行する OS を限定できます。
- 自己テストには、Windows だけで実行する `cygpath` の起動回数の確認があります。`bash` を名前で起動しているため、Windows では WSL の `bash.exe` を選ぶおそれがあります (他のテストは PATH 上の `bash` を明示しています)。

### その他

- `framework/testfw/bin_internal/exec_test_c_cpp.sh` の 188 行目 (`compute_test_signature` の説明のコメント) に、`text_style_jp.py` の指摘が 2 件あります (`1つ` → `1 つ`)。

## 現行の合意内容

ユーザーの依頼は「残課題消化作業を着手」です (2026-10-24、対話セッション)。版 2 で次の内容に合意しました。版 4 で、BL-002 の範囲に、テスト ディレクトリ単位の省略判定を加えることに合意しました ([Q-018](#q-018-テスト-ディレクトリ単位の省略判定への-makefw-の変更の反映))。

- **範囲:** BL-002、BL-003、`exec_test_c_cpp.sh` のコメントの整形の指摘 2 件を対処します。BL-001 は見送り、本案件の残課題一覧に引き継ぎます ([Q-001](#q-001-本案件で対処する残課題)、[Q-007](#q-007-ポインターの条件の検出方式)〜[Q-009](#q-009-点検へのコンパイル-オプションの渡し方))。
- **BL-002:** `framework/makefw/makefiles/` 直下のすべてのファイルと、`framework/makefw/bin_internal/` 直下の `*.sh`、`*.py`、`*.ps1` を、すべての app の `build` と `test` の署名と、テスト ディレクトリ単位の省略判定 (`test.stamp`) に加えます。範囲の定義は makefw の 1 つのスクリプトに置きます ([Q-002](#q-002-署名に加える-makefw-の入力の範囲)、[Q-003](#q-003-makefw-の入力を加える-app)、[Q-018](#q-018-テスト-ディレクトリ単位の省略判定への-makefw-の変更の反映))。
- **BL-003:** 自己テストを `framework/makefw/bin_test/app_paths_selftest.py` へ形式を変えずに移し、一時ワークスペースへ `makefiles/` 全体をコピーします。Linux と Windows の両方で実行し、PATH 上の `bash` を明示して起動します ([Q-004](#q-004-自己テストの移し方)〜[Q-006](#q-006-自己テストを実行する-os))。
- **検証:** 局所的な確認と CI だけにします ([Q-015](#q-015-受入時のテストの範囲))。
- **Git:** 受入条件の確認後に、サブモジュール、リポジトリ本体の順にコミットとプッシュを行い、CI を追跡します ([Q-016](#q-016-コミットとプッシュの扱い))。
- **前案件の文書:** 第 3 ゲートの承認後に `prompt/past/` へ移動します ([Q-017](#q-017-前案件の文書の扱い))。

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | 前案件の残課題一覧の対処。利用者は、C/C++ のコードを書く開発者、makefw を保守する開発者、開発環境で `make` を繰り返す開発者 |
| 範囲・境界 | 確認済み | [Q-001](#q-001-本案件で対処する残課題)、[Q-002](#q-002-署名に加える-makefw-の入力の範囲)、[Q-003](#q-003-makefw-の入力を加える-app)、[Q-012](#q-012-点検の対象のコード)、[Q-018](#q-018-テスト-ディレクトリ単位の省略判定への-makefw-の変更の反映) |
| 入力・根拠 | 確認済み | [調査結果](#調査結果) |
| 整合性・依存関係 | 確認済み | [Q-007](#q-007-ポインターの条件の検出方式)、[Q-009](#q-009-点検へのコンパイル-オプションの渡し方)、[Q-010](#q-010-点検スクリプトの置き場所) |
| 実現性・例外 | 確認済み | [Q-006](#q-006-自己テストを実行する-os)、[Q-008](#q-008-点検ツールの入手方法)、[Q-011](#q-011-点検を実行する-os) |
| 受入条件・検証 | 確認済み | [Q-013](#q-013-点検の指摘の扱い)、[Q-014](#q-014-ci-で点検を実行する-linux-の構成)、[Q-015](#q-015-受入時のテストの範囲)、[Q-016](#q-016-コミットとプッシュの扱い)、[Q-017](#q-017-前案件の文書の扱い) |

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-本案件で対処する残課題) | 本案件で対処する残課題 | 解決済み | なし |
| [Q-002](#q-002-署名に加える-makefw-の入力の範囲) | 署名に加える makefw の入力の範囲 | 解決済み | BL-002 |
| [Q-003](#q-003-makefw-の入力を加える-app) | makefw の入力を加える app | 解決済み | BL-002 |
| [Q-004](#q-004-自己テストの移し方) | 自己テストの移し方 | 解決済み | BL-003 |
| [Q-005](#q-005-自己テストの一時ワークスペースへのコピー) | 自己テストの一時ワークスペースへのコピー | 解決済み | BL-003 |
| [Q-006](#q-006-自己テストを実行する-os) | 自己テストを実行する OS | 解決済み | BL-003 |
| [Q-007](#q-007-ポインターの条件の検出方式) | ポインターの条件の検出方式 | 解決済み | BL-001 |
| [Q-008](#q-008-点検ツールの入手方法) | 点検ツールの入手方法 | 解決済み | BL-001、[Q-007](#q-007-ポインターの条件の検出方式) |
| [Q-009](#q-009-点検へのコンパイル-オプションの渡し方) | 点検へのコンパイル オプションの渡し方 | 解決済み | BL-001 |
| [Q-010](#q-010-点検スクリプトの置き場所) | 点検スクリプトの置き場所 | 解決済み | BL-001 |
| [Q-011](#q-011-点検を実行する-os) | 点検を実行する OS | 解決済み | BL-001 |
| [Q-012](#q-012-点検の対象のコード) | 点検の対象のコード | 解決済み | BL-001 |
| [Q-013](#q-013-点検の指摘の扱い) | 点検の指摘の扱い | 解決済み | BL-001 |
| [Q-014](#q-014-ci-で点検を実行する-linux-の構成) | CI で点検を実行する Linux の構成 | 解決済み | BL-001、[Q-011](#q-011-点検を実行する-os) |
| [Q-015](#q-015-受入時のテストの範囲) | 受入時のテストの範囲 | 解決済み | [Q-001](#q-001-本案件で対処する残課題) |
| [Q-016](#q-016-コミットとプッシュの扱い) | コミットとプッシュの扱い | 解決済み | なし |
| [Q-017](#q-017-前案件の文書の扱い) | 前案件の文書の扱い | 解決済み | なし |
| [Q-018](#q-018-テスト-ディレクトリ単位の省略判定への-makefw-の変更の反映) | テスト ディレクトリ単位の省略判定への makefw の変更の反映 | 解決済み | BL-002、[Q-002](#q-002-署名に加える-makefw-の入力の範囲)、[Q-003](#q-003-makefw-の入力を加える-app) |

## 明確化のための確認項目

BL-001 を選ばない場合、[Q-007](#q-007-ポインターの条件の検出方式)〜[Q-014](#q-014-ci-で点検を実行する-linux-の構成) の回答は不要です。同様に、BL-002 を選ばない場合は [Q-002](#q-002-署名に加える-makefw-の入力の範囲) と [Q-003](#q-003-makefw-の入力を加える-app)、BL-003 を選ばない場合は [Q-004](#q-004-自己テストの移し方)〜[Q-006](#q-006-自己テストを実行する-os) の回答が不要です。

### Q-001 本案件で対処する残課題

**Status**

解決済み

**Question**

本案件で対処する残課題はどれですか。

**Context**

- BL-001 (ポインターの条件の常設の点検) は、2 回見送られてきました。見送りの主な理由は CI コンテナー (別リポジトリ) の変更でした。[調査結果](#bl-001-ポインターの条件の常設の点検) のとおり、libclang のホイールを使えばコンテナーの変更は不要です。ただし、makefw への点検の組み込み、Windows での実行、CI の手順の追加 (GitHub Actions と Jenkins) が必要で、作業量は 3 件の中で最も大きくなります。
- BL-002 (makefw の変更の省略判定への反映) は、前案件で testfw に行った変更と同じ方式で実装できます。
- BL-003 (自己テストの修復) は、修正の見通しを一時フォルダーで確認済みです。BL-002 の `bin_internal/` の範囲と関係します (自己テストを `bin_internal/` から移すと、`bin_internal/` はビルドとテストの手順のスクリプトだけになります)。
- D は残課題一覧にない、前案件で気付いた整形の指摘 2 件です ([調査結果](#その他))。

**Answer**

選択方式: 複数選択

- [ ] A: (推奨) BL-001 ポインターの条件の常設の点検
- [x] B: (推奨) BL-002 makefw の変更の省略判定への反映
- [x] C: (推奨) BL-003 依存パス解決の自己テストの修復
- [x] D: (推奨) `exec_test_c_cpp.sh` のコメントの整形の指摘 2 件
- [ ] E: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A、B、C、D  
理由: 依頼が残課題の消化であり、BL-001 の主な見送り理由 (コンテナーの変更) は libclang のホイールで解消できるためです。選ばなかった残課題は、本案件の残課題一覧に引き継ぎます。

**Assessment**

BL-002、BL-003、および `exec_test_c_cpp.sh` の整形の指摘 2 件を対処します。BL-001 は対象外とし、本案件の残課題一覧に引き継ぎます。BL-001 の質問 ([Q-007](#q-007-ポインターの条件の検出方式)〜[Q-014](#q-014-ci-で点検を実行する-linux-の構成)) は、本案件では適用しません。

### Q-002 署名に加える makefw の入力の範囲

**Status**

解決済み

**Question**

app 直下の署名に、makefw のどのファイルを加えますか。

**Context**

- [調査結果](#bl-002-makefw-の変更の省略判定への反映) のとおり、`bin_internal/` には Doxygen と Coverity でだけ使うスクリプトもあります。
- 案ごとの対象は次のとおりです。

```text
A の場合: makefiles/ 直下のすべてのファイル + bin_internal/ 直下のすべてのスクリプト (*.sh、*.py、*.ps1)
  一覧は持たず、ディレクトリで決める (前案件の testfw と同じ考え方)。
  Doxygen と Coverity のスクリプトを変えた場合も、ビルドとテストを再実行する。
B の場合: makefiles/ 直下のすべてのファイルだけ
  スクリプト (resolve_app_deps.sh、msvc_compile.ps1 など) の変更は反映しない。
C の場合: makefiles/ 直下のすべてのファイル + ビルドとテストに使うスクリプトの列挙
  doxy_signature.py と cov-build-app.sh を除く。一覧の保守が必要になる。
```

- `framework/makefw/bin/` (開発環境の補助) は、どの案でも対象にしません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `makefiles/` 直下のすべてのファイルと、`bin_internal/` 直下のすべてのスクリプト
- [ ] B: `makefiles/` 直下のすべてのファイルだけ
- [ ] C: `makefiles/` 直下のすべてのファイルと、ビルドとテストに使うスクリプトの列挙
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 一覧の保守が不要で、更新漏れで古い成果物を残すことがありません。Doxygen と Coverity のスクリプトの変更は少なく、再実行の負担は小さいためです。前案件の testfw の範囲の決め方とも一致します。

**Related**

[Q-003](#q-003-makefw-の入力を加える-app)、[Q-004](#q-004-自己テストの移し方)、[Q-018](#q-018-テスト-ディレクトリ単位の省略判定への-makefw-の変更の反映)

**Assessment**

`framework/makefw/makefiles/` 直下のすべてのファイルと、`framework/makefw/bin_internal/` 直下の `*.sh`、`*.py`、`*.ps1` を署名に加えます。`__pycache__/` はディレクトリのため対象になりません。署名の計算スクリプト (`resolve_app_deps.sh`) 自体も含まれます。testfw の入力として加わる `get_files_lang.sh` と重複しますが、署名は項目を `sort -u` で重複除去するため、影響はありません。

### Q-003 makefw の入力を加える app

**Status**

解決済み

**Question**

makefw の入力を、どの app の署名に加えますか。

**Context**

- 前案件の testfw の入力は、テストのビルドと結果の入力であるため、`test/` を持つ app だけに加えました。
- makefw は、製品のビルドにも使います。`test/` を持たない app も、makefw の変更でビルドの結果が変わります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) すべての app の `build` と `test` の署名に加える。
- [ ] B: `test/` を持つ app だけに加える。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: makefw は、テストの有無にかかわらずすべての app のビルドの入力であるためです。

**Related**

[Q-002](#q-002-署名に加える-makefw-の入力の範囲)、[Q-018](#q-018-テスト-ディレクトリ単位の省略判定への-makefw-の変更の反映)

**Assessment**

makefw の入力は、`test/` の有無にかかわらず、すべての app の `build` と `test` の署名に加えます。

### Q-004 自己テストの移し方

**Status**

解決済み

**Question**

`app_paths_selftest.py` を、どの形で `bin_test/` に移しますか。

**Context**

- 現在のテストは、関数を順に呼び、失敗すると `AssertionError` で終了する形式です。`run-bin-tests.py` は、`*_selftest.py` をこの形式のまま実行できます。
- unittest の形式 (`test_*.py`) に書き換えると、失敗した確認の名前が表示され、他のテストと同じ形式になります。書き換えの分、差分が大きくなります。

```text
A の場合: framework/makefw/bin_test/app_paths_selftest.py (内容は修正だけ)
B の場合: framework/makefw/bin_test/test_app_paths.py (unittest の形式に書き換え)
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `bin_test/app_paths_selftest.py` に移し、形式は変えない。
- [ ] B: `bin_test/test_app_paths.py` に、unittest の形式で書き換える。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: `run-bin-tests.py` の既存の規則で実行でき、`bin_test/` には同じ形式の `link_objects_selftest.py` などがあるためです。差分が小さく、修正の内容が明確になります。

**Related**

[Q-005](#q-005-自己テストの一時ワークスペースへのコピー)

**Assessment**

`framework/makefw/bin_test/app_paths_selftest.py` へ移し、関数を順に呼ぶ形式を維持します。[Q-002](#q-002-署名に加える-makefw-の入力の範囲) の A により、移動後の自己テストは署名の対象外になります。

### Q-005 自己テストの一時ワークスペースへのコピー

**Status**

解決済み

**Question**

自己テストの一時ワークスペースに、makefw の make ファイルをどうコピーしますか。

**Context**

- 失敗の原因は、テンプレートが読み込む `makefiles/_path_functions.mk` などを一時ワークスペースにコピーしていないことです。
- 案ごとの違いは次のとおりです。

```text
A の場合: makefiles/ 全体をコピーする (bin_test/test_path_spaces.py と同じ方式)
  テンプレートが新しい make ファイルを読み込んでも追随する。一時フォルダーでの試行で合格を確認済み。
B の場合: テンプレートが読み込むファイルだけを列挙してコピーする
  コピーの量は少ないが、読み込むファイルを追加するたびにテストの修正が必要になる。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `makefiles/` 全体をコピーする。
- [ ] B: 読み込むファイルだけを列挙してコピーする。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 今回の失敗と同じ原因 (読み込むファイルの追加) で再び壊れることがないためです。

**Related**

[Q-004](#q-004-自己テストの移し方)

**Assessment**

一時ワークスペースへ `makefiles/` 全体をコピーします。一時フォルダーでの試行で、Linux の合格を確認済みです。

### Q-006 自己テストを実行する OS

**Status**

解決済み

**Question**

移した自己テストを、どの OS で実行しますか。

**Context**

- 自己テストには、Windows だけで実行する確認 (`cygpath` の起動回数、変換の失敗、出力件数の不一致) があります。
- Windows でこのテストが合格するかは未確認です。`bash` を名前で起動しているため、Windows では WSL の `bash.exe` を選ぶおそれがあります。両方で実行する場合は、他のテストと同じく PATH 上の `bash` を明示する修正を加え、Windows の CI で失敗した場合は原因を調べて直します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) Linux と Windows の両方で実行する (`BIN_TEST_PLATFORM` を宣言しない)。
- [ ] B: Linux だけで実行する (`BIN_TEST_PLATFORM = "linux"`)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: Windows のパスの一括変換は、このテストでしか確認されていないためです。Linux だけにすると、Windows の確認のコードが実行されないまま残ります。

**Assessment**

`BIN_TEST_PLATFORM` を宣言せず、Linux と Windows の両方で実行します。PATH 上の `bash` を明示して起動する修正を加え、Windows は CI で確認します。

### Q-007 ポインターの条件の検出方式

**Status**

解決済み

**Question**

ポインターをそのまま条件にする書き方を、どの方式で検出しますか。

**Context**

- [調査結果](#bl-001-ポインターの条件の常設の点検) のとおり、2 つの方式には検出範囲の差があります。

Table: ポインターの条件の検出方式の比較

| 観点 | A: libclang の点検スクリプト | B: clang-tidy |
| --- | --- | --- |
| 生のポインター | 検出する | 検出する |
| スマート ポインター | 検出する | 検出しない |
| 規範にない指摘 (整数など) | 出ない | 出るため、絞り込みが必要 |
| 保守 | スクリプトをリポジトリで保守する (前回の一時点検は 37 行) | 検査は LLVM が保守する |
| 入手 | pip のホイール | CI コンテナーの変更、または CI の手順での `dnf install` |

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) libclang (Python) の点検スクリプトをリポジトリに置く。
- [ ] B: clang-tidy の `readability-implicit-bool-conversion` を使い、ポインター以外の指摘を除く。
- [x] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: 今回は見送り

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 規範の対象 (ポインターとスマート ポインター) と検出範囲が一致し、規範にない指摘が出ないためです。CI コンテナーの変更も不要です。

**Related**

[Q-008](#q-008-点検ツールの入手方法)

**Assessment**

「今回は見送り」の回答により、BL-001 の検出方式は決めません。推奨案の A (libclang) は、本案件の残課題一覧の BL-001 に推奨方針として残します。

### Q-008 点検ツールの入手方法

**Status**

解決済み

**Question**

点検に使うツールを、CI と開発環境でどう入手しますか。

**Context**

- [Q-007](#q-007-ポインターの条件の検出方式) で A (libclang) を選んだ場合は、A または B から選んでください。B (clang-tidy) を選んだ場合は、C または D から選んでください。
- libclang のホイールは、libclang の共有ライブラリを同梱しており、Linux と Windows の Python 3 で動作します。版を固定しない場合、新しい版の公開で解析結果が変わるおそれがあります。

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) libclang のホイールを版を固定して (`libclang==18.1.1`) pip で導入する。CI は手順の中で導入し、開発環境は手順を文書に記載する。
- [ ] B: libclang を CI コンテナーに導入する (別リポジトリの変更とイメージの再発行)。
- [ ] C: clang-tidy を CI の手順で導入する (Linux は `dnf install clang-tools-extra`、Windows は Visual Studio 同梱のもの)。
- [ ] D: clang-tidy を CI コンテナーでソースからビルドする (clang-format と同じ LLVM 23.1.1)。
- [x] E: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: 今回は見送り

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: コンテナーの変更が不要で、Linux と Windows で同じ版を使えるためです。

**Related**

[Q-007](#q-007-ポインターの条件の検出方式)

**Assessment**

「今回は見送り」の回答により、点検ツールの入手方法は決めません。

### Q-009 点検へのコンパイル オプションの渡し方

**Status**

解決済み

**Question**

点検に、ビルドと同じインクルード パスとマクロ定義を、どう渡しますか。

**Context**

- ビルドと同じオプションで解析しないと、前回のように解析エラーが出て、エラーの箇所より後ろが点検されないおそれがあります。
- 案ごとの違いは次のとおりです。

```text
A の場合: makefw に点検の make 対象を設ける (例: make check-pointer-conditions)
  make test と同じく app 直下から各ディレクトリへ再帰し、各ディレクトリのソースの一覧、
  インクルード パス、マクロ定義を点検スクリプトに渡す。ビルドの後に実行する
  (ビルド時だけに作られるシンボリック リンクなどがそろった状態で解析する)。
B の場合: ビルド時に、コンパイルしたファイルごとのオプションを compile_commands.json に書き出す
  点検スクリプトは compile_commands.json を読む。並列ビルドでの書き込みの競合を避けるため、
  ファイルごとの断片を書き、後で結合する仕組みが必要になる。Windows は cl.exe 形式の
  オプションを clang の形式で解釈させる必要がある。
```

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) makefw に点検の make 対象を設け、各ディレクトリのソース、インクルード パス、マクロ定義を渡す。
- [ ] B: ビルド時に `compile_commands.json` を書き出し、点検スクリプトが読む。
- [x] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: 今回は見送り

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: ビルドの規則を変えずに追加でき、Linux と Windows で同じ make の変数 (インクルード パスとマクロ定義の一覧) から渡せるためです。B はビルドの全体に関わる変更になり、Windows のオプションの変換も必要です。

**Related**

[Q-010](#q-010-点検スクリプトの置き場所)

**Assessment**

「今回は見送り」の回答により、コンパイル オプションの渡し方は決めません。

### Q-010 点検スクリプトの置き場所

**Status**

解決済み

**Question**

点検スクリプトをどこに置きますか。

**Context**

- [Q-009](#q-009-点検へのコンパイル-オプションの渡し方) の A を選んだ場合、点検の make 対象は makefw に置きます。点検スクリプトの置き場所は次の 2 案です。
    - A: `framework/makefw/bin_internal/` に置く。make 対象と同じ場所で完結します。点検の内容は C/C++ の書き方の規則で、makefw の他の処理 (ビルド) とは性格が異なります。
    - B: `app/general/bin_internal/` に置き、makefw の make 対象から呼ぶ。規範 (`app/general/docs/coding-guideline.md`) と同じ場所に置けますが、framework から `app/general` を呼ぶ依存が新たに生じます。
- 汎用層 (framework) から個別の app の実装を規範として参照しない方針があります。`app/general` はワークスペース共通の文書と道具を置く場所で、個別の app ではありません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `framework/makefw/bin_internal/` に置く。
- [ ] B: `app/general/bin_internal/` に置き、makefw から呼ぶ。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: framework から `app/general` への新たな依存を作らず、make 対象とスクリプトを同じサブモジュールで保守できるためです。

**Related**

[Q-009](#q-009-点検へのコンパイル-オプションの渡し方)

**Assessment**

BL-001 を本案件の対象外としたため ([Q-001](#q-001-本案件で対処する残課題))、本案件では適用しません。将来 BL-001 に取り組む際の参考として、本案件の残課題一覧の BL-001 に転記します。

### Q-011 点検を実行する OS

**Status**

解決済み

**Question**

点検を、どの OS で実行しますか。

**Context**

- Windows だけでコンパイルされるコードは、Linux では解析できません。前回の一時点検では、正規表現と目視で点検しました。
- Windows でも実行する場合は、Windows の CI に手順を加え、Windows のインクルード パスとマクロ定義で解析します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) Linux と Windows の両方で実行する。
- [ ] B: Linux だけで実行する (Windows だけのコードは点検の対象外とする)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 前回の一時点検で漏れの可能性が残った理由の 1 つが Windows だけのコードであり、常設の点検でこの漏れをなくせるためです。

**Assessment**

BL-001 を本案件の対象外としたため ([Q-001](#q-001-本案件で対処する残課題))、本案件では適用しません。将来 BL-001 に取り組む際の参考として、本案件の残課題一覧の BL-001 に転記します。

### Q-012 点検の対象のコード

**Status**

解決済み

**Question**

点検の対象とするコードの範囲はどれですか。

**Context**

- 規範は第一者管理の C/C++ のコードに適用されます。外部由来のコード (cjson、zlib、lua、sqlite の取り込みソース、Google Test) は手動変更の対象外です。
- 点検で見つかった既存の逸脱は、本案件で直します (規範の新設時と同じく、既存のコードも同じ変更で移行します)。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 第一者管理のコード (各 app の製品コードとテスト コード、testfw のライブラリ) を対象にし、外部由来のコードを除く。
- [ ] B: 各 app の製品コードだけを対象にする。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 規範はテスト コードと testfw にも適用され、前回の一時点検でもテスト コードと testfw で逸脱が見つかったためです。

**Assessment**

BL-001 を本案件の対象外としたため ([Q-001](#q-001-本案件で対処する残課題))、本案件では適用しません。将来 BL-001 に取り組む際の参考として、本案件の残課題一覧の BL-001 に転記します。

### Q-013 点検の指摘の扱い

**Status**

解決済み

**Question**

点検で逸脱が見つかった場合、どう扱いますか。

**Context**

- コンパイラの警告は、`.warn` ファイルに記録され、CI の `warnings-summary` が警告として表示します。CI は失敗にしません。
- 点検の指摘を CI の失敗にすると、逸脱の混入を確実に止められますが、他の警告との扱いが異なります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) コンパイラの警告と同じく `.warn` ファイルに記録し、CI では警告として表示する。
- [ ] B: CI を失敗にする。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 既存の警告の扱いと一致し、警告の集約と対処の手順 (fix-ci-warnings) をそのまま使えるためです。

**Assessment**

BL-001 を本案件の対象外としたため ([Q-001](#q-001-本案件で対処する残課題))、本案件では適用しません。将来 BL-001 に取り組む際の参考として、本案件の残課題一覧の BL-001 に転記します。

### Q-014 CI で点検を実行する Linux の構成

**Status**

解決済み

**Question**

CI の Linux の 3 構成 (Oracle Linux 8、9、10) のうち、どれで点検を実行しますか。

**Context**

- 点検は、ビルド済みの状態で全ディレクトリのソースを解析します。前回の一時点検では、Linux の翻訳単位 1076 個を解析しました。
- 解析の結果は、システムのヘッダーの違いを除けば、構成によって変わりません。
- ドキュメントの生成は Oracle Linux 8 の構成だけが担当しています。Jenkins (`.jenkins/build.sh`) にも同じ考え方の分担があります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 1 つの構成 (Oracle Linux 8) だけで実行する。Jenkins も同じ考え方で 1 つの構成だけにする。
- [ ] B: 3 つの構成すべてで実行する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 構成による解析結果の差はほとんどなく、CI の時間の増加を 1 構成分に抑えられるためです。

**Related**

[Q-011](#q-011-点検を実行する-os)

**Assessment**

BL-001 を本案件の対象外としたため ([Q-001](#q-001-本案件で対処する残課題))、本案件では適用しません。将来 BL-001 に取り組む際の参考として、本案件の残課題一覧の BL-001 に転記します。

### Q-015 受入時のテストの範囲

**Status**

解決済み

**Question**

受入時に、どの範囲のテストを実行しますか。

**Context**

- 局所的な確認として、次を予定しています。
    - `framework/makefw/bin_test` の全テスト (BL-002、BL-003)
    - 1 つの app (`app/tutorial`) で、makefw の make ファイルに一時的な変更を加えると app 直下の省略が解除されることの確認 (確認後に変更を戻します)
    - BL-001 を選んだ場合: 点検スクリプトの単体テストと、全 app での点検の実行 (Linux)。Windows は CI で確認します。
    - 変更した Markdown への `text_style_jp.py --dry-run`
- リポジトリ ルートの `make test` は、署名の変更ですべての app が再ビルドと再実行になります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 局所的な確認と CI だけにする。
- [ ] B: 局所的な確認と、リポジトリ ルートの `make test` 1 回を行う。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: テストの実行内容は変わらず、点検の既存の逸脱を直した場合も、その影響は CI の全テストで確認できるためです。

**Assessment**

局所的な確認 (`framework/makefw/bin_test` の全テスト、`app/tutorial` での省略の解除の確認、変更した Markdown の `text_style_jp.py --dry-run`) と CI だけにします。リポジトリ ルートの `make test` は行いません。

### Q-016 コミットとプッシュの扱い

**Status**

解決済み

**Question**

受入条件の確認後、コミットとプッシュを行いますか。

**Context**

- 変更は、サブモジュール (`framework/makefw`、点検で逸脱を直す場合は該当する app や testfw) とリポジトリ本体 (CI の手順、サブモジュールの参照) にまたがる見込みです。
- AGENTS.md により、コミットとプッシュはユーザーの個別の指示が必要です。前案件の許可は、前案件に限るものです。

**Answer**

選択方式: 単一選択

- [x] A: 受入条件の確認後に、サブモジュール、リポジトリ本体の順にコミットとプッシュを行い、CI を追跡する。
- [ ] B: コミットとプッシュは行わない (ユーザーが行う)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨なし。Git の書き込み操作はユーザーの判断事項のためです。

**Assessment**

受入条件の確認後に、`framework/makefw`、`framework/testfw`、リポジトリ本体の順にコミットとプッシュを行い、CI を追跡します。この許可は本案件に限ります。

### Q-017 前案件の文書の扱い

**Status**

解決済み

**Question**

第 3 ゲートの承認後、前案件の 3 文書 (`test-assert-comment-followup4-discussion.md`、`-design.md`、`-backlog.md`) をどう扱いますか。

**Context**

- これまでの案件では、前案件の 3 文書を、第 3 ゲートの承認後に `prompt/past/` へ `mv` で移動しました。`prompt/` は Git の管理対象外です。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 第 3 ゲートの承認後に、`prompt/past/` へ `mv` で移動する。
- [ ] B: `prompt/current/` に残す。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 12:19:31 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: これまでの案件と同じ扱いで、`prompt/current/` に進行中の案件の文書だけが残るためです。

**Assessment**

第 3 ゲートの承認後に、前案件の 3 文書を `prompt/past/` へ `mv` で移動します。

### Q-018 テスト ディレクトリ単位の省略判定への makefw の変更の反映

**Status**

解決済み

**Question**

makefw の変更を、テスト ディレクトリ単位の省略判定 (`test.stamp`) にも反映しますか。

**Context**

- 最終仕様の作成中に、次の事実を確認しました。
    - テストの省略は 2 段階です。app 直下の署名 (`make_test.stamp`) が一致すると、サブディレクトリへの再帰を省きます。一致しない場合も、各テスト ディレクトリの `exec_test_c_cpp.sh` が `test.stamp` と比べ、一致すればテストの実行を省いて前回の結果を残します。
    - `test.stamp` の入力 (`compute_test_signature`) は、そのディレクトリのテスト ソース、`makepart.mk` と `makelocal.mk`、リンクするライブラリ (`.a` / `.lib`)、共通処理の抜粋元、testfw のスクリプト (`list_signature_scripts.sh` の出力) です。テストの実行ファイル、コンパイル オプション、makefw の make ファイルは含みません。
- このため、[Q-002](#q-002-署名に加える-makefw-の入力の範囲) と [Q-003](#q-003-makefw-の入力を加える-app) の合意だけでは、残課題の「ビルドの規則を変えた場合は、影響する app のビルドとテストが再実行される」を一部しか満たしません。

```text
例: makefw の _flags.mk でコンパイル オプションを変える
  app 直下:   署名が変わる → サブディレクトリへ再帰する (合意済みの変更で実現)
  製品ライブラリ: 再コンパイルされ、.a が変わる → それをリンクするテストは test.stamp が変わり、再実行される
  テスト ソース: 再コンパイルされ、実行ファイルが作り直される → test.stamp は変わらず、テストは省略される
```

- 案ごとの違いは次のとおりです。

```text
A の場合: test.stamp の入力に、makefw の入力 (makefiles/ 直下のすべてのファイル、bin_internal/ 直下のスクリプト) を加える
  makefw の範囲の定義を makefw 側の 1 つのスクリプト (例: framework/makefw/bin_internal/list_signature_inputs.sh) に置き、
  resolve_app_deps.sh と、testfw の list_signature_scripts.sh の両方がそれを使う (範囲の定義は 1 か所)。
  makefw を変えると、すべてのテスト ディレクトリでテストを再実行する。
  Windows では、ファイルごとのハッシュの計算が 1 ディレクトリあたり約 40 回増えるため、
  1 回の md5sum で一覧をまとめて計算するよう改める。
B の場合: test.stamp の入力に、テストの実行ファイルのハッシュを加える
  makefw に限らず、コンパイル オプションやコンパイラの更新も検出できる。
  ただし、実行ファイルが作り直されるたびに内容が変わるか (MSVC のリンク時刻の埋め込み、
  ビルド情報の埋め込み) を調べる必要があり、意図しない再実行が起きるおそれがある。現時点で未検証。
C の場合: test.stamp は変えない
  makefw を変えた後に結果を確実に作り直すには、MAKEFW_TEST_FORCE=1 を指定する必要がある。
  文書にこの制約を記載し、残課題一覧に記録する。
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `test.stamp` の入力に makefw の入力を加え、範囲の定義を makefw の 1 つのスクリプトにまとめる。
- [ ] B: `test.stamp` の入力に、テストの実行ファイルのハッシュを加える。
- [ ] C: `test.stamp` は変えず、制約として文書と残課題一覧に記録する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 17:29:30 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: [Q-002](#q-002-署名に加える-makefw-の入力の範囲) で合意した範囲をそのまま使え、makefw を変えた後に古い結果が残ることがなくなるためです。範囲の定義を makefw に置くので、app 直下とテスト ディレクトリで範囲がずれません。B は対象が広い反面、意図しない再実行の有無が未検証です。  
制約: A では、testfw のスクリプトが makefw のスクリプトを呼ぶ依存が増えます (現在も `get_files_lang.sh` を参照しています)。また、makefw の変更のたびに、全テストの再実行に時間がかかります。

**Related**

[Q-002](#q-002-署名に加える-makefw-の入力の範囲)、[Q-003](#q-003-makefw-の入力を加える-app)

**Assessment**

`test.stamp` の入力にも makefw の入力を加えます。範囲は新設の `framework/makefw/bin_internal/list_signature_inputs.sh` だけで定義し、`resolve_app_deps.sh` (app 直下) と `exec_test_c_cpp.sh` (テスト ディレクトリ単位) の両方が使います。testfw の `list_signature_scripts.sh` が個別に挙げている `get_files_lang.sh` は makefw の範囲に含まれるため、testfw の一覧からは外し、範囲の定義の重複をなくします。Windows の追加のハッシュの計算は、キャッシュにないファイルを 1 回の `md5sum` でまとめて計算する方式に改めます (Linux も同じ方式)。

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `prompt/current/test-assert-comment-followup5-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 4

最終仕様 (design.md) の合意版 (第 2 ゲート): 2026-10-24 承認 (対話セッション、「承認」)

更新予定の成果物: `framework/makefw/bin_internal/list_signature_inputs.sh` (新設)、`framework/makefw/bin_internal/resolve_app_deps.sh`、`framework/testfw/bin_internal/list_signature_scripts.sh`、`framework/testfw/bin_test/test_signature_scripts.py`、`framework/testfw/bin_test/test_path_spaces.py`、`framework/testfw/docs/how-to-test.md`、`framework/makefw/bin_test/app_paths_selftest.py` (`bin_internal/` から移動)、`framework/makefw/bin_test/test_build_signature.py`、`framework/makefw/docs/build-configurations.md`、`framework/testfw/bin_internal/exec_test_c_cpp.sh`、リポジトリ本体のサブモジュールの参照

受入条件: `test-assert-comment-followup5-design.md` の受入条件に従います。

## 反映と検証の記録

| 項目 | 結果 |
| --- | --- |
| 自己テストの移動と修正 | `framework/makefw/bin_test/app_paths_selftest.py` へ `mv` で移動。`makefiles/` 全体のコピーと、PATH 上の `bash` の明示を追加。Linux で 2 つの確認が合格 |
| makefw の入力の範囲の定義 | `framework/makefw/bin_internal/list_signature_inputs.sh` を新設 (41 件を出力) |
| app 直下の署名 | `resolve_app_deps.sh` に `collect_makefw_signature_files` を追加。`app/tutorial` の署名に `framework/makefw/` の 41 件が含まれることを確認 |
| テスト ディレクトリ単位の判定 | `exec_test_c_cpp.sh` の `compute_test_signature` に makefw の入力を追加し、キャッシュにない入力を 1 回の `md5sum` で計算する方式に変更。testfw の `list_signature_scripts.sh` から `get_files_lang.sh` を除外 |
| コメントの整形 | `exec_test_c_cpp.sh` の `1つ` を `1 つ` に修正 |
| 文書 | `framework/makefw/docs/build-configurations.md`、`framework/testfw/docs/how-to-test.md` |
| 受入条件 1 (`bin_test`、Linux) | `run-bin-tests.py framework/makefw/bin_test framework/testfw/bin_test` で 5 グループすべて合格 (makefw の unittest 16 件と自己テスト 3 本、testfw の unittest 108 件) |
| 受入条件 3 (`app/tutorial`) | 変更前に `make test` の省略を確認。`framework/makefw/makefiles/_flags.mk` に一時的に 1 行加えると、app 直下とテスト ディレクトリのどちらも省略せず、2 つのテスト ディレクトリでテストを再実行し、14 個の `results.md` と 2 個の `test.stamp` を更新。変更を戻し、差分がないことを確認 |
| 受入条件 4 | `bin_internal/` に `app_paths_selftest.py` がなく、文書の案内が移動先を指すことを確認 |
| 受入条件 5 | 変更した Markdown 2 件とスクリプト 9 件に `text_style_jp.py --dry-run` の指摘なし。整形の指摘 2 件は解消 |
| 受入条件 6 | `app/tutorial` と `framework/testfw` に内容のある `.warn` なし |
| コミットとプッシュ | `framework/makefw` `5a72b0c`、`framework/testfw` `9939605`、リポジトリ本体 `db722064`。リポジトリ本体は、リモートに先行していた 2 件のコミット (`app/cplat` と `app/porter` の参照の更新) の上に rebase してからプッシュ |
| 受入条件 2、7 (CI) | 成功 (`CI (Build, Test, and Docs)`、run 40079463868。Linux の ol8、ol9、ol10、Windows、警告の集計、Pages の発行の全ジョブが成功) |
