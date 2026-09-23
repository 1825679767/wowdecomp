#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJDEF_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJDEF_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/c44matrix.h"
#include "Engine/Source/Tempest/caabox.h"
#include "Engine/Source/Tempest/caasphere.h"
#include "Storm/H/stpl.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"

class CMapObj;
class CMapObjDefGroup;
struct CMapObjDefMapObjDefGroupLink;

class CMapObjDefGroup : public CMapBaseObj {
    public:
    /* 0024 */ CAaBox m_bbox;
    /* 003C */ CAaSphere m_sphere;
    /* 004C */ float m_distanceToCamera;
    /* 0050 */ uint32_t m_groupNum;
    /* 0054 */ uint32_t m_unkFlags;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ uint32_t m_ambientColor;
    /* 0060 */ int32_t unk_0060;
    /* 0064 */ int32_t unk_0064;
    /* 0068 */ int32_t unk_0068;
    /* 006C */ TSList<void> m_frustumList;
    /* 0078 */ TSList<void> m_doodadDefLinkList;
    /* 0084 */ TSList<void> m_entityLinkList;
    /* 0090 */ uint32_t unk_0090;
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint32_t unk_0098;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ TSLink<void> m_sortEntryLink;
    /* 00B0 */ TSLink<void> m_sortTableLink;
    /* 00B8 */ uint32_t unk_00B8;
    /* 00BC */ uint32_t unk_00BC;
};

class CMapObjDef : public CMapBaseObj {
    public:
    /* 0024 */ void* unk_0024;
    /* 0028 */ void* unk_0028;
    /* 002C */ void* unk_002C;
    /* 0030 */ void* unk_0030;
    /* 0034 */ void* unk_0034;
    /* 0038 */ int32_t unk_0038;
    /* 003C */ C3Vector m_position;
    /* 0048 */ CAaBox m_bbox;
    /* 0060 */ CAaSphere m_sphere;
    /* 0070 */ C44Matrix m_mat;
    /* 00B0 */ C44Matrix m_invMat;
    /* 00F0 */ int32_t m_nameId;
    /* 00F4 */ CMapObj* m_owner;
    /* 00F8 */ int32_t unk_00F8;
    /* 00FC */ uint32_t m_unkFlags;
    /* 0100 */ int32_t m_doodadSet;
    /* 0104 */ int32_t m_nameSet;
    /* 0108 */ int32_t unk_0108;
    /* 010C */ int32_t unk_010C;
    /* 0110 */ int32_t unk_0110;
    /* 0114 */ TSExplicitList<CMapObjDefMapObjDefGroupLink> m_mapObjDefGroupLinkList;
    /* 0120 */ TSGrowableArray<CMapObjDefGroup> m_defGroups;
    /* 0130 */ uint32_t m_groupCount;
    /* 0134 */ TSGrowableArray<void> m_lightsArray;
    /* 0144 */ uint32_t m_argbColor;
    /* 0148 */ int32_t unk_0148;
    /* 014C */ int32_t unk_014C;
    /* 0150 */ int16_t m_doodadSetOverrides [3];
    /* 0156 */ uint8_t unk_0156[2];
};

#endif
