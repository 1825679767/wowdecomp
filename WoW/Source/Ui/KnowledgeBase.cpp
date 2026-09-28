#include "KnowledgeBase.h"
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Ui/CKBArticle.h"
#include "WoW/Source/Ui/CKBQuery.h"

// STATIC: 0x00ACD538
CKBSetup g_KnowledgeBaseSetup;

// STATIC: 0x00BEA7F8
CKBPage* CKBPage::s_pLoadingQueue_Head;

// INLINED
CKBPage::CKBPage() {
    this->unk_0004 = 0;
    this->unk_0030 = nullptr;
    this->unk_0034 = nullptr;
    this->m_loadingNext = nullptr;
    this->unk_003C = 0;
}

// VERIFY: a93a16db auto
// OFFSET: 0x005526D0
CKBPage::~CKBPage() {
    if (this->unk_0034) {
        XMLTree_Free(this->unk_0034);
        XMLTree_Cleanup();
    }

    if (this->unk_0030) {
        CDynamicString* data = this->unk_0030;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
    }
}

void CKBPage::_ComputeCachedData() {
}

void CKBPage::_SendSuccessEvent() {
}

void CKBPage::_SendFailureEvent() {
}

// INLINED
CKBSetup::CKBSetup() {
    this->unk_0040 = 0;
    this->unk_0044 = 0;
    this->unk_0048 = 0;
    this->unk_004C = 0;
    this->unk_0050 = -1;
    this->unk_0054 = 0;
    this->unk_0058 = 0;
    this->unk_005C = 0;
    this->unk_0060 = 0;
    this->unk_0064 = 0;
    this->unk_0068 = 0;
    this->unk_006C = 0;
    this->unk_0070 = 0;
}

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

// VERIFY: d1c1f4ac manual
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
