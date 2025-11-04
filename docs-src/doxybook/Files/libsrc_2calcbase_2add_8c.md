---
author: doxygen and doxybook
toc: true
---

<!-- IMPORTANT: This is an AUTOMATICALLY GENERATED file by doxygen and doxybook. Manual edits are NOT allowed. -->

# calc/libsrc/calcbase/add.c

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
* [in] a 第一オペランド。
* [in] b 第二オペランド。

#### Return

##### a と b の合計値。
##### a と b の合計値。

#### Note

オーバーフローが発生する可能性がある場合は、 呼び出し側で範囲チェックを行ってください。

#### Details

この関数は 2 つの整数を受け取り、その合計を返します。 オーバーフローのチェックは行いません。

使用例: ```c

int result = add(10, 20);
printf("Result: %d\n", result);  // 出力: Result: 30
```
