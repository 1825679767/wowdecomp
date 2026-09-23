#ifndef WOW_SOURCE_OBJECT_UNIT_H
#define WOW_SOURCE_OBJECT_UNIT_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGUnitData;

class CGUnit {
    public:
    /* 0000 */ CGUnitData* m_unit;
    /* 0004 */ void* m_unitMirror;
};

class CGUnitData {
    public:
    /* 0000 */ WGUID m_unitFieldCharm;
    /* 0008 */ WGUID m_unitFieldSummon;
    /* 0010 */ WGUID m_unitFieldCritter;
    /* 0018 */ WGUID m_unitFieldCharmedby;
    /* 0020 */ WGUID m_unitFieldSummonedby;
    /* 0028 */ WGUID m_unitFieldCreatedby;
    /* 0030 */ WGUID m_unitFieldTarget;
    /* 0038 */ WGUID m_unitFieldChannelObject;
    /* 0040 */ uint32_t m_unitChannelSpell;
    /* 0044 */ uint32_t m_unitFieldBytes0;
    /* 0048 */ uint32_t m_unitFieldHealth;
    /* 004C */ uint32_t m_unitFieldPower1;
    /* 0050 */ uint32_t m_unitFieldPower2;
    /* 0054 */ uint32_t m_unitFieldPower3;
    /* 0058 */ uint32_t m_unitFieldPower4;
    /* 005C */ uint32_t m_unitFieldPower5;
    /* 0060 */ uint32_t m_unitFieldPower6;
    /* 0064 */ uint32_t m_unitFieldPower7;
    /* 0068 */ uint32_t m_unitFieldMaxhealth;
    /* 006C */ uint32_t m_unitFieldMaxpower1;
    /* 0070 */ uint32_t m_unitFieldMaxpower2;
    /* 0074 */ uint32_t m_unitFieldMaxpower3;
    /* 0078 */ uint32_t m_unitFieldMaxpower4;
    /* 007C */ uint32_t m_unitFieldMaxpower5;
    /* 0080 */ uint32_t m_unitFieldMaxpower6;
    /* 0084 */ uint32_t m_unitFieldMaxpower7;
    /* 0088 */ float m_unitFieldPowerRegenFlatModifier [7];
    /* 00A4 */ float m_unitFieldPowerRegenInterruptedFlatModifier [7];
    /* 00C0 */ uint32_t m_unitFieldLevel;
    /* 00C4 */ uint32_t m_unitFieldFactiontemplate;
    /* 00C8 */ uint32_t m_unitVirtualItemSlotId [3];
    /* 00D4 */ uint32_t m_unitFieldFlags;
    /* 00D8 */ uint32_t m_unitFieldFlags2;
    /* 00DC */ uint32_t m_unitFieldAurastate;
    /* 00E0 */ uint32_t m_unitFieldBaseattacktime [2];
    /* 00E8 */ uint32_t m_unitFieldRangedattacktime;
    /* 00EC */ float m_unitFieldBoundingradius;
    /* 00F0 */ float m_unitFieldCombatreach;
    /* 00F4 */ uint32_t m_unitFieldDisplayid;
    /* 00F8 */ uint32_t m_unitFieldNativedisplayid;
    /* 00FC */ uint32_t m_unitFieldMountdisplayid;
    /* 0100 */ float m_unitFieldMindamage;
    /* 0104 */ float m_unitFieldMaxdamage;
    /* 0108 */ float m_unitFieldMinoffhanddamage;
    /* 010C */ float m_unitFieldMaxoffhanddamage;
    /* 0110 */ uint32_t m_unitFieldBytes1;
    /* 0114 */ uint32_t m_unitFieldPetnumber;
    /* 0118 */ uint32_t m_unitFieldPetNameTimestamp;
    /* 011C */ uint32_t m_unitFieldPetexperience;
    /* 0120 */ uint32_t m_unitFieldPetnextlevelexp;
    /* 0124 */ uint32_t m_unitDynamicFlags;
    /* 0128 */ float m_unitModCastSpeed;
    /* 012C */ uint32_t m_unitCreatedBySpell;
    /* 0130 */ uint32_t m_unitNpcFlags;
    /* 0134 */ uint32_t m_unitNpcEmotestate;
    /* 0138 */ uint32_t m_unitFieldStat0;
    /* 013C */ uint32_t m_unitFieldStat1;
    /* 0140 */ uint32_t m_unitFieldStat2;
    /* 0144 */ uint32_t m_unitFieldStat3;
    /* 0148 */ uint32_t m_unitFieldStat4;
    /* 014C */ uint32_t m_unitFieldPosstat0;
    /* 0150 */ uint32_t m_unitFieldPosstat1;
    /* 0154 */ uint32_t m_unitFieldPosstat2;
    /* 0158 */ uint32_t m_unitFieldPosstat3;
    /* 015C */ uint32_t m_unitFieldPosstat4;
    /* 0160 */ uint32_t m_unitFieldNegstat0;
    /* 0164 */ uint32_t m_unitFieldNegstat1;
    /* 0168 */ uint32_t m_unitFieldNegstat2;
    /* 016C */ uint32_t m_unitFieldNegstat3;
    /* 0170 */ uint32_t m_unitFieldNegstat4;
    /* 0174 */ uint32_t m_unitFieldResistances [7];
    /* 0190 */ uint32_t m_unitFieldResistancebuffmodspositive [7];
    /* 01AC */ uint32_t m_unitFieldResistancebuffmodsnegative [7];
    /* 01C8 */ uint32_t m_unitFieldBaseMana;
    /* 01CC */ uint32_t m_unitFieldBaseHealth;
    /* 01D0 */ uint32_t m_unitFieldBytes2;
    /* 01D4 */ uint32_t m_unitFieldAttackPower;
    /* 01D8 */ uint32_t m_unitFieldAttackPowerMods;
    /* 01DC */ float m_unitFieldAttackPowerMultiplier;
    /* 01E0 */ uint32_t m_unitFieldRangedAttackPower;
    /* 01E4 */ uint32_t m_unitFieldRangedAttackPowerMods;
    /* 01E8 */ float m_unitFieldRangedAttackPowerMultiplier;
    /* 01EC */ float m_unitFieldMinrangeddamage;
    /* 01F0 */ float m_unitFieldMaxrangeddamage;
    /* 01F4 */ uint32_t m_unitFieldPowerCostModifier [7];
    /* 0210 */ float m_unitFieldPowerCostMultiplier [7];
    /* 022C */ float m_unitFieldMaxhealthmodifier;
    /* 0230 */ float m_unitFieldHoverheight;
    /* 0234 */ uint32_t m_unitFieldPadding;
};

#endif
