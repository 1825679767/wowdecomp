#include "BarberShop.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00BD19B8
int32_t CGBarberShop::m_barberShopEnabled;

// STATIC: 0x00ACC788
float CGBarberShop::m_minCameraDistance = 1.0f;

// STATIC: 0x00BD19BA
uint8_t CGBarberShop::m_initialized;

// STATIC: 0x00BD19BB
uint8_t CGBarberShop::m_resetPortrait;

// STATIC: 0x00BD19B4
int32_t CGBarberShop::m_currentSkinColor;

// VERIFY: 28b6e499 auto
// OFFSET: 0x0052E480
float CGBarberShop::GetMinCameraDistance() {
    return CGBarberShop::m_minCameraDistance;
}

// VERIFY: 6b04b74f auto
// OFFSET: 0x0052E4D0
void CGBarberShop::AppearanceApplied() {
    Script_SendUniqueSignal(SCRIPT_EVENT_BARBER_SHOP_APPEARANCE_APPLIED);
    CGBarberShop::m_resetPortrait = 1;
}

// VERIFY: 1f780087 auto
// OFFSET: 0x0052E5B0
int32_t Packet_SMSG_BARBER_SHOP_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t result = 0xFFFFFFFF;
    msg->Get(result);

    switch (result) {
        case 0:
            FrameScript_SignalEvent(SCRIPT_EVENT_BARBER_SHOP_SUCCESS, nullptr);
            break;
        case 1:
        case 3:
            CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
            break;
        case 2:
            CGGameUI::DisplayError(ERR_NOT_BARBER_SITTING);
            break;
        default:
            break;
    }

    return 1;
}

// VERIFY: 1a363132 auto
// OFFSET: 0x0052FE60
void CGBarberShop::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_ENABLE_BARBER_SHOP, Packet_SMSG_ENABLE_BARBER_SHOP, nullptr);
    ClientServices::SetMessageHandler(SMSG_BARBER_SHOP_RESULT, Packet_SMSG_BARBER_SHOP_RESULT, nullptr);
    CGBarberShop::m_initialized = 0;
}
