#ifndef WOW_SOURCE_GAME_GAMECLIENT_COMSATCLIENT_H
#define WOW_SOURCE_GAME_GAMECLIENT_COMSATCLIENT_H

#include <cstddef>
#include <cstdint>

struct CVar;

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

struct COMSATDATA {
    /* 0000 */ uint64_t m_sessionID;
    /* 0008 */ uint8_t unk_0008[776];
};

extern COMSATDATA s_comsatData[32];
extern CVar* s_cvarEnableMicrophone;
extern CVar* s_cvarEnableVoiceChat;
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

#endif
