# Python の import 経路の規範

第一者管理の Python スクリプトおよびそのテスト コードは、埋め込み版 Python でも動くように、同じディレクトリのモジュールを import する前に自分のディレクトリを `sys.path` へ追加します。

## 背景

通常の Python は、実行したスクリプトのあるディレクトリを `sys.path` の先頭に入れます。そのため、同じディレクトリにある `test_summary.py` を `from test_summary import ...` で読み込めます。

Windows 向けの埋め込み版 Python (Windows embeddable package) は、`python314._pth` のような `._pth` ファイルで `sys.path` を固定します。このとき、スクリプトのディレクトリは `sys.path` に入らず、環境変数 `PYTHONPATH` も無視されます。結果として、通常の Python では動くスクリプトが埋め込み版では `ModuleNotFoundError` になります。実際に、testfw の `bin_internal/` のスクリプトがこの理由で `make test` を失敗させました。

## 規範

同じディレクトリのモジュール (`.py` またはパッケージ) を import するファイルは、モジュール直下で、その import より前に次の行を置きます。

```python
import os
import sys

# 埋め込み版 Python (._pth) はスクリプトのディレクトリを sys.path に入れないため、明示的に追加する。
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from test_summary import text_encoding
```

`pathlib` を使うファイルでは `sys.path.insert(0, str(Path(__file__).resolve().parent))` でも構いません。いずれかの式を代入した変数を渡す書き方も認めます。

関数の中で遅れて import する場合も、同じ行をモジュール直下に置きます。ほかのファイルから import されるだけのモジュールも対象とします。呼び出し元のディレクトリが異なる場合に失敗するためです。

別のディレクトリのモジュールを読み込む場合 (`bin/` から `bin_internal/` を読む場合など) は、従来どおりそのディレクトリを `sys.path` に明示的に追加します。`PYTHONPATH` には依存しません。

## 検証

検証は、静的な検査と、埋め込み版と同じ条件での実行の 2 つで行います。

静的な検査は `app/general/bin/check-python-import-path.py` で行います。Git 管理下の `.py` をサブモジュールも含めて調べ、規範に合わない import を報告します。CI と Jenkins でも実行します。

```bash
python3 app/general/bin/check-python-import-path.py
```

埋め込み版と同じ条件での実行には、環境変数 `PYTHONSAFEPATH=1` を使います。Python 3.11 以降では、この設定でスクリプトのディレクトリと作業ディレクトリが `sys.path` に入らなくなります。子プロセスの Python にも引き継がれるため、make から起動されるスクリプトも検証できます。`app/general/bin_internal/run-bin-tests.py` (`make bin-test`) と CI は、この設定で実行します。

```bash
PYTHONSAFEPATH=1 python3 -m unittest discover -s path/to/bin_test -p 'test_*.py'
```

`PYTHONSAFEPATH` は `PYTHONPATH` を無視する挙動までは再現しません。`PYTHONPATH` に依存しないことは、規範とレビューで担保します。

## 参考仕様

- [Python 公式ドキュメント: `sys.path` の初期化と `._pth` ファイル](https://docs.python.org/3/library/sys_path_init.html)
- [Python 公式ドキュメント: PYTHONSAFEPATH](https://docs.python.org/3/using/cmdline.html#envvar-PYTHONSAFEPATH)
- [Python 公式ドキュメント: Windows の埋め込み可能なパッケージ](https://docs.python.org/3/using/windows.html#the-embeddable-package)
