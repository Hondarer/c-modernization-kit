<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新エージェント: claude code / セッション ID: session_018UJwBxipLvskQMgge7hiBW
上流文書: `261022-test-assert-comment-followup3-design.md` (最終仕様)、`261022-test-assert-comment-followup3-discussion.md` (検討用 Markdown)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい検討用 Markdown から開始してください。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 残課題一覧 (バックログ)

ファイル名: `prompt/current/test-assert-comment-followup3-backlog.md`

## 案件の識別

案件名: リンク先の整形、注入しない呼び出しの診断、パラメーターの掲載 (test-assert-comment-followup3)  
作成日: 2026-10-23  
整理状態: 整理済み

この文書は、見送る決定がなされた問題点や、作業中に潜在または別問題として判明したが今回は対策しなかった最終的な残渣 (残課題) を記録したものです。作業期間中は気付いた時点で随時追記し、設計および実装・検証作業がすべて完了した後に、再構築して整理しています。作業中に解消した問題や、仕様・制約として容認した問題点は含みません。各項目はチケット管理システム (GitHub Issues、Jira など) への起票にそのまま利用できる詳細度・具体性で記載されています。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | ポインターをそのまま条件にする書き方の常設の点検 | 検討時見送り | 低 | 未着手 |
| BL-002 | 結果の生成に使われていない testfw のスクリプトの整理 | 作業中摘出 | 低 | 未着手 |
| BL-003 | testfw の変更が app 直下の make test の省略判定に反映されない | 作業中摘出 | 中 | 未着手 |

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

### 本案件での確認事項

- **摘出契機 (本案件)**: 前案件の残課題一覧の BL-002 を、本案件の検討用 Markdown の Q-001 で対象外とし、引き継ぎました (2026-10-23)。見送り理由は、CI コンテナー (別リポジトリ) と makefw の変更、CI の手順の追加を伴い、他の課題と独立した判断が多いためです。
- **導入状況 (2026-10-22 時点)**:
    - この環境と CI コンテナー (`/home/user/oracle-linux-container/src/Dockerfile`) には clang-tidy がありません。コンテナーは LLVM のソースから clang-format だけをビルドしています。
    - Windows の Visual Studio には `VC/Tools/Llvm/x64/bin/clang-tidy.exe` があります。
    - makefw には、`compile_commands.json` を作る仕組みがありません。

### 推奨される対応策

- **恒久対応案**: clang-tidy の `readability-implicit-bool-conversion` (`AllowPointerConditions: false`) を、Linux と Windows の両方の構成で、ビルドと同じコンパイル オプション (`compile_commands.json`) を使って実行する方針が考えられます。ビルドと同じオプションを使えば、上記の解析エラーと Windows の分岐の漏れが解消します。整数や bool の暗黙の変換も同じ検査で報告されるため、ポインター以外の指摘を抑止するオプションの調整が必要です。
- **参考情報**: 一時的な点検で検出して直した箇所は、`app/cplat/prod/libsrc/cplat/runtime/module.c` (3 か所)、`app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` (1 か所)、`framework/testfw/include/processController.h` と `framework/testfw/libsrc/test_com/processController*.cc`、`app/porter/test/src/integration/porterSendRecvTest/porterSendRecvTest.cc` です。

## BL-002 結果の生成に使われていない testfw のスクリプトの整理

### 概要と問題点

- **現状の事象**: `framework/testfw/bin_internal/` の次の 3 つのスクリプトは、テストの実行スクリプト (`exec_test_c_cpp.sh`、`exec_test_dotnet.sh`) のどちらからも呼び出されていません。テスト項目のサマリーとテスト コードの抽出は、`test_subprocedures.py` と `test_summary.py` が行っています。
    - `insert_summary_c_cpp.py`: 参照は、`exec_test_c_cpp.sh` の再生成判定用のハッシュの対象一覧、`bin_test/test_summary_counts.py` の 2 件のテスト、`app/general/docs/dotnet-test-results-design.md` のファイル一覧だけです。
    - `insert_summary_dotnet.py`: 参照は、`bin_test/test_summary_counts.py` の 2 件のテストと `dotnet-test-results-design.md` だけです。
    - `get_test_code_dotnet.py`: 参照は `dotnet-test-results-design.md` だけです。
