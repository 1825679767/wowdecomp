#ifndef WOW_SOURCE_UI_LFGLIST_H
#define WOW_SOURCE_UI_LFGLIST_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

struct LFGListPlayer : public TSHashObject<LFGListPlayer, CHashKeyGUID> {
};

struct LFGListParty : public TSHashObject<LFGListParty, CHashKeyGUID> {
    /* 0020 */ uint8_t unk_0020[280];
    /* 0138 */ uint32_t unk_0138;
    /* 013C */ LFGListPlayer* m_members;
    /* 0140 */ uint32_t unk_0140;
};

class LFGLIST {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011[3];
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ TSHashTable<LFGListParty, CHashKeyGUID> m_parties;
    /* 0040 */ TSHashTable<LFGListPlayer, CHashKeyGUID> m_players;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint8_t unk_0070[12];
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint32_t unk_0080;
    /* 0084 */ uint32_t unk_0084;

    void Clear();
};

void LFGNameCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success);

#endif
