#ifndef WOW_SOURCE_UI_QUESTFRAME_H
#define WOW_SOURCE_UI_QUESTFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct QuestItemInfo {
    /* 0000 */ int32_t m_itemID;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t unk_0008;
};

struct QuestItemSlot {
    /* 0000 */ QuestItemInfo m_reward;
    /* 000C */ QuestItemInfo m_choice;
    /* 0018 */ QuestItemInfo m_required;
};

struct QuestGiverQuest {
    /* 0000 */ int32_t m_questID;
    /* 0004 */ int32_t m_level;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ char m_title[512];
    /* 020C */ int32_t unk_020C;
    /* 0210 */ int32_t unk_0210;
};

class CGQuestInfo {
    public:
    static void QuestGiverFinished(int32_t keepOpen, uint32_t questId, int32_t notify);
    static void EnterWorld();
    static WOWGUID m_npc;
    static WOWGUID m_npcInform;
    static char m_greetingText[2048];
    static char m_questTitle[512];
    static char m_rewardText[3000];
    static char m_progressText[3000];
    static char m_questLogText[3000];
    static char m_questText[3000];
    static int32_t m_rewardMoney;
    static int32_t m_rewardXP;
    static int32_t m_rewardTalents;
    static int32_t m_numQuests;
    static int32_t m_numInProgress;
    static QuestItemSlot m_questItems[6];
    static QuestGiverQuest m_inProgressQuests[32];
    static QuestGiverQuest m_availableQuests[32];
    static uint32_t m_rewardHonor;
    static int32_t m_rewardArenaPoints;
    static int32_t m_requiredMoney;
    static int32_t m_suggestedGroupNum;
    static uint32_t m_questFlags;
    static uint32_t m_pendingQuest;

    static void ConfirmAcceptQuest(int32_t questId, const char* title, const WOWGUID* guid);
};
struct lua_State;

int32_t Script_CloseQuest(lua_State* L);
int32_t Script_GetActiveTitle(lua_State* L);
int32_t Script_IsAvailableQuestTrivial(lua_State* L);
int32_t Script_IsActiveQuestTrivial(lua_State* L);
int32_t Script_SelectAvailableQuest(lua_State* L);
int32_t Script_SelectActiveQuest(lua_State* L);
int32_t Script_AcceptQuest(lua_State* L);
int32_t Script_DeclineQuest(lua_State* L);
int32_t Script_IsQuestCompletable(lua_State* L);
int32_t Script_CompleteQuest(lua_State* L);
int32_t Script_GetQuestReward(lua_State* L);
int32_t Script_GetRewardSpell(lua_State* L);
int32_t Script_GetQuestItemInfo(lua_State* L);
int32_t Script_GetQuestItemLink(lua_State* L);
int32_t Script_GetQuestSpellLink(lua_State* L);
int32_t Script_GetQuestBackgroundMaterial(lua_State* L);
int32_t Script_GetRewardTitle(lua_State* L);
int32_t Script_GetAvailableQuestInfo(lua_State* L);
void QuestInfoRegisterScriptFunctions();

#endif
