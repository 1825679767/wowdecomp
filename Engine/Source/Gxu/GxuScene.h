#ifndef ENGINE_SOURCE_GXU_GXUSCENE_H
#define ENGINE_SOURCE_GXU_GXUSCENE_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class GxuSceneObject {
    public:
    virtual void Unk00();
    virtual void Release();
    virtual bool IsAlive();
    virtual void Update(float elapsed);
};

class GxuScene {
    public:
    /* 0004 */ TSGrowableArray<GxuSceneObject*> m_objects;
    /* 0014 */ uint32_t m_lastUpdateTime;

    virtual ~GxuScene();
    void Clear();
    void Update(uint32_t time);
};

#endif
