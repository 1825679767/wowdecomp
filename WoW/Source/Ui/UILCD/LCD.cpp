#include "LCD.h"
#include <new>
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/UILCD/Windows/EZ_LCD.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDUtils.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/DB/Rec/BattlemasterListRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"

// STATIC: 0x00B1C0BC
TSList<POIIcon, TSGetLink<POIIcon> > CGLCD::m_POIIcons;

// STATIC: 0x00B1C0C8
TSList<MAPDATA, TSGetLink<MAPDATA> > CGLCD::m_mapData;

// STATIC: 0x00D41314
CEzLcd* CGLCD::m_lcd;

// STATIC: 0x00D41330
int32_t CGLCD::m_chatAlertLines;

// STATIC: 0x00D41338
uint8_t CGLCD::initComplete;

// STATIC: 0x00D41348
TSHashTable<LCDHANDLE, HASHKEY_CONSTSTRI> CGLCD::m_handles;

// STATIC: 0x00D41318
CGPlayer_C* CGLCD::m_player;

// STATIC: 0x00D4131C
uint8_t CGLCD::m_inWorld;

// STATIC: 0x00D4131D
static uint8_t s_classFlag8;

// STATIC: 0x00D4131E
static uint8_t s_unk_D4131E;

// STATIC: 0x00D412D0
static char s_battlefieldAlert[64];

// STATIC: 0x00D41320
static uint32_t s_battlefieldPortExpiration;

// STATIC: 0x00D4132C
static uint32_t s_battlefieldStartTime;

// STATIC: 0x00B1C098
static int32_t s_currentScreen = -1;

// STATIC: 0x00B1C09C
static int32_t s_previousScreen = -1;

// STATIC: 0x00B1C0A0
static int32_t s_unk_B1C0A0;

// VERIFY: d134d241 auto
// OFFSET: 0x00848030
static bool IsLCDAvailable() {
    if (CGLCD::m_lcd->IsDeviceAvailable(1)) {
        return true;
    }

    switch (s_locale) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
        case 7: {
            return true;
        }

        default: {
            return false;
        }
    }
}

// VERIFY: cdaa7032 auto
// OFFSET: 0x00848140
void CGLCD::ShowBattlefieldScreen() {
    if (CGLCD::m_lcd && CGLCD::m_lcd->IsConnected() && IsLCDAvailable() && CGLCD::initComplete) {
        s_unk_D4131E = 0;
        CGLCD::SetScreen(1);
    }
}

// VERIFY: e986adf3 auto
// OFFSET: 0x00848180
void CGLCD::ClearWaitQueueScreen() {
    if (CGLCD::m_lcd && CGLCD::m_lcd->IsConnected() && IsLCDAvailable() && CGLCD::initComplete) {
        CGLCD::SetScreen(0);
    }
}

// VERIFY: 6e32be08 manual
// OFFSET: 0x0084AED0
void CGLCD::Initialize() {
    CGLCD::m_lcd = new (SMemAlloc(sizeof(CEzLcd), __FILE__, __LINE__, 0)) CEzLcd();
    CGLCD::m_lcd->Initialize("World of Warcraft", 2, 0, 1, nullptr, nullptr);
    CGLCD::m_lcd->Update();

    if (!CGLCD::m_lcd->IsConnected()) {
        CGLCD::Shutdown();
        return;
    }

    EventRegister(EVENT_ID_IDLE, CGLCD::Update);
}

// VERIFY: 03c1a51c auto
// OFFSET: 0x0084A6E0
void CGLCD::OnBattlefieldActive() {
    if (CGLCD::m_lcd && CGLCD::m_lcd->IsConnected() && IsLCDAvailable() && CGLCD::initComplete) {
        s_unk_D4131E = 1;
        s_battlefieldStartTime = CGBattlefieldInfo::m_instanceStartTime;
        CGLCD::UpdateBattlefieldStats();
        CGLCD::SetScreen(3);
    }
}

