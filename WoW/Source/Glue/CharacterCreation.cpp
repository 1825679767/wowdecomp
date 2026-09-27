#include "CharacterCreation.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00AC4224
int32_t CCharacterCreation::m_existingCharacterIndex = -1;

// STATIC: 0x00B6B0D0
uint8_t CCharacterCreation::m_charPreferences[176];

// STATIC: 0x00B6B184
CSimpleModelFFX* CCharacterCreation::m_charCustomizeFrame;

// STATIC: 0x00B6B188
float CCharacterCreation::m_charFacing;

// STATIC: 0x00B6B1CC
TSGrowableArray<int32_t> CCharacterCreation::m_races;

// VERIFY: cbaddeb2 manual
// OFFSET: 0x004E1C20
void CCharacterCreation::Initialize() {
    CCharacterCreation::m_charFacing = 0.0f;
    CCharacterCreation::m_charCustomizeFrame = nullptr;
    CCharacterCreation::m_existingCharacterIndex = -1;
    memset(CCharacterCreation::m_charPreferences, 0, sizeof(CCharacterCreation::m_charPreferences));
    CCharacterCreation::m_races.m_count = 0;

    for (int32_t faction = 0; faction < 2; faction++) {
        for (int32_t i = 0; i < g_chrRacesDB.m_numRecords; i++) {
            ChrRacesRec* race = i >= 0 && i < g_chrRacesDB.m_numRecords ? &g_chrRacesDB.m_records[i] : nullptr;

            if (race && (race->m_flags & 0x1)) {
                continue;
            }

            FactionTemplateRec* factionTemplate = g_factionTemplateDB.GetRecord(race->m_factionID);

            if (!factionTemplate) {
                continue;
            }

            for (int32_t j = 0; j < g_factionGroupDB.m_numRecords; j++) {
                FactionGroupRec* group = j >= 0 && j < g_factionGroupDB.m_numRecords ? &g_factionGroupDB.m_records[j] : nullptr;

                if (!group || !group->m_maskID || !((1 << group->m_maskID) & factionTemplate->m_factionGroup)) {
                    continue;
                }

                if (!SStrCmpI(group->m_internalName, faction == 0 ? "Alliance" : "Horde", STORM_MAX_STR)) {
                    CCharacterCreation::m_races.New(race->m_id);
                }
            }
        }
    }
}
