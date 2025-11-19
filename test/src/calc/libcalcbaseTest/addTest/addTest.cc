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

class addTest : public Test
{
};

TEST_F(addTest, test_1_add_2)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = add(1, 2); // [手順] - add(1, 2) を呼び出す。

    // Assert
    EXPECT_EQ(3, rtc); // [確認] - 戻り値が 3 であること。
}

TEST_F(addTest, test_2_add_1)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = add(2, 1); // [手順] - add(2, 1) を呼び出す。

    // Assert
    EXPECT_EQ(3, rtc); // [確認] - 戻り値が 3 であること。
}