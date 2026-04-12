# VS Code における環境変数の扱いと保守手順

本ドキュメントは、VS Code で環境変数がどこに効くかを説明したうえで、このリポジトリでアプリ追加・削除・改名が発生した際に、どのファイルをどう見直すべきかをまとめた手引きです。

## 概要

本リポジトリでは、実行時に必要なライブラリ探索パスとコマンド探索パスを複数の場所で管理しています。

- VS Code の統合ターミナル
- VS Code の `make test` タスク
- VS Code のデバッグ構成
- GitHub Actions
- Jenkins

更新要否の判断は、GitHub Actions の設定そのものではなく、`app` 配下の各アプリケーションの構成と依存関係に基づいて行います。  
ただし、現時点では GitHub Actions の CI が成功しているため、`ci.yml` は少なくとも現在の必要十分な実装例として参照できます。

## VS Code で環境変数が効く場所

### `.vscode/settings.json`

`terminal.integrated.env.*` は、VS Code の統合ターミナルにだけ反映されます。  
新しく開いたターミナルには反映されますが、既存のターミナルには反映されません。

このリポジトリでは、Linux では `LD_LIBRARY_PATH` と `PATH`、Windows では `PATH` を設定しています。

### `.vscode/tasks.json`

`tasks.json` の `options.env` は、タスク実行時の環境変数です。  
このリポジトリの `make test` タスクでは、OS ごとに必要な実行時パスを明示的に設定しています。

特に、テスト失敗時に

```text
error while loading shared libraries: libxxxx.so: cannot open shared object file
```

のようなエラーが出た場合は、まず `tasks.json` の Linux 側 `LD_LIBRARY_PATH` を疑います。

### `.vscode/launch.json`

デバッグ構成は統合ターミナルの環境変数をそのまま使うわけではありません。  
必要な環境変数は、各デバッグ構成の `environment` または `env` に明示的に書く必要があります。

- `cppdbg` / `cppvsdbg` は `environment`
- `coreclr` は `env`

## このリポジトリで何を根拠に判断するか

環境変数の更新対象は、次の順で判断します。

1. `app/<name>/prod/lib` があるか
2. `app/<name>/prod/bin` があるか
3. そのアプリのライブラリや実行ファイルが、テスト・デバッグ・サンプル実行で必要か
4. その必要性が VS Code、GitHub Actions、Jenkins のどこまで及ぶか

### 基本ルール

- `prod/lib` を持つアプリ
  - Linux では `LD_LIBRARY_PATH` 候補
  - Windows では `PATH` 候補
- `prod/bin` を持つアプリ
  - Linux / Windows ともに `PATH` 候補
- ただし、`lib` や `bin` が存在しても、現在のテスト・デバッグ・実行で使わないなら追加しません

### 現在の代表例

現時点で `prod/lib` / `prod/bin` を持つ主なアプリは以下です。

| アプリ | `prod/lib` | `prod/bin` | 現在の環境変数設定に含める理由 |
|---|---|---|---|
| `calc` | あり | あり | C テスト、サンプル実行、.NET 呼び出しで使用 |
| `calc.net` | あり | あり | .NET テスト、サンプル実行で使用 |
| `override-sample` | あり | あり | CI / VS Code 実行対象として扱っている |
| `porter` | あり | あり | CI / VS Code 実行対象として扱っている |
| `com_util` | あり | なし | 多くのアプリが実行時に依存 |
| `subfolder-sample` | あり | あり | 現在の VS Code / CI の実行対象には含めていない |

`subfolder-sample` のように `lib` / `bin` を持つアプリでも、現在の実行対象に入っていなければ、環境変数へ自動的には追加しません。  
追加可否は「存在するか」だけではなく、「実行時に必要か」で判断します。

## どのファイルを更新するか

| 対象 | ファイル | 何を更新するか |
|---|---|---|
| 統合ターミナル | `.vscode/settings.json` | Linux の `LD_LIBRARY_PATH` / `PATH`、Windows の `PATH` |
| VS Code テストタスク | `.vscode/tasks.json` | `make test` と `make test (current dir)` の OS 別環境変数 |
| VS Code デバッグ | `.vscode/launch.json` | Linux / Windows の各デバッグ構成の環境変数 |
| GitHub Actions Linux | `.github/workflows/ci.yml` | `$GITHUB_ENV` と `$GITHUB_PATH` |
| GitHub Actions Windows | `.github/workflows/ci.yml` | `$GITHUB_PATH` に追加するパス配列 |
| Jenkins 実装 | `.jenkins/inner-build.sh` | `LD_LIBRARY_PATH`、`PATH`、成果物収集パス |
| Jenkins 説明 | `.jenkins/README.md` | 実装に対応する説明と例 |
| 個別ドキュメント | 各 README / docs | 実行例やトラブルシュートの古いパス |

## 具体的な更新手順

### 1. 追加・削除・改名の対象を確認する

まず、対象アプリに `prod/lib` と `prod/bin` があるかを確認します。

