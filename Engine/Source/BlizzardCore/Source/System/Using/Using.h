#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_USING_USING_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_USING_USING_H

#include <cstdint>

namespace System_Using {

struct Depends {
    /* 0000 */ int16_t id;
    /* 0002 */ uint8_t unk_0002[2];
    /* 0004 */ Depends* next;
};

struct UsingRec {
    /* 0000 */ char* name;
    /* 0004 */ int16_t id;
    /* 0006 */ int16_t refCount;
    /* 0008 */ void (*shutdownproc)();
    /* 000C */ int16_t dependCount;
    /* 000E */ uint8_t unk_000E[2];
    /* 0010 */ Depends* depends;
};

UsingRec* Intern(const char* name);
void RegisterNull();
void ShutdownUnused();

}

namespace Blizzard {
namespace Using {

void DependsOn(const char* me, const char* dependsOn);
void StartUsing(const char* name);
void StopUsing(const char* name);

}
}

#endif
