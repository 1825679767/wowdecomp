#include "KnowledgeBase.h"
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Ui/CKBArticle.h"
#include "WoW/Source/Ui/CKBQuery.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00ACD538
CKBSetup g_KnowledgeBaseSetup;

// STATIC: 0x00BEA7F8
CKBPage* CKBPage::s_pLoadingQueue_Head;

// INLINED
CKBPage::CKBPage() {
    this->m_state = 0;
    this->m_data = nullptr;
    this->m_tree = nullptr;
    this->m_loadingNext = nullptr;
    this->m_inQueue = 0;
}

// VERIFY: 8d159cad manual
// OFFSET: 0x00550B80
bool CKBPage::LoadingCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error) {
    CKBPage* page = static_cast<CKBPage*>(param);

    if (!page) {
        return false;
    }

    page->m_lock.Enter();
    bool loading = page->m_state == 1;

    if (!done) {
        if (!data) {
            loading = false;
        }

        if (!size) {
            page->m_lock.Leave();
            return true;
        }
    }

    if (error || !loading) {
        if (error == 2) {
            page->m_state = 5;
        } else if (error == 3) {
            page->m_state = 6;
        } else {
            page->m_state = 4;
        }

        page->m_lock.Leave();
        return false;
    }

    if (!page->m_data) {
        page->m_data = new (SMemAlloc(sizeof(CDynamicString), __FILE__, __LINE__, 0)) CDynamicString();
        page->m_data->SetCapacity(0x8000);
    }

    if (!page->m_data) {
        page->m_state = 7;
        page->m_lock.Leave();
        return false;
    }

    page->m_data->Append(data, size);

    if (!done) {
        page->m_lock.Leave();
        return true;
    }

    if (page->m_tree) {
        XMLTree_Free(page->m_tree);
    }

    page->m_tree = XMLTree_Load(page->m_data->m_buffer, page->m_data->m_length);
    CDynamicString* buffer = page->m_data;

    if (buffer) {
        buffer->~CDynamicString();
        SMemFree(buffer, "delete", SLOG_FUNCTION, 0);
    }

    page->m_data = nullptr;

    if (!page->m_tree) {
        page->m_state = 8;
        page->m_lock.Leave();
        return false;
    }

    page->_ComputeCachedData();
    page->m_state = 2;
    page->m_lock.Leave();
    return true;
}

// VERIFY: b649a859 auto
// OFFSET: 0x00550E10
int32_t CKBSetup::BeginLoading(int32_t numArticles, int32_t pageNumber) {
    if (this->m_state == 1 || this->m_inQueue) {
        return 0;
    }

    CDynamicString url;
    url.SetCapacity(1024);
    url += KnowledgeBaseGetURLForLocale();
    url += "getKBSetup.xml";
    url += "?languageId=";
    url += s_locale;
    url += "&numArticles=";
    url += numArticles;
    url += "&pageNumber=";
    url += pageNumber;
    url += "&locale=";
    url += s_localeCode;

    if (!OsURLDownload(url.m_buffer, &CKBPage::LoadingCallback, this)) {
        this->m_state = 3;
        this->_SendFailureEvent();
        return 0;
    }

    this->unk_0050 = -1;
    this->unk_0054 = 0;
    this->unk_005C = 0;
    this->unk_0064 = 0;
    this->unk_006C = 0;
    this->m_state = 1;
    this->m_loadingNext = CKBPage::s_pLoadingQueue_Head;
    CKBPage::s_pLoadingQueue_Head = this;
    this->m_inQueue = 1;
    return 1;
}

// VERIFY: 772164f4 auto
// OFFSET: 0x005526D0
CKBPage::~CKBPage() {
    if (this->m_tree) {
        XMLTree_Free(this->m_tree);
        XMLTree_Cleanup();
    }

    if (this->m_data) {
        CDynamicString* data = this->m_data;
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

// VERIFY: c721f005 auto
// OFFSET: 0x00550940
const char* KnowledgeBaseGetURLForLocale() {
    switch (LocaleNameToIndex(s_localeCode)) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 11:
            return "http://support.wow-europe.com/kb/";

        case 5:
            return "http://support.worldofwarcraft.co.kr/kb/";

        case 6:
        case 8:
            return "http://cn.kbase.blizzard.com/kb/wow/";

        case 7:
        case 9:
            return "http://support.wowtaiwan.com.tw/kb/";

        default:
            return "http://support.worldofwarcraft.com/kb/";
    }
}

// VERIFY: 8a5437bf manual
// OFFSET: 0x00550A80
void CKBPage::UpdateLoadingQueue() {
    CKBPage* prev = nullptr;
    CKBPage* page = CKBPage::s_pLoadingQueue_Head;

    while (page) {
        if (page->m_state == 1) {
            prev = page;
            page = page->m_loadingNext;
            continue;
        }

        if (prev) {
            prev->m_loadingNext = page->m_loadingNext;
        } else {
            CKBPage::s_pLoadingQueue_Head = page->m_loadingNext;
        }

        if (page->m_state == 2) {
            page->_SendSuccessEvent();
        } else {
            page->_SendFailureEvent();
        }

        CKBPage* next = page->m_loadingNext;
        page->m_inQueue = 0;
        page->m_loadingNext = nullptr;
        page = next;
    }
}

// VERIFY: c9bdf799 manual
// OFFSET: 0x00552740
void CKBPage::Shutdown() {
    while (CKBPage::s_pLoadingQueue_Head) {
        OsSleep(1);
        CKBPage::UpdateLoadingQueue();
    }

    if (g_KnowledgeBaseSetup.m_tree) {
        XMLTree_Free(g_KnowledgeBaseSetup.m_tree);
        XMLTree_Cleanup();
        g_KnowledgeBaseSetup.m_tree = nullptr;
    }

    if (g_KnowledgeBaseSetup.m_data) {
        CDynamicString* data = g_KnowledgeBaseSetup.m_data;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseSetup.m_data = nullptr;
    }

    g_KnowledgeBaseSetup.m_state = 0;

    if (g_KnowledgeBaseQuery.m_tree) {
        XMLTree_Free(g_KnowledgeBaseQuery.m_tree);
        XMLTree_Cleanup();
        g_KnowledgeBaseQuery.m_tree = nullptr;
    }

    if (g_KnowledgeBaseQuery.m_data) {
        CDynamicString* data = g_KnowledgeBaseQuery.m_data;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseQuery.m_data = nullptr;
    }

    g_KnowledgeBaseQuery.m_state = 0;

    if (g_KnowledgeBaseArticle.m_tree) {
        XMLTree_Free(g_KnowledgeBaseArticle.m_tree);
        XMLTree_Cleanup();
        g_KnowledgeBaseArticle.m_tree = nullptr;
    }

    if (g_KnowledgeBaseArticle.m_data) {
        CDynamicString* data = g_KnowledgeBaseArticle.m_data;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseArticle.m_data = nullptr;
    }

    g_KnowledgeBaseArticle.m_state = 0;
}
