---
summary: 計算ライブラリ (動的リンク用) のヘッダーファイル。
author: doxygen and doxybook2
toc: true
---

<!-- IMPORTANT: This is an AUTOMATICALLY GENERATED file by doxygen and doxybook2. Manual edits are NOT allowed. -->

# calc/include/libcalc.h

## ファイル

### calc/include/libcalc.h

計算ライブラリ (動的リンク用) のヘッダーファイル。

#### Author

c-modenization-kit sample team

#### Version

1.0.0

#### Date

2025/11/22

#### Details

このライブラリは基本的な整数演算を提供します。  
動的リンクによる機能外提供用の API を模しています。

#### Copyright

Copyright (C) CompanyName, Ltd. 2025. All rights reserved.

## 関数

### calcHandler

```cpp
CALC_API int WINAPI calcHandler (
    const int kind,
    const int a,
    const int b,
    int * result
)
```

指定された演算種別に基づいて計算を実行します。

#### Parameters

* [in] kind 演算の種別 (CALC_KIND_ADD など)。
* [in] a 第一オペランド。
* [in] b 第二オペランド。
* [out] result 計算結果を格納するポインタ。

#### Return

成功時は CALC_SUCCESS、失敗時は CALC_SUCCESS 以外の値を返します。

#### Warning

無効な kind を指定した場合、ゼロ除算の場合、 または result が NULL の場合は失敗を返します。 呼び出し側で戻り値のチェックを行ってください。

#### Details

この関数は演算種別を受け取り、対応する計算関数を呼び出します。 現在サポートされている演算種別は以下の通りです。

| 演算種別   | 説明   |
|  -------- | -------- |
| CALC_KIND_ADD   | 加算を実行   |
| CALC_KIND_SUBTRACT   | 減算を実行   |
| CALC_KIND_MULTIPLY   | 乗算を実行   |
| CALC_KIND_DIVIDE   | 除算を実行   |

使用例:

```c
int result;
if (calcHandler(CALC_KIND_ADD, 10, 20, &result) == CALC_SUCCESS) {
    printf("Result: %d\n", result);  // 出力: Result: 30
}
```

## 定数、マクロ

### WINAPI

```cpp
#define WINAPI
```

### CALC_API

```cpp
#define CALC_API
```
