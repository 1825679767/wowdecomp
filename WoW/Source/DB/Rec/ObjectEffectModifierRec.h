#ifndef WOW_SOURCE_DB_REC_OBJECTEFFECTMODIFIERREC_H
#define WOW_SOURCE_DB_REC_OBJECTEFFECTMODIFIERREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ObjectEffectModifierRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_inputType;
    /* 0008 */ int32_t m_mapType;
    /* 000C */ int32_t m_outputType;
    /* 0010 */ float m_param [4];

// VERIFY: 690cc52c auto
    // OFFSET: 0x008BD700
    static const char* GetFilename() {
        return "DBFilesClient\\ObjectEffectModifier.dbc";
    }

// VERIFY: 1103e23b manual
    // OFFSET: 0x008BD710
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t inputTypeRead = SFileReadFile(file, &this->m_inputType, sizeof(this->m_inputType), nullptr, nullptr, 0);
        int32_t mapTypeRead = SFileReadFile(file, &this->m_mapType, sizeof(this->m_mapType), nullptr, nullptr, 0);
        int32_t outputTypeRead = SFileReadFile(file, &this->m_outputType, sizeof(this->m_outputType), nullptr, nullptr, 0);
        int32_t paramRead = SFileReadFile(file, this->m_param, sizeof(this->m_param), nullptr, nullptr, 0);

        if (!idRead
            | !inputTypeRead
            | !mapTypeRead
            | !outputTypeRead
            | !paramRead) {
            return false;
        }

        return true;
    }
};

#endif
