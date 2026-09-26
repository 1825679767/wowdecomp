#ifndef WOW_SOURCE_DB_REC_SCALINGSTATDISTRIBUTIONREC_H
#define WOW_SOURCE_DB_REC_SCALINGSTATDISTRIBUTIONREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ScalingStatDistributionRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_statID [10];
    /* 002C */ int32_t m_bonus [10];
    /* 0054 */ int32_t m_maxlevel;

// VERIFY: 3a712f43 auto
    // OFFSET: 0x008B5680
    static const char* GetFilename() {
        return "DBFilesClient\\ScalingStatDistribution.dbc";
    }

// VERIFY: 0c8d74e8 manual
    // OFFSET: 0x008B5690
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t statIDRead = SFileReadFile(file, this->m_statID, sizeof(this->m_statID), nullptr, nullptr, 0);
        int32_t bonusRead = SFileReadFile(file, this->m_bonus, sizeof(this->m_bonus), nullptr, nullptr, 0);
        int32_t maxlevelRead = SFileReadFile(file, &this->m_maxlevel, sizeof(this->m_maxlevel), nullptr, nullptr, 0);

        if (!idRead
            | !statIDRead
            | !bonusRead
            | !maxlevelRead) {
            return false;
        }

        return true;
    }
};

#endif
