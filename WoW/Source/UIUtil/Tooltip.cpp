#include "Tooltip.h"
#include <cmath>
#include <cstddef>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleStatusBar.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Object/GameObject.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include <typeinfo>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Tempest/CAaRect.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CreatureTypeRec.h"
#include "WoW/Source/DB/Rec/FactionRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassMaskRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/DB/Rec/PowerDisplayRec.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellRuneCostRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"
#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"
#include "WoW/Source/DB/Rec/TalentRec.h"
#include "WoW/Source/DB/Rec/TalentTabRec.h"
#include "WoW/Source/DB/Rec/TotemCategoryRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/CreatureStats.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/QuestLogLeaderBoardParams.h"
#include "WoW/Source/Ui/QuestTextParser.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "_Triage/AreaList.h"

// STATIC: 0x00C5CF4C
int32_t CGTooltip::s_objectType;

// STATIC: 0x00AD2D08
int32_t CGTooltip::s_metatable = LUA_NOREF;

// STATIC: 0x00AD2A84
static const char* s_itemQualityColorStrings[8] = { "|cff9d9d9d", "|cffffffff", "|cff1eff00", "|cff0070dd", "|cffa335ee", "|cffff8000", "|cffe6cc80", "|cffe6cc80" };

// STATIC: 0x00AD2D2C
uint32_t s_tooltipTitleColor = 0xFFFFD200;

// STATIC: 0x00AD2D30
static uint32_t s_tooltipTextColor = 0xFFFFFFFF;

// STATIC: 0x00AD2D34
static uint32_t s_tooltipRedColor = 0xFFFF2020;

// STATIC: 0x00AD2D38
static uint32_t s_tooltipGrayColor = 0xFF808080;

// STATIC: 0x00AD2D40
static uint32_t s_tooltipGreenColor = 0xFF00FF00;

// STATIC: 0x00AD2D70
static uint32_t s_threatStatusColors[5] = { 0xFFFFFFFF, 0xFFB0B0B0, 0xFFFFFF77, 0xFFFF9900, 0xFFFF0000 };

// STATIC: 0x00AD2E6C
static const char* s_classificationTokens[6] = { nullptr, "ELITE", "ELITE", nullptr, nullptr, nullptr };

// STATIC: 0x00AD2D84
static uint32_t s_itemQualityColors[8] = { 0xFF9D9D9D, 0xFFFFFFFF, 0xFF1EFF00, 0xFF0070DD, 0xFFA335EE, 0xFFFF8000, 0xFFE6CC80, 0xFFE6CC80 };

// STATIC: 0x00AD2ADC
const char* CGTooltip::s_objectTypeName = "GameTooltip";

// STATIC: 0x00C5D354
CDataAllocator CGTooltip::s_tooltipHeap(sizeof(CGTooltip), 5);

// STATIC: 0x00AD2A68
static const char* s_questDifficultyColorStrings[5] = {
    "|cffff2020",
    "|cffff8040",
    "|cffffff00",
    "|cff40c040",
    "|cff808080",
};

// STATIC: 0x00AD2A5C
static const char* s_tooltipRedColorString = "|cffff2020";

// STATIC: 0x00AD2EA0
static const char* s_powerCostTokens[7] = { "MANA_COST", "RAGE_COST", "FOCUS_COST", "ENERGY_COST", "HAPPINESS_COST", "RUNE_COST", "RUNIC_POWER_COST" };

// STATIC: 0x00AD2E84
static const char* s_useAllPowerTokens[7] = { "SPELL_USE_ALL_MANA", "SPELL_USE_ALL_RAGE", "SPELL_USE_ALL_FOCUS", "SPELL_USE_ALL_ENERGY", "SPELL_USE_ALL_HAPPINESS", "SPELL_USE_ALL_RUNE", nullptr };

// STATIC: 0x00C5CF50
static char s_itemLink[1024];

// VERIFY: 1bfb61f6 manual
// OFFSET: 0x0061BF10
static int32_t HealthUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(param);
        statusBar->SetMinMaxValues(0.0f, static_cast<float>(static_cast<int32_t>(unit->m_unit->m_unitFieldMaxhealth)));
        statusBar->SetValue(static_cast<float>(unit->m_unit->m_unitFieldHealth));
    }

    return 1;
}

// VERIFY: 30f0882c manual
// OFFSET: 0x0061BF80
static int32_t ObjHealthUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if (object) {
        CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(param);
        statusBar->SetMinMaxValues(0.0f, 1.0f);
        statusBar->SetValue(static_cast<float>(object->m_gameObject->m_animProgress) * 0.0039215689f);
    }

    return 1;
}

// INLINED
void CGTooltip::operator delete(void* ptr) {
    CGTooltip::s_tooltipHeap.PutData(ptr, nullptr, 0);
}

// INLINED
int32_t CGTooltip::GetObjectType() {
    if (!CGTooltip::s_objectType) {
        CGTooltip::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGTooltip::s_objectType;
}

// VERIFY: 620233cc auto
// OFFSET: 0x0050F590
void ItemLinkInfo::Clear() {
    this->m_itemId = 0;
    memset(this->m_enchants, 0, sizeof(this->m_enchants));
    memset(this->m_gems, 0, sizeof(this->m_gems));
    memset(this->m_gemEnchants, 0, sizeof(this->m_gemEnchants));
    this->unk_0094 = 0;
    this->unk_0098 = 0;
    this->unk_00A0 = 0;
    this->unk_00A4 = 0;
    this->unk_00A8 = 0;
    this->unk_00AC = 0;
    this->unk_00B0 = 0;
    this->unk_00B4 = 0;
    this->unk_00BC = 0;
    this->unk_00C0 = 0;
    this->unk_00C4 = 0;
    this->unk_00C8 = 0;
    this->unk_00CC = 0;
    this->unk_00B8 = -1;
    this->unk_00D0 = 1;
    this->unk_00D4 = -1;
}

// VERIFY: bd6b1c11 auto
// OFFSET: 0x005143A0
CSimpleFrame* CGTooltip::Create(CSimpleFrame* parent) {
    if (CGTooltip::s_tooltipHeap.m_bytesPerData < sizeof(CGTooltip)) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    void* data = CGTooltip::s_tooltipHeap.GetData(0, typeid(CGTooltip).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGTooltip(parent) : nullptr;
}

// VERIFY: bd81ee98 auto
// OFFSET: 0x005143F0
void CGTooltip::SetDefaultAnchor() {
    if (this->m_onTooltipSetDefaultAnchor.m_luaRef) {
        this->RunScript(this->m_onTooltipSetDefaultAnchor, 0, nullptr);
    }
}

// VERIFY: b6747482 auto
// OFFSET: 0x0061A280
FrameScript_Object::ScriptIx* CGTooltip::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = CSimpleFrame::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmpI(name, "OnTooltipSetDefaultAnchor", STORM_MAX_STR)) {
        return &this->m_onTooltipSetDefaultAnchor;
    }

    if (!SStrCmpI(name, "OnTooltipCleared", STORM_MAX_STR)) {
        return &this->m_onTooltipCleared;
    }

    if (!SStrCmpI(name, "OnTooltipAddMoney", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,cost,maxcost) %s end";
        return &this->m_onTooltipAddMoney;
    }

    if (!SStrCmpI(name, "OnTooltipSetUnit", STORM_MAX_STR)) {
        return &this->m_onTooltipSetUnit;
    }

    if (!SStrCmpI(name, "OnTooltipSetItem", STORM_MAX_STR)) {
        return &this->m_onTooltipSetItem;
    }

    if (!SStrCmpI(name, "OnTooltipSetSpell", STORM_MAX_STR)) {
        return &this->m_onTooltipSetSpell;
    }

    if (!SStrCmpI(name, "OnTooltipSetQuest", STORM_MAX_STR)) {
        return &this->m_onTooltipSetQuest;
    }

    if (!SStrCmpI(name, "OnTooltipSetAchievement", STORM_MAX_STR)) {
        return &this->m_onTooltipSetAchievement;
    }

    if (!SStrCmpI(name, "OnTooltipSetEquipmentSet", STORM_MAX_STR)) {
        return &this->m_onTooltipSetEquipmentSet;
    }

    if (!SStrCmpI(name, "OnTooltipSetFrameStack", STORM_MAX_STR)) {
        return &this->m_onTooltipSetFrameStack;
    }

    return nullptr;
}

// VERIFY: 38ecbe18 auto
// OFFSET: 0x0061A400
void CGTooltip::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetDefaultAnchor, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipCleared, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipAddMoney, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetUnit, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetItem, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetSpell, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetQuest, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetAchievement, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetEquipmentSet, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetFrameStack, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// VERIFY: fd8d870d auto
// OFFSET: 0x0061A510
const CImVector* CGTooltip::GetItemQualityColor(uint32_t quality) {
    if (quality >= sizeof(s_itemQualityColors) / sizeof(s_itemQualityColors[0])) {
        quality = 1;
    }

    return reinterpret_cast<const CImVector*>(&s_itemQualityColors[quality]);
}

// VERIFY: 334f5517 auto
// OFFSET: 0x0061A530
const char* CGTooltip::GetItemQualityColorString(uint32_t quality) {
    if (quality >= sizeof(s_itemQualityColorStrings) / sizeof(s_itemQualityColorStrings[0])) {
        quality = 1;
    }

    return s_itemQualityColorStrings[quality];
}

