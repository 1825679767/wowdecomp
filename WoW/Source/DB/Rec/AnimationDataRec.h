#ifndef WOW_SOURCE_DB_REC_ANIMATIONDATAREC_H
#define WOW_SOURCE_DB_REC_ANIMATIONDATAREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct AnimationDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_weaponflags;
    /* 000C */ int32_t m_bodyflags;
    /* 0010 */ int32_t m_flags;
    /* 0014 */ int32_t m_fallback;
    /* 0018 */ int32_t m_behaviorID;
    /* 001C */ int32_t m_behaviorTier;

// VERIFY: 13dfe232 auto
    // OFFSET: 0x008A93D0
    static const char* GetFilename() {
        return "DBFilesClient\\AnimationData.dbc";
    }

// VERIFY: cce4cd82 manual
    // OFFSET: 0x008A93E0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
        int32_t weaponflagsRead = SFileReadFile(file, &this->m_weaponflags, sizeof(this->m_weaponflags), nullptr, nullptr, 0);
        int32_t bodyflagsRead = SFileReadFile(file, &this->m_bodyflags, sizeof(this->m_bodyflags), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t fallbackRead = SFileReadFile(file, &this->m_fallback, sizeof(this->m_fallback), nullptr, nullptr, 0);
        int32_t behaviorIDRead = SFileReadFile(file, &this->m_behaviorID, sizeof(this->m_behaviorID), nullptr, nullptr, 0);
        int32_t behaviorTierRead = SFileReadFile(file, &this->m_behaviorTier, sizeof(this->m_behaviorTier), nullptr, nullptr, 0);

        if (!idRead
            | !nameRead
            | !weaponflagsRead
            | !bodyflagsRead
            | !flagsRead
            | !fallbackRead
            | !behaviorIDRead
            | !behaviorTierRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_name = &stringBuffer[nameOffset];
        } else {
            this->m_name = "";
        }

        return true;
    }
};

#endif
