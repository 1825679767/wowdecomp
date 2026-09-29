#include "CKBSystem.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00BEA7FC
CDynamicString s_motd;

// STATIC: 0x00BEA808
static CDynamicString s_unk_BEA808;

// STATIC: 0x00BEA814
static CDynamicString s_unk_BEA814;

// VERIFY: 029a4910 auto
// OFFSET: 0x00551660
static int32_t Packet_SMSG_MOTD(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (msgId != SMSG_MOTD) {
        return 0;
    }

    if (!msg) {
        return 0;
    }

    uint32_t lineCount;
    msg->Get(lineCount);

    s_motd.Free();

    for (int32_t i = 0; i < static_cast<int32_t>(lineCount); i++) {
        if (i > 0) {
            s_motd += '\n';
        }

        char line[100];
        msg->GetString(line, sizeof(line));
        CGChat::AddChatMessage(line, 0, 0, 0, 0, 0, "MOTD", WOWGUID(), 0, WOWGUID(), 0, 0, 0);
        s_motd += line;
    }

    FrameScript_SignalEvent(492, nullptr);

    return 1;
}

// VERIFY: ac7db77e auto
// OFFSET: 0x00552380
void CKBSystem::Initialize() {
    ClientServices::SetMessageHandler(SMSG_MOTD, Packet_SMSG_MOTD, nullptr);
}

// VERIFY: e4cff749 auto
// OFFSET: 0x005515F0
void CKBSystem::Shutdown() {
    s_motd.Free();
    s_unk_BEA808.Free();
    s_unk_BEA814.Free();
    ClientServices::ClearMessageHandler(SMSG_MOTD);
}

// VERIFY: 15855df3 manual
// OFFSET: 0x00551620
void CKBSystem::CacheServerMessage(const char* text, int32_t id) {
    if (id == 3) {
        s_unk_BEA814 = text;
    } else {
        s_unk_BEA808 = text;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_SERVER_MESSAGE, nullptr);
}
