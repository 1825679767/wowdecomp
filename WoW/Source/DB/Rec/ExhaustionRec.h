#ifndef WOW_SOURCE_DB_REC_EXHAUSTIONREC_H
#define WOW_SOURCE_DB_REC_EXHAUSTIONREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "WoW/Source/Client.h"

struct ExhaustionRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_xp;
    /* 0008 */ float m_factor;
    /* 000C */ float m_outdoorHours;
    /* 0010 */ float m_innHours;
    /* 0014 */ const char* m_name;
    /* 0018 */ float m_threshold;

// VERIFY: 4a6e1321 auto
    // OFFSET: 0x008AEED0
    static const char* GetFilename() {
        return "DBFilesClient\\Exhaustion.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset[16];
        uint32_t nameFlags;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t xpRead = SFileReadFile(file, &this->m_xp, sizeof(this->m_xp), nullptr, nullptr, 0);
        int32_t factorRead = SFileReadFile(file, &this->m_factor, sizeof(this->m_factor), nullptr, nullptr, 0);
        int32_t outdoorHoursRead = SFileReadFile(file, &this->m_outdoorHours, sizeof(this->m_outdoorHours), nullptr, nullptr, 0);
        int32_t innHoursRead = SFileReadFile(file, &this->m_innHours, sizeof(this->m_innHours), nullptr, nullptr, 0);
        int32_t nameRead0 = SFileReadFile(file, &nameOffset[0], sizeof(nameOffset[0]), nullptr, nullptr, 0);
        int32_t nameRead1 = SFileReadFile(file, &nameOffset[1], sizeof(nameOffset[1]), nullptr, nullptr, 0);
        int32_t nameRead2 = SFileReadFile(file, &nameOffset[2], sizeof(nameOffset[2]), nullptr, nullptr, 0);
        int32_t nameRead3 = SFileReadFile(file, &nameOffset[3], sizeof(nameOffset[3]), nullptr, nullptr, 0);
        int32_t nameRead4 = SFileReadFile(file, &nameOffset[4], sizeof(nameOffset[4]), nullptr, nullptr, 0);
        int32_t nameRead5 = SFileReadFile(file, &nameOffset[5], sizeof(nameOffset[5]), nullptr, nullptr, 0);
        int32_t nameRead6 = SFileReadFile(file, &nameOffset[6], sizeof(nameOffset[6]), nullptr, nullptr, 0);
        int32_t nameRead7 = SFileReadFile(file, &nameOffset[7], sizeof(nameOffset[7]), nullptr, nullptr, 0);
        int32_t nameRead8 = SFileReadFile(file, &nameOffset[8], sizeof(nameOffset[8]), nullptr, nullptr, 0);
        int32_t nameRead9 = SFileReadFile(file, &nameOffset[9], sizeof(nameOffset[9]), nullptr, nullptr, 0);
        int32_t nameRead10 = SFileReadFile(file, &nameOffset[10], sizeof(nameOffset[10]), nullptr, nullptr, 0);
        int32_t nameRead11 = SFileReadFile(file, &nameOffset[11], sizeof(nameOffset[11]), nullptr, nullptr, 0);
        int32_t nameRead12 = SFileReadFile(file, &nameOffset[12], sizeof(nameOffset[12]), nullptr, nullptr, 0);
        int32_t nameRead13 = SFileReadFile(file, &nameOffset[13], sizeof(nameOffset[13]), nullptr, nullptr, 0);
        int32_t nameRead14 = SFileReadFile(file, &nameOffset[14], sizeof(nameOffset[14]), nullptr, nullptr, 0);
        int32_t nameRead15 = SFileReadFile(file, &nameOffset[15], sizeof(nameOffset[15]), nullptr, nullptr, 0);
        int32_t nameFlagsRead = SFileReadFile(file, &nameFlags, sizeof(nameFlags), nullptr, nullptr, 0);
        int32_t thresholdRead = SFileReadFile(file, &this->m_threshold, sizeof(this->m_threshold), nullptr, nullptr, 0);

        if (!idRead
            | !xpRead
            | !factorRead
            | !outdoorHoursRead
            | !innHoursRead
            | !nameRead0
            | !nameRead1
            | !nameRead2
            | !nameRead3
            | !nameRead4
            | !nameRead5
            | !nameRead6
            | !nameRead7
            | !nameRead8
            | !nameRead9
            | !nameRead10
            | !nameRead11
            | !nameRead12
            | !nameRead13
            | !nameRead14
            | !nameRead15
            | !nameFlagsRead
            | !thresholdRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_name = &stringBuffer[nameOffset[s_locale]];
        } else {
            this->m_name = "";
        }

        return true;
    }
};

#endif
