# デモ用 app の構成

## 資料の位置付け

この資料は、GitHub リポジトリ `c-modernization-kit` に配置した app の役割と依存関係を説明します。  
ここで示す組み合わせはデモ用であり、フレームワークを別のワークスペースへ展開するときの必須構成ではありません。

`app/general` は標準配布する一般資料です。  
それ以外の app は目的に応じて選択でき、この資料を含む `app/c-modernization-kit` も削除できます。

## app の役割

| app | 役割 |
|---|---|
| `general` | フレームワークの一般規範、設計、運用手順 |
| `c-modernization-kit` | このリポジトリの app 構成と CI/CD の説明 |
| `cplat` | 複数の C app が利用する共通ユーティリティ ライブラリ |
| `calc` | 静的ライブラリ、共有ライブラリ、コマンド、Google Test の組み合わせ例 |
| `calc.net` | `calc` の共有ライブラリを P/Invoke で利用する .NET 実装例 |
| `porter` | `cplat` を利用する通信ライブラリ |
| `cjson`、`lua`、`sqlite`、`zlib` | 外部 OSS を makefw の規約でビルドする例 |
| `doxygen-sample` | Doxygen 文書生成の例 |
| `empty-lib` | ソースが空のライブラリを扱う例 |
| `string-catalog-sample` | 文字列 ID、引数スキーマ、言語別の文字列リソースを分離して文字列を組み立てる例 |
| `override-sample` | ライブラリ関数を差し替える例 |
| `service-sample` | Linux のデーモンと Windows サービスの実装例 |
| `subfolder-sample` | サブディレクトリを再帰的にコンパイルする例 |
| `tutorial` | 小さなコマンドを段階的に追加する例 |

## app 間の依存関係

各 app の `appdeps.mk` がビルド順序の正本です。  
現在の依存関係は次のとおりです。

- `calc`、`porter`、`empty-lib`、`override-sample`、`service-sample`、`subfolder-sample`、`tutorial` は `cplat` に依存します。
- `cplat` は JSON 設定解析に `cjson`、圧縮・展開に `zlib` を利用します。
- `calc.net` は `calc` に依存し、推移的に `cplat` も利用します。
- `cjson`、`lua`、`sqlite`、`zlib`、`doxygen-sample`、`string-catalog-sample` は、ほかの app に依存しません。
- `general` と `c-modernization-kit` は文書だけを持つため、ビルド依存関係を定義しません。

makefw はこの依存グラフから app のビルド順序を決定します。  
依存関係のない app は並列にビルドできます。

## calc と calc.net の構成

`calc` は、基本計算関数を持つ静的ライブラリと、計算処理の公開 API を持つ共有ライブラリを組み合わせます。  
コマンドとテストは、静的リンク、動的リンク、両者の併用を比較できる構成です。

`calc.net` は `calc` の共有ライブラリを P/Invoke で呼び出します。  
Linux と Windows で同じ .NET API を提供し、ネイティブ ライブラリの探索パスはワークスペースの生成設定から取得します。

## cplat のリンク方式

`cplat` は動的ライブラリだけを生成し、すべての製品利用者が同じライブラリを共有します。  
これにより、トレース レジストリや既定パーサーなどのプロセス グローバルな状態がプロセス内で一元化されます。

同梱 CLI、`service-sample`、`tutorial` では、`libcplat`、`libcjson`、`libzlib` を実行ファイルと同じ `prod/cbin` へコピーします。  
Linux では `$ORIGIN` の実行時探索パス、Windows では実行ファイルと同じディレクトリの DLL 探索を利用するため、開発環境のライブラリ探索パスに依存せず実行できます。

記述方法と配布時の要件は、[リンク方式の規約](../../c-platform/docs/link-policy.md) に従います。

## 関連資料

- [クロスプラットフォーム ビルド システムの実装](../../general/docs/build-design.md)
- [GitHub Actions CI/CD 仕様](github-actions.md)
- [VS Code における環境変数と c_cpp_properties.json の保守手順](../../general/docs/vscode-variables.md)
