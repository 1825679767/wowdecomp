#ifndef WOW_SOURCE_UI_COMMENTATORFRAME_H
#define WOW_SOURCE_UI_COMMENTATORFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Net/Types.h"

class CDataStore;
struct lua_State;

class CGPlayer_C;

class CGCommentator {
    public:
    struct MapInfo {
        /* 0000 */ uint32_t unk_0000;
        /* 0004 */ uint32_t unk_0004;
        /* 0008 */ uint32_t unk_0008;
        /* 000C */ uint32_t unk_000C;
        /* 0010 */ uint32_t unk_0010;
        /* 0014 */ uint32_t unk_0014;
        /* 0018 */ uint32_t unk_0018;
        /* 001C */ uint32_t unk_001C;
    };

    struct QueueNode {
        /* 0000 */ WOWGUID unk_0000;
        /* 0008 */ WOWGUID unk_0008;
        /* 0010 */ int32_t unk_0010;
        /* 0014 */ int32_t unk_0014;
    };

    static void PostInit(CGPlayer_C* player);
    static CGCommentator s_Commentator;

    static void InitializeGame();
    static void RegisterScriptFunctions();

    /* 0000 */ float m_facing;
    /* 0004 */ float m_pitch;
    /* 0008 */ float m_yawVelocity;
    /* 000C */ C3Vector m_position;
    /* 0018 */ float m_forwardVelocity;
    /* 001C */ float m_strafeVelocity;
    /* 0020 */ float m_verticalVelocity;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ WOWGUID unk_0028;
    /* 0030 */ WOWGUID unk_0030;
    /* 0038 */ float m_distance;
    /* 003C */ float m_fov;
    /* 0040 */ float m_targetFov;
    /* 0044 */ float unk_0044;
    /* 0048 */ int32_t m_cameraCollision;
    /* 004C */ float m_targetHeightOffset;
    /* 0050 */ TSFixedArray<CGCommentator::MapInfo> m_maps;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C[3];
    /* 0078 */ WOWGUID m_battlemaster;
    /* 0080 */ TSFixedArray<CGCommentator::QueueNode> m_skirmishQueue;
    /* 008C */ uint32_t unk_008C;
    /* 0090 */ int32_t m_skirmishMode;

    static int32_t OnReceivePlayerInfo(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnReceiveMapInfo(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnStateChanged(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnSkirmishPlayersInQueue(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnSkirmishPlayersModeState(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

    WOWGUID GetArenaMember(uint32_t index);
    void Init();
    void OnUpdate(float elapsed);
    void UpdateCameraVelocity(uint32_t controlFlags, int32_t canControl);
};

class Commentator {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[4];
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[40];
    /* 0040 */ uint32_t unk_0040;
};

int32_t Script_CommentatorSetMapAndInstanceIndex(lua_State* L);
int32_t Script_CommentatorSetPlayerIndex(lua_State* L);
int32_t Script_CommentatorUpdatePlayerInfo(lua_State* L);
int32_t Script_CommentatorUpdateMapInfo(lua_State* L);
int32_t Script_CommentatorGetInstanceInfo(lua_State* L);
int32_t Script_CommentatorEnterInstance(lua_State* L);
int32_t Script_CommentatorExitInstance(lua_State* L);
int32_t Script_CommentatorGetNumPlayers(lua_State* L);
int32_t Script_CommentatorGetPlayerInfo(lua_State* L);
int32_t Script_CommentatorFollowPlayer(lua_State* L);
int32_t Script_CommentatorLookatPlayer(lua_State* L);
int32_t Script_CommentatorSetCamera(lua_State* L);
int32_t Script_CommentatorGetCurrentMapID(lua_State* L);
int32_t Script_CommentatorStartInstance(lua_State* L);
int32_t Script_CommentatorAddPlayer(lua_State* L);
int32_t Script_CommentatorRemovePlayer(lua_State* L);
int32_t Script_CommentatorSetMoveSpeed(lua_State* L);
int32_t Script_CommentatorSetSkirmishMatchmakingMode(lua_State* L);
int32_t Script_CommentatorRequestSkirmishQueueData(lua_State* L);
int32_t Script_CommentatorStartSkirmishMatch(lua_State* L);
int32_t Script_CommentatorRequestSkirmishMode(lua_State* L);

#endif
