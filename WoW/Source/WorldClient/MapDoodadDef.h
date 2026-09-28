#ifndef WOW_SOURCE_WORLDCLIENT_MAPDOODADDEF_H
#define WOW_SOURCE_WORLDCLIENT_MAPDOODADDEF_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/MapLoad.h"

class CM2Lighting;
class CMapBaseObj;
class CMapObjDef;

class CMapDoodadDef : public CMapStaticEntity {
    public:
    /* 0090 */ uint32_t m_uniqueId;
    /* 0094 */ void* unk_0094;
    /* 0098 */ void* unk_0098;
    /* 009C */ void* m_prevMapDoodadDef09c;
    /* 00A0 */ void* m_nextMapDoodadDef0a0;
    /* 00A4 */ int32_t unk_00A4;
    /* 00A8 */ TSLink<CMapDoodadDef> m_doodadDefLink;
    /* 00B0 */ int32_t m_cullPassStamp;
    /* 00B4 */ int32_t unk_00B4;
    /* 00B8 */ uint64_t unk_00B8;
    /* 00C0 */ CAaBox m_bboxDoodadDef;
    /* 00D8 */ C44Matrix m_mat;
    /* 0118 */ C44Matrix m_identity;
    /* 0158 */ int32_t unk_0158;
    /* 015C */ int32_t unk_015C;
    /* 0160 */ int32_t unk_0160;
    /* 0164 */ int32_t unk_0164;
    /* 0168 */ int32_t unk_0168;
    /* 016C */ int32_t unk_016C;

    virtual void SelectLights(CM2Lighting* lighting);
    virtual void SelectUnderwater(CM2Lighting* lighting);
    virtual bool QueryInteriorLighting(CMapObjDef* mapObjDef, int32_t groupNum, uint16_t* faceIndex, C3Vector* pos);

    CMapDoodadDef();
    void UpdateBounds();
    void ExtendBounds(CMapBaseObj* parent);
    void ExtendChunkBounds();
};

#endif
