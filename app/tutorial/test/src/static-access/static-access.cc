#include <testfw.h>
#include "sample-static-lib/samplestatic.h"
#include "samplestatic.inject.h"

class test_static_access : public Test
{
};

// static 変数の値が samplestatic の戻り値になることの確認
TEST_F(test_static_access, test)
{
    // Arrange
    // inject 機能を使用して static 変数に値を設定
    set_static_int(123); // [状態] - static_int に 123 を設定する。

    // Pre-Assert

    // Act
    int actual_ret = samplestatic(); // [手順] - samplestatic() を呼び出す。

    // Assert
    EXPECT_EQ(123, actual_ret); // [確認_正常系] - samplestatic() からの戻り値が 123 であること。
}
