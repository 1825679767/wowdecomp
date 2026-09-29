#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITCOMBATLOG_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITCOMBATLOG_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Storm/H/stpl.h"

class SpellRec;

struct CombatLogEntry : public TSLinkedNode<CombatLogEntry> {
    /* 0008 */ uint32_t m_timestamp;
    /* 000C */ int32_t m_event;
    /* 0010 */ uint32_t m_pendingMask;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ WOWGUID m_sourceGuid;
    /* 0020 */ const char* m_sourceName;
    /* 0024 */ const char* m_sourceRealm;
    /* 0028 */ uint32_t m_sourceFlags;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ WOWGUID m_destGuid;
    /* 0038 */ const char* m_destName;
    /* 003C */ const char* m_destRealm;
    /* 0040 */ uint32_t m_destFlags;
    /* 0044 */ int32_t m_spellId;
    /* 0048 */ RCString m_spellName;
    /* 0054 */ uint32_t m_infoFlags;
    /* 0058 */ union {
        char* m_text;
        int32_t m_values[7];
    } m_info;
    /* 0074 */ uint8_t m_infoBits;

    void Clear();
    void SetFailReason(const char* reason);
    void SetMissInfo(int32_t missType, int32_t amount);
};

struct InvalidatedName : public TSLinkedNode<InvalidatedName> {
    /* 0008 */ char* m_name;

    InvalidatedName();
    ~InvalidatedName();
};

struct PendingCombatLogData : public TSLinkedNode<PendingCombatLogData> {
    /* 0008 */ CombatLogEntry* m_entry;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ int32_t m_id;
    /* 001C */ int32_t m_type;
    /* 0020 */ uint32_t m_mask;
    /* 0024 */ uint32_t unk_0024;
};

struct CombatLogFilter : public TSLinkedNode<CombatLogFilter> {
    /* 0008 */ TSBitField<TSFixedArray<uint32_t>> m_events;
    /* 0018 */ WOWGUID m_sourceGuid;
    /* 0020 */ WOWGUID m_destGuid;
    /* 0028 */ uint32_t m_sourceFlags;
    /* 002C */ uint32_t m_destFlags;
    /* 0030 */ int32_t m_spellId;
    /* 0034 */ RCString m_spellName;

    bool SetFilterData(const char* events, const WOWGUID& sourceGuid, uint32_t sourceFlags, const WOWGUID& destGuid, uint32_t destFlags, int32_t spellId, const char* spellName);
};

PendingCombatLogData* GetPendingCombatLogData(CombatLogEntry* entry, const WOWGUID& guid, uint32_t mask);
void UnitCombatLogInvalidateName(const char* name);
void __stdcall CheckToSanitizeUnitLowGuid(WOWGUID* guid);
int32_t IsCombatTextActiveUnit(WOWGUID guid);
void SendToCombatText(int32_t type, const char* text, int32_t amount);
void SendToCombatText(int32_t type, const char* text, int32_t amount, int32_t overflow);
void SendToCombatText(int32_t type, int32_t amount, int32_t overflow);
void SendToCombatText(int32_t type, int32_t amount);
void SendToCombatText(int32_t type, const char* text);
void SendToCombatText(int32_t type);
void UnitCombatLogProcGo(const WOWGUID& casterGuid, const SpellRec* spell);

#endif
