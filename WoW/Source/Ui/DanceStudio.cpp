#include "DanceStudio.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// VERIFY: 166fac44 auto
// OFFSET: 0x00576A90
void CGDanceStudio::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_PLAY_DANCE, Packet_SMSG_PLAY_DANCE, nullptr);
    ClientServices::SetMessageHandler(SMSG_STOP_DANCE, Packet_SMSG_STOP_DANCE, nullptr);
    ClientServices::SetMessageHandler(SMSG_NOTIFY_DANCE, Packet_SMSG_NOTIFY_DANCE, nullptr);
    ClientServices::SetMessageHandler(SMSG_LEARNED_DANCE_MOVES, Packet_SMSG_LEARNED_DANCE_MOVES, nullptr);
}
