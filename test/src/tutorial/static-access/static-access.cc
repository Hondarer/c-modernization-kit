#include <testfw.h>
#include "sample-static-lib/samplestatic.h"
#include "samplestatic.inject.h"

using namespace testing;

class test_static_access : public Test
{
};

TEST_F(test_static_access, test)
{
    // Arrange
    // inject 機能を使用して static 変数に値を設定
    set_static_int(123); // [状態] - static_int に 123 を設定する。

    // Pre-Assert

    // Act
    int rtc = samplestatic(); // [手順] - samplestatic() を呼び出す。

    // Assert
    EXPECT_EQ(123, rtc); // [確認] - samplestatic() からの戻り値が 123 であること。
}
