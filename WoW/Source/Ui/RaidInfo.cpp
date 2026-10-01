#include "RaidInfo.h"
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Object/Unit.h"

// STATIC: 0x00BEB568
CGRaidMember* CGRaidInfo::m_members[40];

// STATIC: 0x00BEB608
uint32_t CGRaidInfo::m_numMembers;

// STATIC: 0x00BEB618
static int32_t s_pendingNameQueries;

// STATIC: 0x00BEB528
WOWGUID CGRaidInfo::m_raidTargets[8];

// STATIC: 0x00BEB60C
uint32_t CGRaidInfo::m_realNumMembers;

// STATIC: 0x00BEB610
WOWGUID CGRaidInfo::m_selectedMember;

// STATIC: 0x00BEB61C
uint32_t CGRaidInfo::m_readyCheckTime;

// STATIC: 0x00BEB620
WOWGUID CGRaidInfo::m_readyCheckInitiator;

// STATIC: 0x00BEB628
uint32_t CGRaidInfo::m_readyCheckTimer;

// STATIC: 0x00BD0CE0
int32_t CGRaidInfo::m_partyReadyCheckReady[4];

// STATIC: 0x00BD0CF0
int32_t CGRaidInfo::m_partyReadyCheckPending[4];

// STATIC: 0x00ACE788
uint8_t CGRaidInfo::m_rosterCounter = 0xFF;

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

// VERIFY: 6bb7db76 manual
// OFFSET: 0x005728C0
uint32_t GetRaidTargetIndexFromGuid(WOWGUID guid) {
    uint32_t index = 0;

    while (CGRaidInfo::m_raidTargets[index] != guid) {
        if (++index >= sizeof(CGRaidInfo::m_raidTargets) / sizeof(CGRaidInfo::m_raidTargets[0])) {
            return 8;
        }
    }

    return index;
}

// VERIFY: 37639a60 manual
// OFFSET: 0x005727B0
CGPartyMemberState* CGRaidInfo::GetRemoteStats(const WOWGUID& guid) {
    if (!guid) {
        return nullptr;
    }

    for (uint32_t i = 0; i < sizeof(CGRaidInfo::m_members) / sizeof(CGRaidInfo::m_members[0]); i++) {
        if (CGRaidInfo::m_members[i]->m_guid == guid) {
            return &CGRaidInfo::m_members[i]->m_stats;
        }
    }

    return nullptr;
}

// VERIFY: a4ba6b2b manual
// OFFSET: 0x00572850
CGPartyPetState* CGRaidInfo::GetRemotePetStats(const WOWGUID& guid) {
    if (!guid) {
        return nullptr;
    }

    for (uint32_t i = 0; i < sizeof(CGRaidInfo::m_members) / sizeof(CGRaidInfo::m_members[0]); i++) {
        CGRaidMember* member = CGRaidInfo::m_members[i];

        if (member->m_guid && (member->m_stats.m_flags & 1) && member->m_stats.m_pet.m_guid == guid) {
            return &CGRaidInfo::m_members[i]->m_stats.m_pet;
        }
    }

    return nullptr;
}

// VERIFY: 52124f6c manual
// OFFSET: 0x00573150
int32_t CGRaidInfo::IsPetInRaid(const WOWGUID& guid, int32_t* index) {
    if (!guid) {
        return 0;
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        CGRaidMember* member = CGRaidInfo::m_members[i];

        if (!member) {
            continue;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(member->m_guid, TYPEMASK_PLAYER, __FILE__, __LINE__));
        bool match;

        if (player) {
            const WOWGUID& pet = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
            match = pet == guid;
        } else {
            match = (member->m_stats.m_flags & 1) && member->m_stats.m_pet.m_guid == guid;
        }

        if (match) {
            if (index) {
                *index = i;
            }

            return 1;
        }
    }

    return 0;
}

