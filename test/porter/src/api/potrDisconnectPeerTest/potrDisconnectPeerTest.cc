#include <testfw.h>
#include <mock_potrLog.h>
#include <mock_potrPeerTable.h>

#include <porter_const.h>
#include <porter.h>
#include <potrContext.h>

#include <pthread.h>
#include <string.h>

using namespace testing;

/* コールバックキャプチャ */
struct CallbackCapture
{
    int        count;
    int        service_id;
    PotrPeerId peer_id;
    PotrEvent  event;
};

static CallbackCapture g_cb;

static void mock_callback(int service_id, PotrPeerId peer_id, PotrEvent event,
                           const void * /* data */, size_t /* len */)
{
    g_cb.count++;
    g_cb.service_id = service_id;
    g_cb.peer_id    = peer_id;
    g_cb.event      = event;
}

class potrDisconnectPeerTest : public Test
{
protected:
    void SetUp() override
    {
        memset(&ctx, 0, sizeof(ctx));
        pthread_mutex_init(&ctx.peers_mutex, nullptr);
        ctx.service.service_id = 42;

        memset(&peer_ctx, 0, sizeof(peer_ctx));
        peer_ctx.peer_id = 1;

        memset(&g_cb, 0, sizeof(g_cb));
    }

    void TearDown() override
    {
        pthread_mutex_destroy(&ctx.peers_mutex);
    }

    struct PotrContext_ ctx;
    PotrPeerContext     peer_ctx; /* peer_find_by_id が返す実体 */
};

/* ---------- 異常系 ---------- */

// ハンドルが NULL の場合に POTR_ERROR を返すことの確認
TEST_F(potrDisconnectPeerTest, handle_null)
{
    // Arrange
    NiceMock<Mock_potrLog>        mock_log;
    NiceMock<Mock_potrPeerTable>  mock_peer_table;

    // Pre-Assert
    EXPECT_CALL(mock_log, log_write(POTR_LOG_ERROR,
                                    EndsWith("potrDisconnectPeer.c"), _,
                                    HasSubstr("handle is NULL")))
        .Times(1); // [Pre-Assert確認_異常系] - ERROR ログに "handle is NULL" が含まれること。

    // Act
    int rtc = potrDisconnectPeer(NULL, 1); // [手順] - handle=NULL で potrDisconnectPeer を呼び出す。

    // Assert
    EXPECT_EQ(POTR_ERROR, rtc); // [確認_異常系] - 戻り値が POTR_ERROR であること。
}

// peer_id に POTR_PEER_NA を指定した場合に POTR_ERROR を返すことの確認
TEST_F(potrDisconnectPeerTest, peer_id_na)
{
    // Arrange
    NiceMock<Mock_potrLog>        mock_log;
    NiceMock<Mock_potrPeerTable>  mock_peer_table;

    // Pre-Assert
    EXPECT_CALL(mock_log, log_write(POTR_LOG_ERROR,
                                    EndsWith("potrDisconnectPeer.c"), _,
                                    HasSubstr("invalid peer_id")))
        .Times(1); // [Pre-Assert確認_異常系] - ERROR ログに "invalid peer_id" が含まれること。

    // Act
    int rtc = potrDisconnectPeer(&ctx, POTR_PEER_NA); // [手順] - peer_id=POTR_PEER_NA で potrDisconnectPeer を呼び出す。

    // Assert
    EXPECT_EQ(POTR_ERROR, rtc); // [確認_異常系] - 戻り値が POTR_ERROR であること。
}

// peer_id に POTR_PEER_ALL を指定した場合に POTR_ERROR を返すことの確認
TEST_F(potrDisconnectPeerTest, peer_id_all)
{
    // Arrange
    NiceMock<Mock_potrLog>        mock_log;
    NiceMock<Mock_potrPeerTable>  mock_peer_table;

    // Pre-Assert
    EXPECT_CALL(mock_log, log_write(POTR_LOG_ERROR,
                                    EndsWith("potrDisconnectPeer.c"), _,
                                    HasSubstr("invalid peer_id")))
        .Times(1); // [Pre-Assert確認_異常系] - ERROR ログに "invalid peer_id" が含まれること。

    // Act
    int rtc = potrDisconnectPeer(&ctx, POTR_PEER_ALL); // [手順] - peer_id=POTR_PEER_ALL で potrDisconnectPeer を呼び出す。

    // Assert
    EXPECT_EQ(POTR_ERROR, rtc); // [確認_異常系] - 戻り値が POTR_ERROR であること。
}

// N:1 モードでない場合に POTR_ERROR を返すことの確認
TEST_F(potrDisconnectPeerTest, not_multi_peer)
{
    // Arrange
    NiceMock<Mock_potrLog>        mock_log;
    NiceMock<Mock_potrPeerTable>  mock_peer_table;
    ctx.is_multi_peer = 0; // [状態] - N:1 モードでない (is_multi_peer=0) に設定する。

    // Pre-Assert
    EXPECT_CALL(mock_log, log_write(POTR_LOG_ERROR,
                                    EndsWith("potrDisconnectPeer.c"), _,
                                    HasSubstr("not in N:1 mode")))
        .Times(1); // [Pre-Assert確認_異常系] - ERROR ログに "not in N:1 mode" が含まれること。

    // Act
    int rtc = potrDisconnectPeer(&ctx, 1); // [手順] - is_multi_peer=0 の状態で potrDisconnectPeer を呼び出す。

    // Assert
    EXPECT_EQ(POTR_ERROR, rtc); // [確認_異常系] - 戻り値が POTR_ERROR であること。
}

