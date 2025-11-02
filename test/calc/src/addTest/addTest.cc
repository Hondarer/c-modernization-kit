#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gtest_wrapmain.h>

#include <mock_calc.h>

using namespace testing;

class addTest : public Test
{
};

TEST_F(addTest, call_via_main)
{
    // Arrange
    Mock_calc mock_calc;
    int argc = 3;
    const char *argv[] = {"test_samplefunc", "1", "2"};

    // Pre-Assert
    EXPECT_CALL(mock_calc, add(1, 2))
        .WillOnce(Return(3));

    // Act
    int rtc = __real_main(argc, (char **)&argv);

    // Assert
    EXPECT_EQ(0, rtc);
}