// VERIFY: 268f30a3 manual
// OFFSET: 0x005756C0
void CGRaidInfo::InitializeGame() {
    ClientServices::SetMessageHandler(MSG_RAID_TARGET_UPDATE, Packet_MSG_RAID_TARGET_UPDATE, nullptr);
    ClientServices::SetMessageHandler(MSG_RAID_READY_CHECK, Packet_MSG_RAID_READY_CHECK, nullptr);
    ClientServices::SetMessageHandler(MSG_RAID_READY_CHECK_CONFIRM, Packet_MSG_RAID_READY_CHECK_CONFIRM, nullptr);
    ClientServices::SetMessageHandler(MSG_RAID_READY_CHECK_FINISHED, Packet_MSG_RAID_READY_CHECK_FINISHED, nullptr);
    ClientServices::SetMessageHandler(SMSG_READY_CHECK_ERROR, Packet_SMSG_RAID_READY_CHECK_ERROR, nullptr);
    ClientServices::SetMessageHandler(MSG_NOTIFY_PARTY_SQUELCH, Packet_MSG_NOTIFY_PARTY_SQUELCH, nullptr);
    ClientServices::SetMessageHandler(SMSG_ECHO_PARTY_SQUELCH, Packet_SMSG_ECHO_PARTY_SQUELCH, nullptr);
    CGRaidInfo::m_numMembers = 0;
    CGRaidInfo::m_realNumMembers = 0;
    CGRaidInfo::m_selectedMember = WOWGUID();
    s_pendingNameQueries = 0;
    CGRaidInfo::m_rosterCounter = 0xFF;
    memset(CGRaidInfo::m_raidTargets, 0, sizeof(CGRaidInfo::m_raidTargets));

    for (uint32_t i = 0; i < sizeof(CGRaidInfo::m_members) / sizeof(CGRaidInfo::m_members[0]); i++) {
        CGRaidInfo::m_members[i] = static_cast<CGRaidMember*>(SMemAlloc(sizeof(CGRaidMember), __FILE__, __LINE__, 0));
        memset(CGRaidInfo::m_members[i], 0, sizeof(CGRaidMember));
    }

    CGRaidInfo::m_readyCheckTime = 0;
    CGRaidInfo::m_readyCheckInitiator = WOWGUID();
    CGRaidInfo::m_readyCheckTimer = 0;
}

// VERIFY: f8514842 manual
// OFFSET: 0x00608130
int32_t CGRaidInfo::GetMemberIndexByGuid(WOWGUID guid) {
    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (CGRaidInfo::m_members[i]->m_guid == guid) {
            return i;
        }
    }

    return -1;
}

// VERIFY: 2a11deb6 manual
// OFFSET: 0x00549670
CGRaidMember* CGRaidInfo::FindMemberByGuid(WOWGUID guid) {
    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (CGRaidInfo::m_members[i]->m_guid == guid) {
            return CGRaidInfo::m_members[i];
        }
    }

    return nullptr;
}

// VERIFY: 2ec03ec8 manual
// OFFSET: 0x00573200
int32_t CGRaidInfo::IsPlayerOrPetInRaid(const WOWGUID& guid) {
    if (!static_cast<uint32_t>(guid.m_guid)) {
        return static_cast<uint32_t>(guid.m_guid >> 32);
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        CGRaidMember* member = CGRaidInfo::m_members[i];

        if (!member) {
            continue;
        }

        if (member->m_guid == guid) {
            return 1;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(member->m_guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            CGUnitData* unit = player->m_unit;
            const WOWGUID& pet = unit->m_unitFieldCharm ? unit->m_unitFieldCharm : unit->m_unitFieldSummon;

            if (pet == guid) {
                return 1;
            }
        } else if ((member->m_stats.m_flags & 1) && member->m_stats.m_pet.m_guid == guid) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 2ceb7b94 manual
// OFFSET: 0x005723E0
int32_t CGRaidInfo::GetPartyMemberIndex(WOWGUID guid) {
    for (int32_t i = 0; i < 4; i++) {
        if (CGPartyInfo::m_members[i] == guid) {
            return i;
        }
    }

    return -1;
}

// VERIFY: 7ec85a21 manual
// OFFSET: 0x00572450
int32_t CGRaidInfo::IsPartyMemberReadyCheckPending(uint32_t index) {
    return index < 4 ? CGRaidInfo::m_partyReadyCheckPending[index] : 0;
}

// VERIFY: 1c65956c manual
// OFFSET: 0x00572470
int32_t CGRaidInfo::IsPartyMemberReadyCheckReady(uint32_t index) {
    return index < 4 ? CGRaidInfo::m_partyReadyCheckReady[index] : 0;
}

// VERIFY: 052e75dd manual
// OFFSET: 0x00572800
CGPartyMemberState* CGRaidInfo::GetRemoteStatsFromPet(const WOWGUID& petGuid) {
    for (uint32_t i = 0; i < sizeof(CGRaidInfo::m_members) / sizeof(CGRaidInfo::m_members[0]); i++) {
        CGRaidMember* member = CGRaidInfo::m_members[i];

        if (member->m_guid && member->m_stats.m_pet.m_guid == petGuid) {
            return &CGRaidInfo::m_members[i]->m_stats;
        }
    }

    return nullptr;
}

// VERIFY: 9af530d4 auto
// OFFSET: 0x00572A90
void CGRaidInfo::ClearRecruitAFriend(WOWGUID guid) {
    CGPartyMemberState* stats = CGRaidInfo::GetRemoteStats(guid);

    if (stats) {
        stats->m_flags &= ~0x100;
    }
}
