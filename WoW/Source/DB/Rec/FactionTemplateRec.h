#ifndef WOW_SOURCE_DB_REC_FACTIONTEMPLATEREC_H
#define WOW_SOURCE_DB_REC_FACTIONTEMPLATEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct FactionTemplateRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_faction;
    /* 0008 */ int32_t m_flags;
    /* 000C */ int32_t m_factionGroup;
    /* 0010 */ int32_t m_friendGroup;
    /* 0014 */ int32_t m_enemyGroup;
    /* 0018 */ int32_t m_enemies [4];
    /* 0028 */ int32_t m_friend [4];

// VERIFY: cc1b906c auto
    // OFFSET: 0x008AF800
    static const char* GetFilename() {
        return "DBFilesClient\\FactionTemplate.dbc";
    }

// VERIFY: 05748cfa manual
    // OFFSET: 0x008AF810
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t factionRead = SFileReadFile(file, &this->m_faction, sizeof(this->m_faction), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t factionGroupRead = SFileReadFile(file, &this->m_factionGroup, sizeof(this->m_factionGroup), nullptr, nullptr, 0);
        int32_t friendGroupRead = SFileReadFile(file, &this->m_friendGroup, sizeof(this->m_friendGroup), nullptr, nullptr, 0);
        int32_t enemyGroupRead = SFileReadFile(file, &this->m_enemyGroup, sizeof(this->m_enemyGroup), nullptr, nullptr, 0);
        int32_t enemiesRead = SFileReadFile(file, this->m_enemies, sizeof(this->m_enemies), nullptr, nullptr, 0);
        int32_t friendRead = SFileReadFile(file, this->m_friend, sizeof(this->m_friend), nullptr, nullptr, 0);

        if (!idRead
            | !factionRead
            | !flagsRead
            | !factionGroupRead
            | !friendGroupRead
            | !enemyGroupRead
            | !enemiesRead
            | !friendRead) {
            return false;
        }

        return true;
    }
};

#endif
