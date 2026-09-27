#ifndef WOW_SOURCE_UI_UILCD_WINDOWS_LCDOUTPUT_H
#define WOW_SOURCE_UI_UILCD_WINDOWS_LCDOUTPUT_H

#include <cstdint>
#include "ThirdParty/lgLcd/lglcd.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDCollection.h"

class CLCDGfxBase;

class CLCDOutput : public CLCDCollection {
    public:
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ int32_t m_hDevice;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t m_nPriority;
    /* 0064 */ CLCDGfxBase* m_pGfx;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint32_t unk_0074;
    /* 0078 */ uint32_t unk_0078;

    CLCDOutput();
    virtual ~CLCDOutput();
    virtual void Shutdown();
    virtual void OnUpdate(uint32_t timestamp);
    virtual void OnDraw(int32_t a1);
    virtual void OnLCDButtonDown(int32_t button);
    virtual void OnLCDButtonUp(int32_t button);
    virtual void OnSoftButtonEvent(uint32_t buttons);
    virtual int32_t DoesBitmapNeedUpdate(lgLcdBitmapHeader* bitmap);
    virtual void OnPageShown(CLCDCollection* page);
    virtual void OnPageExpired(CLCDCollection* page);
    virtual void OnEnteringIdle();
    virtual void OnClosingDevice(int32_t device);
    void Close();
    void SetGfx(CLCDGfxBase* gfx);
};

#endif
