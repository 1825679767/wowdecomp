#include "CGAutoCompleteName.h"
#include "Engine/Source/Storm/H/storm.h"
#include <new>
#include <typeinfo>
#include "WoW/Source/WorldClient/FrameTime.h"

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

// VERIFY: 70f6d109 manual
// OFFSET: 0x0057B330
void CGAutoCompleteName::ResortAll() {
    for (AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::m_list.Head(); !CGAutoCompleteName::m_list.IsEnd(node); ) {
        AUTOCOMPLETE_NAMENODE* next = node->RawNext();
        SortNameNode(node);
        node = next;
    }
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
