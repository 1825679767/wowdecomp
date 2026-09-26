#ifndef WOW_SOURCE_DB_REC_EMOTESREC_H
#define WOW_SOURCE_DB_REC_EMOTESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct EmotesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_emoteSlashCommand;
    /* 0008 */ int32_t m_animID;
    /* 000C */ int32_t m_emoteFlags;
    /* 0010 */ int32_t m_emoteSpecProc;
    /* 0014 */ int32_t m_emoteSpecProcParam;
    /* 0018 */ int32_t m_eventSoundID;

// VERIFY: 6e0ada49 auto
    // OFFSET: 0x008AEDC0
    static const char* GetFilename() {
        return "DBFilesClient\\Emotes.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t emoteSlashCommandOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t emoteSlashCommandRead = SFileReadFile(file, &emoteSlashCommandOffset, sizeof(emoteSlashCommandOffset), nullptr, nullptr, 0);
        int32_t animIDRead = SFileReadFile(file, &this->m_animID, sizeof(this->m_animID), nullptr, nullptr, 0);
        int32_t emoteFlagsRead = SFileReadFile(file, &this->m_emoteFlags, sizeof(this->m_emoteFlags), nullptr, nullptr, 0);
        int32_t emoteSpecProcRead = SFileReadFile(file, &this->m_emoteSpecProc, sizeof(this->m_emoteSpecProc), nullptr, nullptr, 0);
        int32_t emoteSpecProcParamRead = SFileReadFile(file, &this->m_emoteSpecProcParam, sizeof(this->m_emoteSpecProcParam), nullptr, nullptr, 0);
        int32_t eventSoundIDRead = SFileReadFile(file, &this->m_eventSoundID, sizeof(this->m_eventSoundID), nullptr, nullptr, 0);

        if (!idRead
            | !emoteSlashCommandRead
            | !animIDRead
            | !emoteFlagsRead
            | !emoteSpecProcRead
            | !emoteSpecProcParamRead
            | !eventSoundIDRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_emoteSlashCommand = &stringBuffer[emoteSlashCommandOffset];
        } else {
            this->m_emoteSlashCommand = "";
        }

        return true;
    }
};

#endif
