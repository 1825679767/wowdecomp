#ifndef WOW_SOURCE_DB_REC_CINEMATICSEQUENCESREC_H
#define WOW_SOURCE_DB_REC_CINEMATICSEQUENCESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CinematicSequencesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundID;
    /* 0008 */ int32_t m_camera [8];

// VERIFY: 57fc53e7 auto
    // OFFSET: 0x008ACF90
    static const char* GetFilename() {
        return "DBFilesClient\\CinematicSequences.dbc";
    }

// VERIFY: fbebc030 manual
    // OFFSET: 0x008ACFA0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t soundIDRead = SFileReadFile(file, &this->m_soundID, sizeof(this->m_soundID), nullptr, nullptr, 0);
        int32_t cameraRead = SFileReadFile(file, this->m_camera, sizeof(this->m_camera), nullptr, nullptr, 0);

        if (!idRead
            | !soundIDRead
            | !cameraRead) {
            return false;
        }

        return true;
    }
};

#endif
