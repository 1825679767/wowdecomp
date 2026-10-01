#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_LOOTROLL_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_LOOTROLL_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

struct PendingLootRoll : public TSLinkedNode<PendingLootRoll> {
    /* 0008 */ int32_t m_id;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ WOWGUID m_lootGuid;
    /* 0018 */ uint32_t m_slot;
    /* 001C */ uint32_t m_itemId;
    /* 0020 */ uint32_t m_randomPropertyId;
    /* 0024 */ uint32_t m_randomSuffix;
    /* 0028 */ uint32_t m_count;
    /* 002C */ uint32_t m_expireTime;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034 [3];
    /* 0040 */ uint8_t unk_0040;
    /* 0041 */ uint8_t m_showPending;
    /* 0042 */ uint8_t unk_0042;
    /* 0043 */ uint8_t unk_0043;
    /* 0044 */ uint32_t m_rollTime;
    /* 0048 */ uint32_t m_voteMask;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint8_t unk_0050 [2];
    /* 0052 */ uint8_t m_reasonNeed;
    /* 0053 */ uint8_t m_reasonGreed;
    /* 0054 */ uint8_t m_reasonDisenchant;
    /* 0055 */ uint8_t unk_0055 [3];
};

extern TSExplicitList<PendingLootRoll, offsetof(PendingLootRoll, m_link)> s_pendingLootRolls;
extern WOWGUID s_masterLootCandidates[40];
extern uint32_t s_numShownLootRolls;

PendingLootRoll* FindRoll(int32_t id);
WOWGUID GetMasterLootCandidate(uint32_t index);
void LootRollShowPending();
void LootRoll(int32_t id, int32_t rollType, bool confirmed);

void CloseAllLootRolls();
void LootRollDestroy();
void LootRollShutdown();

#endif
