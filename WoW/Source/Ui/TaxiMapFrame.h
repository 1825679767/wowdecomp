#ifndef WOW_SOURCE_UI_TAXIMAPFRAME_H
#define WOW_SOURCE_UI_TAXIMAPFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/WGUID.h"

class CTexture;
class TaxiNodesRec;
class TaxiPathNodeRec;
class TaxiPathRec;
struct lua_State;

struct TaxiNodeInfo {
    /* 0000 */ TaxiNodesRec* m_rec;
    /* 0004 */ float m_x;
    /* 0008 */ float m_y;
    /* 000C */ uint8_t m_noDirectPath;
    /* 000D */ uint8_t unk_000D;
    /* 000E */ uint8_t unk_000E[2];
    /* 0010 */ int32_t unk_0010[5];
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ uint32_t m_numHops;
    /* 002C */ float m_distance;
};

struct TAXIPATHNODESCLIENT : public TSHashObject<TAXIPATHNODESCLIENT, HASHKEY_NONE> {
    /* 0018 */ TaxiPathNodeRec* m_firstNode;
    /* 001C */ int32_t m_numNodes;
};

struct TAXIROUTELINE {
    /* 0000 */ C3Vector m_src;
    /* 000C */ C3Vector m_dest;
};

class CGTaxiMap {
    public:
    static void CloseMap();
    static CTexture* m_mapTexture;
    static int32_t m_numNodes;
    static TSGrowableArray<TaxiNodeInfo> m_nodes;
    static TSFixedArray<TSGrowableArray<TAXIROUTELINE>> m_routeLines;
    static WOWGUID m_unit;
    static uint32_t m_maxNodeID;
    static uint32_t m_nodeMaskSize;
    static void InitializeGame();
    static void RegisterScriptFunctions();
};

int32_t Script_TaxiNodeCost(lua_State* L);
int32_t Script_TakeTaxiNode(lua_State* L);
int32_t Script_CloseTaxiMap(lua_State* L);
int32_t Script_TaxiNodeGetType(lua_State* L);
int32_t Script_TaxiNodeSetCurrent(lua_State* L);

#endif
