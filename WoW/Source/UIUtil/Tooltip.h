#ifndef WOW_SOURCE_UIUTIL_TOOLTIP_H
#define WOW_SOURCE_UIUTIL_TOOLTIP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/WowTime.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/UIUtil/Types.h"

class CImVector;
class CSimpleFontString;
class CSimpleStatusBar;
class CSimpleTexture;
class CStatus;
class XMLNode;
struct lua_State;

class ItemLinkInfo {
    public:
    /* 0000 */ uint32_t m_itemId;
    /* 0004 */ uint32_t m_enchants [12];
    /* 0034 */ uint32_t m_gems [12];
    /* 0064 */ uint32_t m_gemEnchants [12];
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint32_t unk_0098;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ uint32_t unk_00A8;
    /* 00AC */ uint32_t unk_00AC;
    /* 00B0 */ uint32_t unk_00B0;
    /* 00B4 */ uint32_t unk_00B4;
    /* 00B8 */ int32_t unk_00B8;
    /* 00BC */ uint32_t unk_00BC;
    /* 00C0 */ uint32_t unk_00C0;
    /* 00C4 */ uint32_t unk_00C4;
    /* 00C8 */ uint32_t unk_00C8;
    /* 00CC */ uint32_t unk_00CC;
    /* 00D0 */ uint32_t unk_00D0;
    /* 00D4 */ int32_t unk_00D4;

    void Clear();
};

class CGTooltip;
class CAaRect;
class CGItem_C;
class CGUnit_C;
class CImVector;
class CSimpleFontString;

void TooltipUnitNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void TooltipCorpseCallback(int32_t id, const WOWGUID& guid, void* param, bool success);

class CGTooltip : public CSimpleFrame {
    public:
    static CDataAllocator s_tooltipHeap;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static const CImVector* GetItemQualityColor(uint32_t quality);
    static const char* GetItemQualityColorString(uint32_t quality);
    static const char* CreateItemLink(int32_t itemId, uint32_t quality, int32_t enchant, const int32_t* gems, int32_t randomPropertiesId, int32_t seed);
    static const char* GetItemHyperlinkString(CGItem_C* item);
    static const char* GetQuestDifficultyColorString(int32_t level);
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 029C */ CSimpleFrame* m_owner;
    /* 02A0 */ int32_t m_anchorType;
    /* 02A4 */ uint32_t m_numLines;
    /* 02A8 */ uint32_t m_maxLines;
    /* 02AC */ TSFixedArray<CSimpleFontString*> m_leftStrings;
    /* 02B8 */ TSFixedArray<CSimpleFontString*> m_rightStrings;
    /* 02C4 */ TSFixedArray<int32_t> m_lineWrap;
    /* 02D0 */ CSimpleStatusBar* m_statusBar;
    /* 02D4 */ CSimpleTexture* m_textures [10];
    /* 02FC */ uint32_t m_textureLines [10];
    /* 0324 */ uint32_t m_numTextures;
    /* 0328 */ WOWGUID m_unitGuid;
    /* 0330 */ uint32_t unk_0330;
    /* 0334 */ uint32_t unk_0334;
    /* 0338 */ uint32_t unk_0338;
    /* 033C */ uint32_t unk_033C;
    /* 0340 */ WOWGUID m_itemGuid;
    /* 0348 */ uint32_t unk_0348;
    /* 034C */ uint32_t unk_034C;
    /* 0350 */ uint32_t unk_0350;
    /* 0354 */ uint32_t unk_0354;
    /* 0358 */ WOWGUID m_healthGuid;
    /* 0360 */ uint32_t unk_0360;
    /* 0364 */ uint32_t unk_0364;
    /* 0368 */ uint32_t unk_0368;
    /* 036C */ uint32_t unk_036C;
    /* 0370 */ uint32_t unk_0370;
    /* 0374 */ uint32_t unk_0374 [4];
    /* 0384 */ WowTime m_time;
    /* 03A4 */ uint32_t unk_03A4 [4];
    /* 03B4 */ uint32_t unk_03B4;
    /* 03B8 */ uint32_t unk_03B8;
    /* 03BC */ uint32_t unk_03BC;
    /* 03C0 */ uint32_t unk_03C0;
    /* 03C4 */ uint32_t unk_03C4;
    /* 03C8 */ uint32_t unk_03C8;
    /* 03CC */ uint32_t m_fading;
    /* 03D0 */ float m_fadeTime;
    /* 03D4 */ float m_padding;
    /* 03D8 */ float m_minimumWidth;
    /* 03DC */ uint32_t m_minimumWidthForce;
    /* 03E0 */ float m_offsetX;
    /* 03E4 */ float m_offsetY;
    /* 03E8 */ ItemLinkInfo m_itemLinkInfo;
    /* 04C0 */ uint32_t unk_04C0;
    /* 04C4 */ uint32_t unk_04C4;
    /* 04C8 */ uint32_t unk_04C8;
    /* 04CC */ uint32_t unk_04CC;
    /* 04D0 */ uint32_t unk_04D0;
    /* 04D4 */ FrameScript_Object::ScriptIx m_onTooltipSetDefaultAnchor;
    /* 04DC */ FrameScript_Object::ScriptIx m_onTooltipCleared;
    /* 04E4 */ FrameScript_Object::ScriptIx m_onTooltipAddMoney;
    /* 04EC */ FrameScript_Object::ScriptIx m_onTooltipSetUnit;
    /* 04F4 */ FrameScript_Object::ScriptIx m_onTooltipSetItem;
    /* 04FC */ FrameScript_Object::ScriptIx m_onTooltipSetSpell;
    /* 0504 */ FrameScript_Object::ScriptIx m_onTooltipSetQuest;
    /* 050C */ FrameScript_Object::ScriptIx m_onTooltipSetAchievement;
    /* 0514 */ FrameScript_Object::ScriptIx m_onTooltipSetEquipmentSet;
    /* 051C */ FrameScript_Object::ScriptIx m_onTooltipSetFrameStack;
    /* 0524 */ uint32_t unk_0524;

