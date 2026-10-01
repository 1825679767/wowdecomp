#include "PartyFrame.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "WoW/Source/Object/Unit.h"

// STATIC: 0x00BD1948
WOWGUID CGPartyInfo::m_members[4];

// STATIC: 0x00BD1978
uint32_t CGPartyInfo::m_dungeonDifficulty;

// STATIC: 0x00BD0D08
CGPartyMemberState unk_BD0D08[4];

// STATIC: 0x00BD1968
WOWGUID CGPartyInfo::m_leader;

// STATIC: 0x00BD197C
uint32_t CGPartyInfo::m_raidDifficulty;

// STATIC: 0x00BD1980
int32_t CGPartyInfo::m_dynamicDifficulty;

// STATIC: 0x00BD19A8
int32_t CGPartyInfo::m_lfgRestricted;

// STATIC: 0x00ACC6C4
int32_t CGPartyInfo::m_leaderIndex = -1;

// STATIC: 0x00ACC6CC
int32_t CGPartyInfo::m_lootThreshold = 2;

// STATIC: 0x00BD1998
uint32_t CGPartyInfo::m_realNumMembers;

// STATIC: 0x00BD19A0
int32_t CGPartyInfo::m_optOutOfLoot;

// STATIC: 0x00BD19A4
int32_t CGPartyInfo::m_lfgSlot;

// STATIC: 0x00BD0CD0
uint32_t CGPartyInfo::m_memberRoles[4];

// STATIC: 0x00BD0D00
uint32_t CGPartyInfo::unk_BD0D00;

// STATIC: 0x00BD1984
uint32_t CGPartyInfo::unk_BD1984;

// STATIC: 0x00BD1988
uint8_t CGPartyInfo::unk_BD1988;

// STATIC: 0x00BD198A
uint8_t CGPartyInfo::m_playerRole;

// STATIC: 0x00BD1989
uint8_t CGPartyInfo::unk_BD1989;

// STATIC: 0x00BD1990
WOWGUID CGPartyInfo::m_realLeader;

// STATIC: 0x00ACC6C8
int32_t CGPartyInfo::m_lootMethod = 3;

// STATIC: 0x00BD1970
WOWGUID CGPartyInfo::m_lootMaster;

// VERIFY: e02ce9a8 auto
// OFFSET: 0x0052BC50
WOWGUID GetPartyMemberGUID(uint32_t index) {
    if (index >= 4) {
        return WOWGUID();
    }

    return CGPartyInfo::m_members[index];
}

// VERIFY: ad92fae3 auto
// OFFSET: 0x0052BD10
uint32_t CGPartyInfo::GetNumPartyMembers() {
    uint32_t count = 0;

    for (uint32_t i = 0; i < sizeof(CGPartyInfo::m_members) / sizeof(CGPartyInfo::m_members[0]); i++) {
        if (CGPartyInfo::m_members[i]) {
            count++;
        }
    }

    return count;
}

