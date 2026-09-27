#ifndef WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_CLIENTSERVICES_H
#define WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_CLIENTSERVICES_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

struct RealmInfo {
    /* 0000 */ uint32_t m_id;
    /* 0004 */ uint8_t m_type;
    /* 0005 */ uint8_t m_flags;
    /* 0006 */ char m_realmName [256];
    /* 0106 */ char m_realmAddress [32];
    /* 0126 */ uint8_t unk_0126[2];
    /* 0128 */ float m_population;
    /* 012C */ uint8_t m_numCharacters;
    /* 012D */ uint8_t unk_012D;
    /* 012E */ uint8_t unk_012E;
    /* 012F */ uint8_t unk_012F;
    /* 0130 */ uint32_t unk_0130 [4];
    /* 0140 */ uint8_t m_otherFlags;
    /* 0141 */ uint8_t m_versionMajor;
    /* 0142 */ uint8_t m_versionMinor;
    /* 0143 */ uint8_t m_versionRevision;
    /* 0144 */ uint16_t m_versionBuild;
    /* 0146 */ uint8_t unk_0146[2];
};

struct CLIENTCDKEY {
    /* 0000 */ uint16_t m_length;
    /* 0002 */ uint8_t unk_0002[2];
    /* 0004 */ uint8_t m_key[28];
};

class ClientServices {
    public:
    /* 0000 */ uint8_t unk_0000[12000];
    /* 2EE0 */ uint32_t unk_2EE0[23];
    /* 2F3C */ uint32_t unk_2F3C;
    /* 2F40 */ uint32_t unk_2F40;
    /* 2F44 */ uint32_t unk_2F44;
    /* 2F48 */ uint32_t unk_2F48;
    /* 2F4C */ uint32_t unk_2F4C;
    /* 2F50 */ uint32_t unk_2F50;
    /* 2F54 */ uint32_t unk_2F54;
    /* 2F58 */ uint8_t unk_2F58;
    /* 2F59 */ uint8_t unk_2F59;
    /* 2F5A */ uint8_t unk_2F5A;
    /* 2F5B */ uint8_t unk_2F5B[1];
    /* 2F5C */ uint32_t unk_2F5C;
    /* 2F60 */ uint8_t unk_2F60[11872];

    static RealmInfo* GetSelectedRealm();
    static void InitLoginServerCVars(int32_t overwrite, const char* locale);
    static void LoadCDKey();
};

#endif
