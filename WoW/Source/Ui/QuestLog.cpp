#include "QuestLog.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MinimapFrame.h"

// STATIC: 0x00C23AD0
int32_t CGQuestLog::m_numQuest;

// STATIC: 0x00C23AF4
static uint32_t s_questCallbackCount;

// STATIC: 0x00AD1018
float CGQuestLog::m_questIconOverlapPushDist = 18.0f;

// STATIC: 0x00C23AD8
int32_t CGQuestLog::m_selectedQuest;

// STATIC: 0x00AD1014
float CGQuestLog::m_questIconOverlapDist = 12.0f;

// STATIC: 0x00A1C4A0
static const int32_t s_questGreenRange[20] = { 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

// STATIC: 0x00C23680
QuestWatch CGQuestLog::m_questWatches[25];

// STATIC: 0x00C237B0
QuestLogEntry CGQuestLog::m_quests[50];

// STATIC: 0x00C23AE4
uint32_t CGQuestLog::m_numShownQuests;

// STATIC: 0x00C23ADC
int32_t CGQuestLog::m_abandonQuest;

// STATIC: 0x00C23AE0
uint32_t CGQuestLog::m_numQuestWatches;

// STATIC: 0x00C23AE8
int32_t CGQuestLog::m_timeOffset;

// STATIC: 0x00C23AF8
int32_t CGQuestLog::m_nextQuestUpdate;

// STATIC: 0x00C23AFC
static CVar* s_cvPOIShiftComplete;

// STATIC: 0x00C23B00
QuestPOIInfo CGQuestLog::m_questPOIs[25];

// VERIFY: 45bd143e manual
// OFFSET: 0x005E6FA0
void CGQuestLog::EnterWorld() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUERY_TIME));
    msg.Finalize();
    ClientServices::Send2(&msg);
    s_questCallbackCount = 0;
    CGQuestLog::Update(1);
}

// VERIFY: ff43712e manual
// OFFSET: 0x005E2860
int32_t CGQuestLog::GetQuestGreenRange() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t index = static_cast<int32_t>(player->m_unit->m_unitFieldLevel) / 5;

    if (index >= 20) {
        index = 19;
    }

    return s_questGreenRange[index];
}

// VERIFY: 673a158b manual
// OFFSET: 0x005DEC40
int32_t CGQuestLog::GetQuestAt(int32_t index) {
    if (index < 0 || index >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    return CGQuestLog::m_quests[index].m_questId;
}

// VERIFY: 5dca5cb5 manual
// OFFSET: 0x005E71A0
void CGQuestLog::InitializeGame() {
    CGQuestLog::m_selectedQuest = 0;
    CGQuestLog::m_abandonQuest = 0;
    CGQuestLog::m_numQuest = 0;
    CGQuestLog::m_numShownQuests = 0;
    memset(CGQuestLog::m_quests, 0, sizeof(CGQuestLog::m_quests));
    CGQuestLog::m_numQuestWatches = 0;
    memset(CGQuestLog::m_questWatches, 0, sizeof(CGQuestLog::m_questWatches));
    CGQuestLog::m_timeOffset = 0;
    CGQuestLog::m_nextQuestUpdate = 0;

    for (uint32_t i = 0; i < 25; i++) {
        QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[i];
        poi->m_questId = 0;
        poi->unk_0014 = 0;
        poi->unk_0018 = 0;
        poi->unk_001C = 0;
        poi->unk_0020 = 0;
        poi->unk_0024 = 0;
        poi->unk_0029 = 0;
        poi->unk_0028 = 0;
        poi->unk_002C = 0;
    }

    CGxTexFlags flags(GxTex_Linear, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    flags.m_forceMipTracking = 1;
    flags.m_renderTarget = 1;
    ClientServices::SetMessageHandler(SMSG_QUERY_TIME_RESPONSE, Packet_SMSG_QUERY_TIME_RESPONSE, nullptr);
    s_cvPOIShiftComplete = CVar::Register("POIShiftComplete", nullptr, 0, "0.6", nullptr, 0, false, nullptr, false);
    ConsoleCommandRegister("UpdatePOI", UpdateQuestPOIHandler, CATEGORY_GAME, "Updates Quest POI from User Server");
}

// VERIFY: 988b76a4 auto
// OFFSET: 0x005DEBD0
int32_t CGQuestLog::GetSelectedLogEntry() {
    uint32_t index = 0;

    for (; index < static_cast<uint32_t>(CGQuestLog::m_numQuest); index++) {
        if (!CGQuestLog::m_quests[index].m_isHeader && CGQuestLog::m_quests[index].m_questId == CGQuestLog::m_selectedQuest) {
            break;
        }
    }

    if (index == static_cast<uint32_t>(CGQuestLog::m_numQuest)) {
        return -1;
    }

    return index;
}

// VERIFY: 98372535 manual
// OFFSET: 0x005DFFA0
void CGQuestLog::SetSelectedQuest(int32_t index) {
    if (CGQuestLog::m_selectedQuest) {
        DisplayQuestPoint(0.0f, 0.0f, 0, "");
    }

    if (index < 0 || index >= CGQuestLog::m_numQuest) {
        CGQuestLog::m_selectedQuest = 0;
    } else if (!CGQuestLog::m_quests[index].m_isHeader) {
        CGQuestLog::m_selectedQuest = CGQuestLog::m_quests[index].m_questId;
        AddQuestPointDisplay();
    }
}

// VERIFY: 4aaec04c manual
// OFFSET: 0x005E57E0
int32_t CGQuestLog::SetQuestExpired(uint32_t index) {
    if (index >= static_cast<uint32_t>(CGQuestLog::m_numQuest) || CGQuestLog::IsQuestCompleteEx(index, 1)) {
        return 0;
    }

    CGQuestLog::m_quests[index].m_expired = 1;

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::QueryQuestGiverStatusMultiple();
    }

    return 1;
}

// VERIFY: 5dcc8e24 auto
// OFFSET: 0x005DE980
void DisplayQuestFailed(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        ConsoleWrite("Invalid quest log entry", DEFAULT_COLOR);
        return;
    }

    QuestCache* quest = g_questDBCache.GetRecord(id, WOWGUID(), nullptr, nullptr, false);

    if (quest) {
        CGGameUI::DisplayError(ERR_QUEST_FAILED_S, quest->m_name);
    }
}
