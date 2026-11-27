# 実機検証手順書 (Windows 性能比較および共通サービス空間)

本書は、Linux (WSL) 環境では直接実行できない Windows/MSVC でのビルド・テスト・トランスポート性能比較 (`AF_UNIX` vs 名前付きパイプ)、および将来フェーズで共通サービス空間 (`POS_IPC_SCOPE_SYSTEM`) を実装・検証する際の確認手順をまとめます。

## Windows/MSVC でのビルドと自動テスト

### 前提環境

- Windows 10 (1803 以降) または Windows 11 (`afunix.h` によるネイティブ `AF_UNIX` ソケット対応)
- Visual Studio 2022 (MSVC ツールセット) および `nmake` / GNU `make` (ワークスペース標準のビルド環境)
- リポジトリ ルートの `.vscode/.env.windows` に定義された環境変数 (`MAKEFW_HOME`、`TESTFW_HOME`、`DOXYFW_HOME`、`DOCSFW_HOME`、`PATH` 等) が読み込まれていること

### ビルド手順

1. リポジトリ ルートでワークスペース環境変数の同期状態を確認します。

```powershell
bash framework/makefw/bin/sync_c_cpp_properties.sh --check
```

2. `app/process-orchestration-sample` へ移動してビルドします。

```powershell
cd app/process-orchestration-sample
make
```

3. ビルド後、内容のある `.warn` ファイルが存在しないことを確認します。

```powershell
Get-ChildItem -Recurse -Filter *.warn | Where-Object { $_.Length -gt 0 }
```

### 自動テスト実行 (AF_UNIX および NAMED_PIPE)

`app/process-orchestration-sample` 配下で `make test` を実行します。

```powershell
cd app/process-orchestration-sample
make test
```

- `ipcUnitTest` は Windows 環境でパラメーター化テストとして `POS_IPC_BACKEND_AF_UNIX` と `POS_IPC_BACKEND_NAMED_PIPE` の双方に対して同一のテスト ケース群を実行します。
- `orchestrationE2ETest` は実バイナリ (`orchestrator-parent.exe`、`orchestrator-child.exe`、`orchestrator-ctl.exe`、`orchestrator-bench.exe`) を起動し、親・子ライフサイクルと異常終了時の非自動再起動 (`CRASHED` 状態保持)・再起動復帰を検証します。

## Windows でのトランスポート性能比較ベンチマーク

`orchestrator-bench.exe` を用いて、同一マシン・同一条件で `AF_UNIX` と `NAMED_PIPE` の単発接続 (`oneshot`)・持続接続 (`persistent`) レイテンシとスループットを比較します。

### 計測コマンド例

```powershell
$bench = "app/process-orchestration-sample/prod/cbin/orchestrator-bench.exe"

# 1. 小ペイロード (256 バイト)・単一接続
& $bench --backend af_unix    --mode both --iterations 2000 --payload-size 256  --connections 1
& $bench --backend named_pipe --mode both --iterations 2000 --payload-size 256  --connections 1

# 2. 小ペイロード (256 バイト)・並列 4 接続
& $bench --backend af_unix    --mode both --iterations 4000 --payload-size 256  --connections 4
& $bench --backend named_pipe --mode both --iterations 4000 --payload-size 256  --connections 4

# 3. 中ペイロード (64 KiB)・単一接続
& $bench --backend af_unix    --mode both --iterations 1000 --payload-size 65536 --connections 1
& $bench --backend named_pipe --mode both --iterations 1000 --payload-size 65536 --connections 1
```

### 計測結果の記録テンプレート

Table: Windows トランスポート性能比較結果の記録欄

| バックエンド | モード | ペイロード [byte] | 並列接続数 | 反復回数 | avg [us] | p50 [us] | p95 [us] | p99 [us] | スループット [req/s] |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `af_unix` | `oneshot` | 256 | 1 | 2000 | - | - | - | - | - |
| `af_unix` | `persistent` | 256 | 1 | 2000 | - | - | - | - | - |
| `named_pipe` | `oneshot` | 256 | 1 | 2000 | - | - | - | - | - |
| `named_pipe` | `persistent` | 256 | 1 | 2000 | - | - | - | - | - |
| `af_unix` | `oneshot` | 65536 | 1 | 1000 | - | - | - | - | - |
| `af_unix` | `persistent` | 65536 | 1 | 1000 | - | - | - | - | - |
| `named_pipe` | `oneshot` | 65536 | 1 | 1000 | - | - | - | - | - |
| `named_pipe` | `persistent` | 65536 | 1 | 1000 | - | - | - | - | - |

計測結果は `app/cplat` への正式統合時 (`BL-002`) に Windows 既定バックエンドの最終判断根拠として参照します。

## 将来フェーズ向け: 共通サービス空間 (POS_IPC_SCOPE_SYSTEM) の検証観点

本 PoC では `POS_IPC_SCOPE_SYSTEM` を公開型として予約し、`CPLAT_ERR_UNSUPPORTED` を返却しています (`BL-001`)。将来フェーズで daemon / Windows Service 等の異ユーザー間通信を実装する際は、以下の観点を実機で検証します。

### Linux (systemd daemon + 一般ユーザー クライアント)

- **配置パス**: `/run/<namespace>/<instance>/<target>.sock` (`RuntimeDirectory` で事前作成)
- **権限検証**:
    - `POS_IPC_ACCESS_GROUP` 指定時に親ディレクトリが `0750`、ソケット ファイルが `0660` + 指定グループ所有となり、所属グループ ユーザーから接続可能、非所属ユーザーから `CPLAT_ERR_PERMISSION_DENIED` で拒否されること。
    - `POS_IPC_ACCESS_AUTHENTICATED_USERS` 指定時に一般ローカル ユーザーから接続可能であり、`SO_PEERCRED` でクライアントの UID / GID / PID をサーバー側で取得できること。

### Windows (Windows Service LocalSystem / 仮想サービス アカウント + 一般ユーザー クライアント)

- **配置パス**:
    - `AF_UNIX`: `%ProgramData%\<namespace>\run\<instance>\<target>.sock`
    - `NAMED_PIPE`: `\\.\pipe\<namespace>\system\<instance>\<target>`
- **権限検証**:
    - サーバー生成時に `SECURITY_ATTRIBUTES` (SDDL DACL) を明示設定し、`LocalSystem` / `Administrators` にフル制御、許可対象 (`Authenticated Users` または指定グループ SID) に必要最小限の読み書き権限を付与すること。
    - 権限のないアカウント (例: 制限付きアカウント / 非所属グループ) からの接続が `CPLAT_ERR_PERMISSION_DENIED` で拒否されること。
    - 必要に応じて `GetNamedPipeClientProcessId` / `SIO_AF_UNIX_GETPEERPID` によりクライアント PID を識別できること。
