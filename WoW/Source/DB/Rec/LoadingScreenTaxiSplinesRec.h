#ifndef WOW_SOURCE_DB_REC_LOADINGSCREENTAXISPLINESREC_H
#define WOW_SOURCE_DB_REC_LOADINGSCREENTAXISPLINESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct LoadingScreenTaxiSplinesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_pathID;
    /* 0008 */ float m_locx [8];
    /* 0028 */ float m_locy [8];
    /* 0048 */ int32_t m_legIndex;

// VERIFY: 662898a6 auto
    // OFFSET: 0x008B2BF0
    static const char* GetFilename() {
        return "DBFilesClient\\LoadingScreenTaxiSplines.dbc";
    }

// VERIFY: e65ef261 manual
    // OFFSET: 0x008B2C00
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t pathIDRead = SFileReadFile(file, &this->m_pathID, sizeof(this->m_pathID), nullptr, nullptr, 0);
        int32_t locxRead = SFileReadFile(file, this->m_locx, sizeof(this->m_locx), nullptr, nullptr, 0);
        int32_t locyRead = SFileReadFile(file, this->m_locy, sizeof(this->m_locy), nullptr, nullptr, 0);
        int32_t legIndexRead = SFileReadFile(file, &this->m_legIndex, sizeof(this->m_legIndex), nullptr, nullptr, 0);

        if (!idRead
            | !pathIDRead
            | !locxRead
            | !locyRead
            | !legIndexRead) {
            return false;
        }

        return true;
    }
};

#endif