- **本来あるべき姿**: 結果の生成に使うスクリプトだけが置かれ、読み手が実際の処理を誤認しないことが望まれます。

### 経緯と見送り理由

- **摘出契機**: 本案件で、使われていない `get_test_code_c_cpp.awk` を削除した際に、同じハッシュの対象一覧と `dotnet-test-results-design.md` のファイル一覧で気付きました (2026-10-23)。
- **今回の見送り理由**: 本案件で削除を合意したのは `get_test_code_c_cpp.awk` だけで、上記の 3 つは範囲外です。`test_summary_counts.py` のテストは、2 つのスクリプトの CLI を通じて `test_summary.py` の共通処理を検証しているため、削除する場合はテストの移し替えが必要です。

### 影響範囲とリスク

- **影響対象**: `framework/testfw/bin_internal/`、`framework/testfw/bin_test/test_summary_counts.py`、`app/general/docs/dotnet-test-results-design.md`。
- **放置した場合のリスク**: サマリーの規則を変える際に、使われていないスクリプトを直す、または直し忘れた側を正しいと誤認するおそれがあります。`insert_summary_c_cpp.py` の変更は、ハッシュの対象一覧を通じて、不要なエビデンスの再生成を引き起こします。

### 推奨される対応策

- **恒久対応案**: 3 つのスクリプトと、ハッシュの対象一覧の `insert_summary_c_cpp.py` を削除します。`test_summary_counts.py` の 2 件のテストは、`test_summary.py` の関数を直接呼ぶ形に移し替えます。`dotnet-test-results-design.md` のファイル一覧からも削除します。
- **参考情報**: 本案件で削除した `get_test_code_c_cpp.awk` と同じ扱いです。

## BL-003 testfw の変更が app 直下の make test の省略判定に反映されない

### 概要と問題点

- **現状の事象**: app 直下の `make test` は、ビルド署名 (`framework/makefw/bin_internal/resolve_app_deps.sh --signature`) が前回と一致すると、テストの実行を省略します。署名の入力 (`framework/makefw/docs/build-configurations.md` の「ビルド署名に含まれる入力」) には `framework/testfw` が含まれません。このため、testfw の結果の生成処理を変えても、ソースに変更がない app (本案件では `app/cjson`、`app/zlib`、`app/lua`、`app/sqlite`) のテストは再実行されず、結果の Markdown が古い書式のまま残ります。
- **本来あるべき姿**: テスト ディレクトリ単位の省略判定 (`exec_test_c_cpp.sh` のハッシュの対象一覧) は、「集計処理を変更した場合も古いエビデンスを再利用しない」ことを意図しています。app 直下の省略判定も、結果の生成処理の変更で無効になることが望まれます。

### 経緯と見送り理由

- **摘出契機**: 本案件の受入確認で、リポジトリ ルートの `make test` を実行した際に、上記の 4 つの app で `INFO: Skipping test (dependencies are unchanged and clean)` が出力されました (2026-10-23)。
- **今回の見送り理由**: makefw の署名の仕様の変更で、本案件の範囲外です。署名に testfw を含めると、testfw の変更のたびに全 app のテストが再実行されるため、対象とするファイルの選定に別途判断が必要です。

### 影響範囲とリスク

- **影響対象**: `framework/makefw/bin_internal/resolve_app_deps.sh`、`framework/makefw/docs/build-configurations.md`、テスト結果を参照する利用者。
- **放置した場合のリスク**: testfw の変更後に、app によって結果の Markdown の書式や判定が混在します。testfw の変更が既存のテストを失敗させる場合も、省略された app では検出されません。

### 推奨される対応策

- **恒久対応案**: `make test` の署名 (`test` モード) に、`exec_test_c_cpp.sh` のハッシュの対象一覧と同じ testfw のスクリプト群を加えます。対象の一覧は 1 か所で定義し、両方の判定で共有します。
- **参考情報**: `framework/testfw/bin_internal/exec_test_c_cpp.sh` の「集計処理を変更した場合も古いエビデンスを再利用しない」のハッシュの対象一覧。
