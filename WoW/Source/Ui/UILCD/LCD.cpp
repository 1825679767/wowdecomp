#include "LCD.h"
#include <new>
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Ui/UILCD/Windows/EZ_LCD.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDUtils.h"

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
