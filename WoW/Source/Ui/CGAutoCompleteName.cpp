#include "CGAutoCompleteName.h"
#include "Engine/Source/Storm/H/storm.h"
#include <new>
#include <typeinfo>
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: 0x00ACEB70
TSList<AUTOCOMPLETE_NAMENODE, TSGetLink<AUTOCOMPLETE_NAMENODE>> CGAutoCompleteName::m_list;

// STATIC: 0x00BEB994
bool CGAutoCompleteName::s_resortNamesOnRecency;

// STATIC: 0x00BEB995
bool CGAutoCompleteName::s_whenEditingFromCenter;

// STATIC: 0x00BEB996
bool CGAutoCompleteName::s_useContext;

// VERIFY: be0e83f7 manual
// OFFSET: 0x0057B130
static void SortNameNode(AUTOCOMPLETE_NAMENODE* node) {
    for (AUTOCOMPLETE_NAMENODE* current = CGAutoCompleteName::m_list.Head(); !CGAutoCompleteName::m_list.IsEnd(current); current = current->RawNext()) {
        if (CGAutoCompleteName::s_resortNamesOnRecency) {
            if (current->m_lastUsed > node->m_lastUsed) {
                continue;
            }

            if (current->m_lastUsed == node->m_lastUsed && SStrCmpUTF8I(current->m_name, node->m_name, 0x7FFFFFFF) < 0) {
                continue;
            }
        } else if (SStrCmpUTF8I(current->m_name, node->m_name, 0x7FFFFFFF) < 0) {
            continue;
        }

        if (current == node) {
            continue;
        }

        CGAutoCompleteName::m_list.LinkNode(node, 2, current);
        return;
    }

    CGAutoCompleteName::m_list.LinkToTail(node);
}

// VERIFY: 22923be0 manual
// OFFSET: 0x0057B250
int32_t CGAutoCompleteName::GetMatchingResults(lua_State* L, const char* text, uint32_t include, uint32_t exclude, int32_t numReturns, int32_t cursor, bool allowFullMatch) {
    int32_t count = 0;

    for (AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::m_list.Head(); node; node = CGAutoCompleteName::m_list.Next(node)) {
        if (!CGAutoCompleteName::NodeMatchesParams(node, text, include, exclude, cursor, allowFullMatch)) {
            continue;
        }

        lua_pushstring(L, node->m_name);

        if (++count == numReturns) {
            break;
        }
    }

    return count;
}

// VERIFY: dd3503e3 manual
// OFFSET: 0x0057B2C0
int32_t CGAutoCompleteName::GetPresenceID(const char* name, uint32_t* presenceId) {
    for (AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::m_list.Head(); node; node = CGAutoCompleteName::m_list.Next(node)) {
        if (node->m_presenceId && !SStrCmpUTF8I(name, node->m_name, STORM_MAX_STR)) {
            *presenceId = node->m_presenceId;
            return 1;
        }
    }

    return 0;
}

// VERIFY: 70f6d109 manual
// OFFSET: 0x0057B330
void CGAutoCompleteName::ResortAll() {
    for (AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::m_list.Head(); !CGAutoCompleteName::m_list.IsEnd(node); ) {
        AUTOCOMPLETE_NAMENODE* next = node->RawNext();
        SortNameNode(node);
        node = next;
    }
}

// VERIFY: 70e01eea manual
// OFFSET: 0x0057ACA0
int32_t CGAutoCompleteName::NodeMatchesParams(AUTOCOMPLETE_NAMENODE* node, const char* text, uint32_t include, uint32_t exclude, int32_t cursor, bool allowFullMatch) {
    if (CGAutoCompleteName::s_useContext && ((node->m_flags & exclude) || !(node->m_flags & include))) {
        return 0;
    }

    if (node->m_guid == ClientServices::GetCharacterInfo()->m_guid) {
        return 0;
    }

    if (!allowFullMatch) {
        int32_t result = SStrCmpUTF8I(text, node->m_name, STORM_MAX_STR);

        if (!result) {
            return result;
        }
    }

    if (!CGAutoCompleteName::s_whenEditingFromCenter) {
        return SStrCmpUTF8I(text, node->m_name, SStrLenUTF8(text)) ? 0 : 1;
    }

    if (SStrCmpUTF8I(text, node->m_name, cursor)) {
        return 0;
    }

    int32_t nameLength = SStrLenUTF8(node->m_name);
    int32_t textLength = SStrLenUTF8(text);
    int32_t tail = textLength - cursor;

    if (nameLength < textLength) {
        return 0;
    }

    if (textLength == cursor) {
        return 1;
    }

    const char* namePosition = SStrAdvanceCharsUTF8(node->m_name, cursor);
    int32_t remaining = nameLength - tail - cursor;
    const char* textPosition = SStrAdvanceCharsUTF8(text, cursor);

    if (!remaining) {
        return 0;
    }

    while (SStrCmpUTF8I(namePosition, textPosition, tail)) {
        if (!--remaining) {
            return 0;
        }

        namePosition = SStrAdvanceCharsUTF8(namePosition, 1);
    }

    return 1;
}

