#include "Grunt.h"
#include <cstring>
#include <malloc.h>
#include "Engine/Source/Os/OsSecureRandom.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "WoW/Source/WowServices/SRP6_Random.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00B24D00
static Grunt::Command<Grunt::ClientLink> s_authCmds[] = {
    { 0x00, {}, "ClientLink::CMD_AUTH_LOGON_CHALLENGE", &Grunt::ClientLink::CmdAuthLogonChallenge },
    { 0x01, {}, "ClientLink::CMD_AUTH_LOGON_PROOF", &Grunt::ClientLink::CmdAuthLogonProof },
    { 0x02, {}, "ClientLink::CMD_AUTH_RECONNECT_CHALLENGE", &Grunt::ClientLink::CmdAuthReconnectChallenge },
    { 0x03, {}, "ClientLink::CMD_AUTH_RECONNECT_PROOF", &Grunt::ClientLink::CmdAuthReconnectProof },
    { 0x10, {}, "ClientLink::CMD_REALM_LIST", &Grunt::ClientLink::CmdRealmList },
    { 0x30, {}, "ClientLink::CMD_XFER_INITIATE", &Grunt::ClientLink::CmdXferInitiate },
    { 0x31, {}, "ClientLink::CMD_XFER_DATA", &Grunt::ClientLink::CmdXferData },
};

// STATIC: 0x00D45CD0
static char s_accountNameUnDecorated[1280];

// INLINED
Grunt::Pending::Pending(uint32_t timeout) {
    this->m_timeout = timeout;
    this->unk_0008 = 0;
}

// INLINED
void Grunt::Timer::Shutdown() {
    if (!this->m_thread.Valid()) {
        return;
    }

    this->m_event.Set();
    this->m_thread.Wait(0xFFFFFFFF);
    this->m_thread.Close();

    this->m_lock.Enter();
    this->m_list.UnlinkAll();
    this->m_lock.Leave();
}

// VERIFY: 93be7f01 auto
// OFFSET: 0x008CBA40
void Grunt::ClientLink::Send(CDataStore& msg) {
    this->m_lock.Enter();

    if (this->m_connection) {
        uint32_t size = msg.m_size;
        void* data;
        msg.GetDataInSitu(data, size);
        this->m_connection->Send(static_cast<uint8_t*>(data), size, false);
    }

    this->m_lock.Leave();
}

// VERIFY: c3ecd7d1 auto
// OFFSET: 0x008CBA90
void Grunt::ClientLink::Shutdown() {
    this->m_lock.Enter();

    if (this->m_connection) {
        this->m_connection->Disconnect();
        this->m_connection->SetResponse(nullptr, false);
        this->m_connection->Release();
        this->m_connection = nullptr;
    }

    this->m_lock.Leave();
}

// VERIFY: 08907387 auto
// OFFSET: 0x008CBAE0
void Grunt::ClientLink::Disconnect() {
    this->m_lock.Enter();

    if (this->m_connection) {
        this->m_connection->Disconnect();
    }

    this->m_lock.Leave();
}

// VERIFY: a80ac601 auto
// OFFSET: 0x008CBB10
void Grunt::ClientLink::SetState(int32_t state) {
    this->m_lock.Enter();
    this->m_state = state;
    this->m_lock.Leave();
}

// VERIFY: 32e7119e manual
// OFFSET: 0x008CBB40
void Grunt::ClientLink::Connect(const char* address) {
    if (this->m_state) {
        return;
    }

    this->SetState(1);

    this->m_connection = new (__FILE__, __LINE__) WowConnection(this, nullptr);
    this->m_connection->SetType(WOWC_TYPE_STREAM);

    if (SStrChr(address, ':')) {
        this->m_connection->Connect(address, 5000);
    } else {
        this->m_connection->Connect(address, 3724, 5000);
    }
}

