#ifndef WOW_SOURCE_DB_REC_ITEMREC_H
#define WOW_SOURCE_DB_REC_ITEMREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ItemRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_classID;
    /* 0008 */ int32_t m_subclassID;
    /* 000C */ int32_t m_soundOverrideSubclassID;
    /* 0010 */ int32_t m_material;
    /* 0014 */ int32_t m_displayInfoID;
    /* 0018 */ int32_t m_inventoryType;
    /* 001C */ int32_t m_sheatheType;

// VERIFY: e8a996b7 auto
    // OFFSET: 0x008B02C0
    static const char* GetFilename() {
        return "DBFilesClient\\Item.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t classIDRead = SFileReadFile(file, &this->m_classID, sizeof(this->m_classID), nullptr, nullptr, 0);
        int32_t subclassIDRead = SFileReadFile(file, &this->m_subclassID, sizeof(this->m_subclassID), nullptr, nullptr, 0);
        int32_t soundOverrideSubclassIDRead = SFileReadFile(file, &this->m_soundOverrideSubclassID, sizeof(this->m_soundOverrideSubclassID), nullptr, nullptr, 0);
        int32_t materialRead = SFileReadFile(file, &this->m_material, sizeof(this->m_material), nullptr, nullptr, 0);
        int32_t displayInfoIDRead = SFileReadFile(file, &this->m_displayInfoID, sizeof(this->m_displayInfoID), nullptr, nullptr, 0);
        int32_t inventoryTypeRead = SFileReadFile(file, &this->m_inventoryType, sizeof(this->m_inventoryType), nullptr, nullptr, 0);
        int32_t sheatheTypeRead = SFileReadFile(file, &this->m_sheatheType, sizeof(this->m_sheatheType), nullptr, nullptr, 0);

        if (!idRead
            | !classIDRead
            | !subclassIDRead
            | !soundOverrideSubclassIDRead
            | !materialRead
            | !displayInfoIDRead
            | !inventoryTypeRead
            | !sheatheTypeRead) {
            return false;
        }

        return true;
    }
};

#endif
