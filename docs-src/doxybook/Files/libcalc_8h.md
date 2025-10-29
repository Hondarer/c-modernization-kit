---
summary: 計算ライブラリのヘッダーファイル。
author: doxygen and doxybook
toc: true
---

<!-- IMPORTANT: This is an AUTOMATICALLY GENERATED file by doxygen and doxybook. Manual edits are NOT allowed. -->

# calc/include/libcalc.h

## ファイル

### calc/include/libcalc.h

計算ライブラリのヘッダーファイル。

#### Author

doxygen-sample team

#### Version

1.0.0

#### Date

2025/01/30

#### Details

このライブラリは基本的な整数演算を提供します。

#### Copyright

Copyright (C) CompanyName, Ltd. 2025. All rights reserved.

## 関数

### calcHandler

```cpp
int calcHandler (
    int kind,
    int a,
    int b
)
```

指定された演算種別に基づいて計算を実行します。

#### Parameters

* [in] kind 演算の種別 (CALC_KIND_ADD など)。
* [in] a 第一オペランド。
* [in] b 第二オペランド。
* [in] kind 演算の種別 (CALC_KIND_ADD など)。
* [in] a 第一オペランド。
* [in] b 第二オペランド。

#### Return

##### 計算結果。kind が無効な場合は -1 を返します。
##### 計算結果。kind が無効な場合は -1 を返します。

#### Note

##### 現在サポートされている演算は加算 (CALC_KIND_ADD) のみです。
##### 現在サポートされている演算は加算 (CALC_KIND_ADD) のみです。

#### Warning

無効な kind を指定した場合、-1 を返します。 呼び出し側で戻り値のチェックを行ってください。

#### Details

この関数は演算種別を受け取り、対応する計算関数を呼び出します。 現在サポートされている演算種別は以下の通りです。

| 演算種別   | 説明    |
|  -------- | -------- |
| CALC_KIND_ADD   | 加算を実行    |

使用例: ```c

int result = calcHandler(CALC_KIND_ADD, 10, 20);
printf("Result: %d\n", result);  // 出力: Result: 30
```
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
* [in] a 第一オペランド。
* [in] b 第二オペランド。

 #### Return

##### a と b の合計値。
##### a と b の合計値。

 #### Note

オーバーフローが発生する可能性がある場合は、 呼び出し側で範囲チェックを行ってください。

 #### Details

`a` と `b` を加算します。

この関数は 2 つの整数を受け取り、その合計を返します。 オーバーフローのチェックは行いません。

使用例: ```c

int result = add(10, 20);
printf("Result: %d\n", result);  // 出力: Result: 30
```

## 定数、マクロ

### CALC_KIND_ADD

```cpp
#define CALC_KIND_ADD 1
```

加算の演算種別を表す定数。
