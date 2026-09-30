#include "Tooltip.h"
#include <cstdint>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Types.h"

int32_t Script_AddFontStrings(lua_State* L);
int32_t Script_GetAnchorType(lua_State* L);
int32_t Script_SetAnchorType(lua_State* L);
int32_t Script_AddTexture(lua_State* L);
int32_t Script_SetHyperlink(lua_State* L);
int32_t Script_SetAction(lua_State* L);
int32_t Script_SetPetAction(lua_State* L);
int32_t Script_SetShapeshift(lua_State* L);
int32_t Script_SetPossession(lua_State* L);
int32_t Script_TooltipSetTracking(lua_State* L);
int32_t Script_SetSpell(lua_State* L);
int32_t Script_SetSpellByID(lua_State* L);
int32_t Script_SetGlyph(lua_State* L);
int32_t Script_SetInventoryItem(lua_State* L);
int32_t Script_SetLootItem(lua_State* L);
int32_t Script_SetQuestItem(lua_State* L);
int32_t Script_SetQuestLogItem(lua_State* L);
int32_t Script_SetTrainerService(lua_State* L);
int32_t Script_SetTradeSkillItem(lua_State* L);
int32_t Script_SetMerchantItem(lua_State* L);
int32_t Script_SetMerchantCostItem(lua_State* L);
int32_t Script_SetTradePlayerItem(lua_State* L);
int32_t Script_SetTradeTargetItem(lua_State* L);
int32_t Script_SetBagItem(lua_State* L);
int32_t Script_SetUnit(lua_State* L);
int32_t Script_SetUnitBuff(lua_State* L);
int32_t Script_SetUnitDebuff(lua_State* L);
int32_t Script_SetUnitAura(lua_State* L);
int32_t Script_SetTalent(lua_State* L);
int32_t Script_SetSendMailItem(lua_State* L);
int32_t Script_SetInboxItem(lua_State* L);
int32_t Script_SetAuctionSellItem(lua_State* L);
int32_t Script_SetAuctionItem(lua_State* L);
int32_t Script_SetQuestRewardSpell(lua_State* L);
int32_t Script_SetQuestLogRewardSpell(lua_State* L);
int32_t Script_SetHyperlinkCompareItem(lua_State* L);
int32_t Script_SetBuybackItem(lua_State* L);
int32_t Script_SetLootRollItem(lua_State* L);
int32_t Script_SetSocketedItem(lua_State* L);
int32_t Script_SetSocketGem(lua_State* L);
int32_t Script_SetExistingSocketGem(lua_State* L);
int32_t Script_SetGuildBankItem(lua_State* L);
int32_t Script_IsUnit(lua_State* L);
int32_t Script_GetUnit(lua_State* L);
int32_t Script_GetItem(lua_State* L);
int32_t Script_GetSpell(lua_State* L);
int32_t Script_SetTotem(lua_State* L);
int32_t Script_SetCurrencyToken(lua_State* L);
int32_t Script_SetBackpackToken(lua_State* L);
int32_t Script_SetQuestLogSpecialItem(lua_State* L);
int32_t Script_SetEquipmentSet(lua_State* L);
int32_t Script_SetFrameStack(lua_State* L);
int32_t Script_SetLFGDungeonReward(lua_State* L);
int32_t Script_SetLFGCompletionReward(lua_State* L);

// VERIFY: 321f8a1a auto
// OFFSET: 0x0061B470
static int32_t FindItemByGUID(const CGItem_C* item, void* param) {
    return item->m_obj->m_guid == *static_cast<WOWGUID*>(param);
}

// VERIFY: f6c0cb74 auto
// OFFSET: 0x0061D040
static int32_t Script_SetMinimumWidth(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetMinimumWidth(width [,force])", name ? name : "<unnamed>");
        return 0;
    }

    int32_t force = StringToBOOL(L, 3, 0);
    tooltip->m_minimumWidth = lua_tonumber(L, 2);
    tooltip->m_minimumWidthForce = force;
    return 0;
}

