#ifndef WOW_SOURCE_GAME_GAMECLIENT_COMSATCLIENT_H
#define WOW_SOURCE_GAME_GAMECLIENT_COMSATCLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/ComSat/ComSatInterface.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

struct CVar;
struct lua_State;

struct ComSatEvent {
    /* 0000 */ uint32_t m_type;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint64_t m_guid;
    /* 0010 */ union {
        uint32_t m_uintValue;
        float m_floatValue;
    };
    /* 0014 */ uint32_t unk_0014;
};

struct ComSatCallback {
    /* 0000 */ uint32_t m_type;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint64_t unk_0008;
    /* 0010 */ uint64_t unk_0010;
};

struct MUTEDPLAYER : public TSHashObject<MUTEDPLAYER, CHashKeyGUID> {
};

struct TALKINGPLAYER : public TSHashObject<TALKINGPLAYER, CHashKeyGUID> {
};

class ComSatClientAllocator : public ComSat::Allocator {
    public:
    void* Alloc(uint32_t size) override;
    void Free(void* ptr) override;
};

int32_t ComSatClient_HasSession(int32_t type, const char* name);
int32_t ComSatClient_GetCurrentSessionType();
const char* ComSatClient_GetCurrentSessionName();

namespace Comsat {
void VoicePlateStart(uint64_t sessionId, uint64_t guid);
void VoicePlateStop(uint64_t sessionId, uint64_t guid);
void VoiceStart();
void VoiceStop();
}

struct COMSATDATA {
    /* 0000 */ uint64_t m_sessionID;
    /* 0008 */ int32_t m_type;
    /* 000C */ char m_name[128];
    /* 008C */ uint32_t m_numMembers;
    /* 0090 */ uint8_t unk_0090[640];
};

extern COMSATDATA s_comsatData[32];
extern int32_t s_currentSession;
extern TSHashTable<TALKINGPLAYER, CHashKeyGUID> unk_D37D88;
extern CVar* s_cvarEnableMicrophone;
extern CVar* s_cvarEnableVoiceChat;
extern CVar* s_cvarPushToTalkButton;
extern int32_t s_pushToTalkKey;
extern uint32_t s_pushToTalkModifiers;
extern uint32_t s_pushToTalkMouseButton;
extern int32_t s_voiceActivation;
extern int32_t s_voiceChatWasEnabled;

void AddComSatEvent(uint32_t type, uint32_t value, uint64_t guid);
void AddComSatEvent(uint32_t type, float value, uint64_t guid);
void ComSatClient_SetCaptureDevice(int32_t index);
void ComSatClient_SetOutputDevice(int32_t index);
bool EnableMicrophoneHandler(CVar* var, const char* oldValue, const char* value, void* arg);
bool EnableVoiceChatHandler(CVar* var, const char* oldValue, const char* value, void* arg);
bool InboundChatVolumeHandler(CVar* var, const char* oldValue, const char* value, void* arg);
bool OutboundChatVolumeHandler(CVar* var, const char* oldValue, const char* value, void* arg);
bool PushToTalkButtonHandler(CVar* var, const char* oldValue, const char* value, void* arg);
bool SelfMuteHandler(CVar* var, const char* oldValue, const char* value, void* arg);
bool VoiceActivationSensitivityHandler(CVar* var, const char* oldValue, const char* value, void* arg);
bool VoiceChatModeHandler(CVar* var, const char* oldValue, const char* value, void* arg);

void ComSatClient_Init();
int32_t ComSatClient_IsComsatDisabledByClient();
void ComSatClient_Shutdown();
void UpdatePushToTalkKey();
int32_t VoiceChat_TickWrapper(const void* data, void* param);

int32_t Script_VoiceGetCurrentOutputDevice(lua_State* L);
int32_t Script_GetVoiceStatus(lua_State* L);
int32_t Script_SetActiveVoiceChannelBySessionID(lua_State* L);
int32_t Script_GetVoiceSessionMemberInfoBySessionID(lua_State* L);

extern int32_t s_comsatDisabledByClient;

class SCritSect;
extern SCritSect s_comsatDeviceLock;
void VoiceRegisterScriptFunctions();

void VoiceSessionSetTalkerMuted(WOWGUID guid, int32_t muted);

#endif