    ~CGTooltip() override;
    int32_t GetScriptMetaTable() override;
    FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) override;
    void PostLoadXML(XMLNode* node, CStatus* status) override;
    void OnLayerUpdate(float elapsedSec) override;
    int32_t HideThis() override;
    int32_t ShowThis() override;

    CGTooltip(CSimpleFrame* parent);
    void CalculateSize();
    void AppendText(const char* text);
    void ClearTooltip();
    void FadeOut();
    void ResetPosition(int32_t force);
    void SetOwner(CSimpleFrame* owner, TOOLTIP_ANCHORPOINT anchorType, float offsetX, float offsetY);
    void SetDefaultAnchor();
    int32_t SetUnit(const WOWGUID& guid, int32_t showHealth, const WOWGUID& healthGuid, int32_t a4);
    void SetObject(const WOWGUID& guid);
    void SetCorpse(const WOWGUID& guid);
    void AddFontStrings(CSimpleFontString* left, CSimpleFontString* right);
    void AddLine(const char* left, const char* right, const CImVector& leftColor, const CImVector& rightColor, int32_t wrap);
    void AddTexture(const char* texture, const CAaRect& coords, const CImVector& color);
    void AddEmpathyText(CGUnit_C* unit, char* buffer, uint32_t size);
    void AddSkinnableText(CGUnit_C* unit, char* buffer, uint32_t size);
    static void GetSummonedByString(CGUnit_C* unit, char* buffer, uint32_t size);
};

class CGItemStatsSummary {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[28];
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint8_t unk_0030[180];
    /* 00E4 */ uint32_t unk_00E4;
    /* 00E8 */ uint8_t unk_00E8[64];
    /* 0128 */ uint32_t unk_0128;
};

class CSimpleUI {
    public:
    /* 0000 */ uint8_t unk_0000[720];
    /* 02D0 */ uint32_t unk_02D0;
    /* 02D4 */ uint8_t unk_02D4[84];
    /* 0328 */ uint32_t unk_0328;
    /* 032C */ uint32_t unk_032C;
    /* 0330 */ uint8_t unk_0330[40];
    /* 0358 */ uint32_t unk_0358;
    /* 035C */ uint32_t unk_035C;
    /* 0360 */ uint8_t unk_0360[396];
    /* 04EC */ uint32_t unk_04EC;
};

extern uint32_t s_tooltipTitleColor;

#endif
