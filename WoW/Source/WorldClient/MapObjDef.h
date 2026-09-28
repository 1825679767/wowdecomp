#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJDEF_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJDEF_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"

class CM2Lighting;

class CMapObj;
class CMapObjDefGroup;
class CMapObjDefMapObjDefGroupLink;

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
    /* 0078 */ TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > m_doodadDefLinkList;
    /* 0084 */ TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > m_entityLinkList;
    /* 0090 */ TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > m_lightLinkList;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ TSLink<void> m_sortEntryLink;
    /* 00B0 */ TSLink<void> m_sortTableLink;
    /* 00B8 */ uint32_t unk_00B8;
    /* 00BC */ uint32_t unk_00BC;

    virtual void SelectLights(CM2Lighting* lighting);

    CMapObjDefGroup();
    void MarkPrepared();
    void UpdateLights();
};

class CMapObjDef : public CMapBaseObj, public TSHashObject<CMapObjDef, HASHKEY_NONE> {
    public:
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
    /* 0114 */ TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > m_mapObjDefGroupLinkList;
    /* 0120 */ TSCGrowableArray<CMapObjDefGroup*, 4> m_defGroups;
    /* 0134 */ TSGrowableArray<uint8_t> m_lightsArray;
    /* 0144 */ uint32_t m_argbColor;
    /* 0148 */ int32_t unk_0148;
    /* 014C */ int32_t unk_014C;
    /* 0150 */ int16_t m_doodadSetOverrides [3];
    /* 0156 */ uint8_t unk_0156[2];

    CMapObjDef();
    bool TestAABox(C3Vector& start, C3Vector& end);
    int32_t GetGroundType(uint32_t groupNum, uint16_t faceIndex);
    void RenderWithCallback(TSGrowableArray<uint16_t>* groups);
};

#endif
