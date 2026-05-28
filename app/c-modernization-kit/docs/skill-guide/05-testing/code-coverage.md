# コード カバレッジ

## 概要

コード カバレッジは、テストがソース コードのどの割合を実行しているかを計測する指標です。行カバレッジ・分岐カバレッジなどの種類があり、テストの網羅性の確認や未テスト箇所の特定に使います。100% を目指すことが目的ではなく、重要なコード パスが網羅されているかを確認するために活用します。

Linux 環境では gcov (GCC 付属のカバレッジ ツール) と lcov (gcov の結果を HTML レポートに変換するツール) または gcovr を組み合わせて使用します。Windows 環境では OpenCppCoverage が利用できます。

このリポジトリの `framework/testfw/` サブモジュール (論理名: `testfw`) はカバレッジ計測とレポート生成の機能を提供しています。テストをカバレッジ計測付きでビルドするには、コンパイル時に `--coverage` (または `-fprofile-arcs -ftest-coverage`) フラグが必要です。

## 習得目標

- [ ] コード カバレッジ (行・分岐・関数) の概念を説明できる
- [ ] `--coverage` フラグ付きでコンパイル・リンクしてカバレッジ計測を有効にできる
- [ ] テスト実行後に `.gcda`・`.gcno` ファイルが生成されることを確認できる
- [ ] `gcov` コマンドでカバレッジ データを生成できる
- [ ] `lcov` または `gcovr` で HTML レポートを生成できる
- [ ] カバレッジ レポートを読んで未テスト箇所を特定できる

## 学習マテリアル

### 公式ドキュメント

- [gcovr ドキュメント](https://gcovr.com/en/stable/) - gcovr の使い方 (英語)
- [lcov GitHub](https://github.com/linux-test-project/lcov) - lcov のリポジトリと使い方 (英語)
- [OpenCppCoverage GitHub](https://github.com/OpenCppCoverage/OpenCppCoverage) - Windows 向けカバレッジ ツール (英語)
- [GCC gcov ドキュメント](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) - gcov の公式説明 (英語)

## このリポジトリとの関連

### 使用箇所 (具体的なファイル・コマンド)

カバレッジ計測付きビルドの基本手順 (Linux / GCC):

```bash
# 1. --coverage フラグ付きでコンパイル
gcc -c --coverage -fprofile-arcs -ftest-coverage \
    -I app/calc/prod/include \
    app/calc/prod/libsrc/calcbase/add.c \
    -o add.o

# 2. テストを実行 (.gcda ファイルが生成される)
./addTest

# 3. カバレッジレポートを生成 (gcovr 使用)
gcovr --html --html-details -o coverage.html

# または lcov + genhtml 使用
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage-report/
```

`framework/testfw/` が提供するカバレッジ機能:
- テスト実行後の自動カバレッジ計測
- レポートの HTML 生成
- GitHub Actions との統合

Windows での計測 (OpenCppCoverage):

```powershell
OpenCppCoverage.exe --sources prod\calc\libsrc -- addTest.exe
```

### 関連ドキュメント

- [テスト チュートリアル](../../testing-tutorial.md) - テスト フレームワークの実践的な使い方
- [Google Test (スキル ガイド)](google-test.md) - テスト コードの書き方
- [GitHub Actions (スキル ガイド)](../07-ci-cd/github-actions.md) - CI でのカバレッジ自動計測