// VERIFY: 8c632a8e auto
// OFFSET: 0x008CCCB0
void Grunt::ClientLink::WCConnected(WowConnection* connection, WowConnection* inbound, uint32_t timeStamp, const NETCONNADDR* addr) {
    this->m_lock.Enter();
    this->SetState(2);
    bool failed = !this->m_clientResponse->Connected(addr->peerAddr);
    this->m_localAddress = OsNetAddrGetAddress(&addr->selfAddr, nullptr);
    this->m_lock.Leave();

    if (failed) {
        this->Disconnect();
    }
}

// VERIFY: 1c69ff99 manual
// OFFSET: 0x008CCD40
void Grunt::ClientLink::WCDisconnected(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) {
    this->m_lock.Enter();
    this->Shutdown();
    this->SetState(0);
    this->m_lock.Leave();

    if (this->m_clientResponse->Disconnected()) {
        delete this;
    }
}

// VERIFY: bc4a5436 manual
// OFFSET: 0x008CCDA0
void Grunt::ClientLink::WCCantConnect(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) {
    this->m_lock.Enter();
    this->Shutdown();
    this->SetState(0);
    this->m_lock.Leave();

    if (this->m_clientResponse->CantConnect()) {
        delete this;
    }
}

// VERIFY: 5fb5257a auto
// OFFSET: 0x008CCE00
void Grunt::ClientLink::PackLogon(CDataStore& msg, const Grunt::ClientLink::Logon& logon) {
    uint32_t start = msg.m_size;

    msg.Put(static_cast<uint16_t>(0));
    msg.Put(logon.m_programId);
    msg.Put(logon.m_versionMajor);
    msg.Put(logon.m_versionMinor);
    msg.Put(logon.m_versionPatch);
    msg.Put(logon.m_build);
    msg.Put(logon.m_processor);
    msg.Put(logon.m_os);
    msg.Put(logon.m_locale);
    msg.Put(static_cast<uint32_t>(logon.m_timeZone));
    msg.Put(this->m_localAddress);

    uint8_t length = SStrLen(this->m_accountName);
    msg.Put(length);
    msg.PutData(this->m_accountName, length);

    msg.Set(start, static_cast<uint16_t>(msg.m_size - start - 2));
}

// VERIFY: abfd5c66 manual
// OFFSET: 0x008CD7E0
void Grunt::ClientLink::LogonNewSession(const Grunt::ClientLink::Logon& logon) {
    this->SetState(3);

    SStrCopy(this->m_accountName, logon.m_accountName, sizeof(this->m_accountName));
    SStrUpper(this->m_accountName);

    char* password = static_cast<char*>(alloca(SStrLen(logon.m_password) + 1));
    SStrCopy(password, logon.m_password, STORM_MAX_STR);
    SStrUpper(password);

    SStrCopy(s_accountNameUnDecorated, this->m_accountName, STORM_MAX_STR);

    char* decoration = SStrChr(s_accountNameUnDecorated, '#');

    if (decoration) {
        *decoration = '\0';
    }

    this->m_srpClient.BeginAuthentication(s_accountNameUnDecorated, password);

    CDataStoreCache<1024> msg;
    msg.Put(static_cast<uint8_t>(0));
    msg.Put(static_cast<uint8_t>(8));
    this->PackLogon(msg, logon);
    msg.Finalize();
    this->Send(msg);

    memset(password, 0, SStrLen(password));
}

// VERIFY: 902b90a7 manual
// OFFSET: 0x008CD9F0
void Grunt::ClientLink::LogonStoredSession(const Grunt::ClientLink::Logon& logon) {
    this->SetState(3);

    SStrCopy(this->m_accountName, logon.m_accountName, sizeof(this->m_accountName));
    SStrUpper(this->m_accountName);
    memcpy(this->m_reconnectKey, logon.m_password, sizeof(this->m_reconnectKey));

    CDataStoreCache<1024> msg;
    msg.Put(static_cast<uint8_t>(2));
    msg.Put(static_cast<uint8_t>(8));
    this->PackLogon(msg, logon);
    msg.Finalize();
    this->Send(msg);
}

