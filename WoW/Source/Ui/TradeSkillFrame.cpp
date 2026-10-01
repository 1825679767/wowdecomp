#include "TradeSkillFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include <cstdlib>
#include <malloc.h>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00A1B688
static const uint8_t s_base64Decode[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0,
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0,
};

// STATIC: 0x00AD0E00
uint32_t CGTradeSkillInfo::m_subClassFilter = 0xFFFFFFFF;

// STATIC: 0x00AD0E08
uint32_t CGTradeSkillInfo::m_invTypeFilter = 0xFFFFFFFF;

// STATIC: 0x00AD0E0C
uint32_t CGTradeSkillInfo::m_collapseFilter = 0xFFFFFFFF;

// STATIC: 0x00C23598
WOWGUID CGTradeSkillInfo::m_playerGuid;

// STATIC: 0x00C235AC
uint32_t CGTradeSkillInfo::m_itemsPending;

// STATIC: 0x00C235B0
uint32_t CGTradeSkillInfo::m_numDisplayEntries;

// STATIC: 0x00C235B4
uint32_t CGTradeSkillInfo::m_numSubClasses;

// STATIC: 0x00C235C8
int32_t CGTradeSkillInfo::m_showOnlyMakeable;

// STATIC: 0x00C235CC
int32_t CGTradeSkillInfo::m_showOnlySkillUps;

// STATIC: 0x00C235D0
uint32_t CGTradeSkillInfo::m_invSlots;

// STATIC: 0x00C235E8
int32_t CGTradeSkillInfo::m_skillRank;

// STATIC: 0x00C235EC
int32_t CGTradeSkillInfo::m_skillMaxRank;

// STATIC: 0x00C235F0
int32_t CGTradeSkillInfo::unk_C235F0;

// STATIC: 0x00C235F4
TSGrowableArray<TradeSkillInfo*> CGTradeSkillInfo::m_displayList;

// STATIC: 0x00C23604
TSGrowableArray<TradeSkillSubClassInfo*> CGTradeSkillInfo::m_subClassList;

// STATIC: 0x00C235B8
int32_t CGTradeSkillInfo::m_filteredSkills;

// STATIC: 0x00C235BC
char* CGTradeSkillInfo::m_itemNameFilter;

// STATIC: 0x00C235D4
int32_t CGTradeSkillInfo::m_recastSpellID;

// STATIC: 0x00C235DC
int32_t CGTradeSkillInfo::m_pendingRecastSpellID;

// STATIC: 0x00C235C0
int32_t CGTradeSkillInfo::m_itemMinLevelFilter;

// STATIC: 0x00C235C4
int32_t CGTradeSkillInfo::m_itemMaxLevelFilter;

// STATIC: 0x00C235A0
int32_t CGTradeSkillInfo::m_spellID;

// STATIC: 0x00C235A4
int32_t CGTradeSkillInfo::m_skillLineID;
// STATIC: 0x00C235A8
int32_t CGTradeSkillInfo::m_selectedSkill;

// STATIC: 0x00C235D8
uint32_t CGTradeSkillInfo::m_recastTimes;

// STATIC: 0x00C235E0
uint32_t CGTradeSkillInfo::m_pendingRecastTimes;

// STATIC: 0x00C235E4
void* CGTradeSkillInfo::m_linkData;

// VERIFY: c0b1eab0 manual
// OFFSET: 0x005D9FF0
void CGTradeSkillInfo::StopRecast() {
    if (CGTradeSkillInfo::m_recastSpellID || CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
    }
}

// VERIFY: da469b43 auto
// OFFSET: 0x005DA020
void UpdateTradeSkillRecast(int32_t spellId) {
    if (spellId == CGTradeSkillInfo::m_recastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
    }

    if (spellId == CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
    }
}

// VERIFY: e58a5a53 manual
// OFFSET: 0x005DA070
void CGTradeSkillInfo::DoRecast(int32_t spellId) {
    if (!CGTradeSkillInfo::m_recastSpellID) {
        if (!CGTradeSkillInfo::m_pendingRecastSpellID) {
            return;
        }

        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
        return;
    }

    if (!CGTradeSkillInfo::m_recastTimes || CGTradeSkillInfo::m_recastSpellID != spellId) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
        return;
    }

    if (--CGTradeSkillInfo::m_recastTimes) {
        Spell_C::CastSpell(spellId, nullptr, WOWGUID{ 0 }, false);
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
    } else {
        CGTradeSkillInfo::m_recastSpellID = 0;
    }
}

