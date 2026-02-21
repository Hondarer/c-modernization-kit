# VS Code（Visual Studio Code）

## 概要

Visual Studio Code（VS Code）は Microsoft が提供する無料・オープンソースのコードエディタです。豊富な拡張機能によって C/C++・C#・Python など多様な言語に対応し、Git 統合・デバッガ・タスクランナーなどの機能を持ちます。軽量でありながら IDE に近い機能を持つため、クロスプラットフォーム開発に広く使われています。

このリポジトリの開発環境として VS Code を使用することで、C ソースコードのインテリセンス（補完・定義ジャンプ）、デバッグ、Makefile タスクの実行、Git 操作をひとつのエディタで行えます。Windows 環境では `Start-VSCode-With-Env.ps1` スクリプトが MinGW / VSBT の環境変数を設定した状態で VS Code を起動するため、ターミナルから直接 `gcc` や `make` コマンドを利用できます。

`.vscode/` ディレクトリに `settings.json`・`tasks.json`・`launch.json`・`c_cpp_properties.json` を配置することで、チームで共通の開発環境設定を共有できます。

## 習得目標

- [ ] VS Code をインストールし、C/C++ 拡張機能を導入できる
- [ ] `c_cpp_properties.json` でインクルードパスを設定してインテリセンスを有効にできる
- [ ] `tasks.json` で `make` コマンドをタスクとして登録できる
- [ ] `launch.json` でデバッガを設定し、ブレークポイントでデバッグできる
- [ ] VS Code の統合ターミナルから Git コマンドを実行できる
- [ ] VS Code 内の変数（`${workspaceFolder}` など）の意味を理解できる

## 学習マテリアル

### 公式ドキュメント

- [VS Code 公式ドキュメント](https://code.visualstudio.com/docs) — VS Code の完全なドキュメント（英語）
  - [C/C++ 拡張機能](https://code.visualstudio.com/docs/languages/cpp) — C/C++ の設定方法
  - [タスク設定](https://code.visualstudio.com/docs/editor/tasks) — `tasks.json` の設定リファレンス
  - [デバッグ設定](https://code.visualstudio.com/docs/editor/debugging) — `launch.json` の設定
  - [Git 統合](https://code.visualstudio.com/docs/sourcecontrol/overview) — VS Code 内での Git 操作

## このリポジトリとの関連

### 使用箇所（具体的なファイル・コマンド）

VS Code のタスク設定例（`.vscode/tasks.json`）:

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "make all",
      "type": "shell",
      "command": "make",
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "presentation": {
        "reveal": "always"
      }
    },
    {
      "label": "make clean",
      "type": "shell",
      "command": "make clean"
    }
  ]
}
```

C/C++ インテリセンス設定（`.vscode/c_cpp_properties.json`）:

```json
{
  "configurations": [
    {
      "name": "Linux",
      "includePath": [
        "${workspaceFolder}/prod/calc/include",
        "${workspaceFolder}/testfw/include"
      ],
      "compilerPath": "/usr/bin/gcc",
      "cStandard": "c11"
    }
  ],
  "version": 4
}
```

Windows での VS Code 起動（環境変数設定込み）:

```powershell
# MinGW と VSBT の環境変数を設定して VS Code を起動
.\Start-VSCode-With-Env.ps1
```

### 関連ドキュメント

- [VS Code 変数リファレンス](../../vscode-variables.md) — `${workspaceFolder}` などの変数説明
- [C/C++ インクルードフォルダ設定](../../c-cpp-include-folder.md) — インテリセンスのパス設定
- [WSL / MinGW 環境（スキルガイド）](wsl-mingw.md) — Windows でのビルド環境
