# include フォルダについて

C/C++ のヘッダーファイルについての注意事項を以下に示します。

## 正本の置き場所

新しい include フォルダを追加したとき、最初に更新するのは `.vscode/c_cpp_properties.json` ではありません。  
`makepart.mk`、`app/makepart.mk`、各 C/C++ app の `app/<name>/makepart.mk` にある `INCDIR` の合成結果が正本です。

```makefile
# app/calc/makepart.mk
INCDIR += \
    $(MYAPP_DIR)/prod/include \
    $(MYAPP_DIR)/../com_util/prod/include
```

`app/<name>/` 配下の `makepart.mk` では `$(MYAPP_DIR)` を使用します。
自 app 内のパスは `$(MYAPP_DIR)/...`、他 app のパスは `$(MYAPP_DIR)/../{otherapp}/...` と記述します。
`$(MYAPP_DIR)` の詳細は [makeparts.md](../framework/makefw/docs/makeparts.md) を参照してください。

`app/makepart.mk` (app/ 直下) やワークスペースルートの `makepart.mk` では、従来どおり `$(WORKSPACE_DIR)` を使用します。

個別ターゲットだけが必要な追加 include は、対象ディレクトリ配下の `makepart.mk` で上乗せします。

`DEFINES` も同じ合成結果が正本ですが、`.vscode/c_cpp_properties.json` には同期時の特殊条件があります。

- `_DEFAULT_SOURCE` のように実ビルドでも必要な define は `makepart.mk` / `app/makepart.mk` に置く
- `TARGET_ARCH` は app 側の実値を使わず、`.vscode` では常に `TARGET_ARCH=\"\"` になる
- `TARGET_ARCH=\"\"` を先頭に置き、それ以外の項目はソートして並べる

## VS Code への反映方法

`.vscode/c_cpp_properties.json` は派生物です。  
`makepart.mk`、`app/makepart.mk`、`app/<name>/makepart.mk` を更新したら、ワークスペースルートで次を実行して反映します。

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

1. `makepart.mk`、`app/makepart.mk`、`app/<name>/makepart.mk` の `INCDIR` を更新する
2. `bash framework/makefw/bin/sync_c_cpp_properties.sh --write` を実行する
3. `.vscode/c_cpp_properties.json` の `includePath` と `defines` を確認する
