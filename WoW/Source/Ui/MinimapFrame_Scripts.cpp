#include "MinimapFrame.h"
#include <cstdint>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Ui/SpellBookFrame.h"

int32_t Script_SetMaskTexture(lua_State* L);
int32_t Script_SetIconTexture(lua_State* L);
int32_t Script_SetBlipTexture(lua_State* L);
int32_t Script_SetClassBlipTexture(lua_State* L);
int32_t Script_SetPOIArrowTexture(lua_State* L);
int32_t Script_SetStaticPOIArrowTexture(lua_State* L);
int32_t Script_SetCorpsePOIArrowTexture(lua_State* L);
int32_t Script_SetPlayerTexture(lua_State* L);
int32_t Script_GetZoomLevels(lua_State* L);
int32_t Script_GetZoom(lua_State* L);
int32_t Script_SetZoom(lua_State* L);
int32_t Script_PingLocation(lua_State* L);
int32_t Script_GetPingPosition(lua_State* L);

// VERIFY: 756048df auto
// OFFSET: 0x0057C060
void CGMinimapFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CGMinimapFrame::s_metatable);
    CGMinimapFrame::s_metatable = LUA_NOREF;
}

// VERIFY: 6485707d auto
// OFFSET: 0x0057E340
void CGMinimapFrame::CreateScriptMetaTable() {
    CGMinimapFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CGMinimapFrame::RegisterScriptMethods);
}

// VERIFY: 36da42d2 auto
// OFFSET: 0x0057F170
static int32_t Script_GetNumTrackingTypes(lua_State* L) {
    uint32_t spells = s_trackingSpells.Count();
    lua_pushnumber(L, spells + CGMinimapFrame::CountAvailableTrackingTypes());
    return 1;
}

// VERIFY: e7934d4c auto
// OFFSET: 0x0057F4F0
static int32_t Script_GetTrackingTexture(lua_State* L) {
    char texture[260];

    if (CGMinimapFrame::s_trackingInfo) {
        SStrPrintf(texture, sizeof(texture), "%s\\%s", "Interface\\Minimap\\Tracking", CGMinimapFrame::s_trackingInfo->m_texture);
    } else {
        SpellRec spell;

        if (g_spellDB.GetRecord(CGMinimapFrame::s_trackingSpell, &spell)) {
            SpellIconRec* icon = g_spellIconDB.GetRecord(spell.m_activeIconID ? spell.m_activeIconID : spell.m_spellIconID);

            if (icon) {
                lua_pushstring(L, icon->m_textureFilename);
                return 1;
            }
        }

        SStrPrintf(texture, sizeof(texture), "%s\\%s", "Interface\\Minimap\\Tracking", "None");
    }

    lua_pushstring(L, texture);
    return 1;
}

// VERIFY: daed492b auto
// OFFSET: 0x0057E1C0
static int32_t Script_SetPlayerTextureHeight(lua_State* L) {
    CGMinimapFrame* minimap = static_cast<CGMinimapFrame*>(FrameScript_GetObjectThis(L, CGMinimapFrame::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = minimap->GetName();
        luaL_error(L, "Usage: %s:SetPlayerTextureHeight(height)", name ? name : "<unnamed>");
        return 0;
    }

    float height = lua_tonumber(L, 2);
    height = NDCToDDCWidth(height / (CoordinateGetAspectCompensation() * 1024.0f));

    if (minimap->m_playerTexture) {
        minimap->m_playerTexture->SetHeight(height);
    }

    return 0;
}

// VERIFY: e965c33d auto
// OFFSET: 0x0057E280
static int32_t Script_SetPlayerTextureWidth(lua_State* L) {
    CGMinimapFrame* minimap = static_cast<CGMinimapFrame*>(FrameScript_GetObjectThis(L, CGMinimapFrame::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = minimap->GetName();
        luaL_error(L, "Usage: %s:SetPlayerTextureWidth(Width)", name ? name : "<unnamed>");
        return 0;
    }

    float width = lua_tonumber(L, 2);
    width = NDCToDDCWidth(width / (CoordinateGetAspectCompensation() * 1024.0f));

    if (minimap->m_playerTexture) {
        minimap->m_playerTexture->SetWidth(width);
    }

    return 0;
}

// VERIFY: c8962cb4 manual
// OFFSET: 0x0057F1B0
static int32_t Script_GetTrackingInfo(lua_State* L) {
    int32_t index = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1;

    if (index >= static_cast<int32_t>(s_trackingSpells.Count())) {
        const TrackingTypeInfo* info = CGMinimapFrame::GetTrackingTypeByIndex(index - s_trackingSpells.Count());

        if (!info) {
            return 0;
        }

        lua_pushstring(L, FrameScript_GetText(info->m_name, -1, GENDER_NOT_APPLICABLE));
        char texture[128];
        SStrPrintf(texture, sizeof(texture), "%s\\%s", "Interface\\Minimap\\Tracking", info->m_texture);
        lua_pushstring(L, texture);

        if (CGMinimapFrame::s_trackingInfo == info) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        lua_pushstring(L, "other");
        return 4;
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(s_trackingSpells.m_data[index], &spell)) {
        return 0;
    }

    bool active = CGMinimapFrame::s_trackingSpell == spell.m_id;
    lua_pushstring(L, spell.m_name);
    SpellIconRec* icon = g_spellIconDB.GetRecord(active && spell.m_activeIconID ? spell.m_activeIconID : spell.m_spellIconID);

    if (icon) {
        lua_pushstring(L, icon->m_textureFilename);
    } else {
        lua_pushnil(L);
    }

    if (active) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushstring(L, "spell");
    return 4;
}

// STATIC: 0x00ACEBB8
static FrameScript_Method s_ScriptMethods[] = {
    { "SetMaskTexture", &Script_SetMaskTexture },
    { "SetIconTexture", &Script_SetIconTexture },
    { "SetBlipTexture", &Script_SetBlipTexture },
    { "SetClassBlipTexture", &Script_SetClassBlipTexture },
    { "SetPOIArrowTexture", &Script_SetPOIArrowTexture },
    { "SetStaticPOIArrowTexture", &Script_SetStaticPOIArrowTexture },
    { "SetCorpsePOIArrowTexture", &Script_SetCorpsePOIArrowTexture },
    { "SetPlayerTexture", &Script_SetPlayerTexture },
    { "SetPlayerTextureHeight", &Script_SetPlayerTextureHeight },
    { "SetPlayerTextureWidth", &Script_SetPlayerTextureWidth },
    { "GetZoomLevels", &Script_GetZoomLevels },
    { "GetZoom", &Script_GetZoom },
    { "SetZoom", &Script_SetZoom },
    { "PingLocation", &Script_PingLocation },
    { "GetPingPosition", &Script_GetPingPosition },
};

// VERIFY: acea5034 auto
// OFFSET: 0x0057C040
void CGMinimapFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}

// STATIC: 0x00ACEC30
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumTrackingTypes", &Script_GetNumTrackingTypes },
    { "GetTrackingInfo", &Script_GetTrackingInfo },
    { "SetTracking", &Script_SetTracking },
    { "GetTrackingTexture", &Script_GetTrackingTexture },
};

// VERIFY: 41ea4a09 auto
// OFFSET: 0x0057C090
void MinimapRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
