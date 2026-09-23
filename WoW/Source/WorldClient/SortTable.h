#ifndef WOW_SOURCE_WORLDCLIENT_SORTTABLE_H
#define WOW_SOURCE_WORLDCLIENT_SORTTABLE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C4Vector.h"
#include "Storm/H/stpl.h"
#include "WoW/Source/WorldClient/World.h"

class CSortTable {
    public:
    /* 0000 */ CSortEntry m_table [64];
    /* 1B00 */ C4Vector m_bucketPlanes [64];
    /* 1F00 */ TSList<void> m_pendingExteriorGroupList;
    /* 1F0C */ TSList<void> m_detailDoodadInstList;
    /* 1F18 */ TSList<void> m_renderChunkList [24];
    /* 2038 */ TSList<void> m_mapObjDefGroup;
    /* 2044 */ TSList<void> m_interiorLiquidGroupList;
    /* 2050 */ TSList<void> m_visibleEntityList;
    /* 205C */ TSList<void> m_culledEntityList;
    /* 2068 */ TSList<void> m_detailDoodadBatchList;
    /* 2074 */ TSList<void> m_horizonChunkList;
    /* 2080 */ TSList<void> m_horizonMapObjList;
};

#endif
