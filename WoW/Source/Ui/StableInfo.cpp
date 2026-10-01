#include "StableInfo.h"
#include <cstring>
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00C0F480
StablePetInfo CGStableInfo::m_pets[6];

// STATIC: 0x00C0F6C0
WOWGUID CGStableInfo::m_stableMaster;

// STATIC: 0x00C0F6C8
int32_t CGStableInfo::unk_C0F6C8;

// STATIC: 0x00C0F6CC
int32_t CGStableInfo::unk_C0F6CC;

// STATIC: 0x00C0F6D0
int32_t CGStableInfo::unk_C0F6D0;

// VERIFY: da4c52c5 auto
// OFFSET: 0x005A1ED0
void CGStableInfo::InitializeGame() {
    ClientServices::SetMessageHandler(MSG_LIST_STABLED_PETS, Packet_MSG_LIST_STABLED_PETS, nullptr);
    ClientServices::SetMessageHandler(SMSG_STABLE_RESULT, Packet_SMSG_STABLE_RESULT, nullptr);
}

// VERIFY: 83895cc1 auto
// OFFSET: 0x005A0F10
void CGStableInfo::CloseStables(bool force) {
    if (!CGStableInfo::m_stableMaster && !force) {
        return;
    }

    CGGameUI::ClearInteractTarget(CGStableInfo::m_stableMaster, 0);
    CGStableInfo::m_stableMaster = WOWGUID();
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_STABLE_CLOSED, nullptr);
}

// VERIFY: 8f51bf00 auto
// OFFSET: 0x005A0EC0
void CGStableInfo::EnterWorld() {
    CGStableInfo::m_stableMaster = WOWGUID{ 0 };
    CGStableInfo::unk_C0F6C8 = 0;
    CGStableInfo::unk_C0F6CC = 0;
    CGStableInfo::unk_C0F6D0 = 0;
    memset(&CGStableInfo::m_pets[0], 0, sizeof(CGStableInfo::m_pets[0]));
}
