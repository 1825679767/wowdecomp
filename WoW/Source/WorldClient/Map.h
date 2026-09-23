#ifndef WOW_SOURCE_WORLDCLIENT_MAP_H
#define WOW_SOURCE_WORLDCLIENT_MAP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/c4quaternion.h"
#include "Engine/Source/Tempest/caabox.h"
#include "Engine/Source/Tempest/cimvector.h"

struct CMapArea;

class CGxVertexPC {
    public:
    /* 0000 */ C3Vector m_p;
    /* 000C */ CImVector m_c;
};

class CMapAreaEntry {
    public:
    /* 0000 */ CMapArea* m_area;
    /* 0004 */ float m_dist;
};

struct SMAreaInfo {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_unused;
};

struct SMDoodadDef {
    /* 0000 */ uint32_t m_nameId;
    /* 0004 */ uint32_t m_uniqueId;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ C3Vector m_rotation;
    /* 0020 */ uint16_t m_scale;
    /* 0022 */ uint16_t m_flags;
};

struct SMMapObjDef {
    /* 0000 */ uint32_t m_nameId;
    /* 0004 */ uint32_t m_uniqueId;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ C3Vector m_rotation;
    /* 0020 */ CAaBox m_extents;
    /* 0038 */ uint16_t m_flags;
    /* 003A */ uint16_t m_doodadSet;
    /* 003C */ uint16_t m_nameSet;
    /* 003E */ uint16_t m_scale;
};

struct SMODoodadDef {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ C3Vector m_position;
    /* 0010 */ C4Quaternion m_orientation;
    /* 0020 */ float m_scale;
    /* 0024 */ CImVector m_color;
};

class CMap {
    public:
    /* 0000 */ uint8_t unk_0000[180];
    /* 00B4 */ uint32_t unk_00B4;
};

class CMapAreaLow {
    public:
    /* 0000 */ uint8_t unk_0000[1];
    /* 0001 */ uint8_t unk_0001;
    /* 0002 */ uint8_t unk_0002;
    /* 0003 */ uint8_t unk_0003;
    /* 0004 */ uint8_t unk_0004;
    /* 0005 */ uint8_t unk_0005;
    /* 0006 */ uint8_t unk_0006;
    /* 0007 */ uint8_t unk_0007;
    /* 0008 */ uint8_t unk_0008;
    /* 0009 */ uint8_t unk_0009;
    /* 000A */ uint8_t unk_000A;
    /* 000B */ uint8_t unk_000B;
    /* 000C */ uint8_t unk_000C;
    /* 000D */ uint8_t unk_000D;
    /* 000E */ uint8_t unk_000E;
    /* 000F */ uint8_t unk_000F;
    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016[4];
};

#endif
