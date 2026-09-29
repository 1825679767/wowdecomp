#include "QuestLog.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C23AD0
uint32_t CGQuestLog::m_numQuest;

// STATIC: 0x00C23AF4
static uint32_t s_questCallbackCount;

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
