#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITSOUND_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITSOUND_C_H

#include <cstddef>
#include <cstdint>

class CreatureSoundDataRec;

struct DEATTHUDSOUNDINFO {
    /* 0000 */ int32_t m_soundEntryId;
    /* 0004 */ int32_t m_soundEntryIdWater;

    DEATTHUDSOUNDINFO();
};

void UnitSoundInitialize();
void UnitSoundShutdown();
int32_t GetSoundID(const CreatureSoundDataRec* soundData, uint32_t soundType);
int32_t CheckUnitSoundTimer(uint32_t soundType);

#endif
