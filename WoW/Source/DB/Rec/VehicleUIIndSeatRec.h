#ifndef WOW_SOURCE_DB_REC_VEHICLEUIINDSEATREC_H
#define WOW_SOURCE_DB_REC_VEHICLEUIINDSEATREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct VehicleUIIndSeatRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_vehicleUiindicatorID;
    /* 0008 */ int32_t m_virtualSeatIndex;
    /* 000C */ float m_xpos;
    /* 0010 */ float m_ypos;

// VERIFY: 2d15d94a auto
    // OFFSET: 0x008BB550
    static const char* GetFilename() {
        return "DBFilesClient\\VehicleUIIndSeat.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t vehicleUiindicatorIDRead = SFileReadFile(file, &this->m_vehicleUiindicatorID, sizeof(this->m_vehicleUiindicatorID), nullptr, nullptr, 0);
        int32_t virtualSeatIndexRead = SFileReadFile(file, &this->m_virtualSeatIndex, sizeof(this->m_virtualSeatIndex), nullptr, nullptr, 0);
        int32_t xposRead = SFileReadFile(file, &this->m_xpos, sizeof(this->m_xpos), nullptr, nullptr, 0);
        int32_t yposRead = SFileReadFile(file, &this->m_ypos, sizeof(this->m_ypos), nullptr, nullptr, 0);

        if (!idRead
            | !vehicleUiindicatorIDRead
            | !virtualSeatIndexRead
            | !xposRead
            | !yposRead) {
            return false;
        }

        return true;
    }
};

#endif
