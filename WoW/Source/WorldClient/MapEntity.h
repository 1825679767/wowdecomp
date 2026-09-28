#ifndef WOW_SOURCE_WORLDCLIENT_MAPENTITY_H
#define WOW_SOURCE_WORLDCLIENT_MAPENTITY_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/MapLoad.h"

class CMapObjDef;

class CMapEntity : public CMapStaticEntity {
    public:
    /* 0090 */ uint32_t (__cdecl* m_func )(uint32_t, uint32_t, uint64_t, uint32_t);
    /* 0094 */ void* m_funcParam;
    /* 0098 */ uint64_t m_funcParam64;
    /* 00A0 */ uint32_t m_funcParam32;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ C3Vector unk_00A8;
    /* 00B4 */ uint32_t unk_00B4;
    /* 00B8 */ int32_t m_groundType;
    /* 00BC */ uint16_t unk_00BC;
    /* 00BE */ uint16_t unk_00BE;
    /* 00C0 */ CImVector unk_00C0;
    /* 00C4 */ float unk_00C4;
    /* 00C8 */ TSLink<void> m_sortEntryLink;

    virtual bool QueryInteriorLighting(CMapObjDef* mapObjDef, int32_t groupNum, uint16_t* faceIndex, C3Vector* pos);

    CMapEntity();
    void QueryLiquidSounds(uint32_t* results, uint32_t* distance);
    bool UpdateMapObjLiquid();
};

#endif
