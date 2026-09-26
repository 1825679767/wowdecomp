#ifndef WOW_SOURCE_DB_REC_ZONEMUSICREC_H
#define WOW_SOURCE_DB_REC_ZONEMUSICREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ZoneMusicRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_setName;
    /* 0008 */ int32_t m_silenceIntervalMin [2];
    /* 0010 */ int32_t m_silenceIntervalMax [2];
    /* 0018 */ int32_t m_sounds [2];

// VERIFY: 546515e1 auto
    // OFFSET: 0x008BCEA0
    static const char* GetFilename() {
        return "DBFilesClient\\ZoneMusic.dbc";
    }

// VERIFY: ff5b0fa1 manual
    // OFFSET: 0x008BCEB0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t setNameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t setNameRead = SFileReadFile(file, &setNameOffset, sizeof(setNameOffset), nullptr, nullptr, 0);
        int32_t silenceIntervalMinRead = SFileReadFile(file, this->m_silenceIntervalMin, sizeof(this->m_silenceIntervalMin), nullptr, nullptr, 0);
        int32_t silenceIntervalMaxRead = SFileReadFile(file, this->m_silenceIntervalMax, sizeof(this->m_silenceIntervalMax), nullptr, nullptr, 0);
        int32_t soundsRead = SFileReadFile(file, this->m_sounds, sizeof(this->m_sounds), nullptr, nullptr, 0);

        if (!idRead
            | !setNameRead
            | !silenceIntervalMinRead
            | !silenceIntervalMaxRead
            | !soundsRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_setName = &stringBuffer[setNameOffset];
        } else {
            this->m_setName = "";
        }

        return true;
    }
};

#endif
