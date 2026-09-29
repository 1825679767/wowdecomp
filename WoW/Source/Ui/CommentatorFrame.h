#ifndef WOW_SOURCE_UI_COMMENTATORFRAME_H
#define WOW_SOURCE_UI_COMMENTATORFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGPlayer_C;

class CGCommentator {
    public:
    static void PostInit(CGPlayer_C* player);
    static CGCommentator s_Commentator;

    static void InitializeGame();
    static void RegisterScriptFunctions();

    /* 0000 */ float m_facing;
    /* 0004 */ float m_pitch;
    /* 0008 */ float m_yawVelocity;
    /* 000C */ uint8_t unk_000C[12];
    /* 0018 */ float m_forwardVelocity;
    /* 001C */ float m_strafeVelocity;
    /* 0020 */ float m_verticalVelocity;

    WOWGUID GetArenaMember(uint32_t index);
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

int32_t Script_CommentatorSetMode(lua_State* L);
int32_t Script_CommentatorToggleMode(lua_State* L);
int32_t Script_CommentatorGetMode(lua_State* L);
int32_t Script_CommentatorSetMapAndInstanceIndex(lua_State* L);
int32_t Script_CommentatorSetPlayerIndex(lua_State* L);
int32_t Script_CommentatorUpdatePlayerInfo(lua_State* L);
int32_t Script_CommentatorUpdateMapInfo(lua_State* L);
int32_t Script_CommentatorGetNumMaps(lua_State* L);
int32_t Script_CommentatorGetMapInfo(lua_State* L);
int32_t Script_CommentatorGetInstanceInfo(lua_State* L);
int32_t Script_CommentatorEnterInstance(lua_State* L);
int32_t Script_CommentatorExitInstance(lua_State* L);
int32_t Script_CommentatorGetNumPlayers(lua_State* L);
int32_t Script_CommentatorGetPlayerInfo(lua_State* L);
int32_t Script_CommentatorFollowPlayer(lua_State* L);
int32_t Script_CommentatorLookatPlayer(lua_State* L);
int32_t Script_CommentatorZoomIn(lua_State* L);
int32_t Script_CommentatorZoomOut(lua_State* L);
int32_t Script_CommentatorSetCamera(lua_State* L);
int32_t Script_CommentatorGetCamera(lua_State* L);
int32_t Script_CommentatorGetCurrentMapID(lua_State* L);
int32_t Script_CommentatorStartInstance(lua_State* L);
int32_t Script_CommentatorAddPlayer(lua_State* L);
int32_t Script_CommentatorRemovePlayer(lua_State* L);
int32_t Script_CommentatorSetBattlemaster(lua_State* L);
int32_t Script_CommentatorSetMoveSpeed(lua_State* L);
int32_t Script_CommentatorSetCameraCollision(lua_State* L);
int32_t Script_CommentatorSetTargetHeightOffset(lua_State* L);
int32_t Script_CommentatorSetSkirmishMatchmakingMode(lua_State* L);
int32_t Script_CommentatorRequestSkirmishQueueData(lua_State* L);
int32_t Script_CommentatorGetSkirmishQueueCount(lua_State* L);
int32_t Script_CommentatorGetSkirmishQueuePlayerInfo(lua_State* L);
int32_t Script_CommentatorStartSkirmishMatch(lua_State* L);
int32_t Script_CommentatorRequestSkirmishMode(lua_State* L);
int32_t Script_CommentatorGetSkirmishMode(lua_State* L);

#endif
