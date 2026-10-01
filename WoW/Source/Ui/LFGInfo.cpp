#include "LFGInfo.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/Rec/LfgDungeonExpansionRec.h"
#include "WoW/Source/DB/Rec/LfgDungeonGroupRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowServices/RealmConnection.h"

// STATIC: 0x00BEAA14
int32_t CGLookingForGroup::m_searchJoinedID;

// STATIC: 0x00BEAA18
uint32_t CGLookingForGroup::m_lastProposalID;

// STATIC: 0x00BEAA24
uint32_t CGLookingForGroup::m_randomDungeonBestChoice;

// STATIC: 0x00BEAA70
TSFixedArray<uint32_t> CGLookingForGroup::m_randomDungeons;

// STATIC: 0x00BEAA7C
TSFixedArray<CGLookingForGroup::LFGEntryInfo> CGLookingForGroup::m_lfdEntries;

// STATIC: 0x00BEAA88
TSFixedArray<CGLookingForGroup::LFGEntryInfo> CGLookingForGroup::m_lfrEntries;

// STATIC: 0x00BEA820
CGLookingForGroup::LFGEntryInfo CGLookingForGroup::m_unkBEA820[7];

// STATIC: 0x00BEA980
LFGProposal CGLookingForGroup::m_proposal;

// STATIC: 0x00BEAA10
uint8_t CGLookingForGroup::m_unkBEAA10;

// STATIC: 0x00BEAA98
LFGLIST CGLookingForGroup::m_prevSearchResults;

// STATIC: 0x00ACD660
uint8_t CGLookingForGroup::m_unkACD660;

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

// STATIC: 0x00BEAA4C
int32_t CGLookingForGroup::m_roleCheckState;

// STATIC: 0x00BEAA50
TSGrowableArray<uint32_t> CGLookingForGroup::m_roleCheckSlots;

// STATIC: 0x00BEAA60
TSGrowableArray<LFGRoleCheckMember> CGLookingForGroup::m_roleCheckMembers;

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

// VERIFY: 9c153620 manual
// OFFSET: 0x005536A0
static int QSortLFGDungeons(const void* a, const void* b) {
    const CGLookingForGroup::LFGEntryInfo* left = static_cast<const CGLookingForGroup::LFGEntryInfo*>(a);
    const CGLookingForGroup::LFGEntryInfo* right = static_cast<const CGLookingForGroup::LFGEntryInfo*>(b);
    int32_t leftID = left->m_id;
    int32_t rightID = right->m_id;
    bool leftIsGroup = leftID < 0;
    bool rightIsGroup = rightID < 0;
    int32_t leftGroupID = leftIsGroup ? -leftID : left->m_order;
    int32_t rightGroupID = rightIsGroup ? -rightID : right->m_order;

    if (leftGroupID != rightGroupID) {
        int32_t leftOrder = -1;
        int32_t rightOrder = -1;

        if (leftGroupID) {
            LfgDungeonGroupRec* group = g_lfgDungeonGroupDB.GetRecord(leftGroupID);
            leftOrder = group ? group->m_orderIndex : 0;
        }

        if (rightGroupID) {
            LfgDungeonGroupRec* group = g_lfgDungeonGroupDB.GetRecord(rightGroupID);
            rightOrder = group ? group->m_orderIndex : 0;
        }

        if (leftOrder != rightOrder) {
            return leftOrder - rightOrder;
        }
    }

    if (leftIsGroup != rightIsGroup) {
        return 2 * !leftIsGroup - 1;
    }

    LfgDungeonsRec* leftRec = g_lfgDungeonsDB.GetRecord(leftID);
    LfgDungeonsRec* rightRec = g_lfgDungeonsDB.GetRecord(rightID);

    if (leftRec->m_orderIndex != rightRec->m_orderIndex) {
        return leftRec->m_orderIndex - rightRec->m_orderIndex;
    }

    LfgDungeonExpansionRec* leftExpansion = GetLfgDungeonExpansionRec(leftRec->m_id, ClientServices::Connection()->m_accountExpansion);
    LfgDungeonExpansionRec* rightExpansion = GetLfgDungeonExpansionRec(rightRec->m_id, ClientServices::Connection()->m_accountExpansion);
    int32_t leftLevel = leftExpansion ? leftExpansion->m_hardLevelMin + leftExpansion->m_hardLevelMax : leftRec->m_minLevel + leftRec->m_maxLevel;
    int32_t rightLevel = rightExpansion ? rightExpansion->m_hardLevelMin + rightExpansion->m_hardLevelMax : rightRec->m_minLevel + rightRec->m_maxLevel;

    if (leftLevel / 2 != rightLevel / 2) {
        return leftLevel / 2 - rightLevel / 2;
    }

    return SStrCmpUTF8I(leftRec->m_name, rightRec->m_name, 0x7FFFFFFF);
}

