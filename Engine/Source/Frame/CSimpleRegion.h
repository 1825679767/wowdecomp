#ifndef ENGINE_SOURCE_FRAME_CSIMPLEREGION_H
#define ENGINE_SOURCE_FRAME_CSIMPLEREGION_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/Types.h"

class CStatus;
class XMLNode;

class CSimpleRegion : public CScriptRegion {
    public:
    /* 00A0 */ uint32_t m_alphaCount;
    /* 00A4 */ uint8_t m_alphas [4];
    /* 00A8 */ uint32_t m_colorCount;
    /* 00AC */ CColor m_colors [4];
    /* 00BC */ uint32_t m_regionUkn8;
    /* 00C0 */ uint32_t m_regionUkn9;
    /* 00C4 */ uint32_t m_regionUkn10;
    /* 00C8 */ uint32_t m_regionUkn11;
    /* 00CC */ uint32_t m_regionUkn12;
    /* 00D0 */ uint32_t m_regionUkn13;

    void PreLoadXML(XMLNode* node, CStatus* status);
};

#endif
