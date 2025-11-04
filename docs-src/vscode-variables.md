# VS Code における環境変数の設定ガイド

VS Code では、プロジェクトごとに `.vscode` フォルダ配下の設定ファイルを使って環境変数を設定できます。ただし、設定できる範囲は用途によって異なります。

## 設定できる範囲の概要

VS Code で環境変数を設定できる主な場面は 3 つあります。

**統合ターミナルを開いた時**に環境変数を設定するには、`settings.json` を使います。ターミナルで直接コマンドを実行する際や、ターミナル経由で起動するプログラムに環境変数を渡せます。

**タスクを実行する時**に環境変数を設定するには、`tasks.json` を使います。ビルドやテスト実行などのタスクに環境変数を渡せます。

**デバッグ実行する時**に環境変数を設定するには、`launch.json` を使います。デバッガー経由で起動するプログラムに環境変数を渡せます。

## 統合ターミナルでの環境変数設定

ターミナルを開いた時に自動的に環境変数を設定するには、プラットフォームごとに設定項目を指定します。

```{.json caption=".vscode/settings.json"}
{
  "terminal.integrated.env.linux": {
    "LD_LIBRARY_PATH": "${workspaceFolder}/lib:/usr/local/lib"
  },
  "terminal.integrated.env.osx": {
    "DYLD_LIBRARY_PATH": "${workspaceFolder}/lib:/usr/local/lib"
  },
  "terminal.integrated.env.windows": {
    "PATH": "${workspaceFolder}\\bin;${env:PATH}"
  }
}
```

この設定を行うと、VS Code でターミナルを新しく開いた時に、指定した環境変数が自動的に追加されます。既に開いているターミナルには反映されないため、設定後は新しいターミナルを開く必要があります。

既存の環境変数を参照するには `${env:変数名}` の形式を使います。ワークスペースのルートパスを参照するには `${workspaceFolder}` を使います。

## タスク実行時の環境変数設定

タスクを実行する際に環境変数を設定するには、`tasks.json` の `options.env` を指定します。

```{.json caption=".vscode/tasks.json"}
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Build Project",
      "type": "shell",
      "command": "make",
      "options": {
        "env": {
          "LD_LIBRARY_PATH": "${workspaceFolder}/lib:/usr/local/lib"
        }
      }
    },
    {
      "label": "Run Tests",
      "type": "process",
      "command": "${workspaceFolder}/bin/test",
      "options": {
        "env": {
          "LD_LIBRARY_PATH": "${workspaceFolder}/lib:/usr/local/lib"
        }
      }
    }
  ]
}
```

タスクの種類 (type) によって、settings.json で設定した環境変数の影響を受けるかどうかが変わります。

`"type": "shell"` のタスクは統合ターミナルで実行されるため、settings.json の `terminal.integrated.env.*` で設定した環境変数も利用できます。

`"type": "process"` のタスクは統合ターミナルを経由せず直接プロセスを起動するため、settings.json の設定は反映されません。環境変数が必要な場合は tasks.json に明示的に指定する必要があります。

## デバッグ実行時の環境変数設定

デバッグ実行時に環境変数を設定するには、`launch.json` の各構成に `env` プロパティを指定します。

```{.json caption=".vscode/launch.json"}
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "C++: Debug",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/build/myapp",
      "args": [],
      "cwd": "${workspaceFolder}",
      "env": {
        "LD_LIBRARY_PATH": "${workspaceFolder}/lib:/usr/local/lib"
      }
    },
    {
      "name": "Python: Current File",
      "type": "python",
      "request": "launch",
      "program": "${file}",
      "env": {
        "LD_LIBRARY_PATH": "${workspaceFolder}/lib:/usr/local/lib",
        "PYTHONPATH": "${workspaceFolder}/src"
      }
    }
  ]
}
```

デバッグ構成は統合ターミナルを経由せず直接プロセスを起動するため、settings.json の `terminal.integrated.env.*` の設定は反映されません。

## VS Code の定義済み変数の活用

すべての設定ファイルで VS Code の定義済み変数を使えます。これらを使うことで、プロジェクト構造に依存しない設定を作れます。

よく使う変数は以下です。

`${workspaceFolder}` はワークスペースのルートパスを表します。プロジェクトの絶対パスが環境によって異なる場合でも、この変数を使うことでポータブルな設定を作れます。

`${workspaceFolderBasename}` はワークスペースフォルダ名を表します。

`${env:既存の環境変数名}` は既存の環境変数を参照します。PATH に値を追加する場合などに便利です。

`${file}` は現在開いているファイルのパスを表します (tasks.json と launch.json で使用可能)。

より詳しい変数の一覧は、公式ドキュメントで確認できます。

[Variables Reference - Visual Studio Code](https://code.visualstudio.com/docs/editor/variables-reference)

## 推奨される設定方法

プロジェクト全体で共通の環境変数を使いたい場合は、以下のように設定するのが確実です。

settings.json で `terminal.integrated.env.*` を設定することで、ターミナルでの作業時に環境変数を利用できます。

launch.json の各デバッグ構成に `env` を設定することで、デバッグ実行時に環境変数を利用できます。

tasks.json のプロセスタスクに `options.env` を設定することで、タスク実行時に環境変数を利用できます。シェルタスクの場合は settings.json の設定が反映されますが、明示的に指定することで動作を明確にできます。

設定が重複しますが、それぞれの実行環境に確実に環境変数を渡せます。

## LD_LIBRARY_PATH 設定の完全な例

共有ライブラリのパスを設定する完全な例を示します。

```{.json caption=".vscode/settings.json"}
{
  "terminal.integrated.env.linux": {
    "LD_LIBRARY_PATH": "${workspaceFolder}/lib:${workspaceFolder}/external/lib:/usr/local/lib"
  }
}
```

```{.json caption=".vscode/tasks.json"}
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Build",
      "type": "shell",
      "command": "cmake --build build",
      "options": {
        "env": {
          "LD_LIBRARY_PATH": "${workspaceFolder}/lib:${workspaceFolder}/external/lib:/usr/local/lib"
        }
      }
    }
  ]
}
```

```{.json caption=".vscode/launch.json"}
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug App",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/build/myapp",
      "cwd": "${workspaceFolder}",
      "env": {
        "LD_LIBRARY_PATH": "${workspaceFolder}/lib:${workspaceFolder}/external/lib:/usr/local/lib"
      }
    }
  ]
}
```

この設定により、ターミナルでのコマンド実行、タスク実行、デバッグ実行のすべてで同じライブラリパスを使えます。
