---
summary: divide 関数の実装ファイル。
author: doxygen and doxybook2
toc: true
---

<!-- IMPORTANT: This is an AUTOMATICALLY GENERATED file by doxygen and doxybook2. Manual edits are NOT allowed. -->

# calc/libsrc/calcbase/divide.c

## ファイル

### calc/libsrc/calcbase/divide.c

divide 関数の実装ファイル。

#### Author

c-modenization-kit sample team

#### Version

1.0.0

#### Date

2025/11/22

#### Details

2 つの整数を除算する関数を提供します。

#### Copyright

Copyright (C) CompanyName, Ltd. 2025. All rights reserved.

## 関数

### divide

```cpp
int divide (
    const int a,
    const int b,
    int * result
)
```

2 つの整数を除算します。

#### Parameters

* [in] a 被除数。
* [in] b 除数。
* [out] result 計算結果を格納するポインタ。

#### Return

成功時は CALC_SUCCESS、失敗時は CALC_SUCCESS 以外の値を返します。

#### Warning

ゼロ除算の場合、または result が NULL の場合は失敗を返します。

#### Details

この関数は 2 つの整数を受け取り、その商を result に格納します。 整数除算のため、小数点以下は切り捨てられます。

使用例:

```c
int result;
if (divide(20, 4, &result) == CALC_SUCCESS) {
    printf("Result: %d\n", result);  // 出力: Result: 5
}
```
