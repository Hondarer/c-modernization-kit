#include <testfw.h>
#include <subfolder-sample.h>
#include <string.h>
#include <errno.h>

class subfolder_sampleTest_b : public Test
{
};

// func_b() が 2 を返すことの確認
TEST_F(subfolder_sampleTest_b, test_func_b)
{
    // Arrange

    // Pre-Assert

    // Act
    int actual_ret = func_b(); // [手順] - func_b() を呼び出す。

    // Assert
    EXPECT_EQ(2, actual_ret); // [確認_正常系] - func_b() から 2 が返されること。
}