// VERIFY: 217f6dcb manual
// OFFSET: 0x0061B040
void CGTooltip::ResetPosition(int32_t force) {
    if (!this->m_owner || this->m_anchorType == ANCHOR_PRESERVE) {
        return;
    }

    if (force || this->m_anchorType != ANCHOR_NONE) {
        this->MarkFramePointsUnused();
    }

    switch (this->m_anchorType) {
        case ANCHOR_LEFT:
            this->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this->m_owner, FRAMEPOINT_TOPLEFT, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_RIGHT:
            this->SetPoint(FRAMEPOINT_BOTTOMLEFT, this->m_owner, FRAMEPOINT_TOPRIGHT, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_BOTTOMLEFT:
            this->SetPoint(FRAMEPOINT_TOPRIGHT, this->m_owner, FRAMEPOINT_BOTTOMLEFT, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_BOTTOM:
            this->SetPoint(FRAMEPOINT_TOP, this->m_owner, FRAMEPOINT_BOTTOM, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_BOTTOMRIGHT:
            this->SetPoint(FRAMEPOINT_TOPLEFT, this->m_owner, FRAMEPOINT_BOTTOMRIGHT, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_TOPLEFT:
            this->SetPoint(FRAMEPOINT_BOTTOMLEFT, this->m_owner, FRAMEPOINT_TOPLEFT, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_TOP:
            this->SetPoint(FRAMEPOINT_BOTTOM, this->m_owner, FRAMEPOINT_TOP, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_TOPRIGHT:
            this->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this->m_owner, FRAMEPOINT_TOPRIGHT, this->m_offsetX, this->m_offsetY, 1);
            break;

        default:
            break;
    }
}

// VERIFY: 5e00a252 auto
// OFFSET: 0x0061B290
void CGTooltip::FadeOut() {
    if (this->m_anchorType == ANCHOR_CURSOR || this->m_anchorType == ANCHOR_CURSOR_RIGHT) {
        this->HideThis();
        this->m_fading = 0;
    } else {
        this->m_fading = 1;
        this->m_fadeTime = 2.0f;
    }
}

// VERIFY: 5d77733c manual
// OFFSET: 0x0061B2E0
void CGTooltip::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);

    if (this->m_anchorType == ANCHOR_CURSOR) {
        float x = 0.0f;
        float y = 0.0f;
        NDCToDDC(this->m_simpleTop->m_mousePosition.m_x, this->m_simpleTop->m_mousePosition.m_y, &x, &y);
        float scale = 1.0f / this->m_layoutScale;
        x *= scale;
        y *= scale;
        this->SetPoint(FRAMEPOINT_BOTTOM, CSimpleTop::s_instance, FRAMEPOINT_BOTTOMLEFT, x, y, 1);
    } else if (this->m_anchorType == ANCHOR_CURSOR_RIGHT) {
        float x = 0.0f;
        float y = 0.0f;
        NDCToDDC(this->m_simpleTop->m_mousePosition.m_x, this->m_simpleTop->m_mousePosition.m_y, &x, &y);
        float scale = 1.0f / this->m_layoutScale;
        x *= scale;
        y *= scale;
        this->SetPoint(FRAMEPOINT_BOTTOMLEFT, CSimpleTop::s_instance, FRAMEPOINT_BOTTOMLEFT, x + this->m_offsetX, y + this->m_offsetY, 1);
    }

    if (this->m_fading) {
        this->m_fadeTime -= elapsedSec;

        if (this->m_fadeTime <= 0.0f) {
            this->SetFrameAlpha(255);
            this->Hide();
            return;
        }

        float alpha = this->m_fadeTime;

        if (alpha >= 1.0f) {
            alpha = 1.0f;
        }

        this->SetFrameAlpha(static_cast<uint8_t>(static_cast<int32_t>(alpha * 255.0f)));
    }
}

// VERIFY: 5a252c7f manual
// OFFSET: 0x0061C620
void CGTooltip::ClearTooltip() {
    if (this->m_healthGuid != WOWGUID()) {
        if (this->m_unitGuid != WOWGUID()) {
            ClntObjMgrUnsetObjMirrorHandler(this->m_healthGuid, ID_UNIT, offsetof(CGUnitData, m_unitFieldHealth), HealthUpdateHandler, this->m_statusBar);
        } else {
            ClntObjMgrUnsetObjMirrorHandler(this->m_healthGuid, ID_GAMEOBJECT, offsetof(CGGameObjectData, m_animProgress), ObjHealthUpdateHandler, this->m_statusBar);
        }

        this->m_statusBar->Hide();
    }

    this->m_unitGuid = WOWGUID();
    this->unk_0330 = 0;
    this->unk_0334 = 0;
    this->unk_0338 = 0;
    this->unk_033C = 0;
    this->unk_0348 = 0;
    this->unk_034C = 0;
    this->m_healthGuid = WOWGUID();
    this->unk_0360 = 0;
    this->m_spellId = 0;
    this->unk_0368 = 0;
    this->unk_036C = 0;
    this->unk_0370 = 0;

    for (uint32_t i = 0; i < this->m_numTextures; i++) {
        uint32_t line = this->m_textureLines[i];
        CSimpleFontString** left = &this->m_leftStrings.m_data[line];
        this->m_leftStrings.m_data[line]->SetPoint(FRAMEPOINT_TOPLEFT, left[-1], FRAMEPOINT_BOTTOMLEFT, 0.0f, NDCToDDCWidth(-2.0f / (CoordinateGetAspectCompensation() * 1024.0f)), 1);

        if (line < this->m_maxLines - 1 && this->m_leftStrings.m_data[line + 1]) {
            CSimpleFontString** strings = &this->m_leftStrings.m_data[line];
            strings[1]->SetPoint(FRAMEPOINT_TOPLEFT, strings[0], FRAMEPOINT_BOTTOMLEFT, 0.0f, 0.0f, 1);
        }

        this->m_textures[i]->Hide();
    }

    bool hadLines = this->m_numLines > 0;
    this->m_numTextures = 0;

    if (this->m_numLines) {
        for (uint32_t i = 0; i < this->m_numLines; i++) {
            this->m_leftStrings.m_data[i]->SetWidth(0.0f);
            this->m_leftStrings.m_data[i]->SetText("", 1);
            this->m_leftStrings.m_data[i]->Hide();
            this->m_rightStrings.m_data[i]->SetText("", 1);
            this->m_rightStrings.m_data[i]->Hide();
            this->m_lineWrap.m_data[i] = 0;
        }

        this->m_numLines = 0;
    }

    if (!this->m_minimumWidthForce) {
        this->m_minimumWidth = 0.0f;
    }

    if (hadLines && this->m_onTooltipCleared.m_luaRef) {
        this->RunScript(this->m_onTooltipCleared, 0, nullptr);
    }
}

// VERIFY: 994304f7 manual
// OFFSET: 0x0061CAF0
void CGTooltip::CalculateSize() {
    if (!this->m_owner) {
        return;
    }

    float width = NDCToDDCWidth(this->m_minimumWidth / (CoordinateGetAspectCompensation() * 1024.0f));

    if (!this->m_minimumWidthForce) {
        for (uint32_t i = 0; i < this->m_numLines; i++) {
            if (this->m_lineWrap.m_data[i]) {
                continue;
            }

            float lineWidth = 0.0f;

            if (this->m_leftStrings.m_data[i]->m_shown && this->m_rightStrings.m_data[i]->m_shown) {
                lineWidth = NDCToDDCWidth(38.400002f / (CoordinateGetAspectCompensation() * 1024.0f));
            }

            if (this->m_leftStrings.m_data[i]->m_shown) {
                lineWidth = this->m_leftStrings.m_data[i]->GetWidth() + lineWidth;
            }

            if (this->m_rightStrings.m_data[i]->m_shown) {
                lineWidth = this->m_rightStrings.m_data[i]->GetWidth() + lineWidth;
            }

            for (uint32_t j = 0; j < this->m_numTextures; j++) {
                if (this->m_textureLines[j] == i) {
                    lineWidth = this->m_textures[j]->GetWidth() + lineWidth + 0.0049999999f;
                    break;
                }
            }

            if (lineWidth > width) {
                width = lineWidth;
            }
        }
    }

    for (uint32_t i = 0; i < this->m_numLines; i++) {
        if (!this->m_minimumWidthForce && !this->m_lineWrap.m_data[i]) {
            continue;
        }

        float stringWidth = this->m_leftStrings.m_data[i]->GetStringWidth();

        if (stringWidth >= NDCToDDCWidth(230.39999f / (CoordinateGetAspectCompensation() * 1024.0f))) {
            stringWidth = NDCToDDCWidth(230.39999f / (CoordinateGetAspectCompensation() * 1024.0f));
        }

        float lineWidth = stringWidth;

        if (width < lineWidth) {
            CSimpleFontString* left = this->m_leftStrings.m_data[i];
            const char* text = left->m_text && *left->m_text ? left->m_text : nullptr;
            uint32_t lineStarts[30];
            uint32_t count = left->WrapText(text, lineWidth, lineStarts, 30) >= 30 ? 30 : this->m_leftStrings.m_data[i]->WrapText(text, stringWidth, lineStarts, 30);

            lineWidth = 0.0f;
            stringWidth = 0.0f;

            for (uint32_t j = 0; j < count; j++) {
                uint32_t start = lineStarts[j];
                uint32_t end = j >= count - 1 ? SStrLen(text) : lineStarts[j + 1];
                lineWidth = this->m_leftStrings.m_data[i]->GetTextWidth(&text[start], end - start);

                if (stringWidth >= lineWidth) {
                    lineWidth = stringWidth;
                } else {
                    stringWidth = lineWidth;
                }
            }
        }

        if (width > lineWidth) {
            lineWidth = width;
        } else {
            width = lineWidth;
        }

        this->m_leftStrings.m_data[i]->SetWidth(lineWidth);
    }

    for (uint32_t i = 0; i < this->m_numLines; i++) {
        float offset = width;

        if (!this->m_rightStrings.m_data[i]->m_shown) {
            continue;
        }

        for (uint32_t j = 0; j < this->m_numTextures; j++) {
            if (this->m_textureLines[j] == i) {
                offset = width - this->m_textures[j]->GetWidth() - 0.0049999999f;
                break;
            }
        }

        this->m_rightStrings.m_data[i]->SetPoint(FRAMEPOINT_RIGHT, this->m_leftStrings.m_data[i], FRAMEPOINT_LEFT, offset, 0.0f, 1);
    }

    float height = 0.0f;

    for (uint32_t i = 0; i < this->m_numLines; i++) {
        if (!this->m_leftStrings.m_data[i]->m_shown) {
            continue;
        }

        if (height != 0.0f) {
            height = NDCToDDCWidth(2.0f / (CoordinateGetAspectCompensation() * 1024.0f)) + height;
        }

        height = this->m_leftStrings.m_data[i]->GetHeight() + height;
    }

    float borderX = NDCToDDCWidth(10.24f / (CoordinateGetAspectCompensation() * 1024.0f));
    float totalWidth = borderX + borderX + this->m_padding + width;
    float borderY = NDCToDDCWidth(10.24f / (CoordinateGetAspectCompensation() * 1024.0f));
    float totalHeight = borderY + borderY + height;
    this->SetWidth(totalWidth);
    this->SetHeight(totalHeight);

    if (this->m_anchorType != ANCHOR_CURSOR && this->m_anchorType != ANCHOR_CURSOR_RIGHT) {
        this->ResetPosition(0);
    }

    this->Resize(1);
}

// VERIFY: 49557c47 auto
// OFFSET: 0x0061CFF0
int32_t CGTooltip::ShowThis() {
    if (!this->m_owner || !this->m_numLines) {
        this->Hide();
        return 0;
    }

    this->SetFrameAlpha(255);
    this->m_fading = 0;
    this->CalculateSize();
    return CSimpleFrame::ShowThis();
}

// VERIFY: 4aa6f7f3 manual
// OFFSET: 0x0061DEE0
CGTooltip::CGTooltip(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_itemLinkInfo.Clear();
    this->m_padding = 0.0f;
    this->m_minimumWidth = 0.0f;
    this->m_owner = nullptr;
    this->m_offsetX = 0.0f;
    this->m_numLines = 0;
    this->m_offsetY = 0.0f;
    this->m_maxLines = 0;
    this->m_numTextures = 0;
    this->m_unitGuid = WOWGUID();
    this->unk_0330 = 0;
    this->unk_0334 = 0;
    this->unk_0338 = 0;
    this->unk_033C = 0;
    this->m_itemGuid = WOWGUID();
    this->unk_0348 = 0;
    this->unk_034C = 0;
    this->m_healthGuid = WOWGUID();
    this->unk_0360 = 0;
    this->m_spellId = 0;
    this->unk_0368 = 0;
    this->unk_036C = 0;
    this->unk_0370 = 0;
    this->unk_03B4 = 0;
    this->unk_03B8 = 0;
    this->unk_03BC = 0;
    this->unk_03C0 = 0;
    this->unk_03C4 = 0;
    this->m_pendingQueries = 0;
    this->m_fading = 0;
    this->m_minimumWidthForce = 0;
    this->unk_04C8 = 0;
    this->unk_04D0 = 0;
    this->unk_04CC = 0;
    this->m_statusBar = nullptr;
}

// VERIFY: c7c9afc0 auto
// OFFSET: 0x0061E0B0
const char* CGTooltip::GetObjectTypeName() {
    return CGTooltip::s_objectTypeName;
}

// VERIFY: e430a47c auto
// OFFSET: 0x0061E0C0
int32_t CGTooltip::GetScriptMetaTable() {
    return CGTooltip::s_metatable;
}

// VERIFY: f9970628 auto
// OFFSET: [0x0061E0D0, 0x0061E160, 0x0061F910]
CGTooltip::~CGTooltip() {
    if (this->m_healthGuid != WOWGUID() && ClntObjMgrGetCurrent()) {
        if (this->m_unitGuid != WOWGUID()) {
            ClntObjMgrUnsetObjMirrorHandler(this->m_healthGuid, ID_UNIT, offsetof(CGUnitData, m_unitFieldHealth), HealthUpdateHandler, this->m_statusBar);
        } else {
            ClntObjMgrUnsetObjMirrorHandler(this->m_healthGuid, ID_GAMEOBJECT, offsetof(CGGameObjectData, m_animProgress), ObjHealthUpdateHandler, this->m_statusBar);
        }
    }
}

// VERIFY: 870e83bb auto
// OFFSET: 0x0061E0E0
bool CGTooltip::IsA(int32_t type) {
    return type == CGTooltip::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: 92061154 auto
// OFFSET: 0x0061E120
bool CGTooltip::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGTooltip::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// VERIFY: 6c7a9cf3 auto
// OFFSET: 0x0061EA00
void CGTooltip::SetOwner(CSimpleFrame* owner, TOOLTIP_ANCHORPOINT anchorType, float offsetX, float offsetY) {
    this->ClearTooltip();
    this->SetFrameAlpha(255);
    this->m_fading = 0;

    if (owner == this->m_owner && anchorType == this->m_anchorType && fabs(offsetX - this->m_offsetX) < 0.00000023841858f && fabs(offsetY - this->m_offsetY) < 0.00000023841858f) {
        return;
    }

    this->m_offsetX = offsetX;
    this->m_owner = owner;
    this->m_offsetY = offsetY;
    this->m_anchorType = ANCHOR_NONE;

    if (owner) {
        this->m_anchorType = anchorType;
    }

    this->ResetPosition(1);
}

// VERIFY: 499594c2 auto
// OFFSET: 0x0061EB20
int32_t CGTooltip::HideThis() {
    this->SetOwner(nullptr, ANCHOR_LEFT, 0.0f, 0.0f);
    return CSimpleFrame::HideThis();
}

// VERIFY: 37802838 manual
// OFFSET: 0x0061FB30
void CGTooltip::PostLoadXML(XMLNode* node, CStatus* status) {
    CSimpleFrame::PostLoadXML(node, status);

    for (int32_t pass = 0; pass < 2; pass++) {
        if (pass == 1) {
            this->m_leftStrings.SetCount(this->m_maxLines);
            this->m_rightStrings.SetCount(this->m_maxLines);
            this->m_lineWrap.SetCount(this->m_maxLines);
        }

        for (this->m_maxLines = 0; ; this->m_maxLines++) {
            char leftName[256];
            char rightName[256];

            const char* name = this->GetName();
            SStrPrintf(leftName, sizeof(leftName), "%sTextLeft%d", name ? name : "<unnamed>", this->m_maxLines + 1);

            name = this->GetName();
            SStrPrintf(rightName, sizeof(rightName), "%sTextRight%d", name ? name : "<unnamed>", this->m_maxLines + 1);

            CSimpleFontString* left = static_cast<CSimpleFontString*>(CScriptObject::GetScriptObjectByName(leftName, CSimpleFontString::GetObjectType()));
            CSimpleFontString* right = static_cast<CSimpleFontString*>(CScriptObject::GetScriptObjectByName(rightName, CSimpleFontString::GetObjectType()));

            if (!left || !right) {
                break;
            }

            if (pass == 1) {
                this->m_leftStrings[this->m_maxLines] = left;
                this->m_rightStrings[this->m_maxLines] = right;
                this->m_lineWrap[this->m_maxLines] = 0;
            }
        }
    }

    char buffer[256];

    for (uint32_t i = 0; i < sizeof(this->m_textures) / sizeof(this->m_textures[0]); i++) {
        const char* name = this->GetName();
        SStrPrintf(buffer, sizeof(buffer), "%sTexture%d", name ? name : "<unnamed>", i + 1);
        this->m_textures[i] = static_cast<CSimpleTexture*>(CScriptObject::GetScriptObjectByName(buffer, CSimpleTexture::GetObjectType()));
    }

    const char* name = this->GetName();
    SStrPrintf(buffer, sizeof(buffer), "%sStatusBar", name ? name : "<unnamed>");
    this->m_statusBar = static_cast<CSimpleStatusBar*>(CScriptObject::GetScriptObjectByName(buffer, CSimpleStatusBar::GetObjectType()));
}

// VERIFY: d942bcf0 manual
// OFFSET: 0x0061E830
void CGTooltip::GetSummonedByString(CGUnit_C* unit, char* buffer, uint32_t size) {
    if (!unit || !buffer) {
        return;
    }

    WOWGUID owner = *unit->GetCharmerOrCreatorGuid();

    if (!owner) {
        return;
    }

    CGUnit_C* ownerUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(owner, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (ownerUnit && *ownerUnit->GetCharmerOrCreatorGuid()) {
        owner = *ownerUnit->GetCharmerOrCreatorGuid();
    }

    char token[1024];
    SpellRec spell;

    if (g_spellDB.GetRecord(unit->m_unit->m_unitCreatedBySpell, &spell)) {
        for (uint32_t i = 0; i < sizeof(spell.m_effect) / sizeof(spell.m_effect[0]); i++) {
            if (spell.m_effect[i] != 28) {
                continue;
            }

            SummonPropertiesRec* properties = g_summonPropertiesDB.GetRecord(spell.m_effectMiscValueB[i]);

            if (properties && properties->m_title != -1) {
                if (properties->m_title) {
                    SStrPrintf(token, sizeof(token), "UNITNAME_SUMMON_TITLE%d", properties->m_title);
                    SStrPrintf(buffer, size, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), GetObjectNameFromGuid(owner));
                }

                return;
            }

            break;
        }
    }

    if (unit->GetCreatureType() == 1) {
        SStrPrintf(token, sizeof(token), "UNITNAME_SUMMON_TITLE1");
    } else {
        SStrPrintf(token, sizeof(token), "UNITNAME_SUMMON_TITLE3");
    }

    SStrPrintf(buffer, size, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), GetObjectNameFromGuid(owner));
}

// VERIFY: 4411ff17 auto
// OFFSET: 0x0061FE30
void CGTooltip::AddFontStrings(CSimpleFontString* left, CSimpleFontString* right) {
    this->m_maxLines++;
    this->m_leftStrings.SetCount(this->m_maxLines);
    this->m_rightStrings.SetCount(this->m_maxLines);
    this->m_lineWrap.SetCount(this->m_maxLines);
    this->m_leftStrings.m_data[this->m_maxLines - 1] = left;
    this->m_rightStrings.m_data[this->m_maxLines - 1] = right;
    this->m_lineWrap.m_data[this->m_maxLines - 1] = 0;
}

// VERIFY: c9a37f57 manual
// OFFSET: 0x0061FEC0
void CGTooltip::AddLine(const char* left, const char* right, const CImVector& leftColor, const CImVector& rightColor, int32_t wrap) {
    if ((!left || !*left) && (!right || !*right)) {
        return;
    }

    if (!this->m_maxLines) {
        return;
    }

    if (this->m_numLines == this->m_maxLines - 1) {
        char name[256];

        const char* tooltipName = this->GetName();
        SStrPrintf(name, sizeof(name), "%sTextLeft%d", tooltipName ? tooltipName : "<unnamed>", this->m_maxLines + 1);
        void* data = CSimpleFontString::s_allocator.GetData(0, typeid(CSimpleFontString).raw_name(), SLOG_OBJECT);
        CSimpleFontString* leftString = data ? new (data) CSimpleFontString(this, 2, true) : nullptr;
        leftString->SetName(name);
        leftString->SetFontObject(this->m_leftStrings.m_data[this->m_maxLines - 1]->m_fontObject);
        CSimpleFontString* previous = this->m_leftStrings.m_data[this->m_maxLines - 1];
        leftString->SetPoint(FRAMEPOINT_TOPLEFT, previous, FRAMEPOINT_BOTTOMLEFT, 0.0f, NDCToDDCWidth(-2.0f / (CoordinateGetAspectCompensation() * 1024.0f)), 0);
        leftString->m_shown = 0;
        leftString->HideThis();

        tooltipName = this->GetName();
        SStrPrintf(name, sizeof(name), "%sTextRight%d", tooltipName ? tooltipName : "<unnamed>", this->m_maxLines + 1);
        data = CSimpleFontString::s_allocator.GetData(0, typeid(CSimpleFontString).raw_name(), SLOG_OBJECT);
        CSimpleFontString* rightString = data ? new (data) CSimpleFontString(this, 2, true) : nullptr;
        rightString->SetName(name);
        rightString->SetFontObject(this->m_rightStrings.m_data[this->m_maxLines - 1]->m_fontObject);
        rightString->SetPoint(FRAMEPOINT_RIGHT, leftString, FRAMEPOINT_LEFT, NDCToDDCWidth(40.0f / (CoordinateGetAspectCompensation() * 1024.0f)), 0.0f, 0);
        rightString->m_shown = 0;
        rightString->HideThis();

        this->AddFontStrings(leftString, rightString);
    }

    if (left && *left) {
        CSimpleFontString* string = this->m_leftStrings.m_data[this->m_numLines];
        string->SetVertexColor(leftColor);
        string->SetText(left, 1);
        this->m_leftStrings.m_data[this->m_numLines]->m_shown = 1;
        this->m_leftStrings.m_data[this->m_numLines]->ShowThis();
    }

    int32_t lineWrap;

    if (right && *right) {
        CSimpleFontString* string = this->m_rightStrings.m_data[this->m_numLines];
        string->SetVertexColor(rightColor);
        string->SetText(right, 1);
        this->m_rightStrings.m_data[this->m_numLines]->m_shown = 1;
        this->m_rightStrings.m_data[this->m_numLines]->ShowThis();
        lineWrap = 0;
    } else {
        lineWrap = wrap;
    }

    this->m_lineWrap.m_data[this->m_numLines++] = lineWrap;
}

// VERIFY: caaae9c0 manual
// OFFSET: 0x00621070
int32_t CGTooltip::SetUnit(const WOWGUID& guid, int32_t showHealth, const WOWGUID& healthGuid, int32_t a4) {
    const CImVector& titleColor = *reinterpret_cast<const CImVector*>(&s_tooltipTitleColor);
    const CImVector& textColor = *reinterpret_cast<const CImVector*>(&s_tooltipTextColor);
    const CImVector& greenColor = *reinterpret_cast<const CImVector*>(&s_tooltipGreenColor);

    this->ClearTooltip();

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!healthGuid && unit && unit->HasAuraEffect(279)) {
        SpellRec spell;

        for (uint32_t i = 0; i < unit->GetAuraCount(); i++) {
            AuraData* aura = unit->GetAura(i);

            if (!g_spellDB.GetRecord(aura->m_spellId, &spell)) {
                continue;
            }

            for (uint32_t j = 0; j < sizeof(spell.m_effectAura) / sizeof(spell.m_effectAura[0]); j++) {
                if (spell.m_effectAura[j] == 279 && ((1 << j) & aura->m_flags)) {
                    return this->SetUnit(aura->m_creator, showHealth, guid, 0);
                }
            }
        }
    }

    CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(guid);
    CGPartyPetState* petState = CGPartyMemberStateRepository::FindPetState(guid);
    WOWGUID none;
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, none, nullptr, nullptr, false);
    CreatureStats_C* creature = nullptr;

    if (unit) {
        creature = unit->m_creatureCacheEntry;
    } else if (!state && !petState) {
        uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);
        WOWGUID requester;

        if (((high & 0xF0F00000) != 0xF0300000 && (high & 0xF0F00000) != 0xF0500000) || !(creature = g_creatureDBCache.GetRecord((guid.m_guid >> 24) & 0xFFFFFFF, requester, nullptr, nullptr, false))) {
            if (!name) {
                this->Hide();
                return 0;
            }
        }

        showHealth = 0;
    }

    this->m_unitGuid = guid;

    if (this->m_statusBar && showHealth) {
        this->m_healthGuid = healthGuid ? healthGuid : this->m_unitGuid;
        CGUnit_C* healthUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_healthGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
        ClntObjMgrSetObjMirrorHandler(this->m_healthGuid, ID_UNIT, offsetof(CGUnitData, m_unitFieldHealth), sizeof(int32_t), HealthUpdateHandler, this->m_statusBar, static_cast<HANDLER_PRIORITY>(0), 0);

        float maxHealth;

        if (healthUnit) {
            maxHealth = static_cast<float>(healthUnit->m_unit->m_unitFieldMaxhealth);
        } else if (state) {
            maxHealth = static_cast<float>(state->m_maxHealth);
        } else {
            maxHealth = static_cast<float>(petState->m_maxHealth);
        }

        this->m_statusBar->SetMinMaxValues(0.0f, maxHealth);

        float health;

        if (healthUnit) {
            health = (healthUnit->m_unit->m_unitDynamicFlags & 0x20) ? 0.0f : static_cast<float>(healthUnit->m_unit->m_unitFieldHealth);
        } else if (state) {
            health = static_cast<float>(state->m_health);
        } else {
            health = static_cast<float>(petState->m_health);
        }

        this->m_statusBar->SetValue(health);
        this->m_statusBar->Show();
    }

    char line[1024];
    char title[1024];

    if (unit) {
        const char* realm = nullptr;
        unit->GetUnitName(&realm, 1);

        if (realm && *realm) {
            SStrPrintf(title, sizeof(title), "%s - %s", unit->GetUnitChosenTitle(line, sizeof(line), 1, nullptr), realm);
        } else {
            SStrPrintf(title, sizeof(title), unit->GetUnitChosenTitle(line, sizeof(line), 1, nullptr));
        }

        this->AddLine(title, nullptr, titleColor, titleColor, 0);

        if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
            uint32_t guildId = static_cast<CGPlayer_C*>(unit)->m_player->m_playerGuildid;

            if (guildId) {
                WOWGUID requester = unit->m_obj->m_guid;
                GuildStats_C* guild = g_guildInfoCache.GetRecord(guildId, requester, TooltipUnitNameCallback, this, false);

                if (guild) {
                    this->AddLine(guild->m_name, nullptr, textColor, textColor, 0);
                }
            }
        }
    } else if (name) {
        if (name->m_realmName[0]) {
            SStrPrintf(line, sizeof(line), "%s - %s", name->m_name, name->m_realmName);
            this->AddLine(line, nullptr, titleColor, titleColor, 0);
        } else {
            this->AddLine(name->m_name, nullptr, titleColor, titleColor, 0);
        }
    } else if (petState) {
        this->AddLine(reinterpret_cast<const char*>(&petState->unk_0008[0]), nullptr, titleColor, titleColor, 0);
    } else if (creature) {
        this->AddLine(creature->m_name[0], nullptr, titleColor, titleColor, 0);
    }

    CVar* colorblind = CVar::Lookup("colorblindMode");
    CGPlayer_C* player = nullptr;
    const char* subName = nullptr;

    if (colorblind && colorblind->m_intValue && (player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) != nullptr) {
        if (unit) {
            line[0] = 0;
            FRAMESCRIPT_GENDER gender = player->m_unit->m_unitFieldBytes0[2] == 1 ? GENDER_FEMALE : GENDER_MALE;
            SStrPrintf(line, sizeof(line), "FACTION_STANDING_LABEL%d", unit->UnitReaction(player) + 1);
            this->AddLine(FrameScript_GetText(line, 0, gender), nullptr, textColor, textColor, 0);
        }
    }

    if (!subName) {
        if (unit && !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            subName = unit->GetUnitTitle();
        } else if (creature) {
            subName = creature->m_subName;
        }
    }

    if (subName && *subName) {
        this->AddLine(subName, nullptr, textColor, textColor, 0);
    }

    line[0] = 0;
    CGTooltip::GetSummonedByString(unit, line, sizeof(line));

    if (line[0]) {
        this->AddLine(line, nullptr, textColor, textColor, 0);
    }

    char race[128] = {};
    char className[128] = {};
    char type[64] = {};
    char level[32];

    CGPlayer_C* activePlayer = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t dead = 0;
    CreatureTypeRec* creatureType = nullptr;
    int32_t unitLevel = 0;
    int32_t showLevel = 1;

    if (unit) {
        dead = unit->m_unit->m_unitFieldHealth <= 0 || (unit->m_unit->m_unitDynamicFlags & 0x20);
        unitLevel = unit->m_unit->m_unitFieldLevel;

        if (unit->GetCreatureType() != 10) {
            if (activePlayer) {
                if (activePlayer->UnitReaction(unit) < 4 && !activePlayer->IsTreatAsRaidUnit()) {
                    creatureType = g_creatureTypeDB.GetRecord(unit->GetCreatureType());
                }
            }
        }

        if (unit->GetCreatureType() == 10 || activePlayer) {
            if (activePlayer && unit->UnitReaction(activePlayer) <= 1 && static_cast<int32_t>(activePlayer->m_unit->m_unitFieldLevel) <= unitLevel - 10) {
                showLevel = 0;
            }
        }

        if (showLevel && unit->IsBossMob()) {
            showLevel = 0;
        }
    } else if (state) {
        dead = (state->m_flags & 0x1) && state->m_health <= 0;
        unitLevel = reinterpret_cast<int32_t*>(state)[12];
    } else {
        showLevel = 0;

        if (petState) {
            dead = petState->m_health <= 0;
        } else if (creature && creature->m_type != 10) {
            creatureType = g_creatureTypeDB.GetRecord(creature->m_type);
        }
    }

    if (showLevel && unitLevel > 0) {
        int32_t displayLevel = unitLevel;

        if (activePlayer && ((activePlayer->m_player->m_playerBytes3[1]) || activePlayer->m_player->m_playerFakeInebriation) && unit && activePlayer->CanAttack(unit)) {
            displayLevel = unitLevel - RoundToInt64(activePlayer->GetDrunknessTurnValue() * 5.0f);

            if (displayLevel < 1) {
                displayLevel = 1;
            }
        }

        if (displayLevel > 0) {
            SStrPrintf(level, sizeof(level), "%d", displayLevel);
        } else {
            SStrCopy(level, "??", sizeof(level));
        }
    } else {
        SStrCopy(level, "??", sizeof(level));
    }

    if (dead) {
        SStrCopy(className, FrameScript_GetText("CORPSE", -1, GENDER_NOT_APPLICABLE), sizeof(className));
    } else if ((unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) || name) {
        const char* raceName = CGUnit_C::GetDisplayRaceName(unit, name);
        const char* classNameText = CGUnit_C::GetDisplayClassName(unit, name);

        if (raceName && classNameText) {
            SStrCopy(race, raceName, sizeof(race));
            SStrCopy(className, classNameText, sizeof(className));
        }
    } else if (creatureType) {
        SStrCopy(className, creatureType->m_name, sizeof(className));
    }

    const char* classification = nullptr;

    if (unit) {
        classification = unit->IsBossMob() ? "BOSS" : s_classificationTokens[unit->GetCreatureClassification()];

        if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
            SStrCopy(type, FrameScript_GetText("PLAYER", -1, GENDER_NOT_APPLICABLE), sizeof(type));
        } else if (name) {
            SStrCopy(type, FrameScript_GetText("PLAYER", -1, GENDER_NOT_APPLICABLE), sizeof(type));
        } else if (classification) {
            SStrCopy(type, FrameScript_GetText(classification, -1, GENDER_NOT_APPLICABLE), sizeof(type));
        }
    } else {
        if (creature) {
            classification = (creature->m_typeFlags & 0x4) ? "BOSS" : s_classificationTokens[creature->m_rank];
        }

        if (name) {
            SStrCopy(type, FrameScript_GetText("PLAYER", -1, GENDER_NOT_APPLICABLE), sizeof(type));
        } else if (classification) {
            SStrCopy(type, FrameScript_GetText(classification, -1, GENDER_NOT_APPLICABLE), sizeof(type));
        }
    }

    if (race[0] && className[0]) {
        if (type[0]) {
            SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL_RACE_CLASS_TYPE", -1, GENDER_NOT_APPLICABLE), level, race, className, type);
        } else {
            SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL_RACE_CLASS", -1, GENDER_NOT_APPLICABLE), level, race, className);
        }
    } else if (className[0]) {
        if (type[0]) {
            SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL_CLASS_TYPE", -1, GENDER_NOT_APPLICABLE), level, className, type);
        } else {
            SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL_CLASS", -1, GENDER_NOT_APPLICABLE), level, className);
        }
    } else if (type[0]) {
        SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL_TYPE", -1, GENDER_NOT_APPLICABLE), level, type);
    } else {
        SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL", -1, GENDER_NOT_APPLICABLE), level);
    }

    this->AddLine(line, nullptr, textColor, textColor, 0);

    if (!unit && state && !(state->m_flags & 0x1) && !a4) {
        this->AddLine(FrameScript_GetText("PLAYER_OFFLINE", -1, GENDER_NOT_APPLICABLE), nullptr, textColor, textColor, 0);

        if (this->m_onTooltipSetUnit.m_luaRef) {
            this->RunScript(this->m_onTooltipSetUnit, 0, nullptr);
        }

        this->Show();
        return 0;
    }

    if (activePlayer && unit && unit->ShowFactionTooltip()) {
        FactionTemplateRec* factionTemplate = g_factionTemplateDB.GetRecord(unit->m_unit->m_unitFieldFactiontemplate);
        FactionRec* faction = factionTemplate ? g_factionDB.GetRecord(factionTemplate->m_faction) : nullptr;

        if (faction && faction->m_reputationIndex >= 0) {
            for (uint32_t i = 0; i < 4; i++) {
                uint32_t raceMask = faction->m_reputationRaceMask[i];
                uint32_t classMask = faction->m_reputationClassMask[i];

                if ((!raceMask || ((1 << (activePlayer->m_unit->m_unitFieldBytes0[0] - 1)) & raceMask)) && (raceMask || classMask) && (!classMask || ((1 << (activePlayer->m_unit->m_unitFieldBytes0[1] - 1)) & classMask))) {
                    if (!(faction->m_reputationFlags[i] & 0x4)) {
                        this->AddLine(faction->m_name, nullptr, textColor, textColor, 0);
                    }

                    break;
                }
            }
        }
    }

    if (unit) {
        if (((unit->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(unit)->m_player->m_playerFlags & 0x100)) || (unit->m_unit->m_unitFieldBytes2[1] & 0x1)) {
            this->AddLine(FrameScript_GetText("PVP_ENABLED", -1, GENDER_NOT_APPLICABLE), nullptr, textColor, textColor, 0);
        }

        if (unit->m_unit->m_unitFieldHealth <= 0 && (unit->m_obj->m_type & TYPEMASK_PLAYER) && activePlayer && activePlayer->CanAssist(unit, false)) {
            this->AddLine(FrameScript_GetText("RESURRECTABLE", -1, GENDER_NOT_APPLICABLE), nullptr, greenColor, greenColor, 0);
        }

        if (unit->m_unit->m_unitDynamicFlags & 0x10) {
            this->AddEmpathyText(unit, line, sizeof(line));
        }

        if ((unit->m_unit->m_unitFieldFlags & 0x4000000) && !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            this->AddSkinnableText(unit, line, sizeof(line));
        }
    }

    if (CGGameUI::IsPartyMember(guid) && guid != ClntObjMgrGetActivePlayer() && !unit) {
        if (state && AreaListGetName(reinterpret_cast<int32_t*>(state)[13], line, sizeof(line), 0)) {
            this->AddLine(line, nullptr, textColor, textColor, 0);
        }
    } else if (unit && unit->IsPVPLeader()) {
        this->AddLine(FrameScript_GetText("PVP_RANK_LEADER", -1, GENDER_NOT_APPLICABLE), nullptr, textColor, textColor, 0);
    }

    CVar* questTracking = CVar::Lookup("showQuestTrackingTooltips");

    if (creature && questTracking && questTracking->m_intValue) {
        uint32_t summonedHigh = unit ? static_cast<uint32_t>(unit->m_unit->m_unitFieldSummonedby.m_guid >> 32) : 0;

        if (!unit || (summonedHigh & 0xF0000000) || !((summonedHigh & 0xF07FFFFF) | static_cast<uint32_t>(unit->m_unit->m_unitFieldSummonedby.m_guid))) {
            uint32_t entry = unit ? unit->m_obj->m_entryID : static_cast<uint32_t>((guid.m_guid >> 24) & 0xFFFFFFF);
            QuestLogLeaderBoardParams params;
            params.unk_0010 = reinterpret_cast<uint32_t>(TooltipUnitNameCallback);
            params.unk_0014 = reinterpret_cast<uint32_t>(TooltipUnitNameCallback);
            params.unk_0018 = reinterpret_cast<uint32_t>(TooltipUnitNameCallback);
            params.unk_001C = reinterpret_cast<uint32_t>(TooltipUnitNameCallback);
            params.unk_0020 = reinterpret_cast<uint32_t>(this);
            const char* objective = reinterpret_cast<const char*>(params.unk_0024);
            char buffer[4099];

            for (uint32_t i = 0; i < CGQuestLog::m_numQuest; i++) {
                WOWGUID requester;
                QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::GetQuestAt(i), requester, TooltipUnitNameCallback, this, false);

                if (!quest) {
                    continue;
                }

                bool header = false;

                if (Script_GetQuestLogLeaderBoard(quest->m_id, entry, &params)) {
                    this->AddLine(quest->m_name, nullptr, titleColor, titleColor, 0);
                    header = true;
                    SStrPrintf(buffer, sizeof(buffer), " - %s", objective);
                    this->AddLine(buffer, nullptr, textColor, textColor, 0);
                }

                for (uint32_t j = 0; j < sizeof(creature->m_killCredit) / sizeof(creature->m_killCredit[0]); j++) {
                    if (creature->m_killCredit[j] && Script_GetQuestLogLeaderBoard(quest->m_id, creature->m_killCredit[j], &params)) {
                        if (!header) {
                            this->AddLine(quest->m_name, nullptr, titleColor, titleColor, 0);
                            header = true;
                        }

                        SStrPrintf(buffer, sizeof(buffer), " - %s", objective);
                        this->AddLine(buffer, nullptr, textColor, textColor, 0);
                    }
                }

                for (uint32_t j = 0; j < sizeof(creature->m_questItem) / sizeof(creature->m_questItem[0]); j++) {
                    int32_t itemId = creature->m_questItem[j];

                    if (!itemId) {
                        break;
                    }

                    if (CGQuestLog::GetQuestLeaderBoardForItem(quest->m_id, itemId, &params)) {
                        if (!header) {
                            this->AddLine(quest->m_name, nullptr, titleColor, titleColor, 0);
                            header = true;
                        }

                        SStrPrintf(buffer, sizeof(buffer), " - %s", objective);
                        this->AddLine(buffer, nullptr, textColor, textColor, 0);
                    }

                    if (!CGQuestLog::IsQuestCompleteEx(i, 1)) {
                        for (uint32_t k = 0; k < sizeof(quest->m_intermediateItemId) / sizeof(quest->m_intermediateItemId[0]); k++) {
                            if (quest->m_intermediateItemId[k] != static_cast<uint32_t>(itemId)) {
                                continue;
                            }

                            if (!header) {
                                this->AddLine(quest->m_name, nullptr, titleColor, titleColor, 0);
                                header = true;
                            }

                            WOWGUID itemRequester;
                            ItemStats_C* stats = g_itemDBCache.GetRecord(quest->m_intermediateItemId[k], itemRequester, TooltipUnitNameCallback, this, false);

                            if (stats) {
                                SStrPrintf(buffer, sizeof(buffer), " - %s", stats->m_name[0]);
                                this->AddLine(buffer, nullptr, textColor, textColor, 0);
                            }
                        }
                    }
                }
            }
        }
    }

    if (unit && dead) {
        if (unit->GetMasterLooter()) {
            WOWGUID requester;
            NameCache* looter = g_nameDBCache.GetRecord(unit->GetMasterLooter(), requester, TooltipUnitNameCallback, this, false);

            if (looter) {
                char buffer[64];
                SStrPrintf(buffer, sizeof(buffer), "%s: %s", FrameScript_GetText("MASTER_LOOTER", -1, GENDER_NOT_APPLICABLE), looter->m_name);
                this->AddLine(buffer, nullptr, textColor, textColor, 0);
            }
        }

        if (unit->GetLooter()) {
            WOWGUID requester;
            NameCache* looter = g_nameDBCache.GetRecord(unit->GetLooter(), requester, TooltipUnitNameCallback, this, false);

            if (looter) {
                char buffer[64];
                SStrPrintf(buffer, sizeof(buffer), "%s: %s", FrameScript_GetText("LOOT", -1, GENDER_NOT_APPLICABLE), looter->m_name);
                this->AddLine(buffer, nullptr, textColor, textColor, 0);
            }
        }
    }

    int32_t result = 0;

    if (activePlayer && CGGameUI::m_threatWarningEnabled && unit) {
        WOWGUID playerGuid = activePlayer->m_obj->m_guid;
        uint8_t status;
        float percent;

        if (unit->CalculateThreat(playerGuid, &status, nullptr, &percent, nullptr)) {
            SStrPrintf(line, sizeof(line), FrameScript_GetText("THREAT_TOOLTIP", -1, GENDER_NOT_APPLICABLE), static_cast<int32_t>(percent - 0.5f));
            this->AddLine(line, nullptr, textColor, textColor, 0);

            uint32_t threatStatus = status >= 5 ? 1 : status;
            CAaRect coords = { 0.0f, 0.0f, 1.0f, 1.0f };
            this->AddTexture("Interface\\RaidFrame\\UI-RaidFrame-Threat", coords, *reinterpret_cast<const CImVector*>(&s_threatStatusColors[threatStatus]));
            result = 1;
        }
    }

    if (this->m_onTooltipSetUnit.m_luaRef) {
        this->RunScript(this->m_onTooltipSetUnit, 0, nullptr);
    }

    this->Show();
    return result;
}

