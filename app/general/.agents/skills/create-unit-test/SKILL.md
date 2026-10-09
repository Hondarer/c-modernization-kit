---
name: create-unit-test
description: app 配下の C/C++ 単体テストを作成・変更・レビューする際に、testfw 固有の配置とエビデンス規則を適用します。
---

# app 単体テストの作成

作業に応じて次の節を参照してください。

- 配置やビルド対象を変更する場合は `framework/testfw/docs/how-to-test.md` の `TEST_SRCS`、`ADD_SRCS`、main の扱いと、必要なら `app/general/docs/testing-tutorial.md` の配置例
- フェーズやエビデンス コメントを変更する場合は `framework/testfw/docs/about-test-phase.md` の該当するテスト形式と「期待を確認する行為 1 回を 1 件として集計する」

ループ、条件分岐、パラメーター テストの確認回数は、アルゴリズムとテスト データから決定して記載してください。  
共通関数や fixture の状態、手順、確認を含める場合は `framework/testfw/docs/test-subprocedures.md` に従ってサブ手順を定義し、テストから参照してください。  
呼び出し側へ内部の確認件数を転記せず、参照の実施回数を記載してください。

- 期待値や照合方法を変更する場合は `framework/testfw/docs/how-to-expect.md` の該当するマクロ
- mock を新設する場合は、対象パスの指示にある専用スキル、または通常の app 関数向け `create-mock`
- mock ライブラリをリンクする場合は `framework/testfw/docs/how-to-test.md` の「GoogleTest / GoogleMock は testfw.h 経由で取り込む」を参照し、対応する mock ヘッダーをテスト翻訳単位がインクルードしていることの確認

既存の近いテストと対象の契約を確認し、要求された振る舞いを検証してください。  
Linux と Windows でモックのリンク方式が異なるため、一方の環境のみで成功した結果を、他方の環境におけるリンク確認の代用として扱わないでください。  
変更後は影響する局所テストを実行し、網羅性の判断に必要ならカバレッジを確認してください。  
レビューのみの場合は、指摘を裏付ける確認方法を選択し、テストの追加や全面的な再実行を前提にしないでください。
