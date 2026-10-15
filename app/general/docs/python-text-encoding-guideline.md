# Python のテキスト入出力における文字コード指定規範

第一者管理の Python スクリプトおよびそのテスト コードでは、テキスト入出力の文字コードを常に明示します。  
リポジトリ内の文書、ソース、設定、テスト用一時ファイルを読み書きする際は、別の文字コードが仕様で定められている場合を除き、原則として `encoding="utf-8"` を指定します。

## ファイル入出力および子プロセス通信での明示

テキスト データを扱うすべての API 呼び出しにおいて、`encoding` の指定を省略してはなりません。エラー メッセージや将来追加されるデータに非 ASCII 文字が含まれる可能性があるため、ASCII のみのデータや出力が空のテストであっても明示を必須とします。

### ファイルの読み書き

`open()`、`Path.open()`、`Path.read_text()`、`Path.write_text()` では、必ず `encoding="utf-8"` を指定します。

```python
source = path.read_text(encoding="utf-8")
path.write_text(source, encoding="utf-8")

with open(log_path, "w", encoding="utf-8") as output:
    output.write(message)
```

### 子プロセスとのテキスト通信

`subprocess.run()`、`Popen()`、`check_output()` などで `text=True` または `universal_newlines=True` を指定する場合も、`encoding="utf-8"` を明示します。

```python
result = subprocess.run(
    command,
    input=source,
    text=True,
    encoding="utf-8",
    capture_output=True,
    check=True,
)
```

> [!NOTE]
> 親プロセスで `encoding="utf-8"` を指定しても、子プロセス側の標準入出力エンコーディングは変更されません。両者の文字コード契約を一致させる必要があります。  
> 子プロセスの標準出力と標準エラー出力でエンコーディングが異なる場合は、テキスト モードではなくバイト列として受け取り、それぞれの出力仕様に合わせて個別にデコードします。

## 非 UTF-8 文字コードの取り扱いと禁止事項

既存ファイル形式、外部ツール、特定ターゲットの仕様により UTF-8 以外が指定されている場合は、その仕様に従って文字コードを明示し、一律に UTF-8 へ置き換えてはなりません。

### 禁止事項

- **`encoding=None` の使用禁止**: プラットフォーム依存の既定エンコーディングに委ねてはなりません。
- **システム環境依存の禁止**: システム ロケールや環境変数 `PYTHONUTF8` に依存して、コード側の明示を省略してはなりません。
- **安易なエラー抑制の禁止**: デコード エラーを隠蔽する目的で `errors="ignore"` や `errors="replace"` を指定してはなりません。
- **バイナリ モードでの誤指定禁止**: バイナリ モード (`"rb"`, `"wb"`, `Path.read_bytes()`) やバイト列通信に対してテキスト用の `encoding` を指定してはなりません (`str` とバイト列の境界で `encode()` / `decode()` を明示します)。

## プラットフォーム差異の検証

Linux 環境の Python は既定値が UTF-8 であることが多く、文字コードを省略しても正常に動作してしまうため、指定漏れを見落とす原因になります。Windows 環境では既定値が ANSI コード ページ (例: CP1252) となり、日本語などのマルチバイト文字で `UnicodeEncodeError` が発生します。

### 指定漏れの検出コマンド

スクリプトの変更時は、局所テストで `EncodingWarning` をエラーとして扱い、実行経路上の指定漏れを機械的に検出します。

```bash
# path/to/bin_test 配下のテストを実行し、文字コード未指定をエラーとして検出
python3 -X warn_default_encoding -W error::EncodingWarning -m unittest discover -s path/to/bin_test -p 'test_*.py'
```

子プロセスとして起動される Python スクリプトにも適用する場合は、環境変数 `PYTHONWARNDEFAULTENCODING=1` および `PYTHONWARNINGS=error::EncodingWarning` を設定します。

### 回帰テストの考慮事項

- 文字コードに起因する不具合の回帰テストには、日本語入力、成功時出力、エラー時の診断メッセージを含めます。
- Linux 上であっても Windows 相当の環境 (非 UTF-8 既定値) を模擬し、文字コードが明示されていれば正常動作することを検証します。
- 外部依存ライブラリから警告が出た場合は、自作コードの漏れと切り分け、第三者コードを直接書き換えるのではなく報告・調査を行います。

## 参考仕様

- [Python 公式ドキュメント: テキスト入出力の文字コードと EncodingWarning](https://docs.python.org/3/library/io.html#text-encoding)
- [Python 公式ドキュメント: subprocess のテキスト モードと encoding](https://docs.python.org/3/library/subprocess.html#frequently-used-arguments)
