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
    static int32_t m_rewardFactionList[10];
    static int32_t m_rewardFactionAmounts[10];
    static int32_t IsLowLevel(int32_t level);
    static int32_t m_state;
    static int32_t m_currentQuest;
    static int32_t m_autoLaunched;
    static int32_t m_startQuestCheat;
    static int32_t m_lastChosenItem;
    static int32_t m_responseSent;
    static float m_rewardHonorMultiplier;
    static int32_t m_rewardSpell;
    static int32_t m_rewardSpellCast;
    static int32_t m_rewardTitle;
    static uint32_t m_rewardFactionMask;
    static int32_t m_rewardFactionIDs[5];
    static int32_t m_rewardFactionValues[5];
    static int32_t m_rewardFactionOverrides[5];
    static void ClearState();
    static void ClearQuestItems();
    static void ProcessQuestLogRewardFactions();
    static void InitRewRepFields(uint32_t index, int32_t faction, int32_t value, int32_t override);
    static void InitRewRepShowMask(uint32_t mask);
    static int32_t m_numRewardFactions;
    static void SetState(WOWGUID npc, WOWGUID inform, int32_t state, const char* text, int32_t questId, int32_t startCheat);
    static void SetLogDescription(const char* text);
    static void AddReward(const char* title, const uint32_t* choiceItems, const uint32_t* choiceDisplays, const uint32_t* choiceCounts, int32_t numChoices, const uint32_t* rewardItems, const uint32_t* rewardDisplays, const uint32_t* rewardCounts, int32_t numRewards, int32_t money, int32_t xp, uint32_t honor, float honorMultiplier, int32_t spell, int32_t spellCast, int32_t autoLaunched, int32_t suggestedGroupNum, int32_t rewardTitle, int32_t talents, int32_t arenaPoints, uint32_t factionMask, const uint32_t* factionIDs, const uint32_t* factionValues, const uint32_t* factionOverrides, uint32_t questFlags);
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
void ProcessQuestRewardFactions(int32_t factionId, int32_t amount, int32_t upperRank, int32_t lowerRank, int32_t source, int32_t noSpillover);

#endif
