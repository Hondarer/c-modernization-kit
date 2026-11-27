# process-orchestration-sample 機能仕様の記載規範

## 概要

本書は、process-orchestration-sample の機能仕様に固有の設定と追加規則を定めます。  
対象は `docs/functional-spec/` 配下の機能仕様です。

記載する内容、記載しない内容、章立て、記載粒度、要件 ID と UUID の運用、下流成果物での参照記法は、[機能仕様の記載規範](../../general/docs/functional-spec-guideline.md) に従います。  
本書は、同規範が app 固有規範に委ねる項目だけを定めます。

## 要件 ID の構成

Table: process-orchestration-sample 要件 ID の構成要素

| 項目 | 値 |
|---|---|
| 要件 ID 接頭辞 | `POS` |
| 参照コメント タグ | `pos-req` |
| 機能要件表の見出し | `process-orchestration-sample の要件` |

要件 ID は `POS-<CATEGORY>-<TYPE>-NNN` の形式になります。  
例を次に示します。

```text
POS-IPC-FUNC-001
POS-ORCHESTRATION-QUAL-001
```

## カテゴリごとの主語

要件文は、カテゴリに対応する次の主語で始めます。

Table: 機能仕様カテゴリごとの要件文の主語

| カテゴリ | 主語 |
|---|---|
| `IPC` | process-orchestration-sample の同期 IPC 機能 |
| `ORCHESTRATION` | process-orchestration-sample のプロセスとりまとめ機能 |
| `CLI_TOOLS` | process-orchestration-sample のコマンド ライン機能 |

カテゴリは、`docs/functional-spec/` に配置する機能仕様のファイル名と一対一で対応します。  
機能カテゴリを追加または削除する場合は、この表も同じ変更で更新してください。

## process-orchestration-sample 固有の追加規則

process-orchestration-sample は、下位の OS 抽象やプロセス制御を基盤層を通じて利用しますが、機能仕様には依存先の app 名や API 名を記載しません。  
通信電文やエンドポイントの命名など相互運用性のために固定する条件は、内部構造体や実装方式ではなく、通信の両端から観測できる形式と制約として記載します。

## 確認

機能仕様の変更後は、[機能仕様の記載規範](../../general/docs/functional-spec-guideline.md) の「確認」に記載されている項目を確認します。

要件 ID と UUID の形式および参照関係は、ワークスペース ルートで次のコマンドを実行して確認します。  
検査は、機能仕様が配置されているすべての app を対象とします。

```shell
python3 app/general/bin/check_functional_spec.py
```