// VERIFY: 01aad7f9 auto
// OFFSET: 0x008CE5C0
void Grunt::Timer::Insert(Grunt::Timer::Event& event) {
    this->m_lock.Enter();

    event.m_nextTime = event.m_interval + OsGetAsyncTimeMs();

    for (Grunt::Timer::Event* existing = this->m_list.Head(); !this->m_list.IsEnd(existing); existing = this->m_list.RawNext(existing)) {
        if (static_cast<int32_t>(event.m_nextTime - existing->m_nextTime) < 0) {
            this->m_list.LinkNode(&event, 2, existing);
            this->m_lock.Leave();
            return;
        }
    }

    this->m_list.LinkToTail(&event);
    this->m_lock.Leave();
}

// VERIFY: 2b47cb0c manual
// OFFSET: 0x008CE6D0
Grunt::Pending::~Pending() {
}

// VERIFY: 5ce0f696 manual
// OFFSET: 0x008CED90
int32_t Grunt::Timer::Pump() {
    this->m_lock.Enter();

    Grunt::Timer::Event* event = this->m_list.Head();

    if (!event) {
        this->m_lock.Leave();
        return 100;
    }

    int32_t remaining = event->m_nextTime - OsGetAsyncTimeMs();

    if (remaining > 0) {
        this->m_lock.Leave();
        return remaining;
    }

    this->m_list.UnlinkNode(event);
    this->m_lock.Leave();

    event->Call();
    this->Insert(*event);

    return 100;
}

// VERIFY: c0fe3e15 manual
// OFFSET: 0x008CEE20
Grunt::Timer::~Timer() {
    this->Shutdown();
}

// VERIFY: 374cd430 manual
// OFFSET: 0x008CEF70
void Grunt::Pending::CheckExpired(bool all) {
    uint32_t now = OsGetAsyncTimeMs();

    TSExplicitList<Grunt::Pending::Data, offsetof(Grunt::Pending::Data, m_link)> expired;

    this->m_lock.Enter();

    Grunt::Pending::Data* data = this->m_list.Head();

    while (!this->m_list.IsEnd(data)) {
        if (!all) {
            if (!data->m_time) {
                data = this->m_list.RawNext(data);
                continue;
            }

            if (now - data->m_time < this->m_timeout) {
                break;
            }
        }

        this->m_list.UnlinkNode(data);
        expired.LinkToTail(data);
        data = this->m_list.RawNext(data);
    }

    this->m_lock.Leave();

    for (data = expired.Head(); data; data = expired.DeleteNode(data)) {
        this->Expired(*data);
    }
}

// VERIFY: a73f2c4e manual
// OFFSET: 0x008CF600
void Grunt::ClientLink::Call() {
    this->CheckExpired(false);

    this->m_lock.Enter();

    if (this->m_state == 2) {
        this->m_clientResponse->GetLogonMethod();
    } else if (this->m_state == 6 && !this->m_clientResponse->OnlineIdle()) {
        this->Disconnect();
    }

    this->m_lock.Leave();
}

// VERIFY: 10d55201 manual
// OFFSET: [0x008CF680, 0x008CF910]
Grunt::ClientLink::~ClientLink() {
    this->m_timer.Shutdown();
    this->Shutdown();
}

// VERIFY: 8e029f02 auto
// OFFSET: 0x008CF770
uint32_t STORMAPI Grunt::Timer::ThreadProc(void* param) {
    Grunt::Timer* timer = static_cast<Grunt::Timer*>(param);
    uint32_t timeout;

    do {
        timeout = timer->Pump();
    } while (timer->m_event.Wait(timeout));

    return 1;
}

// VERIFY: 44245c80 manual
// OFFSET: 0x008CF7A0
Grunt::Timer::Timer() : m_event(0, 0) {
    SThread::Create(Grunt::Timer::ThreadProc, this, this->m_thread, const_cast<char*>("GruntTimerEvt"), 0);
}

