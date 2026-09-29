#include "DuelInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C22B60
WOWGUID CGDuelInfo::m_dueler;

// VERIFY: 7eda6e31 auto
// OFFSET: 0x005CFDD0
void CGDuelInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_DUEL_REQUESTED, Packet_SMSG_DUEL_REQUESTED, nullptr);
    ClientServices::SetMessageHandler(SMSG_DUEL_OUT_OF_BOUNDS, Packet_SMSG_DUEL_OUTOFBOUNDS, nullptr);
    ClientServices::SetMessageHandler(SMSG_DUEL_IN_BOUNDS, Packet_SMSG_DUEL_INBOUNDS, nullptr);
    ClientServices::SetMessageHandler(SMSG_DUEL_COUNTDOWN, Packet_SMSG_DUEL_COUNTDOWN, nullptr);
    ClientServices::SetMessageHandler(SMSG_DUEL_COMPLETE, Packet_SMSG_DUEL_COMPLETE, nullptr);
    ClientServices::SetMessageHandler(SMSG_DUEL_WINNER, Packet_SMSG_DUEL_WINNER, nullptr);
}
