#include "StableInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// VERIFY: da4c52c5 auto
// OFFSET: 0x005A1ED0
void CGStableInfo::InitializeGame() {
    ClientServices::SetMessageHandler(MSG_LIST_STABLED_PETS, Packet_MSG_LIST_STABLED_PETS, nullptr);
    ClientServices::SetMessageHandler(SMSG_STABLE_RESULT, Packet_SMSG_STABLE_RESULT, nullptr);
}