// VERIFY: 22405cf4 auto
// OFFSET: 0x005DA0F0
void CGTradeSkillInfo::CommitPendingRecast(int32_t spellId) {
    if (spellId == CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = CGTradeSkillInfo::m_pendingRecastSpellID;
        CGTradeSkillInfo::m_recastTimes = CGTradeSkillInfo::m_pendingRecastTimes;
    }

    CGTradeSkillInfo::m_pendingRecastTimes = 0;
    CGTradeSkillInfo::m_pendingRecastSpellID = 0;
}

// VERIFY: b4e8cb94 auto
// OFFSET: 0x005D9F80
static void TradeSkillListItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (CGTradeSkillInfo::m_itemsPending && !--CGTradeSkillInfo::m_itemsPending) {
        CGTradeSkillInfo::RefreshList(1);
    }
}

// VERIFY: 5bd87f16 manual
// OFFSET: 0x005DA7D0
int32_t CGTradeSkillInfo::GetSubClassIndexFromSkill(uint32_t index) {
    if (index >= CGTradeSkillInfo::m_numDisplayEntries) {
        return -1;
    }

    TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[index];

    if (info->m_spellId >= 0) {
        return -1;
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
        TradeSkillSubClassInfo* subClass = CGTradeSkillInfo::m_subClassList.m_data[i];

        if (subClass->m_itemClass == info->m_itemClass && subClass->m_itemSubClass == info->m_itemSubClass) {
            return i;
        }
    }

    return -1;
}

// VERIFY: f9844be1 manual
// OFFSET: 0x005DAD10
void CGTradeSkillInfo::ClearItemCallbacks() {
    if (!CGTradeSkillInfo::m_itemsPending) {
        return;
    }

    SpellRec spell;

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_numDisplayEntries; i++) {
        if (g_spellDB.GetRecord(CGTradeSkillInfo::m_displayList.m_data[i]->m_spellId, &spell)) {
            g_itemDBCache.CancelCallback(spell.m_effectItemType[0], TradeSkillListItemCallback, nullptr);
        }
    }

    CGTradeSkillInfo::m_itemsPending = 0;
}

