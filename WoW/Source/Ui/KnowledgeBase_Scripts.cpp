#include "KnowledgeBase.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

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
