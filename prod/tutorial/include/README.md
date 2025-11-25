# include フォルダについて

このフォルダには、C/C++ のヘッダーファイルが配置されています。

## VS Code でのインテリセンス設定

VS Code の C/C++ 拡張機能でインテリセンス (コード補完、定義へのジャンプなど) を有効にするには、`.vscode/c_cpp_properties.json` の `includePath` にこのフォルダのパスを記載する必要があります。

### 設定例

```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/prod/calc/include",
                "${workspaceFolder}/testfw/include",
                "${workspaceFolder}/testfw/gtest/include",
                "${workspaceFolder}/test/include"
            ],
            ...
        }
    ]
}
```

### 設定の確認方法

1. VS Code でプロジェクトを開く
2. `.vscode/c_cpp_properties.json` を確認
3. 各プラットフォーム設定 (Linux, Win32) の `includePath` にこのフォルダが含まれているか確認

### パスが設定されていない場合

インテリセンスが正しく動作せず、以下のような問題が発生します。

- ヘッダーファイルが見つからないエラー表示
- 関数や変数の定義へジャンプできない
- コード補完が機能しない
- 型情報が表示されない

新しい include フォルダを追加した場合は、`.vscode/c_cpp_properties.json` の `includePath` に必ずパスを追加してください。
