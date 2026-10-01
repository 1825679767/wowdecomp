#include "DanceStudio.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/Base/CDataStore.h"

// STATIC: 0x00BEB630
uint64_t CGDanceStudio::m_learnedDanceMoves;

// VERIFY: 166fac44 auto
// OFFSET: 0x00576A90
void CGDanceStudio::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_PLAY_DANCE, Packet_SMSG_PLAY_DANCE, nullptr);
    ClientServices::SetMessageHandler(SMSG_STOP_DANCE, Packet_SMSG_STOP_DANCE, nullptr);
    ClientServices::SetMessageHandler(SMSG_NOTIFY_DANCE, Packet_SMSG_NOTIFY_DANCE, nullptr);
    ClientServices::SetMessageHandler(SMSG_LEARNED_DANCE_MOVES, Packet_SMSG_LEARNED_DANCE_MOVES, nullptr);
}

// VERIFY: 4758a0e1 auto
// OFFSET: 0x005758A0
int32_t Packet_SMSG_LEARNED_DANCE_MOVES(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    msg->Get(CGDanceStudio::m_learnedDanceMoves);
    return 1;
}
