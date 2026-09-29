#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_TRADE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_TRADE_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/ObjectClient/Object_C.h"

extern uint32_t s_clientStateIndex;

bool Trade_C_AddItem(WOWGUID item, WOWGUID bag, uint8_t slot, uint8_t tradeSlot);
void Trade_C_BeginTrade();
void Trade_C_CancelTrade();
void Trade_C_Destroy();
void Trade_C_PlayerBusy();
void Trade_C_PlayerIgnored();
void Trade_C_UnacceptTrade();

#endif
