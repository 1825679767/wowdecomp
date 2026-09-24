#ifndef WOW_SOURCE_WORLDCLIENT_WORLD_H
#define WOW_SOURCE_WORLDCLIENT_WORLD_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CRect.h"

class CMapBaseObj;
class CMapChunk;
class CMapDoodadDef;
class CMapDoodadDefMapChunkLink;

class CRange {
    public:
    /* 0000 */ float m_l;
    /* 0004 */ float m_h;
};

class CBoundingBox {
    public:
    /* 0000 */ CRange m_x;
    /* 0008 */ CRange m_y;
    /* 0010 */ CRange m_z;
};

class CMapDoodadDefMapChunkLink {
    public:
    /* 0000 */ uint32_t m_objectIndex;
    /* 0004 */ CMapDoodadDef* m_owner;
    /* 0008 */ CMapBaseObj* m_ref;
    /* 000C */ TSLink<CMapDoodadDefMapChunkLink> m_refLink;
    /* 0014 */ TSLink<CMapDoodadDefMapChunkLink> m_ownerLink;
};

class CSortEntry {
    public:
    /* 0000 */ TSExplicitList<CMapChunk> m_mapChunkList;
    /* 000C */ TSList<void> m_exteriorGroupList;
    /* 0018 */ TSList<void> m_entityList;
    /* 0024 */ TSList<void> m_doodadDefList;
    /* 0030 */ TSList<void> m_liquidList;
    /* 003C */ TSList<void> m_occluderList;
    /* 0048 */ TSList<void> m_unkList7;
    /* 0054 */ TSList<void> m_holedChunkList;
    /* 0060 */ TSList<void> m_unkList9;
};

struct EVENT_DATA_UPDATE {
    /* 0000 */ float m_elapsedSec;
    /* 0004 */ uint32_t m_time;
};

struct OcclusionVolumeData {
    /* 0000 */ C3Vector m_min;
    /* 000C */ C3Vector m_max;
    /* 0018 */ C3Vector* m_positions;
    /* 001C */ uint32_t m_positionCount;
};

struct OcclusionVolume {
    /* 0000 */ int32_t m_mapID;
    /* 0004 */ uint32_t m_flags;
    /* 0008 */ OcclusionVolumeData m_data;
};

struct SPortalExt {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ CRect m_rect;
    /* 0014 */ uint32_t m_stamp;
    /* 0018 */ uint32_t m_pad;
};

struct WorldStatus {
    /* 0000 */ uint32_t m_prepareAll;
    /* 0004 */ uint32_t m_chunksChanged;
};

class CWorldScene {
    public:
    /* 0000 */ uint8_t unk_0000[16];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[24];
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint8_t unk_0030[28];
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint8_t unk_0050[32];
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint8_t unk_0074[4];
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint8_t unk_0080[8];
    /* 0088 */ uint32_t unk_0088;
    /* 008C */ uint8_t unk_008C[36];
    /* 00B0 */ uint32_t unk_00B0;
    /* 00B4 */ uint8_t unk_00B4[64];
    /* 00F4 */ uint32_t unk_00F4;
    /* 00F8 */ uint8_t unk_00F8[36];
    /* 011C */ uint32_t unk_011C;
    /* 0120 */ uint8_t unk_0120[16];
    /* 0130 */ uint32_t unk_0130;
    /* 0134 */ uint32_t unk_0134;
    /* 0138 */ uint8_t unk_0138[52];
    /* 016C */ uint32_t unk_016C;
    /* 0170 */ uint8_t unk_0170[84];
    /* 01C4 */ uint32_t unk_01C4;
    /* 01C8 */ uint8_t unk_01C8[28];
    /* 01E4 */ uint32_t unk_01E4;
};

#endif
