#include "LootRoll.h"
#include "Engine/Source/FrameScript/FrameScript.h"

// STATIC: 0x00ADA7B4
TSExplicitList<PendingLootRoll, offsetof(PendingLootRoll, m_link)> s_pendingLootRolls;

// STATIC: 0x00CA0550
WOWGUID s_masterLootCandidates[40];

// STATIC: 0x00CA0694
uint32_t s_numShownLootRolls;

// VERIFY: 291d5e6d auto
// OFFSET: 0x006FA770
WOWGUID GetMasterLootCandidate(uint32_t index) {
    if (index >= sizeof(s_masterLootCandidates) / sizeof(s_masterLootCandidates[0])) {
        return WOWGUID();
    }

    return s_masterLootCandidates[index];
}

// VERIFY: d35c8b17 manual
// OFFSET: 0x006FB150
PendingLootRoll* FindRoll(int32_t id) {
    for (PendingLootRoll* roll = s_pendingLootRolls.Head(); roll; roll = roll->Next()) {
        if (roll->m_id == id) {
            return roll;
        }
    }

    return nullptr;
}

// VERIFY: 0229e37b manual
// OFFSET: 0x006FB180
void LootRollShowPending() {
    if (s_numShownLootRolls >= 4) {
        return;
    }

    for (PendingLootRoll* roll = s_pendingLootRolls.Head(); roll; roll = roll->Next()) {
        if (!roll->m_showPending) {
            continue;
        }

        s_numShownLootRolls++;
        roll->m_showPending = 0;
        FrameScript_SignalEvent(442, "%d%d", roll->m_id, roll->m_rollTime);

        if (s_numShownLootRolls >= 4) {
            break;
        }
    }
}
