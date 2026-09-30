#include "QuestLog.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include <ctime>
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00C0173C
static int32_t s_numQuestLogRewardFactions;

// VERIFY: ee052397 auto
// OFFSET: 0x005DF0D0
static int32_t Script_SetAbandonQuest(lua_State* L) {
    CGQuestLog::m_abandonQuest = CGQuestLog::m_selectedQuest;
    return 0;
}

// VERIFY: 8eb8a2cc auto
// OFFSET: 0x005DF910
static int32_t Script_GetNumQuestLogRewardFactions(lua_State* L) {
    lua_pushnumber(L, s_numQuestLogRewardFactions);
    return 1;
}

// VERIFY: 5504824c manual
// OFFSET: 0x005DF5D0
static int32_t Script_GetNumQuestWatches(lua_State* L) {
    lua_pushnumber(L, CGQuestLog::m_numQuestWatches);
    return 1;
}

// VERIFY: 86a938d8 auto
// OFFSET: 0x005DFB00
static int32_t Script_SetPOIIconOverlapDistance(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetPOIIconOverlapDistance(index)");
    }

    CGQuestLog::m_questIconOverlapDist = static_cast<float>(lua_tonumber(L, 1));
    return 0;
}

// VERIFY: 48abddb9 auto
// OFFSET: 0x005DFB40
static int32_t Script_SetPOIIconOverlapPushDistance(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetPOIIconOverlapPushDistance(index)");
        return 0;
    }

    float distance = static_cast<float>(lua_tonumber(L, 1));

    if (CGQuestLog::m_questIconOverlapDist <= distance) {
        CGQuestLog::m_questIconOverlapPushDist = distance;
    }

    return 0;
}

// VERIFY: 13e3138c auto
// OFFSET: 0x005E51A0
static int32_t Script_GetQuestGreenRange(lua_State* L) {
    lua_pushnumber(L, CGQuestLog::GetQuestGreenRange());
    return 1;
}

// VERIFY: 28d4ab3f manual
// OFFSET: 0x005DF010
static int32_t Script_GetNumQuestLogEntries(lua_State* L) {
    lua_pushnumber(L, CGQuestLog::m_numShownQuests);
    uint32_t quests = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(CGQuestLog::m_numQuest); i++) {
        if (static_cast<int32_t>(i) < 0 || static_cast<int32_t>(i) >= CGQuestLog::m_numQuest || !CGQuestLog::m_quests[i].m_isHeader) {
            quests++;
        }
    }

    lua_pushnumber(L, quests);
    return 2;
}

// VERIFY: 8f4872a2 manual
// OFFSET: 0x005DFC40
static int32_t Script_GetQuestWatchIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestWatchIndex(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    int32_t questId = CGQuestLog::m_quests[index].m_questId;

    if (!questId) {
        return 0;
    }

    for (uint32_t i = 0; i < CGQuestLog::m_numQuestWatches; i++) {
        if ((i < 25 ? CGQuestLog::m_questWatches[i].m_questId : 0) == questId) {
            lua_pushnumber(L, i + 1);
            return 1;
        }
    }

    return 0;
}

