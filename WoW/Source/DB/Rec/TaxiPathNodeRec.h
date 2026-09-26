#ifndef WOW_SOURCE_DB_REC_TAXIPATHNODEREC_H
#define WOW_SOURCE_DB_REC_TAXIPATHNODEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct TaxiPathNodeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_pathID;
    /* 0008 */ int32_t m_nodeIndex;
    /* 000C */ int32_t m_continentID;
    /* 0010 */ float m_loc [3];
    /* 001C */ int32_t m_flags;
    /* 0020 */ int32_t m_delay;
    /* 0024 */ int32_t m_arrivalEventID;
    /* 0028 */ int32_t m_departureEventID;

// VERIFY: 5f444ac6 auto
    // OFFSET: 0x008BA170
    static const char* GetFilename() {
        return "DBFilesClient\\TaxiPathNode.dbc";
    }

// VERIFY: 55b28d2c auto
    // OFFSET: 0x008BA5B0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t pathIDRead = SFileReadFile(file, &this->m_pathID, sizeof(this->m_pathID), nullptr, nullptr, 0);
        int32_t nodeIndexRead = SFileReadFile(file, &this->m_nodeIndex, sizeof(this->m_nodeIndex), nullptr, nullptr, 0);
        int32_t continentIDRead = SFileReadFile(file, &this->m_continentID, sizeof(this->m_continentID), nullptr, nullptr, 0);
        int32_t locRead0 = SFileReadFile(file, &this->m_loc[0], sizeof(this->m_loc[0]), nullptr, nullptr, 0);
        int32_t locRead1 = SFileReadFile(file, &this->m_loc[1], sizeof(this->m_loc[1]), nullptr, nullptr, 0);
        int32_t locRead2 = SFileReadFile(file, &this->m_loc[2], sizeof(this->m_loc[2]), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t delayRead = SFileReadFile(file, &this->m_delay, sizeof(this->m_delay), nullptr, nullptr, 0);
        int32_t arrivalEventIDRead = SFileReadFile(file, &this->m_arrivalEventID, sizeof(this->m_arrivalEventID), nullptr, nullptr, 0);
        int32_t departureEventIDRead = SFileReadFile(file, &this->m_departureEventID, sizeof(this->m_departureEventID), nullptr, nullptr, 0);

        if (!idRead
            | !pathIDRead
            | !nodeIndexRead
            | !continentIDRead
            | !locRead0
            | !locRead1
            | !locRead2
            | !flagsRead
            | !delayRead
            | !arrivalEventIDRead
            | !departureEventIDRead) {
            return false;
        }

        return true;
    }
};

#endif
