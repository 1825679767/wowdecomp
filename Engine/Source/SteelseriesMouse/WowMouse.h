#ifndef ENGINE_SOURCE_STEELSERIESMOUSE_WOWMOUSE_H
#define ENGINE_SOURCE_STEELSERIESMOUSE_WOWMOUSE_H

#include <cstddef>
#include <cstdint>

class WowMouse {
    public:
    /* 0004 */ void* m_hidManager;

    virtual ~WowMouse();
    virtual int32_t IsConnected();

    WowMouse();
};

#endif
