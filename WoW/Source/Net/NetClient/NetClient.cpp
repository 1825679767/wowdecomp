#include "NetClient.h"
#include <cstring>
#include <new>
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00AD2ED0
static TSList<NETCLIENTNODE> s_clientList;

// STATIC: 0x00C5D624
static HPROPCONTEXT s_propContext;

// STATIC: 0x00C5D628
static uint32_t s_stats[6];

// STATIC: 0x00C5D640
static uint32_t s_initializeTime;

// STATIC: 0x00C5D644
static uint32_t s_idleStats[4];

// STATIC: 0x00C5D654
uint32_t NetClient::s_clientCount;

// VERIFY: 74c166be manual
// OFFSET: 0x00633440
NETEVENTQUEUE::NETEVENTQUEUE(NetClient* client) {
    this->m_client = client;
}

// VERIFY: eb4e833d manual
// OFFSET: 0x006334F0
void NETEVENTQUEUE::Poll() {
    this->m_critSect.Enter();

    bool destroyed = false;
    this->m_client->m_refCount.Increment();

    for (NETEVENTQUEUENODE* node = this->m_eventQueue.Head(); node; node = this->m_eventQueue.Next(node)) {
        NetClient* client = this->m_client;

        if (!client->m_deleteMe) {
            switch (node->m_eventId) {
                case EVENT_ID_NET_DATA:
                    client->HandleData(node->m_timeReceived, node->m_data, node->m_dataSize);
                    break;

                case EVENT_ID_NET_CONNECT:
                    client->HandleConnect();
                    break;

                case EVENT_ID_NET_DISCONNECT:
                    client->HandleDisconnect();
                    break;

                case EVENT_ID_NET_CANTCONNECT:
                    client->HandleCantConnect();
                    break;

                case EVENT_ID_NET_DESTROY:
                    client->m_deleteMe = true;
                    destroyed = true;
                    break;

                case EVENT_ID_NET_AUTH_CHALLENGE:
                    client->HandleAuthChallenge(static_cast<AuthenticationChallenge*>(node->m_data));
                    break;

                default:
                    break;
            }
        }

        client = this->m_client;

        if (client->m_refCount.Decrement() == 0 && client->m_deleteMe) {
            delete client;
        }
    }

    if (!destroyed) {
        this->m_client->HandleIdle();
    }

    NetClient* client = this->m_client;

    if (client->m_refCount.Decrement() == 0 && client->m_deleteMe) {
        delete client;
    }

    this->m_eventQueue.Clear();
    this->m_critSect.Leave();
}

// VERIFY: dd90b2c3 auto
// OFFSET: 0x00631D10
static void InitializePropContext() {
    if (PropGetSelectedContext() != s_propContext) {
        PropSelectContext(s_propContext);
    }
}

// VERIFY: 8093d1d7 manual
// OFFSET: 0x00631D30
int32_t NetClient::Initialize() {
    if (!NetClient::s_clientCount) {
        s_propContext = PropGetSelectedContext();

        int32_t result = WowConnection::InitOsNet(nullptr, InitializePropContext, 1, false);

        if (!result) {
            return result;
        }
    }

    NetClient::s_clientCount++;

    this->m_netEventQueue = new (SMemAlloc(sizeof(NETEVENTQUEUE), __FILE__, __LINE__, 0)) NETEVENTQUEUE(this);
    memset(this->m_handlers, 0, sizeof(this->m_handlers));
    memset(this->m_handlerParams, 0, sizeof(this->m_handlerParams));
    this->m_serverConnection = new (SMemAlloc(sizeof(WowConnection), __FILE__, __LINE__, 0)) WowConnection(this, nullptr);
    this->m_netState = NS_INITIALIZED;
    s_initializeTime = OsGetAsyncTimeMs();

    return 1;
}

// VERIFY: f1080d7c auto
// OFFSET: 0x00631FA0
void NetClient::SetMessageHandler(NETMESSAGE msgId, MESSAGE_HANDLER handler, void* param) {
    this->m_handlers[msgId] = handler;
    this->m_handlerParams[msgId] = param;
}

// VERIFY: acb4d340 auto
// OFFSET: 0x00631FC0
void NetClient::ResetHandler(NETMESSAGE msgId) {
    this->m_handlers[msgId] = nullptr;
    this->m_handlerParams[msgId] = nullptr;
}

// VERIFY: 2549c452 auto
// OFFSET: 0x006321A0
void NetClient::PollEventQueue() {
    this->m_netEventQueue->Poll();
}

// VERIFY: 1f76bddf manual
// OFFSET: 0x00632A40
NetClient::NetClient() {
    this->m_netState = NS_UNINITIALIZED;
    this->m_netEventQueue = nullptr;
    this->m_serverConnection = nullptr;
    this->m_redirectConnection = nullptr;
    this->m_suspended = false;
    this->m_refCount.m_value = 0;
    this->m_deleteMe = false;
    this->m_pingSent = 0;
    this->m_pingSequence = 0;
    this->m_bytesSent = 0;
    this->m_bytesReceived = 0;
    this->m_connectedTimestamp = 0;
    this->m_objMgr = nullptr;

    NETCLIENTNODE* node = s_clientList.NewNode(2, 0, 0);
    node->m_client = this;
}

// VERIFY: af338d14 manual
// OFFSET: 0x00632CD0
void NetClient::Ping() {
    if (!this->m_serverConnection->m_encrypt) {
        return;
    }

    CDataStore msg;

    this->m_pingLock.Enter();
    this->m_pingSent = OsGetAsyncTimeMs();
    msg.Put(static_cast<uint32_t>(CMSG_PING));
    msg.Put(++this->m_pingSequence);

    if (this->m_netState == NS_CONNECTED) {
        if (this->m_latencyEnd) {
            msg.Put(this->m_latency[this->m_latencyEnd - 1]);
        } else {
            msg.Put(static_cast<uint32_t>(0));
        }
    }

    this->m_pingLock.Leave();
    msg.Finalize();
    this->Send(&msg);
}

// VERIFY: 55fb6d51 auto
// OFFSET: 0x00632DB0
void NetClient::HandleIdle() {
    s_idleStats[0] = s_stats[0];
    s_idleStats[1] = s_stats[3];
    s_idleStats[2] = s_stats[1];
    s_idleStats[3] = s_stats[4];

    if (static_cast<int32_t>(OsGetAsyncTimeMs() - this->m_pingSent - 30000) >= 0) {
        this->Ping();
    }
}
