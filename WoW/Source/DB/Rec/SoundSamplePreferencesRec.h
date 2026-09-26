#ifndef WOW_SOURCE_DB_REC_SOUNDSAMPLEPREFERENCESREC_H
#define WOW_SOURCE_DB_REC_SOUNDSAMPLEPREFERENCESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SoundSamplePreferencesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_field0603592001;
    /* 0008 */ int32_t m_field0603592002;
    /* 000C */ int32_t m_eax2SampleRoom;
    /* 0010 */ int32_t m_field0603592004;
    /* 0014 */ int32_t m_field0603592005;
    /* 0018 */ float m_field0603592006;
    /* 001C */ int32_t m_field0603592007;
    /* 0020 */ float m_eax2SampleOcclusionLfratio;
    /* 0024 */ float m_eax2SampleOcclusionRoomRatio;
    /* 0028 */ int32_t m_field0603592010;
    /* 002C */ float m_eax1EffectLevel;
    /* 0030 */ int32_t m_field0603592012;
    /* 0034 */ float m_field0603592013;
    /* 0038 */ float m_eax3SampleExclusion;
    /* 003C */ float m_field0603592015;
    /* 0040 */ int32_t m_field0603592016;

// VERIFY: 3172d423 auto
    // OFFSET: 0x008B7050
    static const char* GetFilename() {
        return "DBFilesClient\\SoundSamplePreferences.dbc";
    }

// VERIFY: 4fef2852 auto
    // OFFSET: 0x008B7060
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t field0603592001Read = SFileReadFile(file, &this->m_field0603592001, sizeof(this->m_field0603592001), nullptr, nullptr, 0);
        int32_t field0603592002Read = SFileReadFile(file, &this->m_field0603592002, sizeof(this->m_field0603592002), nullptr, nullptr, 0);
        int32_t eax2SampleRoomRead = SFileReadFile(file, &this->m_eax2SampleRoom, sizeof(this->m_eax2SampleRoom), nullptr, nullptr, 0);
        int32_t field0603592004Read = SFileReadFile(file, &this->m_field0603592004, sizeof(this->m_field0603592004), nullptr, nullptr, 0);
        int32_t field0603592005Read = SFileReadFile(file, &this->m_field0603592005, sizeof(this->m_field0603592005), nullptr, nullptr, 0);
        int32_t field0603592006Read = SFileReadFile(file, &this->m_field0603592006, sizeof(this->m_field0603592006), nullptr, nullptr, 0);
        int32_t field0603592007Read = SFileReadFile(file, &this->m_field0603592007, sizeof(this->m_field0603592007), nullptr, nullptr, 0);
        int32_t eax2SampleOcclusionLfratioRead = SFileReadFile(file, &this->m_eax2SampleOcclusionLfratio, sizeof(this->m_eax2SampleOcclusionLfratio), nullptr, nullptr, 0);
        int32_t eax2SampleOcclusionRoomRatioRead = SFileReadFile(file, &this->m_eax2SampleOcclusionRoomRatio, sizeof(this->m_eax2SampleOcclusionRoomRatio), nullptr, nullptr, 0);
        int32_t field0603592010Read = SFileReadFile(file, &this->m_field0603592010, sizeof(this->m_field0603592010), nullptr, nullptr, 0);
        int32_t eax1EffectLevelRead = SFileReadFile(file, &this->m_eax1EffectLevel, sizeof(this->m_eax1EffectLevel), nullptr, nullptr, 0);
        int32_t field0603592012Read = SFileReadFile(file, &this->m_field0603592012, sizeof(this->m_field0603592012), nullptr, nullptr, 0);
        int32_t field0603592013Read = SFileReadFile(file, &this->m_field0603592013, sizeof(this->m_field0603592013), nullptr, nullptr, 0);
        int32_t eax3SampleExclusionRead = SFileReadFile(file, &this->m_eax3SampleExclusion, sizeof(this->m_eax3SampleExclusion), nullptr, nullptr, 0);
        int32_t field0603592015Read = SFileReadFile(file, &this->m_field0603592015, sizeof(this->m_field0603592015), nullptr, nullptr, 0);
        int32_t field0603592016Read = SFileReadFile(file, &this->m_field0603592016, sizeof(this->m_field0603592016), nullptr, nullptr, 0);

        if (!idRead
            | !field0603592001Read
            | !field0603592002Read
            | !eax2SampleRoomRead
            | !field0603592004Read
            | !field0603592005Read
            | !field0603592006Read
            | !field0603592007Read
            | !eax2SampleOcclusionLfratioRead
            | !eax2SampleOcclusionRoomRatioRead
            | !field0603592010Read
            | !eax1EffectLevelRead
            | !field0603592012Read
            | !field0603592013Read
            | !eax3SampleExclusionRead
            | !field0603592015Read
            | !field0603592016Read) {
            return false;
        }

        return true;
    }
};

#endif
