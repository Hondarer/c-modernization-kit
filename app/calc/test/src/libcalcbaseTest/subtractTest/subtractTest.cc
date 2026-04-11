#include <testfw.h>
#include <mock_calcbase.h>
#include <libcalcbase.h>

class subtractTest : public Test
{
};

TEST_F(subtractTest, test_10_subtract_3)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, add(10, -3, _))
        .WillOnce([](int, int, int *_result)
                  {
                    *_result = 7;
                    return CALC_SUCCESS; }); // [Pre-Assert確認] - add(10, -3, &result) が 1 回呼び出されること。
                        // [Pre-Assert手順] - add(10, -3, &result) にて result に 7 を設定し、CALC_SUCCESS を返す。

    // Act
    int rtc = subtract(10, 3, &result); // [手順] - subtract(10, 3, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(7, result);         // [確認] - 結果が 7 であること。
}

TEST_F(subtractTest, test_3_subtract_10)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, add(3, -10, _))
        .WillOnce([](int, int, int *_result)
                  {
                    *_result = -7;
                    return CALC_SUCCESS; }); // [Pre-Assert確認] - add(3, -10, &result) が 1 回呼び出されること。
                        // [Pre-Assert手順] - add(3, -10, &result) にて result に -7 を設定し、CALC_SUCCESS を返す。

    // Act
    int rtc = subtract(3, 10, &result); // [手順] - subtract(3, 10, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(-7, result);        // [確認] - 結果が -7 であること。
}

TEST_F(subtractTest, test_5_subtract_5)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, add(5, -5, _))
        .WillOnce([](int, int, int *_result)
                  {
                    *_result = 0;
                    return CALC_SUCCESS; }); // [Pre-Assert確認] - add(5, -5, &result) が 1 回呼び出されること。
                        // [Pre-Assert手順] - add(5, -5, &result) にて result に 0 を設定し、CALC_SUCCESS を返す。

    // Act
    int rtc = subtract(5, 5, &result); // [手順] - subtract(5, 5, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(0, result);         // [確認] - 結果が 0 であること。
}

TEST_F(subtractTest, test_null_result)
{
    // Arrange
    Mock_calcbase mock_calcbase;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, add(10, -3, NULL))
        .WillOnce(Return(CALC_ERROR)); // [Pre-Assert確認] - add(10, -3, NULL) が 1 回呼び出されること。
                                       // [Pre-Assert手順] - add(10, -3, NULL) にて CALC_ERROR を返す。

    // Act
    int rtc = subtract(10, 3, NULL); // [手順] - subtract(10, 3, NULL) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_ERROR, rtc); // [確認] - 戻り値が CALC_ERROR であること。
}
