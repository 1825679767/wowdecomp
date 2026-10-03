#ifndef WOW_SOURCE_WOWSERVICES_GRUNT_H
#define WOW_SOURCE_WOWSERVICES_GRUNT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Os/OsTcp.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Storm/Source/SThread.h"
#include "WoW/Source/WowServices/SRP6_Client.h"
#include "WoW/Source/WowServices/Types.h"
#include "WoW/Source/WowServices/WowConnection.h"

bool CanReadNumBytes(CDataStore& msg, uint32_t bytes);

namespace Grunt {
struct CdKey {
    struct Info {
        /* 0000 */ uint16_t m_keyLength;
        /* 0002 */ uint8_t unk_0002[2];
        /* 0004 */ uint32_t m_productId;
        /* 0008 */ uint8_t m_publicValue [4];
        /* 000C */ uint8_t m_hash [20];
    };

    /* 0000 */ uint32_t m_count;
    /* 0004 */ TSGrowableArray<Grunt::CdKey::Info> m_keys;
};

template <class T>
struct Command {
    /* 0000 */ uint8_t m_cmd;
    /* 0001 */ uint8_t unk_0001[3];
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t (T::*m_handler)(CDataStore& msg);

    static bool Process(CDataStore& msg, Grunt::Command<T>* commands, uint32_t commandCount, T& object, uint32_t& pos);
};

class ClientResponse {
    public:
    virtual bool Connected(const NETADDR& address) = 0;
    virtual bool Disconnected() = 0;
    virtual bool CantConnect() = 0;
    virtual bool OnlineIdle() = 0;
    virtual void GetLogonMethod() = 0;
    virtual void GetVersionProof(const uint8_t* versionChallenge) = 0;
    virtual void SetPinInfo(bool enabled, uint32_t seed, const uint8_t* salt) = 0;
    virtual void GetPinInfo(bool* enabled, uint8_t** salt, uint8_t** hash) = 0;
    virtual void SetMatrixInfo(bool enabled, uint8_t width, uint8_t height, uint8_t minDigits, uint8_t maxDigits, bool flipCoords, uint8_t challengeCount, uint64_t seed, const uint8_t* sessionKey, uint32_t sessionKeyLength) = 0;
    virtual void GetMatrixProof(bool* enabled, const uint8_t** proof) const = 0;
    virtual void SetTokenInfo(bool enabled, uint8_t required) = 0;
    virtual void GetTokenInfo(bool* enabled, char* token) = 0;
    virtual bool GetCdKeys(Grunt::CdKey& cdKey) = 0;
    virtual void GetFileStatus(const char* fileName, uint64_t size, const uint8_t* md5) = 0;
    virtual void LogonResult(Grunt::Result result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags) = 0;
    virtual void ReconnectResult(Grunt::Result result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags) = 0;
    virtual void FileData(const uint8_t* data, uint32_t size) = 0;
    virtual void RealmListResult(CDataStore* msg) = 0;
    virtual LOGIN_STATE NextSecurityState(LOGIN_STATE state) = 0;
    virtual int32_t GetServerId() = 0;
    virtual const uint8_t* GetVersionChallenge() = 0;
};

class Pending {
    public:
    struct Data {
        /* 0000 */ TSLink<Grunt::Pending::Data> m_link;
        /* 0008 */ uint32_t unk_0008;
        /* 000C */ uint32_t unk_000C;
        /* 0010 */ uint32_t m_time;
    };

    /* 0004 */ uint32_t m_timeout;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ SCritSect m_lock;
    /* 0034 */ TSExplicitList<Grunt::Pending::Data, offsetof(Grunt::Pending::Data, m_link)> m_list;

    virtual void Expired(Grunt::Pending::Data& data) = 0;

    Pending(uint32_t timeout);
    ~Pending();
    void CheckExpired(bool all);
};

class Timer {
    public:
    class Event {
        public:
        /* 0004 */ uint32_t m_nextTime;
        /* 0008 */ uint32_t m_interval;
        /* 000C */ TSLink<Grunt::Timer::Event> m_link;

        virtual void Call() = 0;
    };

    static uint32_t STORMAPI ThreadProc(void* param);

    /* 0000 */ SEvent m_event;
    /* 0004 */ SThread m_thread;
    /* 0008 */ SCritSect m_lock;
    /* 0030 */ TSExplicitList<Grunt::Timer::Event, offsetof(Grunt::Timer::Event, m_link)> m_list;

