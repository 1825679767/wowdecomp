#ifndef WOW_SOURCE_DB_REC_ATTACKANIMKITSREC_H
#define WOW_SOURCE_DB_REC_ATTACKANIMKITSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct AttackAnimKitsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemSubclassID;
    /* 0008 */ int32_t m_animTypeID;
    /* 000C */ int32_t m_animFrequency;
    /* 0010 */ int32_t m_whichHand;

// VERIFY: 3466c338 auto
    // OFFSET: 0x008AA0F0
    static const char* GetFilename() {
        return "DBFilesClient\\AttackAnimKits.dbc";
    }

// VERIFY: 38e7fbc3 manual
    // OFFSET: 0x008B8BA0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t itemSubclassIDRead = SFileReadFile(file, &this->m_itemSubclassID, sizeof(this->m_itemSubclassID), nullptr, nullptr, 0);
        int32_t animTypeIDRead = SFileReadFile(file, &this->m_animTypeID, sizeof(this->m_animTypeID), nullptr, nullptr, 0);
        int32_t animFrequencyRead = SFileReadFile(file, &this->m_animFrequency, sizeof(this->m_animFrequency), nullptr, nullptr, 0);
        int32_t whichHandRead = SFileReadFile(file, &this->m_whichHand, sizeof(this->m_whichHand), nullptr, nullptr, 0);

        if (!idRead
            | !itemSubclassIDRead
            | !animTypeIDRead
            | !animFrequencyRead
            | !whichHandRead) {
            return false;
        }

        return true;
    }
};

#endif
