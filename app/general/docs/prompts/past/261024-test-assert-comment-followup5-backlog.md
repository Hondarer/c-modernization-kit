<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新エージェント: claude code / セッション ID: session_018UJwBxipLvskQMgge7hiBW
上流文書: `261024-test-assert-comment-followup5-design.md` (最終仕様)、`261024-test-assert-comment-followup5-discussion.md` (検討用 Markdown)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい検討用 Markdown から開始してください。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 残課題一覧 (バックログ)

ファイル名: `prompt/current/test-assert-comment-followup5-backlog.md`

## 案件の識別

案件名: makefw の変更の省略判定への反映と、依存パス解決の自己テストの修復 (test-assert-comment-followup5)  
作成日: 2026-10-24  
整理状態: 整理済み

この文書は、見送る決定がなされた問題点や、作業中に潜在または別問題として判明したが今回は対策しなかった最終的な残渣 (残課題) を記録したものです。作業期間中は気付いた時点で随時追記し、設計および実装・検証作業がすべて完了した後に、再構築して整理しています。作業中に解消した問題や、仕様・制約として容認した問題点は含みません。各項目はチケット管理システム (GitHub Issues、Jira など) への起票にそのまま利用できる詳細度・具体性で記載されています。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | ポインターをそのまま条件にする書き方の常設の点検 | 検討時見送り | 低 | 実施しない (2026-10-25、機械的な点検は過剰とするユーザーの判断。`test-assert-comment-followup6-discussion.md` を中止) |
| BL-002 | ビルド署名の文書に経緯の記述が残っている | 作業中摘出 | 低 | 解消 (2026-10-25、案件の完了後の通常の対話で対策) |
| BL-003 | テスト ディレクトリ単位の省略判定の入力の説明が実装と一致しない | 作業中摘出 | 低 | 解消 (2026-10-25、案件の完了後の通常の対話で対策) |

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

- **引き継ぎ (3 回目)**: `test-assert-comment-followup4-backlog.md` の BL-001 を、`test-assert-comment-followup5-discussion.md` の Q-001 で対象外とし、引き継ぎました (2026-10-24)。検出方式、ツールの入手方法、コンパイル オプションの渡し方は、ユーザーの回答「今回は見送り」により決めていません。
- **方式の調査 (2026-10-24 時点)**:
    - libclang は、PyPI のホイール (`pip install libclang`、最新 18.1.1) で Linux と Windows のどちらにも導入でき、CI コンテナーの変更は不要です。
    - clang-tidy の `readability-implicit-bool-conversion` は、スマート ポインターの `if (p)` を報告しません (`explicit operator bool` の呼び出しになるため)。規範の対象と検出範囲を一致させるには、libclang の点検スクリプトのほうが適します。
    - 同じ検討で、点検の方式を決めた場合の付随事項として次の回答を得ています。取り組む際の出発点にできます: 点検スクリプトは `framework/makefw/bin_internal/` に置く、Linux と Windows の両方で実行する、対象は第一者管理のコード (外部由来のコードを除く)、指摘は `.warn` に記録して CI では警告にする、CI の Linux は Oracle Linux 8 だけで実行する (Jenkins も同じ考え方)。
- **引き継ぎ (2 回目)**: `test-assert-comment-followup3-backlog.md` の BL-001 を、`test-assert-comment-followup4-discussion.md` の Q-001 で対象外とし、引き継ぎました (2026-10-24)。見送り理由は、CI コンテナー (別リポジトリ) での clang-tidy のビルドとイメージの再発行、makefw の変更、CI の手順の追加を伴い、他の課題と独立した判断が多いためです。
- **引き継ぎ (1 回目)**: `test-assert-comment-followup2-backlog.md` の BL-002 を、`test-assert-comment-followup3-discussion.md` の Q-001 で対象外とし、引き継ぎました (2026-10-23)。
- **導入状況 (2026-10-22 時点)**:
    - この環境と CI コンテナー (`/home/user/oracle-linux-container/src/Dockerfile`) には clang-tidy がありません。コンテナーは LLVM のソースから clang-format だけをビルドしています。
    - Windows の Visual Studio には `VC/Tools/Llvm/x64/bin/clang-tidy.exe` があります。
    - makefw には、`compile_commands.json` を作る仕組みがありません。

### 推奨される対応策

