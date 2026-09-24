#ifndef WOW_SOURCE_WOWSERVICES_REALMCONNECTION_H
#define WOW_SOURCE_WOWSERVICES_REALMCONNECTION_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

class CHARACTER_INFO;
struct RealmResponse;

class CHARACTER_INFO {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ char m_name [48];
    /* 0038 */ uint32_t m_mapID;
    /* 003C */ uint32_t m_zoneID;
    /* 0040 */ uint32_t m_guildID;
    /* 0044 */ C3Vector m_position;
    /* 0050 */ uint32_t m_inventoryItemDisplayID [23];
    /* 00AC */ uint32_t m_inventoryItemType [23];
    /* 0108 */ uint32_t m_inventoryItemVisualID [23];
    /* 0164 */ uint32_t m_petDisplayInfoID;
    /* 0168 */ uint32_t m_petExperienceLevel;
    /* 016C */ uint32_t m_petCreatureFamilyID;
    /* 0170 */ uint32_t m_flags;
    /* 0174 */ uint32_t m_customizeFlags;
    /* 0178 */ uint8_t m_raceID;
    /* 0179 */ uint8_t m_classID;
    /* 017A */ uint8_t m_sexID;
    /* 017B */ uint8_t m_skinColorID;
    /* 017C */ uint8_t m_faceID;
    /* 017D */ uint8_t m_hairStyleID;
    /* 017E */ uint8_t m_hairColorID;
    /* 017F */ uint8_t m_facialHairStyleID;
    /* 0180 */ uint8_t m_experienceLevel;
    /* 0181 */ uint8_t m_firstLogin;
    /* 0182 */ uint8_t unk_0182[6];
};

class RealmConnection {
    public:
    /* 0000 */ NetClient unk_0000;
    /* 2EE0 */ RealmResponse* m_realmResponse;
    /* 2EE4 */ uint32_t m_restrictHuman;
    /* 2EE8 */ uint32_t m_restrictDwarf;
    /* 2EEC */ uint32_t m_restrictGnome;
    /* 2EF0 */ uint32_t m_restrictNightElf;
    /* 2EF4 */ uint32_t m_restrictDraenei;
    /* 2EF8 */ uint32_t m_restrictOrc;
    /* 2EFC */ uint32_t m_restrictTroll;
    /* 2F00 */ uint32_t m_restrictTauren;
    /* 2F04 */ uint32_t m_restrictUndead;
    /* 2F08 */ uint32_t m_restrictBloodElf;
    /* 2F0C */ TSFixedArray<CHARACTER_INFO> m_characterList;
    /* 2F18 */ uint8_t m_authenticated;
    /* 2F19 */ uint8_t unk_2F19[3];
    /* 2F1C */ uint32_t m_queuePosition;
    /* 2F20 */ uint32_t m_freeCharacterMigration;
    /* 2F24 */ uint32_t m_billingTimeRemaining;
    /* 2F28 */ uint32_t m_billingTimeRested;
    /* 2F2C */ uint8_t m_billingFlags;
    /* 2F2D */ uint8_t m_accountExpansion;
    /* 2F2E */ uint8_t unk_2F2E[2];
};

#endif
