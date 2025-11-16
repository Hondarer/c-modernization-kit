---
summary: add 関数の実装ファイル。
author: doxygen and doxybook2
toc: true
---

<!-- IMPORTANT: This is an AUTOMATICALLY GENERATED file by doxygen and doxybook2. Manual edits are NOT allowed. -->

# calc/libsrc/calcbase/add.c

## ファイル

### calc/libsrc/calcbase/add.c

add 関数の実装ファイル。

#### Author

doxygen-sample team

#### Version

1.0.0

#### Date

2025/01/30

#### Details

2 つの整数を加算する関数を提供します。

#### Copyright

Copyright (C) CompanyName, Ltd. 2025. All rights reserved.

## 関数

### add

```cpp
int add (
    int a,
    int b
)
```

2 つの整数を加算します。

#### Parameters

* [in] a 第一オペランド。
* [in] b 第二オペランド。

#### Return

a と b の合計値。

#### Note

オーバーフローが発生する可能性がある場合は、 呼び出し側で範囲チェックを行ってください。

#### Details

この関数は 2 つの整数を受け取り、その合計を返します。 オーバーフローのチェックは行いません。

使用例:

```c
int result = add(10, 20);
printf("Result: %d\n", result);  // 出力: Result: 30
```
