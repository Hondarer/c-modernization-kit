# include フォルダーについて

C/C++ のヘッダー ファイルについての注意事項を以下に示します。

## 正本の置き場所

新しい include フォルダーを追加したとき、最初に更新するのは `.vscode/c_cpp_properties.json` ではありません。  
`makepart.mk`、`app/makepart.mk`、各 C/C++ app の `app/<name>` 配下にあるすべての `makepart.mk` にある `INCDIR` と `SYSTEM_INCDIR` の合成結果が正本です。  
`SYSTEM_INCDIR` はビルド時に外来ヘッダーの警告を分離しますが、VS Code IntelliSense では `INCDIR` と同じ include パスとして同期します。

```makefile
# app/example/makepart.mk
INCDIR += \
    $(MYAPP_DIR)/prod/include \
    $(MYAPP_DIR)/../utility/prod/include
```

`app/<name>/` 配下の `makepart.mk` では `$(MYAPP_DIR)` を使用します。  
自 app 内のパスは `$(MYAPP_DIR)/...`、他 app のパスは `$(MYAPP_DIR)/../{otherapp}/...` と記述します。  
`$(MYAPP_DIR)` の詳細は [makeparts.md](../../../framework/makefw/docs/makeparts.md) を参照してください。

`app/makepart.mk` (app/ 直下) やワークスペース ルートの `makepart.mk` では、従来どおり `$(WORKSPACE_DIR)` を使用します。

個別ターゲットだけが必要な追加 include は、対象ディレクトリ配下の `makepart.mk` で上乗せします。  
この追加分も `sync_c_cpp_properties.sh` の同期対象に含まれます。

`DEFINES` も同じ合成結果が正本ですが、`.vscode/c_cpp_properties.json` には同期時の特殊条件があります。  
ただし `DEFINES` の同期対象は `makepart.mk`、`app/makepart.mk`、`app/<name>/makepart.mk` までです。

- `_DEFAULT_SOURCE` のように実ビルドでも必要な define は `makepart.mk` または `app/makepart.mk` に配置します。
- `TARGET_ARCH` は app 側の実値を使わず、`.vscode` では常に `TARGET_ARCH=target_arch` になります。
- `TARGET_ARCH=target_arch"` を先頭に配置し、それ以外の項目はソートして並べます。

### 同期評価時の注意 (MAKEFW_SYNC_EVAL)

`sync_c_cpp_properties.sh` は、ホスト OS に関係なく Linux 構成と Win32 構成の両方で各 `makepart.mk` を評価します。  
この同期評価では `MAKEFW_SYNC_EVAL := 1` が定義されるため、`makepart.mk` でホスト環境のプローブ (`pkg-config` など) と `$(error)` を組み合わせる場合は `ifndef MAKEFW_SYNC_EVAL` でガードしてください。  
詳細は [makeparts.md](../../../framework/makefw/docs/makeparts.md) の「ホスト環境プローブと同期評価」を参照してください。

## VS Code への反映方法

`.vscode/c_cpp_properties.json` は派生物です。  
`INCDIR` または `SYSTEM_INCDIR` を更新したら、ワークスペース ルートで次を実行して反映します。  
対象は `makepart.mk`、`app/makepart.mk`、`app/<name>` 配下の任意の `makepart.mk` です。

```bash
bash framework/makefw/bin/sync_c_cpp_properties.sh --write
```

差分の確認だけを行う場合は、次のコマンドを実行します。

```bash
bash framework/makefw/bin/sync_c_cpp_properties.sh --check
```

`make -C app` のデフォルト ビルド後にも dry-run が実行され、差分がある場合は `app/c_cpp_properties.warn` に出力されます。

### パスが設定されていない場合

IntelliSense が正しく動作せず、以下のような問題が発生します。

- ヘッダー ファイルが見つからないエラーの表示
- 関数や変数の定義へジャンプできない現象
- コード補完が機能しない問題
- 型情報が表示されない現象

## 最低限の確認

1. `makepart.mk`、`app/makepart.mk`、または `app/<name>` 配下の `makepart.mk` の `INCDIR` または `SYSTEM_INCDIR` を更新します。
2. `bash framework/makefw/bin/sync_c_cpp_properties.sh --write` を実行します。
3. `.vscode/c_cpp_properties.json` の `includePath` と `defines` を確認します。
