#include "WorldMap.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACCF18
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetMapContinents", &Script_GetMapContinents },
    { "GetMapZones", &Script_GetMapZones },
    { "SetMapZoom", &Script_SetMapZoom },
    { "ZoomOut", &Script_ZoomOut },
    { "SetDungeonMapLevel", &Script_SetDungeonMapLevel },
    { "GetNumDungeonMapLevels", &Script_GetNumDungeonMapLevels },
    { "DungeonUsesTerrainMap", &Script_DungeonUsesTerrainMap },
    { "SetMapToCurrentZone", &Script_SetMapToCurrentZone },
    { "GetMapInfo", &Script_GetMapInfo },
    { "GetCurrentMapContinent", &Script_GetCurrentMapContinent },
    { "GetCurrentMapAreaID", &Script_GetCurrentMapAreaID },
    { "GetCurrentMapZone", &Script_GetCurrentMapZone },
    { "GetCurrentMapDungeonLevel", &Script_GetCurrentMapDungeonLevel },
    { "SetMapByID", &Script_SetMapByID },
    { "IsZoomOutAvailable", &Script_IsZoomOutAvailable },
    { "ProcessMapClick", &Script_ProcessMapClick },
    { "UpdateMapHighlight", &Script_UpdateMapHighlight },
    { "GetPlayerMapPosition", &Script_GetPlayerMapPosition },
    { "GetCorpseMapPosition", &Script_GetCorpseMapPosition },
    { "GetDeathReleasePosition", &Script_GetDeathReleasePosition },
    { "GetNumMapLandmarks", &Script_GetNumMapLandmarks },
    { "GetMapLandmarkInfo", &Script_GetMapLandmarkInfo },
    { "GetNumMapOverlays", &Script_GetNumMapOverlays },
    { "GetMapOverlayInfo", &Script_GetMapOverlayInfo },
    { "CreateWorldMapArrowFrame", &Script_CreateWorldMapArrowFrame },
    { "InitWorldMapPing", &Script_InitWorldMapPing },
    { "CreateMiniWorldMapArrowFrame", &Script_CreateMiniWorldMapArrowFrame },
    { "UpdateWorldMapArrowFrames", &Script_UpdateWorldMapArrowFrames },
    { "PositionWorldMapArrowFrame", &Script_PositionWorldMapArrowFrame },
    { "PositionMiniWorldMapArrowFrame", &Script_PositionMiniWorldMapArrowFrame },
    { "ShowWorldMapArrowFrame", &Script_ShowWorldMapArrowFrame },
    { "ShowMiniWorldMapArrowFrame", &Script_ShowMiniWorldMapArrowFrame },
    { "ClickLandmark", &Script_ClickLandmark },
    { "GetNumMapDebugObjects", &Script_GetNumMapDebugObjects },
    { "GetMapDebugObjectInfo", &Script_GetMapDebugObjectInfo },
    { "TeleportToDebugObject", &Script_TeleportToDebugObject },
    { "HasDebugZoneMap", &Script_HasDebugZoneMap },
    { "GetDebugZoneMap", &Script_GetDebugZoneMap },
    { "GetWintergraspWaitTime", &Script_GetWintergraspWaitTime },
    { "CanQueueForWintergrasp", &Script_CanQueueForWintergrasp },
};

// VERIFY: 9f60c73b auto
// OFFSET: 0x00543650
void WorldMapRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
