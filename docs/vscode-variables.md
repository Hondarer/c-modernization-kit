# VS Code における環境変数の注入方法

VS Code で環境変数を扱う方法は大きく分けて2つあります。それぞれの特徴と実装方法を説明します。

## 方法1: 環境変数設定後に VS Code を起動 (最も適切)

VS Code プロセスを起動する前に環境変数を設定する方法です。この方法では、VS Code 本体とそこから起動されるすべての子プロセス (ターミナル、ビルドタスク、デバッグ実行) が同じ環境変数を継承します。

### 利点

- 設定ファイルの重複が不要
- すべてのシーンで完全に同一の環境変数
- 設定の一元管理が容易
- プラットフォーム間の差異を吸収しやすい

### 欠点

- VS Code を GUI から直接起動できない (スクリプト経由での起動が必要)
- チームメンバー全員が同じ起動方法を理解する必要がある

### 実装方法

#### Linux / macOS

プロジェクトルートに起動スクリプトを作成します。

`start-vscode.sh`

```bash
#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ENV_FILE="${SCRIPT_DIR}/.env"

if [ ! -f "${ENV_FILE}" ]; then
    echo "Error: .env file not found at ${ENV_FILE}"
    exit 1
fi

while IFS='=' read -r key value; do
    [[ "${key}" =~ ^#.*$ ]] && continue
    [[ -z "${key}" ]] && continue
    
    value="${value//\$\{WORKSPACE\}/${SCRIPT_DIR}}"
    value=$(eval echo "${value}")
    
    export "${key}=${value}"
done < "${ENV_FILE}"

code "${SCRIPT_DIR}"
```

実行権限を付与して使用します。

```bash
chmod +x start-vscode.sh
./start-vscode.sh
```

#### Windows (PowerShell)

`start-vscode.ps1`

```powershell
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$EnvFile = Join-Path $ScriptDir ".env"

if (-not (Test-Path $EnvFile)) {
    Write-Error "Error: .env file not found at $EnvFile"
    exit 1
}

Get-Content $EnvFile | ForEach-Object {
    $line = $_.Trim()
    
    if ($line -match '^#' -or $line -eq '') {
        return
    }
    
    if ($line -match '^([^=]+)=(.*)$') {
        $key = $matches[1].Trim()
        $value = $matches[2].Trim()
        
        $value = $value -replace '\$\{WORKSPACE\}', $ScriptDir
        $value = $ExecutionContext.InvokeCommand.ExpandString($value)
        
        [Environment]::SetEnvironmentVariable($key, $value, 'Process')
    }
}

code $ScriptDir
```

実行ポリシーを設定して使用します。

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
.\start-vscode.ps1
```

#### .env ファイルの例

`.env`

```text
# プロジェクト固有のライブラリパス
LD_LIBRARY_PATH=${WORKSPACE}/lib:${LD_LIBRARY_PATH}
DYLD_LIBRARY_PATH=${WORKSPACE}/lib:${DYLD_LIBRARY_PATH}

# プロジェクト固有の実行ファイルパス
PATH=${WORKSPACE}/bin:${PATH}

# カスタム環境変数
PROJECT_ROOT=${WORKSPACE}
BUILD_TYPE=Debug
```

Windows では `LD_LIBRARY_PATH` は無視され、`PATH` のみが有効になります。プラットフォーム固有の変数を混在させても問題ありません。

### 使用例

```bash
# ターミナルから起動
./start-vscode.sh

# VS Code 内の統合ターミナルで確認
echo $LD_LIBRARY_PATH
# 出力: /path/to/project/lib:/original/ld/library/path

