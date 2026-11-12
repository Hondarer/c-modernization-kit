<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新エージェント: claude code / セッション ID: session_01KJc53SaXJKaFZiB7mV4bp4
下流文書: `261111-ci-docs-job-split-design.md` (最終仕様)、`261111-ci-docs-job-split-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。セッション情報が見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があります。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。
-->

# CI のドキュメント生成ジョブの分離と make gen の追加を検討する

検討 ID: `ci-docs-job-split-20261111`  
版: 4  
状態: 完了 (第 3 ゲート通過、2026-11-11 に承認)

## 作業 WBS と進捗管理

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

フェーズ 4 の項目は、最終仕様 (`261111-ci-docs-job-split-design.md`) の変更対象に対応します。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. CI の所要時間の内訳と、ドキュメント生成が依存する生成物を調査する。
    - ✅ 1.2. 論点を抽出し、確認項目 ([Q-001](#q-001-make-gen-の契約) から [Q-010](#q-010-windows-の-make-gen-の検証方法)) を作成する。
    - ✅ 1.3. ユーザーの回答を受け付けて記録する。
    - ✅ 1.4. 回答に基づいて前提と制約を検証し、作業 WBS を見直す。版 2 で 4.1、4.4、4.8 を、版 3 で 4.1、4.4、4.9 を見直した。
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート)
    - ✅ 2.1. 全論点の未解決事項と質問が解消したことを確認する。
    - ✅ 2.2. 方針と前提条件の合意を確定する。
- ✅ 3. 最終仕様の作成と承認の受付
    - ✅ 3.1. `261111-ci-docs-job-split-design.md` を作成する。
    - ✅ 3.2. 最終仕様の受入条件と整合性を確認する。
    - ✅ 3.3. 最終仕様の承認を受け付ける (第 2 ゲート)。2026-11-11 に承認。あわせて、全サブモジュールを含むコミットと push、CI の問題に対する再コミットの事前許可を受けた。サブエージェントとして codex CLI と grok を 1 つずつ使う指示を受けた。
- ✅ 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. `framework/makefw/makefiles/makemain.mk` に空の `gen` と `SUBDIRS` への伝播を追加する。`_flex_bison_compile.mk` と `_resource_compile.mk` から `gen` へ前提を追加し、`.mc` の規則を生成とコンパイルに分ける。
    - ✅ 4.2. `framework/makefw/makefiles/__each_app_template.mk` と `__app_template.mk` へ `gen` を追加する。
    - ✅ 4.3. `framework/makefw/bin_internal/resolve_app_deps.sh` へ `--gen-build-apps` を追加する。
    - ✅ 4.4. `framework/makefw/bin_test/` へ、flex / bison の生成、`.mc` の生成、中間ディレクトリの伝播、app の解決のテストを追加する。
    - ✅ 4.5. `bin/update_template_makefiles.py` で、各 app の `makefile` をテンプレートに同期する。
    - ✅ 4.6. `app/struct-meta/appdeps.mk` へ、自前ビルドの生成器を持つ宣言を追加する。
    - ✅ 4.7. ルートの `makefile` へ `gen` を追加する。
    - ✅ 4.8. `.github/workflows/ci.yml` へ `docs` ジョブを追加し、OL8 のレグからドキュメント関連のステップを外す。`.jenkins/build.sh` に触れるコメントを実態に合わせる。
    - ✅ 4.9. 生成漏れの照合 (`verify-gen` ジョブと、ファイル一覧のアーティファクト) を `ci.yml` へ追加する。差分があれば失敗にする。
    - ✅ 4.10. 正本ドキュメント (makefw の `docs/`、`app/general/docs/workspace-agent-workflow.md`) とコード コメントへ、永続的な知見を反映する。
    - ✅ 4.11. 局所テストと機械的検証 (`text_style_jp.py`、`make bin-test` の該当分) を実施する。
    - ✅ 4.12. 受入条件への適合を確認する。手元の確認と、GitHub Actions の実走 (実行 42930377100) での確認を完了した。
    - ✅ 4.13. 残課題一覧 (`261111-ci-docs-job-split-backlog.md`) を再構築して整理する。
- ✅ 5. 成果物の承認の受付 (第 3 ゲート)
    - ✅ 5.1. 成果物、検証結果、残課題を提示する。
    - ✅ 5.2. 成果物の承認を受け付ける。2026-11-11 に承認。

## 対象と参照資料

対象成果物: makefw のテンプレートと補助スクリプト、各 app の `makefile`、`app/struct-meta/appdeps.mk`、ルートの `makefile`、`.github/workflows/ci.yml`、関連する正本ドキュメント。確定は回答後です。

今回の対象範囲: GitHub Actions の構成と、`make gen` の追加です。`.jenkins/` は対象外です ([Q-008](#q-008-jenkins-の構成を対象に含めるか))。

書き込み可能な検討用 Markdown: この文書 (`app/general/docs/prompts/past/261111-ci-docs-job-split-discussion.md`)

参照資料と版: リポジトリは `main` の `2ccd1c5e` です。CI の実績は、GitHub Actions の実行 42821358874、42786336853、42741579412 です。並列数と時間の上限は [Actions limits](https://docs.github.com/en/actions/reference/limits) を参照しました。

### 調査で確認した事実

直近の成功 3 回では、OL8 のレグが毎回最長で、その中でドキュメント生成が 70 ～ 92[分] を占めます。

| 実行 | 全体 | OL8 | うち `make doxy && make docs` | Windows | OL9 |
| --- | --- | --- | --- | --- | --- |
| 42821358874 | 320[分] | 317[分] | 85[分] | 275[分] | 232[分] |
| 42786336853 | 280[分] | 277[分] | 70[分] | 271[分] | 216[分] |
| 42741579412 | 353[分] | 349[分] | 92[分] | 196[分] | 178[分] |

Table: 直近の成功 3 回のジョブ別所要時間

実行 42821358874 では、`make doxy` が 3.5[分]、`make docs` が 81.5[分] でした。  
ドキュメント生成を別ジョブにすると、全体は「OL8 からドキュメント生成を除いた時間」と「Windows」の長いほうで決まります。短縮の見込みは、上の 3 回でそれぞれ約 42[分]、約 6[分]、約 92[分] です。  
GitHub の Free プランでは、標準ランナーの同時実行は 20 ジョブまで、1 ジョブの上限は 6 時間です。実行 42741579412 の OL8 は 349[分] で、上限まで 11[分] でした。

`make docs` が処理した Markdown 1243 件のうち、925 件は `make doxy` が出力する doxybook2 の Markdown です。そのため `make docs` の前に `make doxy` が必要です。  
Doxygen の入力は各 app の `prod` 配下で、`gen/` を除外していません (`framework/doxyfw/Doxyfile` の `EXCLUDE_PATTERNS` は `*/obj/*` だけです)。

Linux で Doxygen の入力に入る生成物は、次の 3 種類です。

| 種類 | 対象 | 生成の仕組み | ビルドの要否 |
| --- | --- | --- | --- |
| パース時スクリプト | `app/string-catalog-sample` の `gen/` 3 か所と `prod/include/samplecatalog/` | app 直下の `makepart.mk` が、パース時に Python を実行する | 不要 |
| ツールだけの規則 | `app/struct-meta/prod/libsrc/struct_meta/parse/gen/` | makefw の flex / bison 規則 (`_flex_bison_compile.mk`) | 不要 |
| 自前ビルドの生成器 | `app/struct-meta/prod/src/cmd/struct-meta-sample/gen/` | ビルドした `struct-meta-gen` を実行する | 必要 |

Table: Doxygen の入力に入る生成物の種類

`struct-meta-gen` は libstruct_meta、cplat、cjson にリンクします。cplat は `appdeps.mk` で cjson、zlib、openssl に依存します。  
makefw には、依存先の `prod` だけを make し、`make_build.stamp` を更新しない先例があります (`__each_app_template.mk` の `_makefw_with_cov_prod` と、`resolve_app_deps.sh --coverity-apps`)。  
各 app の `makefile` は `__each_app_template.mk` の複製であり、テンプレートを変更した場合は `bin/update_template_makefiles.py` による同期が必要です。  
`framework/makefw` と、app のうち porter、cjson、sqlite、lua、zlib、cplat、openssl はサブモジュールです (`.gitmodules`)。テンプレートの変更と `makefile` の同期は、これらのサブモジュールにもまたがります。ステージングとコミットは、ユーザーの個別の指示があるまで行いません。

### 未確認の事項

- cplat の `prod` だけのビルド時間は実測していません。ログからの推定は [Q-007](#q-007-cplat-の-prod-のビルド時間の測定方法) の Assessment に記録しました。実測は CI の実走で確認します。
- 中間ディレクトリのテンプレートが、新しいゴール `gen` を下位へ伝える方法は読んでいません。最終仕様の作成時に確認します。

## 現行の合意内容

ユーザーが明示した内容は、次のとおりです (対話セッション、2026-11-11)。

- CI のドキュメント生成を分け、全体の所要時間を短縮したい。GitHub Actions の並列制限を考慮する。
- `make gen` などのターゲットは、結果としてあったほうがよい。

解決済みの質問で確定した内容は、次のとおりです (回答: 2026-11-11 06:08 JST)。

- `make gen` は、自前ビルドの生成器を持つ app とその依存先だけ、`prod` をテスト抜きでビルドする ([Q-001](#q-001-make-gen-の契約))。
- 宣言は app 単位で `appdeps.mk` に置き、変数は `APP_GEN_REQUIRES_BUILD := 1` とする ([Q-002](#q-002-宣言の粒度)、[Q-003](#q-003-宣言に使う変数名))。
- ルートの `doxy` は `gen` に依存させず、CI で `make gen && make doxy && make docs` と並べる ([Q-004](#q-004-ルートの-doxy-を-gen-に依存させるか))。
- 生成漏れの照合を CI に入れ、差分があれば失敗にする ([Q-005](#q-005-生成漏れの照合))。
- Windows だけの生成物も `gen` の対象に含め、`.mc` の規則を分割する ([Q-006](#q-006-windows-だけの生成物の扱い))。
- cplat の `prod` のビルド時間はエージェントが事前に評価する。評価の結果は 25[分] 以内の見込みである ([Q-007](#q-007-cplat-の-prod-のビルド時間の測定方法))。
- `.jenkins/` は対象外とする ([Q-008](#q-008-jenkins-の構成を対象に含めるか))。
- 受入の判定に CI の実走結果 (全ジョブの成功、ページ数の一致、所要時間の短縮) を含める ([Q-009](#q-009-受入の判定に-ci-の実走結果を含めるか))。

- Windows の `make gen` は、makefw の `bin_test/` に追加するテストで確認する ([Q-010](#q-010-windows-の-make-gen-の検証方法)、回答: 2026-11-11 06:18 JST)。

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | 現行の合意内容。CI の所要時間の短縮と、6 時間の上限に対する余裕の確保 |
| 範囲・境界 | 確認済み | [Q-006](#q-006-windows-だけの生成物の扱い)、[Q-008](#q-008-jenkins-の構成を対象に含めるか) |
| 入力・根拠 | 確認済み | [Q-007](#q-007-cplat-の-prod-のビルド時間の測定方法) (推定。実測は CI の実走で確認する) |
| 整合性・依存関係 | 確認済み | [Q-001](#q-001-make-gen-の契約)、[Q-002](#q-002-宣言の粒度)、[Q-003](#q-003-宣言に使う変数名)、[Q-004](#q-004-ルートの-doxy-を-gen-に依存させるか) |
| 実現性・例外 | 確認済み | [Q-005](#q-005-生成漏れの照合)、[Q-007](#q-007-cplat-の-prod-のビルド時間の測定方法) |
| 受入条件・検証 | 確認済み | [Q-009](#q-009-受入の判定に-ci-の実走結果を含めるか)、[Q-010](#q-010-windows-の-make-gen-の検証方法) |

Table: 観点別の確認状況

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-make-gen-の契約) | `make gen` の契約 | 解決済み | なし |
| [Q-002](#q-002-宣言の粒度) | 宣言の粒度 | 解決済み | [Q-001](#q-001-make-gen-の契約) |
| [Q-003](#q-003-宣言に使う変数名) | 宣言に使う変数名 | 解決済み | [Q-002](#q-002-宣言の粒度) |
| [Q-004](#q-004-ルートの-doxy-を-gen-に依存させるか) | ルートの `doxy` を `gen` に依存させるか | 解決済み | [Q-001](#q-001-make-gen-の契約) |
| [Q-005](#q-005-生成漏れの照合) | 生成漏れの照合 | 解決済み | [Q-001](#q-001-make-gen-の契約) |
| [Q-006](#q-006-windows-だけの生成物の扱い) | Windows だけの生成物の扱い | 解決済み | [Q-001](#q-001-make-gen-の契約) |
| [Q-007](#q-007-cplat-の-prod-のビルド時間の測定方法) | cplat の `prod` のビルド時間の測定方法 | 解決済み | [Q-001](#q-001-make-gen-の契約) |
| [Q-008](#q-008-jenkins-の構成を対象に含めるか) | Jenkins の構成を対象に含めるか | 解決済み | なし |
| [Q-009](#q-009-受入の判定に-ci-の実走結果を含めるか) | 受入の判定に CI の実走結果を含めるか | 解決済み | [Q-007](#q-007-cplat-の-prod-のビルド時間の測定方法) |
| [Q-010](#q-010-windows-の-make-gen-の検証方法) | Windows の `make gen` の検証方法 | 解決済み | [Q-005](#q-005-生成漏れの照合)、[Q-006](#q-006-windows-だけの生成物の扱い) |

Table: 質問一覧

## 明確化のための確認項目

### Q-001 make gen の契約

**Status**

解決済み

**Question**

`make gen` は、生成器の実行に必要な範囲のコンパイルを含めますか。

**Context**

`app/struct-meta` の `struct-meta-sample/gen/` は、ビルドした `struct-meta-gen` が生成します。この実行体は cplat、cjson、libstruct_meta にリンクするため、ビルドに一切依存しない `make gen` は、今の構成では成立しません。  
案 A では、CI は `make gen && make doxy && make docs` を呼びます。生成だけで済む app は生成だけを行い、自前ビルドの生成器を持つ app とその依存先だけ、テスト抜きで `prod` をビルドします。今の構成では struct-meta、cplat、cjson、zlib、openssl の 5 つが `prod` ビルドの対象になります。  
案 B では、`make gen` はコンパイルを行いません。`struct-meta-sample/gen/` は生成できないため、該当の 2 ページ (`sample_types_meta.c`、`sample_types_meta.h`) が文書から欠けます。  
案 C では、`make gen` を作らず、CI が `prod` だけのビルドを直接指定します。構成が変わるたびに `ci.yml` を直す必要があります。

**Answer**

選択方式: 単一選択  
回答者: Tetsuo Honda  
回答日時: 2026-11-11 06:08:06 JST

- [x] A: (推奨) 含める。自前ビルドの生成器を持つ app とその依存先だけ、`prod` をテスト抜きでビルドする。
- [ ] B: 含めない。ビルドが必要な生成物は、分離したジョブの文書から外す。
- [ ] C: `make gen` を作らず、CI で `prod` だけのビルドを直接指定する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 発行する文書の内容を変えずに分離でき、構成の変更を app 側の宣言で吸収できます。`with-cov` に、依存先の `prod` だけを make する同じ形の先例があります。制約として、cplat の `prod` のビルド時間が長い場合は短縮効果が減ります ([Q-007](#q-007-cplat-の-prod-のビルド時間の測定方法))。

**Assessment**

案 A で確定です。`make gen` は、自前ビルドの生成器を持つ app とその依存先だけ、`prod` をテスト抜きでビルドします。所要時間の制約は [Q-007](#q-007-cplat-の-prod-のビルド時間の測定方法) の評価で確認しました。

### Q-002 宣言の粒度

**Status**

解決済み

**Question**

「`gen` に `prod` のビルドが必要である」ことを、どの単位で宣言しますか。

**Context**

[Q-001](#q-001-make-gen-の契約) で案 A を採る場合の質問です。  
app 単位では、宣言した app とその `APP_DEPS` の推移的な依存先について、`gen` を `$(MAKE) -C prod` に切り替えます。`resolve_app_deps.sh --coverity-apps` と同じ形で解決できます。struct-meta は `prod` 全体をビルドします。  
ディレクトリ単位では、libstruct_meta と `struct-meta-gen` のディレクトリだけをビルド対象にできます。ただし、依存する他 app のライブラリを指す宣言も別に必要になり、仕組みが増えます。減るのは struct-meta の `prod` の一部 (`struct-meta-sample` のコンパイルとリンク) だけです。

**Answer**

選択方式: 単一選択  
回答者: Tetsuo Honda  
回答日時: 2026-11-11 06:08:06 JST

- [x] A: (推奨) app 単位で宣言する (`appdeps.mk`)。
- [ ] B: ディレクトリ単位で宣言する (`makepart.mk`)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 既存の解決スクリプトと `with-cov` の先例を流用でき、追加する仕組みが少なく済みます。ディレクトリ単位で減らせるビルド量は小さいと見ています。ただし、この見込みは実測していません。

**Related**

[Q-001](#q-001-make-gen-の契約)、[Q-003](#q-003-宣言に使う変数名)

**Assessment**

案 A で確定です。宣言は app 単位で `appdeps.mk` に置きます。[Q-001](#q-001-make-gen-の契約) の回答と矛盾しません。

### Q-003 宣言に使う変数名

**Status**

解決済み

**Question**

宣言に使う変数名と値を、どれにしますか。

**Context**

[Q-002](#q-002-宣言の粒度) で案 A を採る場合の質問です。`appdeps.mk` には現在 `APP_DEPS` だけがあります。`resolve_app_deps.sh` は `APP_DEPS` の自明な代入を awk で読むため、同じ形式の代入にそろえます。

```makefile
# 案 A
APP_DEPS := cplat cjson
APP_GEN_MODE := build
```

```makefile
# 案 B
APP_DEPS := cplat cjson
APP_GEN_REQUIRES_BUILD := 1
```

**Answer**

選択方式: 単一選択  
回答者: Tetsuo Honda  
回答日時: 2026-11-11 06:08:06 JST

- [ ] A: `APP_GEN_MODE := build` (未定義の場合は生成だけを行う)
- [x] B: `APP_GEN_REQUIRES_BUILD := 1`
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: なし  
理由: 命名の規範として参照できる根拠を、makefw の文書から見つけていません。どちらも動作は同じです。

**Related**

[Q-002](#q-002-宣言の粒度)

**Assessment**

案 B で確定です。変数は `APP_GEN_REQUIRES_BUILD := 1` とします。未定義の app は生成だけを行います。今の構成で宣言する app は `app/struct-meta` だけです。

### Q-004 ルートの doxy を gen に依存させるか

**Status**

解決済み

**Question**

ルートの `makefile` の `doxy` を、`gen` に依存させますか。

**Context**

依存させると、クリーンな作業ツリーで `make doxy` を実行しても、生成物を含む文書が得られます。その代わり、手元の `make doxy` が `prod` のビルドを誘発する場合があります。  
依存させない場合は、`make doxy` の動作は今と変わりません。CI は `make gen && make doxy && make docs` と明示的に並べます。

**Answer**

選択方式: 単一選択  
回答者: Tetsuo Honda  
回答日時: 2026-11-11 06:08:06 JST

- [x] A: (推奨) 依存させない。CI で明示的に並べる。
- [ ] B: 依存させる。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 既存の `make doxy` の動作と所要時間を変えません。各 app 直下の `make doxy` は `gen` に依存しないため、ルートだけ依存させると動作がそろいません。

**Related**

[Q-001](#q-001-make-gen-の契約)

**Assessment**

案 A で確定です。ルートの `doxy` は変更しません。CI は `make gen && make doxy && make docs` と並べます。

### Q-005 生成漏れの照合

**Status**

解決済み

**Question**

`make gen` の生成漏れを CI で検出する照合を、今回の範囲に入れますか。

**Context**

将来、宣言を忘れた生成物が増えると、`docs` ジョブの文書からページが欠けます。エラーにはならないため、気付きにくい不具合です。  
照合の案は次のとおりです。OL8 のレグ (フル ビルド後) と `docs` ジョブ (`make gen` 後) で、`prod` 配下の `gen/` のファイル一覧をアーティファクトに出します。`warnings-summary` ジョブで 2 つを突き合わせ、差分があれば失敗にします。

**Answer**

選択方式: 単一選択  
回答者: Tetsuo Honda  
回答日時: 2026-11-11 06:08:06 JST

- [x] A: (推奨) 入れる。差分があれば CI を失敗にする。
- [ ] B: 入れる。差分は警告にとどめる。
- [ ] C: 入れない。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 文書の欠落は、発行結果を目で比べない限り見つかりません。照合はファイル一覧の比較だけで、実行時間への影響は小さいと見ています。

**Related**

[Q-001](#q-001-make-gen-の契約)

**Assessment**

案 A で確定です。OL8 のレグと `docs` ジョブの `gen/` のファイル一覧を突き合わせ、差分があれば CI を失敗にします。照合は Linux の生成物だけが対象です。Windows の生成物の確認は [Q-010](#q-010-windows-の-make-gen-の検証方法) で扱います。

### Q-006 Windows だけの生成物の扱い

**Status**

解決済み

**Question**

Windows だけで生成される生成物を、`gen` の対象に含めますか。

**Context**

`app/cplat/prod/libsrc/cplat_eventlog_messages/gen/` は、Windows の mc.exe が生成します (`_resource_compile.mk`)。Linux の CI の発行には入りません。  
含める場合は、Windows で `make gen` を実行したときにも生成物がそろいます。`.mc` の規則は `.res` までを 1 つの規則で作るため、生成だけを切り出すには規則の分割が必要です。  
含めない場合は、Windows の `make gen` は flex / bison とパース時スクリプトの生成物だけを作ります。

**Answer**

選択方式: 単一選択  
回答者: Tetsuo Honda  
回答日時: 2026-11-11 06:08:06 JST

- [x] A: 含める。`.mc` の規則を分割する。
- [ ] B: 含めない。今回は Linux の発行に必要な範囲に限り、残課題として記録する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: なし  
理由: 目的 (CI の短縮) だけなら案 B で足ります。一方、ルートの `AGENTS.md` は Linux と Windows の双方で動作する構成の維持を求めています。Windows で `make gen` の結果が不完全になることを許容するかは、ユーザーの判断が必要です。

**Related**

[Q-001](#q-001-make-gen-の契約)

**Assessment**

案 A で確定です。`_resource_compile.mk` の `.mc` の規則を、mc.exe による生成 (`gen/` への出力) と rc.exe によるコンパイル (`.res`) に分け、生成の側を `gen` の対象にします。  
この回答により、Windows の `make gen` を確認する方法が新たに必要になりました。CI の Windows ジョブは `make gen` を実行しないためです。[Q-010](#q-010-windows-の-make-gen-の検証方法) で確認します。

**Related** (回答後に追加)

[Q-010](#q-010-windows-の-make-gen-の検証方法)

### Q-007 cplat の prod のビルド時間の測定方法

**Status**

解決済み

**Question**

cplat の `prod` だけのビルド時間を、どの方法で測定しますか。

**Context**

`docs` ジョブは、[Q-001](#q-001-make-gen-の契約) の案 A では cplat の `prod` をビルドします。CI の cplat のビルドは 124[分] ですが、テストを含む値です。CI のログは app ごとにまとめて出力されるため、`prod` だけの時間を読み取れませんでした。  
`prod` のビルドが 130[分] 程度を超えると、`docs` ジョブが最長になり、短縮効果がなくなります。この値は、実行 42821358874 の OL8 (ドキュメント生成を除く 232[分]) から、`make doxy && make docs` の約 85[分] と準備の時間を引いた目安です。

**Answer**

選択方式: 単一選択  
回答者: Tetsuo Honda  
回答日時: 2026-11-11 06:08:06 JST

- [ ] A: ユーザーが手元で測定し、結果をこの文書に記入する。
- [ ] B: 設計の前には測定しない。実装後の CI の実走で確認し、効果がなければ見直す。
- [x] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: Agent が事前評価する。

**Recommendation**

推奨: なし  
理由: 手元の測定は早く結果が出ますが、CI のランナー (4 vCPU) との性能差があります。CI の実走は正確ですが、結果が出るまで実装が先行します。どちらを採るかは、手戻りの許容度によります。

**Related**

[Q-001](#q-001-make-gen-の契約)、[Q-009](#q-009-受入の判定に-ci-の実走結果を含めるか)

**Assessment**

自由記述「Agent が事前評価する」で確定です。エージェントが、実行 42821358874 の OL8 のログから評価しました。合意前のため、ビルドは実行していません。

評価の結果、`make gen` は 25[分] 以内に収まる見込みで、短縮効果がなくなる目安 (130[分]) を大きく下回ります。根拠は次のとおりです。

- cplat のビルド 124[分] のうち、ログに出たコンパイルとリンクのコマンドは、`prod` が 172 件 (gcc 158、g++ 14)、`test` が 950 件 (g++ 730、gcc 220) でした。`prod` は件数で全体の約 15[%] です。
- 件数に比例すると、`prod` は約 19[分] です。`test` は Google Mock を使う C++ が大半で 1 件あたりが重いため、実際の `prod` はこれより短いと見ています。
- 依存先の所要時間は、テストを含む値で、openssl が約 2[分]、zlib と cjson が各約 1[分]、struct-meta が約 2[分] でした。

この見込みから、`docs` ジョブは 105 ～ 125[分] 程度になり、OL8 のレグ (ドキュメント生成を除いて 207 ～ 257[分]) より短くなります。  
これは件数からの推定であり、実測ではありません。実測は、[Q-009](#q-009-受入の判定に-ci-の実走結果を含めるか) の CI の実走で確認します。

### Q-008 Jenkins の構成を対象に含めるか

**Status**

解決済み

**Question**

`.jenkins/` の構成を、今回の変更範囲に含めますか。

**Context**

`ci.yml` のコメントは、ドキュメント生成を 1 つのレグが担当する考え方を「`.jenkins/build.sh` の `BUILD_DOCS` と同じ」と説明しています。GitHub Actions だけを分離すると、2 つの CI の構成が異なる状態になります。`.jenkins/build.sh` の内容は、まだ読んでいません。

**Answer**

選択方式: 単一選択  
回答者: Tetsuo Honda  
回答日時: 2026-11-11 06:08:06 JST

- [x] A: 含めない。GitHub Actions だけを変更し、`ci.yml` のコメントを実態に合わせる。
- [ ] B: 含める。Jenkins でも `make gen` を使う構成に合わせる。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: 現状の Jenkins は直列なのでこのままで良い

**Recommendation**

推奨: なし  
理由: Jenkins の運用状況と、並列実行の可否を把握していません。

**Assessment**

案 A で確定です。Jenkins は直列で実行しているため変更しません (ユーザーの補足)。`.jenkins/` は対象外とし、`ci.yml` のコメントを実態に合わせます。

### Q-009 受入の判定に CI の実走結果を含めるか

**Status**

解決済み

**Question**

成果物の受入の判定に、GitHub Actions の実走結果を含めますか。

**Context**

`ci.yml` の変更は、push して実走するまで正しさを確認できません。push はユーザーの個別の許可が必要な操作です。また、`make` と `make test` の実行確認はユーザーが担当する運用です。  
案 A では、エージェントは `bin_test/` のスクリプト テストと機械的検証までを行います。ユーザーが push した後、CI の実走で次の 3 点を確認してから第 3 ゲートに進みます。全ジョブが成功すること、発行した文書のページ数が分離前と一致すること、全体の所要時間が分離前より短いことです。  
案 B では、CI の実走を待たずに第 3 ゲートに進み、実走の確認は残課題として記録します。

**Answer**

選択方式: 単一選択  
回答者: Tetsuo Honda  
回答日時: 2026-11-11 06:08:06 JST

- [x] A: (推奨) 含める。CI の実走で 3 点を確認してから承認を求める。
- [ ] B: 含めない。実走の確認は残課題として記録する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 今回の目的は CI の所要時間の短縮であり、実走しない限り達成を確認できません。制約として、CI の 1 回の実走に 4 ～ 6 時間かかります。

**Related**

[Q-007](#q-007-cplat-の-prod-のビルド時間の測定方法)

**Assessment**

案 A で確定です。エージェントはスクリプト テストと機械的検証までを行います。ユーザーが push した後、CI の実走で、全ジョブの成功、文書のページ数の一致、全体の所要時間の短縮の 3 点を確認してから、第 3 ゲートに進みます。

### Q-010 Windows の make gen の検証方法

**Status**

解決済み

**Question**

Windows の `make gen` が正しく動作することを、どの方法で確認しますか。

**Context**

[Q-006](#q-006-windows-だけの生成物の扱い) の回答で、`.mc` の規則を分割し、Windows の生成物も `gen` の対象に含めることが決まりました。  
規則の分割そのものは、CI の Windows ジョブの通常ビルドで確認できます。一方、ゴール `gen` を指定した場合の動作は、CI のどのジョブも Windows では実行しません。[Q-005](#q-005-生成漏れの照合) の照合も Linux だけが対象です。  
makefw の `bin_test/` には、一時ディレクトリで make を実行して確認するテストの先例があります (`test_flex_bison_warnings.py`)。`bin_test/` のテストは、CI の全レグ (Windows を含む) の「Run script tests」で実行されます。

**Answer**

選択方式: 単一選択  
回答者: Tetsuo Honda  
回答日時: 2026-11-11 06:18:18 JST

- [x] A: (推奨) makefw の `bin_test/` に、ゴール `gen` のテストを追加する。flex / bison と `.mc` の生成、および通常ソースをコンパイルしないことを確認する。
- [ ] B: ユーザーが Windows の手元で `make gen` を実行して確認する。
- [ ] C: A と B の両方を行う。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: Windows の make gen はドキュメント生成には現状パイプラインでは寄与しないが、windows だけのプロジェクトなどで将来利用する可能性があり、退行を防ぐ必要はある。

**Recommendation**

推奨: A  
理由: Linux と Windows の両方の CI で継続して確認でき、将来のテンプレート変更による退行も検出できます。制約として、`.mc` の確認は mc.exe がある Windows のレグだけで実行し、Linux ではスキップの扱いを定める必要があります。

**Related**

[Q-005](#q-005-生成漏れの照合)、[Q-006](#q-006-windows-だけの生成物の扱い)、[Q-009](#q-009-受入の判定に-ci-の実走結果を含めるか)

**Assessment**

案 A で確定です。makefw の `bin_test/` に、ゴール `gen` のテストを追加します。Windows の `make gen` は、現状のパイプラインではドキュメント生成に寄与しませんが、Windows だけのプロジェクトで将来利用する可能性があるため、退行を防ぎます (ユーザーの補足)。  
`.mc` のテストは、理由が `[Windows]` で始まるスキップとして宣言します。`app/general/bin_internal/run-bin-tests.py` は、この形式のスキップを Linux では想定内として扱います。

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261111-ci-docs-job-split-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 3 (2026-11-11、残懸念なし)

最終仕様 (design.md) の合意版 (第 2 ゲート): 2026-11-11 に承認

更新予定の成果物: 最終仕様の「変更対象」に記載します。

受入条件: 最終仕様の「受入条件」に記載します。

## 反映と検証の記録

### 実装 (2026-11-11)

最終仕様の変更対象をすべて実装しました。最終仕様から変えた点は、次の 2 つです。どちらも最終仕様に反映済みです。

- `prod` 配下のゴール名を `gen` から `_gen` に変えました。末端のディレクトリでは、ターゲット名 `gen` が生成物の置き場所 `$(GENDIR)` を作る既存の規則と重なるためです。ルート、`app/`、app 直下のゴール名は `gen` のままです。
- 生成ソースの一覧を作るスクリプト `.github/scripts/list-generated-sources.sh` を追加しました。2 つのジョブが同じコマンドで一覧を作るためです。

サブエージェントは、codex CLI にテスト (`framework/makefw/bin_test/test_gen_goal.py`) の作成を、grok に実装のレビューを依頼しました。codex はサンドボックスの制約でテストを実行できなかったため、実行は claude code が行いました。

### 手元の検証 (Windows、2026-11-11)

- `test_gen_goal.py` の 4 件は成功しました。
- makefw の `bin_test/` 全体は、28 件中 26 件が成功しました。失敗した 2 件 (`test_ident_batch`) は `cc` コマンドを直接呼ぶテストで、この PC に `cc` がないことが原因です。今回変更したファイルを読み込まないテストであり、変更とは無関係です。
- 変更した Markdown は `text_style_jp.py --dry-run` で指摘なしです。
- 各 app の `makefile` は `update_template_makefiles.py` で同期しました (18 件)。
- `resolve_app_deps.sh --gen-build-apps` は cjson、cplat、openssl、struct-meta、zlib を出力しました。
- Linux での `make gen` の実行は手元では行っていません。GitHub Actions の実走で確認します。

### GitHub Actions の実走 (実行 42930377100、2026-11-11)

対象のコミットは、ルートが `2ca7a85c`、makefw が `7e17710` です。再コミットは発生していません。

| ジョブ | 結果 | 所要時間 |
| --- | --- | --- |
| `docs` | 成功 | 78[分] |
| `build-and-test-linux` (OL8) | 成功 | 177[分] |
| `build-and-test-linux` (OL9) | 成功 | 218[分] |
| `build-and-test-linux` (OL10) | 成功 | 120[分] |
| `build-and-test-windows` | 成功 | 202[分] |
| `verify-gen` | 成功 | 6[秒] |
| `warnings-summary` | 成功 | 4[秒] |
| `deploy-pages` | 成功 | 3[分] |

Table: 実行 42930377100 のジョブ別の結果

受入条件ごとの結果は、次のとおりです。

- 全ジョブが成功しました。
- 生成ソースの一覧は、OL8 のフル ビルド後と `make gen` 後のどちらも 15 件で、差分はありません (`verify-gen` が成功)。
- `make docs` が処理した doxybook2 の Markdown は 925 件で、分離前の実行 42821358874 と一覧が一致しました。
- 実行全体は 221[分] でした。同じ実行で分離しなかった場合は、OL8 のレグ 177[分] に `make doxy && make docs` の 71[分] が加わり、248[分] 以上になります。短縮は約 27[分] です。

`make gen` は 96[秒] で完了しました。事前の見積もり (25[分] 以下) より短い理由を、`linux-docs-gen.log` で確認しました。

- ビルドが必要な 5 app は `make -C prod` を実行し、合計 1355 件をコンパイルしています (openssl 1145、cplat 159、struct-meta 30、zlib 16、cjson 5)。
- cplat の `prod` のコンパイルとリンクは 172 件で、分離前のログから数えた件数と一致しました。分離前の cplat の 124[分] は、大半が `test` 側でした。
- 残りの 12 app は `make -C prod _gen` を実行し、コンパイルは 0 件です。

分離後は、最も長いジョブが OL9 のレグ (218[分]) に変わりました。これ以上の短縮には、テストのビルドと実行の時間を減らす必要があります。残課題一覧に記載しています。
