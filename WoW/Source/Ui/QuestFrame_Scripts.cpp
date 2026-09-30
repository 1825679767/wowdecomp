#include "QuestFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Client.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// VERIFY: a3ee6735 auto
// OFFSET: 0x0058C3A0
static int32_t Script_QuestChooseRewardError(lua_State* L) {
    CGGameUI::DisplayError(ERR_QUEST_MUST_CHOOSE);
    return 0;
}

// VERIFY: b53b4b57 auto
// OFFSET: 0x0058BD10
static int32_t Script_GetTitleText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_questTitle);
    return 1;
}

// VERIFY: 77b20f8a auto
// OFFSET: 0x0058BD30
static int32_t Script_GetGreetingText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_greetingText);
    return 1;
}

// VERIFY: 65b16336 auto
// OFFSET: 0x0058BD50
static int32_t Script_GetQuestText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_questText);
    return 1;
}

// VERIFY: e376d742 auto
// OFFSET: 0x0058BD70
static int32_t Script_GetObjectiveText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_questLogText);
    return 1;
}

// VERIFY: 72726c6f auto
// OFFSET: 0x0058BD90
static int32_t Script_GetProgressText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_progressText);
    return 1;
}

// VERIFY: 092ea285 auto
// OFFSET: 0x0058BDB0
static int32_t Script_GetRewardText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_rewardText);
    return 1;
}

// VERIFY: 0283a0c5 auto
// OFFSET: 0x0058C140
static int32_t Script_GetRewardTalents(lua_State* L) {
    lua_pushinteger(L, CGQuestInfo::m_rewardTalents);
    return 1;
}

// VERIFY: db1d2e60 auto
// OFFSET: 0x0058BDD0
static int32_t Script_GetNumAvailableQuests(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_numQuests);
    return 1;
}

// VERIFY: 2f7697ab auto
// OFFSET: 0x0058BE00
static int32_t Script_GetNumActiveQuests(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_numInProgress);
    return 1;
}

// VERIFY: 945cd00b auto
// OFFSET: 0x0058C0B0
static int32_t Script_GetRewardMoney(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_rewardMoney);
    return 1;
}

// VERIFY: b0875ab9 auto
// OFFSET: 0x0058C0E0
static int32_t Script_GetRewardXP(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_rewardXP);
    return 1;
}

// VERIFY: c101580c auto
// OFFSET: 0x0058C110
static int32_t Script_GetRewardHonor(lua_State* L) {
    lua_pushinteger(L, CGQuestInfo::m_rewardHonor / 10);
    return 1;
}

// VERIFY: 9f65bb76 auto
// OFFSET: 0x0058C160
static int32_t Script_GetQuestMoneyToGet(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_requiredMoney);
    return 1;
}

// VERIFY: d0de3ef0 auto
// OFFSET: 0x0058C3B0
static int32_t Script_GetSuggestedGroupNum(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_suggestedGroupNum);
    return 1;
}

// VERIFY: f7458957 auto
// OFFSET: 0x0058C470
static int32_t Script_GetMaxDailyQuests(lua_State* L) {
    lua_pushnumber(L, 25.0);
    return 1;
}

// VERIFY: 6dc26a10 auto
// OFFSET: 0x0058C4A0
static int32_t Script_GetRewardArenaPoints(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_rewardArenaPoints);
    return 1;
}