// VERIFY: ec01e82e manual
// OFFSET: 0x008CF800
Grunt::ClientLink::ClientLink(Grunt::ClientResponse& clientResponse) : Grunt::Pending(18000), m_serverId(0), m_accountFlags(0), m_surveyId(0), m_localAddress(0), m_connection(nullptr), m_clientResponse(&clientResponse) {
    this->SetState(0);

    if (this->m_timer.m_thread.Valid()) {
        this->m_interval = 100;
        this->m_timer.Insert(*this);
    }
}

void Grunt::ClientLink::WCMessageReady(WowConnection* connection, uint32_t timeStamp, CDataStore* msg) {
}

void Grunt::ClientLink::Expired(Grunt::Pending::Data& data) {
}

// VERIFY: 54a85476 auto
// OFFSET: 0x008CBBF0
bool CanReadNumBytes(CDataStore& msg, uint32_t bytes) {
    return msg.m_read <= msg.m_size && msg.m_size - msg.m_read >= bytes;
}

// VERIFY: 58b79446 auto
// OFFSET: 0x008CC250
int32_t Grunt::ClientLink::CmdRealmList(CDataStore& msg) {
    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 2) {
        return 0;
    }

    uint16_t size;
    msg.Get(size);

    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < size) {
        return 0;
    }

    uint32_t start = msg.m_read;

    uint32_t unknown;
    msg.Get(unknown);

    uint32_t realmStart = msg.m_read;

    uint16_t count;
    msg.Get(count);

    for (uint32_t i = 0; i < count; i++) {
        uint8_t type;
        uint8_t locked;
        uint8_t flags;
        char name[256];
        char address[256];
        float population;
        uint8_t characters;
        uint8_t timeZone;
        uint8_t realmId;

        msg.Get(type);
        msg.Get(locked);
        msg.Get(flags);
        msg.GetString(name, sizeof(name));
        msg.GetString(address, sizeof(address));
        msg.Get(population);
        msg.Get(characters);
        msg.Get(timeZone);
        msg.Get(realmId);

        if (flags & 0x4) {
            uint8_t versionMajor;
            uint8_t versionMinor;
            uint8_t versionRevision;
            uint16_t build;

            msg.Get(versionMajor);
            msg.Get(versionMinor);
            msg.Get(versionRevision);
            msg.Get(build);
        }

        if (msg.m_read > msg.m_size) {
            return 1;
        }
    }

    uint16_t trailer;
    msg.Get(trailer);

    uint32_t end = msg.m_read;

    if (end > msg.m_size || end - start != size) {
        return 1;
    }

    msg.m_read = realmStart;
    this->m_clientResponse->RealmListResult(&msg);
    msg.m_read = end;

    return 2;
}

