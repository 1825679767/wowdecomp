#include "BarberShop.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// STATIC: 0x00BD19B8
int32_t CGBarberShop::m_barberShopEnabled;

// STATIC: 0x00ACC788
float CGBarberShop::m_minCameraDistance = 1.0f;

// STATIC: 0x00BD19BA
uint8_t CGBarberShop::m_initialized;

// STATIC: 0x00BD19BB
uint8_t CGBarberShop::m_resetPortrait;

// STATIC: 0x00BD19AC
STYLEREC* CGBarberShop::m_currentHairStyle;

// STATIC: 0x00BD19B0
STYLEREC* CGBarberShop::m_currentFacialHair;

// STATIC: 0x00BD19B4
STYLEREC* CGBarberShop::m_currentSkinColor;

// STATIC: 0x00BD19C4
TSHashTable<USEABLESTYLE, HASHKEY_NONE> CGBarberShop::m_useableStyles;

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

// VERIFY: d45ddf6d manual
// OFFSET: 0x0052F760
STYLEREC* CGBarberShop::GetStyle(int32_t type, int32_t race, int32_t sex, int32_t data) {
    HASHKEY_NONE key;
    USEABLESTYLE* useable = CGBarberShop::m_useableStyles.Ptr(type, key);

    if (!useable) {
        return nullptr;
    }

    for (STYLEREC* style = useable->m_styles.Head(); !useable->m_styles.IsEnd(style); style = useable->m_styles.RawNext(style)) {
        const BarberShopStyleRec* rec = style->m_rec;

        if (rec->m_race == race && (rec->m_sex == sex || rec->m_sex == -1) && rec->m_data == data) {
            return style;
        }
    }

    return nullptr;
}

// VERIFY: fdac6219 manual
// OFFSET: 0x0052FE90
void CGBarberShop::EnterWorld() {
    if (CGBarberShop::m_initialized) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t race = player->m_unit->m_unitFieldBytes0[0];
    int32_t sex = player->m_player->m_playerBytes3[0];

    for (int32_t i = 0; i < g_barberShopStyleDB.m_numRecords; i++) {
        BarberShopStyleRec* rec = g_barberShopStyleDB.GetRecordByIndex(i);

        if (!rec || rec->m_race != race || (rec->m_sex != sex && rec->m_sex != -1)) {
            continue;
        }

        HASHKEY_NONE key;
        USEABLESTYLE* useable = CGBarberShop::m_useableStyles.Ptr(rec->m_type, key);

        if (!useable) {
            useable = CGBarberShop::m_useableStyles.New(rec->m_type, key, 0, 0);
        }

        STYLEREC* style = useable->m_styles.NewNode(2, 0, 0);
        style->m_rec = rec;
    }

    CGBarberShop::m_currentHairStyle = CGBarberShop::GetStyle(0, player->m_unit->m_unitFieldBytes0[0], player->m_player->m_playerBytes3[0], player->m_player->m_playerBytes[2]);
    CGBarberShop::m_currentFacialHair = CGBarberShop::GetStyle(2, player->m_unit->m_unitFieldBytes0[0], player->m_player->m_playerBytes3[0], player->m_player->m_playerBytes2[0]);
    CGBarberShop::m_currentSkinColor = CGBarberShop::GetStyle(3, player->m_unit->m_unitFieldBytes0[0], player->m_player->m_playerBytes3[0], player->m_player->m_playerBytes[0]);
}
