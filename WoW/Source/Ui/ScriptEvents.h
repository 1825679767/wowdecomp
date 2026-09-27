#ifndef WOW_SOURCE_UI_SCRIPTEVENTS_H
#define WOW_SOURCE_UI_SCRIPTEVENTS_H

#include <cstdint>

struct UniqueSignal {
    /* 0000 */ uint64_t m_guid;
    /* 0008 */ int32_t m_signal;
    /* 000C */ uint32_t unk_000C;
};

extern const char* g_scriptEvents[722];

void ScriptEventsInitialize();
void SystemRegisterFunctions();
void Script_SendUniqueSignal(const uint64_t& guid, int32_t signal);
void Script_SendUniqueSignal(int32_t signal);

#endif
