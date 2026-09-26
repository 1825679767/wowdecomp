#ifndef WOW_SOURCE_DB_REC_MATERIALREC_H
#define WOW_SOURCE_DB_REC_MATERIALREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct MaterialRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ int32_t m_foleySoundID;
    /* 000C */ int32_t m_sheatheSoundID;
    /* 0010 */ int32_t m_unsheatheSoundID;

// VERIFY: bbc5037b auto
    // OFFSET: 0x008B47D0
    static const char* GetFilename() {
        return "DBFilesClient\\Material.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t foleySoundIDRead = SFileReadFile(file, &this->m_foleySoundID, sizeof(this->m_foleySoundID), nullptr, nullptr, 0);
        int32_t sheatheSoundIDRead = SFileReadFile(file, &this->m_sheatheSoundID, sizeof(this->m_sheatheSoundID), nullptr, nullptr, 0);
        int32_t unsheatheSoundIDRead = SFileReadFile(file, &this->m_unsheatheSoundID, sizeof(this->m_unsheatheSoundID), nullptr, nullptr, 0);

        if (!idRead
            | !flagsRead
            | !foleySoundIDRead
            | !sheatheSoundIDRead
            | !unsheatheSoundIDRead) {
            return false;
        }

        return true;
    }
};

#endif
