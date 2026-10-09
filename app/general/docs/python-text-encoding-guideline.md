# Python のテキスト入出力で文字コードを明示する

第一者管理の Python スクリプトとそのテストでは、テキスト入出力の文字コードを明示します。  
リポジトリの文書、ソース、設定、テスト用の一時ファイルを読み書きする場合は、別の文字コードを指定する契約がない限り `encoding="utf-8"` を指定します。  
子プロセスのテキスト通信も、入出力の契約に従って文字コードを指定します。  
外部配布物と生成済み成果物は手動で変更せず、第一者管理の生成元や呼び出し元を修正します。

## ファイルと子プロセスの両方で指定する

`open()`、`Path.open()`、`Path.read_text()`、`Path.write_text()` のテキスト入出力では、`encoding` を省略しません。  
`subprocess.run()`、`Popen()`、`check_output()` などで `text=True` または `universal_newlines=True` を使う場合も、`encoding` を指定します。  
エラー時の診断メッセージや将来追加するデータに日本語が含まれる可能性があるため、ASCII だけのデータや、成功時の出力が空のテストでも文字コードを指定します。

```python
source = path.read_text(encoding="utf-8")
path.write_text(source, encoding="utf-8")

with open(log_path, "w", encoding="utf-8") as output:
    output.write(message)

result = subprocess.run(
    command,
    input=source,
    text=True,
    encoding="utf-8",
    capture_output=True,
    check=True,
)
```

親プロセスで `encoding="utf-8"` を指定しても、子プロセスの標準入出力の文字コードは変更されません。  
親プロセスの `encoding` と、子プロセスの標準入出力の文字コードを一致させます。  
第一者管理の CLI では、ファイルやパイプの文字コードを仕様として定め、必要に応じて標準入出力の `reconfigure()` やバイト列の明示的な変換で対応します。  
子プロセスの標準出力と標準エラー出力の文字コードが異なる場合は、それぞれをバイト列として受け取り、各出力の契約でデコードします。

## 異なる文字コードは契約を確認して指定する

既存ファイル、外部コマンド、ターゲット設定が UTF-8 以外を定めている場合は、その文字コードを明示して保持します。  
入力と出力の仕様、設定値、指定理由を確認し、UTF-8 へ一律に置き換えません。  
`encoding=None` は使いません。  
システムのロケールや環境変数 `PYTHONUTF8` に依存して、呼び出し元の文字コード指定を省略しません。  
デコード失敗を隠すために `errors="ignore"` や `errors="replace"` を追加しません。

バイナリ モードの `open(..., "rb")`、`Path.read_bytes()`、`Path.write_bytes()`、バイト列での子プロセス通信には、テキスト用の `encoding` を指定しません。  
`str` とバイト列を相互変換する箇所では、契約に従って `encode("utf-8")` や `decode("utf-8")` などを明示します。

## 既定値が UTF-8 の Linux だけで判断しない

Windows CI では、文字コード未指定の子プロセス通信に CP1252 が使われ、日本語の入力で `UnicodeEncodeError` が発生しました。  
Linux での通常実行に成功しても、文字コードの指定漏れがないとは判断しません。  
レビューでは、ファイルの読み書きと診断出力のデコードも対象にして指定漏れを確認します。

変更した Python スクリプトとその局所テストを、次のオプションで検証します。  
`EncodingWarning` をエラーにして、実行された経路の指定漏れを検出します。  
`path/to/bin_test` は検証対象のディレクトリへ置き換えます。  
テストが起動する Python にも適用する場合は、`PYTHONWARNDEFAULTENCODING=1` と `PYTHONWARNINGS=error::EncodingWarning` を渡します。

```bash
python3 -X warn_default_encoding -W error::EncodingWarning -m unittest discover -s path/to/bin_test -p 'test_*.py'
```

第三者コードの警告が出た場合は、第一者コードの指定漏れと分けて報告し、依存側の原因を確認します。  
第三者コードを直接変更したり、全警告を無効にして合格扱いにしたりしません。

文字コードが問題になった CLI の回帰テストには、日本語の入力、成功時の出力、エラー時の診断を含めます。  
Linux でも既定値が CP1252 の条件を模擬し、文字コードを明示した実際の入出力が成功することを検証します。  
Windows 固有の実行結果は、Windows CI で別途確認します。

## Python の公式仕様

- [テキスト入出力の文字コードと EncodingWarning](https://docs.python.org/3/library/io.html#text-encoding)
- [subprocess のテキスト モードと encoding](https://docs.python.org/3/library/subprocess.html#frequently-used-arguments)
