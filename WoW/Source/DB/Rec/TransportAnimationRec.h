#ifndef WOW_SOURCE_DB_REC_TRANSPORTANIMATIONREC_H
#define WOW_SOURCE_DB_REC_TRANSPORTANIMATIONREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct TransportAnimationRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_transportID;
    /* 0008 */ int32_t m_timeIndex;
    /* 000C */ float m_pos [3];
    /* 0018 */ int32_t m_sequenceID;

// VERIFY: 0e320e2d auto
    // OFFSET: 0x008BA590
    static const char* GetFilename() {
        return "DBFilesClient\\TransportAnimation.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t transportIDRead = SFileReadFile(file, &this->m_transportID, sizeof(this->m_transportID), nullptr, nullptr, 0);
        int32_t timeIndexRead = SFileReadFile(file, &this->m_timeIndex, sizeof(this->m_timeIndex), nullptr, nullptr, 0);
        int32_t posRead0 = SFileReadFile(file, &this->m_pos[0], sizeof(this->m_pos[0]), nullptr, nullptr, 0);
        int32_t posRead1 = SFileReadFile(file, &this->m_pos[1], sizeof(this->m_pos[1]), nullptr, nullptr, 0);
        int32_t posRead2 = SFileReadFile(file, &this->m_pos[2], sizeof(this->m_pos[2]), nullptr, nullptr, 0);
        int32_t sequenceIDRead = SFileReadFile(file, &this->m_sequenceID, sizeof(this->m_sequenceID), nullptr, nullptr, 0);

        if (!idRead
            | !transportIDRead
            | !timeIndexRead
            | !posRead0
            | !posRead1
            | !posRead2
            | !sequenceIDRead) {
            return false;
        }

        return true;
    }
};

#endif
