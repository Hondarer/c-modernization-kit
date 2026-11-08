#include <testfw.h>
#include <mock_cplat.h>
#include "sample_filter_share_region.h"
using namespace testing;

// ローカル ロック取得後の時計失敗でロックを解放することの確認
TEST(sampleFilterShareClockTest, clock_failure_releases_local_lock)
{
    // Arrange
    NiceMock<Mock_cplat> mock;
    sample_filter_share_lock *lock = nullptr;
    auto *local = reinterpret_cast<cplat_local_lock *>(uintptr_t{1});
    auto *process = reinterpret_cast<cplat_interprocess_lock *>(uintptr_t{2});
    EXPECT_CALL(mock, cplat_local_lock_create(_)).WillOnce(DoAll(SetArgPointee<0>(local), Return(CPLAT_OK)));
    EXPECT_CALL(mock, cplat_interprocess_lock_open(_, _)).WillOnce(DoAll(SetArgPointee<1>(process), Return(CPLAT_OK)));

    // [状態] - 実 OS の資源を使用しない排他ハンドルを用意する。
    ASSERT_EQ(SAMPLE_FILTER_SHARE_REGION_OK, sample_filter_share_lock_create("test", &lock));

    // Pre-Assert
    EXPECT_CALL(mock, cplat_clock_get_monotonic_ms(_))
        .WillOnce(DoAll(SetArgPointee<0>(uint64_t{100}), Return(CPLAT_OK)))
        .WillOnce(Return(CPLAT_ERR_UNKNOWN));

    // [Pre-Assert確認_異常系] - 開始時とローカル ロック取得後の時刻を取得すること。
    // [Pre-Assert手順] - 2 回目だけ失敗させる。
    EXPECT_CALL(mock, cplat_local_lock_lock(local, 100)).WillOnce(Return(CPLAT_OK));
    EXPECT_CALL(mock, cplat_local_lock_unlock(local)).WillOnce(Return(CPLAT_OK));

    // [Pre-Assert確認_異常系] - 取得したローカル ロックを 1 回解放すること。
    EXPECT_CALL(mock, cplat_interprocess_lock_lock(_, _)).Times(0);

    // [Pre-Assert確認_異常系] - 時計失敗後にプロセス間ロックを取得しないこと。
    EXPECT_CALL(mock, cplat_local_lock_dispose(local)).WillOnce(Return());
    EXPECT_CALL(mock, cplat_interprocess_lock_dispose(process)).WillOnce(Return());

    // [Pre-Assert手順] - センチネルの破棄を実 OS へ渡さない。

    // Act
    auto result = sample_filter_share_lock_acquire(lock, 100); // [手順] - ロック取得後の時計を失敗させる。
    sample_filter_share_lock_dispose(&lock);                   // [手順] - テスト用の排他ハンドルを破棄する。

    // Assert
    EXPECT_EQ(SAMPLE_FILTER_SHARE_REGION_FAILED, result); // [確認_異常系] - 時計失敗を返すこと。
    EXPECT_EQ(nullptr, lock);                             // [確認_正常系] - 所有ハンドルを破棄すること。
}
