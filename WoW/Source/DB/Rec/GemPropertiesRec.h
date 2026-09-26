#ifndef WOW_SOURCE_DB_REC_GEMPROPERTIESREC_H
#define WOW_SOURCE_DB_REC_GEMPROPERTIESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct GemPropertiesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_enchantID;
    /* 0008 */ int32_t m_maxcountInv;
    /* 000C */ int32_t m_maxcountItem;
    /* 0010 */ int32_t m_type;

// VERIFY: e625ac0e auto
    // OFFSET: 0x008AFDE0
    static const char* GetFilename() {
        return "DBFilesClient\\GemProperties.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t enchantIDRead = SFileReadFile(file, &this->m_enchantID, sizeof(this->m_enchantID), nullptr, nullptr, 0);
        int32_t maxcountInvRead = SFileReadFile(file, &this->m_maxcountInv, sizeof(this->m_maxcountInv), nullptr, nullptr, 0);
        int32_t maxcountItemRead = SFileReadFile(file, &this->m_maxcountItem, sizeof(this->m_maxcountItem), nullptr, nullptr, 0);
        int32_t typeRead = SFileReadFile(file, &this->m_type, sizeof(this->m_type), nullptr, nullptr, 0);

        if (!idRead
            | !enchantIDRead
            | !maxcountInvRead
            | !maxcountItemRead
            | !typeRead) {
            return false;
        }

        return true;
    }
};

#endif
