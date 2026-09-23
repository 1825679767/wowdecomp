#ifndef ENGINE_SOURCE_BASE_STATUS_H
#define ENGINE_SOURCE_BASE_STATUS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/Types.h"
#include "Storm/H/stpl.h"

class CStatus {
    public:
    struct STATUSENTRY {
        /* 0000 */ char* m_text;
        /* 0004 */ STATUS_TYPE m_severity;
        /* 0008 */ TSLink<CStatus::STATUSENTRY> m_link;
    };

    /* 0000 */ TSExplicitList<CStatus::STATUSENTRY> m_statusList;
};

#endif