// VERIFY: ab53bd79 manual
// OFFSET: 0x00558350
void CGLookingForGroup::UpdateLFDEntries() {
    uint32_t count = 0;

    for (int32_t i = 0; i < g_lfgDungeonsDB.m_numRecords; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

        if (rec && (rec->m_flags & 0x2) && rec->m_typeID != 6 && !(rec->m_flags & 0x4)) {
            count++;
        }
    }

    for (int32_t i = 0; i < g_lfgDungeonGroupDB.m_numRecords; i++) {
        LfgDungeonGroupRec* rec = g_lfgDungeonGroupDB.GetRecordByIndex(i);

        if (rec && rec->m_typeID) {
            count++;
        }
    }

    CGLookingForGroup::m_lfdEntries.SetCount(count);

    uint32_t index = 0;

    for (int32_t i = 0; i < g_lfgDungeonsDB.m_numRecords; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

        if (rec && (rec->m_flags & 0x2) && rec->m_typeID != 6 && !(rec->m_flags & 0x4)) {
            CGLookingForGroup::m_lfdEntries.m_data[index].m_id = rec->m_id;
            CGLookingForGroup::m_lfdEntries.m_data[index].m_order = rec->m_groupID;
            index++;
        }
    }

    for (int32_t i = 0; i < g_lfgDungeonGroupDB.m_numRecords; i++) {
        LfgDungeonGroupRec* rec = g_lfgDungeonGroupDB.GetRecordByIndex(i);

        if (rec && rec->m_typeID) {
            CGLookingForGroup::m_lfdEntries.m_data[index].m_id = -rec->m_id;
            CGLookingForGroup::m_lfdEntries.m_data[index].m_order = rec->m_parentGroupID;
            index++;
        }
    }

    qsort(CGLookingForGroup::m_lfdEntries.m_data, count, sizeof(CGLookingForGroup::LFGEntryInfo), QSortLFGDungeons);
}

// VERIFY: 76abef22 manual
// OFFSET: 0x00558520
void CGLookingForGroup::UpdateLFREntries() {
    uint32_t count = 0;

    for (int32_t i = 0; i < g_lfgDungeonsDB.m_numRecords; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

        if (rec && rec->m_typeID == 2) {
            count++;
        }
    }

    for (int32_t i = 0; i < g_lfgDungeonGroupDB.m_numRecords; i++) {
        if (g_lfgDungeonGroupDB.GetRecordByIndex(i)) {
            count++;
        }
    }

    CGLookingForGroup::m_lfrEntries.SetCount(count);

    uint32_t index = 0;

    for (int32_t i = 0; i < g_lfgDungeonsDB.m_numRecords; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

        if (rec && rec->m_typeID == 2) {
            CGLookingForGroup::m_lfrEntries.m_data[index].m_id = rec->m_id;
            CGLookingForGroup::m_lfrEntries.m_data[index].m_order = rec->m_groupID;
            index++;
        }
    }

    for (int32_t i = 0; i < g_lfgDungeonGroupDB.m_numRecords; i++) {
        LfgDungeonGroupRec* rec = g_lfgDungeonGroupDB.GetRecordByIndex(i);

        if (rec) {
            CGLookingForGroup::m_lfrEntries.m_data[index].m_id = -rec->m_id;
            CGLookingForGroup::m_lfrEntries.m_data[index].m_order = rec->m_parentGroupID;
            index++;
        }
    }

    qsort(CGLookingForGroup::m_lfrEntries.m_data, count, sizeof(CGLookingForGroup::LFGEntryInfo), QSortLFGDungeons);
}

