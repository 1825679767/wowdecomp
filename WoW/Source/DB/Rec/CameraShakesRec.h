#ifndef WOW_SOURCE_DB_REC_CAMERASHAKESREC_H
#define WOW_SOURCE_DB_REC_CAMERASHAKESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct CameraShakesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_shakeType;
    /* 0008 */ int32_t m_direction;
    /* 000C */ float m_amplitude;
    /* 0010 */ float m_frequency;
    /* 0014 */ float m_duration;
    /* 0018 */ float m_phase;
    /* 001C */ float m_coefficient;

// VERIFY: 6b5b2e9a auto
    // OFFSET: 0x008AAA90
    static const char* GetFilename() {
        return "DBFilesClient\\CameraShakes.dbc";
    }

// VERIFY: 5cacea1a manual
    // OFFSET: 0x008BCFA0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t shakeTypeRead = SFileReadFile(file, &this->m_shakeType, sizeof(this->m_shakeType), nullptr, nullptr, 0);
        int32_t directionRead = SFileReadFile(file, &this->m_direction, sizeof(this->m_direction), nullptr, nullptr, 0);
        int32_t amplitudeRead = SFileReadFile(file, &this->m_amplitude, sizeof(this->m_amplitude), nullptr, nullptr, 0);
        int32_t frequencyRead = SFileReadFile(file, &this->m_frequency, sizeof(this->m_frequency), nullptr, nullptr, 0);
        int32_t durationRead = SFileReadFile(file, &this->m_duration, sizeof(this->m_duration), nullptr, nullptr, 0);
        int32_t phaseRead = SFileReadFile(file, &this->m_phase, sizeof(this->m_phase), nullptr, nullptr, 0);
        int32_t coefficientRead = SFileReadFile(file, &this->m_coefficient, sizeof(this->m_coefficient), nullptr, nullptr, 0);

        if (!idRead
            | !shakeTypeRead
            | !directionRead
            | !amplitudeRead
            | !frequencyRead
            | !durationRead
            | !phaseRead
            | !coefficientRead) {
            return false;
        }

        return true;
    }
};

#endif
