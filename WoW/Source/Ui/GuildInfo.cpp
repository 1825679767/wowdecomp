#include "GuildInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstdlib>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Ui/CGAutoCompleteName.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "Engine/Source/Os/TimeManager.h"

// STATIC: 0x00C22ACC
int32_t CGGuildInfo::m_numEventLogEntries;

// STATIC: 0x00C22AB8
uint32_t CGGuildInfo::m_numRanks;
// STATIC: 0x00C21E60
GuildRankInfo CGGuildInfo::m_rankInfo[10];

// STATIC: 0x00C22090
char CGGuildInfo::m_infoText[2004];

// STATIC: 0x00C22868
char CGGuildInfo::m_motd[516];

// STATIC: 0x00C22A70
GuildRosterSortCriteria CGGuildInfo::m_sortCriteria[8];

// STATIC: 0x00C22AB0
uint32_t CGGuildInfo::m_numMembers;

// STATIC: 0x00C22AB4
uint32_t CGGuildInfo::m_numOnlineMembers;

// STATIC: 0x00C22ABC
int32_t CGGuildInfo::m_showOffline;

// STATIC: 0x00C22AC0
WOWGUID CGGuildInfo::m_selectedMember;

// STATIC: 0x00C22AC8
int32_t CGGuildInfo::unk_C22AC8;

// STATIC: 0x00C22AD0
int32_t CGGuildInfo::unk_C22AD0;

// STATIC: 0x00C22AD4
uint8_t CGGuildInfo::unk_C22AD4;

// STATIC: 0x00C22AD8
TSGrowableArray<GuildMemberInfo*> CGGuildInfo::m_roster;

// STATIC: 0x00AD07A8
int32_t s_guildControlRank = -1;


// VERIFY: 3d0e376e manual
// OFFSET: 0x005CAA40
void CGGuildInfo::QueryPermissions() {
    CDataStore permissions;
    permissions.Put(static_cast<uint32_t>(MSG_GUILD_PERMISSIONS));
    permissions.Finalize();
    ClientServices::Send2(&permissions);

    CDataStore withdrawn;
    withdrawn.Put(static_cast<uint32_t>(MSG_GUILD_BANK_MONEY_WITHDRAWN));
    withdrawn.Finalize();
    ClientServices::Send2(&withdrawn);
}

// VERIFY: f0cf42f9 auto
// OFFSET: 0x005CD0B0
void CGGuildInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_GUILD_ROSTER, Packet_SMSG_GUILD_ROSTER, nullptr);
    ClientServices::SetMessageHandler(MSG_GUILD_PERMISSIONS, Packet_MSG_GUILD_PERMISSIONS, nullptr);
    ClientServices::SetMessageHandler(MSG_GUILD_EVENT_LOG_QUERY, Packet_MSG_GUILD_EVENT_LOG_QUERY, nullptr);

    CGGuildInfo::m_numMembers = 0;
    CGGuildInfo::m_numOnlineMembers = 0;
    CGGuildInfo::m_numRanks = 0;
    CGGuildInfo::m_showOffline = 0;
    CGGuildInfo::m_selectedMember.m_guid = 0;
    CGGuildInfo::unk_C22AC8 = 0;
    CGGuildInfo::m_numEventLogEntries = 0;
    CGGuildInfo::unk_C22AD0 = 0;
    CGGuildInfo::m_infoText[0] = '\0';
    CGGuildInfo::m_motd[0] = '\0';

    for (int32_t i = 0; i < 8; i++) {
        CGGuildInfo::m_sortCriteria[i].m_column = i;
        CGGuildInfo::m_sortCriteria[i].m_descending = 0;
    }

    s_guildControlRank = -1;
}

// VERIFY: 6089fd22 auto
// OFFSET: 0x005CA870
void CGGuildInfo::SetShowOffline(int32_t show) {
    if (show == CGGuildInfo::m_showOffline) {
        return;
    }

    CGGuildInfo::m_showOffline = show;

    if (CGGuildInfo::m_numMembers) {
        qsort(CGGuildInfo::m_roster.m_data, CGGuildInfo::m_numMembers, sizeof(GuildMemberInfo*), QSortRoster);
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, nullptr);
    }
}

// VERIFY: 504618a5 manual
// OFFSET: 0x005CB900
void CGGuildInfo::EnterWorld() {
    CGGuildInfo::m_infoText[0] = '\0';
    CGGuildInfo::m_motd[0] = '\0';

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(MSG_GUILD_PERMISSIONS));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    for (int32_t i = 0; i < 6; i++) {
        CGGuildInfo::m_rankInfo[0].m_bankTabRights[i] = -1;
        CGGuildInfo::m_rankInfo[0].m_bankTabStacksPerDay[i] = -1;
    }
}

