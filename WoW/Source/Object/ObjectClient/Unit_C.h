#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_UNIT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_UNIT_C_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/Unit.h"

class AuraData;
struct CGNamePlateFrame;
class CM2Model;
class CMovementShared;
class CharacterComponent;
struct CreatureDisplayInfoExtraRec;
struct CreatureDisplayInfoRec;
struct CreatureModelDataRec;
struct CreatureSoundDataRec;
class CreatureStats_C;
struct UnitBloodLevelsRec;

struct AnimSlot {
    /* 0000 */ uint32_t m_animId;
    /* 0004 */ uint32_t m_variation;
    /* 0008 */ uint32_t m_time;
    /* 000C */ float m_speed;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
};

class CGUnitAuraStuff {
    public:
    /* 0000 */ AuraData* unk_0000;
    /* 0004 */ uint32_t m_auraCount1;
    /* 0008 */ AuraData* m_auradata1;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
};

class CGUnitVirtualItem {
    public:
    /* 0000 */ uint8_t m_classID;
    /* 0001 */ uint8_t m_subclassID;
    /* 0002 */ uint8_t m_soundOverrideSubclassID;
    /* 0003 */ uint8_t m_material;
    /* 0004 */ uint8_t m_inventoryType;
    /* 0005 */ uint8_t m_sheatheType;
    /* 0006 */ uint8_t m_pad [2];
};

