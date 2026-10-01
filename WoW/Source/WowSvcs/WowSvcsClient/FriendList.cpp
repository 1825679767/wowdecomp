#include "FriendList.h"
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <new>
#include <cstdlib>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Ui/Calendar.h"
#include "WoW/Source/Ui/CGAutoCompleteName.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00C79F98
FriendList* FriendList::s_friendList;

// STATIC: 0x00C79F9C
WhoSortField FriendList::s_whoSort[7];

// STATIC: 0x00C7BFE0
uint32_t FriendList::s_numWhoResults;

// STATIC: 0x00C7BFE4
uint32_t FriendList::s_totalWhoResults;

// VERIFY: a4507187 manual
// OFFSET: 0x006B3510
FriendInfo* FriendList::GetFriend(WOWGUID guid) {
    if (!guid) {
        return nullptr;
    }

    for (uint32_t i = 0; i < 100; i++) {
        if (this->m_friends[i].m_guid == guid) {
            return &this->m_friends[i];
        }
    }

    return nullptr;
}

// VERIFY: 5305c965 auto
// OFFSET: 0x006B34A0
uint32_t FriendList::GetNumFriends() {
    uint32_t count = 0;

    while (count < 100 && this->m_friends[count].m_guid) {
        count++;
    }

    return count;
}

// INLINED
FriendInfo::FriendInfo() {
    this->m_connected = 0;
    this->m_name = nullptr;
    this->m_notes[0] = '\0';
    this->m_guid = WOWGUID();
}

// INLINED
FriendInfo::~FriendInfo() {
    if (this->m_name) {
        SMemFree(this->m_name, __FILE__, __LINE__, 0);
    }
}

// VERIFY: a8b220e2 manual
// OFFSET: 0x006B3190
FriendList::FriendList() {
    memset(this->m_friends, 0, sizeof(this->m_friends));
    memset(this->m_ignores, 0, sizeof(this->m_ignores));
    memset(this->m_mutes, 0, sizeof(this->m_mutes));
    memset(this->unk_DAF0, 0, sizeof(this->unk_DAF0));
    this->m_pendingFriendNames = 0;
    this->unk_D488 = 0;
    this->unk_D48C = 0;
    this->m_pendingIgnoreNames = 0;
    this->unk_D7B8 = 0;
    this->unk_D7BC = 0;
    this->m_pendingMuteNames = 0;
    this->unk_DAE8 = 0;
    this->unk_DAEC = 0;
}

// VERIFY: 2f86035c manual
// OFFSET: 0x006B3240
FriendList::~FriendList() {
}

// VERIFY: c0f3513f manual
// OFFSET: 0x006B8990
void FriendList::Initialize() {
    if (FriendList::s_friendList) {
        return;
    }

    void* mem = SMemAlloc(sizeof(FriendList), __FILE__, __LINE__, 0);
    FriendList::s_friendList = mem ? new (mem) FriendList() : nullptr;

    ClientServices::SetMessageHandler(SMSG_WHO, Packet_SMSG_WHO, nullptr);
    ClientServices::SetMessageHandler(SMSG_WHO_IS, Packet_SMSG_WHOIS, nullptr);
    ClientServices::SetMessageHandler(SMSG_RWHOIS, Packet_SMSG_RWHOIS, nullptr);
    ClientServices::SetMessageHandler(SMSG_CONTACT_LIST, Packet_SMSG_CONTACT_LIST, nullptr);
    ClientServices::SetMessageHandler(SMSG_FRIEND_STATUS, Packet_SMSG_FRIEND_STATUS, nullptr);
    ConsoleCommandRegister("whois", CCommand_Whois, CATEGORY_DEBUG, "Ask the server to do an account/real name lookup on a character name");

    for (int32_t i = 0; i < 7; i++) {
        FriendList::s_whoSort[i].m_field = i;
        FriendList::s_whoSort[i].unk_0004 = 0;
    }

    FriendList::s_numWhoResults = 0;
    FriendList::s_totalWhoResults = 0;
}

// VERIFY: 7186f5af manual
// OFFSET: 0x006B3F90
FriendInfo* FriendList::ClearRecruitAFriend(WOWGUID guid) {
    for (uint32_t i = 0; i < 100; i++) {
        if (this->m_friends[i].m_guid == guid) {
            this->m_friends[i].m_flags &= ~0x8;
            return &this->m_friends[i];
        }
    }

    return nullptr;
}

