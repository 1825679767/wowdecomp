#ifndef WOW_SOURCE_DB_REC_CREATUREDISPLAYINFOEXTRAREC_H
#define WOW_SOURCE_DB_REC_CREATUREDISPLAYINFOEXTRAREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CreatureDisplayInfoExtraRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_displayRaceID;
    /* 0008 */ int32_t m_displaySexID;
    /* 000C */ int32_t m_skinID;
    /* 0010 */ int32_t m_faceID;
    /* 0014 */ int32_t m_hairStyleID;
    /* 0018 */ int32_t m_hairColorID;
    /* 001C */ int32_t m_facialHairID;
    /* 0020 */ int32_t m_npcitemDisplay [11];
    /* 004C */ int32_t m_flags;
    /* 0050 */ const char* m_bakeName;

// VERIFY: d9158e43 auto
    // OFFSET: 0x008AD010
    static const char* GetFilename() {
        return "DBFilesClient\\CreatureDisplayInfoExtra.dbc";
    }

// VERIFY: d992a866 auto
    // OFFSET: 0x008AD020
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t bakeNameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t displayRaceIDRead = SFileReadFile(file, &this->m_displayRaceID, sizeof(this->m_displayRaceID), nullptr, nullptr, 0);
        int32_t displaySexIDRead = SFileReadFile(file, &this->m_displaySexID, sizeof(this->m_displaySexID), nullptr, nullptr, 0);
        int32_t skinIDRead = SFileReadFile(file, &this->m_skinID, sizeof(this->m_skinID), nullptr, nullptr, 0);
        int32_t faceIDRead = SFileReadFile(file, &this->m_faceID, sizeof(this->m_faceID), nullptr, nullptr, 0);
        int32_t hairStyleIDRead = SFileReadFile(file, &this->m_hairStyleID, sizeof(this->m_hairStyleID), nullptr, nullptr, 0);
        int32_t hairColorIDRead = SFileReadFile(file, &this->m_hairColorID, sizeof(this->m_hairColorID), nullptr, nullptr, 0);
        int32_t facialHairIDRead = SFileReadFile(file, &this->m_facialHairID, sizeof(this->m_facialHairID), nullptr, nullptr, 0);
        int32_t npcitemDisplayRead = SFileReadFile(file, this->m_npcitemDisplay, sizeof(this->m_npcitemDisplay), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t bakeNameRead = SFileReadFile(file, &bakeNameOffset, sizeof(bakeNameOffset), nullptr, nullptr, 0);

        if (!idRead
            | !displayRaceIDRead
            | !displaySexIDRead
            | !skinIDRead
            | !faceIDRead
            | !hairStyleIDRead
            | !hairColorIDRead
            | !facialHairIDRead
            | !npcitemDisplayRead
            | !flagsRead
            | !bakeNameRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_bakeName = &stringBuffer[bakeNameOffset];
        } else {
            this->m_bakeName = "";
        }

        return true;
    }
};

#endif