// VERIFY: 41cff6c4 manual
// OFFSET: 0x008CC3E0
int32_t Grunt::ClientLink::CmdAuthLogonChallenge(CDataStore& msg) {
    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 2) {
        return 0;
    }

    uint8_t protocol;
    msg.Get(protocol);

    if (protocol) {
        return 1;
    }

    uint8_t result;
    msg.Get(result);

    if (result == Grunt::GRUNT_RESULT_SUCCESS) {
        if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 33) {
            return 0;
        }

        void* serverPublicKey;
        msg.GetDataInSitu(serverPublicKey, 32);

        uint8_t generatorLength;
        msg.Get(generatorLength);

        if (!CanReadNumBytes(msg, generatorLength + 1)) {
            return 0;
        }

        void* generator;
        msg.GetDataInSitu(generator, generatorLength);

        uint8_t largeSafePrimeLength;
        msg.Get(largeSafePrimeLength);

        if (!CanReadNumBytes(msg, largeSafePrimeLength + 48)) {
            return 0;
        }

        void* largeSafePrime;
        msg.GetDataInSitu(largeSafePrime, largeSafePrimeLength);

        void* salt;
        msg.GetDataInSitu(salt, 32);

        void* versionChallenge;
        msg.GetDataInSitu(versionChallenge, 16);

        if (!CanReadNumBytes(msg, 1)) {
            return 0;
        }

        uint8_t securityFlags;
        msg.Get(securityFlags);

        uint32_t pinSeed = 0;
        void* pinSalt = nullptr;

        if (securityFlags & 0x1) {
            if (!CanReadNumBytes(msg, 20)) {
                return 0;
            }

            msg.Get(pinSeed);
            msg.GetDataInSitu(pinSalt, 16);
        }

        uint8_t matrixWidth = 0;
        uint8_t matrixHeight = 0;
        uint8_t matrixDigitCount = 0;
        uint8_t matrixChallengeCount = 0;
        uint64_t matrixSeed = 0;

        if (securityFlags & 0x2) {
            if (!CanReadNumBytes(msg, 12)) {
                return 0;
            }

            msg.Get(matrixWidth);
            msg.Get(matrixHeight);
            msg.Get(matrixDigitCount);
            msg.Get(matrixChallengeCount);
            msg.Get(matrixSeed);

            if ((securityFlags & 0x2) && !matrixChallengeCount) {
                return 1;
            }
        }

        uint8_t tokenRequired = 0;

        if (securityFlags & 0x4) {
            if (!CanReadNumBytes(msg, 1)) {
                return 0;
            }

            msg.Get(tokenRequired);
        }

        if (msg.m_read > msg.m_size) {
            return 1;
        }

        memcpy(this->m_serverChallenge, serverPublicKey, sizeof(this->m_serverChallenge));

        uint8_t seed[16];
        OsSecureRandom(seed, sizeof(seed));
        SRP6_Random random(seed, sizeof(seed));

        if (this->m_srpClient.CalculateProof(static_cast<uint8_t*>(largeSafePrime), largeSafePrimeLength, static_cast<uint8_t*>(generator), generatorLength, static_cast<uint8_t*>(salt), 32, static_cast<uint8_t*>(serverPublicKey), 32, random)) {
            this->SetState(2);
            this->m_clientResponse->LogonResult(Grunt::GRUNT_RESULT_INCORRECT_PASSWORD, nullptr, 0, 0);
        } else {
            this->SetState(4);
            this->m_clientResponse->SetPinInfo((securityFlags & 0x1) == 0x1, pinSeed, static_cast<uint8_t*>(pinSalt));
            this->m_clientResponse->SetMatrixInfo((securityFlags & 0x2) == 0x2, matrixWidth, matrixHeight, matrixDigitCount, matrixDigitCount, false, matrixChallengeCount, matrixSeed, this->m_srpClient.m_sessionKey, sizeof(this->m_srpClient.m_sessionKey));
            this->m_clientResponse->SetTokenInfo((securityFlags & 0x4) == 0x4, tokenRequired);
            this->m_clientResponse->GetVersionProof(static_cast<uint8_t*>(versionChallenge));
        }

        return 2;
    }

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    this->SetState(2);

    if (result >= 33) {
        DebugLog("Unknown grunt result %d @ %d", result, __LINE__);
        result = Grunt::GRUNT_RESULT_DISCONNECTED;
    }

    this->m_clientResponse->LogonResult(static_cast<Grunt::Result>(result), nullptr, 0, 0);

    return 2;
}

