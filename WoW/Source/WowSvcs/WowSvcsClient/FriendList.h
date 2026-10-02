#ifndef WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_FRIENDLIST_H
#define WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_FRIENDLIST_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"

class CDataStore;
struct lua_State;

struct FriendInfo {
    /* 0000 */ uint8_t m_connected;
    /* 0001 */ uint8_t m_flags;
    /* 0004 */ char* m_name;
    /* 0008 */ char m_notes[512];
    /* 0208 */ WOWGUID m_guid;
    /* 0210 */ uint32_t m_level;
    /* 0214 */ uint32_t m_class;
    /* 0218 */ uint32_t m_area;
    /* 021C */ uint8_t unk_021C;

    FriendInfo();
    ~FriendInfo();
};

struct IgnoreInfo {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint8_t m_flags;
};

class CGPlayer_C;

struct WhoResult {
    /* 0000 */ char m_name[48];
    /* 0030 */ char m_guild[96];
    /* 0090 */ int32_t m_level;
    /* 0094 */ int32_t m_race;
    /* 0098 */ int32_t m_class;
    /* 009C */ int32_t m_gender;
    /* 00A0 */ int32_t m_zone;
};

struct WhoSortField {
    /* 0000 */ int32_t m_field;
    /* 0004 */ int32_t unk_0004;
};

class FriendList {
    public:
    static void __stdcall AddFriend(const char* name, const char* note);
    static FriendList* s_friendList;
    static WhoSortField s_whoSort[7];
    static uint32_t s_numWhoResults;
    static uint32_t s_totalWhoResults;
    static WhoResult s_whoResults[50];
    static int32_t s_whoToUI;

    /* 0000 */ FriendInfo m_friends[100];
    /* D480 */ uint32_t m_pendingFriendNames;
    /* D484 */ uint32_t unk_D484;
    /* D488 */ uint32_t unk_D488;
    /* D48C */ uint32_t unk_D48C;
    /* D490 */ IgnoreInfo m_ignores[50];
    /* D7B0 */ uint32_t m_pendingIgnoreNames;
    /* D7B4 */ uint32_t unk_D7B4;
    /* D7B8 */ uint32_t unk_D7B8;
    /* D7BC */ uint32_t unk_D7BC;
    /* D7C0 */ IgnoreInfo m_mutes[50];
    /* DAE0 */ uint32_t m_pendingMuteNames;
    /* DAE4 */ uint32_t unk_DAE4;
    /* DAE8 */ uint32_t unk_DAE8;
    /* DAEC */ uint32_t unk_DAEC;
    /* DAF0 */ uint64_t m_complained[32];
    /* DBF0 */ int32_t m_whoMinLevel;
    /* DBF4 */ int32_t m_whoMaxLevel;
    /* DBF8 */ char m_whoName[48];
    /* DC28 */ char m_whoGuild[96];
    /* DC88 */ int32_t m_whoRaceMask;
    /* DC8C */ int32_t m_whoClassMask;
    /* DC90 */ int32_t m_whoStringCount;
    /* DC94 */ char m_whoStrings[10][128];
    /* E094 */ uint8_t unk_E094[260];

    FriendList();
    ~FriendList();

    FriendInfo* GetFriend(WOWGUID guid);
    void SendWho(const char* filter);
    int32_t DoLocalWho();
    bool AllMatchFilter(CGPlayer_C* player);
    static bool __stdcall MatchesFilter(CGPlayer_C* player, const char* filter);
    FriendInfo* ClearRecruitAFriend(WOWGUID guid);
    uint32_t GetNumFriends();
    uint32_t GetNumIgnores();

    static void Destroy();
    static void Initialize();
    static void RegisterScriptFunctions();
    static void UnregisterScriptFunctions();

    void ReadPacket(CDataStore* msg);
    bool IsIgnored(WOWGUID guid, bool a2);
    bool IsMuted(WOWGUID guid);
    bool IsIgnoredOrMuted(WOWGUID guid);
    bool IsComplained(WOWGUID guid);
    void AddComplained(WOWGUID guid);
    bool IsFriend(WOWGUID guid);
};

int32_t QSortFriends(const void* a, const void* b);
int32_t QSortIgnore(const void* a, const void* b);
void FriendListNameCallbackWithSort(int32_t id, const WOWGUID& guid, void* param, bool success);
void FriendListIgnoreNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void FriendListMuteNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void FriendListModuleLoaded();
void LoadEncryptedModule(const uint8_t* key, uint32_t length, void (*callback)());
int32_t Packet_SMSG_WHO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_WHOIS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_RWHOIS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_CONTACT_LIST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_FRIEND_STATUS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t CCommand_Whois(const char* command, const char* arguments);
int32_t Script_GetFriendInfo(lua_State* L);
int32_t Script_SetSelectedFriend(lua_State* L);
int32_t Script_GetSelectedFriend(lua_State* L);
int32_t Script_AddOrRemoveFriend(lua_State* L);
int32_t Script_RemoveFriend(lua_State* L);
int32_t Script_SetFriendNotes(lua_State* L);
int32_t Script_GetIgnoreName(lua_State* L);
int32_t Script_SetSelectedIgnore(lua_State* L);
int32_t Script_GetSelectedIgnore(lua_State* L);
int32_t Script_AddOrDelIgnore(lua_State* L);
int32_t Script_AddIgnore(lua_State* L);
int32_t Script_DelIgnore(lua_State* L);
int32_t Script_GetNumMutes(lua_State* L);
int32_t Script_GetMuteName(lua_State* L);
int32_t Script_SetSelectedMute(lua_State* L);
int32_t Script_GetSelectedMute(lua_State* L);
int32_t Script_AddOrDelMute(lua_State* L);
int32_t Script_AddMute(lua_State* L);
int32_t Script_DelMute(lua_State* L);
int32_t Script_IsIgnored(lua_State* L);
int32_t Script_IsMuted(lua_State* L);
int32_t Script_IsIgnoredOrMuted(lua_State* L);
int32_t Script_SortWho(lua_State* L);

#endif
