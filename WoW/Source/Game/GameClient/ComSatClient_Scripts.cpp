#include "ComSatClient.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AF29C0
static FrameScript_Method s_ScriptFunctions[] = {
    { "VoiceEnumerateOutputDevices", &Script_VoiceEnumerateOutputDevices },
    { "VoiceEnumerateCaptureDevices", &Script_VoiceEnumerateCaptureDevices },
    { "VoiceSelectOutputDevice", &Script_VoiceSelectOutputDevice },
    { "VoiceSelectCaptureDevice", &Script_VoiceSelectCaptureDevice },
    { "VoiceGetCurrentOutputDevice", &Script_VoiceGetCurrentOutputDevice },
    { "VoiceGetCurrentCaptureDevice", &Script_VoiceGetCurrentCaptureDevice },
    { "GetVoiceStatus", &Script_GetVoiceStatus },
    { "GetNumVoiceSessions", &Script_GetNumVoiceSessions },
    { "GetVoiceSessionInfo", &Script_GetVoiceSessionInfo },
    { "GetVoiceCurrentSessionID", &Script_GetVoiceCurrentSessionID },
    { "SetActiveVoiceChannelBySessionID", &Script_SetActiveVoiceChannelBySessionID },
    { "GetNumVoiceSessionMembersBySessionID", &Script_GetNumVoiceSessionMembersBySessionID },
    { "GetVoiceSessionMemberInfoBySessionID", &Script_GetVoiceSessionMemberInfoBySessionID },
    { "VoiceIsDisabledByClient", &Script_VoiceIsDisabledByClient },
    { "UnitIsTalking", &Script_UnitIsTalking },
};

// VERIFY: e606eb4c auto
// OFFSET: 0x007DB9B0
void VoiceRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