// VERIFY: c4dc2df1 manual
// OFFSET: 0x008CC7A0
int32_t Grunt::ClientLink::CmdAuthLogonProof(CDataStore& msg) {
    if (msg.m_read >= msg.m_size) {
        return 0;
    }

    uint8_t result;
    msg.Get(result);

    if (result == Grunt::GRUNT_RESULT_SUCCESS) {
        if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 24) {
            return 0;
        }

        void* serverProof;
        msg.GetDataInSitu(serverProof, 20);

        uint32_t accountFlags = 0;
        msg.Get(accountFlags);

        uint32_t surveyId;
        msg.Get(surveyId);

        if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 2) {
            return 0;
        }

        uint16_t loginFlags = 0;
        msg.Get(loginFlags);

        if (msg.m_read > msg.m_size) {
            return 1;
        }

        if (this->m_srpClient.VerifyServerProof(static_cast<uint8_t*>(serverProof), 20)) {
            this->SetState(2);
            this->m_clientResponse->LogonResult(Grunt::GRUNT_RESULT_UNKNOWN_11, nullptr, 0, 0);
        } else {
            this->m_accountFlags = accountFlags;
            this->m_serverId = 0;
            this->m_surveyId = surveyId;
            this->SetState(6);
            this->m_clientResponse->LogonResult(Grunt::GRUNT_RESULT_SUCCESS, this->m_srpClient.m_sessionKey, sizeof(this->m_srpClient.m_sessionKey), loginFlags);
        }

        return 2;
    }

    if (result == Grunt::GRUNT_RESULT_UNKNOWN_ACCOUNT) {
        if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 2) {
            return 0;
        }

        uint16_t unknown = 0;
        msg.Get(unknown);
    }

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    if (result != Grunt::GRUNT_RESULT_DOWNLOAD_FILE) {
        this->SetState(2);

        if (result >= 33) {
            DebugLog("Unknown grunt result %d @ %d", result, __LINE__);
            result = Grunt::GRUNT_RESULT_DISCONNECTED;
        }

        this->m_clientResponse->LogonResult(static_cast<Grunt::Result>(result), nullptr, 0, 0);
    }

    return 2;
}

// VERIFY: ea70b617 manual
// OFFSET: 0x008CD710
void Grunt::ClientLink::WCDataReady(WowConnection* connection, uint32_t timeStamp, uint8_t* data, int32_t length) {
    this->m_datastore.PutArray(data, length);
    this->m_datastore.Finalize();

    uint32_t pos = 0;

    if (Grunt::Command<Grunt::ClientLink>::Process(this->m_datastore, s_authCmds, sizeof(s_authCmds) / sizeof(s_authCmds[0]), *this, pos)) {
        uint32_t remaining = this->m_datastore.m_size - pos;
        this->m_datastore.m_read = pos;

        void* rest;
        this->m_datastore.GetDataInSitu(rest, remaining);
        this->m_datastore.m_read = -1;
        this->m_datastore.Reset();
        this->m_datastore.PutData(rest, remaining);
    } else {
        this->m_datastore.m_read = -1;
        this->m_datastore.Reset();
        this->Disconnect();
    }
}

// VERIFY: 8e4d36ef manual
// OFFSET: 0x008CE0F0
void Grunt::ClientLink::GetRealmList() {
    if (this->m_state != 6) {
        return;
    }

    CDataStoreCache<1024> msg;
    msg.Put(static_cast<uint8_t>(0x10));
    msg.Put(static_cast<uint32_t>(0));
    msg.Finalize();
    this->Send(msg);
}

