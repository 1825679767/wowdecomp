#include "LFGInfo.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"

// STATIC: 0x00BEAA14
int32_t CGLookingForGroup::m_searchJoinedID;

// STATIC: 0x00BEAA24
uint32_t CGLookingForGroup::m_randomDungeonBestChoice;

// STATIC: 0x00BEAA70
TSFixedArray<uint32_t> CGLookingForGroup::m_randomDungeons;

// STATIC: 0x00BEAA7C
TSFixedArray<CGLookingForGroup::LFGEntryInfo> CGLookingForGroup::m_lfdEntries;

// STATIC: 0x00BEAA88
TSFixedArray<CGLookingForGroup::LFGEntryInfo> CGLookingForGroup::m_lfrEntries;

// STATIC: 0x00BEAB20
LFGLIST CGLookingForGroup::m_searchResults;

// STATIC: 0x00ACD880
LFGInfo CGLookingForGroup::m_localInfo;

// STATIC: 0x00ACD9A0
LFGInfo CGLookingForGroup::m_playerInfo;

// STATIC: 0x00ACDAC0
LFGInfo CGLookingForGroup::m_partyInfo;

// STATIC: 0x00ACDBE0
LFGLockInfo CGLookingForGroup::m_playerLockInfo;

// STATIC: 0x00ACDC08
LFGPartyLockInfo CGLookingForGroup::m_partyLockInfo;

// STATIC: 0x00ACDC18
LFGRewardInfo CGLookingForGroup::m_reward;

// STATIC: 0x00BEAA28
LFGQueueStatus CGLookingForGroup::m_queueStatus;

// VERIFY: 0e4c2bfa manual
// OFFSET: 0x00557260
bool CGLookingForGroup::HasRaidDungeon(LFGInfo* info) {
    if (!info->m_joined) {
        return false;
    }

    for (LFGDungeonEntry* entry = info->m_dungeons.Head(); entry; entry = info->m_dungeons.Next(entry)) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(entry->m_slot & 0xFFFFFF);

        if (rec && rec->m_typeID == 2) {
            return true;
        }
    }

    return false;
}
