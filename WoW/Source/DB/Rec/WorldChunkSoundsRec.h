#ifndef WOW_SOURCE_DB_REC_WORLDCHUNKSOUNDSREC_H
#define WOW_SOURCE_DB_REC_WORLDCHUNKSOUNDSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct WorldChunkSoundsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_chunkX;
    /* 0008 */ int32_t m_chunkY;
    /* 000C */ int32_t m_subchunkX;
    /* 0010 */ int32_t m_subchunkY;
    /* 0014 */ int32_t m_zoneIntroMusicID;
    /* 0018 */ int32_t m_zoneMusicID;
    /* 001C */ int32_t m_soundAmbienceID;
    /* 0020 */ int32_t m_soundProviderPreferencesID;
    /* 0024 */ int32_t m_generatedID;

// VERIFY: 5b4f0525 auto
    // OFFSET: 0x008BD0B0
    static const char* GetFilename() {
        return "DBFilesClient\\WorldChunkSounds.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t chunkXRead = SFileReadFile(file, &this->m_chunkX, sizeof(this->m_chunkX), nullptr, nullptr, 0);
        int32_t chunkYRead = SFileReadFile(file, &this->m_chunkY, sizeof(this->m_chunkY), nullptr, nullptr, 0);
        int32_t subchunkXRead = SFileReadFile(file, &this->m_subchunkX, sizeof(this->m_subchunkX), nullptr, nullptr, 0);
        int32_t subchunkYRead = SFileReadFile(file, &this->m_subchunkY, sizeof(this->m_subchunkY), nullptr, nullptr, 0);
        int32_t zoneIntroMusicIDRead = SFileReadFile(file, &this->m_zoneIntroMusicID, sizeof(this->m_zoneIntroMusicID), nullptr, nullptr, 0);
        int32_t zoneMusicIDRead = SFileReadFile(file, &this->m_zoneMusicID, sizeof(this->m_zoneMusicID), nullptr, nullptr, 0);
        int32_t soundAmbienceIDRead = SFileReadFile(file, &this->m_soundAmbienceID, sizeof(this->m_soundAmbienceID), nullptr, nullptr, 0);
        int32_t soundProviderPreferencesIDRead = SFileReadFile(file, &this->m_soundProviderPreferencesID, sizeof(this->m_soundProviderPreferencesID), nullptr, nullptr, 0);

        if (!idRead
            | !chunkXRead
            | !chunkYRead
            | !subchunkXRead
            | !subchunkYRead
            | !zoneIntroMusicIDRead
            | !zoneMusicIDRead
            | !soundAmbienceIDRead
            | !soundProviderPreferencesIDRead) {
            return false;
        }

        return true;
    }
};

#endif
