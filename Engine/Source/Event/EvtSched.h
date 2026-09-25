#ifndef ENGINE_SOURCE_EVENT_EVTSCHED_H
#define ENGINE_SOURCE_EVENT_EVTSCHED_H

#include <cstddef>
#include <cstdint>

class EvtContext;

int32_t IEvtSchedulerIsContextInteractive(EvtContext* context);

#endif
