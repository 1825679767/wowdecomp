#ifndef ENGINE_SOURCE_FRAME_CSCRIPTREGION_H
#define ENGINE_SOURCE_FRAME_CSCRIPTREGION_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/Types.h"

class C2Vector;
struct CScriptRegionAnimGroupNode;
class CSimpleAnimGroup;
class CSimpleFrame;

class CScriptRegion : public CScriptObject, public CLayoutFrame {
    public:
    /* 0094 */ CSimpleFrame* m_parent;
    /* 0098 */ CScriptRegionAnimGroupNode* m_animGroups;
    /* 009C */ CScriptRegionAnimGroupNode* m_animGroupNode;

    virtual void SetParent(CSimpleFrame* parent);
    virtual int32_t IsDragging();
    virtual void PreOnAnimUpdate();
    virtual void OnAnimUpdate(float elapsedSec);
    virtual void NotifyAnimBegin(CSimpleAnimGroup* group);
    virtual void NotifyAnimEnd(CSimpleAnimGroup* group);
    virtual void StopAnimating();
    virtual void AnimActivated(CSimpleAnimGroup* group, int32_t a2, int32_t a3);
    virtual void AnimDeactivated(CSimpleAnimGroup* group, int32_t a2, int32_t a3);
    virtual void AddAnimTranslation(CScriptRegion* region, const C2Vector& translation);
    virtual void AddAnimRotation(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, float radians);
    virtual void AddAnimScale(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, const C2Vector& scale);
    virtual void AddAnimAlpha(CScriptRegion* region, int16_t alpha);
};

#endif