# ビルドタスクやデバッグ実行でも同じ環境変数が使用される
```

## 方法2: 個々の設定ファイルで環境変数を設定

VS Code の設定ファイル (`.vscode/settings.json`, `tasks.json`, `launch.json`) に環境変数を記述する方法です。

### 利点

- VS Code を通常通り GUI から起動可能
- 各機能ごとに異なる環境変数を設定できる (必要に応じて)
- VS Code の標準機能のみで実現可能

### 欠点

- 設定ファイルが3箇所に分散し、重複が発生
- 環境変数の変更時に複数ファイルの修正が必要
- 設定の不整合が発生しやすい

### 実装方法

#### 統合ターミナル用: .vscode/settings.json

`.vscode/settings.json`

```json
{
  "terminal.integrated.env.linux": {
    "LD_LIBRARY_PATH": "${workspaceFolder}/lib:${env:LD_LIBRARY_PATH}",
    "PATH": "${workspaceFolder}/bin:${env:PATH}",
    "PROJECT_ROOT": "${workspaceFolder}",
    "BUILD_TYPE": "Debug"
  },
  "terminal.integrated.env.osx": {
    "DYLD_LIBRARY_PATH": "${workspaceFolder}/lib:${env:DYLD_LIBRARY_PATH}",
    "PATH": "${workspaceFolder}/bin:${env:PATH}",
    "PROJECT_ROOT": "${workspaceFolder}",
    "BUILD_TYPE": "Debug"
  },
  "terminal.integrated.env.windows": {
    "PATH": "${workspaceFolder}/bin;${env:PATH}",
    "PROJECT_ROOT": "${workspaceFolder}",
    "BUILD_TYPE": "Debug"
  }
}
```

新しく開くターミナルに環境変数が設定されます。既存のターミナルには反映されないため、設定変更後は再起動が必要です。

##### 本プロジェクトでの実装例

本プロジェクトでは、C プロジェクト (`prod/calc/`) と .NET プロジェクト (`prod/calc.net/`) の両方が存在するため、それぞれのライブラリと実行ファイルへのパスを設定しています。

`.vscode/settings.json`

```json
{
  "terminal.integrated.env.linux": {
    "LD_LIBRARY_PATH": "${workspaceFolder}/prod/calc/lib:${workspaceFolder}/prod/calc.net/lib:${env:LD_LIBRARY_PATH}",
    "PATH": "${workspaceFolder}/prod/calc/bin:${workspaceFolder}/prod/calc.net/bin:${env:PATH}"
  },
  "terminal.integrated.env.windows": {
    "PATH": "${workspaceFolder}\\prod\\calc\\bin;${workspaceFolder}\\prod\\calc.net\\bin;${workspaceFolder}\\prod\\calc\\lib;${workspaceFolder}\\prod\\calc.net\\lib;${env:PATH}"
  }
}
```

**ポイント**:
- Linux では `LD_LIBRARY_PATH` を設定 (動的ライブラリの検索パス)
- Linux では `PATH` も設定 (実行ファイルの検索パス)
- Windows では `PATH` を設定 (実行ファイル・DLL の検索パス)
- C プロジェクトと .NET プロジェクトの両方のパスを含める
- Windows のパス区切り文字は `\` を使用 (`\\` でエスケープ)
- パスの区切りは Linux が `:`, Windows が `;`

#### ビルドタスク用: .vscode/tasks.json

`.vscode/tasks.json`

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "build",
      "type": "shell",
      "command": "make",
      "options": {
        "env": {
          "LD_LIBRARY_PATH": "${workspaceFolder}/lib:${env:LD_LIBRARY_PATH}",
          "PATH": "${workspaceFolder}/bin:${env:PATH}",
          "PROJECT_ROOT": "${workspaceFolder}",
          "BUILD_TYPE": "Debug"
        }
      },
      "group": {
        "kind": "build",
        "isDefault": true
      }
    }
  ]
}
```

##### タスクタイプによる環境変数の扱いの違い

タスクの `type` プロパティによって、`settings.json` で設定した環境変数の影響を受けるかどうかが変わります。

**`"type": "shell"`** のタスクは統合ターミナルで実行されるため、`settings.json` の `terminal.integrated.env.*` で設定した環境変数も自動的に利用できます。

**`"type": "process"`** のタスクは統合ターミナルを経由せず直接プロセスを起動するため、`settings.json` の設定は反映されません。環境変数が必要な場合は `tasks.json` に明示的に指定する必要があります。

Windows と Linux で異なるパス区切り文字を使用する場合は、`windows` プロパティで上書きします。

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "build",
      "type": "shell",
      "command": "make",
      "options": {
        "env": {
          "LD_LIBRARY_PATH": "${workspaceFolder}/lib:${env:LD_LIBRARY_PATH}",
          "PATH": "${workspaceFolder}/bin:${env:PATH}"
        }
      },
      "windows": {
        "options": {
          "env": {
            "PATH": "${workspaceFolder}/bin;${env:PATH}"
          }
        }
      }
    }
  ]
}
```

##### 本プロジェクトでの実装例

本プロジェクトでは、`make test` タスクと `make test (current dir)` タスクでプラットフォーム別の環境変数設定を行っています。C プロジェクトと .NET プロジェクトの両方のライブラリパスを設定しています。

`.vscode/tasks.json`

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "make test",
      "type": "shell",
      "command": "make",
      "args": ["test"],
      "options": {
        "cwd": "${workspaceFolder}"
      },
      "linux": {
        "options": {
          "env": {
            "LD_LIBRARY_PATH": "${workspaceFolder}/prod/calc/lib:${workspaceFolder}/prod/calc.net/lib:${env:LD_LIBRARY_PATH}"
          }
        }
      },
      "windows": {
        "options": {
          "env": {
            "PATH": "${workspaceFolder}\\prod\\calc\\lib;${workspaceFolder}\\prod\\calc.net\\lib;${env:PATH}"
          }
        }
      },
      "group": {
        "kind": "test",
        "isDefault": false
      }
    }
  ]
}
```

