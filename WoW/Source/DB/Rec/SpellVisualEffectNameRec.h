#ifndef WOW_SOURCE_DB_REC_SPELLVISUALEFFECTNAMEREC_H
#define WOW_SOURCE_DB_REC_SPELLVISUALEFFECTNAMEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellVisualEffectNameRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_fileName;
    /* 000C */ float m_areaEffectSize;
    /* 0010 */ float m_scale;
    /* 0014 */ float m_minAllowedScale;
    /* 0018 */ float m_maxAllowedScale;

// VERIFY: bb1a956d auto
    // OFFSET: 0x008B8FD0
    static const char* GetFilename() {
        return "DBFilesClient\\SpellVisualEffectName.dbc";
    }

// VERIFY: 5b2c356f manual
    // OFFSET: 0x008B8FE0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;
        uint32_t fileNameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
        int32_t fileNameRead = SFileReadFile(file, &fileNameOffset, sizeof(fileNameOffset), nullptr, nullptr, 0);
        int32_t areaEffectSizeRead = SFileReadFile(file, &this->m_areaEffectSize, sizeof(this->m_areaEffectSize), nullptr, nullptr, 0);
        int32_t scaleRead = SFileReadFile(file, &this->m_scale, sizeof(this->m_scale), nullptr, nullptr, 0);
        int32_t minAllowedScaleRead = SFileReadFile(file, &this->m_minAllowedScale, sizeof(this->m_minAllowedScale), nullptr, nullptr, 0);
        int32_t maxAllowedScaleRead = SFileReadFile(file, &this->m_maxAllowedScale, sizeof(this->m_maxAllowedScale), nullptr, nullptr, 0);

        if (!idRead
            | !nameRead
            | !fileNameRead
            | !areaEffectSizeRead
            | !scaleRead
            | !minAllowedScaleRead
            | !maxAllowedScaleRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_name = &stringBuffer[nameOffset];
            this->m_fileName = &stringBuffer[fileNameOffset];
        } else {
            this->m_name = "";
            this->m_fileName = "";
        }

        return true;
    }
};

#endif
