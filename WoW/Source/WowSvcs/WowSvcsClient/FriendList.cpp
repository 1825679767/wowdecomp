#include "FriendList.h"

// STATIC: 0x00C79F98
FriendList* FriendList::s_friendList;

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