**ポイント**:
- `linux` プロパティでLinux固有の環境変数を設定
- `windows` プロパティでWindows固有の環境変数を設定
- C プロジェクト (`prod/calc/lib`) と .NET プロジェクト (`prod/calc.net/lib`) の両方のライブラリパスを含める
- トップレベルの `options` とプラットフォーム固有の `options` はマージされる
- この方法により、プラットフォームごとに異なるパス区切り文字や環境変数名を適切に扱える

#### デバッグ実行用: .vscode/launch.json

`.vscode/launch.json`

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug Program",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/build/myapp",
      "args": [],
      "cwd": "${workspaceFolder}",
      "environment": [
        {
          "name": "LD_LIBRARY_PATH",
          "value": "${workspaceFolder}/lib:${env:LD_LIBRARY_PATH}"
        },
        {
          "name": "PATH",
          "value": "${workspaceFolder}/bin:${env:PATH}"
        },
        {
          "name": "PROJECT_ROOT",
          "value": "${workspaceFolder}"
        },
        {
          "name": "BUILD_TYPE",
          "value": "Debug"
        }
      ]
    }
  ]
}
```

**重要**: デバッグ構成は統合ターミナルを経由せず直接プロセスを起動するため、`settings.json` の `terminal.integrated.env.*` の設定は反映されません。必要な環境変数はすべて `launch.json` に明示的に指定する必要があります。

Windows 用には別の設定を追加できます。

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug Program (Linux)",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/build/myapp",
      "environment": [
        {
          "name": "LD_LIBRARY_PATH",
          "value": "${workspaceFolder}/lib:${env:LD_LIBRARY_PATH}"
        }
      ]
    },
    {
      "name": "Debug Program (Windows)",
      "type": "cppvsdbg",
      "request": "launch",
      "program": "${workspaceFolder}/build/myapp.exe",
      "environment": [
        {
          "name": "PATH",
          "value": "${workspaceFolder}/bin;${env:PATH}"
        }
      ]
    }
  ]
}
```

##### 本プロジェクトでの実装例

本プロジェクトでは、4つのデバッグ構成でそれぞれ環境変数を設定しています。

- `(Linux gdb) Debug current directory` - Linux向けネイティブデバッグ
- `(Windows vsdbg) Debug current directory` - Windows向けネイティブデバッグ
- `(Linux coreclr) Debug current directory` - Linux向け.NETデバッグ
- `(Windows coreclr) Debug current directory` - Windows向け.NETデバッグ

すべての構成で C プロジェクトと .NET プロジェクトの両方のライブラリパスを設定しています。

`.vscode/launch.json` (抜粋)

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "(Linux gdb) Debug current directory",
      "type": "cppdbg",
      "request": "launch",
      "program": "${fileDirname}/${fileDirnameBasename}",
      "environment": [
        {
          "name": "LD_LIBRARY_PATH",
          "value": "${workspaceFolder}/prod/calc/lib:${workspaceFolder}/prod/calc.net/lib:${env:LD_LIBRARY_PATH}"
        }
      ]
    },
    {
      "name": "(Windows vsdbg) Debug current directory",
      "type": "cppvsdbg",
      "request": "launch",
      "program": "${fileDirname}/${fileDirnameBasename}.exe",
      "environment": [
        {
          "name": "PATH",
          "value": "${workspaceFolder}\\prod\\calc\\lib;${workspaceFolder}\\prod\\calc.net\\lib;${env:PATH}"
        }
      ]
    },
    {
      "name": "(Linux coreclr) Debug current directory",
      "type": "coreclr",
      "request": "launch",
      "program": "${fileDirname}/bin/${fileDirnameBasename}",
      "env": {
        "LD_LIBRARY_PATH": "${workspaceFolder}/prod/calc/lib:${workspaceFolder}/prod/calc.net/lib:${env:LD_LIBRARY_PATH}"
      }
    },
    {
      "name": "(Windows coreclr) Debug current directory",
      "type": "coreclr",
      "request": "launch",
      "program": "${fileDirname}/bin/${fileDirnameBasename}",
      "env": {
        "PATH": "${workspaceFolder}\\prod\\calc\\lib;${workspaceFolder}\\prod\\calc.net\\lib;${env:PATH}"
      }
    }
  ]
}
```

**ポイント**:
- デバッグ構成ごとにプラットフォームを分けて定義
- `cppdbg` はネイティブC/C++デバッグ用、`cppvsdbg` はWindows用ネイティブC/C++デバッグ用
- `coreclr` は.NETデバッグ用
- C プロジェクト (`prod/calc/lib`) と .NET プロジェクト (`prod/calc.net/lib`) の両方のライブラリパスを含める
- ネイティブデバッグでは `environment` プロパティ、.NETデバッグでは `env` プロパティを使用
- Windows版では必ずパス区切り文字を `\\` にする

#### GitHub Actions ワークフロー用

CI/CD環境でも同様に環境変数を設定する必要があります。GitHub Actionsでは、環境変数を永続化するための特別なメカニズムがあります。

##### Linux ジョブ

`.github/workflows/ci.yml` (build-and-test-linux ジョブ)

```yaml
jobs:
  build-and-test-linux:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Build
        run: make

      - name: Set library path for tests
        run: echo "LD_LIBRARY_PATH=$GITHUB_WORKSPACE/prod/calc/lib:$GITHUB_WORKSPACE/prod/calc.net/lib:$LD_LIBRARY_PATH" >> $GITHUB_ENV

      - name: Run tests
        run: make test
