# ワークスペース作業ガイド

## 対象

この文書は、統合ワークスペースの作業手順を示します。  
個別 framework や app の詳細は、それぞれの AGENTS.md と README.md を参照してください。

## Git の扱い

ユーザーから個別に指示されるまで、ステージング、コミット、アンステージ、スタッシュを行いません。  
サブモジュールを変更するときは、サブモジュール内の差分と、親リポジトリから見たサブモジュール状態を分けて確認します。

サブモジュール境界が影響するのは、コミット単位と作業ツリーの扱いだけです。  
`AGENTS.md` が適用される範囲はディレクトリ階層で決まるため、サブモジュール境界の影響を受けません。

`app/general/docs/prompts/` 配下だけを変更するコミットは、コミット メッセージに `[skip ci]` を含めます。  
検討の記録はビルド、テスト、公開する成果物に影響せず、CI の 1 回の実行に数時間かかるためです。  
GitHub Actions は、push に含まれる最新のコミットのメッセージで起動を判定します。ほかの変更を含むコミットと一緒に push する場合は、CI が起動します。  
`[skip ci]` を付けたコミットの内容は、次に CI が動いた時点で GitHub Pages に反映されます。  
see: [Skipping workflow runs](https://docs.github.com/en/actions/managing-workflow-runs-and-deployments/managing-workflow-runs/skipping-workflow-runs)

## 主要コマンド

```bash
make
make test
make bin-test
make gen
make doxy
make docs
make livedocs
make servedocs
make skills
make sync-app-env
```

`make docs` は Pandoc による静的発行、`make livedocs` と `make servedocs` は MkDocs による動的発行です。  
`make servedocs` はローカル配信を開始し、`make stopdocs` で停止します。  
詳細は [動的発行 (MkDocs)](../../../framework/docsfw/livedocs/README.md) を参照してください。

`make gen` は、`prod/` 配下の生成ソースだけを作ります。  
Doxygen の入力には `gen/` 配下の生成ソースが含まれるため、フル ビルドをしていない作業ツリーで文書を生成する場合は、`make gen && make doxy && make docs` の順に実行します。  
`make doxy` は `make gen` を自動では実行しません。詳細は [生成ソースだけを作る (make gen)](../../../framework/makefw/docs/build-configurations.md#生成ソースだけを作る-make-gen) を参照してください。

GitHub Actions では、ドキュメント生成を `docs` ジョブが担当し、ビルドとテストのジョブに並行して実行します。  
`docs` ジョブは `make gen` で生成ソースをそろえてから、`make doxy && make docs` を実行します。  
`verify-gen` ジョブは、フル ビルド後の生成ソースの一覧と `make gen` 後の一覧を照合し、差分があれば失敗して文書の公開を止めます。  
生成器を使う app を追加して照合が失敗した場合は、その app の `appdeps.mk` に `APP_GEN_REQUIRES_BUILD := 1` を宣言します。  
Jenkins はジョブを直列に実行するため、ビルドとテストの後に同じレグでドキュメントを生成します。

app を追加または削除した場合は、`.vscode`、GitHub Actions、Jenkins の環境変数を手動で編集せず、`make sync-app-env` を実行します。  
詳細は [VS Code 環境変数](vscode-variables.md) を参照してください。

## ビルドとテスト

振る舞い、ABI、ビルド構成を変更した場合は、影響する局所テストを選びます。  
app 全体の確認が必要な場合は、その app 直下で `make test` を実行します。  
コメントや文書だけの変更では、記法や生成結果への影響に応じて確認方法を選びます。  
依存ファイルの内容変更だけで `make clean` を実行する必要はありません。

全体テストが必要な場合は、所要時間と必要性をユーザーへ説明し、確認を得てから実行します。  
許可された局所検証は、変更起因の失敗を修正して影響範囲を再検証するところまで進めます。  
同じ変更で得た結果は再利用し、既存の無関係な問題や環境不足は分けて報告します。

ビルド後は、対象範囲で内容がある `.warn` ファイルを確認します。

```bash
find . -type f -name '*.warn' -size +0 -print
find . -type f -name '*.warn' -size +0 -print0 | xargs -0 -r sed -n '1,200p'
```

`.warn` ファイルは結果であり、直接編集しません。  
警告原因を修正し、同じビルドを再実行します。

`bin/` や `bin_internal/` のスクリプトを変更した場合は、`bin_test/` のテストを実行します。  
Python のファイル入出力と子プロセス通信は、テスト用の一時ファイルも含めて [Python の文字コード指定規範](python-text-encoding-guideline.md) に従います。  
`make bin-test` は、ワークスペース内のすべての `bin_test/` を実行し、CI でも同じコマンドを使用します。  
対象を絞る場合は、ワークスペースからの相対パスに含まれる文字列を指定します。

```bash
make bin-test BIN_TEST_ARGS=framework/docsfw
```

ツールやファイルの不足によるスキップは、検証していないのに成功に見えるため失敗として扱います。  
OS に依存するスキップは、理由を `[Linux]` または `[Windows]` で始めた場合に限り、別の OS で起きたときだけ認めます。  
特定の OS だけで動く `*_selftest.py` は、`BIN_TEST_PLATFORM = "linux"` のように宣言すると、ほかの OS では実行せずに想定どおりのスキップとして扱います。  
手元でツールが不足する場合は、`BIN_TEST_ARGS=--allow-skips` を指定すると、想定外のスキップを失敗にしません。  
NuGet ソースに到達できないときの testfw の .NET テストのスキップも、`make bin-test` では想定外のスキップとして失敗にします。ネットワークのある CI で、そのテストが実際に動いたことを確かめるためです。  
`bin_test/` を含むサブモジュールに `framework/<name>.assured.stamp` や `app/<name>.assured.stamp` があり、ハッシュが HEAD と一致し、作業ツリーに差分が無いときは、その `bin_test/` を実行せず、`assured skip` として成功に数えます。  
判定の条件は [assured.stamp による保証済み app の扱い](../../../framework/makefw/docs/build-configurations.md#assuredstamp-による保証済み-app-の扱い) を参照してください。  
`make bin-test` は `make livedocs-venv` で依存を確認し、発行処理と同じ Python をスクリプトのテストに使用します。  
Python の採用条件と不足分の導入は [動的発行の Python 依存](../../../framework/docsfw/docs/python-components.md) を参照してください。

## ライブラリ構成の確認

`prod/libsrc/` 配下のソースをサブディレクトリへ分ける場合は、各サブディレクトリに makefw のテンプレート `makefile` を置くサブディレクトリ走査方式を利用します。  
`makepart.mk` の `ADD_SRCS` へ相対パスを列挙すると、ライブラリ ルート直下へシンボリック リンクが作成され、`.gitignore` の自動生成、Doxygen の重複読み込み、モジュール私有ヘッダーの探索失敗を招きます。

構成の誤りは次の 3 つのコマンドで検出します。いずれも 0 件である必要があります。

```bash
find app -path '*/prod/libsrc/*' -type l -not -path '*/obj/*'
grep -rln 'ADD_SRCS' app --include=makepart.mk | grep '/prod/libsrc/'
grep -rln 'EXCLUDE_PATTERNS' app --include='Doxyfile.part*'
```

`.gitignore` の有無は検出条件に使いません。  
外部 OSS を扱う app は、zip から展開した生成物を除外する目的で手動定義する場合があり、誤検知を招くためです。

ビルド機構の回避策を追加する前に、他の app が同じ回避策を必要としているかを確認します。  
自分だけが特別な回避策を要する状態は、フレームワークの制約ではなく構成の誤りを示します。

## 文書の変更

Markdown を変更した場合は、対象ごとに次の順で確認します。

```bash
python framework/docsfw/bin/text_style_jp.py <対象ファイル> --dry-run
python framework/docsfw/bin/text_style_jp.py <対象ファイル> --in-place
```

全 Markdown を対象にする場合は、[Markdown 一括スタイル確認](markdown-style-bulk-check.md) に従います。

## ソースの変更

新規ソースは `clang-format`、既存ソースの変更は `git clang-format` で確認します。  
整形後は、Doxygen コメントの字下げも確認します。

C/C++ の規範は [コーディング規範](coding-guideline.md)、テストの規範は [テスト方法](../../../framework/testfw/docs/how-to-test.md) を参照してください。