// VERIFY: 1a6dfa97 manual
// OFFSET: 0x00622410
void CGTooltip::SetCorpse(const WOWGUID& guid) {
    const CImVector& textColor = *reinterpret_cast<const CImVector*>(&s_tooltipTextColor);

    this->ClearTooltip();

    CGCorpse_C* corpse = static_cast<CGCorpse_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_CORPSE, __FILE__, __LINE__));

    if (!corpse) {
        this->Hide();
        return;
    }

    *reinterpret_cast<WOWGUID*>(&this->unk_0348) = guid;
    *reinterpret_cast<WOWGUID*>(&this->unk_0350) = corpse->m_corpse->m_corpseFieldOwner;
    NameCache* name = g_nameDBCache.GetRecord(reinterpret_cast<WOWGUID*>(&this->unk_0350)->m_guid, *reinterpret_cast<WOWGUID*>(&this->unk_0350), TooltipCorpseCallback, this, true);

    if (name) {
        char line[1024];
        SStrPrintf(line, sizeof(line), FrameScript_GetText("CORPSE_TOOLTIP", -1, GENDER_NOT_APPLICABLE), name->m_name);
        this->AddLine(line, nullptr, textColor, textColor, 0);
        this->Show();
    }
}

// VERIFY: cbeee45f manual
// OFFSET: 0x0061E540
const char* CGTooltip::GetQuestDifficultyColorString(int32_t level) {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return s_questDifficultyColorStrings[2];
    }

    int32_t delta = level - static_cast<int32_t>(player->m_unit->m_unitFieldLevel);

    if (delta >= 5) {
        return s_questDifficultyColorStrings[0];
    }

    if (delta >= 3) {
        return s_questDifficultyColorStrings[1];
    }

    if (delta >= -2) {
        return s_questDifficultyColorStrings[2];
    }

    if (-delta <= CGQuestLog::GetQuestGreenRange()) {
        return s_questDifficultyColorStrings[3];
    }

    return s_questDifficultyColorStrings[4];
}

