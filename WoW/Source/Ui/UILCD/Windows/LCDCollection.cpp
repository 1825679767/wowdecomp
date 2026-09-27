#include "WoW/Source/Ui/UILCD/Windows/LCDCollection.h"

// VERIFY: 33719c71 manual
// OFFSET: 0x0095B820
CLCDCollection::CLCDCollection() {
}

// VERIFY: 246f193d manual
// OFFSET: 0x0095B950
CLCDCollection::~CLCDCollection() {
    for (CLCDBASE_NODE* node = this->m_Objects.Head(); node; node = this->m_Objects.Next(node)) {
        delete node->m_pObject;
        node->m_pObject = nullptr;
    }

    this->m_Objects.Clear();
}
