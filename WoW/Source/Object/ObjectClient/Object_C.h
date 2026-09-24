#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_OBJECT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_OBJECT_C_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/Object.h"
#include "WoW/Source/Ui/DBCACHEHASH_NAME.h"

class CGObject_C;
class CHashKeyGUID;
class CM2Model;
struct CMapEntity;
class PLAYERNAMEDESC;

typedef uint64_t WOWGUID;

class CHashKeyGUID {
    public:
    /* 0000 */ WOWGUID m_guid;
};

class CGObject_C : public CGObject, public TSHashObject<CGObject_C, CHashKeyGUID> {
    public:
    /* 0038 */ TSLink<void> m_link;
    /* 0040 */ uint32_t m_disableTime;
    /* 0044 */ TSList<void> m_objMirrorLists [6];
    /* 008C */ CM2Model* m_model;
    /* 0090 */ uint32_t unk_0090;
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ float m_scale;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ uint32_t m_effectList;
    /* 00AC */ uint32_t m_height;
    /* 00B0 */ PLAYERNAMEDESC* m_nameDesc;
    /* 00B4 */ CM2Model* m_worldModel;
    /* 00B8 */ CMapEntity* m_worldObject;
    /* 00BC */ uint32_t m_modelFlags;
    /* 00C0 */ uint32_t unk_00C0;
    /* 00C4 */ uint32_t unk_00C4;
    /* 00C8 */ uint32_t unk_00C8;
    /* 00CC */ uint8_t unk_00CC [4];
};

struct DBCACHEHASH_NAME : public TSHashObject<DBCACHEHASH_NAME, CHashKeyGUID> {
    /* 0020 */ uint8_t m_record [336];
    /* 0170 */ DBCACHETRAILER_GUID m_trailer;
};

#endif
