#ifndef WOW_SOURCE_DB_REC_OBJECTEFFECTREC_H
#define WOW_SOURCE_DB_REC_OBJECTEFFECTREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ObjectEffectRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_objectEffectGroupID;
    /* 000C */ int32_t m_triggerType;
    /* 0010 */ int32_t m_eventType;
    /* 0014 */ int32_t m_effectRecType;
    /* 0018 */ int32_t m_effectRecID;
    /* 001C */ int32_t m_attachment;
    /* 0020 */ float m_offset [3];
    /* 002C */ int32_t m_objectEffectModifierID;

// VERIFY: 8bd23b48 auto
    // OFFSET: 0x008BD530
    static const char* GetFilename() {
        return "DBFilesClient\\ObjectEffect.dbc";
    }

// VERIFY: c7cca486 auto
    // OFFSET: 0x008BD540
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
        int32_t objectEffectGroupIDRead = SFileReadFile(file, &this->m_objectEffectGroupID, sizeof(this->m_objectEffectGroupID), nullptr, nullptr, 0);
        int32_t triggerTypeRead = SFileReadFile(file, &this->m_triggerType, sizeof(this->m_triggerType), nullptr, nullptr, 0);
        int32_t eventTypeRead = SFileReadFile(file, &this->m_eventType, sizeof(this->m_eventType), nullptr, nullptr, 0);
        int32_t effectRecTypeRead = SFileReadFile(file, &this->m_effectRecType, sizeof(this->m_effectRecType), nullptr, nullptr, 0);
        int32_t effectRecIDRead = SFileReadFile(file, &this->m_effectRecID, sizeof(this->m_effectRecID), nullptr, nullptr, 0);
        int32_t attachmentRead = SFileReadFile(file, &this->m_attachment, sizeof(this->m_attachment), nullptr, nullptr, 0);
        int32_t offsetRead0 = SFileReadFile(file, &this->m_offset[0], sizeof(this->m_offset[0]), nullptr, nullptr, 0);
        int32_t offsetRead1 = SFileReadFile(file, &this->m_offset[1], sizeof(this->m_offset[1]), nullptr, nullptr, 0);
        int32_t offsetRead2 = SFileReadFile(file, &this->m_offset[2], sizeof(this->m_offset[2]), nullptr, nullptr, 0);
        int32_t objectEffectModifierIDRead = SFileReadFile(file, &this->m_objectEffectModifierID, sizeof(this->m_objectEffectModifierID), nullptr, nullptr, 0);

        if (!idRead
            | !nameRead
            | !objectEffectGroupIDRead
            | !triggerTypeRead
            | !eventTypeRead
            | !effectRecTypeRead
            | !effectRecIDRead
            | !attachmentRead
            | !offsetRead0
            | !offsetRead1
            | !offsetRead2
            | !objectEffectModifierIDRead) {
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
