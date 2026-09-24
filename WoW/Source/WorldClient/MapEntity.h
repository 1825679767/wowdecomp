#ifndef WOW_SOURCE_WORLDCLIENT_MAPENTITY_H
#define WOW_SOURCE_WORLDCLIENT_MAPENTITY_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/MapLoad.h"

class CMapEntity : public CMapStaticEntity {
    public:
    /* 0090 */ uint32_t (__cdecl* m_func )(uint32_t, uint32_t, uint64_t, uint32_t);
    /* 0094 */ void* m_funcParam;
    /* 0098 */ uint64_t m_funcParam64;
    /* 00A0 */ uint32_t m_funcParam32;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ uint32_t unk_00A8;
    /* 00AC */ uint32_t unk_00AC;
    /* 00B0 */ uint32_t unk_00B0;
    /* 00B4 */ uint32_t unk_00B4;
    /* 00B8 */ uint32_t unk_00B8;
    /* 00BC */ uint32_t unk_00BC;
    /* 00C0 */ uint32_t unk_00C0;
    /* 00C4 */ uint32_t unk_00C4;
    /* 00C8 */ TSLink<void> m_sortEntryLink;
};

#endif
