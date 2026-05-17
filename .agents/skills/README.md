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

skill の正本は 2 種類あります。

| 種別 | 正本 |
|---|---|
| ルート固有の skill | ルートの `.agents/skills/<skill>/` |
| サブモジュール固有の skill | 各サブモジュールの `.agents/skills/<skill>/` |

サブモジュール固有の skill は、`make skills` によりルートの `.agents/skills/` へ集約されます。  
Linux では symlink、Windows ではコピーとして配置されるため、生成された集約先ではなく正本側を編集してください。

同期の仕組みは [`docs/c-modernization-kit/sync-skills.md`](../../docs/c-modernization-kit/sync-skills.md) を参照してください。

## 追加・更新するとき

- ルート固有の作業知識は、このディレクトリに新しい skill を追加します
- サブモジュール固有の作業知識は、対象サブモジュール側の `.agents/skills/` に追加します
- skill 名は、用途が分かる小文字の kebab-case にします
- 各 `SKILL.md` には、少なくとも `name`、`description`、`when_to_use` を記載します
- サブモジュール由来の skill を追加・削除した後は、`make skills` で集約結果を更新します

## スキル一覧

\toc depth=-1 exclude-basedir=true

## 関連文書

- [`AGENTS.md`](../../AGENTS.md) - このリポジトリ全体の作業方針
- [`docs/c-modernization-kit/sync-skills.md`](../../docs/c-modernization-kit/sync-skills.md) - skill 集約の設計と同期手順
