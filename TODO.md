# ToDo

## toc 機能で除外指定がうまく動作していない

要デバッグ。

## toc 機能で、基準を配下にしたい場合がある

## gtest のライブラリ

- /MD, /MT, /MDd, /MTd の対象ごとの LIB フォルダは、処理で切り分ける必要あり。

## Windows の ln -s

TEST_SRCS などでリンク指定して ln -s を発行しても、Windows ではコピーになってしまう。

## Windows の main

Windows では、main のすげ替えができないので、コピーしながら置換する必要がある。

以下を、目的に置換し、対応するプラットフォーム別の処理を書く必要がある。

```makefile
# リンクオプションの追加
# -Wl,--wrap=main により、エントリポイントを __wrap_main() に、元々のエントリポイントを __real_main() に変更
LDFLAGS += -Wl,--wrap=main

# ライブラリの追加
# gtest_wrapmain にて、__wrap_main() 経由でのテスト実施
# テスト対象のソースファイルにある main() は実行されない
LIBS += gtest_wrapmain mock_libc mock_calcbase test_com
```