// 指定した peer_id のピアが存在しない場合に POTR_ERROR を返すことの確認
TEST_F(potrDisconnectPeerTest, peer_not_found)
{
    // Arrange
    NiceMock<Mock_potrLog>        mock_log;
    NiceMock<Mock_potrPeerTable>  mock_peer_table;
    ctx.is_multi_peer = 1; // [状態] - N:1 モードに設定する。

    EXPECT_CALL(mock_peer_table, peer_find_by_id(&ctx, (PotrPeerId)99))
        .WillOnce(Return(nullptr)); // [Pre-Assert確認_異常系] - peer_find_by_id が nullptr を返すこと。

    EXPECT_CALL(mock_log, log_write(POTR_LOG_ERROR,
                                    EndsWith("potrDisconnectPeer.c"), _,
                                    HasSubstr("not found")))
        .Times(1); // [Pre-Assert確認_異常系] - ERROR ログに "not found" が含まれること。

    // Act
    int rtc = potrDisconnectPeer(&ctx, 99); // [手順] - 存在しない peer_id=99 で potrDisconnectPeer を呼び出す。

    // Assert
    EXPECT_EQ(POTR_ERROR, rtc); // [確認_異常系] - 戻り値が POTR_ERROR であること。
}

/* ---------- 正常系 ---------- */

// ピアが存在し health_alive=1 の場合に切断処理とコールバック発火が行われることの確認
TEST_F(potrDisconnectPeerTest, normal_with_callback)
{
    // Arrange
    NiceMock<Mock_potrLog>        mock_log;
    NiceMock<Mock_potrPeerTable>  mock_peer_table;
    ctx.is_multi_peer   = 1;               // [状態] - N:1 モードに設定する。
    ctx.callback        = mock_callback;   // [状態] - 受信コールバックを設定する。
    peer_ctx.health_alive = 1;             // [状態] - ピアを疎通済み状態 (health_alive=1) に設定する。

    EXPECT_CALL(mock_peer_table, peer_find_by_id(&ctx, (PotrPeerId)1))
        .WillOnce(Return(&peer_ctx)); // [Pre-Assert確認_正常系] - peer_find_by_id がピアコンテキストを返すこと。

    EXPECT_CALL(mock_log, log_write(POTR_LOG_INFO,
                                    EndsWith("potrDisconnectPeer.c"), _,
                                    HasSubstr("disconnecting")))
        .Times(1); // [Pre-Assert確認_正常系] - INFO ログに "disconnecting" が含まれること。

    EXPECT_CALL(mock_peer_table, peer_send_fin(&ctx, &peer_ctx))
        .Times(1); // [Pre-Assert確認_正常系] - peer_send_fin が 1 回呼ばれること。

    EXPECT_CALL(mock_peer_table, peer_free(&ctx, &peer_ctx))
        .Times(1); // [Pre-Assert確認_正常系] - peer_free が 1 回呼ばれること。

    // Act
    int rtc = potrDisconnectPeer(&ctx, 1); // [手順] - 正常な状態で potrDisconnectPeer を呼び出す。

    // Assert
    EXPECT_EQ(POTR_SUCCESS, rtc);                        // [確認_正常系] - 戻り値が POTR_SUCCESS であること。
    EXPECT_EQ(1, g_cb.count);                            // [確認_正常系] - コールバックが 1 回呼ばれること。
    EXPECT_EQ(42, g_cb.service_id);                      // [確認_正常系] - コールバックの service_id が 42 であること。
    EXPECT_EQ((PotrPeerId)1, g_cb.peer_id);              // [確認_正常系] - コールバックの peer_id が 1 であること。
    EXPECT_EQ(POTR_EVENT_DISCONNECTED, g_cb.event);      // [確認_正常系] - コールバックのイベントが POTR_EVENT_DISCONNECTED であること。
    EXPECT_EQ(0, peer_ctx.health_alive);                 // [確認_正常系] - health_alive が 0 にクリアされること。
}

/* ---------- 準正常系 ---------- */

// ピアが存在し health_alive=0 の場合に切断処理のみ行われコールバックが発火しないことの確認
TEST_F(potrDisconnectPeerTest, normal_health_dead)
{
    // Arrange
    NiceMock<Mock_potrLog>        mock_log;
    NiceMock<Mock_potrPeerTable>  mock_peer_table;
    ctx.is_multi_peer   = 1;               // [状態] - N:1 モードに設定する。
    ctx.callback        = mock_callback;   // [状態] - 受信コールバックを設定する。
    peer_ctx.health_alive = 0;             // [状態] - ピアを切断済み状態 (health_alive=0) に設定する。

    EXPECT_CALL(mock_peer_table, peer_find_by_id(&ctx, (PotrPeerId)1))
        .WillOnce(Return(&peer_ctx)); // [Pre-Assert確認_準正常系] - peer_find_by_id がピアコンテキストを返すこと。

    EXPECT_CALL(mock_peer_table, peer_send_fin(&ctx, &peer_ctx))
        .Times(1); // [Pre-Assert確認_準正常系] - peer_send_fin が 1 回呼ばれること。

    EXPECT_CALL(mock_peer_table, peer_free(&ctx, &peer_ctx))
        .Times(1); // [Pre-Assert確認_準正常系] - peer_free が 1 回呼ばれること。

    // Act
    int rtc = potrDisconnectPeer(&ctx, 1); // [手順] - health_alive=0 の状態で potrDisconnectPeer を呼び出す。

    // Assert
    EXPECT_EQ(POTR_SUCCESS, rtc);   // [確認_準正常系] - 戻り値が POTR_SUCCESS であること。
    EXPECT_EQ(0, g_cb.count);       // [確認_準正常系] - health_alive=0 のためコールバックが呼ばれないこと。
}