// VERIFY: 58cb67ed manual
// OFFSET: 0x008CE720
void Grunt::ClientLink::ProveVersion(const uint8_t* versionChecksum) {
    CDataStoreCache<1024> msg;
    Grunt::CdKey cdKeys;
    SHA1_CONTEXT context;
    uint8_t hash[20];

    if (!this->m_clientResponse->GetCdKeys(cdKeys)) {
        cdKeys.m_count = 0;
    }

    if (this->m_state == 4) {
        msg.Put(static_cast<uint8_t>(1));
        msg.PutData(this->m_srpClient.m_publicKey, sizeof(this->m_srpClient.m_publicKey));
        msg.PutData(this->m_srpClient.m_clientProof, sizeof(this->m_srpClient.m_clientProof));

        SHA1_Init(&context);
        SHA1_Update(&context, this->m_srpClient.m_publicKey, sizeof(this->m_srpClient.m_publicKey));
        SHA1_Update(&context, versionChecksum, 20);
        SHA1_Final(hash, &context);
        msg.PutData(hash, sizeof(hash));

        msg.Put(static_cast<uint8_t>(cdKeys.m_count));

        for (uint32_t i = 0; i < static_cast<uint8_t>(cdKeys.m_count); i++) {
            Grunt::CdKey::Info& key = cdKeys.m_keys.m_data[i];
            msg.Put(key.m_keyLength);
            msg.Put(key.m_productId);
            msg.PutData(key.m_publicValue, sizeof(key.m_publicValue));

            SHA1_Init(&context);
            SHA1_Update(&context, this->m_srpClient.m_publicKey, sizeof(this->m_srpClient.m_publicKey));
            SHA1_Update(&context, this->m_serverChallenge, sizeof(this->m_serverChallenge));
            SHA1_Update(&context, key.m_hash, sizeof(key.m_hash));
            SHA1_Final(hash, &context);
            msg.PutData(hash, sizeof(hash));
        }

        uint8_t securityFlags = 0;

        bool pinEnabled;
        uint8_t* pinSalt;
        uint8_t* pinHash;
        this->m_clientResponse->GetPinInfo(&pinEnabled, &pinSalt, &pinHash);

        if (pinEnabled) {
            securityFlags = 0x1;
        }

        bool matrixEnabled;
        const uint8_t* matrixProof;
        this->m_clientResponse->GetMatrixProof(&matrixEnabled, &matrixProof);

        if (matrixEnabled) {
            securityFlags |= 0x2;
        }

        char token[17] = {};
        bool tokenEnabled;
        this->m_clientResponse->GetTokenInfo(&tokenEnabled, token);
        uint8_t tokenLength = SStrLen(token);

        if (tokenEnabled) {
            securityFlags |= 0x4;
        }

        msg.Put(securityFlags);

        if (pinEnabled) {
            msg.PutData(pinSalt, 16);
            msg.PutData(pinHash, 20);
        }

        if (matrixEnabled) {
            msg.PutData(matrixProof, 20);
        }

        if (tokenEnabled) {
            msg.Put(tokenLength);
            msg.PutArray(reinterpret_cast<uint8_t*>(token), tokenLength);
        }
    } else {
        uint32_t length = SStrLen(this->m_accountName);

        uint8_t clientSalt[16];
        MD5_CTX md5;
        MD5Init(&md5);
        MD5Update(&md5, reinterpret_cast<uint8_t*>(this->m_accountName), length);
        OsSecureRandom(clientSalt, sizeof(clientSalt));
        MD5Update(&md5, clientSalt, sizeof(clientSalt));
        MD5Final(clientSalt, &md5);

        msg.Put(static_cast<uint8_t>(3));
        msg.PutData(clientSalt, sizeof(clientSalt));

        SHA1_Init(&context);
        SHA1_Update(&context, reinterpret_cast<uint8_t*>(this->m_accountName), length);
        SHA1_Update(&context, clientSalt, sizeof(clientSalt));
        SHA1_Update(&context, this->m_serverChallenge, 16);
        SHA1_Update(&context, this->m_reconnectKey, sizeof(this->m_reconnectKey));
        SHA1_Final(hash, &context);
        msg.PutData(hash, sizeof(hash));

        SHA1_Init(&context);
        SHA1_Update(&context, clientSalt, sizeof(clientSalt));
        SHA1_Update(&context, versionChecksum, 20);
        SHA1_Final(hash, &context);
        msg.PutData(hash, sizeof(hash));

        msg.Put(static_cast<uint8_t>(cdKeys.m_count));

        for (uint32_t i = 0; i < static_cast<uint8_t>(cdKeys.m_count); i++) {
            Grunt::CdKey::Info& key = cdKeys.m_keys.m_data[i];
            msg.Put(key.m_keyLength);
            msg.Put(key.m_productId);
            msg.PutData(key.m_publicValue, sizeof(key.m_publicValue));

            SHA1_Init(&context);
            SHA1_Update(&context, clientSalt, sizeof(clientSalt));
            SHA1_Update(&context, this->m_serverChallenge, 16);
            SHA1_Update(&context, key.m_hash, sizeof(key.m_hash));
            SHA1_Final(hash, &context);
            msg.PutData(hash, sizeof(hash));
        }
    }

    msg.Finalize();
    this->Send(msg);
}

