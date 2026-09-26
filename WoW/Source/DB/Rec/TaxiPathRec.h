#ifndef WOW_SOURCE_DB_REC_TAXIPATHREC_H
#define WOW_SOURCE_DB_REC_TAXIPATHREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct TaxiPathRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_fromTaxiNode;
    /* 0008 */ int32_t m_toTaxiNode;
    /* 000C */ int32_t m_cost;

// VERIFY: 565461ba auto
    // OFFSET: 0x008BA180
    static const char* GetFilename() {
        return "DBFilesClient\\TaxiPath.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t fromTaxiNodeRead = SFileReadFile(file, &this->m_fromTaxiNode, sizeof(this->m_fromTaxiNode), nullptr, nullptr, 0);
        int32_t toTaxiNodeRead = SFileReadFile(file, &this->m_toTaxiNode, sizeof(this->m_toTaxiNode), nullptr, nullptr, 0);
        int32_t costRead = SFileReadFile(file, &this->m_cost, sizeof(this->m_cost), nullptr, nullptr, 0);

        if (!idRead
            | !fromTaxiNodeRead
            | !toTaxiNodeRead
            | !costRead) {
            return false;
        }

        return true;
    }
};

#endif
