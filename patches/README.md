# Patches

このディレクトリには、サブモジュールに適用するパッチファイルが含まれています。

## testfw-ci-color-output.patch

**目的**: CI 環境でのテスト結果の色出力を有効化

**説明**:
- testfw サブモジュールの `exec_test.sh` を修正し、CI 環境でテスト結果が色付きで表示されるようにします
- ローカル Linux 環境では従来通り `script` コマンドを使用
- CI 環境（GitHub Actions 等）では `script` コマンドを使わず直接実行することで、ANSI カラーコードが正しく出力されます

**適用方法**:

CI 環境では自動的に適用されます（`.github/workflows/ci.yml` 参照）。

ローカルで手動適用する場合:
```bash
cd testfw
git apply ../patches/testfw-ci-color-output.patch
```

**注意**: このパッチは testfw サブモジュールの外部リポジトリ（Hondarer/googletest-c-framework）への書き込み権限がないため、一時的な対応として作成されました。将来的に testfw リポジトリに直接マージされた場合は、このパッチは不要になります。
