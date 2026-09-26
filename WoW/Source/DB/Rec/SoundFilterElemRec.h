#ifndef WOW_SOURCE_DB_REC_SOUNDFILTERELEMREC_H
#define WOW_SOURCE_DB_REC_SOUNDFILTERELEMREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SoundFilterElemRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundFilterID;
    /* 0008 */ int32_t m_orderIndex;
    /* 000C */ int32_t m_filterType;
    /* 0010 */ float m_params [9];

// VERIFY: 96d1fb52 auto
    // OFFSET: 0x008BD7F0
    static const char* GetFilename() {
        return "DBFilesClient\\SoundFilterElem.dbc";
    }

// VERIFY: 9210fa5f manual
    // OFFSET: 0x008BD800
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t soundFilterIDRead = SFileReadFile(file, &this->m_soundFilterID, sizeof(this->m_soundFilterID), nullptr, nullptr, 0);
        int32_t orderIndexRead = SFileReadFile(file, &this->m_orderIndex, sizeof(this->m_orderIndex), nullptr, nullptr, 0);
        int32_t filterTypeRead = SFileReadFile(file, &this->m_filterType, sizeof(this->m_filterType), nullptr, nullptr, 0);
        int32_t paramsRead = SFileReadFile(file, this->m_params, sizeof(this->m_params), nullptr, nullptr, 0);

        if (!idRead
            | !soundFilterIDRead
            | !orderIndexRead
            | !filterTypeRead
            | !paramsRead) {
            return false;
        }

        return true;
    }
};

#endif
