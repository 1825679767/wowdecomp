#include "QuestFrame.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/ChatFrame.h"

// STATIC: 0x00C01840
int32_t CGQuestInfo::m_rewardFactionList[10];

// STATIC: 0x00C01818
int32_t CGQuestInfo::m_rewardFactionAmounts[10];

// STATIC: 0x00C0D658
int32_t CGQuestInfo::m_state;

// STATIC: 0x00C0D65C
int32_t CGQuestInfo::m_currentQuest;

// STATIC: 0x00C0D664
int32_t CGQuestInfo::m_autoLaunched;

// STATIC: 0x00C0D668
int32_t CGQuestInfo::m_startQuestCheat;

// STATIC: 0x00C0D66C
int32_t CGQuestInfo::m_lastChosenItem;

// STATIC: 0x00C0D6AC
int32_t CGQuestInfo::m_responseSent;

// STATIC: 0x00C0D67C
float CGQuestInfo::m_rewardHonorMultiplier;

// STATIC: 0x00C0D680
int32_t CGQuestInfo::m_rewardSpell;

// STATIC: 0x00C0D684
int32_t CGQuestInfo::m_rewardSpellCast;

// STATIC: 0x00C0D688
int32_t CGQuestInfo::m_rewardTitle;

// STATIC: 0x00C0D698
uint32_t CGQuestInfo::m_rewardFactionMask;

// STATIC: 0x00C01728
int32_t CGQuestInfo::m_rewardFactionIDs[5];

// STATIC: 0x00C01714
int32_t CGQuestInfo::m_rewardFactionValues[5];

// STATIC: 0x00C01700
int32_t CGQuestInfo::m_rewardFactionOverrides[5];

// STATIC: 0x00C0D648
WOWGUID CGQuestInfo::m_npc;

// STATIC: 0x00C0D650
WOWGUID CGQuestInfo::m_npcInform;

// STATIC: 0x00C0CC48
char CGQuestInfo::m_greetingText[2048];

// STATIC: 0x00C0D448
char CGQuestInfo::m_questTitle[512];

// STATIC: 0x00C09D68
char CGQuestInfo::m_rewardText[3000];

// STATIC: 0x00C0A920
char CGQuestInfo::m_progressText[3000];

// STATIC: 0x00C0B4D8
char CGQuestInfo::m_questLogText[3000];

// STATIC: 0x00C0C090
char CGQuestInfo::m_questText[3000];

// STATIC: 0x00C0D670
int32_t CGQuestInfo::m_rewardMoney;

// STATIC: 0x00C0D674
int32_t CGQuestInfo::m_rewardXP;

// STATIC: 0x00C0D690
int32_t CGQuestInfo::m_rewardTalents;

// STATIC: 0x00C0D69C
int32_t CGQuestInfo::m_numQuests;

// STATIC: 0x00C0D6A0
int32_t CGQuestInfo::m_numInProgress;

// STATIC: 0x00C0D678
uint32_t CGQuestInfo::m_rewardHonor;

// STATIC: 0x00C0D694
int32_t CGQuestInfo::m_rewardArenaPoints;

// STATIC: 0x00C0D6A4
int32_t CGQuestInfo::m_requiredMoney;

// STATIC: 0x00C0D6B0
int32_t CGQuestInfo::m_suggestedGroupNum;

// STATIC: 0x00C0D6B4
uint32_t CGQuestInfo::m_questFlags;

// STATIC: 0x00C0D6A8
uint32_t CGQuestInfo::m_pendingQuest;

// STATIC: 0x00C01740
QuestItemSlot CGQuestInfo::m_questItems[6];

// STATIC: 0x00C01868
QuestGiverQuest CGQuestInfo::m_inProgressQuests[32];

// STATIC: 0x00C05AE8
QuestGiverQuest CGQuestInfo::m_availableQuests[32];

