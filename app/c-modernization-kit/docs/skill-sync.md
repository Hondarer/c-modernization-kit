# スキル同期

## 正本と射影先

追跡対象のスキル正本は、framework または `app/<name>/.agents/skills/` に配置します。  
全 app 向けの正本は `app/general`、個別 app 向けの正本はその app、ワークスペース固有の正本は `app/c-modernization-kit` に配置します。

ルート `.agents/skills/` は `make skills` が作成する射影先です。  
追跡対象のスキル正本を直接配置しません。

## 同期処理

`make skills` は、`.gitmodules` に登録されたサブモジュールと、ルート Git 管理下の `app/<name>` から `.agents/skills/<skill>` を収集します。  
同名スキルが複数の正本に存在する場合はエラーで停止します。

Linux では正本へのシンボリック リンクを作成し、Windows ではディレクトリをコピーします。  
`.claude/skills` も同じ集約結果を参照します。

Windows のルート `.agents/skills/` の Markdown には、発行時に正本へリンクするため `git-origin` が追加されます。  
これは生成コピーだけのメタデータであり、正本の frontmatter には追加しません。  
`.claude/skills/` には付与しないため、コピー比較では改行コードとこのキーを区別して確認します。

前回生成した名前は `.agents/skills/.sync-state` に記録されます。  
`.agents/skills/.gitignore` は README.md と .gitignore 以外を管理対象外にする固定設定であり、同期処理が生成する一覧ではありません。

## 追加と更新

1. 所有する Git ルートの `.agents/skills/<skill>/SKILL.md` を変更します。
2. `SKILL.md` の frontmatter を検証します。
3. `make skills` を実行します。
4. ルートの射影先と `.claude/skills` を確認します。

生成されたルート側のファイルを直接編集しません。

## ドキュメント発行

GitHub Actions と Jenkins は、ドキュメント生成前に `make skills` を実行します。  
これにより、発行対象のスキル文書も正本と同じ内容になります。
