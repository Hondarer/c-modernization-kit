---
name: edit-vscode-env
description: app の追加・削除や出力先変更に伴う探索パスの更新、または DLL・SO が見つからない問題の調査に使います。
---

# VS Code 環境変数の調査と更新

`app/general/docs/vscode-variables.md` の対象設定の節を参照し、`makepart.mk` の `OUTPUT_DIR` と app 依存関係を正本として確認してください。

調査では、対象プロセスの探索パスと生成設定を比較し、必要ならルートで `bash app/general/bin/sync-app-env.sh --check` を実行してください。  
調査だけの依頼では、設定の再生成を行わないでください。

更新が必要で依頼範囲に含まれる場合は、ルートで `make sync-app-env` を実行し、`bash app/general/bin/sync-app-env.sh --check` と差分で確認してください。  
生成ファイルは手編集せず、Linux の `LD_LIBRARY_PATH` と Windows の `PATH` への影響を確認してください。  
探索パスの順序を変える場合は、同名ライブラリの解決順も確認してください。
