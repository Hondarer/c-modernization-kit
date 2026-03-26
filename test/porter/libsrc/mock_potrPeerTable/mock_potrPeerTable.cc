#ifdef _WIN32
    #define _HAS_STD_BYTE 0
#endif
#include <testfw.h>
#include <mock_potrPeerTable.h>
#include <potrContext.h>
#include <potrPeerTable.h>

Mock_potrPeerTable *_mock_potrPeerTable = nullptr;

Mock_potrPeerTable::Mock_potrPeerTable()
{
    ON_CALL(*this, peer_find_by_id(_, _))
        .WillByDefault(Return(nullptr));
    ON_CALL(*this, peer_send_fin(_, _))
        .WillByDefault(Return());
    ON_CALL(*this, peer_free(_, _))
        .WillByDefault(Return());

    _mock_potrPeerTable = this;
}

Mock_potrPeerTable::~Mock_potrPeerTable()
{
    _mock_potrPeerTable = nullptr;
}

WEAK_ATR PotrPeerContext *peer_find_by_id(struct PotrContext_ *ctx, PotrPeerId peer_id)
{
    PotrPeerContext *peer = nullptr;

    if (_mock_potrPeerTable != nullptr)
    {
        peer = _mock_potrPeerTable->peer_find_by_id(ctx, peer_id);
    }

    return peer;
}

WEAK_ATR void peer_send_fin(struct PotrContext_ *ctx, PotrPeerContext *peer)
{
    if (_mock_potrPeerTable != nullptr)
    {
        _mock_potrPeerTable->peer_send_fin(ctx, peer);
    }
}

WEAK_ATR void peer_free(struct PotrContext_ *ctx, PotrPeerContext *peer)
{
    if (_mock_potrPeerTable != nullptr)
    {
        _mock_potrPeerTable->peer_free(ctx, peer);
    }
}
