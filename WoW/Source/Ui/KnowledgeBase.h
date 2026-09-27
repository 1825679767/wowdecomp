#ifndef WOW_SOURCE_UI_KNOWLEDGEBASE_H
#define WOW_SOURCE_UI_KNOWLEDGEBASE_H

#include <cstddef>
#include <cstdint>

class CDynamicString;
struct XMLTree;

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

    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[4];
    /* 000C */ uint8_t unk_000C;
    /* 000D */ uint8_t unk_000D;
    /* 000E */ uint8_t unk_000E;
    /* 000F */ uint8_t unk_000F[4];
    /* 0013 */ uint8_t unk_0013[29];
    /* 0030 */ CDynamicString* unk_0030;
    /* 0034 */ XMLTree* unk_0034;
    /* 0038 */ CKBPage* m_loadingNext;
    /* 003C */ uint32_t unk_003C;

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
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint8_t unk_0058[4];
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint8_t unk_0060[4];
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
};

extern CKBSetup g_KnowledgeBaseSetup;

#endif