// VERIFY: 53c21c45 manual
// OFFSET: 0x008CC980
int32_t Grunt::ClientLink::CmdAuthReconnectChallenge(CDataStore& msg) {
    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 17) {
        return 0;
    }

    uint8_t result;
    msg.Get(result);

    if (result) {
        return 1;
    }

    void* challenge;
    msg.GetDataInSitu(challenge, 16);

    void* versionChallenge;
    msg.GetDataInSitu(versionChallenge, 16);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    memcpy(this->m_serverChallenge, challenge, 16);
    this->SetState(5);
    this->m_clientResponse->GetVersionProof(static_cast<const uint8_t*>(versionChallenge));
    return 2;
}

// VERIFY: 39d99ecd auto
// OFFSET: 0x008CCA50
int32_t Grunt::ClientLink::CmdAuthReconnectProof(CDataStore& msg) {
    if (msg.m_read >= msg.m_size) {
        return 0;
    }

    uint8_t result;
    msg.Get(result);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    if (result) {
        this->SetState(2);

        if (result >= 33) {
            result = Grunt::GRUNT_RESULT_DISCONNECTED;
        }

        this->m_clientResponse->ReconnectResult(static_cast<Grunt::Result>(result), nullptr, 0, 0);
        return 2;
    }

    if (msg.m_size - msg.m_read < 2) {
        return 0;
    }

    uint16_t flags = 0;
    msg.Get(flags);
    this->SetState(6);
    this->m_clientResponse->ReconnectResult(Grunt::GRUNT_RESULT_SUCCESS, this->m_reconnectKey, sizeof(this->m_reconnectKey), flags);
    return 2;
}

// VERIFY: 3cf29e43 manual
// OFFSET: 0x008CCB40
int32_t Grunt::ClientLink::CmdXferInitiate(CDataStore& msg) {
    if (msg.m_read >= msg.m_size) {
        return 0;
    }

    uint8_t length;
    msg.Get(length);

    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < static_cast<uint32_t>(length + 24)) {
        return 0;
    }

    if (length >= 0xFE) {
        length = 0xFE;
    }

    char fileName[256];
    msg.GetArray(reinterpret_cast<uint8_t*>(fileName), length);
    fileName[length] = '\0';

    uint64_t size;
    msg.Get(size);

    void* md5;
    msg.GetDataInSitu(md5, 16);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    this->m_clientResponse->GetFileStatus(fileName, size, static_cast<const uint8_t*>(md5));
    return 2;
}

// VERIFY: 296d35b8 auto
// OFFSET: 0x008CCC20
int32_t Grunt::ClientLink::CmdXferData(CDataStore& msg) {
    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 2) {
        return 0;
    }

    uint16_t size;
    msg.Get(size);

    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < size) {
        return 0;
    }

    void* data;
    msg.GetDataInSitu(data, size);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    this->m_clientResponse->FileData(static_cast<const uint8_t*>(data), size);
    return 2;
}

// VERIFY: e707dad6 manual
// OFFSET: 0x008CDE60
void Grunt::ClientLink::FileStatus(bool accept, uint64_t offset) {
    CDataStoreCache<1024> msg;

    if (accept) {
        if (offset) {
            msg.Put(static_cast<uint8_t>(0x33));
            msg.Put(offset);
        } else {
            msg.Put(static_cast<uint8_t>(0x32));
        }
    } else {
        msg.Put(static_cast<uint8_t>(0x34));
    }

    msg.Finalize();
    this->Send(msg);
}

// VERIFY: 37b6a6bc manual
// OFFSET: 0x008CDFC0
void Grunt::ClientLink::FileComplete() {
    CDataStoreCache<1024> msg;
    msg.Put(static_cast<uint8_t>(0x34));
    msg.Finalize();
    this->Send(msg);
}
