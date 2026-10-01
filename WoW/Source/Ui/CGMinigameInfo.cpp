#include "CGMinigameInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// VERIFY: 7481e654 auto
// OFFSET: 0x005C5720
void CGMinigameInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_MINIGAME_SETUP, Packet_SMSG_MINIGAME_SETUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_MINIGAME_STATE, Packet_SMSG_MINIGAME_STATE, nullptr);
}

// VERIFY: 8dc40695 auto
// OFFSET: 0x005C54B0
void CGMinigameInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_MINIGAME_SETUP);
    ClientServices::ClearMessageHandler(SMSG_MINIGAME_STATE);
}
