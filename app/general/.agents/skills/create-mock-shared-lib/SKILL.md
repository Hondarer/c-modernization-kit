---
name: create-mock-shared-lib
description: 第三者共有ライブラリの公開 API 表から、実関数へ既定委譲する Google Mock 対応 mock を追加・変更します。
---

# 第三者共有ライブラリの mock

`app/general/docs/shared-library-mock-guideline.md` の API 表と実委譲の節、および対象ライブラリの README にある固有設定を確認してください。  
可変長引数、公開変数、Windows の import 属性を扱う場合は、対応する節も参照してください。

API 表から宣言、`MOCK_METHOD`、real delegate、ラッパー、`ON_CALL` を生成してください。  
既定動作は実ライブラリへの委譲とし、解決失敗時は理由を出力して終了してください。  
利用側は実ライブラリと mock を同時リンクせず、Windows では import 属性を解除してください。  
mock の `makepart.mk` を作成・変更する場合は、`framework/testfw/docs/how-to-mock.md` の「モック ライブラリのビルド設定」を確認してください。

変更した API の網羅テストと、実委譲および差し替えを確認する局所テストを実行してください。