```bash
find app -maxdepth 3 \( -path '*/prod/lib' -o -path '*/prod/bin' \) | sort
```

次に、そのアプリがどこから使われるかを確認します。

- `makepart.mk` の `LIBS += ...`
- `LIBSDIR += ...`
- テストコードや README の実行例
- `.NET` 側の P/Invoke やネイティブライブラリ読み込み

### 2. VS Code の 3 ファイルを更新する

#### `.vscode/settings.json`

統合ターミナルで新しいライブラリや実行ファイルが見つかるようにします。

- Linux
  - `prod/lib` が必要なら `LD_LIBRARY_PATH` に追加
  - `prod/bin` が必要なら `PATH` に追加
- Windows
  - `prod/lib` / `prod/bin` が必要なら `PATH` に追加

#### `.vscode/tasks.json`

`make test` と `make test (current dir)` の両方を更新します。  
どちらか片方だけ更新すると、再チェック時に漏れになります。

#### `.vscode/launch.json`

少なくとも以下の構成を更新対象として確認します。

- `(Linux gdb) Debug current directory`
- `(Windows vsdbg) Debug current directory`
- `(Linux coreclr) Debug current directory`
- `(Windows coreclr) Debug current directory`

### 3. GitHub Actions を更新する

`ci.yml` の Linux / Windows で更新箇所が異なります。

#### Linux

- `LD_LIBRARY_PATH` は `$GITHUB_ENV` に書く
- `bin` ディレクトリは `$GITHUB_PATH` に追加する

#### Windows

- `PATH` に追加するパス配列へ `lib` / `bin` を入れる
- `com_util` のように `lib` だけ必要なアプリは `lib` のみ追加する

### 4. Jenkins を更新する

Jenkins を利用する場合は、GitHub Actions と同じ観点で以下を更新します。

- `.jenkins/inner-build.sh`
  - Linux の `LD_LIBRARY_PATH`
  - Linux の `PATH`
  - `results` / `warn` の収集対象パス
- `.jenkins/README.md`
  - 実装に合わせた説明

### 5. 個別 README の実行例を更新する

利用者向けの README に、古い `prod/...` パスや手動 `LD_LIBRARY_PATH` 設定例が残っていないかを確認します。  
特に、アプリ固有 README のトラブルシュート節は見落としやすい箇所です。

## 再チェック用チェックリスト

- `app/<name>/prod/lib` と `app/<name>/prod/bin` の有無を確認した
- 依存関係上、本当に環境変数へ入れる必要があるか判断した
- `.vscode/settings.json` を更新した
- `.vscode/tasks.json` の `make test` と `make test (current dir)` を両方更新した
- `.vscode/launch.json` の Linux / Windows 構成を更新した
- `.github/workflows/ci.yml` の Linux / Windows を更新した
- Jenkins を使う場合は `.jenkins/inner-build.sh` と `.jenkins/README.md` を更新した
- 個別 README の実行例やトラブルシュートを更新した
- 旧 `prod/...` 構成が残っていないことを確認した
- 代表的なテストまたは実行例でライブラリ解決エラーが出ないことを確認した

## 確認コマンド例

### 実在する `lib` / `bin` の確認

```bash
find app -maxdepth 3 \( -path '*/prod/lib' -o -path '*/prod/bin' \) | sort
```

### 設定箇所の確認

```bash
rg -n "LD_LIBRARY_PATH|GITHUB_ENV|GITHUB_PATH|terminal.integrated.env|Set PATH for tests|Set PATH and library path for tests" \
  .vscode .github .jenkins -S
```

### 旧構成の残存確認

```bash
rg -n '\$\{workspaceFolder\}/prod/|/workspace/prod/|/path/to/.*/prod/' \
  docs .vscode .github .jenkins app -S
```

### 依存関係の確認

```bash
rg -n "LIBS \\+=|LIBSDIR \\+=|DllImport|NativeLibrary|include <com_util" app -S
```

### 代表テストの最小確認

Linux では、ライブラリ探索パスを明示して代表テストの起動確認ができます。

```bash
LD_LIBRARY_PATH=$PWD/app/calc/prod/lib:$PWD/app/calc.net/prod/lib:$PWD/app/override-sample/prod/lib:$PWD/app/porter/prod/lib:$PWD/app/com_util/prod/lib:${LD_LIBRARY_PATH} \
  ./app/calc/test/src/main/addTest/bin/addTest --gtest_list_tests
```

## 現在の VS Code / CI 設定を見る場所

日常的な保守では、次のファイルをあわせて確認してください。

- `.vscode/settings.json`
- `.vscode/tasks.json`
- `.vscode/launch.json`
- `.github/workflows/ci.yml`
- `.jenkins/inner-build.sh`

GitHub Actions は「現在の必要十分な実装例」として有用ですが、それだけを正本にしないことが重要です。  
必ず `app` 配下の実在ディレクトリと依存関係に立ち戻って判断してください。
