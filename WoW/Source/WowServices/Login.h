#ifndef WOW_SOURCE_WOWSERVICES_LOGIN_H
#define WOW_SOURCE_WOWSERVICES_LOGIN_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

class CDataStore;
struct LoginData;
class LoginResponse;
struct NETADDR;

namespace Grunt {
struct CdKey;
}

class Login {
    public:
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint8_t m_ukn121;
    /* 000D */ char m_username [1280];
    /* 050D */ char m_rawUsername [1280];
    /* 0A0D */ char m_uknStr2 [1280];
    /* 0F0D */ uint8_t unk_0F0D[3];
    /* 0F10 */ char* m_password;
    /* 0F14 */ uint32_t unk_0F14 [11];
    /* 0F40 */ uint32_t m_fileSizeLow;
    /* 0F44 */ uint32_t m_fileSizeHigh;
    /* 0F48 */ uint8_t m_fileMD5 [16];
    /* 0F58 */ char m_fileName [260];
    /* 105C */ uint32_t unk_105C [16];
    /* 109C */ uint8_t unk_109C;
    /* 109D */ uint8_t m_matrixWidth;
    /* 109E */ uint8_t m_matrixHeight;
    /* 109F */ uint8_t m_matrixMinDigitCount;
    /* 10A0 */ uint8_t m_matrixMaxDigitCount;
    /* 10A1 */ uint8_t m_matrixChallengeCount;
    /* 10A2 */ uint8_t m_matrixFlipCoords;
    /* 10A3 */ uint8_t unk_10A3;
    /* 10A4 */ uint32_t m_loginUkn9;
    /* 10A8 */ uint32_t m_loginUkn10;
    /* 10AC */ uint32_t m_loginUkn11;
    /* 10B0 */ uint32_t m_loginUkn12;
    /* 10B4 */ uint32_t m_loginUkn13;
    /* 10B8 */ uint32_t m_loginUkn14;
    /* 10BC */ uint32_t m_loginUkn15;
    /* 10C0 */ uint32_t m_loginUkn16;
    /* 10C4 */ uint32_t m_loginUkn17;
    /* 10C8 */ uint32_t m_loginUkn18;
    /* 10CC */ uint32_t m_loginUkn19;
    /* 10D0 */ uint32_t m_loginUkn20;
    /* 10D4 */ uint8_t m_tokenRequired;
    /* 10D5 */ uint8_t unk_10D5;
    /* 10D6 */ uint8_t unk_10D6;
    /* 10D7 */ uint8_t unk_10D7;
    /* 10D8 */ uint32_t m_loginUkn2;
    /* 10DC */ uint32_t m_loginUkn3;
    /* 10E0 */ uint32_t m_loginUkn4;
    /* 10E4 */ uint32_t m_loginUkn5;
    /* 10E8 */ uint32_t m_loginUkn6;
    /* 10EC */ LoginResponse* m_loginResponse;

    virtual void Connected(const NETADDR& address);
    virtual void Disconnected();
    virtual void CantConnect();
    virtual void OnlineIdle();
    virtual int32_t GetLogonMethod();
    virtual void GetVersionProof(const uint8_t* versionChallenge);
    virtual void SetPinInfo(bool enabled, uint32_t seed, const uint8_t* salt);
    virtual void GetPinInfo(bool* enabled, uint8_t** salt, uint8_t** hash);
    virtual void SetMatrixInfo(bool enabled, uint8_t width, uint8_t height, uint8_t minDigits, uint8_t maxDigits, bool flipCoords, uint8_t challengeCount, uint64_t seed, const uint8_t* sessionKey, uint32_t sessionKeyLength);
    virtual void GetMatrixProof(bool* enabled, const uint8_t** proof) const;
    virtual void SetTokenInfo(bool enabled, uint8_t required);
    virtual void GetTokenInfo(bool* enabled, char* token);
    virtual void GetCdKeys(Grunt::CdKey& cdKey);
    virtual void GetFileStatus(const char* fileName, uint64_t size, const uint8_t* md5);
    virtual void LogonResult(int32_t result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags);
    virtual void ReconnectResult(int32_t result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags);
    virtual void FileData(const uint8_t* data, uint32_t size);
    virtual void RealmListResult(CDataStore* msg);
    virtual void NextSecurityState(int32_t state);
    virtual int32_t GetServerId();
    virtual const uint8_t* GetVersionChallenge();
    virtual void GetRealmList();
    virtual void Reconnect();
    virtual void Logon(const char* accountName, const char* password);
    virtual int32_t Wait(const char* message, LoginData& loginData);
    virtual void ProveVersion(const uint8_t* versionChecksum);
    virtual void FileAccept(bool accept, uint64_t offset);
    virtual void FileComplete(bool success);
    virtual int32_t GetSurveyId();
    virtual void SendSurveyData(uint16_t size, uint8_t* data);
    virtual void SendSurveyFailed();
    virtual int32_t IsReconnect();
    virtual int32_t IsLoggedOn();
    virtual void Logoff();
    virtual const uint8_t* GetKeyMap();
    virtual void EnterPIN(int32_t count, uint8_t* digits);
    virtual void EnterMatrix(uint8_t value);
    virtual void RevertMatrix();
    virtual void CommitMatrix();
    virtual void FinalizeMatrix();
    virtual bool GetMatrixInfo(uint32_t* width, uint32_t* height, uint32_t* minDigits, uint32_t* maxDigits, bool* flipCoords, uint32_t* challengeCount);
    virtual void GetMatrixCoordinates(uint32_t index, uint32_t* x, uint32_t* y);
    virtual void EnterToken(const char* token);
    virtual bool IsTrialAccount();
    virtual bool IsGMAccount();
    virtual bool IsProPassAccount();
    virtual void Update();
    virtual void Init(LoginResponse* loginResponse);
    virtual ~Login();
    virtual void JoinRealm(uint32_t a1, uint32_t a2, uint32_t a3);
    virtual int32_t GetLoginServerType();
};

#endif
