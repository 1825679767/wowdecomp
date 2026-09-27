#ifndef WOW_SOURCE_CLIENT_H
#define WOW_SOURCE_CLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SThread.h"
#include "WoW/Source/DB/WowClientDB.h"
#include "WoW/Source/DB/Rec/Startup_StringsRec.h"

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

enum WOWCMDOPT {
    CMD_RES_800x600 = 16,
    CMD_RES_1024x768 = 17,
    CMD_RES_1280x960 = 18,
    CMD_RES_1280x1024 = 19,
    CMD_RES_1600x1200 = 20,
    CMD_UP_TO_DATE = 21,
    CMD_16_BIT = 22,
    CMD_NO_FIX_LAG = 24,
    CMD_NO_SOUND = 26,
    CMD_SOUND_CHAOS = 27,
    CMD_DEPTH_16 = 29,
    CMD_DEPTH_24 = 30,
    CMD_DEPTH_32 = 31,
    CMD_WINDOWED = 32,
    CMD_CONSOLE = 35,
    CMD_HW_DETECT = 36,
    CMD_GX_OVERRIDE = 39,
};

struct ARCHIVEINFO {
    /* 0000 */ const char* m_name;
    /* 0004 */ int32_t m_unk04;
    /* 0008 */ int32_t m_unk08;
    /* 000C */ bool m_opened;
};

extern HEVENTCONTEXT g_clientEventContext;
extern bool g_hasIsoLocale[12];
extern uint8_t s_expansionLevel;
extern int32_t s_locale;
extern WowClientDB<Startup_StringsRec> g_startup_StringsDB;

int32_t CommonMain();
const char* GetInstallLocationString();
int32_t DestroyEngineCallback(const void* data, void* param);
int32_t InitializeEngineCallback(const void* data, void* param);
void StartupReport(bool firstRun);
void BeginCloseGame();
bool InitializeGlobal();
int32_t STORMAPI SendErrorLog(uint32_t errorcode, const char* description, const char* filename, int32_t linenumber, const char* errorstring);
void SetPaths();
int32_t STORMAPI WowLogHeader(char* buffer, uint32_t bufferchars);
int main();

#endif
