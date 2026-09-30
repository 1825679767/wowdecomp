#include "GossipInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C016F0
WOWGUID CGGossipInfo::m_gossip;

// STATIC: 0x00C016FC
uint32_t CGGossipInfo::m_gossipOptionsCount;

// STATIC: 0x00C00BF0
GossipOption CGGossipInfo::m_options[32];

// STATIC: 0x00C00EF0
char CGGossipInfo::m_gossipText[2048];

// VERIFY: ac8f3a04 auto
// OFFSET: 0x0058B6E0
void CGGossipInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_GOSSIP_MESSAGE, Packet_SMSG_GOSSIP_MESSAGE, nullptr);
    ClientServices::SetMessageHandler(SMSG_GOSSIP_COMPLETE, Packet_SMSG_GOSSIP_COMPLETE, nullptr);
    ClientServices::SetMessageHandler(SMSG_GOSSIP_POI, Packet_SMSG_GOSSIP_POI, nullptr);
}
