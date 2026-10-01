#ifndef WOW_SOURCE_UI_BARBERSHOP_H
#define WOW_SOURCE_UI_BARBERSHOP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Ui/USEABLESTYLE.h"

struct lua_State;

class CDataStore;

int32_t Packet_SMSG_ENABLE_BARBER_SHOP(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_BARBER_SHOP_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

class CGBarberShop {
    public:
    static void EnterWorld();
    static STYLEREC* GetStyle(int32_t type, int32_t race, int32_t sex, int32_t data);
    static float GetMinCameraDistance();
    static int32_t m_barberShopEnabled;
    static float m_minCameraDistance;
    static uint8_t m_resetPortrait;
    static STYLEREC* m_currentHairStyle;
    static STYLEREC* m_currentFacialHair;
    static STYLEREC* m_currentSkinColor;
    static TSHashTable<USEABLESTYLE, HASHKEY_NONE> m_useableStyles;
    static uint8_t m_initialized;

    static void AppearanceApplied();
    static void DisableBarberShop();
    static void InitializeGame();
    static void RegisterScriptFunctions();
};

int32_t Script_GetBarberShopStyleInfo(lua_State* L);
int32_t Script_SetNextBarberShopStyle(lua_State* L);
int32_t Script_GetBarberShopTotalCost(lua_State* L);
int32_t Script_ApplyBarberShopStyle(lua_State* L);
int32_t Script_CancelBarberShop(lua_State* L);
int32_t Script_GetHairCustomization(lua_State* L);
int32_t Script_GetFacialHairCustomization(lua_State* L);
int32_t Script_BarberShopReset(lua_State* L);

#endif
