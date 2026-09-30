#include "QuestFrame.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

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
