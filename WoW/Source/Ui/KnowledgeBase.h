#ifndef WOW_SOURCE_UI_KNOWLEDGEBASE_H
#define WOW_SOURCE_UI_KNOWLEDGEBASE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Os/OsURLDownload.h"
#include "Engine/Source/Storm/Source/SLock.h"

class CDynamicString;
struct XMLTree;
struct lua_State;

class CGKnowledgeBase {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[44];
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint8_t unk_0038[8];
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint8_t unk_0044[4];
    /* 0048 */ uint32_t unk_0048;
};

class CKBPage {
    public:
    static CKBPage* s_pLoadingQueue_Head;

    static void Initialize();
    static void Shutdown();
    static void UpdateLoadingQueue();

    static bool LoadingCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error);

    /* 0004 */ uint32_t m_state;
    /* 0008 */ SCritSect m_lock;
    /* 0030 */ CDynamicString* m_data;
    /* 0034 */ XMLTree* m_tree;
    /* 0038 */ CKBPage* m_loadingNext;
    /* 003C */ uint32_t m_inQueue;

    CKBPage();
    virtual ~CKBPage();
    virtual void _ComputeCachedData();
    virtual void _SendSuccessEvent();
    virtual void _SendFailureEvent();
};

class CKBSetup : public CKBPage {
    public:
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;

    CKBSetup();
    int32_t BeginLoading(int32_t numArticles, int32_t pageNumber);
    virtual void _ComputeCachedData();
    virtual void _SendSuccessEvent();
    virtual void _SendFailureEvent();
};

extern CKBSetup g_KnowledgeBaseSetup;

int32_t Script_KBSetup_GetLanguageData(lua_State* L);
int32_t Script_KBSetup_GetCategoryData(lua_State* L);
int32_t Script_KBSetup_GetSubCategoryCount(lua_State* L);
int32_t Script_KBSetup_GetSubCategoryData(lua_State* L);
int32_t Script_KBSetup_GetArticleHeaderData(lua_State* L);
int32_t Script_KBSetup_GetTotalArticleCount(lua_State* L);
int32_t Script_KBQuery_BeginLoading(lua_State* L);
int32_t Script_KBQuery_GetArticleHeaderData(lua_State* L);
int32_t Script_KBQuery_GetTotalArticleCount(lua_State* L);
int32_t Script_KBArticle_BeginLoading(lua_State* L);
int32_t Script_KBArticle_GetData(lua_State* L);
const char* KnowledgeBaseGetURLForLocale();
void KnowledgeBase_RegisterScriptFunctions();
void KnowledgeBase_UnregisterScriptFunctions();

#endif
