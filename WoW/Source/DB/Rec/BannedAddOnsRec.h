#ifndef WOW_SOURCE_DB_REC_BANNEDADDONSREC_H
#define WOW_SOURCE_DB_REC_BANNEDADDONSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct BannedAddOnsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_nameMd5 [4];
    /* 0014 */ int32_t m_versionMd5 [4];
    /* 0024 */ int32_t m_lastModified;
    /* 0028 */ int32_t m_flags;

// VERIFY: 7a767112 auto
    // OFFSET: 0x008AA130
    static const char* GetFilename() {
        return "DBFilesClient\\BannedAddOns.dbc";
    }

// VERIFY: 69d55962 manual
    // OFFSET: 0x008AFED0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameMd5Read = SFileReadFile(file, this->m_nameMd5, sizeof(this->m_nameMd5), nullptr, nullptr, 0);
        int32_t versionMd5Read = SFileReadFile(file, this->m_versionMd5, sizeof(this->m_versionMd5), nullptr, nullptr, 0);
        int32_t lastModifiedRead = SFileReadFile(file, &this->m_lastModified, sizeof(this->m_lastModified), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!idRead
            | !nameMd5Read
            | !versionMd5Read
            | !lastModifiedRead
            | !flagsRead) {
            return false;
        }

        return true;
    }
};

#endif
