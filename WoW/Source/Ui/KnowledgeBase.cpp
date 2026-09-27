#include "KnowledgeBase.h"
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Ui/CKBArticle.h"
#include "WoW/Source/Ui/CKBQuery.h"

// STATIC: 0x00BEA7F8
CKBPage* CKBPage::s_pLoadingQueue_Head;

// VERIFY: 1b2d7a48 manual
// OFFSET: 0x005EEB70
void CKBPage::Initialize() {
}

// VERIFY: 820f1773 manual
// OFFSET: 0x00550A80
void CKBPage::UpdateLoadingQueue() {
    CKBPage* prev = nullptr;
    CKBPage* page = CKBPage::s_pLoadingQueue_Head;

    while (page) {
        if (page->unk_0004 == 1) {
            prev = page;
            page = page->m_loadingNext;
            continue;
        }

        if (prev) {
            prev->m_loadingNext = page->m_loadingNext;
        } else {
            CKBPage::s_pLoadingQueue_Head = page->m_loadingNext;
        }

        if (page->unk_0004 == 2) {
            page->_SendSuccessEvent();
        } else {
            page->_SendFailureEvent();
        }

        CKBPage* next = page->m_loadingNext;
        page->unk_003C = 0;
        page->m_loadingNext = nullptr;
        page = next;
    }
}

// OFFSET: 0x00552740
void CKBPage::Shutdown() {
    while (CKBPage::s_pLoadingQueue_Head) {
        OsSleep(1);
        CKBPage::UpdateLoadingQueue();
    }

    if (g_KnowledgeBaseSetup.unk_0034) {
        XMLTree_Free(g_KnowledgeBaseSetup.unk_0034);
        XMLTree_Cleanup();
        g_KnowledgeBaseSetup.unk_0034 = nullptr;
    }

    if (g_KnowledgeBaseSetup.unk_0030) {
        CDynamicString* data = g_KnowledgeBaseSetup.unk_0030;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseSetup.unk_0030 = nullptr;
    }

    g_KnowledgeBaseSetup.unk_0004 = 0;

    if (g_KnowledgeBaseQuery.unk_0034) {
        XMLTree_Free(g_KnowledgeBaseQuery.unk_0034);
        XMLTree_Cleanup();
        g_KnowledgeBaseQuery.unk_0034 = nullptr;
    }

    if (g_KnowledgeBaseQuery.unk_0030) {
        CDynamicString* data = g_KnowledgeBaseQuery.unk_0030;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseQuery.unk_0030 = nullptr;
    }

    g_KnowledgeBaseQuery.unk_0004 = 0;

    if (g_KnowledgeBaseArticle.unk_0034) {
        XMLTree_Free(g_KnowledgeBaseArticle.unk_0034);
        XMLTree_Cleanup();
        g_KnowledgeBaseArticle.unk_0034 = nullptr;
    }

    if (g_KnowledgeBaseArticle.unk_0030) {
        CDynamicString* data = g_KnowledgeBaseArticle.unk_0030;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseArticle.unk_0030 = nullptr;
    }

    g_KnowledgeBaseArticle.unk_0004 = 0;
}
