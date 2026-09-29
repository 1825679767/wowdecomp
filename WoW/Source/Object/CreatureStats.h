#ifndef WOW_SOURCE_OBJECT_CREATURESTATS_H
#define WOW_SOURCE_OBJECT_CREATURESTATS_H

#include <cstddef>
#include <cstdint>

class CDataStore;

class CreatureStats {
    public:
    /* 0004 */ char* m_subName;
    /* 0008 */ char* m_iconName;
    /* 000C */ uint32_t m_typeFlags;
    /* 0010 */ uint32_t m_type;
    /* 0014 */ uint32_t m_family;
    /* 0018 */ uint32_t m_rank;
    /* 001C */ uint32_t m_killCredit [2];
    /* 0024 */ uint32_t m_displayId [4];
    /* 0034 */ float m_hpModifier;
    /* 0038 */ float m_mpModifier;
    /* 003C */ bool m_racialLeader;
    /* 0040 */ uint32_t m_questItem [6];
    /* 0058 */ uint32_t m_movementId;
    /* 005C */ char* m_name [4];

    virtual ~CreatureStats();

    const char* GetName(int32_t index) const;
    void Pack(CDataStore* msg) const;
    void Unpack(CDataStore* msg);
};

#endif