// VERIFY: df833e20 auto
// OFFSET: 0x0061D0D0
static int32_t Script_GetMinimumWidth(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    lua_pushnumber(L, tooltip->m_minimumWidth);

    if (tooltip->m_minimumWidthForce) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// VERIFY: dc0e015c auto
// OFFSET: 0x0061D150
static int32_t Script_SetPadding(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    float padding = lua_tonumber(L, 2);
    tooltip->m_padding = NDCToDDCWidth(padding / (CoordinateGetAspectCompensation() * 1024.0f));
    return 0;
}

// VERIFY: 8db86c86 auto
// OFFSET: 0x0061D1C0
static int32_t Script_GetPadding(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    lua_pushnumber(L, tooltip->m_padding);
    return 1;
}

// VERIFY: 3d13683a auto
// OFFSET: 0x0061D210
static int32_t Script_IsOwned(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (lua_type(L, 2) != LUA_TTABLE) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:IsOwned(frame)", name ? name : "<unnamed>");
        return 0;
    }

    lua_rawgeti(L, 2, 0);
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
    lua_settop(L, -2);

    if (!frame) {
        const char* name = tooltip->GetName();
        luaL_error(L, "%s:IsOwned(): Couldn't find 'this' in frame object", name ? name : "<unnamed>");
        return 0;
    }

    if (!frame->IsA(CSimpleFrame::GetObjectType())) {
        const char* name = tooltip->GetName();
        luaL_error(L, "%s:IsOwned(): Wrong object type, expected frame", name ? name : "<unnamed>");
        return 0;
    }

    if (tooltip->m_owner == frame) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: ef7a4d10 manual
// OFFSET: 0x0061D350
static int32_t Script_GetOwner(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    CSimpleFrame* owner = tooltip->m_owner;

    if (!owner) {
        lua_pushnil(L);
        return 1;
    }

    if (!owner->m_luaRegistered) {
        owner->RegisterScriptObject(nullptr);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, owner->m_luaObjectRef);
    return 1;
}

// VERIFY: a61b06c0 auto
// OFFSET: 0x0061D7D0
static int32_t Script_ClearLines(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    tooltip->ClearTooltip();
    return 0;
}

// VERIFY: 1c7c58ca manual
// OFFSET: 0x0061D940
static int32_t Script_FadeOut(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    tooltip->FadeOut();
    return 0;
}

// VERIFY: 95095d7d manual
// OFFSET: 0x0061D9C0
static int32_t Script_NumLines(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    lua_pushnumber(L, tooltip->m_numLines);
    return 1;
}

// VERIFY: 0c5c962a manual
// OFFSET: 0x0061F1F0
static int32_t Script_IsEquippedItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    WOWGUID guid = tooltip->m_itemGuid;

    if (player && guid != WOWGUID() && player->m_bag.FindItem(FindItemByGUID, &guid, 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: c0a54dc6 auto
// OFFSET: 0x0061B4C0
void CGTooltip::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CGTooltip::s_metatable);
    CGTooltip::s_metatable = LUA_NOREF;
}

// VERIFY: fdbe92dc auto
// OFFSET: 0x0061DBA0
void CGTooltip::CreateScriptMetaTable() {
    CGTooltip::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CGTooltip::RegisterScriptMethods);
}

