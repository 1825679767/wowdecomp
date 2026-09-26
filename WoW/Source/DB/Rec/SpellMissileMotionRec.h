#ifndef WOW_SOURCE_DB_REC_SPELLMISSILEMOTIONREC_H
#define WOW_SOURCE_DB_REC_SPELLMISSILEMOTIONREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellMissileMotionRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_scriptBody;
    /* 000C */ int32_t m_flags;
    /* 0010 */ int32_t m_missileCount;

// VERIFY: c0215d1f auto
    // OFFSET: 0x008B84F0
    static const char* GetFilename() {
        return "DBFilesClient\\SpellMissileMotion.dbc";
    }

// VERIFY: 4e30b79c auto
    // OFFSET: 0x008B8500
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;
        uint32_t scriptBodyOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
        int32_t scriptBodyRead = SFileReadFile(file, &scriptBodyOffset, sizeof(scriptBodyOffset), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t missileCountRead = SFileReadFile(file, &this->m_missileCount, sizeof(this->m_missileCount), nullptr, nullptr, 0);

        if (!idRead
            | !nameRead
            | !scriptBodyRead
            | !flagsRead
            | !missileCountRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_name = &stringBuffer[nameOffset];
            this->m_scriptBody = &stringBuffer[scriptBodyOffset];
        } else {
            this->m_name = "";
            this->m_scriptBody = "";
        }

        return true;
    }
};

#endif
