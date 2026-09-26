#ifndef WOW_SOURCE_DB_REC_TRANSPORTROTATIONREC_H
#define WOW_SOURCE_DB_REC_TRANSPORTROTATIONREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct TransportRotationRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_gameObjectsID;
    /* 0008 */ int32_t m_timeIndex;
    /* 000C */ float m_rot [4];

// VERIFY: d4a97d03 auto
    // OFFSET: 0x008BA720
    static const char* GetFilename() {
        return "DBFilesClient\\TransportRotation.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t gameObjectsIDRead = SFileReadFile(file, &this->m_gameObjectsID, sizeof(this->m_gameObjectsID), nullptr, nullptr, 0);
        int32_t timeIndexRead = SFileReadFile(file, &this->m_timeIndex, sizeof(this->m_timeIndex), nullptr, nullptr, 0);
        int32_t rotRead0 = SFileReadFile(file, &this->m_rot[0], sizeof(this->m_rot[0]), nullptr, nullptr, 0);
        int32_t rotRead1 = SFileReadFile(file, &this->m_rot[1], sizeof(this->m_rot[1]), nullptr, nullptr, 0);
        int32_t rotRead2 = SFileReadFile(file, &this->m_rot[2], sizeof(this->m_rot[2]), nullptr, nullptr, 0);
        int32_t rotRead3 = SFileReadFile(file, &this->m_rot[3], sizeof(this->m_rot[3]), nullptr, nullptr, 0);

        if (!idRead
            | !gameObjectsIDRead
            | !timeIndexRead
            | !rotRead0
            | !rotRead1
            | !rotRead2
            | !rotRead3) {
            return false;
        }

        return true;
    }
};

#endif
