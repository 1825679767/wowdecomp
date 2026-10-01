#include "FriendList.h"
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <new>

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
