#include "MailInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/StationeryRec.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/Rec/PackageRec.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Os/Time.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"

// STATIC: 0x00BEB210
static char s_stationeryTexture[260];

// STATIC: 0x00BEB318
static char s_packageTexture[260];

// VERIFY: 27fef505 manual
// OFFSET: 0x0056D5D0
static int32_t Script_GetSendMailMoney(lua_State* L) {
    lua_pushnumber(L, CGMailInfo::m_sendMoney);
    return 1;
}

// VERIFY: 8948f687 manual
// OFFSET: 0x0056D690
static int32_t Script_GetSendMailCOD(lua_State* L) {
    lua_pushnumber(L, CGMailInfo::m_sendCOD);
    return 1;
}

// VERIFY: 8c50aec0 manual
// OFFSET: 0x0056F350
static int32_t Script_HasNewMail(lua_State* L) {
    if (fabsf(CGMailInfo::m_nextMailTime) >= 2.3841858e-7f) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// VERIFY: d43f229a manual
// OFFSET: 0x0056D6D0
static int32_t Script_GetInboxNumItems(lua_State* L) {
    lua_pushnumber(L, CGMailInfo::m_numInboxItems);
    lua_pushnumber(L, CGMailInfo::m_totalInboxItems);
    return 2;
}

// VERIFY: 54d478fd auto
// OFFSET: 0x0056E170
static int32_t Script_GetSelectedStationeryTexture(lua_State* L) {
    const StationeryRec* rec;

    if (CGMailInfo::m_selectedStationery && (rec = g_stationeryDB.GetRecord(CGMailInfo::m_selectedStationery))) {
        lua_pushstring(L, rec->m_texture);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: ab286db5 manual
// OFFSET: 0x0056E310
static int32_t Script_SelectPackage(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SelectPackage(index)");
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    const PackageRec* rec;

    if (index >= 0 && index < g_packageDB.m_numRecords && (rec = &g_packageDB.m_records[index])) {
        CGMailInfo::m_selectedPackage = rec->m_id;
        return 0;
    }

    CGMailInfo::m_selectedPackage = 0;
    return 0;
}

// VERIFY: 699c569f manual
// OFFSET: 0x0056E440
static int32_t Script_CheckInbox(lua_State* L) {
    if (CGMailInfo::m_commandPending) {
        return 0;
    }

    WOWGUID object = CGMailInfo::m_object;

    if (!object) {
        return 0;
    }

    uint32_t now = OsGetAsyncTimeMs();

    if (CGMailInfo::m_nextUpdateTime && static_cast<int32_t>(now - CGMailInfo::m_nextUpdateTime) < 0) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_INBOX_UPDATE, nullptr);
        return 0;
    }

    CGMailInfo::m_nextUpdateTime = now + 60000;
    CGMailInfo::m_commandPending = 1;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_MAIL_GET_LIST));
    msg.Put(object.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: ff00a2b9 manual
// OFFSET: 0x0056EDF0
static int32_t Script_TakeInboxMoney(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: TakeInboxMoney(index)");
    }

    if (!CGMailInfo::m_commandPending) {
        WOWGUID object = CGMailInfo::m_object;

        if (object) {
            uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

            if (index < CGMailInfo::m_numInboxItems) {
                MailboxMessageInfo* mail = CGMailInfo::m_inbox.m_data[index];

                if (mail && mail->m_money) {
                    CDataStore msg;
                    msg.Put(static_cast<uint32_t>(CMSG_MAIL_TAKE_MONEY));
                    msg.Put(object.m_guid);
                    msg.Put(static_cast<uint32_t>(mail->m_messageID));
                    msg.Finalize();
                    ClientServices::Send2(&msg);
                    CGMailInfo::m_commandPending = 1;
                }
            }
        }
    }

    return 0;
}

// VERIFY: 9c8dcaa5 manual
// OFFSET: 0x0056EEF0
static int32_t Script_TakeInboxTextItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: TakeInboxTextItem(index)");
    }

    if (!CGMailInfo::m_commandPending) {
        WOWGUID object = CGMailInfo::m_object;

        if (object) {
            uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

            if (index < CGMailInfo::m_numInboxItems) {
                MailboxMessageInfo* mail = CGMailInfo::m_inbox.m_data[index];

                if (mail && (SStrLen(mail->m_body) || mail->m_mailTemplateID) && !(mail->m_flags & 0x4)) {
                    CDataStore msg;
                    msg.Put(static_cast<uint32_t>(CMSG_MAIL_CREATE_TEXT_ITEM));
                    msg.Put(object.m_guid);
                    msg.Put(static_cast<uint32_t>(mail->m_messageID));
                    msg.Finalize();
                    ClientServices::Send2(&msg);
                    CGMailInfo::m_commandPending = 1;
                }
            }
        }
    }

    return 0;
}

// VERIFY: 5e8e6e9d manual
// OFFSET: 0x0056E0E0
static int32_t Script_SelectStationery(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SelectStationery(index)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGMailInfo::m_stationeries.Count()) {
        CGMailInfo::m_selectedStationery = 0;
    } else {
        CGMailInfo::m_selectedStationery = CGMailInfo::m_stationeries.m_data[index]->m_id;
    }

    return 0;
}