// VERIFY: 4ce9d9f6 manual
// OFFSET: 0x005DCC40
void CGTradeSkillInfo::SetInvTypeFilter(int32_t a1) {
    CGTradeSkillInfo::m_filteredSkills = CGTradeSkillInfo::m_numDisplayEntries;

    if (a1) {
        for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
            CGTradeSkillInfo::m_subClassList.m_data[i]->m_shown = (CGTradeSkillInfo::m_subClassFilter & (1 << i)) != 0;
            CGTradeSkillInfo::m_subClassList.m_data[i]->m_collapsed = (CGTradeSkillInfo::m_collapseFilter & (1 << i)) == 0;
        }
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_numDisplayEntries; i++) {
        TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[i];
        info->m_visible = 1;

        if (info->m_spellId < 0) {
            continue;
        }

        if (CGTradeSkillInfo::m_numSubClasses) {
            uint32_t index = 0;

            for (uint32_t j = 0; j < CGTradeSkillInfo::m_numSubClasses; j++) {
                TradeSkillSubClassInfo* subClass = CGTradeSkillInfo::m_subClassList.m_data[j];

                if (info->m_itemClass == subClass->m_itemClass && info->m_itemSubClass == subClass->m_itemSubClass) {
                    index = j;
                    break;
                }
            }

            if (!CGTradeSkillInfo::m_subClassList.m_data[index]->m_shown) {
                info->m_visible = 0;
                CGTradeSkillInfo::m_filteredSkills--;
                continue;
            }
        }

        if (!(CGTradeSkillInfo::m_invTypeFilter & info->m_invTypeMask) || (CGTradeSkillInfo::m_showOnlyMakeable && !info->m_numAvailable) || (CGTradeSkillInfo::m_showOnlySkillUps && info->m_difficulty > 2)) {
            info->m_visible = 0;
            CGTradeSkillInfo::m_filteredSkills--;
            continue;
        }

        if ((CGTradeSkillInfo::m_itemNameFilter || CGTradeSkillInfo::m_itemMinLevelFilter || CGTradeSkillInfo::m_itemMaxLevelFilter) && !Profession_FilterSpell(info->m_spellId, CGTradeSkillInfo::m_itemMinLevelFilter, CGTradeSkillInfo::m_itemMaxLevelFilter, CGTradeSkillInfo::m_itemNameFilter)) {
            info->m_visible = 0;
            CGTradeSkillInfo::m_filteredSkills--;
        }
    }

    if (CGTradeSkillInfo::m_numSubClasses) {
        for (uint32_t i = 0; i < CGTradeSkillInfo::m_numDisplayEntries; i++) {
            TradeSkillInfo* header = CGTradeSkillInfo::m_displayList.m_data[i];

            if (header->m_spellId >= 0) {
                continue;
            }

            int32_t subClass = CGTradeSkillInfo::GetSubClassIndexFromSkill(i);
            bool any = false;

            for (uint32_t j = 0; j < CGTradeSkillInfo::m_numDisplayEntries; j++) {
                if (i == j) {
                    continue;
                }

                TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[j];

                if (header->m_itemClass == info->m_itemClass && header->m_itemSubClass == info->m_itemSubClass && info->m_visible) {
                    any = true;

                    if (CGTradeSkillInfo::m_subClassList.m_data[subClass]->m_collapsed) {
                        info->m_visible = 0;
                        CGTradeSkillInfo::m_filteredSkills--;
                    }
                }
            }

            if (!any) {
                header->m_visible = 0;
                CGTradeSkillInfo::m_filteredSkills--;
            }
        }
    }

    qsort(CGTradeSkillInfo::m_subClassList.m_data, CGTradeSkillInfo::m_numSubClasses, sizeof(TradeSkillSubClassInfo*), QSortSubClasses);
    qsort(CGTradeSkillInfo::m_displayList.m_data, CGTradeSkillInfo::m_numDisplayEntries, sizeof(TradeSkillInfo*), QSortSkills);
}

