#ifndef ENGINE_SOURCE_GXU_GXUSCENE_H
#define ENGINE_SOURCE_GXU_GXUSCENE_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class GxuSceneObject {
    public:
    virtual void Unk00();
    virtual void Unk04();
};

class GxuScene {
    public:
    /* 0004 */ TSGrowableArray<GxuSceneObject*> m_objects;

    virtual ~GxuScene();
    void Clear();
};

#endif
