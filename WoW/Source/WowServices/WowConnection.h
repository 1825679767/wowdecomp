#ifndef WOW_SOURCE_WOWSERVICES_WOWCONNECTION_H
#define WOW_SOURCE_WOWSERVICES_WOWCONNECTION_H

#include <cstddef>
#include <cstdint>
#include "Storm/H/stpl.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/WowServices/Types.h"
#include "_Triage/SCritSect.h"

class WowConnectionResponse;

struct NETADDR {
    /* 0000 */ uint16_t m_family;
    /* 0002 */ char m_data [14];
};

struct NETCONNADDR {
    /* 0000 */ NETADDR m_peerAddr;
    /* 0010 */ NETADDR m_selfAddr;
};

struct SARC4Key {
    /* 0000 */ uint8_t m_state [256];
    /* 0100 */ uint8_t m_x;
    /* 0101 */ uint8_t m_y;
};

class WowConnection {
    public:
    struct SENDNODE {
        /* 0000 */ TSLinkedNode<WowConnection::SENDNODE> unk_0000;
        /* 0008 */ uint8_t* m_data;
        /* 000C */ uint32_t m_size;
        /* 0010 */ uint32_t m_offset;
        /* 0014 */ uint32_t m_datasize;
        /* 0018 */ uint8_t m_header [8];
        /* 0020 */ uint32_t m_uint20;
        /* 0024 */ uint32_t m_allocsize;
    };

    /* 0000 */ ATOMIC32 m_refCount;
    /* 0004 */ int32_t m_sock;
    /* 0008 */ int32_t m_oldsock;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ WOW_CONN_STATE m_connState;
    /* 0014 */ WowConnectionResponse* m_response;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ uint8_t* m_readBuffer;
    /* 0020 */ int32_t m_readBytes;
    /* 0024 */ int32_t m_readBufferSize;
    /* 0028 */ SCritSect m_outLock;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ int32_t unk_005C;
    /* 0060 */ int32_t unk_0060;
    /* 0064 */ int32_t unk_0064;
    /* 0068 */ int32_t unk_0068;
    /* 006C */ uint32_t m_connectAddress;
    /* 0070 */ uint16_t m_connectPort;
    /* 0072 */ uint8_t unk_0072[2];
    /* 0074 */ int32_t m_connectRetryInterval;
    /* 0078 */ int32_t unk_0078;
    /* 007C */ int32_t unk_007C;
    /* 0080 */ NETCONNADDR m_peer;
    /* 00A0 */ int32_t unk_00A0;
    /* 00A4 */ SCritSect m_responseLock;
    /* 00CC */ int32_t m_responseRef;
    /* 00D0 */ uintptr_t m_responseRefThread;
    /* 00D4 */ WowConnectionResponse* m_responseD4;
    /* 00D8 */ int32_t unk_00D8;
    /* 00DC */ int32_t unk_00DC;
    /* 00E0 */ TSList<WowConnection::SENDNODE> m_sendList;
    /* 00EC */ int32_t m_sendDepth;
    /* 00F0 */ int32_t unk_00F0;
    /* 00F4 */ int32_t m_maxSendDepth;
    /* 00F8 */ int32_t m_serviceFlags;
    /* 00FC */ int32_t unk_00FC;
    /* 0100 */ int32_t unk_0100;
    /* 0104 */ int32_t unk_0104;
    /* 0108 */ SCritSect m_lock;
    /* 0130 */ int32_t unk_0130;
    /* 0134 */ int32_t unk_0134;
    /* 0138 */ void* m_event;
    /* 013C */ int32_t unk_013C;
    /* 0140 */ int32_t unk_0140;
    /* 0144 */ int32_t unk_0144;
    /* 0148 */ SARC4Key m_sendKey;
    /* 024A */ SARC4Key m_receiveKey;
    /* 034C */ uint8_t m_sendKeyInit [20];
    /* 0360 */ uint8_t m_receiveKeyInit [20];
    /* 0374 */ bool m_encrypt;
    /* 0375 */ uint8_t m_uint375;
    /* 0376 */ uint8_t m_uint376;
    /* 0377 */ uint8_t unk_0377[1];
};

class WowConnectionResponse {
    public:
    virtual ~WowConnectionResponse();
};

class ServerConnection {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[8];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[180];
    /* 00CC */ uint32_t unk_00CC;
    /* 00D0 */ uint32_t unk_00D0;
    /* 00D4 */ uint32_t unk_00D4;
    /* 00D8 */ uint8_t unk_00D8[16];
    /* 00E8 */ uint32_t unk_00E8;
    /* 00EC */ uint32_t unk_00EC;
    /* 00F0 */ uint32_t unk_00F0;
    /* 00F4 */ uint32_t unk_00F4;
    /* 00F8 */ uint8_t unk_00F8[596];
    /* 034C */ uint32_t unk_034C;
    /* 0350 */ uint32_t unk_0350;
    /* 0354 */ uint32_t unk_0354;
    /* 0358 */ uint32_t unk_0358;
    /* 035C */ uint32_t unk_035C;
    /* 0360 */ uint32_t unk_0360;
    /* 0364 */ uint32_t unk_0364;
    /* 0368 */ uint32_t unk_0368;
    /* 036C */ uint32_t unk_036C;
    /* 0370 */ uint32_t unk_0370;
    /* 0374 */ uint8_t unk_0374;
    /* 0375 */ uint8_t unk_0375;
};

class WoWConnection {
    public:
    /* 0000 */ uint8_t unk_0000[232];
    /* 00E8 */ uint32_t unk_00E8;
    /* 00EC */ uint8_t unk_00EC[24];
    /* 0104 */ uint32_t unk_0104;
};

#endif
