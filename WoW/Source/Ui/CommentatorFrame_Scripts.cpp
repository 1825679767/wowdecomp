#include "CommentatorFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACE370
static FrameScript_Method s_ScriptFunctions[] = {
    { "CommentatorSetMode", &Script_CommentatorSetMode },
    { "CommentatorToggleMode", &Script_CommentatorToggleMode },
    { "CommentatorGetMode", &Script_CommentatorGetMode },
    { "CommentatorSetMapAndInstanceIndex", &Script_CommentatorSetMapAndInstanceIndex },
    { "CommentatorSetPlayerIndex", &Script_CommentatorSetPlayerIndex },
    { "CommentatorUpdatePlayerInfo", &Script_CommentatorUpdatePlayerInfo },
    { "CommentatorUpdateMapInfo", &Script_CommentatorUpdateMapInfo },
    { "CommentatorGetNumMaps", &Script_CommentatorGetNumMaps },
    { "CommentatorGetMapInfo", &Script_CommentatorGetMapInfo },
    { "CommentatorGetInstanceInfo", &Script_CommentatorGetInstanceInfo },
    { "CommentatorEnterInstance", &Script_CommentatorEnterInstance },
    { "CommentatorExitInstance", &Script_CommentatorExitInstance },
    { "CommentatorGetNumPlayers", &Script_CommentatorGetNumPlayers },
    { "CommentatorGetPlayerInfo", &Script_CommentatorGetPlayerInfo },
    { "CommentatorFollowPlayer", &Script_CommentatorFollowPlayer },
    { "CommentatorLookatPlayer", &Script_CommentatorLookatPlayer },
    { "CommentatorZoomIn", &Script_CommentatorZoomIn },
    { "CommentatorZoomOut", &Script_CommentatorZoomOut },
    { "CommentatorSetCamera", &Script_CommentatorSetCamera },
    { "CommentatorGetCamera", &Script_CommentatorGetCamera },
    { "CommentatorGetCurrentMapID", &Script_CommentatorGetCurrentMapID },
    { "CommentatorStartInstance", &Script_CommentatorStartInstance },
    { "CommentatorAddPlayer", &Script_CommentatorAddPlayer },
    { "CommentatorRemovePlayer", &Script_CommentatorRemovePlayer },
    { "CommentatorSetBattlemaster", &Script_CommentatorSetBattlemaster },
    { "CommentatorSetMoveSpeed", &Script_CommentatorSetMoveSpeed },
    { "CommentatorSetCameraCollision", &Script_CommentatorSetCameraCollision },
    { "CommentatorSetTargetHeightOffset", &Script_CommentatorSetTargetHeightOffset },
    { "CommentatorSetSkirmishMatchmakingMode", &Script_CommentatorSetSkirmishMatchmakingMode },
    { "CommentatorRequestSkirmishQueueData", &Script_CommentatorRequestSkirmishQueueData },
    { "CommentatorGetSkirmishQueueCount", &Script_CommentatorGetSkirmishQueueCount },
    { "CommentatorGetSkirmishQueuePlayerInfo", &Script_CommentatorGetSkirmishQueuePlayerInfo },
    { "CommentatorStartSkirmishMatch", &Script_CommentatorStartSkirmishMatch },
    { "CommentatorRequestSkirmishMode", &Script_CommentatorRequestSkirmishMode },
    { "CommentatorGetSkirmishMode", &Script_CommentatorGetSkirmishMode },
};

// VERIFY: 7ac1891d auto
// OFFSET: 0x00568460
void CGCommentator::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
