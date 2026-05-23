# skills

このディレクトリは、作業時に参照する skill 文書の配置ディレクトリです。  
skill は、特定の作業で判断に迷いやすい点、参照先、実装方針を `SKILL.md` にまとめた作業ガイドです。

## まず見る場所

各 skill は次の構成です。

```text
.agents/skills/
└── <skill-name>/
    └── SKILL.md
```

`SKILL.md` 冒頭の `description` と `when_to_use` を確認し、作業内容に合う skill を選びます。  
複数の skill が関係する場合は、より対象範囲が狭い skill を優先します。

## このリポジトリでの扱い

このディレクトリ直下は `make skills` による**射影の配置場所**です。リポジトリ管理対象の skill はここに直接置かず、正本はすべて各サブモジュールまたは `app/<appname>/` 配下に置きます。

`make skills` の実行により、以下の正本からこのディレクトリへ集約されます (Linux: symlink、Windows: コピー)。

| 種別 | 正本 |
|---|---|
| サブモジュール固有の skill | 各サブモジュールの `.agents/skills/<skill>/` |
| 非サブモジュール app 固有の skill | `app/<appname>/.agents/skills/<skill>/` |

生成された集約先ではなく正本側を編集してください。

同期の仕組みは [`docs/c-modernization-kit/sync-skills.md`](../../docs/c-modernization-kit/sync-skills.md) を参照してください。

### 個人的・一時的な利用

個人的または一時的なシナリオでは、このディレクトリ直下に skill を直接配置することもできます。  
`.gitignore` により `README.md` と `.gitignore` 以外はすべてリポジトリ管理対象外となるため、誤ってコミットされることはありません。

## 追加・更新するとき

- サブモジュール固有の作業知識は、対象サブモジュール側の `.agents/skills/` に追加します
- 非サブモジュール app 固有の作業知識は、対象 app 側の `.agents/skills/` に追加します
- 上記追加・削除後は `make skills` で集約結果を更新します
- skill 名は、用途が分かる小文字の kebab-case にします
- 各 `SKILL.md` には、少なくとも `name`、`description`、`when_to_use` を記載します

## スキル一覧

\toc depth=-1 exclude-basedir=true

## 関連文書

- [`AGENTS.md`](../../AGENTS.md) - このリポジトリ全体の作業方針
- [`docs/c-modernization-kit/sync-skills.md`](../../docs/c-modernization-kit/sync-skills.md) - skill 集約の設計と同期手順
