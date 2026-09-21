---
name: create-mock
description: 通常の app C 関数へ Google Mock 対応 mock を追加・変更します。専用の委譲方式や API 表生成には対象固有の手順を使います。
---

# app mock の作成

対象パスに専用スキルの指定があれば、その手順を使用してください。  
API 表から生成する第三者共有ライブラリには `create-mock-shared-lib` を使用してください。

通常の app 関数では、`framework/testfw/docs/how-to-mock.md` の宣言、既定動作、ラッパーの該当節を確認してください。  
配置が不明な場合は `app/general/docs/testing-tutorial.md` の mock 節を参照してください。

override ヘッダー、Mock クラス、`ON_CALL`、関数ラッパーのシグネチャを一致させ、app 関数と同名のラッパーには `MOCK_WEAK_IMPL` を使用してください。  
MSVC では、テスト翻訳単位が mock ヘッダーをインクルードすることで `/INCLUDE` と `/ALTERNATENAME` により実装オブジェクトが取り込まれます。一方の環境のみで成功した結果を、他方の環境におけるリンク確認の代用として扱わないでください。  
ソースやリンク先を変更する場合は `makepart.mk` も更新し、変更した mock を使用する局所テストで確認してください。
