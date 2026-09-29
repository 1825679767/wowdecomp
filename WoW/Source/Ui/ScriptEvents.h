#ifndef WOW_SOURCE_UI_SCRIPTEVENTS_H
#define WOW_SOURCE_UI_SCRIPTEVENTS_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Ui/Types.h"

class CGUnit_C;
class SpellRec;

struct UniqueSignal {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ int32_t m_signal;
    /* 000C */ uint32_t unk_000C;
};

extern const char* g_scriptEvents[NUM_SCRIPT_EVENTS];

CGUnit_C* GetUnitFromName(const char* token);
void UnitNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
bool Script_GetGUIDFromString(const char** name, WOWGUID& guid);
bool Script_GetGUIDFromToken(const char* token, WOWGUID& guid, bool defaultToTarget);
void ScriptEventsInitialize();
void ScriptEventsRegisterFunctions();
void SystemRegisterFunctions();
void SystemUnregisterFunctions();
void Script_SendUniqueSignal(const WOWGUID& guid, int32_t signal);
void Script_SendUnitSignal(const WOWGUID& guid, int32_t signal);
void Script_SendUniqueSignal(int32_t signal);
void Script_SendUniqueSignalToAllUnits(int32_t signal);
void Script_SendUniqueSignals();

void Script_SendUnitSpellSignal(const WOWGUID& guid, uint8_t castCount, const SpellRec* spell, int32_t signal);
void ScriptEventsRegisterUnit(CGUnit_C* unit);
void ScriptEventsUnregisterUnit(CGUnit_C* unit);

#endif
