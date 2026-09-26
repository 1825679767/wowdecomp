#ifndef WOW_SOURCE_DB_REC_PARTICLECOLORREC_H
#define WOW_SOURCE_DB_REC_PARTICLECOLORREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ParticleColorRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_start [3];
    /* 0010 */ int32_t m_mid [3];
    /* 001C */ int32_t m_end [3];

// VERIFY: 69666f2b auto
    // OFFSET: 0x008B4D70
    static const char* GetFilename() {
        return "DBFilesClient\\ParticleColor.dbc";
    }

// VERIFY: 69e79c4b manual
    // OFFSET: 0x008B4D80
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t startRead = SFileReadFile(file, this->m_start, sizeof(this->m_start), nullptr, nullptr, 0);
        int32_t midRead = SFileReadFile(file, this->m_mid, sizeof(this->m_mid), nullptr, nullptr, 0);
        int32_t endRead = SFileReadFile(file, this->m_end, sizeof(this->m_end), nullptr, nullptr, 0);

        if (!idRead
            | !startRead
            | !midRead
            | !endRead) {
            return false;
        }

        return true;
    }
};

#endif
