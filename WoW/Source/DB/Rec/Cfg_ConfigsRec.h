#ifndef WOW_SOURCE_DB_REC_CFG_CONFIGSREC_H
#define WOW_SOURCE_DB_REC_CFG_CONFIGSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct Cfg_ConfigsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_realmType;
    /* 0008 */ int32_t m_playerKillingAllowed;
    /* 000C */ int32_t m_roleplaying;
    /* 0010 */ int32_t m_generatedID;

// VERIFY: 202a0c35 auto
    // OFFSET: 0x008AAD90
    static const char* GetFilename() {
        return "DBFilesClient\\Cfg_Configs.dbc";
    }

// VERIFY: 7eaf965d manual
    // OFFSET: 0x008B72B0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t realmTypeRead = SFileReadFile(file, &this->m_realmType, sizeof(this->m_realmType), nullptr, nullptr, 0);
        int32_t playerKillingAllowedRead = SFileReadFile(file, &this->m_playerKillingAllowed, sizeof(this->m_playerKillingAllowed), nullptr, nullptr, 0);
        int32_t roleplayingRead = SFileReadFile(file, &this->m_roleplaying, sizeof(this->m_roleplaying), nullptr, nullptr, 0);

        if (!idRead
            | !realmTypeRead
            | !playerKillingAllowedRead
            | !roleplayingRead) {
            return false;
        }

        return true;
    }
};

#endif
