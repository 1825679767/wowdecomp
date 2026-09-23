#ifndef ENGINE_SOURCE_BASE_CDATARECYCLER_H
#define ENGINE_SOURCE_BASE_CDATARECYCLER_H

#include <cstddef>
#include <cstdint>

class CDataRecycler {
    public:
    struct Node {
        /* 0000 */ CDataRecycler::Node* m_next;
        /* 0004 */ void* m_data;
        /* 0008 */ uint32_t m_bytes;
    };
    struct NodeBlock {
        /* 0000 */ CDataRecycler::NodeBlock* m_next;
        /* 0004 */ CDataRecycler::Node m_nodes [1];
    };

    /* 0004 */ int32_t m_nodesRecyclable;
    /* 0008 */ uint32_t m_nodesPerBlock;
    /* 000C */ CDataRecycler::NodeBlock* m_nodeBlockList;
    /* 0010 */ CDataRecycler::Node* m_nodeFullList;
    /* 0014 */ CDataRecycler::Node* m_nodeEmptyList;

    virtual ~CDataRecycler();
};

#endif
