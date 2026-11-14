<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新エージェント: claude code / セッション ID: e1665a31-f98a-4cbe-aff5-beff19ab41c0
上流文書: [261114-doxygen-mermaid-migration-design.md](261114-doxygen-mermaid-migration-design.md) (最終仕様)、[261114-doxygen-mermaid-migration-discussion.md](261114-doxygen-mermaid-migration-discussion.md) (検討用 Markdown)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい検討用 Markdown から開始してください。継続作業によってこのファイルの残課題を対策する場合は、このファイルや各項目の記載を削除することなく、各項目の状態および経緯と対処を更新することを原則とします。また、フェーズ分けした対策で対処ができていない項目がある場合は、当該継続作業の backlog にも必ず転記し、最新作業の backlog を見れば残件が把握できるようにしてください。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# Doxygen 由来の PlantUML 図を Mermaid へ移行する 残課題一覧

## 案件の識別

案件名: `doxygen-mermaid-migration-20261114`  
作成日: `2026-11-15`  
整理状態: 整理済み

この文書は、Doxygen 由来の図を Mermaid へ移行する作業で、今回は対策しなかった残課題を記録したものです。当該作業中に解消した問題と、仕様として容認した事項 (Doxygen HTML の Mermaid 図が閲覧時の CDN 接続に依存する点、継承グラフの矢印の形が通常の矢印になる点) は含めていません。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| [BL-001](#bl-001-appcplat-以外の-app-と-windows-での生成結果が未確認) | `app/cplat` 以外の app と Windows での生成結果が未確認 | 検討時見送り | 中 | 未着手 |
| [BL-002](#bl-002-doxyfw-の既存単体テストが-encodingwarning-を出す) | doxyfw の既存単体テストが EncodingWarning を出す | 作業中摘出 | 低 | 未着手 |
| [BL-003](#bl-003-check-doxygen-indentpy-の修正モードが改行コードを明示していない) | `check-doxygen-indent.py` の修正モードが改行コードを明示していない | 作業中摘出 | 低 | 未着手 |
| [BL-004](#bl-004-インクルード-グラフの見出し名が語の意味と逆に読める) | インクルード グラフの見出し名が語の意味と逆に読める | 作業中摘出 | 低 | 未着手 |

Table: 残課題の一覧

## BL-001 app/cplat 以外の app と Windows での生成結果が未確認

### 概要と問題点

- **現状の事象**: 変更後の `make doxy` による生成結果の確認は、Linux 上の `app/cplat` だけで行いました。`framework/doxyfw` の変更は Doxygen を利用するすべての app の生成結果に影響しますが、`app/cplat` 以外の app と Windows 上の生成結果は確認していません。次の経路は、`app/cplat` の実データでは通過していません。
    - 継承グラフ (`flowchart BT`)。`app/cplat` は C のライブラリで、継承関係を持ちません。
    - 記号 (`<`、`>`、`*`、`[`、`]`、`~` など) を含む名前のキャプションの数値文字参照への変換。`app/cplat` の生成結果には該当するキャプションがありませんでした。
- **本来あるべき姿**: Doxygen を利用するすべての app で、Linux と Windows の双方について、生成された Markdown の図が ```` ```mermaid ```` と `Figure:` 行で出力され、警告ファイルに新しい警告がないことが確認されている状態です。

### 経緯と見送り理由

- **摘出契機**: 前提検討時に、検証する app の範囲を代表の単一 app (`app/cplat`) と決定しました。
- **今回の見送り理由**: 複数 app の再生成は、1 app あたり数分の実行時間がかかる全体テストに当たるためです。未通過の経路は、単体テスト (`test_extract_graphs.py`、`test_preprocess.py`) と `mmdc` による描画で確認しています。

### 影響範囲とリスク

- **影響対象**: `app/cplat` 以外で Doxygen を利用する app の `docs/doxybook2*` と警告ファイル、および CI の Windows ジョブです。
- **放置した場合のリスク**: C++ や .NET のコードを持つ app で、想定していない名前やラベルの文字により Mermaid の構文エラーやキャプションの表示崩れが起きても、発行された文書を目視するまで気付けません。

### 推奨される対応策

- **恒久対応案**: この変更を取り込んだ CI の実行結果で、Linux と Windows の `docs-warns` に `Text tag "mermaid" not recognised` や新しい警告がないことを確認します。あわせて、継承関係を持つ app (C++ または .NET のコードを含む app) の生成 Markdown から継承グラフを 1 つ選び、描画結果を確認します。
- **参考情報**: 最終仕様書の「検証手順」と「受入条件」、`framework/doxyfw/docs/doxybook2-mermaid.md`。

## BL-002 doxyfw の既存単体テストが EncodingWarning を出す

### 概要と問題点

- **現状の事象**: `python3 -X warn_default_encoding -m unittest` で実行すると、次の既存テストが `EncodingWarning: 'encoding' argument not specified` を出します。テスト自体は成功します。
    - `framework/doxyfw/bin_test/test_doxybook2_templates.py` の L73 と L82
    - `framework/doxyfw/bin_test/test_materialize_group_members.py` の L301 と L316
- **本来あるべき姿**: [Python の文字コード指定規範](../../python-text-encoding-guideline.md) に従い、テキスト入出力と `subprocess` の呼び出しで文字コードを明示し、警告が出ない状態です。

### 経緯と見送り理由

- **摘出契機**: 変更後の単体テストを `-X warn_default_encoding` 付きで実行した際に発見しました。変更前から存在する警告です。
- **今回の見送り理由**: 今回の変更で追加・更新したテスト (`test_preprocess.py`、`test_postprocess.py`、`test_extract_graphs.py`、`test_check_doxygen_indent.py`) は警告を出しておらず、該当箇所は今回の変更範囲外であるためです。

### 影響範囲とリスク

- **影響対象**: 上記 2 つのテスト ファイルです。
- **放置した場合のリスク**: 既定の文字コードが UTF-8 でない Windows 環境で、非 ASCII 文字を含む出力の読み取りに失敗する可能性があります。

### 推奨される対応策

- **恒久対応案**: 該当行の `subprocess.run` または `open` へ `encoding="utf-8"` を指定します。修正後、`-X warn_default_encoding` 付きの実行で警告が出ないことを確認します。
- **参考情報**: [Python の文字コード指定規範](../../python-text-encoding-guideline.md)。

## BL-003 check-doxygen-indent.py の修正モードが改行コードを明示していない

### 概要と問題点

- **現状の事象**: `framework/doxyfw/bin/check-doxygen-indent.py` の修正モード (`--fix`) は、修正後の内容を `open(..., 'w', encoding='utf-8')` で書き込み、`newline` を指定していません。Windows で実行すると、LF のファイルが CRLF で書き戻される可能性があります。
- **本来あるべき姿**: 書き込み時に `newline` を明示し、実行環境によらず元のファイルの改行コードを保つ状態です。

### 経緯と見送り理由

- **摘出契機**: 図ブロックの字下げ検査を追加する際に、既存の書き込み処理で発見しました。
- **今回の見送り理由**: 今回追加した検査は検出だけを行い、ファイルを書き換えません。既存の修正モードの挙動は変更しない仕様であるためです。

### 影響範囲とリスク

- **影響対象**: Windows で `check-doxygen-indent.py --fix` を実行した場合の、修正対象のヘッダー ファイルです。
- **放置した場合のリスク**: 字下げの修正と同時にファイル全体の改行コードが変わり、差分が全行に及びます。

### 推奨される対応策

- **恒久対応案**: 読み込み時に `newline=''` で元の改行コードを保持し、書き込み時も `newline=''` または `newline='\n'` を指定します。CRLF と LF の双方の入力に対する単体テストを `test_check_doxygen_indent.py` へ追加します。
- **参考情報**: [Python の文字コード指定規範](../../python-text-encoding-guideline.md)。

## BL-004 インクルード グラフの見出し名が語の意味と逆に読める

### 概要と問題点

- **現状の事象**: `framework/doxyfw/templates/extract-graphs.py` は、インクルード関係の 2 種類のグラフに次の見出しを付けます。見出しは図のキャプション (`<名前> の<見出し>`) にも使われます。
    - `incdepgraph` (対象のファイルがインクルードするファイルのグラフ): 「インクルード元」。キャプションの例は `calculator.c のインクルード元` です。
    - `invincdepgraph` (対象のファイルをインクルードするファイルのグラフ): 「インクルード先」。キャプションの例は `calculator.h のインクルード先` です。

    「インクルード元」は「対象をインクルードしている側のファイル」、「インクルード先」は「対象がインクルードしているファイル」とも読めます。この読み方では、見出しとグラフの内容が逆になります。
- **本来あるべき姿**: 見出しとキャプションだけで、グラフが示す関係の向きを一意に読み取れる状態です。

### 経緯と見送り理由

- **摘出契機**: `framework/doxyfw/docs/extract-graphs.md` を Mermaid 出力に合わせて書き直す際に、見出し名とグラフの内容の対応を確認して発見しました。変更前の PlantUML 出力から使われている名称です。
- **今回の見送り理由**: 今回の作業は図の記法の移行が目的で、見出し名は変更前の名称を維持する仕様であるためです。見出し名を変えると、Doxygen を利用するすべての app の生成 Markdown で見出しとキャプションが変わります。

### 影響範囲とリスク

- **影響対象**: `extract-graphs.py` の見出し名、`framework/doxyfw/docs/extract-graphs.md` の見出しとキャプションの一覧、`bin_test/test_extract_graphs.py` の期待値、および各 app の `docs/doxybook2*` に生成される見出しとキャプションです。
- **放置した場合のリスク**: 読者が依存の向きを逆に理解する可能性があります。図の矢印の向きから正しい関係は読み取れるため、生成結果そのものは誤っていません。

### 推奨される対応策

- **恒久対応案**: まず、現在の名称が意図したものか、名称を変更するかを決めます。変更する場合は、向きを一意に読み取れる名称 (たとえば `incdepgraph` を「インクルードするファイル」、`invincdepgraph` を「インクルードされるファイル」) を選び、`extract-graphs.py`、`extract-graphs.md`、`test_extract_graphs.py` を同時に更新して、各 app の `make doxy` で生成結果を更新します。
- **参考情報**: `framework/doxyfw/templates/extract-graphs.py` の見出しを設定する箇所 (`heading = 'インクルード元'`、`heading = 'インクルード先'`)、`framework/doxyfw/docs/extract-graphs.md` の見出しとキャプションの表。
