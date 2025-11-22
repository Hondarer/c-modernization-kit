---
summary: 計算ライブラリ (静的リンク用) のヘッダーファイル。
author: doxygen and doxybook2
toc: true
---

<!-- IMPORTANT: This is an AUTOMATICALLY GENERATED file by doxygen and doxybook2. Manual edits are NOT allowed. -->

# calc/include/libcalcbase.h

## ファイル

### calc/include/libcalcbase.h

計算ライブラリ (静的リンク用) のヘッダーファイル。

#### Author

c-modenization-kit sample team

#### Version

1.0.0

#### Date

2025/11/22

#### Details

このライブラリは基本的な整数演算を提供します。  
静的リンクによる機能の内部関数を模しています。

#### Copyright

Copyright (C) CompanyName, Ltd. 2025. All rights reserved.

## 関数

### subtract

```cpp
int subtract (
    const int a,
    const int b,
    int * result
)
```

2 つの整数を減算します。

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

この関数は 2 つの整数を受け取り、その差を result に格納します。

使用例:

```c
int result;
if (subtract(10, 3, &result) == CALC_SUCCESS) {
    printf("Result: %d\n", result);  // 出力: Result: 7
}
```

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

### add

```cpp
int add (
    const int a,
    const int b,
    int * result
)
```

2 つの整数を加算します。

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

この関数は 2 つの整数を受け取り、その合計を result に格納します。

使用例:

```c
int result;
if (add(10, 20, &result) == CALC_SUCCESS) {
    printf("Result: %d\n", result);  // 出力: Result: 30
}
```
