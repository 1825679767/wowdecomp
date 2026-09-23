#ifndef WOW_SOURCE_DB_DBCLIENT_CREATURESTATS_C_H
#define WOW_SOURCE_DB_DBCLIENT_CREATURESTATS_C_H

#include <cstddef>
#include <cstdint>

class CreatureStats_C {
    public:
    /* 0004 */ char* m_subName;
    /* 0008 */ char* m_iconName;
    /* 000C */ int32_t m_typeFlags;
    /* 0010 */ int32_t m_type;
    /* 0014 */ int32_t m_family;
    /* 0018 */ int32_t m_rank;
    /* 001C */ int32_t m_killCredit1;
    /* 0020 */ int32_t m_killCredit2;
    /* 0024 */ int32_t m_displayId1;
    /* 0028 */ int32_t m_displayId2;
    /* 002C */ int32_t m_displayId3;
    /* 0030 */ int32_t m_displayId4;
    /* 0034 */ float m_hpModifier;
    /* 0038 */ float m_mpModifier;
    /* 003C */ bool m_racialLeader;
    /* 003D */ uint8_t unk_003D[3];
    /* 0040 */ int32_t m_questItem [6];
    /* 0058 */ int32_t m_movementId;
    /* 005C */ char* m_name [4];

    virtual ~CreatureStats_C();
};

#endif
