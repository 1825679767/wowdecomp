#include "GMTicketInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"

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

// VERIFY: 96fb670a manual
// OFFSET: 0x005AD240
int32_t Packet_Group_5(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    switch (msgId) {
        case SMSG_GM_TICKET_CREATE:
        case SMSG_GM_TICKET_UPDATE_TEXT: {
            uint32_t result;
            msg->Get(result);

            switch (result) {
                case 1:
                    CGGameUI::DisplayError(ERR_TICKET_ALREADY_EXISTS);
                    break;

                case 2:
                case 4:
                    SendGetTicketOpcode();
                    break;

                case 3:
                    CGGameUI::DisplayError(ERR_TICKET_CREATE_ERROR);
                    break;

                case 5:
                    CGGameUI::DisplayError(ERR_TICKET_UPDATE_ERROR);
                    break;
            }

            break;
        }

        case SMSG_GM_TICKET_GET_TICKET: {
            uint32_t status;
            msg->Get(status);

            if (status != 6) {
                if (!status) {
                    CGGameUI::DisplayError(ERR_TICKET_DB_ERROR);
                }

                FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TICKET, nullptr);
                break;
            }

            CGGMTicketInfo::m_responseID = 0;
            msg->Get(reinterpret_cast<uint32_t&>(CGGMTicketInfo::m_ticketID));
            char text[2000];
            msg->GetString(text, sizeof(text));
            uint8_t category;
            msg->Get(category);
            float ticketAge;
            msg->Get(ticketAge);

            if (ticketAge < 0.0f) {
                ticketAge = 0.0f;
            }

            float oldestTicketAge;
            msg->Get(oldestTicketAge);
            float lastUpdate;
            msg->Get(lastUpdate);
            uint8_t assigned;
            msg->Get(assigned);
            uint8_t opened;
            msg->Get(opened);
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TICKET, "%d%s%f%f%f%d%d", category, text, ticketAge, oldestTicketAge, lastUpdate, assigned, opened);
            break;
        }

        case SMSG_GM_TICKET_DELETE_TICKET: {
            uint32_t result;
            msg->Get(result);

            if (result == 9) {
                CGGMTicketInfo::m_ticketID = 0;
                CGGMTicketInfo::m_responseID = 0;
                FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TICKET, nullptr);
            } else {
                CGGameUI::DisplayError(ERR_TICKET_DB_ERROR);
            }

            break;
        }

        case SMSG_GM_TICKET_STATUS_UPDATE: {
            uint32_t status;
            msg->Get(status);

            switch (status) {
                case 1:
                case 4:
                    SendGetTicketOpcode();
                    break;

                case 2:
                    CGGMTicketInfo::m_ticketID = 0;
                    CGGMTicketInfo::m_responseID = 0;
                    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TICKET, nullptr);
                    break;

                case 3:
                    CGGMTicketInfo::m_ticketID = 0;
                    CGGMTicketInfo::m_responseID = 0;
                    FrameScript_SignalEvent(SCRIPT_EVENT_GMSURVEY_DISPLAY, nullptr);
                    break;
            }

            break;
        }

        case SMSG_GMRESPONSE_DB_ERROR:
            CGGameUI::DisplayError(ERR_GMRESPONSE_DB_ERROR);
            break;

        case SMSG_GMRESPONSE_RECEIVED: {
            msg->Get(reinterpret_cast<uint32_t&>(CGGMTicketInfo::m_responseID));
            msg->Get(reinterpret_cast<uint32_t&>(CGGMTicketInfo::m_ticketID));
            char text[2000];
            msg->GetString(text, sizeof(text));
            char response[16000] = { 0 };

            for (int32_t i = 0; i < 4; i++) {
                char line[4000];
                msg->GetString(line, sizeof(line));
                SStrPack(response, line, 0x7FFFFFFF);
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_GMRESPONSE_RECEIVED, "%s%s", text, response);
            break;
        }

        case SMSG_GMRESPONSE_STATUS_UPDATE: {
            CGGMTicketInfo::m_ticketID = 0;
            CGGMTicketInfo::m_responseID = 0;
            uint8_t showSurvey = 0;
            msg->Get(showSurvey);

            if (showSurvey) {
                FrameScript_SignalEvent(SCRIPT_EVENT_GMSURVEY_DISPLAY, nullptr);
            } else {
                FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TICKET, nullptr);
            }

            break;
        }
    }

    return 1;
}

// VERIFY: 0cce60c6 auto
// OFFSET: 0x005AC100
void CGGMTicketInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_GM_TICKET_CREATE);
    ClientServices::ClearMessageHandler(SMSG_GM_TICKET_UPDATE_TEXT);
    ClientServices::ClearMessageHandler(SMSG_GM_TICKET_GET_TICKET);
    ClientServices::ClearMessageHandler(SMSG_GM_TICKET_DELETE_TICKET);
    ClientServices::ClearMessageHandler(SMSG_GMRESPONSE_DB_ERROR);
    ClientServices::ClearMessageHandler(SMSG_GMRESPONSE_RECEIVED);
    ClientServices::ClearMessageHandler(SMSG_GMRESPONSE_STATUS_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_GM_TICKET_STATUS_UPDATE);
}
