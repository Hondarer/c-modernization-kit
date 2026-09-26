#include <testfw.h>

#include "sample_worker_context.h"

class sampleFilterSequenceNumberTest : public Test
{
};

// SAMPLE_WORKER_SEQUENCE_NUMBER_MAX 回の呼び出し後に、基準値と同じ値へ巡回することの確認
// (プロセス内で共有するカウンターのため、絶対値ではなく周期で確認し、実行順に依存させない)
TEST_F(sampleFilterSequenceNumberTest, wraps_around_after_reaching_the_maximum)
{
    // Arrange
    int32_t actual_base_value;
    int32_t actual_value_after_full_cycle = 0;
    int cycle_index;

    // Pre-Assert

    // Act
    actual_base_value = sample_worker_next_sequence_number(); // [手順] - 基準となる値を 1 回取得する。
    for (cycle_index = 0; cycle_index < SAMPLE_WORKER_SEQUENCE_NUMBER_MAX; cycle_index++)
    {
        actual_value_after_full_cycle = sample_worker_next_sequence_number(); // [手順] - 上限周期と同じ回数だけ呼び出し、1 周させる。
    }

    // Assert
    EXPECT_GE(actual_base_value, 1);                        // [確認_正常系] - 基準値が下限 1 以上であること。
    EXPECT_LE(actual_base_value, SAMPLE_WORKER_SEQUENCE_NUMBER_MAX); // [確認_正常系] - 基準値が上限以下であること。
    EXPECT_EQ(actual_base_value,
             actual_value_after_full_cycle); // [確認_正常系] - 上限周期と同じ回数の呼び出し後、基準値と同じ値へ巡回すること。
}