// VERIFY: 20771c88 auto
// OFFSET: 0x0084AE50
void CGLCD::Shutdown() {
    CGLCD::m_handles.Clear();

    if (CGLCD::m_lcd) {
        CEzLcd* lcd = CGLCD::m_lcd;
        lcd->~CEzLcd();
        SMemFree(lcd, "delete", SLOG_FUNCTION, 0);
    }

    CGLCD::m_lcd = nullptr;
    EventUnregister(EVENT_ID_IDLE, CGLCD::Update);
    CGLCD::m_mapData.Clear();
    CGLCD::m_POIIcons.Clear();
    CGLCD::initComplete = 0;
    CLCDUtils::Shutdown();
    CGLCD::m_chatAlertLines = 0;
}

// VERIFY: b378b02a auto
// OFFSET: 0x0084A530
void CGLCD::OnWhisper(const char* name) {
    if (CGLCD::m_lcd && CGLCD::m_lcd->IsConnected() && IsLCDAvailable() && CGLCD::initComplete) {
        CGLCD::PushAlert(name, 1);
    }
}

// VERIFY: 0fd36640 auto
// OFFSET: 0x0084A5B0
void CGLCD::AuctionOutbid(const char* name) {
    if (CGLCD::m_lcd && CGLCD::m_lcd->IsConnected() && IsLCDAvailable() && CGLCD::initComplete) {
        CGLCD::PushAlert(name, 3);
    }
}

// VERIFY: 29915760 auto
// OFFSET: 0x0084A5F0
void CGLCD::AuctionExpired(const char* name) {
    if (CGLCD::m_lcd && CGLCD::m_lcd->IsConnected() && IsLCDAvailable() && CGLCD::initComplete && CGLCD::m_lcd->IsDeviceAvailable(1)) {
        CGLCD::PushAlert(name, 4);
    }
}

// VERIFY: ed6aa2a2 auto
// OFFSET: 0x0084A640
void CGLCD::AuctionSold(const char* name) {
    if (CGLCD::m_lcd && CGLCD::m_lcd->IsConnected() && IsLCDAvailable() && CGLCD::initComplete && CGLCD::m_lcd->IsDeviceAvailable(1)) {
        CGLCD::PushAlert(name, 6);
    }
}

// VERIFY: e4264b44 auto
// OFFSET: 0x0084A690
void CGLCD::AuctionWon(const char* name) {
    if (CGLCD::m_lcd && CGLCD::m_lcd->IsConnected() && IsLCDAvailable() && CGLCD::initComplete && CGLCD::m_lcd->IsDeviceAvailable(1)) {
        CGLCD::PushAlert(name, 5);
    }
}

// VERIFY: b3ae8be5 auto
// OFFSET: 0x00848380
void CGLCD::HideBattlefieldAlert() {
    if (CGLCD::m_lcd && CGLCD::m_lcd->IsConnected() && IsLCDAvailable() && CGLCD::initComplete && s_currentScreen == 2) {
        CGLCD::SetScreen(s_previousScreen);
    }
}

// VERIFY: a406c5e1 auto
// OFFSET: 0x008483C0
void CGLCD::OnBattlefieldReady(const BattlefieldStatus* status) {
    if (!CGLCD::m_lcd || !CGLCD::m_lcd->IsConnected() || !IsLCDAvailable() || !CGLCD::initComplete || !status) {
        return;
    }

    if (status->m_bgTypeID == 32) {
        BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(32);

        if (rec) {
            CGLCD::SetScreen(2);
            SStrPrintf(s_battlefieldAlert, sizeof(s_battlefieldAlert), "%s", rec->m_name);
            s_battlefieldPortExpiration = status->m_portExpiration;
        }
    } else {
        MapRec* map = g_mapDB.GetRecord(status->m_mapID);

        if (map) {
            CGLCD::SetScreen(2);
            SStrPrintf(s_battlefieldAlert, sizeof(s_battlefieldAlert), "%s %d", map->m_mapName, status->m_instanceID);
            s_battlefieldPortExpiration = status->m_portExpiration;
        }
    }
}

