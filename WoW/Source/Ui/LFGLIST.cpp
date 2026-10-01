#include "LFGLIST.h"
#include <typeinfo>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"

// VERIFY: fe00a81b manual
// OFFSET: 0x00558AF0
void LFGLIST::Clear() {
    LFGListPlayer* player = this->m_players.m_fulllist.Head();

    while (player) {
        LFGListPlayer* next = this->m_players.m_fulllist.Next(player);
        g_nameDBCache.CancelCallback(player->m_key.m_guid.m_guid, LFGNameCacheCallback, nullptr);
        delete player;
        player = next;
    }

    this->unk_007C = 0;
    this->m_players.Clear();

    LFGListParty* party = this->m_parties.m_fulllist.Head();

    while (party) {
        LFGListParty* next = this->m_parties.m_fulllist.Next(party);

        if (party->m_members) {
            SMemFree(party->m_members, typeid(LFGListPlayer*).raw_name(), SLOG_OBJECT, 0);
        }

        delete party;
        party = next;
    }

    this->unk_006C = 0;
    this->unk_0000 = 0;
    this->unk_0004 = 0;
    this->unk_0008 = 0;
    this->unk_000C = 0;
    this->unk_0010 = 0;
    this->unk_0014 = 0;
}
