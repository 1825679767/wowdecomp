#include "RaidInfo.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Ui/GameUI.h"

// STATIC: 0x00BEB568
CGRaidMember* CGRaidInfo::m_members[40];

// STATIC: 0x00BEB608
uint32_t CGRaidInfo::m_numMembers;

// VERIFY: df976625 manual
// OFFSET: 0x005726F0
int32_t IsGuidInMyRaid(const WOWGUID& guid) {
    if (!guid) {
        return 0;
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (CGRaidInfo::m_members[i]->m_guid == guid) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 2f9aefc8 manual
// OFFSET: 0x00572740
int32_t CGRaidInfo::IsPlayerRaidOfficer(const WOWGUID& guid) {
    if (!guid) {
        return 0;
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (CGRaidInfo::m_members[i]->m_guid == guid) {
            return CGRaidInfo::m_members[i]->m_rank > 0 || CGPartyInfo::m_leader == guid;
        }
    }

    return 0;
}

// VERIFY: 3bba49eb auto
// OFFSET: 0x00572F50
int32_t CGRaidInfo::IsPVPRaid() {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (map && map->m_instanceType == 3) {
        return 1;
    }

    return map->m_instanceType == 4;
}

// STATIC: 0x00BEB618
static int32_t s_pendingNameQueries;

// VERIFY: 642f8d2b auto
// OFFSET: 0x00572E40
void CGRaidInfo::EnterWorld() {
    uint32_t i = 0;

    for (; i < CGRaidInfo::m_numMembers; i++) {
        if (!CGRaidInfo::m_members[i]->m_guid) {
            break;
        }
    }

    if (i >= CGRaidInfo::m_numMembers) {
        return;
    }

    CGRaidInfo::m_members[i]->m_guid = ClntObjMgrGetActivePlayer();
    CGRaidMember* member = CGRaidInfo::m_members[i];

    if (g_nameDBCache.GetRecord(member->m_guid.m_guid, member->m_guid, RaidRosterNameCallback, nullptr, false)) {
        if (!s_pendingNameQueries) {
            FrameScript_SignalEvent(SCRIPT_EVENT_RAID_ROSTER_UPDATE, nullptr);
        }
    } else {
        s_pendingNameQueries++;
    }
}
