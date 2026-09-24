#ifndef WOW_SOURCE_WORLDCLIENT_MAPBASEOBJ_H
#define WOW_SOURCE_WORLDCLIENT_MAPBASEOBJ_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/Types.h"

class CMapBaseObj;
class CMapBaseObjLink;

class CMapBaseObj {
    public:
    /* 0004 */ int32_t m_objectIndex;
    /* 0008 */ uint16_t m_type;
    /* 000A */ uint16_t m_refCount;
    /* 000C */ MapObjFlag m_flags;
    /* 0010 */ TSLink<CMapBaseObj> m_lameAssLink;
    /* 0018 */ TSExplicitList<CMapBaseObjLink> m_parentLinkList;

    virtual ~CMapBaseObj();
};

#endif
