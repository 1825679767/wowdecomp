#ifndef WOW_SOURCE_DB_REC_FOOTSTEPTERRAINLOOKUPREC_H
#define WOW_SOURCE_DB_REC_FOOTSTEPTERRAINLOOKUPREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct FootstepTerrainLookupRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_creatureFootstepID;
    /* 0008 */ int32_t m_terrainSoundID;
    /* 000C */ int32_t m_soundID;
    /* 0010 */ int32_t m_soundIdsplash;

// VERIFY: 7541cfe6 auto
    // OFFSET: 0x008AF9F0
    static const char* GetFilename() {
        return "DBFilesClient\\FootstepTerrainLookup.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t creatureFootstepIDRead = SFileReadFile(file, &this->m_creatureFootstepID, sizeof(this->m_creatureFootstepID), nullptr, nullptr, 0);
        int32_t terrainSoundIDRead = SFileReadFile(file, &this->m_terrainSoundID, sizeof(this->m_terrainSoundID), nullptr, nullptr, 0);
        int32_t soundIDRead = SFileReadFile(file, &this->m_soundID, sizeof(this->m_soundID), nullptr, nullptr, 0);
        int32_t soundIdsplashRead = SFileReadFile(file, &this->m_soundIdsplash, sizeof(this->m_soundIdsplash), nullptr, nullptr, 0);

        if (!idRead
            | !creatureFootstepIDRead
            | !terrainSoundIDRead
            | !soundIDRead
            | !soundIdsplashRead) {
            return false;
        }

        return true;
    }
};

#endif
