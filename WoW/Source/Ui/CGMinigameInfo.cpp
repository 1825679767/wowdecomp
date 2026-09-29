#include "CGMinigameInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// VERIFY: 7481e654 auto
// OFFSET: 0x005C5720
void CGMinigameInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_MINIGAME_SETUP, Packet_SMSG_MINIGAME_SETUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_MINIGAME_STATE, Packet_SMSG_MINIGAME_STATE, nullptr);
}
