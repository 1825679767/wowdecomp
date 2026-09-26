#ifndef WOW_SOURCE_DB_REC_HOLIDAYSREC_H
#define WOW_SOURCE_DB_REC_HOLIDAYSREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct HolidaysRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_duration [10];
    /* 002C */ int32_t m_date [26];
    /* 0094 */ int32_t m_region;
    /* 0098 */ int32_t m_looping;
    /* 009C */ int32_t m_calendarFlags [10];
    /* 00C4 */ int32_t m_holidayNameID;
    /* 00C8 */ int32_t m_holidayDescriptionID;
    /* 00CC */ const char* m_textureFileName;
    /* 00D0 */ int32_t m_priority;
    /* 00D4 */ int32_t m_calendarFilterType;
    /* 00D8 */ int32_t m_flags;

// VERIFY: 7e331197 auto
    // OFFSET: 0x008B00E0
    static const char* GetFilename() {
        return "DBFilesClient\\Holidays.dbc";
    }

// VERIFY: 95d33254 auto
    // OFFSET: 0x008B00F0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t textureFileNameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t durationRead = SFileReadFile(file, this->m_duration, sizeof(this->m_duration), nullptr, nullptr, 0);
        int32_t dateRead = SFileReadFile(file, this->m_date, sizeof(this->m_date), nullptr, nullptr, 0);
        int32_t regionRead = SFileReadFile(file, &this->m_region, sizeof(this->m_region), nullptr, nullptr, 0);
        int32_t loopingRead = SFileReadFile(file, &this->m_looping, sizeof(this->m_looping), nullptr, nullptr, 0);
        int32_t calendarFlagsRead = SFileReadFile(file, this->m_calendarFlags, sizeof(this->m_calendarFlags), nullptr, nullptr, 0);
        int32_t holidayNameIDRead = SFileReadFile(file, &this->m_holidayNameID, sizeof(this->m_holidayNameID), nullptr, nullptr, 0);
        int32_t holidayDescriptionIDRead = SFileReadFile(file, &this->m_holidayDescriptionID, sizeof(this->m_holidayDescriptionID), nullptr, nullptr, 0);
        int32_t textureFileNameRead = SFileReadFile(file, &textureFileNameOffset, sizeof(textureFileNameOffset), nullptr, nullptr, 0);
        int32_t priorityRead = SFileReadFile(file, &this->m_priority, sizeof(this->m_priority), nullptr, nullptr, 0);
        int32_t calendarFilterTypeRead = SFileReadFile(file, &this->m_calendarFilterType, sizeof(this->m_calendarFilterType), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);

        if (!idRead
            | !durationRead
            | !dateRead
            | !regionRead
            | !loopingRead
            | !calendarFlagsRead
            | !holidayNameIDRead
            | !holidayDescriptionIDRead
            | !textureFileNameRead
            | !priorityRead
            | !calendarFilterTypeRead
            | !flagsRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_textureFileName = &stringBuffer[textureFileNameOffset];
        } else {
            this->m_textureFileName = "";
        }

        return true;
    }
};

#endif
