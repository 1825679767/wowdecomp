#include "ComSatThread.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Storm/Source/SThread.h"

// STATIC: 0x00D37D20
int32_t ComSatThread::s_threadRunning;

// STATIC: 0x00D37D24
ComSat::Interface* ComSatThread::s_comsat;

// STATIC: 0x00D37D58
SThread ComSatThread::s_comsatThread;

// STATIC: 0x00D37D84
SEvent ComSatThread::s_threadShutdownEvent(0, 0);
