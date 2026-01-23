#include <testfw.h>
#include <sabfolder-sample.h>
#include <string.h>
#include <errno.h>

class subfolder_sampleTest_a : public Test
{
};

TEST_F(subfolder_sampleTest_a, test_func_a)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = func_a(); // [手順] - func_a() を呼び出す。

    // Assert
    EXPECT_EQ(1, rtc); // [確認] - func_a() から 1 が返されること。
}
