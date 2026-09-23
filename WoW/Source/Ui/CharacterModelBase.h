#ifndef WOW_SOURCE_UI_CHARACTERMODELBASE_H
#define WOW_SOURCE_UI_CHARACTERMODELBASE_H

#include <cstddef>
#include <cstdint>

class CGCharacterModelBase;

class CModelComplex {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[668];
    /* 02A0 */ uint32_t unk_02A0;
    /* 02A4 */ uint32_t unk_02A4;
    /* 02A8 */ uint8_t unk_02A8[128];
    /* 0328 */ uint32_t unk_0328;
};

#endif
