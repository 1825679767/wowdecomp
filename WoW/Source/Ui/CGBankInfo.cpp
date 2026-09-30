#include "CGBankInfo.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00BEB9A0
WOWGUID CGBankInfo::m_banker;

// VERIFY: 2510a390 auto
// OFFSET: 0x0057B8B0
static int32_t BankUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYERBANKBAGSLOTS_CHANGED, nullptr);
    return 1;
}

// VERIFY: e46cf018 auto
// OFFSET: 0x0057BA60
void CGBankInfo::EnterWorld() {
    ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, 0x1A, 1, BankUpdateHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
}