// VERIFY: f642dbb8 manual
// OFFSET: 0x0056DF80
static int32_t Script_GetStationeryInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetStationeryInfo(index)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    StationeryInfo* stationery;
    ItemStats_C* stats;
    WOWGUID none = { 0 };

    if (index < CGMailInfo::m_stationeries.Count() && (stationery = CGMailInfo::m_stationeries.m_data[index]) && (stats = g_itemDBCache.GetRecord(stationery->m_itemID, none, nullptr, nullptr, false))) {
        const char* path = ClientDBStringLookup(3);
        SStrPrintf(s_stationeryTexture, sizeof(s_stationeryTexture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(stats->m_displayId));
        lua_pushstring(L, stats->m_name[0]);
        lua_pushstring(L, s_stationeryTexture);

        if (stationery->unk_0010) {
            lua_pushnil(L);
        } else {
            lua_pushnumber(L, stats->m_buyPrice);
        }

        return 3;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 3;
}

// VERIFY: 37acfbbe auto
// OFFSET: 0x0056D570
static int32_t Script_SetSendMailShowing(lua_State* L) {
    if (lua_type(L, 1) != LUA_TBOOLEAN) {
        luaL_error(L, "Usage: SetSendMailShowing(bool)");
    }

    CGMailInfo::m_sendMailShowing = lua_toboolean(L, 1);
    return 0;
}

// VERIFY: 14637408 manual
// OFFSET: 0x0056E210
static int32_t Script_GetPackageInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPackageInfo(index)");
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    PackageRec* rec = index >= 0 && index < g_packageDB.m_numRecords ? &g_packageDB.m_records[index] : nullptr;
    double cost;

    if (rec) {
        const char* path = ClientDBStringLookup(3);
        SStrPrintf(s_packageTexture, sizeof(s_packageTexture), "%s%s", path, *path ? "\\" : "");
        SStrPack(s_packageTexture, rec->m_icon, sizeof(s_packageTexture));
        lua_pushstring(L, rec->m_name);
        lua_pushstring(L, s_packageTexture);
        cost = rec->m_cost;
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        cost = 0.0;
    }

    lua_pushnumber(L, cost);
    return 3;
}

// VERIFY: 4291f14f auto
// OFFSET: 0x0056E1E0
static int32_t Script_GetNumPackages(lua_State* L) {
    lua_pushnumber(L, g_packageDB.m_numRecords);
    return 1;
}

// STATIC: 0x00ACE610
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseMail", &Script_CloseMail },
    { "ClearSendMail", &Script_ClearSendMail },
    { "ClickSendMailItemButton", &Script_ClickSendMailItemButton },
    { "SetSendMailMoney", &Script_SetSendMailMoney },
    { "GetSendMailMoney", &Script_GetSendMailMoney },
    { "SetSendMailCOD", &Script_SetSendMailCOD },
    { "GetSendMailCOD", &Script_GetSendMailCOD },
    { "GetNumStationeries", &Script_GetNumStationeries },
    { "GetStationeryInfo", &Script_GetStationeryInfo },
    { "SelectStationery", &Script_SelectStationery },
    { "GetSelectedStationeryTexture", &Script_GetSelectedStationeryTexture },
    { "GetNumPackages", &Script_GetNumPackages },
    { "GetPackageInfo", &Script_GetPackageInfo },
    { "SelectPackage", &Script_SelectPackage },
    { "GetSendMailItem", &Script_GetSendMailItem },
    { "GetSendMailItemLink", &Script_GetSendMailItemLink },
    { "GetSendMailPrice", &Script_GetSendMailPrice },
    { "SendMail", &Script_SendMail },
    { "CheckInbox", &Script_CheckInbox },
    { "GetInboxNumItems", &Script_GetInboxNumItems },
    { "GetInboxHeaderInfo", &Script_GetInboxHeaderInfo },
    { "GetInboxText", &Script_GetInboxText },
    { "GetInboxInvoiceInfo", &Script_GetInboxInvoiceInfo },
    { "GetInboxItem", &Script_GetInboxItem },
    { "GetInboxItemLink", &Script_GetInboxItemLink },
    { "TakeInboxMoney", &Script_TakeInboxMoney },
    { "TakeInboxItem", &Script_TakeInboxItem },
    { "TakeInboxTextItem", &Script_TakeInboxTextItem },
    { "ReturnInboxItem", &Script_ReturnInboxItem },
    { "DeleteInboxItem", &Script_DeleteInboxItem },
    { "InboxItemCanDelete", &Script_InboxItemCanDelete },
    { "HasNewMail", &Script_HasNewMail },
    { "ComplainInboxItem", &Script_ComplainInboxItem },
    { "CanComplainInboxItem", &Script_CanComplainInboxItem },
    { "GetLatestThreeSenders", &Script_GetLatestThreeSenders },
    { "SetSendMailShowing", &Script_SetSendMailShowing },
    { "AutoLootMailItem", &Script_AutoLootMailItem },
    { "RespondMailLockSendItem", &Script_RespondMailLockSendItem },
};

// VERIFY: d763d4b6 auto
// OFFSET: 0x0056D730
void MailInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
