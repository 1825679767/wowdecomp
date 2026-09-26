#ifndef WOW_SOURCE_DB_REC_POWERDISPLAYREC_H
#define WOW_SOURCE_DB_REC_POWERDISPLAYREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct PowerDisplayRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_actualType;
    /* 0008 */ const char* m_globalStringBaseTag;
    /* 000C */ int8_t m_red;
    /* 000D */ int8_t m_green;
    /* 000E */ int8_t m_blue;

// VERIFY: 5eda3753 auto
    // OFFSET: 0x008B50E0
    static const char* GetFilename() {
        return "DBFilesClient\\PowerDisplay.dbc";
    }

// VERIFY: e4612993 manual
    // OFFSET: 0x008B50F0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t globalStringBaseTagOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t actualTypeRead = SFileReadFile(file, &this->m_actualType, sizeof(this->m_actualType), nullptr, nullptr, 0);
        int32_t globalStringBaseTagRead = SFileReadFile(file, &globalStringBaseTagOffset, sizeof(globalStringBaseTagOffset), nullptr, nullptr, 0);
        int32_t redRead = SFileReadFile(file, &this->m_red, sizeof(this->m_red), nullptr, nullptr, 0);
        int32_t greenRead = SFileReadFile(file, &this->m_green, sizeof(this->m_green), nullptr, nullptr, 0);
        int32_t blueRead = SFileReadFile(file, &this->m_blue, sizeof(this->m_blue), nullptr, nullptr, 0);

        if (!idRead
            | !actualTypeRead
            | !globalStringBaseTagRead
            | !redRead
            | !greenRead
            | !blueRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_globalStringBaseTag = &stringBuffer[globalStringBaseTagOffset];
        } else {
            this->m_globalStringBaseTag = "";
        }

        return true;
    }
};

#endif
