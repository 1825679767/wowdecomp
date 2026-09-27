#include <cstddef>
#include <cstdint>
#include <cstring>
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"

// STATIC: 0x00DCEAF8
static int32_t s_vocalErrorSounds[68];

// VERIFY: 2b3e2ff2 manual
// OFFSET: 0x00986C90
void SI2::InitVocalErrorSounds(int32_t race, uint32_t sex) {
    memset(s_vocalErrorSounds, 0, sizeof(s_vocalErrorSounds));

    if (sex > 1) {
        return;
    }

    for (int32_t i = 0; i < g_vocalUISoundsDB.m_numRecords; i++) {
        VocalUISoundsRec* rec = (i < 0 || i >= g_vocalUISoundsDB.m_numRecords) ? nullptr : &g_vocalUISoundsDB.m_records[i];

        if (rec && rec->m_vocalUienum < 68 && rec->m_raceID == race) {
            s_vocalErrorSounds[rec->m_vocalUienum] = rec->m_normalSoundID[sex];
        }
    }
}
