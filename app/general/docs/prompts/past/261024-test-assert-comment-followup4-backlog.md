<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新エージェント: claude code / セッション ID: session_018UJwBxipLvskQMgge7hiBW
上流文書: `261024-test-assert-comment-followup4-design.md` (最終仕様)、`261024-test-assert-comment-followup4-discussion.md` (検討用 Markdown)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい検討用 Markdown から開始してください。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 残課題一覧 (バックログ)

ファイル名: `prompt/current/test-assert-comment-followup4-backlog.md`

## 案件の識別

案件名: 未使用スクリプトの整理と、テストの省略判定への testfw の反映 (test-assert-comment-followup4)  
作成日: 2026-10-24  
整理状態: 整理済み

この文書は、見送る決定がなされた問題点や、作業中に潜在または別問題として判明したが今回は対策しなかった最終的な残渣 (残課題) を記録したものです。作業期間中は気付いた時点で随時追記し、設計および実装・検証作業がすべて完了した後に、再構築して整理しています。作業中に解消した問題や、仕様・制約として容認した問題点は含みません。各項目はチケット管理システム (GitHub Issues、Jira など) への起票にそのまま利用できる詳細度・具体性で記載されています。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | ポインターをそのまま条件にする書き方の常設の点検 | 検討時見送り | 低 | 未着手 |
| BL-002 | makefw の変更が app 直下の make の省略判定に反映されない | 検討時見送り | 中 | 未着手 |
| BL-003 | 依存パス解決の自己テスト (app_paths_selftest.py) が実行できない | 作業中摘出 | 低 | 未着手 |

## BL-001 ポインターをそのまま条件にする書き方の常設の点検

### 概要と問題点

- **現状の事象**: コーディング規範 (`app/general/docs/coding-guideline.md` の空ポインターの表記) は、ポインターとスマート ポインターを `if (p)` や `if (!p)` のようにそのまま条件にせず、`p != NULL` (C) や `p == nullptr` (C++) と比較して書くことを求めています。しかし、この規範からの逸脱を常に検出する仕組みはありません。条件式の値がポインターかどうかは型を解析しないと判定できず、正規表現による点検では整数や bool と区別できないためです。
- **本来あるべき姿**: 新たな逸脱が CI などで機械的に検出されることが望まれます。

### 経緯と見送り理由

- **摘出契機**: 最終仕様の検討で、型を考慮した点検の方式を確認しました。
- **今回の見送り理由**: ユーザーの判断により、点検の仕組みは新設せず、既存の逸脱だけを一時的な点検で 1 回だけ探して直しました。一時的な点検は、libclang で Linux の構成 (`PLATFORM_LINUX`) の翻訳単位 1076 個を解析し、`if`、`while`、`for`、`?:`、`!`、`&&`、`||` の被演算子がポインター型かスマート ポインター型である箇所を報告するものです。この点検には次の限界があり、検出の漏れが残っている可能性があります。
    - 次の 12 ファイルは、ビルド時だけに作られるシンボリック リンクや、ビルド時だけに定義されるマクロに依存するため、解析エラーを含みます。エラーの箇所より後の構文は解析されていない可能性があります (正規表現で目視の点検は行いました)。
        - `app/cplat/test/src/libcplatTest/exportTest/exportTest.cc`
        - `app/cplat/test/src/libcplatTest/hashtable/hashtableTest/hashtable_inject.c`
        - `app/cplat/test/src/libcplatTest/string_catalog/` 配下の 7 ファイル (`stringCatalogArgumentTest.cc`、`stringCatalogFilterPatternCompileTest.cc`、`stringCatalogFilterStructureTest.cc`、`stringCatalogFilterApplyTest.cc`、`stringCatalogFilterSourceTest.cc`、`stringCatalogRenderTest.cc`、`stringCatalogValidateTest.cc`)
        - `app/zlib/test/libsrc/mock_zlib/mock_zlib.cc`、`app/zlib/test/src/cmd/zlibTest/zlibTest.cc`、`app/zlib/test/src/libzlibTest/exportTest/exportTest.cc`
    - Windows だけでコンパイルされるコード (`PLATFORM_WINDOWS` の分岐と `*_windows.c` など) は、libclang では解析していません。正規表現で単純な名前の条件だけを抽出し、目視で点検しました。関数の戻り値や配列の要素を条件にする書き方は、この点検の対象外です。

