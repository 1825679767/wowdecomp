#ifndef WOW_SOURCE_UI_NAMEPLATEFRAME_H
#define WOW_SOURCE_UI_NAMEPLATEFRAME_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/Object/WGUID.h"

class CSimpleFontString;
class CSimpleTexture;

class CGUnit_C;

class CGNamePlateFrame : public CSimpleFrame {
    public:
    static CGNamePlateFrame* GetNamePlateFocus();

    void UpdateLevelDisplay(CGUnit_C* unit);

    /* 029C */ TSLink<CGNamePlateFrame> m_link;
    /* 02A4 */ uint32_t unk_02A4;
    /* 02A8 */ WOWGUID m_guid;
    /* 02B0 */ CSimpleTexture* m_namePlateGlow;
    /* 02B4 */ CSimpleTexture* m_targetingFrameSkull;
    /* 02B8 */ CSimpleTexture* m_raidTragetingIcons;
    /* 02BC */ CSimpleTexture* m_namePlateBorder;
    /* 02C0 */ CSimpleTexture* m_castBarShield;
    /* 02C4 */ CSimpleTexture* m_targetingFrameFlash;
    /* 02C8 */ CSimpleTexture* m_eliteNameplateIcon;
    /* 02CC */ CSimpleTexture* unk_02CC;
    /* 02D0 */ CSimpleFontString* m_unitNameText;
    /* 02D4 */ CSimpleFontString* unk_02D4;
    /* 02D8 */ uint32_t m_simpleHealthBar;
    /* 02DC */ uint32_t m_targetingFrame;
    /* 02E0 */ float unk_02E0;
    /* 02E4 */ float unk_02E4;
    /* 02E8 */ float unk_02E8;
    /* 02EC */ CImVector unk_02EC;
    /* 02F0 */ uint32_t unk_02F0;
    /* 02F4 */ uint32_t unk_02F4;
    /* 02F8 */ float unk_02F8;
    /* 02FC */ float unk_02FC;

    void TriggerDamageFeedback();
    void UpdateColorDisplay(CGUnit_C* unit);
    void UpdateNameDisplay(const CGUnit_C* unit);
    void SetCastBarValues(float minValue, float maxValue, float value, int32_t spellId, bool channel, bool notInterruptible);
};

#endif
