#ifndef WOW_SOURCE_UI_CKBQUERY_H
#define WOW_SOURCE_UI_CKBQUERY_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Ui/KnowledgeBase.h"

class CKBQuery : public CKBPage {
    public:
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
};

extern CKBQuery g_KnowledgeBaseQuery;

#endif