// VERIFY: a380ef23 auto
// OFFSET: 0x0061DDD0
void TooltipUnitNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(param);

    if (tooltip->m_unitGuid) {
        WOWGUID unitGuid = tooltip->m_unitGuid;
        WOWGUID healthGuid = {};
        tooltip->SetUnit(unitGuid, 1, healthGuid, 0);
        tooltip->CalculateSize();
    }
}

// VERIFY: 456c6e01 auto
// OFFSET: 0x0061EAB0
void CGTooltip::AppendText(const char* text) {
    if (!this->m_numLines) {
        return;
    }

    const char* current = this->m_leftStrings.m_data[0]->m_text;

    if (!current || !*current) {
        current = nullptr;
    }

    char buffer[256];
    SStrPrintf(buffer, sizeof(buffer), "%s%s", current, text);
    this->m_leftStrings.m_data[0]->SetText(buffer, 1);
    this->CalculateSize();
}

// VERIFY: 8fe663a0 manual
// OFFSET: 0x0061E290
const char* CGTooltip::CreateItemLink(int32_t itemId, uint32_t quality, int32_t enchant, const int32_t* gems, int32_t randomPropertiesId, int32_t seed) {
    uint32_t level = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        level = player->m_unit->m_unitFieldLevel;
    }

    if (quality >= sizeof(s_itemQualityColorStrings) / sizeof(s_itemQualityColorStrings[0])) {
        quality = 1;
    }

    const char* color = s_itemQualityColorStrings[quality];
    const char* reset = *color ? "|r" : "";
    char name[256];
    CGItem_C::BuildItemName(name, sizeof(name), itemId, randomPropertiesId);
    SStrPrintf(s_itemLink, sizeof(s_itemLink), "%s|Hitem:%d:%d:%d:%d:%d:%d:%d:%d:%d|h[%s]|h%s", color, itemId, enchant, gems[0], gems[1], gems[2], 0, randomPropertiesId, seed, level, name, reset);
    return s_itemLink;
}

