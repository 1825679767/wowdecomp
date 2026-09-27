#include "AccountData.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// VERIFY: 72a9a2be auto
// OFFSET: 0x006B8FC0
static int32_t Packet_SMSG_UPDATE_ACCOUNT_DATA_COMPLETE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t type;
    msg->Get(type);

    uint32_t result;
    msg->Get(result);

    return 1;
}

// VERIFY: d11d72c6 auto
// OFFSET: 0x006B9900
void AccountDataInitializeBasicSystem() {
    s_cvSynchronizeSettings = CVar::Register("synchronizeSettings", "Whether client settings should be stored on the server", 0, "1", nullptr, CATEGORY_DEFAULT, false, nullptr, false);
    ClientServices::SetMessageHandler(SMSG_ACCOUNT_DATA_TIMES, Packet_SMSG_ACCOUNT_DATA_TIMES, nullptr);
    ClientServices::SetMessageHandler(SMSG_UPDATE_ACCOUNT_DATA, Packet_SMSG_UPDATE_ACCOUNT_DATA, nullptr);
    ClientServices::SetMessageHandler(SMSG_UPDATE_ACCOUNT_DATA_COMPLETE, Packet_SMSG_UPDATE_ACCOUNT_DATA_COMPLETE, nullptr);
    ClientServices::SetMessageHandler(SMSG_CHECK_FOR_BOTS, Packet_SMSG_CHECK_FOR_BOTS, nullptr);
}

// OFFSET: 0x006B8FF0
void AccountDataShutdownBasicSystem() {
    ClientServices::ClearMessageHandler(SMSG_ACCOUNT_DATA_TIMES);
    ClientServices::ClearMessageHandler(SMSG_UPDATE_ACCOUNT_DATA);
    ClientServices::ClearMessageHandler(SMSG_UPDATE_ACCOUNT_DATA_COMPLETE);
    ClientServices::ClearMessageHandler(SMSG_CHECK_FOR_BOTS);
}
