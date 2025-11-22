---
summary: multiply 関数の実装ファイル。
author: doxygen and doxybook2
toc: true
---

<!-- IMPORTANT: This is an AUTOMATICALLY GENERATED file by doxygen and doxybook2. Manual edits are NOT allowed. -->

# calc/libsrc/calcbase/multiply.c

## ファイル

### calc/libsrc/calcbase/multiply.c

multiply 関数の実装ファイル。

#### Author

c-modenization-kit sample team

#### Version

1.0.0

#### Date

2025/11/22

#### Details

2 つの整数を乗算する関数を提供します。

#### Copyright

Copyright (C) CompanyName, Ltd. 2025. All rights reserved.

## 関数

### multiply

```cpp
int multiply (
    const int a,
    const int b,
    int * result
)
```

2 つの整数を乗算します。

#### Parameters

* [in] a 第一オペランド。
* [in] b 第二オペランド。
* [out] result 計算結果を格納するポインタ。

#### Return

成功時は CALC_SUCCESS、失敗時は CALC_SUCCESS 以外の値を返します。

#### Note

オーバーフローが発生する可能性がある場合は、 呼び出し側で範囲チェックを行ってください。

#### Warning

result が NULL の場合は失敗を返します。

#### Details

この関数は 2 つの整数を受け取り、その積を result に格納します。

使用例:

```c
int result;
if (multiply(5, 4, &result) == CALC_SUCCESS) {
    printf("Result: %d\n", result);  // 出力: Result: 20
}
```