// VERIFY: 4ceaaf96 auto
// OFFSET: 0x0058C430
static int32_t Script_QuestGetAutoAccept(lua_State* L) {
    if (CGQuestInfo::m_questFlags & 0x80000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 87727b0c auto
// OFFSET: 0x0058C4D0
static int32_t Script_QuestIsDaily(lua_State* L) {
    if (CGQuestInfo::m_questFlags & 0x1000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: dfc603b7 auto
// OFFSET: 0x0058C510
static int32_t Script_QuestIsWeekly(lua_State* L) {
    if (CGQuestInfo::m_questFlags & 0x8000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 1f1988d2 auto
// OFFSET: 0x0058C3E0
static int32_t Script_QuestFlagsPVP(lua_State* L) {
    if (s_isPvPRealm || !(CGQuestInfo::m_questFlags & 0x2000)) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// VERIFY: fa6d3013 manual
// OFFSET: 0x0058C910
static int32_t Script_ConfirmAcceptQuest(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_CONFIRM_ACCEPT));
    msg.Put(CGQuestInfo::m_pendingQuest);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 838b33fa auto
// OFFSET: 0x0058C190
static int32_t Script_GetNumQuestRewards(lua_State* L) {
    int32_t count = 0;

    for (uint32_t i = 0; i < 6; i++) {
        if (!CGQuestInfo::m_questItems[i].m_reward.m_itemID) {
            break;
        }

        count++;
    }

    lua_pushnumber(L, count);
    return 1;
}

// VERIFY: b0f00d3b auto
// OFFSET: 0x0058C1F0
static int32_t Script_GetNumQuestChoices(lua_State* L) {
    int32_t count = 0;

    for (uint32_t i = 0; i < 6; i++) {
        if (!CGQuestInfo::m_questItems[i].m_choice.m_itemID) {
            break;
        }

        count++;
    }

    lua_pushnumber(L, count);
    return 1;
}

// VERIFY: 530b342a auto
// OFFSET: 0x0058C250
static int32_t Script_GetNumQuestItems(lua_State* L) {
    int32_t count = 0;

    for (uint32_t i = 0; i < 6; i++) {
        if (!CGQuestInfo::m_questItems[i].m_required.unk_0004) {
            break;
        }

        count++;
    }

    lua_pushnumber(L, count);
    return 1;
}

// VERIFY: a92581b2 manual
// OFFSET: 0x0058BE30
static int32_t Script_GetAvailableTitle(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetAvailableTitle(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < CGQuestInfo::m_numQuests) {
        lua_pushstring(L, CGQuestInfo::m_availableQuests[index].m_title);
    } else {
        lua_pushstring(L, nullptr);
    }

    return 1;
}

// VERIFY: c93c9173 manual
// OFFSET: 0x0058BF70
static int32_t Script_GetAvailableLevel(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGetAvailableLevel(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t level;

    if (index < CGQuestInfo::m_numQuests) {
        level = CGQuestInfo::m_availableQuests[index].m_level;
    } else {
        level = 0;
    }

    lua_pushnumber(L, level);
    return 1;
}

// VERIFY: 32cdbb3a manual
// OFFSET: 0x0058C010
static int32_t Script_GetActiveLevel(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGetActiveLevel(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t level;

    if (index < CGQuestInfo::m_numInProgress) {
        level = CGQuestInfo::m_inProgressQuests[index].m_level;
    } else {
        level = 0;
    }

    lua_pushnumber(L, level);
    return 1;
}

// VERIFY: 14700cd7 manual
// OFFSET: 0x0058DB30
static int32_t Script_GetDailyQuestsCompleted(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t count = 0;

    if (player) {
        for (uint32_t i = 0; i < 25; i++) {
            if (player->m_player->m_playerFieldDailyQuests1[i]) {
                count++;
            }
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// STATIC: 0x00ACEFD8
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseQuest", &Script_CloseQuest },
    { "GetTitleText", &Script_GetTitleText },
    { "GetGreetingText", &Script_GetGreetingText },
    { "GetQuestText", &Script_GetQuestText },
    { "GetObjectiveText", &Script_GetObjectiveText },
    { "GetProgressText", &Script_GetProgressText },
    { "GetRewardText", &Script_GetRewardText },
    { "GetNumAvailableQuests", &Script_GetNumAvailableQuests },
    { "GetNumActiveQuests", &Script_GetNumActiveQuests },
    { "GetAvailableTitle", &Script_GetAvailableTitle },
    { "GetActiveTitle", &Script_GetActiveTitle },
    { "GetAvailableLevel", &Script_GetAvailableLevel },
    { "GetActiveLevel", &Script_GetActiveLevel },
    { "IsAvailableQuestTrivial", &Script_IsAvailableQuestTrivial },
    { "IsActiveQuestTrivial", &Script_IsActiveQuestTrivial },
    { "SelectAvailableQuest", &Script_SelectAvailableQuest },
    { "SelectActiveQuest", &Script_SelectActiveQuest },
    { "AcceptQuest", &Script_AcceptQuest },
    { "DeclineQuest", &Script_DeclineQuest },
    { "IsQuestCompletable", &Script_IsQuestCompletable },
    { "CompleteQuest", &Script_CompleteQuest },
    { "GetQuestReward", &Script_GetQuestReward },
    { "GetRewardMoney", &Script_GetRewardMoney },
    { "GetRewardXP", &Script_GetRewardXP },
    { "GetRewardHonor", &Script_GetRewardHonor },
    { "GetRewardSpell", &Script_GetRewardSpell },
    { "GetQuestMoneyToGet", &Script_GetQuestMoneyToGet },
    { "GetNumQuestRewards", &Script_GetNumQuestRewards },
    { "GetNumQuestChoices", &Script_GetNumQuestChoices },
    { "GetNumQuestItems", &Script_GetNumQuestItems },
    { "GetQuestItemInfo", &Script_GetQuestItemInfo },
    { "GetQuestItemLink", &Script_GetQuestItemLink },
    { "GetQuestSpellLink", &Script_GetQuestSpellLink },
    { "QuestChooseRewardError", &Script_QuestChooseRewardError },
    { "ConfirmAcceptQuest", &Script_ConfirmAcceptQuest },
    { "GetQuestBackgroundMaterial", &Script_GetQuestBackgroundMaterial },
    { "GetSuggestedGroupNum", &Script_GetSuggestedGroupNum },
    { "QuestFlagsPVP", &Script_QuestFlagsPVP },
    { "QuestGetAutoAccept", &Script_QuestGetAutoAccept },
    { "GetDailyQuestsCompleted", &Script_GetDailyQuestsCompleted },
    { "GetMaxDailyQuests", &Script_GetMaxDailyQuests },
    { "GetRewardTitle", &Script_GetRewardTitle },
    { "GetRewardTalents", &Script_GetRewardTalents },
    { "GetRewardArenaPoints", &Script_GetRewardArenaPoints },
    { "GetAvailableQuestInfo", &Script_GetAvailableQuestInfo },
    { "QuestIsDaily", &Script_QuestIsDaily },
    { "QuestIsWeekly", &Script_QuestIsWeekly },
};

// VERIFY: 9a990fcd auto
// OFFSET: 0x0058C550
void QuestInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
