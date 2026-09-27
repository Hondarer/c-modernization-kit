#include <testfw.h>
#include <subfolder-sample.h>
#include <string.h>
#include <errno.h>

class subfolder_sampleTest_a : public Test
{
};

// func_a() が 1 を返すことの確認
TEST_F(subfolder_sampleTest_a, test_func_a)
{
    // Arrange

    // Pre-Assert

    // Act
    int actual_ret = func_a(); // [手順] - func_a() を呼び出す。

    // Assert
    EXPECT_EQ(1, actual_ret); // [確認_正常系] - func_a() から 1 が返されること。
}
