#include <testfw.h>
#include <sabfolder-sample.h>
#include <string.h>
#include <errno.h>

class subfolder_sampleTest_b : public Test
{
};

TEST_F(subfolder_sampleTest_b, test_func_b)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = func_b(); // [手順] - func_b() を呼び出す。

    // Assert
    EXPECT_EQ(2, rtc); // [確認] - func_b() から 2 が返されること。
}
