#ifndef ENGINE_SOURCE_SERVICES_GFXSINGLETONMANAGER_H
#define ENGINE_SOURCE_SERVICES_GFXSINGLETONMANAGER_H

#include <cstddef>
#include <cstdint>

class C3Vector;

class ParticleSystemManager {
    public:
    static int32_t (*sm_projectCallback)(const C3Vector&, float&, void*);
    static void* sm_projectParam;

    static void Destroy();

    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint8_t unk_000C[8];
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint8_t unk_001C[8];
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint8_t unk_002C[8];
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
};

class RibbonManager {
    public:
    static void Destroy();
};

#endif