// VERIFY: 3f9ff382 manual
// OFFSET: 0x005DD5A0
void CGTradeSkillInfo::RefreshList(int32_t full) {
    CGTradeSkillInfo::ClearItemCallbacks();
    CGTradeSkillInfo::m_numDisplayEntries = 0;

    uint32_t savedCount = CGTradeSkillInfo::m_numSubClasses;
    uint32_t saveSize = CGTradeSkillInfo::m_subClassList.Count();

    if (CGTradeSkillInfo::m_numSubClasses < saveSize) {
        saveSize = CGTradeSkillInfo::m_numSubClasses;
    }

    TradeSkillSubClassInfo* saved = static_cast<TradeSkillSubClassInfo*>(alloca(sizeof(TradeSkillSubClassInfo) * saveSize));

    if (full) {
        for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
            saved[i].m_itemClass = CGTradeSkillInfo::m_subClassList.m_data[i]->m_itemClass;
            saved[i].m_itemSubClass = CGTradeSkillInfo::m_subClassList.m_data[i]->m_itemSubClass;
            saved[i].m_shown = CGTradeSkillInfo::m_subClassList.m_data[i]->m_shown;
            saved[i].m_collapsed = CGTradeSkillInfo::m_subClassList.m_data[i]->m_collapsed;
        }

        CGTradeSkillInfo::m_numSubClasses = 0;
        CGTradeSkillInfo::m_subClassFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_collapseFilter = 0xFFFFFFFF;
    }

    if (!CGTradeSkillInfo::m_skillLineID) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    WOWGUID none = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(CGTradeSkillInfo::m_playerGuid.m_guid, none, nullptr, nullptr, false);

    if (!name) {
        return;
    }

    TSGrowableArray<int32_t>* spells = nullptr;
    TSGrowableArray<const SkillLineAbilityRec*> abilities;
    uint32_t count = 0;

    if (CGTradeSkillInfo::m_linkData) {
        const SkillLineAbilityRec* ability = nullptr;
        uint32_t linkCount = 0;

        if (!SpellTableGetSkillLineLinkInfo(CGTradeSkillInfo::m_skillLineID, ability, linkCount)) {
            return;
        }

        int32_t offset = 0;
        uint8_t bit = 0;

        for (uint32_t i = 0; i < linkCount; i++) {
            if ((1 << bit) & s_base64Decode[static_cast<const uint8_t*>(CGTradeSkillInfo::m_linkData)[offset]]) {
                SpellRec spell;

                if (g_spellDB.GetRecord(ability->m_spell, &spell) && (spell.m_attributes & 0x20)) {
                    if (count == abilities.Count()) {
                        abilities.SetCount(count + 32);
                    }

                    abilities.m_data[count] = ability;
                    count++;
                }
            }

            if (++bit >= 6) {
                bit = 0;
                offset++;
            }

            ability++;
        }
    } else {
        if (!player) {
            return;
        }

        spells = player->GetTradeSkillSpells(CGTradeSkillInfo::m_skillLineID);

        if (!spells) {
            return;
        }

        count = spells->Count();
    }

    for (uint32_t i = CGTradeSkillInfo::m_displayList.Count(); i < count; i++) {
        TradeSkillInfo* info = static_cast<TradeSkillInfo*>(SMemAlloc(sizeof(TradeSkillInfo), __FILE__, __LINE__, 0));
        *CGTradeSkillInfo::m_displayList.New() = info;
    }

    CGTradeSkillInfo::m_numDisplayEntries = count;

    CGPlayer_C* activePlayer = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t skillRank;

    if (activePlayer && CGTradeSkillInfo::m_playerGuid == activePlayer->m_obj->m_guid) {
        skillRank = activePlayer->m_player->m_playerSkillInfo[activePlayer->GetSkillIndex(CGTradeSkillInfo::m_skillLineID)].m_rank;
    } else {
        skillRank = CGTradeSkillInfo::m_skillRank;
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_numDisplayEntries; i++) {
        int32_t spellId;
        const SkillLineAbilityRec* ability;

        if (spells) {
            spellId = spells->m_data[i];
            ability = SpellTableLookupAbility(name->m_race, name->m_class, spellId);
        } else {
            ability = abilities.m_data[i];
            spellId = ability->m_spell;
        }

        TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[i];
        info->m_spellId = spellId;
        info->m_difficulty = 0;
        info->m_skillLineRank = ability->m_trivialSkillLineRankHigh;

        int32_t high = ability->m_trivialSkillLineRankHigh;
        int32_t low = ability->m_trivialSkillLineRankLow;

        if (!low) {
            low = high != 25 ? high - 25 : 0;
        }

        if (skillRank < low) {
            info->m_difficulty = 0;
        } else if (skillRank < (high + low) / 2) {
            info->m_difficulty = 1;
        } else if (skillRank < high) {
            info->m_difficulty = 2;
        } else {
            info->m_difficulty = 3;
        }

        int32_t numAvailable = -1;
        SpellRec spell;

        if (g_spellDB.GetRecord(spellId, &spell)) {
            for (uint32_t m = 0; m < 8 && numAvailable; m++) {
                if (!spell.m_reagent[m] || !spell.m_reagentCount[m]) {
                    continue;
                }

                int32_t available = player ? player->m_bag.GetItemTypeCount(spell.m_reagent[m], 0) / spell.m_reagentCount[m] : 0;

                if (numAvailable == -1 || numAvailable >= available) {
                    numAvailable = available;
                }
            }

            if ((spell.m_effect[0] == 24 || spell.m_effect[0] == 157 || spell.m_effect[0] == 59) && spell.m_effectItemType[0]) {
                WOWGUID requester;
                requester.m_guid = static_cast<uint32_t>(spellId) | 0x1FE0000000000000ULL;
                ItemStats_C* stats = g_itemDBCache.GetRecord(spell.m_effectItemType[0], requester, TradeSkillListItemCallback, nullptr, false);

                if (!stats) {
                    CGTradeSkillInfo::m_itemsPending++;
                }

                if (CGTradeSkillInfo::m_itemsPending) {
                    info->m_numAvailable = numAvailable;
                    continue;
                }

                info->m_itemClass = stats->m_class;
                info->m_itemSubClass = stats->m_subClass;
                info->m_itemLevel = stats->m_itemLevel;
                info->m_invTypeMask = s_inventoryTypeSlots[stats->m_inventoryType];

                if (stats->m_inventoryType == 18) {
                    info->m_invTypeMask = 0x80000;
                } else if (stats->m_inventoryType == 11) {
                    info->m_invTypeMask = 0x400;
                } else if (stats->m_inventoryType == 12) {
                    info->m_invTypeMask = 0x1000;
                }

                if (!info->m_invTypeMask) {
                    info->m_invTypeMask = 0x4000000;
                }
            } else {
                info->m_itemClass = -1;
                info->m_itemSubClass = -1;
                info->m_itemLevel = 0;
                info->m_invTypeMask = 0x4000000;

                if (spell.m_equippedItemClass == 2) {
                    info->m_invTypeMask = (spell.m_equippedItemSubclass & 0x1) ? 0x800000 : 0x1000000;
                } else if (spell.m_equippedItemClass == 4) {
                    if (spell.m_equippedItemInvTypes) {
                        info->m_invTypeMask = 0;

                        for (uint32_t j = 0; j < 29; j++) {
                            if ((1 << j) & spell.m_equippedItemInvTypes) {
                                info->m_invTypeMask = s_inventoryTypeSlots[j];

                                if (info->m_invTypeMask & 0x80000) {
                                    info->m_invTypeMask &= 0xFF8FFFFF;
                                } else if (info->m_invTypeMask & 0x400) {
                                    info->m_invTypeMask &= ~0x800;
                                } else if (info->m_invTypeMask & 0x1000) {
                                    info->m_invTypeMask &= ~0x2000;
                                }

                                break;
                            }
                        }

                        if (!info->m_invTypeMask) {
                            info->m_invTypeMask = 0x4000000;
                        }
                    } else if (spell.m_equippedItemSubclass & 0x40) {
                        info->m_invTypeMask = 0x2000000;
                    }
                }
            }

            CGTradeSkillInfo::m_invSlots |= info->m_invTypeMask;

            if (!CGTradeSkillInfo::m_itemsPending && full) {
                uint32_t index;

                for (index = 0; index < CGTradeSkillInfo::m_numSubClasses; index++) {
                    TradeSkillSubClassInfo* subClass = CGTradeSkillInfo::m_subClassList.m_data[index];

                    if (subClass->m_itemClass == info->m_itemClass && subClass->m_itemSubClass == info->m_itemSubClass) {
                        break;
                    }
                }

                if (index == CGTradeSkillInfo::m_numSubClasses) {
                    if (CGTradeSkillInfo::m_numSubClasses >= CGTradeSkillInfo::m_subClassList.Count()) {
                        TradeSkillSubClassInfo* subClass = static_cast<TradeSkillSubClassInfo*>(SMemAlloc(sizeof(TradeSkillSubClassInfo), __FILE__, __LINE__, 0));
                        *CGTradeSkillInfo::m_subClassList.New() = subClass;
                    }

                    TradeSkillSubClassInfo* subClass = CGTradeSkillInfo::m_subClassList.m_data[index];
                    subClass->m_itemClass = info->m_itemClass;
                    subClass->m_itemSubClass = info->m_itemSubClass;
                    subClass->m_shown = 1;
                    subClass->m_collapsed = 0;
                    subClass->m_invTypeMask = 0;
                    CGTradeSkillInfo::m_numSubClasses++;
                }

                CGTradeSkillInfo::m_subClassList.m_data[index]->m_invTypeMask |= info->m_invTypeMask;
            }
        }

        info->m_numAvailable = numAvailable;
    }

    if (CGTradeSkillInfo::m_itemsPending) {
        return;
    }

    if (full) {
        for (uint32_t k = 0; k < savedCount; k++) {
            for (uint32_t j = 0; j < CGTradeSkillInfo::m_numSubClasses; j++) {
                TradeSkillSubClassInfo* subClass = CGTradeSkillInfo::m_subClassList.m_data[j];

                if (subClass->m_itemClass == saved[k].m_itemClass && subClass->m_itemSubClass == saved[k].m_itemSubClass) {
                    subClass->m_shown = saved[k].m_shown;
                    subClass->m_collapsed = saved[k].m_collapsed;
                    break;
                }
            }
        }
    }

    for (uint32_t i = CGTradeSkillInfo::m_displayList.Count(); i < CGTradeSkillInfo::m_numSubClasses + CGTradeSkillInfo::m_numDisplayEntries; i++) {
        TradeSkillInfo* info = static_cast<TradeSkillInfo*>(SMemAlloc(sizeof(TradeSkillInfo), __FILE__, __LINE__, 0));
        *CGTradeSkillInfo::m_displayList.New() = info;
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
        TradeSkillInfo* header = CGTradeSkillInfo::m_displayList.m_data[CGTradeSkillInfo::m_numDisplayEntries + i];
        header->m_spellId = -1;
        header->m_itemClass = CGTradeSkillInfo::m_subClassList.m_data[i]->m_itemClass;
        header->m_itemSubClass = CGTradeSkillInfo::m_subClassList.m_data[i]->m_itemSubClass;
    }

    CGTradeSkillInfo::m_numDisplayEntries += CGTradeSkillInfo::m_numSubClasses;
    CGTradeSkillInfo::SetInvTypeFilter(0);

    if (full) {
        for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
            if (!CGTradeSkillInfo::m_subClassList.m_data[i]->m_shown) {
                CGTradeSkillInfo::m_subClassFilter &= ~(1 << i);
            }

            if (CGTradeSkillInfo::m_subClassList.m_data[i]->m_collapsed) {
                CGTradeSkillInfo::m_collapseFilter &= ~(1 << i);
            }
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, nullptr);
}