// VERIFY: 245e09f1 manual
// OFFSET: 0x005CC560
void CGGuildInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_GUILD_ROSTER);
    ClientServices::ClearMessageHandler(MSG_GUILD_PERMISSIONS);
    ClientServices::ClearMessageHandler(MSG_GUILD_EVENT_LOG_QUERY);
    uint32_t count = CGGuildInfo::m_roster.m_count;

    for (uint32_t i = 0; i < count; i++) {
        delete CGGuildInfo::m_roster.m_data[i];
        CGGuildInfo::m_roster.m_data[i] = nullptr;
    }

    CGGuildInfo::m_numMembers = 0;
    CGGuildInfo::m_roster.m_count = 0;
}

// VERIFY: 7db72a57 manual
// OFFSET: 0x005CB9F0
int32_t Packet_MSG_GUILD_PERMISSIONS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (!ClntObjMgrGetActivePlayerObj()) {
        return 0;
    }

    uint32_t rank;
    msg->Get(rank);
    msg->Get(CGGuildInfo::m_rankInfo[rank].m_rights);
    msg->Get(reinterpret_cast<uint32_t&>(CGGuildInfo::m_rankInfo[rank].m_bankMoneyPerDay));
    uint8_t numTabs = 0;
    msg->Get(numTabs);
    CGGuildBankInfo::m_numTabs = numTabs;

    for (uint32_t i = 0; i < 6; i++) {
        msg->Get(CGGuildInfo::m_rankInfo[rank].m_bankTabRights[i]);
        msg->Get(reinterpret_cast<uint32_t&>(CGGuildInfo::m_rankInfo[rank].m_bankTabStacksPerDay[i]));
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, nullptr);

    return 1;
}

// VERIFY: 9a68ee60 manual
// OFFSET: 0x005CC5D0
int32_t Packet_SMSG_GUILD_ROSTER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t allocated = CGGuildInfo::m_roster.m_count;
    CGGuildInfo::m_numOnlineMembers = 0;
    CGGuildInfo::m_numRanks = 0;
    msg->Get(CGGuildInfo::m_numMembers);

    if (CGGuildInfo::m_numMembers > allocated) {
        CGGuildInfo::m_roster.SetCount(CGGuildInfo::m_numMembers);

        for (; allocated < CGGuildInfo::m_numMembers; allocated++) {
            CGGuildInfo::m_roster.m_data[allocated] = static_cast<GuildMemberInfo*>(SMemAlloc(sizeof(GuildMemberInfo), __FILE__, __LINE__, 0));
        }
    }

    char motd[516];
    msg->GetString(motd, sizeof(motd));
    ScrubString(motd, CGGuildInfo::m_motd, 129, sizeof(CGGuildInfo::m_motd), 1);
    char infoText[2004];
    msg->GetString(infoText, sizeof(infoText));
    ScrubString(infoText, CGGuildInfo::m_infoText, 501, sizeof(CGGuildInfo::m_infoText), 0);
    msg->Get(CGGuildInfo::m_numRanks);
    uint32_t rank = 0;

    for (; rank < CGGuildInfo::m_numRanks; rank++) {
        GuildRankInfo& info = CGGuildInfo::m_rankInfo[rank];
        msg->Get(info.m_rights);
        msg->Get(reinterpret_cast<uint32_t&>(info.m_bankMoneyPerDay));

        for (uint32_t i = 0; i < 6; i++) {
            msg->Get(info.m_bankTabRights[i]);
            msg->Get(reinterpret_cast<uint32_t&>(info.m_bankTabStacksPerDay[i]));
        }
    }

    for (; rank < 10; rank++) {
        memset(&CGGuildInfo::m_rankInfo[rank], 0, sizeof(GuildRankInfo));
    }

    for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
        GuildMemberInfo* member = CGGuildInfo::m_roster.m_data[i];
        msg->Get(member->m_guid.m_guid);
        uint8_t status;
        msg->Get(status);
        CGGuildInfo::m_roster.m_data[i]->m_online = status != 0;
        CGGuildInfo::m_roster.m_data[i]->m_status = status;
        msg->GetString(CGGuildInfo::m_roster.m_data[i]->m_name, sizeof(member->m_name));
        msg->Get(reinterpret_cast<uint32_t&>(CGGuildInfo::m_roster.m_data[i]->m_rank));
        uint8_t level;
        uint8_t classId;
        uint8_t unk;
        msg->Get(level);
        msg->Get(classId);
        msg->Get(unk);
        CGGuildInfo::m_roster.m_data[i]->m_level = level;
        CGGuildInfo::m_roster.m_data[i]->m_class = classId;
        CGGuildInfo::m_roster.m_data[i]->unk_0040 = unk;
        msg->Get(reinterpret_cast<uint32_t&>(CGGuildInfo::m_roster.m_data[i]->m_zoneID));
        member = CGGuildInfo::m_roster.m_data[i];

        if (member->m_online) {
            member->m_lastOnline = 0.0f;
            CGAutoCompleteName::AddFlags(CGGuildInfo::m_roster.m_data[i]->m_guid, 34, 0, CGGuildInfo::m_roster.m_data[i]->m_name);
        } else {
            msg->Get(member->m_lastOnline);
            CGAutoCompleteName::AddFlags(CGGuildInfo::m_roster.m_data[i]->m_guid, 2, 0, CGGuildInfo::m_roster.m_data[i]->m_name);
            CGAutoCompleteName::RemoveFlags(CGGuildInfo::m_roster.m_data[i]->m_guid, 32);
        }

        char note[128];
        msg->GetString(note, sizeof(note));
        ScrubString(note, CGGuildInfo::m_roster.m_data[i]->m_note, 32, sizeof(member->m_note), 1);
        msg->GetString(note, sizeof(note));
        ScrubString(note, CGGuildInfo::m_roster.m_data[i]->m_officerNote, 32, sizeof(member->m_officerNote), 1);

        if (CGGuildInfo::m_roster.m_data[i]->m_online) {
            CGGuildInfo::m_numOnlineMembers++;
        }
    }

    qsort(CGGuildInfo::m_roster.m_data, CGGuildInfo::m_numMembers, sizeof(GuildMemberInfo*), QSortRoster);
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, nullptr);

    if (CGGuildInfo::unk_C22AD4) {
        CGGuildInfo::SaveToFile();
    }

    return 1;
}