// VERIFY: 2381a775 manual
// OFFSET: 0x005DF610
static int32_t Script_IsQuestWatched(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsQuestWatched(index)");
        return 0;
    }

    int32_t watched = 0;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 0 && index < CGQuestLog::m_numQuest && !CGQuestLog::m_quests[index].m_isHeader) {
        int32_t questId = CGQuestLog::m_quests[index].m_questId;

        if (questId && CGQuestLog::m_numQuestWatches) {
            for (uint32_t i = 0; i < CGQuestLog::m_numQuestWatches; i++) {
                if ((i < 25 ? CGQuestLog::m_questWatches[i].m_questId : 0) == questId) {
                    watched = 1;
                }
            }

            if (watched) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: c6e716e7 manual
// OFFSET: 0x005DF780
static int32_t Script_GetQuestIndexForWatch(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestIndexForWatch(index)");
        return 0;
    }

    uint32_t watch = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t questId;

    if (watch < 25 && (questId = CGQuestLog::m_questWatches[watch].m_questId) && CGQuestLog::m_numQuest) {
        for (uint32_t i = 0; i < static_cast<uint32_t>(CGQuestLog::m_numQuest); i++) {
            int32_t id = static_cast<int32_t>(i) < 0 || static_cast<int32_t>(i) >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[i].m_isHeader ? 0 : CGQuestLog::m_quests[i].m_questId;

            if (id == questId) {
                lua_pushnumber(L, i + 1);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: c32d8dd7 manual
// OFFSET: 0x005E02F0
static int32_t Script_SelectQuestLogEntry(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SelectQuestLogEntry(index)");
        return 0;
    }

    CGQuestLog::SetSelectedQuest(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
    return 0;
}

// VERIFY: a201ed70 auto
// OFFSET: 0x005DF0A0
static int32_t Script_GetQuestLogSelection(lua_State* L) {
    lua_pushnumber(L, CGQuestLog::GetSelectedLogEntry() + 1);
    return 1;
}

// VERIFY: 9be26835 auto
// OFFSET: 0x005DF0E0
static int32_t Script_GetAbandonQuestName(lua_State* L) {
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_abandonQuest, WOWGUID(), nullptr, nullptr, false);

    if (quest) {
        lua_pushstring(L, quest->m_name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 9852e1b1 manual
// OFFSET: 0x005DF520
static int32_t Script_GetQuestLogPushable(lua_State* L) {
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t questId = index ? CGQuestLog::GetQuestAt(index - 1) : CGQuestLog::m_selectedQuest;
    QuestCache* quest = g_questDBCache.GetRecord(questId, WOWGUID(), nullptr, nullptr, false);

    if (!quest || !(quest->m_flags & 8)) {
        return 0;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// VERIFY: b5bc01a2 manual
// OFFSET: 0x005E6240
static int32_t Script_GetQuestTimers(lua_State* L) {
    int32_t count = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t numQuest = CGQuestLog::m_numQuest;
    int32_t timeOffset = CGQuestLog::m_timeOffset;
    int32_t failed = 0;

    if (!numQuest) {
        return 0;
    }

    for (int32_t i = 0; static_cast<uint32_t>(i) < numQuest; i++) {
        if (i < 0 || i >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[i].m_isHeader) {
            continue;
        }

        int32_t slot = CGQuestLog::m_quests[i].m_logSlot;

        if (slot < 0 || slot > 25) {
            continue;
        }

        PlayerQuestLogEntry* entry = &player->m_player->m_playerQuestLog[slot];

        if (!entry || !entry->m_time || (entry->m_state & 2) || (i < CGQuestLog::m_numQuest && CGQuestLog::m_quests[i].m_expired)) {
            continue;
        }

        int32_t left = timeOffset + entry->m_time - static_cast<int32_t>(time(nullptr)) - 1;

        if (left < 0) {
            if (CGQuestLog::SetQuestExpired(i)) {
                QuestCache* quest = g_questDBCache.GetRecord(entry->m_questId, WOWGUID(), DisplayQuestFailed, nullptr, false);

                if (quest) {
                    CGGameUI::DisplayError(ERR_QUEST_FAILED_S, quest->m_name);
                }

                failed = 1;
            }
        } else {
            count++;
            lua_checkstack(L, 1);
            lua_pushnumber(L, left);
        }
    }

    if (failed) {
        Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
    }

    return count;
}

// STATIC: 0x00AD1020
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumQuestLogEntries", &Script_GetNumQuestLogEntries },
    { "GetQuestLogTitle", &Script_GetQuestLogTitle },
    { "SelectQuestLogEntry", &Script_SelectQuestLogEntry },
    { "GetQuestLogSelection", &Script_GetQuestLogSelection },
    { "SetAbandonQuest", &Script_SetAbandonQuest },
    { "GetAbandonQuestName", &Script_GetAbandonQuestName },
    { "GetAbandonQuestItems", &Script_GetAbandonQuestItems },
    { "AbandonQuest", &Script_AbandonQuest },
    { "IsUnitOnQuest", &Script_IsUnitOnQuest },
    { "GetQuestLogQuestText", &Script_GetQuestLogQuestText },
    { "GetNumQuestLeaderBoards", &Script_GetNumQuestLeaderBoards },
    { "GetQuestLogLeaderBoard", &Script_GetQuestLogLeaderBoard },
    { "GetNumQuestItemDrops", &Script_GetNumQuestItemDrops },
    { "GetQuestLogItemDrop", &Script_GetQuestLogItemDrop },
    { "GetQuestPOILeaderBoard", &Script_GetQuestPOILeaderBoard },
    { "GetQuestLogTimeLeft", &Script_GetQuestLogTimeLeft },
    { "IsCurrentQuestFailed", &Script_IsCurrentQuestFailed },
    { "GetNumQuestLogRewards", &Script_GetNumQuestLogRewards },
    { "GetNumQuestLogChoices", &Script_GetNumQuestLogChoices },
    { "GetQuestLogRewardInfo", &Script_GetQuestLogRewardInfo },
    { "GetQuestLogChoiceInfo", &Script_GetQuestLogChoiceInfo },
    { "GetQuestLogItemLink", &Script_GetQuestLogItemLink },
    { "GetQuestLogSpellLink", &Script_GetQuestLogSpellLink },
    { "GetQuestLogRewardMoney", &Script_GetQuestLogRewardMoney },
    { "GetQuestLogRewardXP", &Script_GetQuestLogRewardXP },
    { "GetQuestLogRewardHonor", &Script_GetQuestLogRewardHonor },
    { "GetQuestLogRewardSpell", &Script_GetQuestLogRewardSpell },
    { "GetQuestLogRequiredMoney", &Script_GetQuestLogRequiredMoney },
    { "GetQuestLogPushable", &Script_GetQuestLogPushable },
    { "QuestLogPushQuest", &Script_QuestLogPushQuest },
    { "GetQuestTimers", &Script_GetQuestTimers },
    { "GetQuestIndexForTimer", &Script_GetQuestIndexForTimer },
    { "CollapseQuestHeader", &Script_CollapseQuestHeader },
    { "ExpandQuestHeader", &Script_ExpandQuestHeader },
    { "GetQuestGreenRange", &Script_GetQuestGreenRange },
    { "GetNumQuestWatches", &Script_GetNumQuestWatches },
    { "IsQuestWatched", &Script_IsQuestWatched },
    { "AddQuestWatch", &Script_AddQuestWatch },
    { "RemoveQuestWatch", &Script_RemoveQuestWatch },
    { "GetQuestIndexForWatch", &Script_GetQuestIndexForWatch },
    { "GetQuestLogGroupNum", &Script_GetQuestLogGroupNum },
    { "GetQuestResetTime", &Script_GetQuestResetTime },
    { "GetQuestLink", &Script_GetQuestLink },
    { "GetQuestLogRewardTitle", &Script_GetQuestLogRewardTitle },
    { "GetQuestLogRewardTalents", &Script_GetQuestLogRewardTalents },
    { "GetQuestLogRewardArenaPoints", &Script_GetQuestLogRewardArenaPoints },
    { "GetQuestLogSpecialItemInfo", &Script_GetQuestLogSpecialItemInfo },
    { "GetQuestLogSpecialItemCooldown", &Script_GetQuestLogSpecialItemCooldown },
    { "IsQuestLogSpecialItemInRange", &Script_IsQuestLogSpecialItemInRange },
    { "UseQuestLogSpecialItem", &Script_UseQuestLogSpecialItem },
    { "ProcessQuestLogRewardFactions", &Script_ProcessQuestLogRewardFactions },
    { "GetNumQuestLogRewardFactions", &Script_GetNumQuestLogRewardFactions },
    { "GetQuestLogRewardFactionInfo", &Script_GetQuestLogRewardFactionInfo },
    { "SortQuestWatches", &Script_SortQuestWatches },
    { "ShiftQuestWatches", &Script_ShiftQuestWatches },
    { "GetQuestWatchIndex", &Script_GetQuestWatchIndex },
    { "QuestMapUpdateAllQuests", &Script_QuestMapUpdateAllQuests },
    { "GetQuestSortIndex", &Script_GetQuestSortIndex },
    { "GetQuestWorldMapAreaID", &Script_GetQuestWorldMapAreaID },
    { "QuestPOIUpdateTexture", &Script_QuestPOIUpdateTexture },
    { "QuestPOIUpdateIcons", &Script_QuestPOIUpdateIcons },
    { "QuestPOIGetIconInfo", &Script_QuestPOIGetIconInfo },
    { "QuestPOIGetQuestIDByIndex", &Script_QuestPOIGetQuestIDByIndex },
    { "QuestPOIGetQuestIDByVisibleIndex", &Script_QuestPOIGetQuestIDByVisibleIndex },
    { "GetQuestLogCompletionText", &Script_GetQuestLogCompletionText },
    { "SetPOIIconOverlapDistance", &Script_SetPOIIconOverlapDistance },
    { "SetPOIIconOverlapPushDistance", &Script_SetPOIIconOverlapPushDistance },
};

// VERIFY: 3feca0f3 auto
// OFFSET: 0x005DFD20
void QuestLogRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
