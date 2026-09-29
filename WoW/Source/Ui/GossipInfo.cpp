#include "GossipInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// VERIFY: ac8f3a04 auto
// OFFSET: 0x0058B6E0
void CGGossipInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_GOSSIP_MESSAGE, Packet_SMSG_GOSSIP_MESSAGE, nullptr);
    ClientServices::SetMessageHandler(SMSG_GOSSIP_COMPLETE, Packet_SMSG_GOSSIP_COMPLETE, nullptr);
    ClientServices::SetMessageHandler(SMSG_GOSSIP_POI, Packet_SMSG_GOSSIP_POI, nullptr);
}