// VERIFY: a1411244 manual
// OFFSET: 0x00558A70
void LFGRewardInfo::Reset() {
    this->m_randomSlot = 0;
    this->m_actualSlot = 0;
    this->m_first = 0;
    this->m_strangers = 0;
    this->m_baseMoney = 0;
    this->m_baseXP = 0;
    this->m_variableMoney = 0;
    this->m_variableXP = 0;

    for (LFGRewardItem* item = this->m_items.Head(); !this->m_items.IsEnd(item); ) {
        LFGRewardItem* next = item->RawNext();
        delete item;
        item = next;
    }
}

// VERIFY: 4c628326 manual
// OFFSET: 0x00558D70
LFGDungeonEntry::~LFGDungeonEntry() {
    for (LFGRewardItem* item = this->m_rewards.Head(); !this->m_rewards.IsEnd(item); ) {
        LFGRewardItem* next = item->RawNext();
        delete item;
        item = next;
    }
}

// VERIFY: 82f6daec manual
// OFFSET: 0x00558E60
void LFGInfo::Reset() {
    this->m_joined = 0;
    this->unk_0001 = 0;
    this->m_queued = 0;
    this->unk_0003 = 0;
    this->unk_0004 = 0;
    this->unk_0005 = 0;
    this->m_roles[0] = 0;
    this->m_roles[1] = 0;
    this->m_roles[2] = 0;

    for (LFGDungeonEntry* dungeon = this->m_dungeons.Head(); !this->m_dungeons.IsEnd(dungeon); ) {
        LFGDungeonEntry* next = dungeon->RawNext();
        delete dungeon;
        dungeon = next;
    }

    this->m_comment[0] = '\0';
    this->m_numDungeons = 0;
}

// VERIFY: ff1fe1c8 manual
// OFFSET: 0x00559800
void LFGLockInfo::ClearDungeons() {
    for (LFGDungeonEntry* dungeon = this->m_dungeons.Head(); !this->m_dungeons.IsEnd(dungeon); ) {
        LFGDungeonEntry* next = dungeon->RawNext();
        delete dungeon;
        dungeon = next;
    }
}

// VERIFY: 9ea636a6 manual
// OFFSET: 0x0055AD80
LFGLockInfo::~LFGLockInfo() {
    this->m_valid = 0;
    this->m_guid.m_guid = 0;
    this->ClearDungeons();
}

// VERIFY: 05875565 manual
// OFFSET: 0x0055AE20
void LFGPartyLockInfo::Clear() {
    this->m_valid = 0;

    for (LFGLockInfo* member = this->m_members.Head(); !this->m_members.IsEnd(member); ) {
        LFGLockInfo* next = member->RawNext();
        delete member;
        member = next;
    }
}