// VERIFY: 3d12aa78 auto
// OFFSET: 0x0061EB40
static int32_t Script_SetOwner(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    tooltip->Hide();

    if (lua_type(L, 2) != LUA_TTABLE) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetOwner(frame)", name ? name : "<unnamed>");
        return 0;
    }

    lua_rawgeti(L, 2, 0);
    CSimpleFrame* owner = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
    lua_settop(L, -2);

    if (!owner) {
        const char* name = tooltip->GetName();
        luaL_error(L, "%s:SetOwner(): Couldn't find 'this' in frame object", name ? name : "<unnamed>");
        return 0;
    }

    if (!owner->IsA(CSimpleFrame::GetObjectType())) {
        const char* name = tooltip->GetName();
        luaL_error(L, "%s:SetOwner(): Wrong object type, expected frame", name ? name : "<unnamed>");
        return 0;
    }

    if (owner == tooltip) {
        const char* name = tooltip->GetName();
        luaL_error(L, "%s:SetOwner(): Can't set owner to self", name ? name : "<unnamed>");
        return 0;
    }

    TOOLTIP_ANCHORPOINT anchor = ANCHOR_LEFT;

    if (lua_isstring(L, 3)) {
        const char* point = lua_tolstring(L, 3, nullptr);

        if (!SStrCmpI(point, "ANCHOR_LEFT", STORM_MAX_STR)) {
            anchor = ANCHOR_LEFT;
        } else if (!SStrCmpI(point, "ANCHOR_RIGHT", STORM_MAX_STR)) {
            anchor = ANCHOR_RIGHT;
        } else if (!SStrCmpI(point, "ANCHOR_BOTTOMLEFT", STORM_MAX_STR)) {
            anchor = ANCHOR_BOTTOMLEFT;
        } else if (!SStrCmpI(point, "ANCHOR_BOTTOM", STORM_MAX_STR)) {
            anchor = ANCHOR_BOTTOM;
        } else if (!SStrCmpI(point, "ANCHOR_BOTTOMRIGHT", STORM_MAX_STR)) {
            anchor = ANCHOR_BOTTOMRIGHT;
        } else if (!SStrCmpI(point, "ANCHOR_TOPLEFT", STORM_MAX_STR)) {
            anchor = ANCHOR_TOPLEFT;
        } else if (!SStrCmpI(point, "ANCHOR_TOP", STORM_MAX_STR)) {
            anchor = ANCHOR_TOP;
        } else if (!SStrCmpI(point, "ANCHOR_TOPRIGHT", STORM_MAX_STR)) {
            anchor = ANCHOR_TOPRIGHT;
        } else if (!SStrCmpI(point, "ANCHOR_CURSOR", STORM_MAX_STR)) {
            anchor = ANCHOR_CURSOR;
        } else if (!SStrCmpI(point, "ANCHOR_NONE", STORM_MAX_STR)) {
            anchor = ANCHOR_NONE;
        } else if (!SStrCmpI(point, "ANCHOR_PRESERVE", STORM_MAX_STR)) {
            anchor = ANCHOR_PRESERVE;
        } else if (!SStrCmpI(point, "ANCHOR_CURSOR_RIGHT", STORM_MAX_STR)) {
            anchor = ANCHOR_CURSOR_RIGHT;
        }
    }

    float offsetX = 0.0f;

    if (lua_isnumber(L, 4)) {
        float x = lua_tonumber(L, 4);
        offsetX = NDCToDDCWidth(x / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    float offsetY = 0.0f;

    if (lua_isnumber(L, 5)) {
        float y = lua_tonumber(L, 5);
        offsetY = NDCToDDCWidth(y / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    tooltip->SetOwner(owner, anchor, offsetX, offsetY);
    return 0;
}

// VERIFY: b3dfdf72 auto
// OFFSET: 0x00620340
static int32_t Script_AddLine(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    const char* text = nullptr;
    CImVector color = *reinterpret_cast<const CImVector*>(&s_tooltipTitleColor);

    if (lua_isstring(L, 2)) {
        text = lua_tolstring(L, 2, nullptr);
    }

    if (lua_isnumber(L, 3)) {
        FrameScript_GetColorNoAlpha(L, 3, color);
    }

    int32_t wrap = StringToBOOL(L, 6, 0);
    tooltip->AddLine(text, nullptr, color, color, wrap);
    return 0;
}

// VERIFY: 33c79c5a manual
// OFFSET: 0x006204E0
static int32_t Script_SetText(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetText(\"text\" [, color])", name ? name : "<unnamed>");
        return 0;
    }

    const char* text = lua_tolstring(L, 2, nullptr);
    CImVector color = *reinterpret_cast<const CImVector*>(&s_tooltipTitleColor);

    if (lua_isnumber(L, 3)) {
        FrameScript_GetColor(L, 3, color);
    }

    int32_t wrap = StringToBOOL(L, 7, 0);
    tooltip->ClearTooltip();
    tooltip->AddLine(text, nullptr, color, color, wrap);
    tooltip->Show();
    return 0;
}

// VERIFY: 46a6c27d auto
// OFFSET: 0x006203F0
static int32_t Script_AddDoubleLine(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    const char* left = nullptr;
    const char* right = nullptr;
    CImVector leftColor = *reinterpret_cast<const CImVector*>(&s_tooltipTitleColor);
    CImVector rightColor = *reinterpret_cast<const CImVector*>(&s_tooltipTitleColor);

    if (lua_isstring(L, 2)) {
        left = lua_tolstring(L, 2, nullptr);
    }

    if (lua_isstring(L, 3)) {
        right = lua_tolstring(L, 3, nullptr);
    }

    if (lua_isnumber(L, 4)) {
        FrameScript_GetColorNoAlpha(L, 4, leftColor);
    }

    if (lua_isnumber(L, 7)) {
        FrameScript_GetColorNoAlpha(L, 7, rightColor);
    }

    int32_t wrap = StringToBOOL(L, 10, 0);
    tooltip->AddLine(left, right, leftColor, rightColor, wrap);
    return 0;
}

// VERIFY: b8889e5f manual
// OFFSET: 0x0061EE90
static int32_t Script_AppendText(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:AppendText(\"text\")", name ? name : "<unnamed>");
        return 0;
    }

    tooltip->AppendText(lua_tolstring(L, 2, nullptr));
    return 0;
}

// STATIC: 0x00AD2AE0
static FrameScript_Method s_ScriptMethods[] = {
    { "AddFontStrings", &Script_AddFontStrings },
    { "SetMinimumWidth", &Script_SetMinimumWidth },
    { "GetMinimumWidth", &Script_GetMinimumWidth },
    { "SetPadding", &Script_SetPadding },
    { "GetPadding", &Script_GetPadding },
    { "IsOwned", &Script_IsOwned },
    { "GetOwner", &Script_GetOwner },
    { "SetOwner", &Script_SetOwner },
    { "GetAnchorType", &Script_GetAnchorType },
    { "SetAnchorType", &Script_SetAnchorType },
    { "ClearLines", &Script_ClearLines },
    { "AddLine", &Script_AddLine },
    { "AddDoubleLine", &Script_AddDoubleLine },
    { "AddTexture", &Script_AddTexture },
    { "SetText", &Script_SetText },
    { "AppendText", &Script_AppendText },
    { "FadeOut", &Script_FadeOut },
    { "SetHyperlink", &Script_SetHyperlink },
    { "SetAction", &Script_SetAction },
    { "SetPetAction", &Script_SetPetAction },
    { "SetShapeshift", &Script_SetShapeshift },
    { "SetPossession", &Script_SetPossession },
    { "SetTracking", &Script_TooltipSetTracking },
    { "SetSpell", &Script_SetSpell },
    { "SetSpellByID", &Script_SetSpellByID },
    { "SetGlyph", &Script_SetGlyph },
    { "SetInventoryItem", &Script_SetInventoryItem },
    { "SetLootItem", &Script_SetLootItem },
    { "SetQuestItem", &Script_SetQuestItem },
    { "SetQuestLogItem", &Script_SetQuestLogItem },
    { "SetTrainerService", &Script_SetTrainerService },
    { "SetTradeSkillItem", &Script_SetTradeSkillItem },
    { "SetMerchantItem", &Script_SetMerchantItem },
    { "SetMerchantCostItem", &Script_SetMerchantCostItem },
    { "SetTradePlayerItem", &Script_SetTradePlayerItem },
    { "SetTradeTargetItem", &Script_SetTradeTargetItem },
    { "SetBagItem", &Script_SetBagItem },
    { "SetUnit", &Script_SetUnit },
    { "SetUnitBuff", &Script_SetUnitBuff },
    { "SetUnitDebuff", &Script_SetUnitDebuff },
    { "SetUnitAura", &Script_SetUnitAura },
    { "SetTalent", &Script_SetTalent },
    { "SetSendMailItem", &Script_SetSendMailItem },
    { "SetInboxItem", &Script_SetInboxItem },
    { "SetAuctionSellItem", &Script_SetAuctionSellItem },
    { "SetAuctionItem", &Script_SetAuctionItem },
    { "NumLines", &Script_NumLines },
    { "SetQuestRewardSpell", &Script_SetQuestRewardSpell },
    { "SetQuestLogRewardSpell", &Script_SetQuestLogRewardSpell },
    { "SetHyperlinkCompareItem", &Script_SetHyperlinkCompareItem },
    { "SetBuybackItem", &Script_SetBuybackItem },
    { "SetLootRollItem", &Script_SetLootRollItem },
    { "SetSocketedItem", &Script_SetSocketedItem },
    { "SetSocketGem", &Script_SetSocketGem },
    { "SetExistingSocketGem", &Script_SetExistingSocketGem },
    { "SetGuildBankItem", &Script_SetGuildBankItem },
    { "IsUnit", &Script_IsUnit },
    { "GetUnit", &Script_GetUnit },
    { "GetItem", &Script_GetItem },
    { "GetSpell", &Script_GetSpell },
    { "SetTotem", &Script_SetTotem },
    { "SetCurrencyToken", &Script_SetCurrencyToken },
    { "SetBackpackToken", &Script_SetBackpackToken },
    { "IsEquippedItem", &Script_IsEquippedItem },
    { "SetQuestLogSpecialItem", &Script_SetQuestLogSpecialItem },
    { "SetEquipmentSet", &Script_SetEquipmentSet },
    { "SetFrameStack", &Script_SetFrameStack },
    { "SetLFGDungeonReward", &Script_SetLFGDungeonReward },
    { "SetLFGCompletionReward", &Script_SetLFGCompletionReward },
};

// VERIFY: 483706ce auto
// OFFSET: 0x0061B4A0
void CGTooltip::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
