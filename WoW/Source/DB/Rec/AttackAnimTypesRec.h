#ifndef WOW_SOURCE_DB_REC_ATTACKANIMTYPESREC_H
#define WOW_SOURCE_DB_REC_ATTACKANIMTYPESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct AttackAnimTypesRec {
    /* 0000 */ int32_t m_animID;
    /* 0004 */ const char* m_animName;

// VERIFY: c18c5b8e auto
    // OFFSET: 0x008AA100
    static const char* GetFilename() {
        return "DBFilesClient\\AttackAnimTypes.dbc";
    }

// VERIFY: 0db2f60e auto
    // OFFSET: 0x008B98D0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t animNameOffset;

        int32_t animIDRead = SFileReadFile(file, &this->m_animID, sizeof(this->m_animID), nullptr, nullptr, 0);
        int32_t animNameRead = SFileReadFile(file, &animNameOffset, sizeof(animNameOffset), nullptr, nullptr, 0);

        if (!animIDRead
            | !animNameRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_animName = &stringBuffer[animNameOffset];
        } else {
            this->m_animName = "";
        }

        return true;
    }
};

#endif
