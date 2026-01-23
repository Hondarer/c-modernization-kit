#include <testfw.h>
#include <sabfolder-sample.h>
#include <string.h>
#include <errno.h>

class subfolder_sampleTest : public Test
{
};

TEST_F(subfolder_sampleTest, test_func)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = func(); // [手順] - func() を呼び出す。

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - func() から 0 が返されること。
}