// VERIFY: 63c859fb manual
// OFFSET: 0x0058CA70
void CGQuestInfo::QuestGiverFinished(int32_t keepOpen, uint32_t questId, int32_t notify) {
    if (!CGQuestInfo::m_npc) {
        return;
    }

    if (notify && ClntObjMgrObjectPtr(CGQuestInfo::m_npc, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(MSG_QUEST_PUSH_RESULT));
        msg.Put(CGQuestInfo::m_npcInform ? CGQuestInfo::m_npcInform.m_guid : CGQuestInfo::m_npc.m_guid);
        msg.Put(questId);
        msg.Put(static_cast<uint8_t>(3));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (keepOpen) {
        return;
    }

    WOWGUID npc = CGQuestInfo::m_npc;
    CGQuestInfo::m_npc = WOWGUID();
    CGQuestInfo::m_npcInform = WOWGUID();

    if (!CGGossipInfo::m_gossip) {
        CGGameUI::ClearInteractTarget(npc, 0);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_FINISHED, nullptr);
}

// VERIFY: fa332c43 manual
// OFFSET: 0x0058B720
void CGQuestInfo::ClearState() {
    for (int32_t i = 0; i < 32; i++) {
        CGQuestInfo::m_availableQuests[i].m_questID = 0;
        CGQuestInfo::m_availableQuests[i].m_level = 0;
        CGQuestInfo::m_availableQuests[i].unk_0008 = 0;
        CGQuestInfo::m_availableQuests[i].m_title[0] = 0;
        CGQuestInfo::m_availableQuests[i].unk_020C = 0;
        CGQuestInfo::m_availableQuests[i].unk_0210 = 0;
        CGQuestInfo::m_inProgressQuests[i].m_questID = 0;
        CGQuestInfo::m_inProgressQuests[i].m_level = 0;
        CGQuestInfo::m_inProgressQuests[i].unk_0008 = 0;
        CGQuestInfo::m_inProgressQuests[i].m_title[0] = 0;
        CGQuestInfo::m_inProgressQuests[i].unk_020C = 0;
        CGQuestInfo::m_inProgressQuests[i].unk_0210 = 0;
    }

    CGQuestInfo::m_numQuests = 0;
    CGQuestInfo::m_numInProgress = 0;
    CGQuestInfo::m_greetingText[0] = 0;
    CGQuestInfo::m_questTitle[0] = 0;
    CGQuestInfo::m_questText[0] = 0;
    CGQuestInfo::m_questLogText[0] = 0;
    CGQuestInfo::m_progressText[0] = 0;
    CGQuestInfo::m_rewardText[0] = 0;
    CGQuestInfo::m_suggestedGroupNum = 0;
    CGQuestInfo::m_autoLaunched = 0;
    CGQuestInfo::m_questFlags = 0;
}

// VERIFY: d07a48c9 auto
// OFFSET: 0x0058B7D0
void CGQuestInfo::ClearQuestItems() {
    for (int32_t i = 0; i < 6; i++) {
        CGQuestInfo::m_questItems[i].m_reward.m_itemID = 0;
        CGQuestInfo::m_questItems[i].m_reward.unk_0004 = 0;
        CGQuestInfo::m_questItems[i].m_reward.unk_0008 = 0;
        CGQuestInfo::m_questItems[i].m_choice.m_itemID = 0;
        CGQuestInfo::m_questItems[i].m_choice.unk_0004 = 0;
        CGQuestInfo::m_questItems[i].m_choice.unk_0008 = 0;
        CGQuestInfo::m_questItems[i].m_required.m_itemID = 0;
        CGQuestInfo::m_questItems[i].m_required.unk_0004 = 0;
        CGQuestInfo::m_questItems[i].m_required.unk_0008 = 0;
    }
}

// VERIFY: 2ce3fb4d manual
// OFFSET: 0x0058C5A0
void CGQuestInfo::SetState(WOWGUID npc, WOWGUID inform, int32_t state, const char* text, int32_t questId, int32_t startCheat) {
    CGObject_C* object = ClntObjMgrObjectPtr(npc, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object || !(object->m_obj->m_type & (TYPEMASK_ITEM | TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT))) {
        return;
    }

    if (!state) {
        CGQuestInfo::ClearState();
    }

    char buffer[3000];
    WOWGUID player = ClntObjMgrGetActivePlayer();
    QuestParserParseText(text, buffer, sizeof(buffer), player, nullptr, 0);

    if (!buffer[0]) {
        SStrCopy(buffer, " ", sizeof(buffer));
    }

    switch (state) {
        case 0:
            SStrCopy(CGQuestInfo::m_greetingText, buffer, sizeof(CGQuestInfo::m_greetingText));
            break;

        case 1:
            SStrCopy(CGQuestInfo::m_questText, buffer, sizeof(CGQuestInfo::m_questText));
            break;

        case 2:
            SStrCopy(CGQuestInfo::m_progressText, buffer, sizeof(CGQuestInfo::m_progressText));
            break;

        case 3:
            SStrCopy(CGQuestInfo::m_rewardText, buffer, sizeof(CGQuestInfo::m_rewardText));
            break;

        default:
            break;
    }

    CGQuestInfo::m_state = state;
    CGQuestInfo::m_currentQuest = questId;
    CGQuestInfo::m_startQuestCheat = startCheat;

    if (!(object->m_obj->m_type & TYPEMASK_GAMEOBJECT) || npc != CGGameUI::m_interactTarget) {
        CGGameUI::SetInteractTarget(npc);
    }

    CGQuestInfo::m_npc = npc;
    CGQuestInfo::m_npcInform = inform;
    CGQuestInfo::m_lastChosenItem = 0;
    CGQuestInfo::m_responseSent = 0;
}

// VERIFY: 2ebdd245 auto
// OFFSET: 0x0058C720
void CGQuestInfo::SetLogDescription(const char* text) {
    if (!text || !*text) {
        CGQuestInfo::m_questLogText[0] = 0;
        return;
    }

    char buffer[3000];
    WOWGUID player = ClntObjMgrGetActivePlayer();
    QuestParserParseText(text, buffer, sizeof(buffer), player, nullptr, 0);
    SStrCopy(CGQuestInfo::m_questLogText, buffer, sizeof(CGQuestInfo::m_questLogText));
}

// VERIFY: 99119eef manual
// OFFSET: 0x0058DD50
void CGQuestInfo::AddReward(const char* title, const uint32_t* choiceItems, const uint32_t* choiceDisplays, const uint32_t* choiceCounts, int32_t numChoices, const uint32_t* rewardItems, const uint32_t* rewardDisplays, const uint32_t* rewardCounts, int32_t numRewards, int32_t money, int32_t xp, uint32_t honor, float honorMultiplier, int32_t spell, int32_t spellCast, int32_t autoLaunched, int32_t suggestedGroupNum, int32_t rewardTitle, int32_t talents, int32_t arenaPoints, uint32_t factionMask, const uint32_t* factionIDs, const uint32_t* factionValues, const uint32_t* factionOverrides, uint32_t questFlags) {
    CGQuestInfo::ClearQuestItems();
    int32_t i = 0;

    for (; i < numChoices; i++) {
        CGQuestInfo::m_questItems[i].m_choice.m_itemID = choiceItems[i];
        CGQuestInfo::m_questItems[i].m_choice.unk_0004 = choiceDisplays[i];
        CGQuestInfo::m_questItems[i].m_choice.unk_0008 = choiceCounts[i];
    }

    for (; i < 6; i++) {
        CGQuestInfo::m_questItems[i].m_choice.m_itemID = 0;
        CGQuestInfo::m_questItems[i].m_choice.unk_0004 = 0;
        CGQuestInfo::m_questItems[i].m_choice.unk_0008 = 1;
    }

    i = 0;

    for (; i < numRewards; i++) {
        CGQuestInfo::m_questItems[i].m_reward.m_itemID = rewardItems[i];
        CGQuestInfo::m_questItems[i].m_reward.unk_0004 = rewardDisplays[i];
        CGQuestInfo::m_questItems[i].m_reward.unk_0008 = rewardCounts[i];
    }

    for (; i < 6; i++) {
        CGQuestInfo::m_questItems[i].m_reward.m_itemID = 0;
        CGQuestInfo::m_questItems[i].m_reward.unk_0004 = 0;
        CGQuestInfo::m_questItems[i].m_reward.unk_0008 = 1;
    }

    if (title) {
        SStrCopy(CGQuestInfo::m_questTitle, *title ? title : " ", sizeof(CGQuestInfo::m_questTitle));
    }

    CGQuestInfo::m_autoLaunched = autoLaunched;

    if (money < 0) {
        CGQuestInfo::m_rewardMoney = 0;
        CGQuestInfo::m_requiredMoney = -money;
    } else {
        CGQuestInfo::m_rewardMoney = money;
        CGQuestInfo::m_requiredMoney = 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) < static_cast<int32_t>(player->m_player->m_playerFieldMaxLevel)) {
        CGQuestInfo::m_rewardXP = xp;
    } else {
        CGQuestInfo::m_rewardXP = 0;
    }

    CGQuestInfo::m_rewardHonorMultiplier = honorMultiplier;
    CGQuestInfo::m_rewardSpell = spell;
    CGQuestInfo::m_rewardHonor = honor;
    CGQuestInfo::m_rewardTalents = talents;
    CGQuestInfo::m_rewardSpellCast = spellCast;
    CGQuestInfo::m_rewardTitle = rewardTitle;
    CGQuestInfo::m_rewardArenaPoints = arenaPoints;
    CGQuestInfo::m_rewardFactionMask = factionMask;

    for (uint32_t j = 0; j < 5; j++) {
        CGQuestInfo::m_rewardFactionIDs[j] = factionIDs[j];
    }

    for (uint32_t j = 0; j < 5; j++) {
        CGQuestInfo::m_rewardFactionValues[j] = factionValues[j];
    }

    for (uint32_t j = 0; j < 5; j++) {
        CGQuestInfo::m_rewardFactionOverrides[j] = factionOverrides[j];
    }

    CGQuestInfo::ProcessQuestLogRewardFactions();
    CGQuestInfo::m_suggestedGroupNum = suggestedGroupNum;
    CGQuestInfo::m_questFlags = questFlags;

    if (CGQuestInfo::m_state == 1) {
        FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_DETAIL, nullptr);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_COMPLETE, nullptr);
    }
}

// VERIFY: c5b8f724 auto
// OFFSET: 0x0058BB60
void CGQuestInfo::InitRewRepFields(uint32_t index, int32_t faction, int32_t value, int32_t override) {
    if (index <= 4) {
        CGQuestInfo::m_rewardFactionIDs[index] = faction;
        CGQuestInfo::m_rewardFactionValues[index] = value;
        CGQuestInfo::m_rewardFactionOverrides[index] = override;
    }
}

// VERIFY: 12701ac3 auto
// OFFSET: 0x0058BB90
void CGQuestInfo::InitRewRepShowMask(uint32_t mask) {
    CGQuestInfo::m_rewardFactionMask = mask;
}