// VERIFY: 138d395a auto
// OFFSET: 0x00848490
void CGLCD::EnterWorld() {
    if (CGLCD::m_lcd && CGLCD::m_lcd->IsConnected() && IsLCDAvailable() && CGLCD::initComplete) {
        CGLCD::m_inWorld = 1;
        CGLCD::m_player = ClntObjMgrGetActivePlayerObj();
    }
}

// VERIFY: 54eafed4 auto
// OFFSET: 0x00848B70
void CGLCD::Login() {
    if (!CGLCD::m_lcd || !CGLCD::m_lcd->IsConnected() || !IsLCDAvailable() || !CGLCD::initComplete) {
        return;
    }

    CGLCD::m_player = ClntObjMgrGetActivePlayerObj();
    CGLCD::SetScreen(1);
    CGLCD::m_lcd->ShowSubscreen(0);
    CGPlayer_C* player = CGLCD::m_player;
    s_unk_B1C0A0 = 0;
    CGLCD::m_inWorld = 1;

    ChrClassesRec* classRec = g_chrClassesDB.GetRecord(player->m_unit->m_unitFieldBytes0[1]);

    if (classRec) {
        s_classFlag8 = (classRec->m_flags >> 3) & 1;
    }

    int32_t soulShard;

    switch (player->m_unit->m_unitFieldBytes0[1]) {
        case 1:
        case 3:
        case 4: {
            CGLCD::m_lcd->SetVisible(CGLCD::GetHandle("AMMO"), 1);
            CGLCD::m_lcd->SetVisible(CGLCD::GetHandle("AMMO_FLD"), 1);
            CGLCD::m_lcd->SetVisible(CGLCD::GetHandle("SOUL_SHARD"), 0);
            soulShard = 0;
            break;
        }

        case 9: {
            CGLCD::m_lcd->SetVisible(CGLCD::GetHandle("AMMO"), 0);
            CGLCD::m_lcd->SetVisible(CGLCD::GetHandle("AMMO_FLD"), 0);
            CGLCD::m_lcd->SetVisible(CGLCD::GetHandle("SOUL_SHARD"), 1);
            soulShard = 1;
            break;
        }

        default: {
            CGLCD::m_lcd->SetVisible(CGLCD::GetHandle("AMMO"), 0);
            CGLCD::m_lcd->SetVisible(CGLCD::GetHandle("AMMO_FLD"), 0);
            CGLCD::m_lcd->SetVisible(CGLCD::GetHandle("SOUL_SHARD"), 0);
            soulShard = 0;
            break;
        }
    }

    CGLCD::m_lcd->SetVisible(CGLCD::GetHandle("SOUL_SHARD_FLD"), soulShard);
    const char* text = FrameScript_GetText("BF_NOT_IN", -1, GENDER_NOT_APPLICABLE);
    CGLCD::m_lcd->SetText(CGLCD::GetHandle("BF_NOT_IN"), text);
}

// VERIFY: 638d6a34 auto
// OFFSET: 0x00848100
void CGLCD::SetScreen(int32_t screen) {
    if (screen == s_currentScreen) {
        return;
    }

    s_previousScreen = s_currentScreen;
    s_currentScreen = screen;
    CGLCD::m_lcd->ShowPage(screen);
    CGLCD::m_lcd->HideGlobals();
}

// VERIFY: 6e17fc01 auto
// OFFSET: 0x008481F0
void CGLCD::Logout() {
    if (!CGLCD::m_lcd || !CGLCD::m_lcd->IsConnected() || !IsLCDAvailable()) {
        return;
    }

    if (CGLCD::initComplete) {
        CGLCD::m_inWorld = 0;
        CGLCD::SetScreen(0);
        s_unk_D4131E = 0;
        CGLCD::m_player = nullptr;
        CGLCD::m_lcd->Update();
    }
}

// VERIFY: 6c3ba040 auto
// OFFSET: 0x00847E90
void LCDHANDLE::SetText(const char* text) {
    this->m_text.Copy(text);
}
