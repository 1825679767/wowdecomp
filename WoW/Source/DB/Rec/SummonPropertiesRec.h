#ifndef WOW_SOURCE_DB_REC_SUMMONPROPERTIESREC_H
#define WOW_SOURCE_DB_REC_SUMMONPROPERTIESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SummonPropertiesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_control;
    /* 0008 */ int32_t m_faction;
    /* 000C */ int32_t m_title;
    /* 0010 */ int32_t m_slot;
    /* 0014 */ int32_t m_flags;

// VERIFY: 640e0ff5 auto
    // OFFSET: 0x008B9950
    static const char* GetFilename() {
        return "DBFilesClient\\SummonProperties.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t controlRead = SFileReadFile(file, &this->m_control, sizeof(this->m_control), nullptr, nullptr, 0);
        int32_t factionRead = SFileReadFile(file, &this->m_faction, sizeof(this->m_faction), nullptr, nullptr, 0);
        int32_t titleRead = SFileReadFile(file, &this->m_title, sizeof(this->m_title), nullptr, nullptr, 0);
        int32_t slotRead = SFileReadFile(file, &this->m_slot, sizeof(this->m_slot), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!idRead
            | !controlRead
            | !factionRead
            | !titleRead
            | !slotRead
            | !flagsRead) {
            return false;
        }

        return true;
    }
};

#endif