// VERIFY: ffe3f14b manual
// OFFSET: 0x0061E3A0
const char* CGTooltip::GetItemHyperlinkString(CGItem_C* item) {
    if (!item) {
        return nullptr;
    }

    ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, WOWGUID(), nullptr, nullptr, false);
    uint32_t wrapped = (item->m_item->m_itemFieldFlags >> 3) & 1;
    int32_t gems[3];

    for (uint32_t i = 0; i < 3; i++) {
        if (!wrapped && !(item->GetItemStaticFlags(0) & 0x2000)) {
            gems[i] = item->m_item->m_itemFieldEnchantment[2 + i].m_id;
        } else {
            gems[i] = 0;
        }
    }

    int32_t enchant = 0;
    int32_t randomPropertiesId;
    int32_t seed;

    if (wrapped) {
        seed = 0;
        randomPropertiesId = 0;
    } else {
        randomPropertiesId = item->m_item->m_itemFieldRandomPropertiesId;
        seed = item->m_item->m_itemFieldPropertySeed;
        enchant = item->GetItemStaticFlags(0) & 0x2000 ? 0 : item->m_item->m_itemFieldEnchantment[0].m_id;
    }

    return CGTooltip::CreateItemLink(item->m_obj->m_entryID, stats ? stats->m_quality : 1, enchant, gems, randomPropertiesId, seed);
}

// VERIFY: 55852bae auto
// OFFSET: 0x0061A190
int32_t FormatString(const char* token, int32_t count, FRAMESCRIPT_GENDER gender, char* buffer, uint32_t size) {
    const char* text = FrameScript_GetText(token, count, gender);
    SStrCopy(buffer, text, size);
    return *text != '\0';
}

// VERIFY: e0b2f2ad auto
// OFFSET: 0x0061A960
int32_t TimeFractionBelow(float time, float fraction) {
    double value;

    if (time >= 86400000.0f) {
        value = time * 1.1574074e-8f;
    } else if (time >= 3600000.0f) {
        value = time * 2.7777779e-7f;
    } else if (time >= 60000.0f) {
        value = time * 1.6666667e-5f;
    } else {
        value = time * 0.001f;
    }

    float scaled = static_cast<float>(value);
    return scaled - floor(value) < fraction;
}

