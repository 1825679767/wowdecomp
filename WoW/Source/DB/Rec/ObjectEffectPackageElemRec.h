#ifndef WOW_SOURCE_DB_REC_OBJECTEFFECTPACKAGEELEMREC_H
#define WOW_SOURCE_DB_REC_OBJECTEFFECTPACKAGEELEMREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ObjectEffectPackageElemRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_objectEffectPackageID;
    /* 0008 */ int32_t m_objectEffectGroupID;
    /* 000C */ int32_t m_stateType;

// VERIFY: aa178a67 auto
    // OFFSET: 0x008BD7D0
    static const char* GetFilename() {
        return "DBFilesClient\\ObjectEffectPackageElem.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t objectEffectPackageIDRead = SFileReadFile(file, &this->m_objectEffectPackageID, sizeof(this->m_objectEffectPackageID), nullptr, nullptr, 0);
        int32_t objectEffectGroupIDRead = SFileReadFile(file, &this->m_objectEffectGroupID, sizeof(this->m_objectEffectGroupID), nullptr, nullptr, 0);
        int32_t stateTypeRead = SFileReadFile(file, &this->m_stateType, sizeof(this->m_stateType), nullptr, nullptr, 0);

        if (!idRead
            | !objectEffectPackageIDRead
            | !objectEffectGroupIDRead
            | !stateTypeRead) {
            return false;
        }

        return true;
    }
};

#endif
