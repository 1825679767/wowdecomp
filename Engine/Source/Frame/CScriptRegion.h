#ifndef ENGINE_SOURCE_FRAME_CSCRIPTREGION_H
#define ENGINE_SOURCE_FRAME_CSCRIPTREGION_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "_Triage/CScriptObject.h"

struct CScriptRegionAnimGroupNode;
struct CSimpleFrame;

class CScriptRegion : public CScriptObject {
    public:
    /* 0020 */ CLayoutFrame m_baseLayoutFrame;
    /* 0094 */ CSimpleFrame* m_parent;
    /* 0098 */ CScriptRegionAnimGroupNode* m_animGroups;
    /* 009C */ CScriptRegionAnimGroupNode* m_animGroupNode;
};

#endif
