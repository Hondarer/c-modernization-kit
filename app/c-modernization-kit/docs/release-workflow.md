# リリース タグと GitHub Release の作成

## 対象と基準

対象リポジトリの一覧は [release.py](../.agents/skills/release-tag/scripts/release.py) の `REPOSITORIES` を正本とします。  
対象は Hondarer 配下の統合ワークスペース、対象 framework と app、および独立した devbin-win と oracle-linux-container です。  
一覧を増減する場合は、依頼された公開対象と一致することを確認します。

既定では、リモートの既定ブランチの完全なコミット SHA を基準とし、ローカルのサブモジュールのピン留め SHA は使用しません。  
直近の通常の GitHub Release が同じ SHA を指す場合はスキップします。  
ここで直近とは GitHub の `releases/latest` が返す Release です。  
プレリリースを基準にする依頼は、この手順の対象外です。

### 統合ワークスペースのコミットを基準にする場合

最後に CI が成功したコミットなど、c-modernization-kit の特定のコミットを基準にする依頼では `--plan` に `--workspace-sha SHA` を追加します。  
SHA は 7 桁以上の 16 進数で指定し、計画には完全な SHA として記録します。

- c-modernization-kit は指定したコミットを基準にします。
- 指定コミットの `.gitmodules` と、リモート上のサブモジュールのピン留め SHA を読み取り、`REPOSITORIES` に含まれるサブモジュールはそのピン留め SHA を基準にします。
- サブモジュールではない対象 (devbin-win、oracle-linux-container など) は、従来どおり既定ブランチの SHA を基準にします。
- 指定コミットとピン留め SHA が各リポジトリの既定ブランチに含まれない場合は、計画を作成せずに停止します。

計画の各対象の `source` は、基準の取得元を `workspace`、`workspace-submodule`、`default-branch` のいずれかで示します。  
`--apply` は計画に記録した `workspace_sha` で再計画して比較するため、`--workspace-sha` を再指定しません。

## 事前確認

`gh auth status` で対象アカウントへのログインを確認します。  
タグの既定値は実行日の `vYYYYMMDD.0.0` です。  
同日の別リリースでは末尾を増やす候補を提示し、同名タグを上書きしません。  
ユーザーがすでにタグと対象を指定している場合は、その指定を使用します。

ワークスペース ルートで次を実行すると、全対象の SHA と作成・スキップ判定を JSON で取得できます。  
`--plan` は GET だけを行い、タグと Release を作成しません。

```bash
python app/c-modernization-kit/.agents/skills/release-tag/scripts/release.py --plan vYYYYMMDD.0.0
```

統合ワークスペースのコミットを基準にする場合は、次のように指定します。

```bash
python app/c-modernization-kit/.agents/skills/release-tag/scripts/release.py --plan vYYYYMMDD.0.0 --workspace-sha 5161ac8
```

実際の日付のタグを指定し、成功した出力だけを UTF-8 の作業用 JSON ファイルへ保存します。  
対象一覧、タグ、各 SHA、スキップ結果をまとめて提示します。  
公開の許可がまだなければ、この具体的な結果に対して確認を得ます。  
同じ対象とタグへの公開がすでに明示的に許可されていれば、同じ確認を繰り返しません。

全対象の取得が成功する前に公開しません。  
取得失敗、権限不足、レート制限、不正な応答は不在と区別して停止します。  
リポジトリ取得が成功した後のタグと直近 Release の HTTP 404 だけを不在として扱います。  
同名タグが一つでも存在する場合は、作成処理を開始しません。  
ただし、直近の Release のタグが指定タグと同じで、既定ブランチの SHA を指している対象はスキップとして扱います。

## 公開と完了条件

許可された計画だけを指定します。  
次のコマンドは GitHub の共有状態を変更します。

```bash
python app/c-modernization-kit/.agents/skills/release-tag/scripts/release.py --apply /path/to/approved-plan.json
```

全対象の状態と計画を再比較し、変わっていれば公開前に停止します。  
タグはブランチ名ではなく計画の SHA で作成し、参照先を検証してから GitHub Release を作成します。  
作成時に他の処理が同名タグを作成した場合も、既存タグを流用せず停止します。  
Release 本文は `--generate-notes` による既存の形式を維持します。

作成後は公開状態、タグ、解決先 SHA を検証し、作成・スキップの結果と URL を日本語で報告します。  
失敗や SHA 不一致では後続の公開を止め、完了済みの対象と、タグだけ作成された可能性がある対象を報告します。  
通信障害では結果が不明な場合があるため、自動再試行、タグ削除、ロールバックを行いません。  
再開前に該当するタグと Release の実状態を調査し、必要な追加操作の許可を確認します。

## スクリプトの局所検証

```bash
python -B -m unittest discover -s app/c-modernization-kit/.agents/skills/release-tag/scripts -p 'test_release.py'
```

このテストは GitHub 呼び出しを置換し、実際のタグや Release を作成しません。
