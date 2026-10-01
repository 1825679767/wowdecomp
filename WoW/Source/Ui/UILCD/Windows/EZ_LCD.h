#ifndef WOW_SOURCE_UI_UILCD_WINDOWS_EZ_LCD_H
#define WOW_SOURCE_UI_UILCD_WINDOWS_EZ_LCD_H

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "ThirdParty/lgLcd/lglcd.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDConnection.h"

class CEzLcdPage;
class CLCDOutput;

class CEzLcd {
    public:
    /* 0004 */ char m_friendlyName[260];
    /* 0108 */ CLCDConnection m_connection;
    /* 0174 */ int32_t m_lcdType;
    /* 0178 */ TSGrowableArray<CEzLcdPage*> m_monoPages;
    /* 0188 */ TSGrowableArray<CEzLcdPage*> m_colorPages;
    /* 0198 */ uint32_t unk_0198;
    /* 019C */ uint32_t unk_019C;
    /* 01A0 */ CEzLcdPage* m_monoGlobalPage;
    /* 01A4 */ CEzLcdPage* m_colorGlobalPage;
    /* 01A8 */ uint32_t unk_01A8;
    /* 01AC */ uint32_t unk_01AC;
    /* 01B0 */ uint32_t unk_01B0;
    /* 01B4 */ uint32_t unk_01B4;
    /* 01B8 */ int32_t unk_01B8;
    /* 01BC */ int32_t m_initialized;
    /* 01C0 */ lgLcdConfigureContext* m_configContext;
    /* 01C4 */ int32_t m_isPersistent;
    /* 01C8 */ int32_t m_isAutoStartable;
    /* 01CC */ uint32_t unk_01CC;
    /* 01D0 */ uint32_t unk_01D0;
    /* 01D4 */ int32_t unk_01D4;
    /* 01D8 */ int32_t unk_01D8;
    /* 01DC */ CRITICAL_SECTION m_buttonLock;
    /* 01F4 */ uint32_t m_buttonsPending;
    /* 01F8 */ uint32_t m_buttonsPrevious;
    /* 01FC */ uint32_t m_buttonsCurrent;
    /* 0200 */ lgLcdSoftbuttonsChangedContext m_softbuttonsChangedContext;
    /* 0208 */ CLCDOutput* m_output;

    static DWORD WINAPI OnSoftButtonCB(int device, DWORD dwButtons, const PVOID pContext);

    CEzLcd();
    ~CEzLcd();
    virtual void OnSoftButtons(uint32_t buttons);
    int32_t Initialize(const char* friendlyName, int32_t lcdType, int32_t isAutoStartable, int32_t isPersistent, lgLcdConfigureContext* configContext, lgLcdSoftbuttonsChangedContext* softbuttonsChangedContext);
    int32_t IsConnected();
    void ShowSubscreen(int32_t subscreen);
    void SetVisible(void* handle, int32_t visible);
    void SetText(void* handle, const char* text);
    int32_t IsDeviceAvailable(int32_t type);
    void Update();
};

#endif