// VERIFY: 0146e892 manual
// OFFSET: 0x0052C6E0
WOWGUID GetPartyPetGUID(uint32_t index) {
    if (index >= 4 || !CGPartyInfo::m_members[index]) {
        return WOWGUID();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPartyInfo::m_members[index], TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (unit) {
        const WOWGUID& pet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
        return pet;
    }

    if (unk_BD0D08[index].m_flags & 0x1) {
        return unk_BD0D08[index].m_pet.m_guid;
    }

    return WOWGUID();
}

// VERIFY: f87f490a auto
// OFFSET: 0x004FB0B0
bool CGPartyInfo::InParty() {
    return CGPartyInfo::m_members[0].m_guid != 0;
}

// VERIFY: d97eb33e manual
// OFFSET: 0x0052C680
int32_t CGPartyInfo::IsMember(const WOWGUID& guid) {
    if (!guid) {
        return 0;
    }

    if (guid == ClntObjMgrGetActivePlayer()) {
        return 1;
    }

    for (uint32_t i = 0; i < sizeof(CGPartyInfo::m_members) / sizeof(CGPartyInfo::m_members[0]); i++) {
        if (CGPartyInfo::m_members[i] == guid) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: fcce6249 manual
// OFFSET: 0x0052BEE0
CGPartyMemberState* CGPartyInfo::GetRemoteStats(const WOWGUID& guid) {
    if (!guid) {
        return nullptr;
    }

    for (uint32_t i = 0; i < sizeof(CGPartyInfo::m_members) / sizeof(CGPartyInfo::m_members[0]); i++) {
        if (CGPartyInfo::m_members[i] == guid) {
            return &unk_BD0D08[i];
        }
    }

    return nullptr;
}

// VERIFY: 730809cb manual
// OFFSET: 0x0052BF80
CGPartyPetState* CGPartyInfo::GetRemotePetStats(const WOWGUID& guid) {
    if (!guid) {
        return nullptr;
    }

    for (uint32_t i = 0; i < sizeof(CGPartyInfo::m_members) / sizeof(CGPartyInfo::m_members[0]); i++) {
        if (CGPartyInfo::m_members[i] && (unk_BD0D08[i].m_flags & 1) && unk_BD0D08[i].m_pet.m_guid == guid) {
            return &unk_BD0D08[i].m_pet;
        }
    }

    return nullptr;
}

// VERIFY: 967e64dc auto
// OFFSET: 0x0052CC30
void CGPartyInfo::CheckClearLFGSlot() {
    int32_t count = CGPartyInfo::m_members[0] ? 1 : 0;

    if (CGPartyInfo::m_members[1]) {
        count++;
    }

    if (CGPartyInfo::m_members[2]) {
        count++;
    }

    if (CGPartyInfo::m_members[3]) {
        count++;
    }

    if (count || !CGPartyInfo::m_lfgSlot) {
        return;
    }

    const LfgDungeonsRec* dungeon = g_lfgDungeonsDB.GetRecord(CGPartyInfo::m_lfgSlot & 0xFFFFFF);

    if (dungeon && dungeon->m_mapID != CGGameUI::m_iCurrentMapID) {
        CGPartyInfo::m_lfgSlot = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_LFG_UPDATE, nullptr);
    }
}

// VERIFY: e3cff148 manual
// OFFSET: 0x0052D210
int32_t CGPartyInfo::IsMemberPet(const WOWGUID& guid, int32_t* index) {
    if (!guid) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        const WOWGUID& pet = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

        if (pet == guid) {
            if (index) {
                *index = -1;
            }

            return 1;
        }
    }

    for (int32_t i = 0; i < 4; i++) {
        if (!CGPartyInfo::m_members[i]) {
            continue;
        }

        CGPlayer_C* member = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGPartyInfo::m_members[i], TYPEMASK_PLAYER, __FILE__, __LINE__));
        bool match;

        if (member) {
            const WOWGUID& pet = member->m_unit->m_unitFieldCharm ? member->m_unit->m_unitFieldCharm : member->m_unit->m_unitFieldSummon;
            match = pet == guid;
        } else {
            match = (unk_BD0D08[i].m_flags & 1) && unk_BD0D08[i].m_pet.m_guid == guid;
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

// VERIFY: bc6fbbce auto
// OFFSET: 0x0052D0E0
void CGPartyInfo::InitializeGame() {
    CGPartyInfo::m_members[0].m_guid = 0;
    CGPartyInfo::m_members[1].m_guid = 0;
    CGPartyInfo::m_members[2].m_guid = 0;
    CGPartyInfo::m_members[3].m_guid = 0;
    CGPartyInfo::unk_BD0D00 = 0;
    CGPartyInfo::m_memberRoles[0] = 0;
    CGPartyInfo::m_memberRoles[1] = 0;
    CGPartyInfo::m_memberRoles[2] = 0;
    CGPartyInfo::m_memberRoles[3] = 0;
    CGPartyInfo::m_leader.m_guid = 0;
    CGPartyInfo::m_leaderIndex = -1;
    CGPartyInfo::m_dungeonDifficulty = 0;
    CGPartyInfo::m_raidDifficulty = 0;
    CGPartyInfo::unk_BD1988 = 0;
    CGPartyInfo::unk_BD1989 = 0;
    CGPartyInfo::m_playerRole = 0;
    CGPartyInfo::m_realLeader.m_guid = 0;
    CGPartyInfo::m_realNumMembers = 0;
    CGPartyInfo::m_lootMethod = 3;
    CGPartyInfo::m_lootMaster.m_guid = 0;
    CGPartyInfo::m_lootThreshold = 2;
    CGPartyInfo::unk_BD1984 = 0;
    CGPartyInfo::m_lfgSlot = 0;
    CGPartyInfo::m_lfgRestricted = 0;

    ClientServices::SetMessageHandler(SMSG_INSTANCE_SAVE_CREATED, Packet_SMSG_INSTANCE_SAVE_CREATED, nullptr);
    ClientServices::SetMessageHandler(SMSG_CHANGE_PLAYER_DIFFICULTY_RESULT, Packet_SMSG_PLAYER_DIFFICULTY_CHANGE, nullptr);
    ConsoleCommandRegister("changePlayerDifficulty", CCommand_SetDifficulty, CATEGORY_DEFAULT, nullptr);
}

// VERIFY: 4f4bffd9 manual
// OFFSET: 0x0052C860
uint32_t CGPartyInfo::GetGroupRole(const WOWGUID& guid) {
    if (guid == WOWGUID()) {
        return 0;
    }

    if (guid == ClntObjMgrGetActivePlayer()) {
        return CGPartyInfo::m_playerRole;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (CGPartyInfo::m_members[i] == guid) {
            return CGPartyInfo::m_memberRoles[i];
        }
    }

    return 0;
}

// VERIFY: 6c9ab10c manual
// OFFSET: 0x0052C8C0
int32_t CGPartyInfo::IsPlayerOrPetInParty(const WOWGUID& guid) {
    if (!static_cast<uint32_t>(guid.m_guid)) {
        return static_cast<uint32_t>(guid.m_guid >> 32);
    }

    for (uint32_t i = 0; i < CGPartyInfo::GetNumPartyMembers(); i++) {
        WOWGUID member = i < 4 ? CGPartyInfo::m_members[i] : WOWGUID();

        if (guid == member) {
            return 1;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(i < 4 ? CGPartyInfo::m_members[i] : WOWGUID(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            CGUnitData* unit = player->m_unit;
            const WOWGUID& pet = unit->m_unitFieldCharm ? unit->m_unitFieldCharm : unit->m_unitFieldSummon;

            if (pet == guid) {
                return 1;
            }
        } else if ((unk_BD0D08[i].m_flags & 1) && unk_BD0D08[i].m_pet.m_guid == guid) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: a81ee60b manual
// OFFSET: 0x0052D310
int32_t CGPartyInfo::IsMemberOrPet(const WOWGUID& guid) {
    if (!guid) {
        return 0;
    }

    if (guid == ClntObjMgrGetActivePlayer()) {
        return 1;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        const WOWGUID& pet = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

        if (pet == guid) {
            return 1;
        }
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (!CGPartyInfo::m_members[i]) {
            continue;
        }

        if (CGPartyInfo::m_members[i] == guid) {
            return 1;
        }

        CGUnit_C* member = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPartyInfo::m_members[i], TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (member) {
            const WOWGUID& pet = member->m_unit->m_unitFieldCharm ? member->m_unit->m_unitFieldCharm : member->m_unit->m_unitFieldSummon;

            if (pet == guid) {
                return 1;
            }
        } else if ((unk_BD0D08[i].m_flags & 1) && unk_BD0D08[i].m_pet.m_guid == guid) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 85f3761e manual
// OFFSET: 0x0052BF30
CGPartyMemberState* CGPartyInfo::GetRemoteStatsFromPet(const WOWGUID& petGuid) {
    for (uint32_t i = 0; i < sizeof(CGPartyInfo::m_members) / sizeof(CGPartyInfo::m_members[0]); i++) {
        if (CGPartyInfo::m_members[i] && unk_BD0D08[i].m_pet.m_guid == petGuid) {
            return &unk_BD0D08[i];
        }
    }

    return nullptr;
}

// VERIFY: 8c75d456 auto
// OFFSET: 0x00572420
CGPartyMemberState* CGPartyInfo::GetMemberState(uint32_t index) {
    if (CGPartyInfo::m_members[index]) {
        return &unk_BD0D08[index];
    }

    return nullptr;
}

// VERIFY: 141be72d auto
// OFFSET: 0x0052C080
void CGPartyInfo::ClearRecruitAFriend(WOWGUID guid) {
    CGPartyMemberState* stats = CGPartyInfo::GetRemoteStats(guid);

    if (stats) {
        stats->m_flags &= ~0x100;
    }
}
