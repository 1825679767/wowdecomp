#ifndef WOW_SOURCE_GLUE_CGLUEMGR_H
#define WOW_SOURCE_GLUE_CGLUEMGR_H

#include <cstdint>
#include "Engine/Source/Os/OsURLDownload.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Glue/Types.h"
#include "WoW/Source/Net/Types.h"

class CDataStore;
class CHARACTER_INFO;
class CSimpleTop;
class SThread;

namespace FFX {
    class Effect;
}
struct MipBits;
class CSizeEvent;
class FileDownload;
struct CVar;

struct ConsoleString {
    /* 0000 */ TSLink<ConsoleString> m_link;
    /* 0008 */ char m_text[256];
};

struct LoginErrorURL {
    /* 0000 */ int32_t m_errorCode;
    /* 0004 */ char m_url[32];
};

class CGlueMgr {
    public:
    struct Agreements {
        /* 0000 */ TSGrowableArray<char> m_data;
        /* 0010 */ int32_t m_complete;
    };

    static char m_serverAlert[2048];
    static uint32_t m_queueTime[8];
    static int32_t m_queuePosition[8];
    static MipBits* m_cursorMipBits;
    static char m_currentScreen[64];
    static CSimpleTop* m_simpleTop;
    static int32_t m_reload;
    static int32_t m_initialized;
    static int32_t m_suspended;
    static int32_t m_disconnectPending;
    static int32_t m_reconnect;
    static GLUE_IDLE_STATE m_idleState;
    static uint32_t m_matrixChallengeCount;
    static char m_accountName[1280];
    static int32_t m_loginResult;
    static int32_t m_loginState;
    static uint32_t m_matrixRemaining;
    static int32_t m_estimatedWaitTime;
    static int32_t m_displayingQueueDialog;
    static CHARACTER_INFO* m_characterInfo;
    static int32_t m_rereadTOS;
    static int32_t m_acceptedTOS;
    static int32_t m_rereadEULA;
    static int32_t m_acceptedEULA;
    static int32_t m_rereadTerminationWithoutNotice;
    static int32_t m_acceptedTerminationWithoutNotice;
    static int32_t m_rereadScanning;
    static int32_t m_acceptedScanning;
    static int32_t m_rereadContest;
    static int32_t m_acceptedContest;
    static uint8_t m_authenticated;
    static uint8_t s_scriptFunctionsLoaded;
    static int32_t m_screenWidth;
    static int32_t m_screenHeight;
    static float m_aspect;
    static int32_t m_pendingServerAlert;
    static int32_t m_processServerAlert;
    static uint32_t m_pendingTimerAlert;
    static int32_t m_accountMsgAvailable;
    static uint32_t m_numChangedOptionWarnings;
    static int32_t m_showedDisconnect;
    static Agreements m_latestAgreements;
    static TSGrowableArray<const char*> m_changedOptionWarnings;
    static uint32_t m_surveyTimer;
    static uint8_t m_executedSurvey;
    static FileDownload* m_surveyDownload;
    static FileDownload* m_patchDownload;
    static int32_t m_ffxActive;
    static FFX::Effect* m_deathEffect;
    static FFX::Effect* m_glowEffect;
    static char m_scandllHackName[512];
    static uint8_t m_scandllOkayToLogIn;
    static const char* m_scandllResult;
    static uint8_t m_scandllShouldBlockPlaying;
    static volatile long m_scandllStatus;
    static SThread m_scandllThread;

    static void AcceptContest();
    static void AcceptEULA();
    static void AcceptScanning();
    static void AcceptTerminationWithoutNotice();
    static void AcceptTOS();
    static void AddChangedOptionWarning(const char* warning);
    static bool AgreementsURLCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error);
    static void DisplayLoginStatus();
    static void GetCharacterList();
    static void GetRealmList(int32_t showDialog);
    static int32_t HandleBattlenetDisconnect();
    static void HandleDisplaySizeChanged(const CSizeEvent& event);
    static int32_t Idle(const void* data, void* param);
    static void Initialize();
    static void InitializeFFX();
    static bool IsScanDLLFinished();
    static int32_t OnCharCustomizeResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnCharDeclineResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnCharFactionChangeResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnCharRenameResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnKickReasonMsg(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnRealmSplitMsg(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static void PatchDownloadIdle();
    static void ProcessAgreements(CGlueMgr::Agreements* agreements);
    static void PatchDownloadStart();
    static void Resume();
    static void Shutdown();
    static void ScanDLLContinueAnyway();
    static void ScanDLLFinished();
    static void ScanDLLStart(const char* versionURL, const char* dllURL);
    static bool ServerAlertURLCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error);
    static void SetScreen(const char* screen);
    static void SetUIDepth(float depth);
    static int32_t ShowChangedOptionWarnings();
    static void Suspend();
    static void SurveyDownloadEnd();
    static void SurveyDownloadIdle();
    static void SurveyDownloadStart();
    static bool SurveyExecute();
    static void SurveySendResults();
    static void UpdateCurrentScreen(const char* screen);
};

extern const uint8_t InterfaceKey[256];

#endif
