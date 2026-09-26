#ifndef WOW_SOURCE_DB_REC_SOUNDENTRIESADVANCEDREC_H
#define WOW_SOURCE_DB_REC_SOUNDENTRIESADVANCEDREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SoundEntriesAdvancedRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundEntryID;
    /* 0008 */ float m_innerRadius2d;
    /* 000C */ int32_t m_timeA;
    /* 0010 */ int32_t m_timeB;
    /* 0014 */ int32_t m_timeC;
    /* 0018 */ int32_t m_timeD;
    /* 001C */ int32_t m_randomOffsetRange;
    /* 0020 */ int32_t m_usage;
    /* 0024 */ int32_t m_timeIntervalMin;
    /* 0028 */ int32_t m_timeIntervalMax;
    /* 002C */ int32_t m_volumeSliderCategory;
    /* 0030 */ float m_duckToSfx;
    /* 0034 */ float m_duckToMusic;
    /* 0038 */ float m_duckToAmbience;
    /* 003C */ float m_innerRadiusOfInfluence;
    /* 0040 */ float m_outerRadiusOfInfluence;
    /* 0044 */ int32_t m_timeToDuck;
    /* 0048 */ int32_t m_timeToUnduck;
    /* 004C */ float m_insideAngle;
    /* 0050 */ float m_outsideAngle;
    /* 0054 */ float m_outsideVolume;
    /* 0058 */ float m_outerRadius2d;
    /* 005C */ const char* m_name;

// VERIFY: 59ad0467 auto
    // OFFSET: 0x008BD1F0
    static const char* GetFilename() {
        return "DBFilesClient\\SoundEntriesAdvanced.dbc";
    }

// VERIFY: 9d29b990 auto
    // OFFSET: 0x008BD200
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t nameOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t soundEntryIDRead = SFileReadFile(file, &this->m_soundEntryID, sizeof(this->m_soundEntryID), nullptr, nullptr, 0);
        int32_t innerRadius2dRead = SFileReadFile(file, &this->m_innerRadius2d, sizeof(this->m_innerRadius2d), nullptr, nullptr, 0);
        int32_t timeARead = SFileReadFile(file, &this->m_timeA, sizeof(this->m_timeA), nullptr, nullptr, 0);
        int32_t timeBRead = SFileReadFile(file, &this->m_timeB, sizeof(this->m_timeB), nullptr, nullptr, 0);
        int32_t timeCRead = SFileReadFile(file, &this->m_timeC, sizeof(this->m_timeC), nullptr, nullptr, 0);
        int32_t timeDRead = SFileReadFile(file, &this->m_timeD, sizeof(this->m_timeD), nullptr, nullptr, 0);
        int32_t randomOffsetRangeRead = SFileReadFile(file, &this->m_randomOffsetRange, sizeof(this->m_randomOffsetRange), nullptr, nullptr, 0);
        int32_t usageRead = SFileReadFile(file, &this->m_usage, sizeof(this->m_usage), nullptr, nullptr, 0);
        int32_t timeIntervalMinRead = SFileReadFile(file, &this->m_timeIntervalMin, sizeof(this->m_timeIntervalMin), nullptr, nullptr, 0);
        int32_t timeIntervalMaxRead = SFileReadFile(file, &this->m_timeIntervalMax, sizeof(this->m_timeIntervalMax), nullptr, nullptr, 0);
        int32_t volumeSliderCategoryRead = SFileReadFile(file, &this->m_volumeSliderCategory, sizeof(this->m_volumeSliderCategory), nullptr, nullptr, 0);
        int32_t duckToSfxRead = SFileReadFile(file, &this->m_duckToSfx, sizeof(this->m_duckToSfx), nullptr, nullptr, 0);
        int32_t duckToMusicRead = SFileReadFile(file, &this->m_duckToMusic, sizeof(this->m_duckToMusic), nullptr, nullptr, 0);
        int32_t duckToAmbienceRead = SFileReadFile(file, &this->m_duckToAmbience, sizeof(this->m_duckToAmbience), nullptr, nullptr, 0);
        int32_t innerRadiusOfInfluenceRead = SFileReadFile(file, &this->m_innerRadiusOfInfluence, sizeof(this->m_innerRadiusOfInfluence), nullptr, nullptr, 0);
        int32_t outerRadiusOfInfluenceRead = SFileReadFile(file, &this->m_outerRadiusOfInfluence, sizeof(this->m_outerRadiusOfInfluence), nullptr, nullptr, 0);
        int32_t timeToDuckRead = SFileReadFile(file, &this->m_timeToDuck, sizeof(this->m_timeToDuck), nullptr, nullptr, 0);
        int32_t timeToUnduckRead = SFileReadFile(file, &this->m_timeToUnduck, sizeof(this->m_timeToUnduck), nullptr, nullptr, 0);
        int32_t insideAngleRead = SFileReadFile(file, &this->m_insideAngle, sizeof(this->m_insideAngle), nullptr, nullptr, 0);
        int32_t outsideAngleRead = SFileReadFile(file, &this->m_outsideAngle, sizeof(this->m_outsideAngle), nullptr, nullptr, 0);
        int32_t outsideVolumeRead = SFileReadFile(file, &this->m_outsideVolume, sizeof(this->m_outsideVolume), nullptr, nullptr, 0);
        int32_t outerRadius2dRead = SFileReadFile(file, &this->m_outerRadius2d, sizeof(this->m_outerRadius2d), nullptr, nullptr, 0);
        int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);

        if (!idRead
            | !soundEntryIDRead
            | !innerRadius2dRead
            | !timeARead
            | !timeBRead
            | !timeCRead
            | !timeDRead
            | !randomOffsetRangeRead
            | !usageRead
            | !timeIntervalMinRead
            | !timeIntervalMaxRead
            | !volumeSliderCategoryRead
            | !duckToSfxRead
            | !duckToMusicRead
            | !duckToAmbienceRead
            | !innerRadiusOfInfluenceRead
            | !outerRadiusOfInfluenceRead
            | !timeToDuckRead
            | !timeToUnduckRead
            | !insideAngleRead
            | !outsideAngleRead
            | !outsideVolumeRead
            | !outerRadius2dRead
            | !nameRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_name = &stringBuffer[nameOffset];
        } else {
            this->m_name = "";
        }

        return true;
    }
};

#endif
