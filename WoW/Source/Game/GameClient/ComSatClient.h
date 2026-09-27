#ifndef WOW_SOURCE_GAME_GAMECLIENT_COMSATCLIENT_H
#define WOW_SOURCE_GAME_GAMECLIENT_COMSATCLIENT_H

#include <cstddef>
#include <cstdint>

struct CVar;

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

#endif