// VERIFY: 45c91620 manual
// OFFSET: 0x006B7DD0
void FriendList::ReadPacket(CDataStore* msg) {
    uint32_t friendIndex = 0;
    uint32_t flags;
    uint32_t count;
    msg->Get(flags);
    msg->Get(count);

    if (flags & 0x1) {
        for (uint32_t i = 0; i < 100; i++) {
            if (this->m_friends[i].m_name) {
                SMemFree(this->m_friends[i].m_name, __FILE__, __LINE__, 0);
            }

            this->m_friends[i].m_name = nullptr;
        }
    }

    if (flags & 0x2) {
        memset(this->m_ignores, 0, sizeof(this->m_ignores));
    }

    if (flags & 0x4) {
        memset(this->m_mutes, 0, sizeof(this->m_mutes));
    }

    for (uint32_t n = 0; n < count; n++) {
        WOWGUID guid;
        uint32_t entryFlags;
        char note[512];
        msg->Get(guid.m_guid);
        msg->Get(entryFlags);
        msg->GetString(note, sizeof(note));

        if (entryFlags & 0x1) {
            uint32_t area = 0;
            uint32_t level = 0;
            uint32_t classId = 0;
            uint8_t status;
            msg->Get(status);

            if (status) {
                msg->Get(area);
                msg->Get(level);
                msg->Get(classId);
            }

            if ((flags & 0x1) && friendIndex < 100) {
                FriendInfo& info = this->m_friends[friendIndex];
                info.m_flags = status;
                info.m_guid = guid;
                info.m_area = area;
                info.m_level = level;
                info.m_connected = status != 0;
                info.unk_021C = 0;
                info.m_class = classId;
                SStrCopy(info.m_notes, note, sizeof(info.m_notes));

                NameCache* record = g_nameDBCache.GetRecord(info.m_guid.m_guid, info.m_guid, FriendListNameCallbackWithSort, reinterpret_cast<void*>(ERR_MAX_VALUE), false);

                if (record) {
                    char name[305];

                    if (!record->m_realmName[0]) {
                        SStrCopy(name, record->m_name, sizeof(name));
                    } else {
                        SStrPrintf(name, sizeof(name), "%s-%s", record->m_name, record->m_realmName);
                    }

                    info.m_name = SStrDupA(name, __FILE__, __LINE__);

                    if (info.m_connected) {
                        CGAutoCompleteName::AddFlags(guid, 0x24, 0, nullptr);
                    } else {
                        CGAutoCompleteName::AddFlags(guid, 0x4, 0, nullptr);
                        CGAutoCompleteName::RemoveFlags(guid, 0x20);
                    }
                } else {
                    if (info.m_connected) {
                        CGAutoCompleteName::AddFlags(guid, 0x24, 0, nullptr);
                    } else {
                        CGAutoCompleteName::AddFlags(guid, 0x4, 0, nullptr);
                        CGAutoCompleteName::RemoveFlags(guid, 0x20);
                    }

                    this->m_pendingFriendNames++;
                }

                friendIndex++;
            }
        }

        if (entryFlags & flags & 0x2) {
            WOWGUID ignoreGuid = guid;

            for (uint32_t i = 0; i < 50; i++) {
                if (!this->m_ignores[i].m_guid.m_guid) {
                    this->m_ignores[i].m_guid = ignoreGuid;
                    this->m_ignores[i].m_flags = 0;
                    CGCalendar::UpdateDisplayList(1);
                    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, nullptr);
                    break;
                }
            }

            VoiceSessionSetTalkerMuted(ignoreGuid, 0);

            if (!g_nameDBCache.GetRecord(guid.m_guid, guid, FriendListIgnoreNameCallback, reinterpret_cast<void*>(ERR_MAX_VALUE), false)) {
                this->m_pendingIgnoreNames++;
            }
        }

        if (entryFlags & flags & 0x4) {
            WOWGUID muteGuid = guid;

            for (uint32_t i = 0; i < 50; i++) {
                if (!this->m_mutes[i].m_guid.m_guid) {
                    this->m_mutes[i].m_guid = muteGuid;
                    this->m_mutes[i].m_flags = 0;
                    break;
                }
            }

            VoiceSessionSetTalkerMuted(guid, 0);

            if (!g_nameDBCache.GetRecord(guid.m_guid, guid, FriendListMuteNameCallback, reinterpret_cast<void*>(ERR_MAX_VALUE), false)) {
                this->m_pendingMuteNames++;
            }
        }
    }

    if ((flags & 0x1) && !this->m_pendingFriendNames) {
        qsort(this->m_friends, 100, sizeof(FriendInfo), QSortFriends);
        FrameScript_SignalEvent(SCRIPT_EVENT_FRIENDLIST_UPDATE, nullptr);
    }

    if ((flags & 0x2) && !this->m_pendingIgnoreNames) {
        qsort(this->m_ignores, 50, sizeof(IgnoreInfo), QSortIgnore);
        FrameScript_SignalEvent(SCRIPT_EVENT_IGNORELIST_UPDATE, nullptr);
    }

    if ((flags & 0x4) && !this->m_pendingMuteNames) {
        qsort(this->m_mutes, 50, sizeof(IgnoreInfo), QSortIgnore);
        FrameScript_SignalEvent(SCRIPT_EVENT_MUTELIST_UPDATE, nullptr);
    }

    if (!msg->IsRead()) {
        uint8_t key[16];
        msg->GetArray(key, sizeof(key));

        if (msg->m_read <= msg->m_size) {
            LoadEncryptedModule(key, sizeof(key), FriendListModuleLoaded);
        }
    }
}