// VERIFY: e01e8061 auto
// OFFSET: 0x005CA060
void CGGuildInfo::SetMOTD(const char* motd) {
    ScrubString(motd, CGGuildInfo::m_motd, 129, sizeof(CGGuildInfo::m_motd), 1);
}

// VERIFY: 20231a2d auto
// OFFSET: 0x005CA0F0
void UpdateGuildRoster(int32_t a1) {
    if (a1) {
        CGGuildInfo::unk_C22AC8 = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, "%d", 1);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, nullptr);
    }
}

// VERIFY: d4f94ffa manual
// OFFSET: 0x005CA490
int32_t QSortRoster(const void* a, const void* b) {
    if (!a || !b) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    GuildMemberInfo* left = *static_cast<GuildMemberInfo* const*>(a);
    GuildMemberInfo* right = *static_cast<GuildMemberInfo* const*>(b);

    if (!CGGuildInfo::m_showOffline && (!left->m_online || !right->m_online) && left->m_online != right->m_online) {
        return 2 * (left->m_online == 0) - 1;
    }

    for (uint32_t i = 0; i < 8; i++) {
        GuildRosterSortCriteria* criteria = &CGGuildInfo::m_sortCriteria[i];
        int32_t result;

        switch (criteria->m_column) {
            case 0: {
                if (right->m_rank == left->m_rank) {
                    continue;
                }

                result = static_cast<uint32_t>(right->m_rank) < static_cast<uint32_t>(left->m_rank) ? -1 : 1;
                break;
            }

            case 1: {
                if (left->m_level == right->m_level) {
                    continue;
                }

                result = 2 * (left->m_level >= right->m_level) - 1;
                break;
            }

            case 2: {
                result = SStrCmpUTF8I(left->m_name, right->m_name, STORM_MAX_STR);
                break;
            }

            case 3: {
                AreaTableRec* leftArea = g_areaTableDB.GetRecord(left->m_zoneID);
                AreaTableRec* rightArea = g_areaTableDB.GetRecord(right->m_zoneID);

                if (!leftArea || !rightArea) {
                    continue;
                }

                result = SStrSortUTF8I(leftArea->m_areaName, rightArea->m_areaName, STORM_MAX_STR);
                break;
            }

            case 4: {
                ChrClassesRec* leftClass = g_chrClassesDB.GetRecord(left->m_class);
                ChrClassesRec* rightClass = g_chrClassesDB.GetRecord(right->m_class);

                if (!leftClass || !rightClass) {
                    continue;
                }

                result = SStrSortUTF8I(leftClass->m_name, rightClass->m_name, STORM_MAX_STR);
                break;
            }

            case 6: {
                if (left->m_online && right->m_online) {
                    continue;
                }

                if (left->m_online != right->m_online) {
                    result = 2 * (left->m_online == 0) - 1;
                    break;
                }

                result = right->m_lastOnline <= left->m_lastOnline ? 1 : -1;
                return criteria->m_descending ? -result : result;
            }

            case 7: {
                if (right->unk_0051) {
                    right->unk_0051 = 0;
                }

                if (left->unk_0051) {
                    left->unk_0051 = 0;
                }

                result = SStrSortUTF8I(left->m_note, right->m_note, STORM_MAX_STR);
                break;
            }

            default: {
                continue;
            }
        }

        if (result) {
            return criteria->m_descending ? -result : result;
        }
    }

    return 0;
}

// VERIFY: a23b0c6d manual
// OFFSET: 0x005CA9B0
void CGGuildInfo::QueryRosterAndRanks() {
    uint32_t now = OsGetAsyncTimeMs();

    if (CGGuildInfo::unk_C22AC8 && static_cast<int32_t>(now - CGGuildInfo::unk_C22AC8) < 0) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_GET_ROSTER));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGGuildInfo::unk_C22AC8 = now + 10000;
}
