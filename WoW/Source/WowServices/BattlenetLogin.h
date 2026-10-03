#ifndef WOW_SOURCE_WOWSERVICES_BATTLENETLOGIN_H
#define WOW_SOURCE_WOWSERVICES_BATTLENETLOGIN_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "WoW/Source/WowServices/Login.h"

class TumorManager;

extern TumorManager* g_tumorManager;

class BattlenetLogin : public Login {
    public:
    struct BN_EVENT : public TSLinkedNode<BattlenetLogin::BN_EVENT> {
    };

    struct BN_REALM : public TSLinkedNode<BattlenetLogin::BN_REALM> {
    };

    struct GameAccount {
        /* 0000 */ uint32_t m_region;
        /* 0004 */ char m_name[256];
    };

    struct PatchInstruction {
    };

    /* 10F0 */ uint32_t unk_10F0;
    /* 10F4 */ uint32_t unk_10F4;
    /* 10F8 */ uint32_t unk_10F8;
    /* 10FC */ uint32_t unk_10FC;
    /* 1100 */ uint32_t unk_1100;
    /* 1104 */ TSList<BattlenetLogin::BN_REALM> m_realms;
    /* 1110 */ uint8_t unk_1110;
    /* 1111 */ uint8_t unk_1111[3];
    /* 1114 */ uint32_t unk_1114;
    /* 1118 */ SCritSect m_realmLock;
    /* 1140 */ TSFixedArray<BattlenetLogin::GameAccount> m_gameAccounts;
    /* 114C */ uint32_t unk_114C;
    /* 1150 */ SCritSect m_gameAccountLock;
    /* 1178 */ uint32_t unk_1178;
    /* 117C */ TSFixedArray<BattlenetLogin::PatchInstruction> m_patchInstructions;
    /* 1188 */ uint32_t unk_1188;
    /* 118C */ SCritSect m_patchInstructionLock;
    /* 11B4 */ uint32_t unk_11B4;
    /* 11B8 */ uint32_t unk_11B8;
    /* 11BC */ uint32_t unk_11BC;
    /* 11C0 */ uint8_t unk_11C0;
    /* 11C1 */ uint8_t unk_11C1[3];
    /* 11C4 */ uint32_t unk_11C4;
    /* 11C8 */ TSList<BattlenetLogin::BN_EVENT> m_events [2];
    /* 11E0 */ SCritSect m_eventLock;
    /* 1208 */ uint32_t unk_1208;
    /* 120C */ uint32_t unk_120C;

    bool Connected(const NETADDR& address) override;
    bool Disconnected() override;
    bool CantConnect() override;
    void GetLogonMethod() override;
    void GetVersionProof(const uint8_t* versionChallenge) override;
    void SetPinInfo(bool enabled, uint32_t seed, const uint8_t* salt) override;
    void GetPinInfo(bool* enabled, uint8_t** salt, uint8_t** hash) override;
    void SetMatrixInfo(bool enabled, uint8_t width, uint8_t height, uint8_t minDigits, uint8_t maxDigits, bool flipCoords, uint8_t challengeCount, uint64_t seed, const uint8_t* sessionKey, uint32_t sessionKeyLength) override;
    void GetMatrixProof(bool* enabled, const uint8_t** proof) const override;
    void SetTokenInfo(bool enabled, uint8_t required) override;
    void GetTokenInfo(bool* enabled, char* token) override;
    void LogonResult(Grunt::Result result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags) override;
    void ReconnectResult(Grunt::Result result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags) override;
    LOGIN_STATE NextSecurityState(LOGIN_STATE state) override;
    int32_t GetServerId() override;
    const uint8_t* GetVersionChallenge() override;
    void GetRealmList() override;
    void Reconnect() override;
    void Logon(const char* loginServer, const char* portal) override;
    int32_t Wait(const char* message, LoginData& loginData) override;
    void ProveVersion(const uint8_t* versionChecksum) override;
    void FileAccept(bool accept, uint64_t offset) override;
    void FileComplete(bool success) override;
    int32_t GetSurveyId() override;
    void SendSurveyData(uint16_t size, uint8_t* data) override;
    void SendSurveyFailed() override;
    void Logoff() override;
    const uint8_t* GetKeyMap() override;
    void EnterPIN(int32_t count, uint8_t* digits) override;
    void EnterMatrix(uint8_t value) override;
    void RevertMatrix() override;
    void CommitMatrix() override;
    void FinalizeMatrix() override;
    bool GetMatrixInfo(uint32_t* width, uint32_t* height, uint32_t* minDigits, uint32_t* maxDigits, bool* flipCoords, uint32_t* challengeCount) override;
    bool GetMatrixCoordinates(uint32_t index, uint32_t* x, uint32_t* y) override;
    void EnterToken(const char* token) override;
    bool IsTrialAccount() override;
    bool IsGMAccount() override;
    bool IsProPassAccount() override;
    void Update() override;
    void Init(LoginResponse* loginResponse) override;
    ~BattlenetLogin() override;
    void JoinRealm(uint32_t a1, uint32_t a2, uint32_t a3) override;
    int32_t GetLoginServerType() override;

    BattlenetLogin();

    const char* GetGameAccountName(uint32_t index);
    uint32_t GetGameAccountRegion(uint32_t index);
    uint32_t GetNumGameAccounts();
    bool IsConnected();
    bool SetGameAccount(uint32_t index);
};

class BNetAuthPacketSub1 {
    public:
    /* 0000 */ uint32_t unk_0000;
};

class BattlenetApplication {
    public:
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ uint8_t unk_0008;
    /* 0009 */ uint8_t unk_0009;
    /* 000A */ uint8_t unk_000A;
    /* 000B */ uint8_t unk_000B;
    /* 000C */ uint8_t unk_000C;
    /* 000D */ uint8_t unk_000D;
    /* 000E */ uint8_t unk_000E[4];
    /* 0012 */ uint16_t unk_0012;
    /* 0014 */ uint16_t unk_0014;
    /* 0016 */ uint16_t unk_0016;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
};

#endif