    Timer();
    ~Timer();
    void Insert(Grunt::Timer::Event& event);
    int32_t Pump();
    void Shutdown();
};

class ClientLink : public WowConnectionResponse, public Grunt::Pending, public Grunt::Timer::Event {
    public:
    struct Logon {
        /* 0000 */ const char* m_accountName;
        /* 0004 */ const char* m_password;
        /* 0008 */ uint32_t m_programId;
        /* 000C */ uint32_t m_processor;
        /* 0010 */ uint32_t m_os;
        /* 0014 */ uint8_t m_versionMajor;
        /* 0015 */ uint8_t m_versionMinor;
        /* 0016 */ uint8_t m_versionPatch;
        /* 0017 */ uint8_t unk_0017[1];
        /* 0018 */ uint16_t m_build;
        /* 001A */ uint8_t unk_001A[2];
        /* 001C */ uint32_t m_locale;
        /* 0020 */ int32_t m_timeZone;
    };

    /* 0058 */ Grunt::Timer m_timer;
    /* 0094 */ int32_t m_serverId;
    /* 0098 */ uint32_t m_accountFlags;
    /* 009C */ uint32_t m_surveyId;
    /* 00A0 */ uint32_t m_localAddress;
    /* 00A4 */ int32_t m_state;
    /* 00A8 */ SRP6_Client m_srpClient;
    /* 0188 */ SCritSect m_lock;
    /* 01B0 */ CDataStore m_datastore;
    /* 01C8 */ WowConnection* m_connection;
    /* 01CC */ Grunt::ClientResponse* m_clientResponse;
    /* 01D0 */ char m_accountName [1280];
    /* 06D0 */ uint8_t m_serverChallenge [32];
    /* 06F0 */ uint8_t m_reconnectKey [40];

    ~ClientLink() override;
    void WCMessageReady(WowConnection* connection, uint32_t timeStamp, CDataStore* msg) override;
    void WCConnected(WowConnection* connection, WowConnection* inbound, uint32_t timeStamp, const NETCONNADDR* addr) override;
    void WCCantConnect(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) override;
    void WCDisconnected(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) override;
    void WCDataReady(WowConnection* connection, uint32_t timeStamp, uint8_t* data, int32_t length) override;
    void Expired(Grunt::Pending::Data& data) override;
    void Call() override;

    ClientLink(Grunt::ClientResponse& clientResponse);
    int32_t CmdAuthLogonChallenge(CDataStore& msg);
    int32_t CmdAuthLogonProof(CDataStore& msg);
    int32_t CmdAuthReconnectChallenge(CDataStore& msg);
    int32_t CmdAuthReconnectProof(CDataStore& msg);
    int32_t CmdRealmList(CDataStore& msg);
    int32_t CmdXferData(CDataStore& msg);
    int32_t CmdXferInitiate(CDataStore& msg);
    void Connect(const char* address);
    void Disconnect();
    void FileComplete();
    void FileStatus(bool accept, uint64_t offset);
    void GetRealmList();
    void LogonNewSession(const Grunt::ClientLink::Logon& logon);
    void LogonStoredSession(const Grunt::ClientLink::Logon& logon);
    void SendSurveyData(uint16_t size, uint8_t* data);
    void SendSurveyFailed();
    void PackLogon(CDataStore& msg, const Grunt::ClientLink::Logon& logon);
    void ProveVersion(const uint8_t* versionChecksum);
    void Send(CDataStore& msg);
    void SetState(int32_t state);
    void Shutdown();
};
}

// VERIFY: badf9db2 manual
// OFFSET: 0x008CD220
template <class T>
bool Grunt::Command<T>::Process(CDataStore& msg, Grunt::Command<T>* commands, uint32_t commandCount, T& object, uint32_t& pos) {
    while (CanReadNumBytes(msg, 1)) {
        uint8_t cmd;
        msg.Get(cmd);

        uint32_t i;

        for (i = 0; i < commandCount; i++) {
            if (commands[i].m_cmd == cmd) {
                break;
            }
        }

        if (i >= commandCount) {
            DebugLog("Bad command received [0x%02X] (unrecognized command)", cmd);
        } else {
            int32_t result = (object.*commands[i].m_handler)(msg);

            if (result == 0) {
                return true;
            }

            if (result == 2) {
                pos = msg.m_read;
                continue;
            }

            if (result != 1) {
                continue;
            }

            DebugLog("Bad command received [0x%02X] (Corrupt)", cmd);
        }

        uint32_t bytes = msg.m_size - msg.m_read;

        if (bytes >= 128) {
            bytes = 128;
        }

        void* data;
        msg.GetDataInSitu(data, bytes);

        if (data && bytes) {
            DebugLog("%u bytes", bytes);
        }

        return false;
    }

    return true;
}

#endif
