#ifndef WOW_SOURCE_UI_PARTYFRAME_H
#define WOW_SOURCE_UI_PARTYFRAME_H

#include <cstddef>
#include <cstdint>

class CGPartyInfo {
    public:
    static uint64_t m_members[4];

    static uint32_t GetNumPartyMembers();
};

uint64_t GetPartyMemberGUID(uint32_t index);
uint64_t GetPartyPetGUID(uint32_t index);

#endif
