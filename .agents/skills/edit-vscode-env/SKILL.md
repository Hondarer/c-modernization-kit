---
name: edit-vscode-env
description: |
  VS Code の環境変数設定を編集するときに使うスキルです。
  新しい app モジュールの PATH 追加、envFile 方式の構造、
  編集対象ファイルの一覧をまとめます。
when_to_use: |
  - 新しい app モジュールを追加して PATH を通すとき
  - VS Code のデバッグやタスクで DLL/SO が見つからないとき
  - 環境変数設定の構造を確認したいとき
---

# VS Code 環境変数の編集

このスキルは `.vscode/` 配下の環境変数設定を対象にします。
詳細なドキュメントは `docs/vscode-variables.md` を参照してください。

## 構造

VS Code の環境変数設定は envFile 方式で一元管理しています。

| ファイル | 用途 | envFile 対応 |
|---|---|---|
| `.vscode/.env.linux` | Linux 向け PATH / LD_LIBRARY_PATH | - |
| `.vscode/.env.windows` | Windows 向け PATH | - |
| `.vscode/settings.json` | 統合ターミナル用環境変数 | 非対応 |
| `.vscode/launch.json` | デバッグ構成 (envFile 参照) | 対応 |
| `.vscode/tasks.json` | タスク (envFile 参照) | 対応 |

`launch.json` と `tasks.json` は `.env.linux` / `.env.windows` を参照するため、直接編集は不要です。
`settings.json` の `terminal.integrated.env.*` は envFile をサポートしないため、別途編集が必要です。

## 新しいモジュール追加時の編集箇所

`app/<name>/prod/bin` や `app/<name>/prod/lib` を PATH に追加する場合、以下の 4 箇所を編集します。

### `.vscode/.env.linux`

```text
PATH=${workspaceFolder}/app/<name>/prod/bin:...(既存)...
LD_LIBRARY_PATH=${workspaceFolder}/app/<name>/prod/lib:...(既存)...
```

### `.vscode/.env.windows`

```text
PATH=${workspaceFolder}\app\<name>\prod\lib;${workspaceFolder}\app\<name>\prod\bin;...(既存)...
```

### `.vscode/settings.json` の `terminal.integrated.env.linux`

```json
"terminal.integrated.env.linux": {
    "LD_LIBRARY_PATH": "${workspaceFolder}/app/<name>/prod/lib:...(既存)...",
    "PATH": "${workspaceFolder}/app/<name>/prod/bin:...(既存)..."
}
```

### `.vscode/settings.json` の `terminal.integrated.env.windows`

```json
"terminal.integrated.env.windows": {
    "PATH": "${workspaceFolder}\\app\\<name>\\prod\\lib;${workspaceFolder}\\app\\<name>\\prod\\bin;...(既存)..."
}
```

## env ファイルの形式

`.env.linux` と `.env.windows` は 1 行 1 変数の形式です。

```text
PATH=値1:値2:${env:PATH}
LD_LIBRARY_PATH=値1:値2:${env:LD_LIBRARY_PATH}
```

- `${workspaceFolder}` でワークスペースルートを参照
- `${env:PATH}` で既存の PATH を末尾に追加
- Linux は `:` 区切り、パス区切りは `/`
- Windows は `;` 区切り、パス区切りは `\`

## launch.json / tasks.json の envFile 参照

各デバッグ構成とタスクは `envFile` で env ファイルを参照します。

```json
// launch.json (Linux 構成)
"envFile": "${workspaceFolder}/.vscode/.env.linux"

// launch.json (Windows 構成)
"envFile": "${workspaceFolder}/.vscode/.env.windows"

// tasks.json
"linux": {
    "options": {
        "envFile": "${workspaceFolder}/.vscode/.env.linux"
    }
},
"windows": {
    "options": {
        "envFile": "${workspaceFolder}/.vscode/.env.windows"
    }
}
```

## 判断基準

すべての `app/<name>/prod/bin` や `app/<name>/prod/lib` を PATH に追加するわけではありません。

追加が必要な場合:
- テスト実行時に DLL/SO が必要
- デバッグ実行時に実行ファイルや DLL/SO が必要
- 他のモジュールから実行時に参照される

追加が不要な場合:
- ビルド時のみ必要で実行時には不要
- 現在の VS Code タスク / デバッグ対象に含まれていない

## 確認項目

- `.env.linux` と `.env.windows` の両方を更新したか
- `settings.json` の `terminal.integrated.env.linux` と `terminal.integrated.env.windows` を更新したか
- Linux は `LD_LIBRARY_PATH` と `PATH` の両方に追加したか
- Windows は `lib` と `bin` の両方を `PATH` に追加したか
- パス区切り文字が OS ごとに正しいか (Linux: `/`、Windows: `\`)
- 区切り文字が OS ごとに正しいか (Linux: `:`、Windows: `;`)

## 参照ドキュメント

- `docs/vscode-variables.md` - VS Code における環境変数と保守手順の詳細