// VERIFY: 49b20620 manual
// OFFSET: 0x0061A9E0
void CGTooltip::GetDurationString(char* buffer, uint32_t size, uint64_t time, const char* token, const char* prefix, int32_t roundUp, int32_t seconds) {
    if (!buffer || !token) {
        return;
    }

    uint32_t minute = seconds ? 60 : 60000;
    uint32_t count = static_cast<uint32_t>(time);
    char format[256];
    *buffer = '\0';

    if (time >= 1440 * minute) {
        count = static_cast<uint32_t>(roundUp ? (time - 1) / (1440 * minute) + 1 : time / (1440 * minute));
        SStrPrintf(format, sizeof(format), "%s_DAYS", token);
    } else if (time >= 60 * minute) {
        uint64_t hours = roundUp ? (time - 1) / (60 * minute) + 1 : time / (60 * minute);
        count = static_cast<uint32_t>(hours);

        if (hours == 24) {
            count = 1;
            SStrPrintf(format, sizeof(format), "%s_DAYS", token);
        } else {
            SStrPrintf(format, sizeof(format), "%s_HOURS", token);
        }
    } else if (time >= minute) {
        uint64_t minutes = roundUp ? (time - 1) / minute + 1 : time / minute;
        count = static_cast<uint32_t>(minutes);

        if (minutes == 60) {
            count = 1;
            SStrPrintf(format, sizeof(format), "%s_HOURS", token);
        } else {
            SStrPrintf(format, sizeof(format), "%s_MIN", token);
        }
    } else {
        if (!seconds) {
            count = static_cast<uint32_t>(time / 1000);
        }

        SStrPrintf(format, sizeof(format), "%s_SEC", token);
    }

    if (prefix) {
        SStrPrintf(buffer, size, FrameScript_GetText(format, -1, GENDER_NOT_APPLICABLE), prefix, count);
    } else {
        SStrPrintf(buffer, size, FrameScript_GetText(format, -1, GENDER_NOT_APPLICABLE), count);
    }
}

// VERIFY: 2e33ff12 manual
// OFFSET: 0x0061AEE0
void CGTooltip::GetFloatDurationString(char* buffer, uint32_t size, float time, const char* token, const char* prefix) {
    static float s_msPerHour = 3600000.0f;
    static float s_msPerDay = 24.0f * s_msPerHour;

    if (!buffer || !token) {
        return;
    }

    char format[256];
    float value;
    *buffer = '\0';

    if (time >= s_msPerDay) {
        value = time / s_msPerDay;
        SStrPrintf(format, sizeof(format), "%s_DAYS", token);
    } else if (time >= s_msPerHour) {
        value = time / s_msPerHour;
        SStrPrintf(format, sizeof(format), "%s_HOURS", token);
    } else if (time >= 60000.0f) {
        value = time * 1.6666667e-5f;
        SStrPrintf(format, sizeof(format), "%s_MIN", token);
    } else {
        value = time * 0.001f;
        SStrPrintf(format, sizeof(format), "%s_SEC", token);
    }

    if (!(value > 0.0f)) {
        value = 0.0f;
    }

    if (prefix) {
        SStrPrintf(buffer, size, FrameScript_GetText(format, -1, GENDER_NOT_APPLICABLE), prefix, value);
    } else {
        SStrPrintf(buffer, size, FrameScript_GetText(format, -1, GENDER_NOT_APPLICABLE), value);
    }
}

// VERIFY: 21c050c5 auto
// OFFSET: 0x0061DD60
void TooltipSpellCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(param);

    if (tooltip->m_pendingQueries) {
        tooltip->m_pendingQueries--;
    }

    if (tooltip->m_pendingQueries <= 0 && tooltip->m_spellId) {
        tooltip->SetSpell(tooltip->m_spellId, 0, 0, 0, 0, 0, 0, nullptr, nullptr, 0, 0, 0, -1, -1, 0);
        tooltip->CalculateSize();
    }
}

// VERIFY: cfbc31fa manual
// OFFSET: 0x006224F0
int32_t CGTooltip::AddTalentPrereqs(const TalentRec* talent, CGTalentGroup_C* group, int32_t isInspect, int32_t isPet, int32_t usePreview) {
    const CImVector& redColor = *reinterpret_cast<const CImVector*>(&s_tooltipRedColor);
    char text[256];

    if (!talent) {
        return 0;
    }

    int32_t met = 1;

    for (uint32_t i = 0; i < 3; i++) {
        int32_t prereqId = talent->m_prereqTalent[i];

        if (!prereqId) {
            continue;
        }

        TalentRec* prereq = g_talentDB.GetRecord(prereqId);

        if (!prereq) {
            continue;
        }

        int32_t rank = talent->m_prereqRank[i];
        CGTalentItem* item;

        if (group && (item = group->GetTalentItem(prereqId)) && (usePreview ? item->m_previewRank : item->m_rank) >= rank) {
            continue;
        }

        for (uint32_t r = rank; r < 9; r++) {
            int32_t spellId = prereq->m_spellRank[r];

            if (spellId) {
                met = 0;
                SpellRec spell;

                if (g_spellDB.GetRecord(spellId, &spell)) {
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("TOOLTIP_TALENT_PREREQ", -1, GENDER_NOT_APPLICABLE), rank + 1, spell.m_name);
                    this->AddLine(text, nullptr, redColor, redColor, 0);
                }

                break;
            }
        }
    }

    if (group && talent->m_tierID > 0) {
        TalentTabRec* tab = g_talentTabDB.GetRecord(talent->m_tabID);

        if (tab) {
            TalentGroupTab* groupTab = group->GetTabFromID(talent->m_tabID);
            uint32_t spent = groupTab ? groupTab->m_pointsSpent + groupTab->m_previewPointsSpent : 0;

            if (spent < static_cast<uint32_t>(talent->m_tierID * (2 * (isPet == 0) + 3))) {
                SStrPrintf(text, sizeof(text), FrameScript_GetText("TOOLTIP_TALENT_TIER_POINTS", -1, GENDER_NOT_APPLICABLE), talent->m_tierID * (2 * (isPet == 0) + 3), tab->m_name);
                this->AddLine(text, nullptr, redColor, redColor, 0);
                met = 0;
            }
        }
    }

    if (talent->m_requiredSpellID && !TalentInfoIsSpellKnown(talent->m_requiredSpellID, isInspect, isPet)) {
        SpellRec spell;

        if (g_spellDB.GetRecord(talent->m_requiredSpellID, &spell)) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE), spell.m_name);
            this->AddLine(text, nullptr, redColor, redColor, 0);
            met = 0;
        }
    }

    return met;
}

// VERIFY: 5d06b108 manual
// OFFSET: 0x00622800
void CGTooltip::AddTalentLearnText(const TalentRec* talent, CGTalentGroup_C* group, int32_t prereqsMet, int32_t rank, int32_t maxRank, int32_t a7, int32_t a8, int32_t isPet, int32_t usePreview) {
    const CImVector& redColor = *reinterpret_cast<const CImVector*>(&s_tooltipRedColor);
    const CImVector& greenColor = *reinterpret_cast<const CImVector*>(&s_tooltipGreenColor);

    if (!talent || !ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    uint32_t activeGroup = CGTalentInfo::GetActiveTalentGroup(isPet);

    if (group != CGTalentInfo::GetTalentGroup(activeGroup, isPet)) {
        return;
    }

    if (usePreview) {
        if (prereqsMet && !a7 && !a8) {
            int32_t count;

            if (group->CanAddPreviewTalentPoints(talent, 1, &count, isPet)) {
                this->AddLine(FrameScript_GetText("TALENT_TOOLTIP_ADDPREVIEWPOINT", -1, GENDER_NOT_APPLICABLE), nullptr, greenColor, greenColor, 0);
            }

            if (group->CanAddPreviewTalentPoints(talent, -1, &count, isPet)) {
                this->AddLine(FrameScript_GetText("TALENT_TOOLTIP_REMOVEPREVIEWPOINT", -1, GENDER_NOT_APPLICABLE), nullptr, redColor, redColor, 0);
            }
        }
    } else {
        uint32_t unspent = CGTalentInfo::GetUnspentTalentPoints(activeGroup, isPet);

        if (prereqsMet && !a7 && !a8 && unspent && rank != maxRank) {
            this->AddLine(FrameScript_GetText("TOOLTIP_TALENT_LEARN", -1, GENDER_NOT_APPLICABLE), nullptr, greenColor, greenColor, 0);
        }
    }
}

// INLINED
static int32_t SetSpellPowerCost(char* cost, const SpellRec* spell, const CGUnit_C* caster, int32_t powerCost, const PowerDisplayRec* display) {
    char format[128];
    int32_t costPerSecond = Spell_C_GetPowerCostPerSecond(spell, caster);
    powerCost = static_cast<uint32_t>(powerCost) / Spell_C_GetPowerDisplayMod(spell->m_powerType);
    costPerSecond = static_cast<uint32_t>(costPerSecond) / Spell_C_GetPowerDisplayMod(spell->m_powerType);
    const char* token = static_cast<uint32_t>(spell->m_powerType) > 6 ? "HEALTH_COST" : s_powerCostTokens[spell->m_powerType];

    if (spell->m_powerType == 5) {
        SpellRuneCostRec* runes = g_spellRuneCostDB.GetRecord(spell->m_runeCostID);

        if (!runes) {
            return powerCost;
        }

        if (runes->m_blood) {
            SStrPrintf(format, sizeof(format), FrameScript_GetText("RUNE_COST_BLOOD", -1, GENDER_NOT_APPLICABLE), runes->m_blood);
            SStrPack(cost, format, STORM_MAX_STR);

            if (runes->m_unholy || runes->m_frost) {
                SStrPack(cost, " ", STORM_MAX_STR);
            }
        }

        if (runes->m_unholy) {
            SStrPrintf(format, sizeof(format), FrameScript_GetText("RUNE_COST_UNHOLY", -1, GENDER_NOT_APPLICABLE), runes->m_unholy);
            SStrPack(cost, format, STORM_MAX_STR);

            if (runes->m_frost) {
                SStrPack(cost, " ", STORM_MAX_STR);
            }
        }

        if (runes->m_frost) {
            SStrPrintf(format, sizeof(format), FrameScript_GetText("RUNE_COST_FROST", -1, GENDER_NOT_APPLICABLE), runes->m_frost);
            SStrPack(cost, format, STORM_MAX_STR);
        }

        return powerCost;
    }

    if (powerCost > 0 && costPerSecond <= 0) {
        if (display) {
            SStrCopy(format, FrameScript_GetText(display->m_globalStringBaseTag, -1, GENDER_NOT_APPLICABLE), sizeof(format));
            SStrPrintf(cost, 128, FrameScript_GetText("POWER_DISPLAY_COST", -1, GENDER_NOT_APPLICABLE), powerCost, format);
        } else {
            SStrCopy(format, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), sizeof(format));
            SStrPrintf(cost, 128, format, powerCost);
        }
    } else if (costPerSecond > 0) {
        if (display) {
            SStrCopy(format, FrameScript_GetText(display->m_globalStringBaseTag, -1, GENDER_NOT_APPLICABLE), sizeof(format));
            SStrPrintf(cost, 128, FrameScript_GetText("POWER_DISPLAY_COST_PER_TIME", -1, GENDER_NOT_APPLICABLE), powerCost, format, costPerSecond);
        } else {
            SStrPrintf(format, sizeof(format), "%s_PER_TIME", token);
            SStrPrintf(cost, 128, FrameScript_GetText(format, -1, GENDER_NOT_APPLICABLE), powerCost, costPerSecond);
        }
    }

    return powerCost;
}

// INLINED
static void FormatSpellRange(char* range, float minRange, float maxRange, float enemyMinRange, const char* side) {
    char format[128];
    char distance[32];

    if (maxRange >= 50000.0f) {
        SStrPrintf(range, 128, FrameScript_GetText("SPELL_RANGE_UNLIMITED", -1, GENDER_NOT_APPLICABLE));
        return;
    }

    if (enemyMinRange > 0.0f) {
        SStrPrintf(distance, sizeof(distance), "%d-%d", CMath::fint_(minRange), CMath::fint_(maxRange));
    } else {
        SStrPrintf(distance, sizeof(distance), "%d", CMath::fint_(maxRange));
    }

    if (side) {
        FormatString("SPELL_RANGE_DUAL", -1, GENDER_NOT_APPLICABLE, format, sizeof(format));
        SStrPrintf(range, 128, format, FrameScript_GetText(side, -1, GENDER_NOT_APPLICABLE), distance);
    } else {
        FormatString("SPELL_RANGE", -1, GENDER_NOT_APPLICABLE, format, sizeof(format));
        SStrPrintf(range, 128, format, distance);
    }
}

