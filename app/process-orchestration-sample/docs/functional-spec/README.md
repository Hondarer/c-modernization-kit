# process-orchestration-sample 機能仕様

## この文書群の役割

`functional-spec/` は、利用側または上位設計から見た process-orchestration-sample の責務を、機能カテゴリごとに説明する文書群です。  
process-orchestration-sample が解決する課題、満たす要件、外部から観測できる振る舞い、前提、保証範囲、適用範囲外を定めます。

機能仕様は、API 設計、実装設計、実装、テストに渡す入力です。  
記載範囲、章立て、要件 ID と UUID の運用は [process-orchestration-sample 機能仕様の記載規範](../functional-spec-guideline.md) に従います。

## 文書の位置付け

機能仕様は、関数の一覧、実装方式、テスト方法を扱いません。  
下流成果物は、設計判断または検証対象の根拠を示す必要がある場合に、機能仕様の要件 ID と UUID を参照します。

同じ階層にある機能仕様は、責務の境界または選択方法を説明する場合に相互参照します。

## 機能仕様一覧

Table: process-orchestration-sample の機能仕様一覧と役割

| 機能仕様 | process-orchestration-sample が担う役割 |
|---|---|
| [同期 IPC (ipc)](ipc.md) | 同一ホスト上のプロセス間同期要求応答通信、単発接続と持続接続、エンドポイント空間、同時接続と資源保護、OS 間の互換性 |
| [プロセスとりまとめ (orchestration)](orchestration.md) | 親プロセスによる子プロセス 1〜3 の起動・状態管理・異常終了検知 (自動再起動なし)・グレースフル停止、および親を経由しない子への直接エコー バック |
| [コマンド ライン (cli_tools)](cli_tools.md) | 親プロセス・子プロセス・第三者制御・性能比較ベンチマークのコマンド ライン実行と出力形式 |

## 変更時の扱い

process-orchestration-sample の機能、外部から観測できる振る舞い、前提、保証範囲、適用範囲外を変更する場合は、対応する機能仕様を先に変更します。  
要件 ID と UUID の追加、維持、廃止は [process-orchestration-sample 機能仕様の記載規範](../functional-spec-guideline.md) に従います。
