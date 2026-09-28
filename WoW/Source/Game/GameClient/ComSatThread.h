#ifndef WOW_SOURCE_GAME_GAMECLIENT_COMSATTHREAD_H
#define WOW_SOURCE_GAME_GAMECLIENT_COMSATTHREAD_H

#include <cstddef>
#include <cstdint>

class SEvent;
class SThread;

namespace ComSat {
class Interface;
}

class ComSatThread {
    public:
    static ComSat::Interface* s_comsat;
    static SThread s_comsatThread;
    static int32_t s_threadRunning;
    static SEvent s_threadShutdownEvent;
};

#endif
