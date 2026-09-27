# skills

このディレクトリは、`make skills` がリポジトリ内のスキル正本を集約する射影先です。  
管理対象の `SKILL.md` はここへ直接配置せず、対象範囲を所有する app または framework の `.agents/skills/` に配置します。

## 参照方法

作業内容に該当するスキルがある場合は、`SKILL.md` の `description` で適用条件を確認してから本文と参照先を読みます。  
複数のスキルが該当する場合は、各スキルの必須事項を併用します。

管理対象の YAML front matter は `name` と `description` を基本とし、既存メタデータは用途を確認して保持します。  
Windows の生成コピーに付く `git-origin` は発行用の例外であり、正本へ転記しません。  
適用条件は `description` に含め、詳細な規範や手順は正本ドキュメントへ集約します。

## 正本の配置

| 対象範囲 | 正本 |
|---|---|
| ワークスペース固有 | `app/c-modernization-kit/.agents/skills/<skill>/` |
| 全 app 共通 | `app/general/.agents/skills/<skill>/` |
| app 固有 | `app/<name>/.agents/skills/<skill>/` |
| framework 固有 | `framework/<name>/.agents/skills/<skill>/` |

プロジェクト ルートは射影先であるため、管理対象スキルの正本を原則として配置しません。  
個人用の一時的なスキルは `.gitignore` の対象として配置できます。

追加、削除、または名称変更の後は `make skills` を実行します。  
配置規則と同期手順は、[スキルの配置と同期](../../app/general/docs/skill-sync.md) を参照してください。

## スキル一覧

\toc depth=-1 exclude-basedir=true

## 関連文書

- [ワークスペースの作業規則](../../AGENTS.md)
- [AGENTS.md とスキルの設計指針](../../app/general/docs/agents-and-skills-guideline.md)
- [スキルの配置と同期](../../app/general/docs/skill-sync.md)
