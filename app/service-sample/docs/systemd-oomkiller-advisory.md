# Linux systemd OOM killer 対策アドバイザリー

この文書では、Linux の systemd unit で管理されるプロセスが OOM killer により停止されにくくするための設定を説明します。

## 一般的な考え方

Linux では、OOM による停止経路を 2 つに分けて考える必要があります。

| 種類 | 主な設定 | 説明 |
|---|---|---|
| カーネル OOM killer | `OOMScoreAdjust=` | メモリ不足がカーネル空間で限界に達したとき、プロセスの OOM score をもとに停止対象を選ぶ。 |
| `systemd-oomd` | `ManagedOOMPreference=` | cgroup v2 と PSI を使い、カーネル OOM より前に cgroup 単位で停止対象を選ぶ。 |

カーネル OOM killer への対策には `OOMScoreAdjust=-1000` を設定します。  
systemd の説明では、この値は対象 unit のプロセスに対する OOM killing を無効化する値です。

`systemd-oomd` への対策には `ManagedOOMPreference=omit` を設定します。  
この設定は、`systemd-oomd` が停止候補を選ぶときに対象 unit の cgroup を候補から除外するための設定です。  
`ManagedOOMPreference=` は systemd 248 以降で利用できます。

## 推奨 unit 設定

systemd 248 以降では、次の設定を `[Service]` に追加します。

```ini
[Service]
OOMScoreAdjust=-1000
ManagedOOMPreference=omit
```

systemd 247 以前では、`ManagedOOMPreference=` が未知の設定となるため、次の設定にします。

```ini
[Service]
OOMScoreAdjust=-1000
```

## fork されたプロセスへの作用

systemd の service unit から fork された子プロセスは、通常は親プロセスと同じ service cgroup に残ります。  
この場合、`OOMScoreAdjust=-1000` は fork された子プロセスにも適用され、`systemd-oomd` の停止候補からも unit 単位で除外されます。

ただし、次の場合は子プロセスにも同等の設定が必要です。

- 子プロセスを `systemd-run` などで別 unit または scope として起動する場合。
- 子プロセス自身、または外部の管理処理が cgroup を移動する場合。
- コンテナーやユーザー サービスなど、別の systemd インスタンスに処理を委譲する場合。

`systemd-oomd` は cgroup を単位に停止対象を選びます。  
そのため、保護したい処理が保護対象 unit の cgroup 内に残っていることを確認してください。

## 既存 unit への適用手順

既存の unit には drop-in で設定を追加します。

```bash
sudo systemctl edit <unit-name>
```

systemd 248 以降では、次の内容を追加します。

```ini
[Service]
OOMScoreAdjust=-1000
ManagedOOMPreference=omit
```

systemd 247 以前では、次の内容を追加します。

```ini
[Service]
OOMScoreAdjust=-1000
```

設定後、unit を再読み込みしてサービスを再起動します。

```bash
sudo systemctl daemon-reload
sudo systemctl restart <unit-name>
```

## service-sample での実装

`service-sample` の Linux 実装は、install 時に `/etc/systemd/system/service-sample.service` を生成します。

生成 unit では、カーネル OOM killer 向けに `OOMScoreAdjust=-1000` を常に出力します。  
また、install 時に `systemctl --version` で systemd の major version を確認し、systemd 248 以降では `ManagedOOMPreference=omit` も出力します。

systemd 248 以降で生成される unit の `[Service]` は次の形です。

```ini
[Service]
Type=notify
ExecStart=<service-sample の絶対パス> run
Restart=on-failure
RestartSec=5
OOMScoreAdjust=-1000
ManagedOOMPreference=omit
```

systemd 247 以前、または systemd のバージョン取得に失敗した環境では、互換性を優先して `ManagedOOMPreference=omit` を省略します。

```ini
[Service]
Type=notify
ExecStart=<service-sample の絶対パス> run
Restart=on-failure
RestartSec=5
OOMScoreAdjust=-1000
```

コンソール モードで直接起動したプロセスは systemd unit の管理外になるため、この unit 設定は適用されません。

## service-sample での確認方法

systemd のバージョンを確認します。

```bash
systemctl --version
```

unit に適用された設定を確認します。

```bash
systemctl show service-sample -p OOMScoreAdjust -p ManagedOOMPreference
```

実行中プロセスの `oom_score_adj` を確認します。

```bash
pid=$(systemctl show service-sample -p MainPID --value)
cat /proc/${pid}/oom_score_adj
```

`-1000` が表示されれば、カーネル OOM killer 向けの調整値が適用されています。

fork された子プロセスが同じ cgroup に残っているか確認します。

```bash
systemd-cgls /system.slice/service-sample.service
```

`systemd-oomd` の監視状態を確認できる環境では、次のコマンドも使えます。

```bash
oomctl
```

## 注意事項

`OOMScoreAdjust=-1000` と `ManagedOOMPreference=omit` は、対象 unit を停止対象にしにくくする設定です。  
システム全体のメモリ不足を解消する設定ではありません。

保護対象を増やしすぎると、他の unit が停止されやすくなります。  
恒久運用では、メモリ使用量の上限、再試行間隔、ジョブ ログ、監視メトリクスを別途設計してください。

`MemoryMax=` は unit のメモリ使用量が上限を超えた場合に unit 内で OOM killer を呼び出す設定です。  
停止させたくないプロセスを保護する目的では、このアドバイザリーの主対策としては使いません。

`ManagedOOMSwap=kill` や `ManagedOOMMemoryPressure=kill` は、対象 cgroup を `systemd-oomd` の監視対象にする設定です。  
停止回避を目的にする場合、保護対象 unit 自身へこれらを設定する必要はありません。

## 参考

- systemd.exec: <https://www.freedesktop.org/software/systemd/man/latest/systemd.exec.html>
- systemd.resource-control: <https://www.freedesktop.org/software/systemd/man/latest/systemd.resource-control.html>
- systemd-oomd.service: <https://www.freedesktop.org/software/systemd/man/latest/systemd-oomd.service.html>
