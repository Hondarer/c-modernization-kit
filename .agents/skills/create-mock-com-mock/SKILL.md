---
name: create-mock-com-mock
description: |
  com_util のモックである mock_com_util を実装・保守するときに使うスキルです。
  libcom_util の動的ロード、既定動作、本物未接続時の扱い、失敗時終了方針をまとめます。
when_to_use: |
  - app/com_util/test 配下で mock_com_util の関数を追加・更新するとき
  - mock_com_util の既定動作や未注入時動作を確認したいとき
---

# mock_com_util 向け real delegate

このスキルは `app/com_util/test/` 配下の `mock_com_util` に適用します。
通常の app mock 作成ルールとは異なるため、`create-mock` の一般ルールよりこちらを優先します。
`mock_com_util` は全関数でこの方針を採用します。

## 方針

- `mock_com_util` では全関数について `delegate_real_<func>` を用意します。
- `_mock_com_util == nullptr` のときは全関数で本物の `libcom_util` へ移譲します。
- `Mock_com_util` が注入されていても、全関数の `ON_CALL` 既定動作は `Invoke(delegate_real_<func>)` にします。
- テストコード側で `EXPECT_CALL` / `ON_CALL` を上書きし、`com_util` 単体および `com_util` を利用するモジュールの単体テストを行うことを基本とします。
- testfw 下では実装がある `delegate_fake` や real と fake の切替 API は作りません。
- 本物の呼び出しは `framework/testfw/include/sharedLibrary.h` の `resolveSharedSymbolOrExit()` を使います。
- ライブラリ探索は `.vscode` で設定された `LD_LIBRARY_PATH` / `PATH` に委ねます。
- attach やシンボル解決に失敗した場合は `stderr` に理由を出し、`exit(1)` で終了します。

## 実装箇所

- 宣言追加: `app/com_util/test/include/mock_com_util.h`
- 既定動作設定: `app/com_util/test/libsrc/mock_com_util/mock_com_util.cc`
- 関数本体: `app/com_util/test/libsrc/mock_com_util/<module>/mock_com_util_<func>.cc`
- 共通ローダ: `framework/testfw/include/sharedLibrary.h`
- 共通ローダ実装: `framework/testfw/libsrc/test_com/sharedLibrary.cc`

## 実装形

- `delegate_real_<func>` は static な関数ポインタを保持し、初回のみ `resolveSharedSymbolOrExit()` で解決します。
- `sharedLibrary.h` で `TESTFW_SHARED_LIBRARY_EXTENSION` を定義し、`mock_com_util.h` では `kLibComUtilName` を `"libcom_util" TESTFW_SHARED_LIBRARY_EXTENSION` で 1 回だけ定義します。
- Windows のライブラリ読み込みは UTF-8 を UTF-16 に変換して `LoadLibraryW` を使います。
- シンボル名は export 名として `GetProcAddress` に渡します。
- 実関数の weak wrapper は、未注入時に `delegate_real_<func>` を呼び、mock 注入時は Mock クラスの指定を優先します。
- Mock クラス側で個別指定がない場合は `ON_CALL` の既定動作により再び `delegate_real_<func>` が使われます。

## テスト

- 本物への移譲確認は `app/com_util/test/src/libcom_utilTest/` 配下のテストで行います。
- デザインパターン化されているため、未注入時の動作、`Mock_com_util` 注入済み既定動作、`EXPECT_CALL` / `ON_CALL` による上書き、必要に応じてローダ失敗診断は `app/com_util/test/src/libcom_utilTest/compress/compressDelegateRealTest` の代表実施とします。
  この仕組みが正しく動作していない場合は、各テストが期待通り動作しなくなることでじゅうぶんに確認できます。
