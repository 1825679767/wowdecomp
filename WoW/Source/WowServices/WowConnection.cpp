#include "WowConnection.h"
#include <cstring>
#include <new>
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowServices/WowConnectionNet.h"

// STATIC: 0x00AB96F8
static uint32_t s_idBitCount;

// STATIC: 0x00AB96FC
static TSFixedArray<uint32_t> s_idBits;

// STATIC: 0x00B38D00
static uint32_t s_unkB38D00;

// STATIC: 0x00B38D20
int32_t WowConnection::s_unkB38D20;

// STATIC: 0x00B38D24
int32_t WowConnection::s_unkB38D24;

// STATIC: 0x00B38D28
static volatile LONG s_numConnections;

// STATIC: 0x00B38D18
bool (*WowConnection::s_verifyAddr)(const NETADDR* addr);

// STATIC: 0x00B38D1C
WowConnectionNet* WowConnection::s_network;

// VERIFY: da47c164 manual
// OFFSET: 0x00467330
WowConnection::WowConnection(WowConnectionResponse* response, void (*threadInit)()) {
    this->Init(response, threadInit);
    this->m_sock = -1;
}

// VERIFY: 9c96cf18 manual
// OFFSET: 0x004669D0
void WowConnection::Init(WowConnectionResponse* response, void (*threadInit)()) {
    InterlockedExchangeAdd(&s_numConnections, 1);

    this->m_refCount.m_value = 1;
    this->m_responseRef = 0;
    this->m_responseD4 = nullptr;
    this->m_maxSendDepth = 100000;
    this->m_sendDepth = 0;
    this->unk_00F0 = 0;
    this->m_oldsock = -1;
    this->unk_0140 = 0;
    this->unk_0144 = -1;

    WOW_CONN_STATE oldState = this->m_connState;
    this->m_connState = WOWC_UNINITIALIZED;

    if (WowConnection::s_network) {
        WowConnection::s_network->PlatformChangeState(this, oldState);
    }

    this->m_response = response;
    this->unk_000C = 0;
    this->m_threadInit = threadInit;
    this->unk_0018 = 0;
    this->unk_0050 = 0;
    this->unk_0054 = 0;
    this->unk_0058 = 0;
    this->unk_005C = 0;
    this->unk_0060 = 0;
    this->unk_0064 = 0;
    this->unk_0078 = 0;
    this->m_connectAddress = 0;
    this->m_connectPort = 0;
    this->m_connectRetryInterval = -1;
    memset(&this->m_peer, 0, sizeof(this->m_peer));
    this->unk_00A0 = 0;
    this->m_serviceFlags = 0;
    this->unk_0130 = 0;
    this->unk_0134 = OsGetAsyncTimeMs();
    this->m_readBuffer = nullptr;
    this->m_readBytes = 0;
    this->m_readBufferSize = 0;
    this->m_event = nullptr;
    this->m_encrypt = false;
    this->m_uint375 = 0;
    this->m_uint376 = 0;
    memset(this->m_sendKeyInit, 0, sizeof(this->m_sendKeyInit));
    memset(this->m_receiveKeyInit, 0, sizeof(this->m_receiveKeyInit));

    oldState = this->m_connState;
    this->m_connState = WOWC_INITIALIZED;

    if (WowConnection::s_network) {
        WowConnection::s_network->PlatformChangeState(this, oldState);
    }

    this->unk_013C = 0;
}

// VERIFY: 5a9a9a3f manual
// OFFSET: 0x00468D90
int32_t WowConnection::InitOsNet(bool (*netAddrFilter)(const NETADDR* addr), void (*threadInit)(), int32_t numThreads, bool useEngine) {
    if (!WowConnection::s_network) {
        s_idBitCount = 256;
        s_idBits.SetCount(s_idBitCount / 32);

        if (s_idBitCount) {
            memset(s_idBits.m_data, 0, sizeof(uint32_t) * s_idBits.m_count);
        }

        WDataStore::StaticInitialize();
        WowConnection::s_verifyAddr = netAddrFilter;
        s_unkB38D00 = 0;

        if (static_cast<uint32_t>(numThreads) > 32) {
            numThreads = 32;
        }

        WowConnection::s_network = new (SMemAlloc(sizeof(WowConnectionNet), __FILE__, __LINE__, 0)) WowConnectionNet(numThreads, threadInit);
        WowConnection::s_network->PlatformInit(useEngine);
        WowConnection::s_network->Start();
    }

    return 1;
}
