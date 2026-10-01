#include "CGAutoCompleteName.h"
#include "Engine/Source/Storm/H/storm.h"

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
