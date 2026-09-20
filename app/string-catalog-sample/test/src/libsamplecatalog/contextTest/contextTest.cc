#include <testfw.h>

#include <cstdint>

#include <samplecatalog/samplecatalog_context.h>

// app が定める文脈引数として渡す、巡回する連番の確認
class contextTest : public Test
{
};

// 呼び出すたびに 1 ずつ増えることの確認
TEST_F(contextTest, advances_by_one_on_each_call)
{
    // Arrange
    const int32_t first = samplecatalog_next_sequence_number(); // [状態] - 基準となる 1 つ目の番号を取得する。

    // Pre-Assert
    ASSERT_GE(first, 1); // [事前確認] - 番号が 1 以上であること。
    ASSERT_LE(first, SAMPLECATALOG_SEQUENCE_NUMBER_MAX); // [事前確認] - 番号が上限以下であること。

    // Act
    const int32_t second = samplecatalog_next_sequence_number(); // [手順] - 続けて 2 つ目の番号を取得する。

    // Assert
    const int32_t expected = (first == SAMPLECATALOG_SEQUENCE_NUMBER_MAX) ? 1 : (first + 1);
    EXPECT_EQ(expected, second); // [確認_正常系] - 1 つ増えた番号を返すこと (上限では 1 へ戻ること)。
}

// 上限の回数だけ呼び出すと、同じ番号へ戻ることの確認
TEST_F(contextTest, wraps_around_after_the_maximum)
{
    // Arrange
    const int32_t start = samplecatalog_next_sequence_number(); // [状態] - 周期の起点となる番号を取得する。
    int32_t last = start;

    // Pre-Assert
    ASSERT_GE(start, 1); // [事前確認] - 番号が 1 以上であること。

    // Act
    for (int count = 0; count < SAMPLECATALOG_SEQUENCE_NUMBER_MAX; count++)
    {
        last = samplecatalog_next_sequence_number(); // [手順] - 上限の回数だけ番号を進める。
    }

    // Assert
    EXPECT_EQ(start, last); // [確認_正常系] - 上限の回数で 1 周し、起点と同じ番号へ戻ること。
}

// 番号が上限を超えないことの確認
TEST_F(contextTest, stays_within_the_range)
{
    // Arrange
    bool in_range = true;

    // Pre-Assert

    // Act
    for (int count = 0; count < (SAMPLECATALOG_SEQUENCE_NUMBER_MAX * 2); count++)
    {
        const int32_t value = samplecatalog_next_sequence_number(); // [手順] - 2 周分の番号を取得する。
        if ((value < 1) || (value > SAMPLECATALOG_SEQUENCE_NUMBER_MAX))
        {
            in_range = false;
            break;
        }
    }

    // Assert
    EXPECT_TRUE(in_range); // [確認_正常系] - すべての番号が 1 から上限までに収まること。
}
