#ifndef WOW_SOURCE_NET_NETCLIENT_NETCLIENT_H
#define WOW_SOURCE_NET_NETCLIENT_NETCLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "WoW/Source/Net/NetClient/Types.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

struct CGObject_C;
struct CHashKeyGUID;
struct ClientConnection;
struct ClntObjMgr;
struct NETEVENTQUEUE;
struct WowConnection;

typedef uint32_t ATOMIC32;

struct ClntObjMgr {
    /* 0000 */ TSHashTable<CGObject_C, CHashKeyGUID> m_objects;
    /* 0028 */ TSHashTable<CGObject_C, CHashKeyGUID> m_lazyCleanupObjects;
    /* 0050 */ TSList<CGObject_C> m_lazyCleanupFifo [7];
    /* 00A4 */ TSList<CGObject_C> m_visibleObjects;
    /* 00B0 */ TSList<CGObject_C> m_reenabledObjects;
    /* 00BC */ uint8_t unk_00BC[4];
    /* 00C0 */ WOWGUID m_activePlayer;
    /* 00C8 */ PLAYER_TYPE m_type;
    /* 00CC */ uint32_t m_mapID;
    /* 00D0 */ ClientConnection* m_net;
    /* 00D4 */ uint8_t unk_00D4[4];
};

struct LoginData {
    /* 0000 */ char m_account [1280];
    /* 0500 */ int32_t m_loginServerID;
    /* 0504 */ uint8_t m_sessionKey [40];
    /* 052C */ int32_t m_loginServerType;
};

typedef int32_t (__stdcall* MESSAGE_HANDLER_interface )(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

typedef MESSAGE_HANDLER_interface MESSAGE_HANDLER;

class NetClient {
    public:
    struct HELDMESSAGE {
        /* 0000 */ TSLinkedNode<NetClient::HELDMESSAGE> unk_0000;
        /* 0008 */ CDataStore m_msg;
    };

    /* 0004 */ LoginData m_loginData;
    /* 0534 */ NETSTATE m_netState;
    /* 0538 */ bool m_suspended;
    /* 0539 */ uint8_t unk_0539[3];
    /* 053C */ MESSAGE_HANDLER m_handlers [1311];
    /* 19B8 */ void* m_handlerParams [1311];
    /* 2E34 */ NETEVENTQUEUE* m_netEventQueue;
    /* 2E38 */ WowConnection* m_serverConnection;
    /* 2E3C */ WowConnection* m_redirectConnection;
    /* 2E40 */ ATOMIC32 m_refCount;
    /* 2E44 */ bool m_deleteMe;
    /* 2E45 */ uint8_t unk_2E45[3];
    /* 2E48 */ uint32_t unk_2E48;
    /* 2E4C */ uint32_t m_pingSent;
    /* 2E50 */ uint32_t m_pingSequence;
    /* 2E54 */ uint32_t m_latency [16];
    /* 2E94 */ uint32_t m_latencyStart;
    /* 2E98 */ uint32_t m_latencyEnd;
    /* 2E9C */ uint32_t m_bytesSent;
    /* 2EA0 */ uint32_t m_bytesReceived;
    /* 2EA4 */ uint32_t m_connectedTimestamp;
    /* 2EA8 */ SCritSect m_pingLock;
    /* 2ED0 */ ClntObjMgr* m_objMgr;
    /* 2ED4 */ TSList<NetClient::HELDMESSAGE> m_heldMessages;

    virtual ~NetClient();
};

class CNetClient {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[1328];
    /* 0534 */ uint32_t unk_0534;
    /* 0538 */ uint8_t unk_0538[4];
    /* 053C */ uint32_t unk_053C;
    /* 0540 */ uint8_t unk_0540[5240];
    /* 19B8 */ uint32_t unk_19B8;
    /* 19BC */ uint8_t unk_19BC[5244];
    /* 2E38 */ uint32_t unk_2E38;
    /* 2E3C */ uint8_t unk_2E3C[256];
    /* 2F3C */ uint32_t unk_2F3C;
    /* 2F40 */ uint8_t unk_2F40[4];
    /* 2F44 */ uint32_t unk_2F44;
    /* 2F48 */ uint32_t unk_2F48;
    /* 2F4C */ uint32_t unk_2F4C;
    /* 2F50 */ uint32_t unk_2F50;
    /* 2F54 */ uint32_t unk_2F54;
    /* 2F58 */ uint8_t unk_2F58[1];
    /* 2F59 */ uint8_t unk_2F59;
    /* 2F5A */ uint8_t unk_2F5A[2];
    /* 2F5C */ uint32_t unk_2F5C;
};

#endif
