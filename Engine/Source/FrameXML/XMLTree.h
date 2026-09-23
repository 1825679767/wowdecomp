#ifndef ENGINE_SOURCE_FRAMEXML_XMLTREE_H
#define ENGINE_SOURCE_FRAMEXML_XMLTREE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/RCString.h"
#include "Storm/H/stpl.h"

struct XMLAttribute;
class XMLNode;

struct POSITION {
    /* 0000 */ uint32_t m_lineNumber;
    /* 0004 */ uint32_t m_columnNumber;
};

struct XMLAttribute {
    /* 0000 */ RCString m_name;
    /* 000C */ RCString m_value;
};

class XMLNode {
    public:
    /* 0000 */ void* m_userData;
    /* 0004 */ XMLNode* m_parent;
    /* 0008 */ XMLNode* m_child;
    /* 000C */ RCString m_name;
    /* 0018 */ char* m_body;
    /* 001C */ uint32_t m_bodyLen;
    /* 0020 */ TSGrowableArray<XMLAttribute> m_attributes;
    /* 0030 */ uint32_t m_offset;
    /* 0034 */ XMLNode* m_next;
};

#endif
