#ifndef WOW_SOURCE_DB_REC_SHEATHESOUNDLOOKUPSREC_H
#define WOW_SOURCE_DB_REC_SHEATHESOUNDLOOKUPSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SheatheSoundLookupsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_classID;
    /* 0008 */ int32_t m_subclassID;
    /* 000C */ int32_t m_material;
    /* 0010 */ int32_t m_checkMaterial;
    /* 0014 */ int32_t m_sheatheSound;
    /* 0018 */ int32_t m_unsheatheSound;

// VERIFY: 8548a5eb auto
    // OFFSET: 0x008B5B70
    static const char* GetFilename() {
        return "DBFilesClient\\SheatheSoundLookups.dbc";
    }

// VERIFY: 0e06d763 manual
    // OFFSET: 0x008B5B80
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t classIDRead = SFileReadFile(file, &this->m_classID, sizeof(this->m_classID), nullptr, nullptr, 0);
        int32_t subclassIDRead = SFileReadFile(file, &this->m_subclassID, sizeof(this->m_subclassID), nullptr, nullptr, 0);
        int32_t materialRead = SFileReadFile(file, &this->m_material, sizeof(this->m_material), nullptr, nullptr, 0);
        int32_t checkMaterialRead = SFileReadFile(file, &this->m_checkMaterial, sizeof(this->m_checkMaterial), nullptr, nullptr, 0);
        int32_t sheatheSoundRead = SFileReadFile(file, &this->m_sheatheSound, sizeof(this->m_sheatheSound), nullptr, nullptr, 0);
        int32_t unsheatheSoundRead = SFileReadFile(file, &this->m_unsheatheSound, sizeof(this->m_unsheatheSound), nullptr, nullptr, 0);

        if (!idRead
            | !classIDRead
            | !subclassIDRead
            | !materialRead
            | !checkMaterialRead
            | !sheatheSoundRead
            | !unsheatheSoundRead) {
            return false;
        }

        return true;
    }
};

#endif