// INLINED
static void AddSpellRangeLines(CGTooltip* tooltip, const SpellRec* spell, CGUnit_C* caster, int32_t brief, char* cost, char* range) {
    const CImVector& textColor = *reinterpret_cast<const CImVector*>(&s_tooltipTextColor);
    float enemyMin = 0.0f;
    float enemyMax = 0.0f;
    float friendlyMin = 0.0f;
    float friendlyMax = 0.0f;

    if (!brief && !(spell->m_attributes & 0x404) && !(spell->m_attributesExC & 0x40000000)) {
        if (SpellRec::IsMeleeRange(spell) && !SpellRec::HasModifier(spell, 5)) {
            char format[128];
            FormatString("MELEE_RANGE", -1, GENDER_NOT_APPLICABLE, format, sizeof(format));
            SStrPrintf(range, 128, format);
        } else {
            Spell_C_GetMinMaxRange(caster, spell, &enemyMin, &enemyMax, 0, nullptr);
            Spell_C_GetMinMaxRange(caster, spell, &friendlyMin, &friendlyMax, 1, nullptr);

            if (SpellRec::IsMinimumRange(spell)) {
                SpellRec::GetMinRange(spell, &enemyMin);
                SpellRec::GetMinRange(spell, &friendlyMin);
            }

            if (fnotequal(enemyMin, friendlyMin) || fnotequal(enemyMax, friendlyMax)) {
                if (enemyMax > 0.0f) {
                    FormatSpellRange(range, enemyMin, enemyMax, enemyMin, "ENEMY");
                }

                if (cost[0]) {
                    tooltip->AddLine(" ", range, textColor, textColor, 0);
                } else {
                    SStrCopy(cost, range, 128);
                    range[0] = '\0';
                    tooltip->AddLine(cost, range, textColor, textColor, 0);
                }

                if (friendlyMax > 0.0f) {
                    FormatSpellRange(range, friendlyMin, friendlyMax, enemyMin, "FRIENDLY");
                }

                if (!cost[0]) {
                    SStrCopy(cost, range, 128);
                    range[0] = '\0';
                }

                tooltip->AddLine(cost, range, textColor, textColor, 0);
                return;
            }

            if (enemyMax > 0.0f) {
                FormatSpellRange(range, enemyMin, enemyMax, enemyMin, nullptr);
            }
        }
    }

    if (!cost[0]) {
        SStrCopy(cost, range, 128);
        range[0] = '\0';
    }

    tooltip->AddLine(cost, range, textColor, textColor, 0);
}

// INLINED
static int32_t AddSpellCastTimeLine(CGTooltip* tooltip, const SpellRec* spell, int32_t isPet, int32_t isInspect, int32_t powerCost, int32_t cooldown) {
    const CImVector& textColor = *reinterpret_cast<const CImVector*>(&s_tooltipTextColor);
    char castTime[128];
    char recastTime[128];
    char format[128];

    if (spell->m_effect[0] == 47 || (spell->m_attributes & 0x40)) {
        return 1;
    }

    if (spell->m_effect[0] == 78) {
        return 0;
    }

    int32_t time = SpellRec::GetCastTime(spell, isPet, isInspect, 1);
    int32_t minutes = time >= 60000;

    if (time > 0) {
        FormatString(minutes ? "SPELL_CAST_TIME_MIN" : "SPELL_CAST_TIME_SEC", -1, GENDER_NOT_APPLICABLE, format, sizeof(format));
        SStrPrintf(castTime, sizeof(castTime), format, time / (minutes ? 60000.0f : 1000.0f));
    } else if (spell->m_attributesEx & 0x44) {
        FormatString("SPELL_CAST_CHANNELED", -1, GENDER_NOT_APPLICABLE, castTime, sizeof(castTime));
    } else if (time < 0) {
        FormatString("SPELL_CAST_TIME_INSTANT", -1, GENDER_NOT_APPLICABLE, castTime, sizeof(castTime));
    } else if (spell->m_attributes & 0x404) {
        FormatString("SPELL_ON_NEXT_SWING", -1, GENDER_NOT_APPLICABLE, castTime, sizeof(castTime));
    } else if (spell->m_attributes & 2) {
        FormatString("SPELL_ON_NEXT_RANGED", -1, GENDER_NOT_APPLICABLE, castTime, sizeof(castTime));
    } else if (!spell->m_powerType && powerCost > 0) {
        FormatString("SPELL_CAST_TIME_INSTANT", -1, GENDER_NOT_APPLICABLE, castTime, sizeof(castTime));
    } else {
        FormatString("SPELL_CAST_TIME_INSTANT_NO_MANA", -1, GENDER_NOT_APPLICABLE, castTime, sizeof(castTime));
    }

    if (cooldown <= 0 || (spell->m_attributesExF & 1)) {
        recastTime[0] = '\0';
    } else {
        int32_t recastMinutes = cooldown >= 60000;
        FormatString(recastMinutes ? "SPELL_RECAST_TIME_MIN" : "SPELL_RECAST_TIME_SEC", -1, GENDER_NOT_APPLICABLE, format, sizeof(format));
        SStrPrintf(recastTime, sizeof(recastTime), format, cooldown / (recastMinutes ? 60000.0f : 1000.0f));
    }

    tooltip->AddLine(castTime, recastTime, textColor, textColor, 0);
    return 0;
}

// INLINED
static void AddSpellToolLine(CGTooltip* tooltip, const SpellRec* spell, CGPlayer_C* player, int32_t brief) {
    const CImVector& textColor = *reinterpret_cast<const CImVector*>(&s_tooltipTextColor);
    char text[4096];
    int32_t first = 1;
    int32_t allFound = 1;

    for (uint32_t i = 0; i < 2; i++) {
        int32_t itemId = spell->m_totem[i];

        if (itemId <= 0) {
            continue;
        }

        WOWGUID guid = { (static_cast<uint64_t>(0x1FE00000) << 32) | static_cast<uint32_t>(spell->m_id) };
        ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, guid, &TooltipSpellCallback, tooltip, true);

        if (!stats) {
            tooltip->m_pendingQueries++;
            continue;
        }

        if (first) {
            first = 0;
            SStrCopy(text, FrameScript_GetText("SPELL_TOTEMS", -1, GENDER_NOT_APPLICABLE), sizeof(text));
        } else {
            SStrPack(text, ", ", sizeof(text));
        }

        CGItem_C* item = player->m_bag.FindItemOfType(spell->m_totem[i], 0);

        if (!item) {
            SStrPack(text, s_tooltipRedColorString, sizeof(text));
            allFound = 0;
        }

        SStrPack(text, stats->m_name[0], sizeof(text));

        if (!item) {
            SStrPack(text, "|r", sizeof(text));
        }
    }

    for (uint32_t i = 0; i < 2; i++) {
        TotemCategoryRec* category = g_totemCategoryDB.GetRecord(spell->m_requiredTotemCategoryID[i]);

        if (!category) {
            continue;
        }

        if (first) {
            first = 0;
            SStrCopy(text, FrameScript_GetText("SPELL_TOTEMS", -1, GENDER_NOT_APPLICABLE), sizeof(text));
        } else {
            SStrPack(text, ", ", sizeof(text));
        }

        if (player->m_bag.FindTotemCategory(spell->m_requiredTotemCategoryID[i], 0)) {
            SStrPack(text, category->m_name, sizeof(text));
        } else {
            SStrPack(text, s_tooltipRedColorString, sizeof(text));
            SStrPack(text, category->m_name, sizeof(text));
            SStrPack(text, "|r", sizeof(text));
            allFound = 0;
        }
    }

    if (!first && (!allFound || !brief)) {
        tooltip->AddLine(text, nullptr, textColor, textColor, 0);
    }
}

// INLINED
static void AddSpellEquippedItemLine(CGTooltip* tooltip, const SpellRec* spell, CGPlayer_C* player, int32_t brief, int32_t isPet) {
    char items[512];
    char format[128];
    char text[512];

    if ((spell->m_targets & 0x10) || spell->m_equippedItemClass < 0 || !spell->m_equippedItemSubclass) {
        return;
    }

    items[0] = '\0';
    int32_t found = 0;

    for (int32_t i = 0; i < g_itemSubClassMaskDB.m_numRecords; i++) {
        ItemSubClassMaskRec* mask = &g_itemSubClassMaskDB.m_records[i];

        if (mask->m_classID == spell->m_equippedItemClass && mask->m_mask == spell->m_equippedItemSubclass) {
            SStrCopy(items, mask->m_name, sizeof(items));
            found = 1;
            break;
        }
    }

    int32_t first = 1;
    int32_t equipped = 1;
    uint32_t slotMask = (spell->m_attributesExC & 0x400) ? 0x8000 : (spell->m_attributesExC & 0x1000000) ? 0x10000 : 0xFFFFFFFF;

    if (!player->EquippedItemMeetSpellRequirements(spell, slotMask)) {
        equipped = 0;
    }

    if ((spell->m_attributesExC & 0x1000400) == 0x1000400 && !player->EquippedItemMeetSpellRequirements(spell, 0x10000)) {
        equipped = 0;
    }

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        ItemSubClassRec* subClass = &g_itemSubClassDB.m_records[i];

        if (subClass->m_classID != spell->m_equippedItemClass || !((1 << subClass->m_subClassID) & spell->m_equippedItemSubclass) || found) {
            continue;
        }

        if (first) {
            first = 0;
        } else {
            SStrPack(items, ", ", sizeof(items));
        }

        SStrPack(items, subClass->m_verboseName && *subClass->m_verboseName ? subClass->m_verboseName : subClass->m_displayName, sizeof(items));
    }

    if (isPet) {
        equipped = 1;
    }

    if (!items[0] || (equipped && brief)) {
        return;
    }

    SStrCopy(format, FrameScript_GetText(!equipped && brief ? "SPELL_EQUIPPED_ITEM_NOSPACE" : "SPELL_EQUIPPED_ITEM", -1, GENDER_NOT_APPLICABLE), sizeof(format));
    SStrPrintf(text, sizeof(text), format, items);
    const CImVector& color = *reinterpret_cast<const CImVector*>(equipped ? &s_tooltipTextColor : &s_tooltipRedColor);
    tooltip->AddLine(text, nullptr, color, color, 1);
}

// INLINED
static void AddSpellFormLine(CGTooltip* tooltip, const SpellRec* spell, CGUnit_C* caster, int32_t brief) {
    char forms[512];
    char format[128];
    char text[128];

    if (!spell->m_shapeshiftMask[0] && !spell->m_shapeshiftMask[1]) {
        return;
    }

    int32_t first = 1;
    int32_t needsForm = spell->m_attributesExB & 0x80000;

    if (!needsForm) {
        uint32_t count = g_spellShapeshiftFormDB.m_numRecords;

        if (count > 64) {
            count = 64;
        }

        for (uint32_t i = 0; i < count; i++) {
            if (!SpellRec::UsableInShapeshift(spell, i) || static_cast<int32_t>(i) >= g_spellShapeshiftFormDB.m_numRecords) {
                continue;
            }

            SpellShapeshiftFormRec* form = &g_spellShapeshiftFormDB.m_records[i];

            if (!form->m_name || !*form->m_name) {
                continue;
            }

            if (first) {
                SStrCopy(forms, form->m_name, sizeof(forms));
                first = 0;
            } else {
                SStrPack(forms, ", ", sizeof(forms));
                SStrPack(forms, form->m_name, sizeof(forms));
            }
        }
    }

    int32_t usable = !caster || SpellRec::UsableInShapeshift(spell, caster->GetShapeshiftForm() - 1) || (needsForm && !caster->IsShapeShifted()) || caster->AffectedByAura(275, spell);

    if (first || (usable && brief)) {
        return;
    }

    SStrCopy(format, FrameScript_GetText(!usable && brief ? "SPELL_REQUIRED_FORM_NOSPACE" : "SPELL_REQUIRED_FORM", -1, GENDER_NOT_APPLICABLE), sizeof(format));
    SStrPrintf(text, sizeof(text), format, forms);
    const CImVector& color = *reinterpret_cast<const CImVector*>(usable ? &s_tooltipTextColor : &s_tooltipRedColor);
    tooltip->AddLine(text, nullptr, color, color, 0);
}

