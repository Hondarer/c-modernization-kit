#include <testfw.h>
#include <mock_calcbase.h>
#include <calcbase/calcbase_spec.h>

class subtractTest : public Test
{
};

// calcbase_subtract(10, 3) が 7 を返すことの確認
TEST_F(subtractTest, test_10_subtract_3)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result;
    int actual_ret;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(10, -3, _))
        .WillOnce(
            [](int, int, int *_result)
            {
                *_result = 7;
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - calcbase_add(10, -3, &result) が 1 回呼び出されること。
                // [Pre-Assert手順] - calcbase_add(10, -3, &result) にて result に 7 を設定し、CALC_OK を返す。

    // Act
    actual_ret = calcbase_subtract(10, 3, &result); // [手順] - calcbase_subtract(10, 3, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_subtract の戻り値が CALC_OK であること。
    EXPECT_EQ(7, result);           // [確認_正常系] - calcbase_subtract が result に 7 を設定すること。
}

// calcbase_subtract(3, 10) が -7 を返すことの確認
TEST_F(subtractTest, test_3_subtract_10)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result;
    int actual_ret;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(3, -10, _))
        .WillOnce(
            [](int, int, int *_result)
            {
                *_result = -7;
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - calcbase_add(3, -10, &result) が 1 回呼び出されること。
                // [Pre-Assert手順] - calcbase_add(3, -10, &result) にて result に -7 を設定し、CALC_OK を返す。

    // Act
    actual_ret = calcbase_subtract(3, 10, &result); // [手順] - calcbase_subtract(3, 10, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_subtract の戻り値が CALC_OK であること。
    EXPECT_EQ(-7, result);          // [確認_正常系] - calcbase_subtract が result に -7 を設定すること。
}

// calcbase_subtract(5, 5) が 0 を返すことの確認
TEST_F(subtractTest, test_5_subtract_5)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result;
    int actual_ret;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(5, -5, _))
        .WillOnce(
            [](int, int, int *_result)
            {
                *_result = 0;
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - calcbase_add(5, -5, &result) が 1 回呼び出されること。
                // [Pre-Assert手順] - calcbase_add(5, -5, &result) にて result に 0 を設定し、CALC_OK を返す。

    // Act
    actual_ret = calcbase_subtract(5, 5, &result); // [手順] - calcbase_subtract(5, 5, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_subtract の戻り値が CALC_OK であること。
    EXPECT_EQ(0, result);           // [確認_正常系] - calcbase_subtract が result に 0 を設定すること。
}

// result が NULL のとき calcbase_subtract が CALC_ERR_INVALID_ARGUMENT を返すことの確認
TEST_F(subtractTest, test_null_result)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int actual_ret;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(10, -3, NULL))
        .WillOnce(Return(
            CALC_ERR_INVALID_ARGUMENT)); // [Pre-Assert確認_異常系] - calcbase_add(10, -3, NULL) が 1 回呼び出されること。
    // [Pre-Assert手順] - calcbase_add(10, -3, NULL) にて CALC_ERR_INVALID_ARGUMENT を返す。

    // Act
    actual_ret = calcbase_subtract(10, 3, NULL); // [手順] - calcbase_subtract(10, 3, NULL) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT,
              actual_ret); // [確認_異常系] - calcbase_subtract の戻り値が CALC_ERR_INVALID_ARGUMENT であること。
}
