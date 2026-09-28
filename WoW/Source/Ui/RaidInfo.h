#ifndef WOW_SOURCE_UI_RAIDINFO_H
#define WOW_SOURCE_UI_RAIDINFO_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGRaidMember {
    public:
    /* 0000 */ WGUID m_guid;
    /* 0008 */ uint8_t unk_0008[360];
    /* 0170 */ WGUID m_petGuid;
};

class CGRaidInfo {
    public:
    static CGRaidMember* m_members[40];
    static uint32_t m_numMembers;
};

#endif
