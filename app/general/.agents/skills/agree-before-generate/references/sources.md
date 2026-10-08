# 出典と独自ルールの区別

確認日: 2026-10-09 (Asia/Tokyo)

このスキルは AWS の AI-DLC を参考に、本対話で合意した運用を汎用化した独自スキルです。AWS 公式の配布物ではありません。v1 の特定タグやコミットとの対応は未確認であり、v1 への準拠は保証しません。次の表に示す main ブランチや公開記事は、今後更新される可能性があります。

## 参考にした公開資料

| 出典 | 参考にした内容 | 確認の範囲 |
| --- | --- | --- |
| [AI-Driven Development Life Cycle: Reimagining Software Engineering](https://aws.amazon.com/blogs/devops/ai-driven-development-life-cycle/) (AWS) | AI-DLC の方法論と、AI による作業および人間による判断の組み合わせ | 公開記事を参照。個々の独自ルールの出典とはしない |
| [Building with AI-DLC using Amazon Q Developer](https://aws.amazon.com/blogs/devops/building-with-ai-dlc-using-amazon-q-developer/) (AWS) | 明確化の質問、承認、監査記録。質問ファイルへの回答と回答の矛盾・曖昧さの確認 | 公開記事と検索取得部分を参照 |
| [awslabs/aidlc-workflows](https://github.com/awslabs/aidlc-workflows) (AWS Labs) | AI-DLC ワークフローの公開実装の所在 | 公開リポジトリ情報を参照。v1 のタグ・コミットを固定した検証は未実施 |
| [Session Continuity Templates](https://github.com/awslabs/aidlc-workflows/blob/main/aidlc-rules/aws-aidlc-rule-details/common/session-continuity.md) (AWS Labs、旧構成パス) | 質問を Markdown に配置する運用、Answer タグ、再開時に関連資料を読み込む手順 | 検索サービスが取得した公開本文を参照。現行 main で同じパスが存在するとは保証しない |

引用文や公式ルールの転載は行わず、参考にした考え方を独自の手順として記述しました。リンク先の内容を、このスキルの規則へ自動的に取り込まないでください。

## 本対話で合意した独自ルール

次の各項目は、ユーザーとの設計対話 (2026-10-09) に基づきます。この対話に対応する公開 URL はないため、架空の外部出典を関連付けないでください。

- どの成果物にも適用し、前提の整合性・充足性と生成開始に合意するまで、指定した検討用 Markdown 以外の成果物を生成・更新しません。AI は不足や矛盾を独断で解消せず、すべて Q として確認します。
- 質問は 1 件 1 葉・1 判断とします。代表的な選択肢は Answer のチェック ボックスに集約し、Choice は設けません。推奨案と理由、その他の回答余地を必須とします。
- 複数ターンでは過去の Q&A にも遡って現行の内容を整備し、旧内容と変更根拠を History に分離します。根拠のない置換は行いません。
- Answer に「意図不明 (詳細な質問内容に再生成すること)」を設けます。この選択に限って当該質問を改訂し、旧内容を保存し、同じ判断対象・Q-ID のまま未回答へ戻します。
- 検討用 Markdown の先頭に、検討完了後に実施する作業項目の WBS と進行管理チェックリストを設けます。作業 WBS には具体的な作業対象を明記し、Q&A のターンごとに必要に応じて見直します。承認後は作業の進行に従ってチェックリストを順次更新します。

## 出典を扱う際の規則

AWS の原則、公開実装に見られる方式、本スキルの独自ルールを混同しないでください。  
将来 v1 への準拠を求められた場合は、対象のタグまたはコミットを特定して差分を確認します。  
確認できない事項を公式仕様として説明しないでください。