// VERIFY: 71940fb5 manual
// OFFSET: 0x0055D440
void CGLookingForGroup::InitializeGame() {
    CGLookingForGroup::m_prevSearchResults.Clear();
    CGLookingForGroup::m_searchResults.Clear();
    CGLookingForGroup::m_localInfo.Reset();
    CGLookingForGroup::m_playerInfo.Reset();
    CGLookingForGroup::m_partyInfo.Reset();
    CGLookingForGroup::m_playerLockInfo.m_valid = 0;
    CGLookingForGroup::m_playerLockInfo.m_guid.m_guid = 0;
    CGLookingForGroup::m_playerLockInfo.ClearDungeons();
    CGLookingForGroup::m_partyLockInfo.Clear();
    CGLookingForGroup::m_queueStatus.m_lfgID = 0;
    CGLookingForGroup::m_queueStatus.m_averageWait = 0;
    CGLookingForGroup::m_queueStatus.m_tankWait = 0;
    CGLookingForGroup::m_queueStatus.m_healerWait = 0;
    CGLookingForGroup::m_queueStatus.m_damageWait = 0;
    CGLookingForGroup::m_queueStatus.m_myWait = 0;
    CGLookingForGroup::m_queueStatus.m_tanksNeeded = 0;
    CGLookingForGroup::m_queueStatus.m_healersNeeded = 0;
    CGLookingForGroup::m_queueStatus.m_damageNeeded = 0;
    CGLookingForGroup::m_queueStatus.m_statusTime = 0;
    CGLookingForGroup::m_queueStatus.m_queuedTime = 0;
    CGLookingForGroup::m_reward.Reset();
    CGLookingForGroup::m_unkBEAA10 = 0;
    CGLookingForGroup::m_searchJoinedID = 0;
    CGLookingForGroup::m_randomDungeonBestChoice = 0;
    CGLookingForGroup::m_randomDungeons.Clear();
    CGLookingForGroup::UpdateLFDEntries();
    CGLookingForGroup::UpdateLFREntries();
    CGLookingForGroup::m_proposal.m_id = 0;
    CGLookingForGroup::m_proposal.m_state = 0;
    CGLookingForGroup::m_proposal.unk_0008 = 0;
    CGLookingForGroup::m_proposal.m_completedMask = 0;
    CGLookingForGroup::m_proposal.unk_0010 = 0;
    memset(CGLookingForGroup::m_proposal.m_members, 0, sizeof(CGLookingForGroup::m_proposal.m_members));
    CGLookingForGroup::m_roleCheckState = 0;
    CGLookingForGroup::m_roleCheckSlots.m_count = 0;
    CGLookingForGroup::m_roleCheckMembers.m_count = 0;

    for (uint32_t i = 0; i < 7; i++) {
        CGLookingForGroup::m_unkBEA820[i].m_id = i;
        CGLookingForGroup::m_unkBEA820[i].m_order = 0;
    }

    CGLookingForGroup::m_unkACD660 = 1;

    ClientServices::SetMessageHandler(SMSG_LFG_LFR_LIST, Packet_SMSG_LFG_SEARCH_RESULTS, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_UPDATE_PLAYER, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_UPDATE_PARTY, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_UPDATE_SEARCH, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_QUEUE_STATUS, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_DISABLED, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_PROPOSAL_UPDATE, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_ROLE_CHECK_UPDATE, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_ROLE_CHOSEN, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_JOIN_RESULT, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_BOOT_PROPOSAL_UPDATE, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_PLAYER_INFO, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_PARTY_INFO, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_PLAYER_REWARD, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_TELEPORT_DENIED, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_LFG_OFFER_CONTINUE, LFGPacketHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_OPEN_LFG_DUNGEON_FINDER, Packet_SMSG_LFG_OPEN_FROM_GOSSIP, nullptr);
}

// VERIFY: 40f0cb62 manual
// OFFSET: 0x006349F0
LfgDungeonExpansionRec* GetLfgDungeonExpansionRec(int32_t dungeonID, int32_t expansionLevel) {
    for (int32_t i = 0; i < g_lfgDungeonExpansionDB.m_numRecords; i++) {
        if (g_lfgDungeonExpansionDB.GetRecordByIndex(i)->m_lfgID != dungeonID) {
            continue;
        }

        for (int32_t j = i; j < g_lfgDungeonExpansionDB.m_numRecords; j++) {
            LfgDungeonExpansionRec* rec = g_lfgDungeonExpansionDB.GetRecordByIndex(j);

            if (rec->m_lfgID != dungeonID) {
                return nullptr;
            }

            if (rec->m_expansionLevel == expansionLevel) {
                return rec;
            }
        }
    }

    return nullptr;
}
