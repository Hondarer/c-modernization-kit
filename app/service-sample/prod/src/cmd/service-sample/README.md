---
short-title: service-sample
---

# service-sample

`cplat/service/service.h` を利用して、Windows サービス (SCM) と Linux systemd の両方に対応するクロスプラットフォーム サービス (デーモン) の C 言語サンプルです。

## 概要

サービス登録・解除・常駐実行・停止抽象・OS イベント監視・設定再読込・watchdog 応答といった共通基盤は `app/cplat` (`cplat/service/service.h`) が提供します。  
本サンプルは、CLI エントリ ポイント (`service-sample.c`) とライフサイクル コールバックの雛形 (`service-sample-impl.c`) だけを持つ薄いアプリケーション構成を示します。

詳細なイベント仕様・watchdog 契約・自動再起動・OOM killer 対策については、`app/cplat` の正本ドキュメントを参照してください。

- [サービス イベントのタイミング](../../../../../cplat/docs/service-events.md)
- [Linux systemd OOM killer 対策アドバイザリ](../../../../../cplat/docs/systemd-oomkiller-advisory.md)
- [cplat サービス機能仕様](../../../../../cplat/docs/functional-spec/service.md)

### コマンド ライン

```bash
service-sample install    # OS にサービスを登録する (Windows は UAC 昇格対応)
service-sample uninstall  # OS からサービスを解除する (Windows は UAC 昇格対応)
service-sample run        # サービスとして起動する (SCM/systemd から呼ばれる)
service-sample console    # フォアグラウンドで実行する (デバッグ用)
```

### ライフサイクル コールバック

Table: service-sample のライフサイクル コールバック一覧

| コールバック | 呼ばれるタイミング | 実装すること | 必須 |
|---|---|---|---|
| `on_start` | サービス起動時に 1 回 | 初期化処理。0 を返すと `on_run` が呼ばれ、0 以外を返すと起動を中断します。 | 任意 |
| `on_run` | `on_start` 成功後に 1 回 | `cplat_service_wait_for_stop()` が `CPLAT_ERR_TIMEOUT` を返す間繰り返すメイン ループ。 | 必須 |
| `on_stop` | `on_run` が戻った後に必ず 1 回 | 停止処理。 | 任意 |
| `on_event` | 電源・セッション・シャットダウン前イベント発生時 | OS イベント応答処理 (別スレッド呼び出し)。 | 任意 |
| `on_reload` | 設定再読込要求時 | 設定再読込処理 (別スレッド呼び出し)。 | 任意 |

## ファイル構成

```text
prod/src/cmd/service-sample/
+-- makefile               # ビルドの入口 (makefw のテンプレート)
+-- README.md              # この文書
+-- service-sample.h       # サンプル固有の内部共有宣言
+-- service-sample.c       # CLI エントリ ポイント (引数解析、run_piped 権限昇格、cplat_service_* 呼び出し)
+-- service-sample-impl.c  # サービス定義 (g_service_def) とライフサイクル コールバックのサンプル実装
```

## ビルド

```bash
make -C app/service-sample
```

## 実行

### Linux: コンソール モード (デバッグ用)

```bash
./app/service-sample/prod/cbin/service-sample console
```

### Linux: systemd サービスとして登録・実行

```bash
sudo ./app/service-sample/prod/cbin/service-sample install
sudo systemctl start service-sample
sudo systemctl stop service-sample
sudo ./app/service-sample/prod/cbin/service-sample uninstall
```

### Windows: サービスとして登録・実行

```powershell
.\app\service-sample\prod\cbin\service-sample.exe install
sc.exe start service-sample
sc.exe stop service-sample
.\app\service-sample\prod\cbin\service-sample.exe uninstall
```
