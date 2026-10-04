---
name: release-tag
description: Hondarer 配下の指定リポジトリ群に、日付タグと GitHub Release を一括作成する場合に使います。
---

# リリース タグと GitHub Release の作成

`app/c-modernization-kit/docs/release-workflow.md` の事前確認と公開条件に従ってください。  
対象一覧の正本と実行処理は、このスキルの `scripts/release.py` です。

`--plan TAG` で全対象の SHA、同名タグ、作成・スキップ判定を確認し、具体的な公開内容をユーザーへ提示してください。  
c-modernization-kit の特定コミットを基準にする依頼では `--workspace-sha SHA` を追加し、サブモジュールの対象をそのピン留め SHA へ追従させてください。  
公開が許可されている場合だけ、保存した計画に対して `--apply APPROVED_PLAN` を実行してください。  
すでに同じ対象とタグの公開が許可されていれば、再確認は不要です。

公開するタグは確認した SHA に固定してください。  
取得失敗や重複タグでは公開へ進まず、公開中の失敗は後続を停止して結果を報告してください。  
自動再試行やロールバックは行わず、作成済みの対象と未完了の対象を区別してください。
