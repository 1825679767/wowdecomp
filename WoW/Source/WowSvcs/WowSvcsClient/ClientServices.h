#ifndef WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_CLIENTSERVICES_H
#define WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_CLIENTSERVICES_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/WowServices/Types.h"
#include "_Triage/Types.h"

struct CVar;
class Login;
class LoginResponse;

struct RealmInfo {
    /* 0000 */ uint32_t m_id;
    /* 0004 */ uint8_t m_type;
    /* 0005 */ uint8_t m_flags;
    /* 0006 */ char m_realmName [256];
    /* 0106 */ char m_realmAddress [32];
    /* 0126 */ uint8_t unk_0126[2];
    /* 0128 */ float m_population;
    /* 012C */ uint8_t m_numCharacters;
    /* 012D */ uint8_t unk_012D;
    /* 012E */ uint8_t unk_012E;
    /* 012F */ uint8_t unk_012F;
    /* 0130 */ uint32_t unk_0130 [4];
    /* 0140 */ uint8_t m_otherFlags;
    /* 0141 */ uint8_t m_versionMajor;
    /* 0142 */ uint8_t m_versionMinor;
    /* 0143 */ uint8_t m_versionRevision;
    /* 0144 */ uint16_t m_versionBuild;
    /* 0146 */ uint8_t unk_0146[2];
};

struct CLIENTCDKEY {
    /* 0000 */ uint16_t m_length;
    /* 0002 */ uint8_t unk_0002[2];
    /* 0004 */ uint8_t m_key[28];
};

class ClientConnection : public RealmConnection {
    public:
    /* 2F3C */ uint32_t unk_2F3C;
    /* 2F40 */ uint32_t unk_2F40;
    /* 2F44 */ uint32_t unk_2F44;
    /* 2F48 */ uint32_t unk_2F48;
    /* 2F4C */ uint32_t unk_2F4C;
    /* 2F50 */ uint32_t unk_2F50;
    /* 2F54 */ int32_t m_inGame;
    /* 2F58 */ uint8_t unk_2F58;
    /* 2F59 */ uint8_t m_loggingOut;
    /* 2F5A */ uint8_t unk_2F5A;
    /* 2F5B */ uint8_t unk_2F5B[1];
    /* 2F5C */ uint32_t unk_2F5C;

    ~ClientConnection() override;
    void HandleConnect() override;
    void HandleDisconnect() override;
    void HandleCantConnect() override;
    void HandleCharacterCreate(uint8_t result) override;
    void HandleCharacterDelete(uint8_t result) override;
    void HandleCharacterLoginFailed(uint8_t result) override;
    void HandleLogoutComplete() override;
    void HandleLogoutResponse(int32_t result, int32_t instant) override;
    void HandleLogoutAbortAck() override;

    ClientConnection(RealmResponse* realmResponse);
    void AccountLogin(const char* accountName, const char* password, int32_t region, int32_t locale);
    void Cancel(int32_t errorCode);
    void CharacterLogin(uint64_t guid, int32_t a2);
    void CharacterLogout(bool a1, bool a2);
    void Connect();
    void Disconnect();
    int32_t PollStatus(WOWCS_OPS& op, const char** text, int32_t& result, int32_t& errorCode);
};

class ClientRealmResponseAdapter : public RealmResponse {
    public:
    void LoginServerResult(LOGIN_STATE state, LOGIN_RESULT result, const char* message1, const char* message2, const char* message3) override;
    void RealmListReceived(TSFixedArray<RealmInfo>& realms) override;
    void HandleAuthResponse(RealmConnection* connection, uint8_t result) override;
    void AddOnListReceived(RealmConnection* connection) override;
    void CharacterListReceived(RealmConnection* connection, const TSFixedArray<CHARACTER_INFO>& characters, int32_t success) override;
    void GameServerResult(RealmConnection* connection, const char* message1, const char* message2, const char* message3) override;
    void MessageReceived(RealmConnection* connection, uint32_t message) override;
    void OnChatReceived(RealmConnection* connection, uint64_t guid, const char* text, uint8_t type) override;
    void OnAccountOnline(RealmConnection* connection, int32_t a2, uint64_t guid) override;
    void OnAFKMonitorInfoResponse(RealmConnection* connection, uint64_t guid, int32_t a3, int32_t a4, int32_t a5, int32_t a6) override;
    void HandleConnect(RealmConnection* connection) override;
    void HandleDisconnect(RealmConnection* connection) override;
    void HandleCantConnect(RealmConnection* connection) override;
};

class ClientServices {
    public:
    static LoginResponse* m_instance;
    static RealmResponse* s_clientRealmResponse;
    static Login* s_loginObj;
    static CVar* s_serverAlertVar;
    static CVar* s_realmNameVar;
    static RealmInfo m_selectRealmInfo;
    static bool m_selectRealmInfoValid;
    static ClientConnection* s_currentConnection;

    static void ClearMessageHandler(NETMESSAGE msgId);
    static void Send2(CDataStore* msg);
    static ClientConnection* Connection();
    static void Destroy();
    static LoginResponse* CreateLoginResponse();
    static const char* GetSelectedRealmName();
    static const char* GetServerAlertURL();
    static Login* LoginConnection();
    static RealmInfo* GetSelectedRealm();
    static void Initialize();
    static void SetMessageHandler(NETMESSAGE msgId, MESSAGE_HANDLER handler, void* param);
    static void InitLoginServerCVars(int32_t overwrite, const char* locale);
    static void LoadCDKey();
};

extern ClientConnection* g_clientConnection;

int32_t ConsoleCommand_Logout(const char* command, const char* arguments);

#endif
