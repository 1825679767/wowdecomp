#include "StableInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00C0F6C0
WOWGUID CGStableInfo::m_stableMaster;

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
