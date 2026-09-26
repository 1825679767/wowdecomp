#ifndef WOW_SOURCE_DB_REC_AREAGROUPREC_H
#define WOW_SOURCE_DB_REC_AREAGROUPREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct AreaGroupRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_areaID [6];
    /* 001C */ int32_t m_nextAreaID;

// VERIFY: 57df36f1 auto
    // OFFSET: 0x008A9520
    static const char* GetFilename() {
        return "DBFilesClient\\AreaGroup.dbc";
    }

// VERIFY: 2c41d9f0 manual
    // OFFSET: 0x008A9530
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t areaIDRead = SFileReadFile(file, this->m_areaID, sizeof(this->m_areaID), nullptr, nullptr, 0);
        int32_t nextAreaIDRead = SFileReadFile(file, &this->m_nextAreaID, sizeof(this->m_nextAreaID), nullptr, nullptr, 0);

        if (!idRead
            | !areaIDRead
            | !nextAreaIDRead) {
            return false;
        }

        return true;
    }
};

#endif
