#ifndef ENGINE_SOURCE_SERVICES_MODELBLOB_H
#define ENGINE_SOURCE_SERVICES_MODELBLOB_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/CAaBox.h"

class CModelBlob {
    public:
    /* 0000 */ char m_filename [256];
    /* 0100 */ uint32_t m_version;
    /* 0104 */ void* m_data;
    /* 0108 */ uint32_t m_entries;
    /* 010C */ uint32_t m_entryCount;
    /* 0110 */ uint32_t m_stringTable;
};

class CModelBlobEntry {
    public:
    /* 0000 */ uint32_t m_filenameOffset;
    /* 0004 */ CAaBox m_boundingBox;
};

#endif