class CGUnit_C : public CGObject_C, public CGUnit {
    public:
    /* 00D8 */ CMovementShared* m_passenger;
    /* 00DC */ TSList<void> m_unitMirrorLists [142];
    /* 0784 */ uint32_t unk_0784;
    /* 0788 */ CMovement_C m_movementData;
    /* 08D0 */ uint32_t unk_08D0 [37];
    /* 0964 */ CreatureStats_C* m_creatureCacheEntry;
    /* 0968 */ CreatureDisplayInfoRec* m_displayInfo;
    /* 096C */ CreatureDisplayInfoExtraRec* m_displayInfoExtra;
    /* 0970 */ CreatureModelDataRec* m_modelData;
    /* 0974 */ CreatureSoundDataRec* m_soundData;
    /* 0978 */ uint32_t unk_0978;
    /* 097C */ UnitBloodLevelsRec* m_bloodlevels;
    /* 0980 */ uint32_t unk_0980;
    /* 0984 */ uint32_t unk_0984;
    /* 0988 */ uint32_t unk_0988;
    /* 098C */ uint32_t unk_098C;
    /* 0990 */ uint32_t unk_0990;
    /* 0994 */ uint32_t unk_0994;
    /* 0998 */ uint32_t m_virtualItemDisplayId [3];
    /* 09A4 */ CGUnitVirtualItem m_virtualItem [3];
    /* 09BC */ uint32_t unk_09BC;
    /* 09C0 */ uint32_t unk_09C0;
    /* 09C4 */ uint32_t unk_09C4;
    /* 09C8 */ uint32_t unk_09C8;
    /* 09CC */ uint32_t unk_09CC;
    /* 09D0 */ uint32_t unk_09D0;
    /* 09D4 */ uint32_t m_displayId;
    /* 09D8 */ uint32_t unk_09D8;
    /* 09DC */ uint32_t unk_09DC;
    /* 09E0 */ uint32_t unk_09E0;
    /* 09E4 */ uint32_t unk_09E4;
    /* 09E8 */ uint32_t unk_09E8;
    /* 09EC */ uint32_t unk_09EC;
    /* 09F0 */ uint32_t unk_09F0;
    /* 09F4 */ uint32_t unk_09F4;
    /* 09F8 */ uint32_t unk_09F8;
    /* 09FC */ uint32_t unk_09FC;
    /* 0A00 */ uint32_t m_dataA00;
    /* 0A04 */ uint32_t m_dataA04;
    /* 0A08 */ uint32_t m_dataA08;
    /* 0A0C */ uint32_t m_dataA0C;
    /* 0A10 */ uint32_t m_dataA10;
    /* 0A14 */ uint32_t m_dataA14;
    /* 0A18 */ uint32_t m_dataA18;
    /* 0A1C */ uint32_t m_dataA1C;
    /* 0A20 */ uint32_t m_dataA20;
    /* 0A24 */ uint32_t m_dataA24;
    /* 0A28 */ uint32_t m_dataA28;
    /* 0A2C */ uint32_t m_dataA2C;
    /* 0A30 */ uint32_t unk_0A30;
    /* 0A34 */ uint32_t m_dataA34;
    /* 0A38 */ uint32_t m_animationState;
    /* 0A3C */ uint32_t m_dataA3C;
    /* 0A40 */ uint32_t m_dataA40;
    /* 0A44 */ uint32_t m_dataA44 [20];
    /* 0A94 */ float m_renderFacing;
    /* 0A98 */ float m_facingVelocity;
    /* 0A9C */ float m_facingBlend;
    /* 0AA0 */ float m_targetFacing;
    /* 0AA4 */ float m_dataAA4;
    /* 0AA8 */ float m_turnDelta [4];
    /* 0AB8 */ float m_scriptedFacing;
    /* 0ABC */ uint32_t m_lastTurnTimeMs;
    /* 0AC0 */ uint32_t m_dataAC0;
    /* 0AC4 */ uint32_t m_dataAC4;
    /* 0AC8 */ uint32_t m_dataAC8;
    /* 0ACC */ uint32_t m_dataACC;
    /* 0AD0 */ uint32_t m_dataAD0;
    /* 0AD4 */ uint32_t m_dataAD4;
    /* 0AD8 */ uint32_t m_dataAD8;
    /* 0ADC */ uint32_t m_dataADC;
    /* 0AE0 */ uint32_t m_dataAE0;
    /* 0AE4 */ uint32_t m_dataAE4;
    /* 0AE8 */ uint32_t m_dataAE8;
    /* 0AEC */ uint32_t m_dataAEC;
    /* 0AF0 */ uint32_t m_dataAF0;
    /* 0AF4 */ uint32_t m_dataAF4;
    /* 0AF8 */ uint32_t m_dataAF8 [17];
    /* 0B3C */ uint32_t m_dataB3C;
    /* 0B40 */ CM2Model* m_modelB40;
    /* 0B44 */ CM2Model* m_modelB44;
    /* 0B48 */ uint32_t m_dataB48;
    /* 0B4C */ CharacterComponent* m_characterComponent;
    /* 0B50 */ uint32_t m_weaponTrails [2];
    /* 0B58 */ uint32_t m_dataB58;
    /* 0B5C */ uint32_t m_dataB5C;
    /* 0B60 */ uint32_t m_dataB60;
    /* 0B64 */ uint32_t m_dataB64;
    /* 0B68 */ uint32_t m_dataB68;
    /* 0B6C */ uint32_t m_dataB6C;
    /* 0B70 */ float unk_0B70;
    /* 0B74 */ float unk_0B74;
    /* 0B78 */ float unk_0B78;
    /* 0B7C */ int32_t m_mountAnimBehaviorId;
    /* 0B80 */ int32_t m_animTier;
    /* 0B84 */ int32_t m_torsoKeyBone;
    /* 0B88 */ int32_t m_meleeAttackAnimId;
    /* 0B8C */ int32_t m_pendingAnimId;
    /* 0B90 */ int32_t m_awaitingEndAnimId;
    /* 0B94 */ int32_t unk_0B94;
    /* 0B98 */ uint32_t m_dataB9C;
    /* 0B9C */ uint32_t m_dataB8X [20];
    /* 0BEC */ uint32_t m_dataBEC [19];
    /* 0C38 */ CGNamePlateFrame* m_namePlateFrame;
    /* 0C3C */ uint32_t m_dataC3C;
    /* 0C40 */ uint32_t m_dataC40;
    /* 0C44 */ uint32_t m_dataC44;
    /* 0C48 */ uint32_t m_dataC48;
    /* 0C4C */ uint32_t m_dataC4C;
    /* 0C50 */ CGUnitAuraStuff m_auraData2 [16];
    /* 0DD0 */ uint32_t m_auraCount2;
    /* 0DD4 */ uint32_t m_dataDD4 [34];
    /* 0E5C */ uint32_t m_dataE5C [41];
    /* 0F00 */ uint32_t m_dataF00;
    /* 0F04 */ uint32_t m_dataF04;
    /* 0F08 */ uint32_t m_dataF08;
    /* 0F0C */ uint32_t m_dataF0C;
    /* 0F10 */ uint32_t m_dataF10;
    /* 0F14 */ uint32_t m_dataF14;
    /* 0F18 */ uint32_t m_dataF18;
    /* 0F1C */ uint32_t m_dataF1C;
    /* 0F20 */ uint32_t m_dataF20;
    /* 0F24 */ uint32_t m_dataF24;
    /* 0F28 */ uint32_t m_dataF28;
    /* 0F2C */ uint32_t m_dataF2C;
    /* 0F30 */ uint32_t m_dataF30;
    /* 0F34 */ uint32_t m_dataF34;
    /* 0F38 */ uint32_t m_dataF38;
    /* 0F3C */ uint32_t m_dataF3C;
    /* 0F40 */ uint32_t m_dataF40;
    /* 0F44 */ uint32_t m_dataF44;
    /* 0F48 */ uint32_t m_dataF48;
    /* 0F4C */ uint32_t m_dataF4C;
    /* 0F50 */ uint32_t m_dataF50;
    /* 0F54 */ uint32_t m_dataF54;
    /* 0F58 */ uint32_t m_dataF58;
    /* 0F5C */ uint32_t m_vehicle;
    /* 0F60 */ uint32_t m_vehiclePassenger;
    /* 0F64 */ uint32_t m_vehicleCamera;
    /* 0F68 */ uint32_t m_objectclass1 [38];
    /* 1000 */ uint32_t unk_1000;
    /* 1004 */ uint8_t unk_1004 [4];
};

class CAnimKitManager {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[2944];
    /* 0B84 */ uint32_t unk_0B84;
};

class ClickToMove {
    public:
    /* 0000 */ uint32_t unk_0000;
};

#endif
