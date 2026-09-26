#ifndef WOW_SOURCE_DB_REC_PAPERDOLLITEMFRAMEREC_H
#define WOW_SOURCE_DB_REC_PAPERDOLLITEMFRAMEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct PaperDollItemFrameRec {
    /* 0000 */ const char* m_itemButtonName;
    /* 0004 */ const char* m_slotIcon;
    /* 0008 */ int32_t m_slotNumber;
    /* 000C */ int32_t m_generatedID;

// VERIFY: 77e86987 auto
    // OFFSET: 0x008B4CB0
    static const char* GetFilename() {
        return "DBFilesClient\\PaperDollItemFrame.dbc";
    }

// VERIFY: 974c3d94 auto
    // OFFSET: 0x008B4CC0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t itemButtonNameOffset;
        uint32_t slotIconOffset;

        int32_t itemButtonNameRead = SFileReadFile(file, &itemButtonNameOffset, sizeof(itemButtonNameOffset), nullptr, nullptr, 0);
        int32_t slotIconRead = SFileReadFile(file, &slotIconOffset, sizeof(slotIconOffset), nullptr, nullptr, 0);
        int32_t slotNumberRead = SFileReadFile(file, &this->m_slotNumber, sizeof(this->m_slotNumber), nullptr, nullptr, 0);

        if (!itemButtonNameRead
            | !slotIconRead
            | !slotNumberRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_itemButtonName = &stringBuffer[itemButtonNameOffset];
            this->m_slotIcon = &stringBuffer[slotIconOffset];
        } else {
            this->m_itemButtonName = "";
            this->m_slotIcon = "";
        }

        return true;
    }
};

#endif
