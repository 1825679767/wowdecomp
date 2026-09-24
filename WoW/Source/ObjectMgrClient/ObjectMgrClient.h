#ifndef WOW_SOURCE_OBJECTMGRCLIENT_OBJECTMGRCLIENT_H
#define WOW_SOURCE_OBJECTMGRCLIENT_OBJECTMGRCLIENT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/WGUID.h"

class CGObject_C;
class CHashKeyGUID;
class CMovementGlobals;
struct RealmConnection;

class CMirrorHandler {
    public:
    /* 0000 */ TSLink<void> m_link1;
    /* 0008 */ TSLink<void> m_link2;
    /* 0010 */ uint32_t m_function;
    /* 0014 */ uint32_t m_functionParam;
    /* 0018 */ uint32_t m_count;
    /* 001C */ uint32_t m_fieldByteOffset;
    /* 0020 */ uint32_t m_mirrorByteOffset;
    /* 0024 */ uint32_t m_fieldByteSize;
    /* 0028 */ uint32_t m_linkPositionSelector;
    /* 002C */ uint8_t m_dispatching;
    /* 002D */ uint8_t m_deletePending;
    /* 002E */ uint8_t m_alwaysFire;
    /* 002F */ uint8_t unk_002F;
};

class ObjectMgr {
    public:
    /* 0000 */ TSHashTable<CGObject_C, CHashKeyGUID> m_objects;
    /* 0028 */ TSHashTable<CGObject_C, CHashKeyGUID> m_lazyCleanupObjects;
    /* 0050 */ TSList<void> unk_0050 [7];
    /* 00A4 */ TSList<void> m_visibleObjects;
    /* 00B0 */ TSList<void> unk_00B0;
    /* 00BC */ uint32_t unk_00BC;
    /* 00C0 */ WGUID m_playerGuid;
    /* 00C8 */ uint32_t unk_00C8;
    /* 00CC */ uint32_t m_mapId;
    /* 00D0 */ RealmConnection* m_realmConnection;
    /* 00D4 */ CMovementGlobals* m_movementGlobal;
};

#endif
