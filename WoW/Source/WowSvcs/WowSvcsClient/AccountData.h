#ifndef WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_ACCOUNTDATA_H
#define WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_ACCOUNTDATA_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"

class CDataStore;
struct CVar;

extern CVar* s_cvSynchronizeSettings;

void AccountDataInitializeBasicSystem();
void AccountDataShutdownBasicSystem();
int32_t Packet_SMSG_ACCOUNT_DATA_TIMES(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_CHECK_FOR_BOTS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_UPDATE_ACCOUNT_DATA(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
