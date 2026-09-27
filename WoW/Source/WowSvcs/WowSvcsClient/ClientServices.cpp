#include <cstdint>
#include <cstring>
#include "ClientServices.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Game/GameClient/WardenClient.h"
#include "WoW/Source/WowServices/Login.h"
#include "_Triage/LoginResponse.h"

// STATIC: 0x00C79B98
RealmInfo ClientServices::m_selectRealmInfo;

// STATIC: 0x00C79CE0
ClientConnection* g_clientConnection;

// STATIC: 0x00C79CE4
LoginResponse* ClientServices::m_instance;

// STATIC: 0x00C79CE9
bool ClientServices::m_selectRealmInfoValid;

// STATIC: 0x00C79CEC
Login* ClientServices::s_loginObj;

// STATIC: 0x00C79CF0
RealmResponse* ClientServices::s_clientRealmResponse;

// STATIC: 0x00C79CF4
ClientConnection* ClientServices::s_currentConnection;

// STATIC: 0x00C79B20
static CLIENTCDKEY s_cdKey;

// STATIC: 0x00C79CEA
static bool s_cdKeyLoaded;

// VERIFY: 5ca752ca manual
// OFFSET: 0x006B0250
ClientConnection::ClientConnection(RealmResponse* realmResponse) : RealmConnection(realmResponse) {
    this->unk_2F3C = 0;
    this->unk_2F40 = 0;
    this->m_inGame = 0;
    this->unk_2F44 = 1;
    this->unk_2F48 = 1;
    this->unk_2F4C = 0;
    this->unk_2F50 = 0;
    this->unk_2F5C = 0;
    this->unk_2F58 = 0;
    this->m_loggingOut = 0;
    this->unk_2F5A = 0;
}

// VERIFY: b1444fa0 auto
// OFFSET: 0x006B0190
void ClientServices::LoadCDKey() {
    memset(&s_cdKey, 0, sizeof(s_cdKey));

    SArchive* archive;

    if (!SFile::OpenArchive("Data\\base.MPQ", 100, 0x800, &archive) || !archive) {
        return;
    }

    void* data;
    uint32_t size = sizeof(s_cdKey.m_key);

    if (SFile::ReadFile(archive, "telemetry.dat", &data, &size, 0, 0, nullptr) && data) {
        if (size == sizeof(s_cdKey.m_key)) {
            s_cdKeyLoaded = true;
            s_cdKey.m_length = 20;
            memcpy(s_cdKey.m_key, data, sizeof(s_cdKey.m_key));
        }

        SFile::Unload(data);
    }

    SFile::CloseArchive(archive);
}

// VERIFY: ddc9b4a6 manual
// OFFSET: 0x006B0970
ClientConnection* ClientServices::Connection() {
    if (!ClientServices::s_currentConnection) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    return ClientServices::s_currentConnection;
}

// VERIFY: d41ee95d auto
// OFFSET: 0x006B0990
Login* ClientServices::LoginConnection() {
    return ClientServices::s_loginObj;
}

// VERIFY: 409cba3b auto
// OFFSET: 0x006B0B50
void ClientServices::Send2(CDataStore* msg) {
    if (!ClientServices::s_currentConnection) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    ClientServices::s_currentConnection->Send(msg);
}

// VERIFY: 8de1eeef auto
// OFFSET: 0x006B0B80
void ClientServices::SetMessageHandler(NETMESSAGE msgId, MESSAGE_HANDLER handler, void* param) {
    if (!handler) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (!ClientServices::s_currentConnection) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    ClientServices::s_currentConnection->SetMessageHandler(msgId, handler, param);
}

// VERIFY: 71af073c auto
// OFFSET: 0x006B0BC0
void ClientServices::ClearMessageHandler(NETMESSAGE msgId) {
    if (!ClientServices::s_currentConnection) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    ClientServices::s_currentConnection->ResetHandler(msgId);
}

// VERIFY: 40900e3f auto
// OFFSET: 0x006B0DC0
const char* ClientServices::GetSelectedRealmName() {
    if (!ClientServices::s_realmNameVar) {
        ClientServices::s_realmNameVar = CVar::Register("realmName", "Last realm connected to", 0, "", nullptr, CATEGORY_NET, false, nullptr, false);
    }

    return ClientServices::s_realmNameVar->m_stringValue.m_str;
}

// VERIFY: efa85be0 auto
// OFFSET: 0x006B0DF0
const char* ClientServices::GetServerAlertURL() {
    if (!ClientServices::s_serverAlertVar) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    return ClientServices::s_serverAlertVar->m_stringValue.m_str;
}

// VERIFY: 94d8af94 auto
// OFFSET: 0x006B0ED0
RealmInfo* ClientServices::GetSelectedRealm() {
    return ClientServices::m_selectRealmInfoValid ? &ClientServices::m_selectRealmInfo : nullptr;
}

// VERIFY: 5b8bbe2b manual
// OFFSET: 0x006B1BB0
void ClientServices::Destroy() {
    ClientServices::m_selectRealmInfoValid = false;

    if (ClientServices::s_loginObj) {
        ClientServices::s_loginObj->Logoff();
    }

    ConsoleCommandUnregister("logout");

    if (g_clientConnection) {
        if (g_clientConnection->unk_2F3C) {
            g_clientConnection->NetClient::Disconnect();
            g_clientConnection->unk_2F3C = 0;
            WardenClient_Destroy();
        }

        delete g_clientConnection;
        g_clientConnection = nullptr;
    }

    if (ClientServices::s_loginObj) {
        delete ClientServices::s_loginObj;
        ClientServices::s_loginObj = nullptr;
    }

    if (ClientServices::s_clientRealmResponse) {
        delete ClientServices::s_clientRealmResponse;
        ClientServices::s_clientRealmResponse = nullptr;
    }

    if (ClientServices::m_instance) {
        delete ClientServices::m_instance;
        ClientServices::m_instance = nullptr;
    }
}

// VERIFY: 0e65977c manual
// OFFSET: 0x006B2200
void ClientServices::Initialize() {
    if (!g_clientConnection) {
        ClientServices::s_clientRealmResponse = new (__FILE__, __LINE__) ClientRealmResponseAdapter();
        g_clientConnection = new (__FILE__, __LINE__) ClientConnection(ClientServices::s_clientRealmResponse);

        if (!g_clientConnection) {
            SErrDisplayAppFatalMessage("", "", 0);
        }
    }

    ClientServices::s_currentConnection = g_clientConnection;
    ConsoleCommandRegister("logout", ConsoleCommand_Logout, CATEGORY_DEFAULT, nullptr);
}
