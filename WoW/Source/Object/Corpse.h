#ifndef WOW_SOURCE_OBJECT_CORPSE_H
#define WOW_SOURCE_OBJECT_CORPSE_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGCorpseData;

class CGCorpse {
    public:
    /* 0000 */ CGCorpseData* m_corpse;
    /* 0004 */ void* m_corpseMirror;
};

class CGCorpseData {
    public:
    /* 0000 */ WGUID m_corpseFieldOwner;
    /* 0008 */ WGUID m_corpseFieldParty;
    /* 0010 */ uint32_t m_corpseFieldDisplayId;
    /* 0014 */ uint32_t m_corpseFieldItem [19];
    /* 0060 */ uint8_t m_corpseFieldBytes1 [4];
    /* 0064 */ uint8_t m_corpseFieldBytes2 [4];
    /* 0068 */ uint32_t m_corpseFieldGuild;
    /* 006C */ uint32_t m_corpseFieldFlags;
    /* 0070 */ uint32_t m_corpseFieldDynamicFlags;
    /* 0074 */ uint32_t m_corpseFieldPad;
};

#endif
