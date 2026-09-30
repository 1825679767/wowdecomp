#include "GossipInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00C016F8
uint32_t CGGossipInfo::unk_C016F8;

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

// VERIFY: a0913fd6 manual
// OFFSET: 0x0058A550
void CGGossipInfo::SetGossip(const WOWGUID& guid) {
    if (guid == CGGossipInfo::m_gossip) {
        return;
    }

    if (guid) {
        CGGameUI::SetInteractTarget(guid);
        CGGossipInfo::m_gossip = guid;
        return;
    }

    if (!CGQuestInfo::m_npc) {
        CGGameUI::ClearInteractTarget(CGGossipInfo::m_gossip, 0);
    }

    CGGossipInfo::m_gossip = WOWGUID();
    CGGossipInfo::unk_C016F8 = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_GOSSIP_CLOSED, nullptr);
}
