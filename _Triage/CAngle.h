#ifndef _TRIAGE_CANGLE_H
#define _TRIAGE_CANGLE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CBaseManaged;

class CBaseManaged {
    public:
    /* 0004 */ TSLink<CBaseManaged> m_link;
    /* 000C */ uint8_t m_dataTypeId;
    /* 000D */ uint8_t m_flags;
    /* 000E */ uint8_t unk_000E[2];
    /* 0010 */ void (__stdcall* m_updateFcn )(float, void*, void*);
    /* 0014 */ void* m_updateData;
    /* 0018 */ float m_updatePriority;

    virtual ~CBaseManaged();
};

template <class T>
struct TManaged : public CBaseManaged {
    /* 001C */ T m_data;
};

class CAngle : public TManaged<float> {
    public:
    /* 0020 */ float m_cos;
    /* 0024 */ float m_sin;
};

#endif
