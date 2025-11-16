---
summary: 動的リンク、静的リンクを使った関数の呼び出しコマンド。
author: doxygen and doxybook2
toc: true
---

<!-- IMPORTANT: This is an AUTOMATICALLY GENERATED file by doxygen and doxybook2. Manual edits are NOT allowed. -->

# calc/src/shared-and-static-add/shared-and-static-add.c

## ファイル

### calc/src/shared-and-static-add/shared-and-static-add.c

動的リンク、静的リンクを使った関数の呼び出しコマンド。

#### Author

doxygen-sample team

#### Version

1.0.0

#### Date

2025/11/06

#### Details

コマンドライン引数から 2 つの整数を受け取り、calc 関数、add 関数を使用して 加算結果を標準出力に出力します。

#### Copyright

Copyright (C) CompanyName, Ltd. 2025. All rights reserved.

## 関数

### main

```cpp
int main (
    int argc,
    char * argv[]
)
```

プログラムのエントリーポイント。

#### Parameters

* [in] argc コマンドライン引数の数。
* [in] argv コマンドライン引数の配列。

#### Return

成功時は 0、失敗時は 1 を返します。

#### Attention

引数は正確に 3 つ必要です。

#### Details

使用例: ```c

./add 10 + 20
// 出力: 30
```