// VERIFY: 9ddd6131 manual
// OFFSET: 0x005DCAF0
void CGTradeSkillInfo::ShutdownGame() {
    for (uint32_t i = 0; i < CGTradeSkillInfo::m_displayList.m_count; i++) {
        delete CGTradeSkillInfo::m_displayList.m_data[i];
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_subClassList.m_count; i++) {
        delete CGTradeSkillInfo::m_subClassList.m_data[i];
    }

    CGTradeSkillInfo::m_playerGuid = WOWGUID();
    CGTradeSkillInfo::m_spellID = 0;
    CGTradeSkillInfo::m_skillLineID = 0;
    CGTradeSkillInfo::m_displayList.Clear();
    CGTradeSkillInfo::m_subClassList.Clear();
    CGTradeSkillInfo::m_numDisplayEntries = 0;
    CGTradeSkillInfo::m_filteredSkills = 0;
    CGTradeSkillInfo::m_numSubClasses = 0;

    if (CGTradeSkillInfo::m_itemNameFilter) {
        SMemFree(CGTradeSkillInfo::m_itemNameFilter, __FILE__, __LINE__, 0);
    }

    CGTradeSkillInfo::m_itemNameFilter = nullptr;
    CGTradeSkillInfo::unk_C235F0 = 0;
    CGTradeSkillInfo::m_recastSpellID = 0;
    CGTradeSkillInfo::m_pendingRecastSpellID = 0;

    if (CGTradeSkillInfo::m_linkData) {
        SMemFree(CGTradeSkillInfo::m_linkData, __FILE__, __LINE__, 0);
    }

    CGTradeSkillInfo::m_linkData = nullptr;
    CGTradeSkillInfo::m_skillRank = 0;
    CGTradeSkillInfo::m_skillMaxRank = 0;
}

// VERIFY: 9c6c8940 manual
// OFFSET: 0x005DA5A0
void CGTradeSkillInfo::Close() {
    CGTradeSkillInfo::m_spellID = 0;
    CGTradeSkillInfo::m_skillLineID = 0;

    if (CGTradeSkillInfo::m_recastSpellID || CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_CLOSE, nullptr);
}