// VERIFY: 5a67a606 auto
// OFFSET: 0x006B8700
int32_t Packet_SMSG_CONTACT_LIST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    FriendList::s_friendList->ReadPacket(msg);
    return 1;
}

// VERIFY: 40fabbbe auto
// OFFSET: 0x006B3630
uint32_t FriendList::GetNumIgnores() {
    uint32_t count = 0;

    while (count < 50 && this->m_ignores[count].m_guid) {
        count++;
    }

    return count;
}

// VERIFY: dfd1e135 manual
// OFFSET: 0x006B3960
int32_t QSortFriends(const void* a, const void* b) {
    if (!a || !b) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    const FriendInfo* friendA = static_cast<const FriendInfo*>(a);
    const FriendInfo* friendB = static_cast<const FriendInfo*>(b);

    if (!friendA->m_guid && !friendB->m_guid) {
        return 0;
    }

    if (!friendA->m_guid) {
        return 1;
    }

    if (!friendB->m_guid) {
        return -1;
    }

    if (friendA->m_connected != friendB->m_connected) {
        return friendA->m_connected ? -1 : 1;
    }

    if (friendA->m_name && friendB->m_name) {
        return SStrSortUTF8I(friendA->m_name, friendB->m_name, 0x7FFFFFFF);
    }

    if (!friendA->m_name && !friendB->m_name) {
        return 0;
    }

    return friendA->m_name ? -1 : 1;
}

// VERIFY: 5c7fff6f manual
// OFFSET: 0x006B3A60
int32_t QSortIgnore(const void* a, const void* b) {
    if (!a || !b) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    WOWGUID guidA = static_cast<const IgnoreInfo*>(a)->m_guid;
    WOWGUID guidB = static_cast<const IgnoreInfo*>(b)->m_guid;

    if (!guidA) {
        return guidB ? 1 : 0;
    }

    if (!guidB) {
        return -1;
    }

    NameCache* nameA = g_nameDBCache.GetRecord(guidA.m_guid, WOWGUID(), nullptr, nullptr, false);
    NameCache* nameB = g_nameDBCache.GetRecord(guidB.m_guid, WOWGUID(), nullptr, nullptr, false);

    if (!nameA) {
        return nameB ? 1 : 0;
    }

    if (!nameB) {
        return -1;
    }

    char fullNameA[305];
    char fullNameB[305];
    return SStrCmp(nameA->FullName(fullNameA, sizeof(fullNameA)), nameB->FullName(fullNameB, sizeof(fullNameB)), 0x7FFFFFFF);
}

// VERIFY: 851ea1b0 manual
// OFFSET: 0x006B51B0
void FriendList::Destroy() {
    if (!FriendList::s_friendList) {
        return;
    }

    ClientServices::ClearMessageHandler(SMSG_WHO);
    ClientServices::ClearMessageHandler(SMSG_WHO_IS);
    ClientServices::ClearMessageHandler(SMSG_RWHOIS);
    ClientServices::ClearMessageHandler(SMSG_CONTACT_LIST);
    ClientServices::ClearMessageHandler(SMSG_FRIEND_STATUS);
    ConsoleCommandUnregister("whois");
    delete FriendList::s_friendList;
    FriendList::s_friendList = nullptr;
}
