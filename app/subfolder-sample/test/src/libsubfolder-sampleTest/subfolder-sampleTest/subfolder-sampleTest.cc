#include <testfw.h>
#include <subfolder-sample.h>
#include <string.h>
#include <errno.h>

class subfolder_sampleTest : public Test
{
};

// func() が 0 を返すことの確認
TEST_F(subfolder_sampleTest, test_func)
{
    // Arrange

    // Pre-Assert

    // Act
    int actual_ret = func(); // [手順] - func() を呼び出す。

    // Assert
    EXPECT_EQ(0, actual_ret); // [確認_正常系] - func() から 0 が返されること。
}