### 影響範囲とリスク

- **影響対象**: 第一者管理の C/C++ のコード全体 (製品コード、テスト コード、testfw)。
- **放置した場合のリスク**: 規範に反する書き方が新たに混入しても、レビューでしか気付けません。動作には影響しないため、気付かれないまま増えるおそれがあります。

### 引き継ぎの経緯

- **引き継ぎ (2 回目)**: `test-assert-comment-followup3-backlog.md` の BL-001 を、`test-assert-comment-followup4-discussion.md` の Q-001 で対象外とし、引き継ぎました (2026-10-24)。見送り理由は、CI コンテナー (別リポジトリ) での clang-tidy のビルドとイメージの再発行、makefw の変更、CI の手順の追加を伴い、他の課題と独立した判断が多いためです。
- **引き継ぎ (1 回目)**: `test-assert-comment-followup2-backlog.md` の BL-002 を、`test-assert-comment-followup3-discussion.md` の Q-001 で対象外とし、引き継ぎました (2026-10-23)。
- **導入状況 (2026-10-22 時点)**:
    - この環境と CI コンテナー (`/home/user/oracle-linux-container/src/Dockerfile`) には clang-tidy がありません。コンテナーは LLVM のソースから clang-format だけをビルドしています。
    - Windows の Visual Studio には `VC/Tools/Llvm/x64/bin/clang-tidy.exe` があります。
    - makefw には、`compile_commands.json` を作る仕組みがありません。

### 推奨される対応策

- **恒久対応案**: clang-tidy の `readability-implicit-bool-conversion` (`AllowPointerConditions: false`) を、Linux と Windows の両方の構成で、ビルドと同じコンパイル オプション (`compile_commands.json`) を使って実行する方針が考えられます。ビルドと同じオプションを使えば、上記の解析エラーと Windows の分岐の漏れが解消します。整数や bool の暗黙の変換も同じ検査で報告されるため、ポインター以外の指摘を抑止するオプションの調整が必要です。
- **参考情報**: 一時的な点検で検出して直した箇所は、`app/cplat/prod/libsrc/cplat/runtime/module.c` (3 か所)、`app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` (1 か所)、`framework/testfw/include/processController.h` と `framework/testfw/libsrc/test_com/processController*.cc`、`app/porter/test/src/integration/porterSendRecvTest/porterSendRecvTest.cc` です。

## BL-002 makefw の変更が app 直下の make の省略判定に反映されない

### 概要と問題点

- **現状の事象**: app 直下の `make` と `make test` は、ビルド署名 (`framework/makefw/bin_internal/resolve_app_deps.sh --signature`) が前回と一致すると、サブディレクトリへの再帰を省略します。署名の入力に makefw のテンプレート (`framework/makefw/makefiles/*.mk`) とスクリプト (`framework/makefw/bin_internal/`) が含まれないため、コンパイル オプションやリンクの規則を変えても、ソースに変更のない app はビルドもテストも行いません。
- **本来あるべき姿**: ビルドの規則を変えた場合は、影響する app のビルドとテストが再実行されることが望まれます。

### 経緯と見送り理由

- **摘出契機**: 本案件の検討で、testfw の変更が省略判定に反映されない問題 (前案件の BL-003) を調べた際に、makefw にも同じ問題があることを確認しました (2026-10-24)。
- **今回の見送り理由**: 本案件の検討用 Markdown の Q-007 で、範囲外としました。makefw の変更は製品のビルドにも影響し、対象のファイルの選定 (`makefiles/*.mk` だけか、`bin_internal/` のスクリプトも含めるか、署名の計算自体の `resolve_app_deps.sh` を含めるか) に独立した判断が必要なためです。

