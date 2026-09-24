#ifndef ENGINE_SOURCE_CONSOLEBASE_CONSOLEVAR_H
#define ENGINE_SOURCE_CONSOLEBASE_CONSOLEVAR_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Storm/H/stpl.h"

class CVar;
struct HASHKEY_STRI;

class CVar : public TSHashObject<CVar, HASHKEY_STRI> {
    public:
    /* 0018 */ uint32_t m_category;
    /* 001C */ uint32_t m_flags;
    /* 0020 */ RCString m_stringValue;
    /* 002C */ float m_floatValue;
    /* 0030 */ int32_t m_intValue;
    /* 0034 */ int32_t m_modified;
    /* 0038 */ RCString m_defaultValue;
    /* 0044 */ RCString m_resetValue;
    /* 0050 */ RCString m_latchedValue;
    /* 005C */ RCString m_help;
    /* 0068 */ bool (__stdcall* m_callback )(CVar*, const char*, const char*, void*);
    /* 006C */ void* m_arg;
};

class CVarArrayWrapper {
    public:
    /* 0000 */ uint32_t unk_0000;
};

class CVarIntListWrapper {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[256];
    /* 0108 */ uint32_t unk_0108;
};

#endif
