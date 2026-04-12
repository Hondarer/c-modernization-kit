# include フォルダについて

C/C++ のヘッダーファイルについての注意事項を以下に示します。

## 正本の置き場所

新しい include フォルダを追加したとき、最初に更新するのは `.vscode/c_cpp_properties.json` ではありません。  
各 C/C++ app の `app/<name>/makepart.mk` にある `INCDIR` が正本です。

```makefile
# app/calc/makepart.mk
INCDIR += \
    $(WORKSPACE_FOLDER)/app/calc/prod/include \
    $(WORKSPACE_FOLDER)/app/com_util/prod/include
```

個別ターゲットだけが必要な追加 include は、対象ディレクトリ配下の `makepart.mk` で上乗せします。

`DEFINES` も app 側が正本ですが、`.vscode/c_cpp_properties.json` には同期時の特殊条件があります。

- Linux の `_DEFAULT_SOURCE` は `.vscode` にだけ入る
- `TARGET_ARCH` は app 側の実値を使わず、`.vscode` では常に `TARGET_ARCH=\"\"` になる

## VS Code への反映方法

`.vscode/c_cpp_properties.json` は派生物です。  
`app/<name>/makepart.mk` を更新したら、ワークスペースルートで次を実行して反映します。

```bash
bash framework/makefw/bin/sync_c_cpp_properties.sh --write
```

差異確認だけをしたい場合は次です。

```bash
bash framework/makefw/bin/sync_c_cpp_properties.sh --check
```

`make -C app` のデフォルトビルド後にも dry-run が走り、差異があれば `app/c_cpp_properties.warn` に出ます。

### パスが設定されていない場合

インテリセンスが正しく動作せず、以下のような問題が発生します。

- ヘッダーファイルが見つからないエラー表示
- 関数や変数の定義へジャンプできない
- コード補完が機能しない
- 型情報が表示されない

## 最低限の確認

1. `app/<name>/makepart.mk` の `INCDIR` を更新する
2. `bash framework/makefw/bin/sync_c_cpp_properties.sh --write` を実行する
3. `.vscode/c_cpp_properties.json` の `includePath` と `defines` を確認する
