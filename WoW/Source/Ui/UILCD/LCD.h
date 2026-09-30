#ifndef WOW_SOURCE_UI_UILCD_LCD_H
#define WOW_SOURCE_UI_UILCD_LCD_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CEzLcd;
class CGPlayer_C;

struct LCDHANDLE : public TSHashObject<LCDHANDLE, HASHKEY_CONSTSTRI> {
    /* 0018 */ void* unk_0018;
};

struct MAPDATA : public TSLinkedNode<MAPDATA> {
};

struct POIIcon : public TSLinkedNode<POIIcon> {
};

class CGLCD {
    public:
    static CEzLcd* m_lcd;
    static int32_t m_chatAlertLines;
    static uint8_t initComplete;
    static CGPlayer_C* m_player;
    static uint8_t m_inWorld;
    static TSHashTable<LCDHANDLE, HASHKEY_CONSTSTRI> m_handles;
    static TSList<MAPDATA, TSGetLink<MAPDATA> > m_mapData;
    static TSList<POIIcon, TSGetLink<POIIcon> > m_POIIcons;

    static void ClearWaitQueueScreen();
    static void IdleDisconnectWarning();
    static void Initialize();
    static void SetScreen(int32_t screen);
    static void Shutdown();
    static int32_t Update(const void* data, void* param);
    static void OnWhisper(const char* name);
    static void PushAlert(const char* text, int32_t a2);
    static void Login();
    static void EnterWorld();
};

#endif
