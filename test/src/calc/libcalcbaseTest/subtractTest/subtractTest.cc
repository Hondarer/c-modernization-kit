#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

#include <test_com.h>
#include <mock_stdio.h>

#include <libcalcbase.h>

using namespace testing;

class subtractTest : public Test
{
};

TEST_F(subtractTest, test_10_subtract_3)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = subtract(10, 3, &result); // [手順] - subtract(10, 3, &result) を呼び出す。

    // Assert
    EXPECT_EQ(0, rtc);      // [確認] - 戻り値が 0 (成功) であること。
    EXPECT_EQ(7, result);   // [確認] - 結果が 7 であること。
}

TEST_F(subtractTest, test_3_subtract_10)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = subtract(3, 10, &result); // [手順] - subtract(3, 10, &result) を呼び出す。

    // Assert
    EXPECT_EQ(0, rtc);      // [確認] - 戻り値が 0 (成功) であること。
    EXPECT_EQ(-7, result);  // [確認] - 結果が -7 であること。
}

TEST_F(subtractTest, test_5_subtract_5)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = subtract(5, 5, &result); // [手順] - subtract(5, 5, &result) を呼び出す。

    // Assert
    EXPECT_EQ(0, rtc);      // [確認] - 戻り値が 0 (成功) であること。
    EXPECT_EQ(0, result);   // [確認] - 結果が 0 であること。
}

TEST_F(subtractTest, test_null_result)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = subtract(10, 3, NULL); // [手順] - subtract(10, 3, NULL) を呼び出す。

    // Assert
    EXPECT_EQ(-1, rtc); // [確認] - 戻り値が -1 (失敗) であること。
}
