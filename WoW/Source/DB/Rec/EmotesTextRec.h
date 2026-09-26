#ifndef WOW_SOURCE_DB_REC_EMOTESTEXTREC_H
#define WOW_SOURCE_DB_REC_EMOTESTEXTREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct EmotesTextRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_emoteID;
    /* 000C */ int32_t m_emoteText [16];

// VERIFY: 489f6b5b auto
    // OFFSET: 0x008AEDF0
    static const char* GetFilename() {
        return "DBFilesClient\\EmotesText.dbc";
    }

// VERIFY: 0ffa8f8b manual
    // OFFSET: 0x008AEE00
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
        int32_t emoteIDRead = SFileReadFile(file, &this->m_emoteID, sizeof(this->m_emoteID), nullptr, nullptr, 0);
        int32_t emoteTextRead = SFileReadFile(file, this->m_emoteText, sizeof(this->m_emoteText), nullptr, nullptr, 0);

        if (!idRead
            | !nameRead
            | !emoteIDRead
            | !emoteTextRead) {
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
