#include "ChatFrame.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"

// STATIC: 0x00BCEFEC
int32_t CGChat::m_afk;

// STATIC: 0x00BCF004
int32_t CGChat::m_voiceChatSystemStatus;

// VERIFY: 36ed0e4f auto
// OFFSET: 0x004FBAF0
int32_t CGChat::IsVoiceChatAllowed() {
    return CGChat::m_voiceChatSystemStatus && !ComSatClient_IsComsatDisabledByClient();
}
