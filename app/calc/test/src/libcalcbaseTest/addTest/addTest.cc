#include <testfw.h>
#include <mock_stdio.h>
#include <calcbase/calcbase_spec.h>

class addTest : public Test
{
};

// calcbase_add(1, 2) が 3 を返すことの確認
TEST_F(addTest, test_1_add_2)
{
    // Arrange
    int result;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_add(1, 2, &result); // [手順] - calcbase_add(1, 2, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_add の戻り値が CALC_OK であること。
    EXPECT_EQ(3, result);           // [確認_正常系] - calcbase_add が result に 3 を設定すること。
}

// calcbase_add(2, 1) が 3 を返すことの確認
TEST_F(addTest, test_2_add_1)
{
    // Arrange
    int result;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_add(2, 1, &result); // [手順] - calcbase_add(2, 1, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_add の戻り値が CALC_OK であること。
    EXPECT_EQ(3, result);           // [確認_正常系] - calcbase_add が result に 3 を設定すること。
}

// result が NULL のとき calcbase_add が CALC_ERR_INVALID_ARGUMENT を返すことの確認
TEST_F(addTest, test_null_result)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_add(1, 2, NULL); // [手順] - calcbase_add(1, 2, NULL) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT,
              actual_ret); // [確認_異常系] - calcbase_add の戻り値が CALC_ERR_INVALID_ARGUMENT であること。
}
