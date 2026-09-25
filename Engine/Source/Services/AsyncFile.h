#ifndef ENGINE_SOURCE_SERVICES_ASYNCFILE_H
#define ENGINE_SOURCE_SERVICES_ASYNCFILE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/Types.h"

class AsyncFile {
    public:
    static int32_t Cancel(CAsyncObject* object, void (*callback)(CAsyncObject*));
    static void EnterQueueLock();
    static void LeaveQueueLock();
};

void AsyncFileReadAddPollHandler(void (*handler)());
void AsyncFileReadAddStatusHandler(int32_t (*handler)());
CAsyncObject* AsyncFileReadAllocObject();
void AsyncFileReadDestroyObject(CAsyncObject* object);
void AsyncFileReadIncreasePriority(CAsyncObject* object);
void AsyncFileReadObject(CAsyncObject* object, int32_t urgent);
void AsyncFileReadWait(CAsyncObject* object);

#endif