```

**ポイント**:
- `$GITHUB_ENV` に書き込むことで環境変数を永続化
- 以降のすべてのステップで設定した環境変数が有効になる
- C プロジェクトと .NET プロジェクトの両方のライブラリパスを含める
- `$GITHUB_WORKSPACE` はワークスペースのルートパスを表す

##### Windows ジョブ

`.github/workflows/ci.yml` (build-and-test-windows ジョブ)

```yaml
jobs:
  build-and-test-windows:
    runs-on: windows-2025
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Build
        run: make
        shell: pwsh

      - name: Set library path for tests
        run: |
          $calcLibPath = "${{ github.workspace }}\prod\calc\lib"
          $calcNetLibPath = "${{ github.workspace }}\prod\calc.net\lib"
          Add-Content -Path $env:GITHUB_PATH -Value $calcLibPath
          Add-Content -Path $env:GITHUB_PATH -Value $calcNetLibPath
          Write-Host "Added $calcLibPath to PATH"
          Write-Host "Added $calcNetLibPath to PATH"
        shell: pwsh

      - name: Run tests
        run: make test
        shell: pwsh
```

**ポイント**:
- `$env:GITHUB_PATH` に書き込むことで `PATH` 環境変数に追加
- C プロジェクトと .NET プロジェクトの両方のライブラリパスを含める
- PowerShell を使用 (`shell: pwsh`)
- `${{ github.workspace }}` はワークスペースのルートパスを表す

##### GitHub Actions の環境変数永続化メカニズム

- **`$GITHUB_ENV`**: 環境変数を設定 (`VARIABLE=value` 形式で書き込み)
- **`$GITHUB_PATH`**: `PATH` 環境変数にパスを追加 (パスを1行ずつ書き込み)

これらの特別なファイルに書き込むことで、以降のステップでその環境変数が利用可能になります。

### VS Code の定義済み変数

すべての設定ファイル (`settings.json`、`tasks.json`、`launch.json`) で VS Code の定義済み変数を使えます。これらを使うことで、プロジェクト構造に依存しない設定を作れます。

よく使う変数は以下です。

**`${workspaceFolder}`** はワークスペースのルートパスを表します。プロジェクトの絶対パスが環境によって異なる場合でも、この変数を使うことでポータブルな設定を作れます。

**`${workspaceFolderBasename}`** はワークスペースフォルダ名を表します。

**`${env:既存の環境変数名}`** は既存の環境変数を参照します。PATH に値を追加する場合などに便利です。

**`${file}`** は現在開いているファイルのパスを表します (`tasks.json` と `launch.json` で使用可能)。

より詳しい変数の一覧は、公式ドキュメントで確認できます。

[Variables Reference - Visual Studio Code](https://code.visualstudio.com/docs/editor/variables-reference)

## 比較表

| 項目 | 方法1: 起動前に設定 | 方法2: 設定ファイル |
|------|-------------------|-------------------|
| 環境変数の一貫性 | 完全に一致 | ほぼ一致 (設定次第) |
| 設定の一元管理 | 容易 (.env のみ) | 困難 (3ファイル) |
| VS Code の起動方法 | スクリプト経由 | GUI から直接可能 |
| 学習コスト | やや高い | 低い |
| メンテナンス性 | 高い | 低い (重複管理) |
| チーム共有 | スクリプト共有 | 設定ファイル共有 |

## 推奨する運用方法

### 個人開発の場合

**方法1 (起動前に設定)** を推奨します。

- `.env` で環境変数を一元管理
- `start-vscode.sh` または `start-vscode.ps1` で起動
- 設定ファイルの重複を排除

### チーム開発の場合

**方法2 (設定ファイル)** を推奨します。

- 各設定ファイルを適切に維持
- ドキュメントに注意事項や動作の説明を明記

チームメンバーは通常通り VS Code を起動できます。

## まとめ

VS Code における環境変数の設定は、用途に応じて適切な方法を選択することが重要です。

- 完全な一貫性と一元管理を優先する場合: **方法1**
- チームの利便性と学習コストを優先する場合: **方法2**