### 影響範囲とリスク

- **影響対象**: `framework/makefw/bin_internal/resolve_app_deps.sh`、`framework/makefw/docs/build-configurations.md`、開発環境で `make` を繰り返す開発者。CI は毎回スタンプのない状態からビルドするため影響を受けません。
- **放置した場合のリスク**: makefw の変更後に、開発環境では古い規則でビルドされた成果物が残り、警告や失敗を見落とします。変更の確認には、`clean` や `MAKEFW_TEST_FORCE=1` を意識して使う必要があります。

### 推奨される対応策

- **恒久対応案**: `build` と `test` の署名に、makefw のテンプレートと、ビルドとテストの手順に使うスクリプトを加えます。対象の選定では、開発環境の補助スクリプト (VS Code の環境変数の同期など) を除くことを検討します。
- **参考情報**: 本案件で testfw の入力を署名に加えた実装 (`resolve_app_deps.sh` の `collect_testfw_signature_files`)。

## BL-003 依存パス解決の自己テスト (app_paths_selftest.py) が実行できない

### 概要と問題点

- **現状の事象**: `framework/makefw/bin_internal/app_paths_selftest.py` を実行すると、次のエラーで失敗します。一時ワークスペースに `__each_app_template.mk` だけをコピーしており、このテンプレートが読み込む `makefiles/_path_functions.mk` (2026-10-01 の `07375a3` で追加) がないためです。

    ```text
    make: *** ターゲット '/tmp/makefw-paths-XXXX/framework/makefw/makefiles/_path_functions.mk' を make するルールがありません.  中止.
    ```

- このテストは `bin_internal/` に置かれているため、`make bin-test` (`app/general/bin_internal/run-bin-tests.py`) の対象にならず、CI でも実行されていません。スクリプトのテストを `bin_test/` に置く規則 (リポジトリ ルートの `AGENTS.md`) からも外れています。
- `framework/makefw/docs/build-configurations.md` の「ビルド署名に含まれる入力」は、依存パスの解決のタイミングを、このテストで局所確認するよう案内しています。
- **本来あるべき姿**: テストが `bin_test/` に置かれて CI で実行され、テンプレートが読み込む make ファイルの追加に追随していることが望まれます。

### 経緯と見送り理由

- **摘出契機**: 本案件の受入確認で、署名の変更の局所確認として実行した際に失敗しました (2026-10-24)。テストは署名の計算を偽の `resolve_app_deps.sh` に置き換えるため、本案件の変更とは関係しません。
- **今回の見送り理由**: 本案件の範囲 (未使用スクリプトの整理と、省略判定への testfw の反映) の外の、既存の不具合です。署名の計算の確認は、新設した `framework/makefw/bin_test/test_build_signature.py` で行いました。

### 影響範囲とリスク

- **影響対象**: `framework/makefw/bin_internal/app_paths_selftest.py`、`framework/makefw/docs/build-configurations.md` の案内。
- **放置した場合のリスク**: 依存パスの解決のタイミング、子 make への継承、Windows のパスの一括変換が退行しても検出されません。文書の案内どおりに実行した開発者が、失敗の原因を自分の変更と誤認するおそれがあります。

### 推奨される対応策

- **恒久対応案**: テストを `framework/makefw/bin_test/` へ移し、一時ワークスペースへ `makefiles/` 全体をコピーする (`bin_test/test_path_spaces.py` と同じ方式) ように直します。`build-configurations.md` の案内も移動先に合わせます。
- **参考情報**: `framework/makefw/bin_test/test_path_spaces.py` の `setUp()` (`makefiles` と `bin_internal` を一時ワークスペースへコピー)。
