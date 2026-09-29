#ifndef WOW_SOURCE_UI_RAIDINFO_H
#define WOW_SOURCE_UI_RAIDINFO_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Ui/PartyFrame.h"

class CGPlayer_C;

class CGRaidMember {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t unk_0008[2];
    /* 0010 */ CGPartyMemberState m_stats;
    /* 0320 */ uint32_t unk_0320[4];
};

class CGRaidInfo {
    public:
    static CGRaidMember* m_members[40];
    static uint32_t m_numMembers;

    static void EnableMember(CGPlayer_C* player, int32_t enable);
    static int32_t IsPetInRaid(const WOWGUID& guid, int32_t* index);
};

#endif