// INLINED
static void AddSpellReputationLine(CGTooltip* tooltip, const SpellRec* spell, CGPlayer_C* player, int32_t brief) {
    char token[128];
    char text[128];

    if (!spell->m_minFactionID) {
        return;
    }

    int32_t reached = GetRepListRepValue(spell->m_minFactionID) >= CGReputationInfo::m_reactionThresholds[spell->m_minReputation];

    if (reached && brief) {
        return;
    }

    FactionRec* faction = g_factionDB.GetRecord(spell->m_minFactionID);
    SStrPrintf(token, sizeof(token), "FACTION_STANDING_LABEL%d", spell->m_minReputation + 1);
    const char* name = faction ? faction->m_name : "UNKNOWN";
    SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_REPUTATION", -1, GENDER_NOT_APPLICABLE), name, player->GetFrameScriptText(token, -1));
    const CImVector& color = *reinterpret_cast<const CImVector*>(reached ? &s_tooltipTextColor : &s_tooltipRedColor);
    tooltip->AddLine(text, nullptr, color, color, 0);
}

// INLINED
static void AddSpellReagentLine(CGTooltip* tooltip, const SpellRec* spell, CGPlayer_C* player, int32_t brief) {
    const CImVector& textColor = *reinterpret_cast<const CImVector*>(&s_tooltipTextColor);
    char text[4096];
    char reagent[128];
    int32_t first = 1;
    int32_t allFound = 1;

    for (uint32_t i = 0; i < 8; i++) {
        int32_t itemId = spell->m_reagent[i];

        if (itemId <= 0) {
            continue;
        }

        WOWGUID guid = { (static_cast<uint64_t>(0x1FE00000) << 32) | static_cast<uint32_t>(spell->m_id) };
        ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, guid, &TooltipSpellCallback, tooltip, true);

        if (!stats) {
            tooltip->m_pendingQueries++;
            continue;
        }

        if (first) {
            first = 0;
            SStrCopy(text, FrameScript_GetText("SPELL_REAGENTS", -1, GENDER_NOT_APPLICABLE), sizeof(text));
        } else {
            SStrPack(text, ", ", sizeof(text));
        }

        const char* name = stats->m_name[0];

        if (spell->m_reagentCount[i] <= 1) {
            SStrCopy(reagent, name, sizeof(reagent));
        } else {
            SStrPrintf(reagent, sizeof(reagent), "%s (%d)", name, spell->m_reagentCount[i]);
        }

        if (player->m_bag.GetItemTypeCount(spell->m_reagent[i], 0) >= spell->m_reagentCount[i]) {
            SStrPack(text, reagent, sizeof(text));
        } else {
            SStrPack(text, s_tooltipRedColorString, sizeof(text));
            SStrPack(text, reagent, sizeof(text));
            SStrPack(text, "|r", sizeof(text));
            allFound = 0;
        }
    }

    if (!first && (!allFound || !brief)) {
        tooltip->AddLine(text, nullptr, textColor, textColor, 1);
    }
}

// INLINED
static int32_t AddSpellEffectChanceLine(CGTooltip* tooltip, const SpellRec* spell, CGPlayer_C* player, int32_t isTradeSkill) {
    const CImVector& textColor = *reinterpret_cast<const CImVector*>(&s_tooltipTextColor);
    char text[128];
    int32_t effect = spell->m_effect[0];

    if (effect != 78 && !(isTradeSkill && (effect == 20 || effect == 23 || effect == 22))) {
        return 0;
    }

    float chance;
    const char* token;

    switch (effect) {
        case 20:
            chance = player->m_player->m_playerDodgePercentage;
            token = "CHANCE_TO_DODGE";
            break;

        case 22:
            chance = player->m_player->m_playerParryPercentage;
            token = "CHANCE_TO_PARRY";
            break;

        case 23:
            chance = player->m_player->m_playerBlockPercentage;
            token = "CHANCE_TO_BLOCK";
            break;

        case 78:
            chance = player->m_player->m_playerCritPercentage;
            token = "CHANCE_TO_CRIT";
            break;

        default:
            return 0;
    }

    SStrPrintf(text, sizeof(text), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), chance);
    tooltip->AddLine(text, nullptr, textColor, textColor, 0);
    return 1;
}

// INLINED
static void AddSpellUseAllPowerLine(CGTooltip* tooltip, const SpellRec* spell, const PowerDisplayRec* display) {
    const CImVector& textColor = *reinterpret_cast<const CImVector*>(&s_tooltipTextColor);
    char power[256];
    char text[128];

    if (display) {
        SStrPrintf(power, sizeof(power), FrameScript_GetText(display->m_globalStringBaseTag, -1, GENDER_NOT_APPLICABLE));
        SStrPrintf(text, sizeof(text), FrameScript_GetText("SPELL_USE_ALL_POWER_DISPLAY", -1, GENDER_NOT_APPLICABLE), power);
    } else {
        const char* token = static_cast<uint32_t>(spell->m_powerType) > 6 ? "SPELL_USE_ALL_HEALTH" : s_useAllPowerTokens[spell->m_powerType];
        SStrCopy(text, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), sizeof(text));
    }

    tooltip->AddLine(text, nullptr, textColor, textColor, 0);
}

// VERIFY: 0360572e manual
// OFFSET: 0x006238A0
int32_t CGTooltip::SetSpell(int32_t spellId, int32_t brief, uint32_t cooldown, int32_t isPet, int32_t showRank, int32_t isInspect, int32_t a8, const TalentRec* talent, CGTalentGroup_C* talentGroup, int32_t append, int32_t usePreview, int32_t a13, int32_t rank, int32_t maxRank, int32_t showLearn) {
    const CImVector& titleColor = *reinterpret_cast<const CImVector*>(&s_tooltipTitleColor);
    const CImVector& textColor = *reinterpret_cast<const CImVector*>(&s_tooltipTextColor);
    const CImVector& redColor = *reinterpret_cast<const CImVector*>(&s_tooltipRedColor);
    const CImVector& grayColor = *reinterpret_cast<const CImVector*>(&s_tooltipGrayColor);
    char text[128];
    char format[128];
    char cost[128];
    char range[128];
    char description[2048];

    if (!append) {
        this->ClearTooltip();
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        this->Hide();
        return 0;
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell)) {
        if (!append) {
            this->Hide();
        }

        return 0;
    }

    if (!append) {
        this->m_spellId = spellId;
        this->m_pendingQueries = 0;
    }

    CGUnit_C* caster;

    if (isInspect) {
        caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGCharacterInfo::m_inspectTarget, TYPEMASK_UNIT, __FILE__, __LINE__));
    } else if (isPet) {
        caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
    } else {
        caster = player;
    }

    int32_t tradeSkill = 0;
    int32_t createsItem = 0;

    if (spell.m_attributes & 0x20) {
        for (uint32_t i = 0; i < 3; i++) {
            int32_t effect = spell.m_effect[i];

            if (effect == 53) {
                tradeSkill = 1;
                break;
            }

            if (effect == 24 || effect == 157 || effect == 59) {
                tradeSkill = 1;
                createsItem = 1;
            }
        }
    }

    if (talent && append) {
        SStrPrintf(text, sizeof(text), "\n%s", FrameScript_GetText("TOOLTIP_TALENT_NEXT_RANK", -1, GENDER_NOT_APPLICABLE));
        this->AddLine(text, nullptr, textColor, textColor, 0);
    } else if (tradeSkill) {
        SkillLineAbilityRec* ability = SpellTableLookupAbility(player->m_unit->m_unitFieldBytes0[0], player->m_unit->m_unitFieldBytes0[1], spellId);

        if (ability) {
            SkillLineRec* skillLine = g_skillLineDB.GetRecord(ability->m_skillLine);

            if (skillLine) {
                SStrPrintf(text, sizeof(text), "%s: %s", skillLine->m_displayName, spell.m_name);
                this->AddLine(text, nullptr, titleColor, titleColor, 0);
            }
        }
    } else {
        this->AddLine(spell.m_name, brief || showRank ? spell.m_nameSubtext : nullptr, textColor, grayColor, 0);
    }

    int32_t prereqsMet = 1;

    if (talent && !append) {
        if (maxRank >= 0) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("TOOLTIP_TALENT_RANK", -1, GENDER_NOT_APPLICABLE), rank + 1, maxRank + 1);
            this->AddLine(text, nullptr, textColor, textColor, 0);
        }

        if (rank < 0) {
            prereqsMet = this->AddTalentPrereqs(talent, talentGroup, isInspect, isPet, usePreview);
        }
    }

    cost[0] = '\0';
    range[0] = '\0';
    PowerDisplayRec* display = g_powerDisplayDB.GetRecord(spell.m_powerDisplayID);
    int32_t powerCost = Spell_C_GetPowerCost(&spell, caster);

    if (!brief) {
        powerCost = SetSpellPowerCost(cost, &spell, caster, powerCost, display);
    }

    AddSpellRangeLines(this, &spell, caster, brief, cost, range);

    int32_t recovery = spell.m_recoveryTime;
    int32_t categoryRecovery = spell.m_categoryRecoveryTime;
    Spell_C_ModifySpellValue(&spell, recovery, 11);
    Spell_C_ModifyCategoryRecovery(categoryRecovery, &spell, 0);
    int32_t recast = recovery > categoryRecovery ? recovery : categoryRecovery;
    int32_t isTradeSkill = 0;

    if (!brief && !tradeSkill) {
        isTradeSkill = AddSpellCastTimeLine(this, &spell, isPet, isInspect, powerCost, recast);
    }

    if (!isPet) {
        AddSpellToolLine(this, &spell, player, brief);
    }

    AddSpellEquippedItemLine(this, &spell, player, brief, isPet);
    AddSpellFormLine(this, &spell, caster, brief);
    AddSpellReputationLine(this, &spell, player, brief);

    if (isTradeSkill && caster && static_cast<int32_t>(caster->m_unit->m_unitFieldLevel) < spell.m_baseLevel) {
        SStrCopy(format, FrameScript_GetText("ITEM_MIN_LEVEL", -1, GENDER_NOT_APPLICABLE), sizeof(format));
        SStrPrintf(text, sizeof(text), format, spell.m_baseLevel);
        this->AddLine(text, nullptr, redColor, redColor, 0);
    }

    if (!isPet && !Spell_C_HasNoReagentCost(player, &spell)) {
        AddSpellReagentLine(this, &spell, player, brief);
    }

    int32_t result = 0;

    if (cooldown) {
        CGTooltip::GetDurationString(text, sizeof(text), cooldown, "ITEM_COOLDOWN_TIME", nullptr, 1, 0);
        this->AddLine(text, nullptr, textColor, textColor, 0);
        result = 1;
    }

    if (!brief) {
        if (AddSpellEffectChanceLine(this, &spell, player, isTradeSkill)) {
            result = 1;
        }

        if (spell.m_attributesEx & 2) {
            AddSpellUseAllPowerLine(this, &spell, display);
        }

        if (spell.m_description && *spell.m_description) {
            SpellParserParseText(&spell, description, sizeof(description), isPet, isInspect, 0, 0, 1, 0);
            this->AddLine(description, nullptr, titleColor, titleColor, 1);
        }

        if (talent && !isInspect && showLearn) {
            this->AddTalentLearnText(talent, talentGroup, prereqsMet, rank, maxRank, 0, a8, isPet, usePreview);
        }

        if (createsItem && spell.m_effectItemType[0]) {
            this->m_itemLinkInfo.Clear();
            int32_t itemParams[2] = { 0, 0 };
            WOWGUID itemGuid = { 0 };
            this->SetItem(spell.m_effectItemType[0], itemParams, &itemGuid, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1);
        }
    }

    if (this->m_onTooltipSetSpell.m_luaRef) {
        this->RunScript(this->m_onTooltipSetSpell, 0, nullptr);
    }

    this->Show();
    this->CalculateSize();
    return result;
}
