#ifndef WOW_SOURCE_DB_REC_SOUNDPROVIDERPREFERENCESREC_H
#define WOW_SOURCE_DB_REC_SOUNDPROVIDERPREFERENCESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SoundProviderPreferencesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_description;
    /* 0008 */ int32_t m_flags;
    /* 000C */ int32_t m_eaxenvironmentSelection;
    /* 0010 */ float m_eaxdecayTime;
    /* 0014 */ float m_eax2EnvironmentSize;
    /* 0018 */ float m_eax2EnvironmentDiffusion;
    /* 001C */ int32_t m_eax2Room;
    /* 0020 */ int32_t m_eax2RoomHf;
    /* 0024 */ float m_eax2DecayHfratio;
    /* 0028 */ int32_t m_eax2Reflections;
    /* 002C */ float m_eax2ReflectionsDelay;
    /* 0030 */ int32_t m_eax2Reverb;
    /* 0034 */ float m_eax2ReverbDelay;
    /* 0038 */ float m_eax2RoomRolloff;
    /* 003C */ float m_eax2AirAbsorption;
    /* 0040 */ int32_t m_eax3RoomLf;
    /* 0044 */ float m_eax3DecayLfratio;
    /* 0048 */ float m_eax3EchoTime;
    /* 004C */ float m_eax3EchoDepth;
    /* 0050 */ float m_eax3ModulationTime;
    /* 0054 */ float m_eax3ModulationDepth;
    /* 0058 */ float m_eax3Hfreference;
    /* 005C */ float m_eax3Lfreference;

// VERIFY: b7efd8e2 auto
    // OFFSET: 0x008B6D10
    static const char* GetFilename() {
        return "DBFilesClient\\SoundProviderPreferences.dbc";
    }

// VERIFY: b605846a auto
    // OFFSET: 0x008B6D20
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t descriptionOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t descriptionRead = SFileReadFile(file, &descriptionOffset, sizeof(descriptionOffset), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t eaxenvironmentSelectionRead = SFileReadFile(file, &this->m_eaxenvironmentSelection, sizeof(this->m_eaxenvironmentSelection), nullptr, nullptr, 0);
        int32_t eaxdecayTimeRead = SFileReadFile(file, &this->m_eaxdecayTime, sizeof(this->m_eaxdecayTime), nullptr, nullptr, 0);
        int32_t eax2EnvironmentSizeRead = SFileReadFile(file, &this->m_eax2EnvironmentSize, sizeof(this->m_eax2EnvironmentSize), nullptr, nullptr, 0);
        int32_t eax2EnvironmentDiffusionRead = SFileReadFile(file, &this->m_eax2EnvironmentDiffusion, sizeof(this->m_eax2EnvironmentDiffusion), nullptr, nullptr, 0);
        int32_t eax2RoomRead = SFileReadFile(file, &this->m_eax2Room, sizeof(this->m_eax2Room), nullptr, nullptr, 0);
        int32_t eax2RoomHfRead = SFileReadFile(file, &this->m_eax2RoomHf, sizeof(this->m_eax2RoomHf), nullptr, nullptr, 0);
        int32_t eax2DecayHfratioRead = SFileReadFile(file, &this->m_eax2DecayHfratio, sizeof(this->m_eax2DecayHfratio), nullptr, nullptr, 0);
        int32_t eax2ReflectionsRead = SFileReadFile(file, &this->m_eax2Reflections, sizeof(this->m_eax2Reflections), nullptr, nullptr, 0);
        int32_t eax2ReflectionsDelayRead = SFileReadFile(file, &this->m_eax2ReflectionsDelay, sizeof(this->m_eax2ReflectionsDelay), nullptr, nullptr, 0);
        int32_t eax2ReverbRead = SFileReadFile(file, &this->m_eax2Reverb, sizeof(this->m_eax2Reverb), nullptr, nullptr, 0);
        int32_t eax2ReverbDelayRead = SFileReadFile(file, &this->m_eax2ReverbDelay, sizeof(this->m_eax2ReverbDelay), nullptr, nullptr, 0);
        int32_t eax2RoomRolloffRead = SFileReadFile(file, &this->m_eax2RoomRolloff, sizeof(this->m_eax2RoomRolloff), nullptr, nullptr, 0);
        int32_t eax2AirAbsorptionRead = SFileReadFile(file, &this->m_eax2AirAbsorption, sizeof(this->m_eax2AirAbsorption), nullptr, nullptr, 0);
        int32_t eax3RoomLfRead = SFileReadFile(file, &this->m_eax3RoomLf, sizeof(this->m_eax3RoomLf), nullptr, nullptr, 0);
        int32_t eax3DecayLfratioRead = SFileReadFile(file, &this->m_eax3DecayLfratio, sizeof(this->m_eax3DecayLfratio), nullptr, nullptr, 0);
        int32_t eax3EchoTimeRead = SFileReadFile(file, &this->m_eax3EchoTime, sizeof(this->m_eax3EchoTime), nullptr, nullptr, 0);
        int32_t eax3EchoDepthRead = SFileReadFile(file, &this->m_eax3EchoDepth, sizeof(this->m_eax3EchoDepth), nullptr, nullptr, 0);
        int32_t eax3ModulationTimeRead = SFileReadFile(file, &this->m_eax3ModulationTime, sizeof(this->m_eax3ModulationTime), nullptr, nullptr, 0);
        int32_t eax3ModulationDepthRead = SFileReadFile(file, &this->m_eax3ModulationDepth, sizeof(this->m_eax3ModulationDepth), nullptr, nullptr, 0);
        int32_t eax3HfreferenceRead = SFileReadFile(file, &this->m_eax3Hfreference, sizeof(this->m_eax3Hfreference), nullptr, nullptr, 0);
        int32_t eax3LfreferenceRead = SFileReadFile(file, &this->m_eax3Lfreference, sizeof(this->m_eax3Lfreference), nullptr, nullptr, 0);

        if (!idRead
            | !descriptionRead
            | !flagsRead
            | !eaxenvironmentSelectionRead
            | !eaxdecayTimeRead
            | !eax2EnvironmentSizeRead
            | !eax2EnvironmentDiffusionRead
            | !eax2RoomRead
            | !eax2RoomHfRead
            | !eax2DecayHfratioRead
            | !eax2ReflectionsRead
            | !eax2ReflectionsDelayRead
            | !eax2ReverbRead
            | !eax2ReverbDelayRead
            | !eax2RoomRolloffRead
            | !eax2AirAbsorptionRead
            | !eax3RoomLfRead
            | !eax3DecayLfratioRead
            | !eax3EchoTimeRead
            | !eax3EchoDepthRead
            | !eax3ModulationTimeRead
            | !eax3ModulationDepthRead
            | !eax3HfreferenceRead
            | !eax3LfreferenceRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_description = &stringBuffer[descriptionOffset];
        } else {
            this->m_description = "";
        }

        return true;
    }
};

#endif
