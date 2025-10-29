---
summary: 計算機の定義を提供します。
author: doxygen and doxybook
toc: true
---

<!-- IMPORTANT: This is an AUTOMATICALLY GENERATED file by doxygen and doxybook. Manual edits are NOT allowed. -->

# src/calculator.h

## ファイル

### src/calculator.h

計算機の定義を提供します。

#### Author

初版作成者

#### Version

1.0

#### Date

2025/07/08

#### History

* 2025/07/08 新規作成

#### Copyright

Copyright (C) CompanyName, Ltd. 2025. All rights reserved.

## 関数

### subtract

```cpp
int subtract (
    int a,
    int b
)
```

`a` から `b` を減算します。

#### Parameters

* [in] a 被減数
* [in] b 減数

#### Return

減算結果。

#### Remarks

この関数はスレッド セーフです。

### multiply

```cpp
int multiply (
    int a,
    int b
)
```

`a` と `b` を乗算します。

#### Parameters

* [in] a 因数a
* [in] b 因数b

#### Return

乗算結果。

#### Remarks

この関数はスレッド セーフです。

### divide

```cpp
double divide (
    int a,
    int b
)
```

`a` を `b` で除算します。

#### Parameters

* [in] a 被除数
* [in] b 除数

#### Return

除算結果。

#### Warning

`b` が 0 の場合、結果は未定義です。

#### Remarks

この関数はスレッド セーフです。

#### Details

```plantuml
@startuml
    caption 図のテスト
    circle a
    circle b
    rectangle "a/b" as devide
    circle return
    a -> devide : 被除数
    b -> devide : 除数
    devide -> return
@enduml
```

### add

```cpp
int add (
    int a,
    int b
)
```

`a` と `b` を加算します。

#### Parameters

* [in] a 加算数a
* [in] b 加算数b
* [in] a 第一オペランド。
* [in] b 第二オペランド。
* [in] a 第一オペランド。
* [in] b 第二オペランド。
* [in] a 第一オペランド。
* [in] b 第二オペランド。

#### Return

##### 加算結果。
##### a と b の合計値。
##### a と b の合計値。
##### a と b の合計値。

#### Note

##### オーバーフローが発生する可能性がある場合は、 呼び出し側で範囲チェックを行ってください。
##### オーバーフローが発生する可能性がある場合は、 呼び出し側で範囲チェックを行ってください。

#### Remarks

この関数はスレッド セーフです。

#### Details

2 つの整数を加算します。

この関数は 2 つの整数を受け取り、その合計を返します。 オーバーフローのチェックは行いません。

使用例: ```c

int result = add(10, 20);
printf("Result: %d\n", result);  // 出力: Result: 30
```
`a` と `b` を加算します。

この関数は 2 つの整数を受け取り、その合計を返します。 オーバーフローのチェックは行いません。

使用例: ```c

int result = add(10, 20);
printf("Result: %d\n", result);  // 出力: Result: 30
```

## 定数、マクロ

### ZERO_DEVIDE

```cpp
#define ZERO_DEVIDE (0.0)
```

ゼロ除算の戻り値を定義します。
