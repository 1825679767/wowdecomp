#ifndef WOW_SOURCE_DB_REC_WORLDSTATEZONESOUNDSREC_H
#define WOW_SOURCE_DB_REC_WORLDSTATEZONESOUNDSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct WorldStateZoneSoundsRec {
    /* 0000 */ int32_t m_worldStateID;
    /* 0004 */ int32_t m_worldStateValue;
    /* 0008 */ int32_t m_areaID;
    /* 000C */ int32_t m_wmoareaID;
    /* 0010 */ int32_t m_zoneIntroMusicID;
    /* 0014 */ int32_t m_zoneMusicID;
    /* 0018 */ int32_t m_soundAmbienceID;
    /* 001C */ int32_t m_soundProviderPreferencesID;
    /* 0020 */ int32_t m_generatedID;

// VERIFY: f8f85c68 auto
    // OFFSET: 0x008BCF90
    static const char* GetFilename() {
        return "DBFilesClient\\WorldStateZoneSounds.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t worldStateIDRead = SFileReadFile(file, &this->m_worldStateID, sizeof(this->m_worldStateID), nullptr, nullptr, 0);
        int32_t worldStateValueRead = SFileReadFile(file, &this->m_worldStateValue, sizeof(this->m_worldStateValue), nullptr, nullptr, 0);
        int32_t areaIDRead = SFileReadFile(file, &this->m_areaID, sizeof(this->m_areaID), nullptr, nullptr, 0);
        int32_t wmoareaIDRead = SFileReadFile(file, &this->m_wmoareaID, sizeof(this->m_wmoareaID), nullptr, nullptr, 0);
        int32_t zoneIntroMusicIDRead = SFileReadFile(file, &this->m_zoneIntroMusicID, sizeof(this->m_zoneIntroMusicID), nullptr, nullptr, 0);
        int32_t zoneMusicIDRead = SFileReadFile(file, &this->m_zoneMusicID, sizeof(this->m_zoneMusicID), nullptr, nullptr, 0);
        int32_t soundAmbienceIDRead = SFileReadFile(file, &this->m_soundAmbienceID, sizeof(this->m_soundAmbienceID), nullptr, nullptr, 0);
        int32_t soundProviderPreferencesIDRead = SFileReadFile(file, &this->m_soundProviderPreferencesID, sizeof(this->m_soundProviderPreferencesID), nullptr, nullptr, 0);

        if (!worldStateIDRead
            | !worldStateValueRead
            | !areaIDRead
            | !wmoareaIDRead
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
