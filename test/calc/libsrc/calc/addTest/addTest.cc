#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <test_com.h>
#include <mock_stdio.h>

#include <libcalc.h>

using namespace testing;

class addTest : public Test
{
};

TEST_F(addTest, test2)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = add(1, 2);

    // Assert
    EXPECT_EQ(3, rtc);
}

TEST_F(addTest, test1)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = add(2, 1);

    // Assert
    EXPECT_EQ(3, rtc);
}