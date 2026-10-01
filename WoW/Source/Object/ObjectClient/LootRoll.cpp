#include "LootRoll.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Net/Types.h"

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

// VERIFY: e08c1b05 manual
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
        FrameScript_SignalEvent(SCRIPT_EVENT_START_LOOT_ROLL, "%d%d", roll->m_id, roll->m_rollTime);

        if (s_numShownLootRolls >= 4) {
            break;
        }
    }
}

// VERIFY: 4f4e9712 manual
// OFFSET: 0x006FBC70
void LootRoll(int32_t id, int32_t rollType, bool confirmed) {
    for (PendingLootRoll* roll = s_pendingLootRolls.Head(); roll; roll = s_pendingLootRolls.Next(roll)) {
        if (roll->m_id != id) {
            continue;
        }

        if (roll->unk_0042) {
            return;
        }

        if (rollType && !confirmed) {
            WOWGUID none = WOWGUID();
            ItemStats_C* stats = g_itemDBCache.GetRecord(roll->m_itemId, none, nullptr, nullptr, false);

            if (stats) {
                if (rollType == 3) {
                    if (stats->m_quality >= 3) {
                        FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_DISENCHANT_ROLL, "%d%d", roll->m_id, 3);
                        return;
                    }
                } else if (stats->m_bonding == 1) {
                    FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_LOOT_ROLL, "%d%d", roll->m_id, rollType);
                    return;
                }
            }
        }

        if ((1 << rollType) & roll->m_voteMask) {
            WDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_LOOT_ROLL));
            msg.Put(roll->m_lootGuid.m_guid);
            msg.Put(roll->m_slot);
            msg.Put(static_cast<uint8_t>(rollType));
            msg.Finalize();
            ClientServices::Send2(&msg);
            roll->unk_0030 = 1;
            FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_LOOT_ROLL, "%d", roll->m_id);
            s_numShownLootRolls--;
            roll->unk_0042 = 1;
            LootRollShowPending();
        }

        return;
    }
}

// VERIFY: f5032c00 manual
// OFFSET: 0x006FAF40
void LootRollDestroy() {
    PendingLootRoll* roll;

    while ((roll = s_pendingLootRolls.Head())) {
        s_pendingLootRolls.DeleteNode(roll);
    }
}

// VERIFY: ab7c1274 auto
// OFFSET: 0x006FAFB0
void LootRollShutdown() {
    LootRollDestroy();
    s_numShownLootRolls = 0;
}

// VERIFY: 59202aa4 manual
// OFFSET: 0x006FAFD0
void CloseAllLootRolls() {
    PendingLootRoll* roll = s_pendingLootRolls.Head();

    while (roll) {
        PendingLootRoll* next = s_pendingLootRolls.Next(roll);

        if (!roll->unk_0042) {
            WDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_LOOT_ROLL));
            msg.Put(roll->m_lootGuid.m_guid);
            msg.Put(roll->m_slot);
            msg.Put(static_cast<uint8_t>(0));
            msg.Finalize();
            ClientServices::Send2(&msg);

            if (!roll->m_showPending) {
                FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_LOOT_ROLL, "%d", roll->m_id);
            }
        }

        roll = next;
    }

    LootRollDestroy();
    s_numShownLootRolls = 0;
}
