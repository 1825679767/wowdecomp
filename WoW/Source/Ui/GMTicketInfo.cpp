#include "GMTicketInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C1E8D0
int32_t CGGMTicketInfo::m_ticketID;

// STATIC: 0x00C1E8D4
int32_t CGGMTicketInfo::m_responseID;

// VERIFY: 681f5140 auto
// OFFSET: 0x005AC160
void CGGMTicketInfo::EnterWorld() {
    CGGMTicketInfo::m_ticketID = 0;
    CGGMTicketInfo::m_responseID = 0;
}

// VERIFY: a180cc26 auto
// OFFSET: 0x005AD600
void CGGMTicketInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_CREATE, Packet_Group_5, nullptr);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_UPDATE_TEXT, Packet_Group_5, nullptr);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_GET_TICKET, Packet_Group_5, nullptr);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_DELETE_TICKET, Packet_Group_5, nullptr);
    ClientServices::SetMessageHandler(SMSG_GMRESPONSE_DB_ERROR, Packet_Group_5, nullptr);
    ClientServices::SetMessageHandler(SMSG_GMRESPONSE_RECEIVED, Packet_Group_5, nullptr);
    ClientServices::SetMessageHandler(SMSG_GMRESPONSE_STATUS_UPDATE, Packet_Group_5, nullptr);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_STATUS_UPDATE, Packet_Group_5, nullptr);
}

// VERIFY: 4c103e06 manual
// OFFSET: 0x005ACBF0
void SendGetTicketOpcode() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_TICKET_GET_TICKET));
    msg.Finalize();
    ClientServices::Send2(&msg);
}
