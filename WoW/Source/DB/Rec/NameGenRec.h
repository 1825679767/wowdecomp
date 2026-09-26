#ifndef WOW_SOURCE_DB_REC_NAMEGENREC_H
#define WOW_SOURCE_DB_REC_NAMEGENREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct NameGenRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_raceID;
    /* 000C */ int32_t m_sex;

// VERIFY: ae88b455 auto
    // OFFSET: 0x008B4810
    static const char* GetFilename() {
        return "DBFilesClient\\NameGen.dbc";
    }

// VERIFY: 83392d61 manual
    // OFFSET: 0x008B4820
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
        int32_t raceIDRead = SFileReadFile(file, &this->m_raceID, sizeof(this->m_raceID), nullptr, nullptr, 0);
        int32_t sexRead = SFileReadFile(file, &this->m_sex, sizeof(this->m_sex), nullptr, nullptr, 0);

        if (!idRead
            | !nameRead
            | !raceIDRead
            | !sexRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_name = &stringBuffer[nameOffset];
        } else {
            this->m_name = "";
        }

        return true;
    }
};

#endif
