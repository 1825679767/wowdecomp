#ifndef WOW_SOURCE_CLIENT_H
#define WOW_SOURCE_CLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SThread.h"

class CAsyncObject;
class CAsyncQueue;
class CAsyncThread;
struct CObjectHeapList;

class CAsyncQueue {
    public:
    /* 0000 */ TSLink<CAsyncQueue> m_link;
    /* 0008 */ TSExplicitList<CAsyncObject> m_readList;
    /* 0014 */ TSExplicitList<CAsyncObject> m_unkList;
    /* 0020 */ int32_t unk_0020;
};

class CAsyncThread {
    public:
    /* 0000 */ TSLink<CAsyncThread> m_list;
    /* 0008 */ SThread m_thread;
    /* 000C */ CAsyncQueue* m_queue;
    /* 0010 */ CAsyncObject* m_currentObject;
};

struct OBJALLOCGLOBALS {
    /* 0000 */ TSGrowableArray<CObjectHeapList> m_objects;
};

extern int32_t s_locale;

void SetPaths();

#endif
