#ifndef WOW_SOURCE_UI_WORLDMAP_H
#define WOW_SOURCE_UI_WORLDMAP_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct DebugElement {
    /* 0000 */ int32_t unk_0000;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t m_array1 [6];
    /* 0020 */ int32_t m_array2 [4];
    /* 0030 */ int32_t m_computed48;
    /* 0034 */ int32_t m_computed52;
    /* 0038 */ int32_t m_computed56;
    /* 003C */ int32_t m_extra60;
    /* 0040 */ int32_t m_extra64;
    /* 0044 */ int32_t m_extra68;
    /* 0048 */ float m_extra76;
    /* 004C */ void* unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ int32_t unk_005C;
    /* 0060 */ int32_t unk_0060;
    /* 0064 */ int32_t unk_0064;
    /* 0068 */ int32_t unk_0068;
};

class CGWorldMap {
    public:
    static void EnterWorld();
    static void InitializeGame();
};

int32_t Script_GetMapContinents(lua_State* L);
int32_t Script_GetMapZones(lua_State* L);
int32_t Script_SetMapZoom(lua_State* L);
int32_t Script_ZoomOut(lua_State* L);
int32_t Script_SetDungeonMapLevel(lua_State* L);
int32_t Script_GetNumDungeonMapLevels(lua_State* L);
int32_t Script_DungeonUsesTerrainMap(lua_State* L);
int32_t Script_SetMapToCurrentZone(lua_State* L);
int32_t Script_GetMapInfo(lua_State* L);
int32_t Script_GetCurrentMapContinent(lua_State* L);
int32_t Script_GetCurrentMapAreaID(lua_State* L);
int32_t Script_GetCurrentMapZone(lua_State* L);
int32_t Script_GetCurrentMapDungeonLevel(lua_State* L);
int32_t Script_SetMapByID(lua_State* L);
int32_t Script_IsZoomOutAvailable(lua_State* L);
int32_t Script_ProcessMapClick(lua_State* L);
int32_t Script_UpdateMapHighlight(lua_State* L);
int32_t Script_GetPlayerMapPosition(lua_State* L);
int32_t Script_GetCorpseMapPosition(lua_State* L);
int32_t Script_GetDeathReleasePosition(lua_State* L);
int32_t Script_GetNumMapLandmarks(lua_State* L);
int32_t Script_GetMapLandmarkInfo(lua_State* L);
int32_t Script_GetNumMapOverlays(lua_State* L);
int32_t Script_GetMapOverlayInfo(lua_State* L);
int32_t Script_CreateWorldMapArrowFrame(lua_State* L);
int32_t Script_InitWorldMapPing(lua_State* L);
int32_t Script_CreateMiniWorldMapArrowFrame(lua_State* L);
int32_t Script_UpdateWorldMapArrowFrames(lua_State* L);
int32_t Script_PositionWorldMapArrowFrame(lua_State* L);
int32_t Script_PositionMiniWorldMapArrowFrame(lua_State* L);
int32_t Script_ShowWorldMapArrowFrame(lua_State* L);
int32_t Script_ShowMiniWorldMapArrowFrame(lua_State* L);
int32_t Script_ClickLandmark(lua_State* L);
int32_t Script_GetNumMapDebugObjects(lua_State* L);
int32_t Script_GetMapDebugObjectInfo(lua_State* L);
int32_t Script_TeleportToDebugObject(lua_State* L);
int32_t Script_HasDebugZoneMap(lua_State* L);
int32_t Script_GetDebugZoneMap(lua_State* L);
int32_t Script_GetWintergraspWaitTime(lua_State* L);
int32_t Script_CanQueueForWintergrasp(lua_State* L);
void WorldMapRegisterScriptFunctions();

int32_t PlayerExploredZonesUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);

#endif
