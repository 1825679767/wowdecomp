#ifndef WOW_SOURCE_UI_PARTYFRAME_H
#define WOW_SOURCE_UI_PARTYFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGPlayer_C;

class CGPartyPetState {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint8_t unk_0008[416];
};

class CGPartyMemberState {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint16_t m_flags;
    /* 000A */ uint8_t unk_000A[342];
    /* 0160 */ CGPartyPetState m_pet;
    /* 0308 */ uint32_t unk_0308[2];
};

class CGPartyInfo {
    public:
    static WOWGUID m_members[4];

    static uint32_t GetNumPartyMembers();
    static void EnableMember(CGPlayer_C* player, int32_t enable);
    static int32_t IsMemberPet(const WOWGUID& guid, int32_t* index);
};

extern CGPartyMemberState unk_BD0D08[4];

WOWGUID GetPartyMemberGUID(uint32_t index);
WOWGUID GetPartyPetGUID(uint32_t index);

#endif