- **恒久対応案**: libclang (Python) の点検スクリプトを、makefw の点検用の make ターゲットから、各ディレクトリのソース、インクルード パス、マクロ定義を渡して実行する方針を推奨します。ビルドの後に実行すれば、ビルド時だけに作られるシンボリック リンクがそろった状態で解析できます。別案として、clang-tidy の `readability-implicit-bool-conversion` (`AllowPointerConditions: false`) を、Linux と Windows の両方の構成で、ビルドと同じコンパイル オプション (`compile_commands.json`) を使って実行する方針が考えられます。ビルドと同じオプションを使えば、上記の解析エラーと Windows の分岐の漏れが解消します。整数や bool の暗黙の変換も同じ検査で報告されるため、ポインター以外の指摘を抑止するオプションの調整が必要です。
- **参考情報**: 一時的な点検で検出して直した箇所は、`app/cplat/prod/libsrc/cplat/runtime/module.c` (3 か所)、`app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` (1 か所)、`framework/testfw/include/processController.h` と `framework/testfw/libsrc/test_com/processController*.cc`、`app/porter/test/src/integration/porterSendRecvTest/porterSendRecvTest.cc` です。

## BL-002 ビルド署名の文書に経緯の記述が残っている

### 概要と問題点

- **現状の事象**: `framework/makefw/docs/build-configurations.md` の「ビルド署名に含まれる入力」に、外来 OSS を取り込む app の入力 (`patches/`、`packages/`、`bin_internal/`) について、「これら 3 つの入力そのものが変化してもビルド署名には反映されない、という欠陥がかつて存在しました。」という経緯の記述があります。
- **本来あるべき姿**: 正本文書は最終仕様だけを記載し、経緯を含めないことが望まれます。入力を署名に含める理由は、「サブディレクトリへの再帰を省くと、`$(shell)` で行う展開とパッチの適用も行われないため」のように、現在の仕組みの説明として書くことが望まれます。

### 経緯と見送り理由

- **摘出契機**: 本案件で、同じ節に makefw の入力を書き加えるために節を読んだ際に気付きました (2026-10-24)。
- **今回の見送り理由**: 本案件の合意範囲 (makefw の変更の省略判定への反映、依存パス解決の自己テストの修復、コメントの整形の指摘 2 件) に含まれないためです。

### 影響範囲とリスク

- **影響対象**: `framework/makefw/docs/build-configurations.md` の 1 文。動作への影響はありません。
- **放置した場合のリスク**: 読者が、現在も欠陥が残っているのか、すでに対策済みなのかを読み取りにくくなります。経緯の記述が他の文書の手本として広がるおそれがあります。

### 推奨される対応策

- **恒久対応案**: 該当の 1 文を、現在の仕組みの説明 (再帰の省略で展開とパッチの適用も行われないため、取り込みの入力を署名に含める) に書き換えます。変更後は `text_style_jp.py --dry-run` で確認します。

## BL-003 テスト ディレクトリ単位の省略判定の入力の説明が実装と一致しない

### 概要と問題点

- **現状の事象**: テスト ディレクトリ単位の省略判定 (`framework/testfw/bin_internal/exec_test_c_cpp.sh` の `compute_test_signature`) の入力には、テスト ソース、`makepart.mk` と `makelocal.mk`、リンクするライブラリ、共通処理の抜粋元、testfw のスクリプト、makefw の入力が含まれます。しかし、次の 2 か所の説明は、入力の一部だけを挙げています。
    - `compute_test_signature` の前のコメント: `TEST_SRCS`、`ADD_SRCS`、`makepart.mk`、`makelocal.mk` だけを挙げています。
    - `framework/testfw/docs/how-to-test.md` の「再テストのスキップ」の、テスト対象ディレクトリ単位の判定の段落: リンクするライブラリと共通処理の抜粋元を挙げていません (testfw のスクリプトと makefw の入力は、同じ節の前の段落に記載があります)。
- **本来あるべき姿**: 説明が、判定の入力をすべて挙げることが望まれます。

### 経緯と見送り理由

- **摘出契機**: 本案件で、`compute_test_signature` に makefw の入力を加える際に気付きました (2026-10-24)。
- **今回の見送り理由**: 本案件の合意範囲に含まれないためです。本案件では、追加した makefw の入力の説明だけを加えました。

### 影響範囲とリスク

- **影響対象**: `framework/testfw/bin_internal/exec_test_c_cpp.sh` のコメント、`framework/testfw/docs/how-to-test.md`。動作への影響はありません。
- **放置した場合のリスク**: 判定の入力を変えるときに、説明だけを読んで既存の入力を見落とすおそれがあります。

### 推奨される対応策

- **恒久対応案**: コメントと文書に、判定の入力を種類ごとに列挙します。入力の追加のたびに説明がずれないよう、コメントは種類の列挙にとどめ、範囲の詳細は各一覧のスクリプト (`list_signature_scripts.sh`、`list_signature_inputs.sh`) の説明を参照する形にします。
