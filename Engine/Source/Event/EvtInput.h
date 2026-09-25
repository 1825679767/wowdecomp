#ifndef ENGINE_SOURCE_EVENT_EVTINPUT_H
#define ENGINE_SOURCE_EVENT_EVTINPUT_H

#include <cstdint>

class EvtContext;

void IEvtInputProcess(EvtContext* context, int32_t* shutdown);

#endif
