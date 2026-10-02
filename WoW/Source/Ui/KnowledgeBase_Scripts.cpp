#include "KnowledgeBase.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Ui/CKBSystem.h"
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/CKBQuery.h"
#include "WoW/Source/Ui/CKBArticle.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// VERIFY: 9e42057a auto
// OFFSET: 0x00551FB0
static int32_t Script_KBSystem_GetMOTD(lua_State* L) {
    lua_pushstring(L, s_motd.m_buffer);
    return 1;
}

// VERIFY: 813271c5 auto
// OFFSET: 0x00551FD0
static int32_t Script_KBSystem_GetServerStatus(lua_State* L) {
    lua_pushstring(L, s_serverStatus.m_buffer);
    return 1;
}

// VERIFY: 88390c3b auto
// OFFSET: 0x00551FF0
static int32_t Script_KBSystem_GetServerNotice(lua_State* L) {
    lua_pushstring(L, s_serverNotice.m_buffer);
    return 1;
}

// VERIFY: 64710f8f auto
// OFFSET: 0x00551720
static int32_t Script_KBSetup_BeginLoading(lua_State* L) {
    int32_t numArticles = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t pageNumber = static_cast<int32_t>(lua_tonumber(L, 2));
    g_KnowledgeBaseSetup.BeginLoading(numArticles, pageNumber);
    return 0;
}

// VERIFY: e0016275 auto
// OFFSET: 0x00551760
static int32_t Script_KBSetup_IsLoaded(lua_State* L) {
    lua_pushboolean(L, g_KnowledgeBaseSetup.m_state == 2);
    return 1;
}

// VERIFY: bc5e561c auto
// OFFSET: 0x00551B90
static int32_t Script_KBQuery_IsLoaded(lua_State* L) {
    lua_pushboolean(L, g_KnowledgeBaseQuery.m_state == 2);
    return 1;
}

// VERIFY: 1d0a0bbd auto
// OFFSET: 0x00551DA0
static int32_t Script_KBArticle_IsLoaded(lua_State* L) {
    lua_pushboolean(L, g_KnowledgeBaseArticle.m_state == 2);
    return 1;
}

// VERIFY: 755e1b76 auto
// OFFSET: 0x00551790
static int32_t Script_KBSetup_GetLanguageCount(lua_State* L) {
    if (g_KnowledgeBaseSetup.m_state != 2) {
        return luaL_error(L, "KBSetup_GetLanguageCount() failed because setup is not loaded");
    }

    lua_pushnumber(L, g_KnowledgeBaseSetup.unk_0040);
    return 1;
}

// VERIFY: 4a05c9da auto
// OFFSET: 0x005518C0
static int32_t Script_KBSetup_GetCategoryCount(lua_State* L) {
    if (g_KnowledgeBaseSetup.m_state != 2) {
        return luaL_error(L, "KBSetup_GetCategoryCount() failed because setup is not loaded");
    }

    lua_pushnumber(L, g_KnowledgeBaseSetup.unk_0044);
    return 1;
}

// VERIFY: e7e8714f auto
// OFFSET: 0x005519F0
static int32_t Script_KBSetup_GetArticleHeaderCount(lua_State* L) {
    if (g_KnowledgeBaseSetup.m_state != 2) {
        return luaL_error(L, "KBSetup_GetArticleHeaderCount() failed because setup is not loaded");
    }

    lua_pushnumber(L, g_KnowledgeBaseSetup.unk_004C);
    return 1;
}

// VERIFY: 6c7bcdb3 auto
// OFFSET: 0x00551BC0
static int32_t Script_KBQuery_GetArticleHeaderCount(lua_State* L) {
    if (g_KnowledgeBaseQuery.m_state != 2) {
        return luaL_error(L, "KBQuery_GetArticleHeaderCount() failed because query is not loaded");
    }

    lua_pushnumber(L, g_KnowledgeBaseQuery.unk_0040);
    return 1;
}

// STATIC: 0x00ACD488
static FrameScript_Method s_apMethodMap[] = {
    { "KBSetup_BeginLoading", &Script_KBSetup_BeginLoading },
    { "KBSetup_IsLoaded", &Script_KBSetup_IsLoaded },
    { "KBSetup_GetLanguageCount", &Script_KBSetup_GetLanguageCount },
    { "KBSetup_GetLanguageData", &Script_KBSetup_GetLanguageData },
    { "KBSetup_GetCategoryCount", &Script_KBSetup_GetCategoryCount },
    { "KBSetup_GetCategoryData", &Script_KBSetup_GetCategoryData },
    { "KBSetup_GetSubCategoryCount", &Script_KBSetup_GetSubCategoryCount },
    { "KBSetup_GetSubCategoryData", &Script_KBSetup_GetSubCategoryData },
    { "KBSetup_GetArticleHeaderCount", &Script_KBSetup_GetArticleHeaderCount },
    { "KBSetup_GetArticleHeaderData", &Script_KBSetup_GetArticleHeaderData },
    { "KBSetup_GetTotalArticleCount", &Script_KBSetup_GetTotalArticleCount },
    { "KBQuery_BeginLoading", &Script_KBQuery_BeginLoading },
    { "KBQuery_IsLoaded", &Script_KBQuery_IsLoaded },
    { "KBQuery_GetArticleHeaderCount", &Script_KBQuery_GetArticleHeaderCount },
    { "KBQuery_GetArticleHeaderData", &Script_KBQuery_GetArticleHeaderData },
    { "KBQuery_GetTotalArticleCount", &Script_KBQuery_GetTotalArticleCount },
    { "KBArticle_BeginLoading", &Script_KBArticle_BeginLoading },
    { "KBArticle_IsLoaded", &Script_KBArticle_IsLoaded },
    { "KBArticle_GetData", &Script_KBArticle_GetData },
    { "KBSystem_GetMOTD", &Script_KBSystem_GetMOTD },
    { "KBSystem_GetServerStatus", &Script_KBSystem_GetServerStatus },
    { "KBSystem_GetServerNotice", &Script_KBSystem_GetServerNotice },
};

// VERIFY: a3329701 auto
// OFFSET: 0x00552010
void KnowledgeBase_RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_apMethodMap) / sizeof(s_apMethodMap[0]); i++) {
        FrameScript_RegisterFunction(s_apMethodMap[i].m_name, s_apMethodMap[i].m_method);
    }
}

// VERIFY: 25924890 auto
// OFFSET: 0x00552040
void KnowledgeBase_UnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_apMethodMap) / sizeof(s_apMethodMap[0]); i++) {
        FrameScript_UnregisterFunction(s_apMethodMap[i].m_name);
    }
}