// VERIFY: cd399474 manual
// OFFSET: 0x0057AE80
static AUTOCOMPLETE_NAMENODE* FindNodeByGuid(const WOWGUID& guid) {
    for (AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::m_list.Head(); node; node = CGAutoCompleteName::m_list.Next(node)) {
        if (!node->m_presenceId && node->m_guid == guid) {
            return node;
        }
    }

    return nullptr;
}

// VERIFY: 040c821a auto
// OFFSET: 0x0057B630
static int32_t ApplyNodeFlags(AUTOCOMPLETE_NAMENODE* node, uint32_t flags, int32_t touch, const char* name) {
    if (!node) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    node->m_flags |= flags;

    if (touch) {
        node->m_lastUsed = FrameTime::s_curTimeMs;
    }

    if (name && !node->m_name[0]) {
        SStrCopy(node->m_name, name, 305);
    }

    SortNameNode(node);
    return 1;
}

// VERIFY: 4d5b0997 manual
// OFFSET: 0x0057B6A0
static AUTOCOMPLETE_NAMENODE* AllocNameNode(const WOWGUID& guid, uint32_t flags, uint32_t lastUsed, const char* name) {
    AUTOCOMPLETE_NAMENODE* node = new (SMemAlloc(sizeof(AUTOCOMPLETE_NAMENODE), typeid(AUTOCOMPLETE_NAMENODE).raw_name(), -2, 0x8)) AUTOCOMPLETE_NAMENODE;
    node->m_presenceId = 0;
    node->m_guid = guid;

    if (name) {
        SStrCopy(node->m_name, name, 305);
    } else {
        node->m_name[0] = '\0';
    }

    if (node->m_lastUsed != lastUsed) {
        node->m_lastUsed = lastUsed;
        SortNameNode(node);
    }

    node->m_flags = flags;
    SortNameNode(node);
    return node;
}

// VERIFY: d91cc811 auto
// OFFSET: 0x0057B7C0
void CGAutoCompleteName::AddFlags(const WOWGUID& guid, uint32_t flags, int32_t add, const char* name) {
    if (guid == WOWGUID()) {
        return;
    }

    if (flags & 0x8) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    AUTOCOMPLETE_NAMENODE* node = FindNodeByGuid(guid);

    if (node) {
        ApplyNodeFlags(node, flags, add, name);
    } else {
        AllocNameNode(guid, flags, add ? FrameTime::s_curTimeMs : 0, name);
    }
}

// VERIFY: 1e7342ce auto
// OFFSET: 0x0057AF80
int32_t CGAutoCompleteName::SetName(const WOWGUID& guid, const char* name, bool overwrite) {
    AUTOCOMPLETE_NAMENODE* node = FindNodeByGuid(guid);

    if (!node) {
        return 0;
    }

    if (node->m_name[0] && !overwrite) {
        return 0;
    }

    SStrCopy(node->m_name, name, 305);
    return 1;
}

// VERIFY: 912690b2 auto
// OFFSET: 0x0057B000
int32_t CGAutoCompleteName::RemoveFlags(const WOWGUID& guid, uint32_t flags) {
    AUTOCOMPLETE_NAMENODE* node = FindNodeByGuid(guid);

    if (node) {
        node->m_flags &= ~flags;

        if (!(node->m_flags & ~0x20)) {
            CGAutoCompleteName::m_list.DeleteNode(node);
        }
    }

    return 1;
}
