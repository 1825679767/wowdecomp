#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_ITEM_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_ITEM_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

class CGItem_C : public CGObject_C, public CGItem {
    public:
    /* 00D8 */ void* unk_00D8;
    /* 00DC */ uint32_t unk_00DC [192];
    /* 03DC */ uint8_t unk_03DC [4];
};

class ItemCache {
    public:
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_class;
    /* 0008 */ int32_t m_subClass;
    /* 000C */ int32_t m_unkInt;
    /* 0010 */ int32_t m_displayId;
    /* 0014 */ int32_t m_quality;
    /* 0018 */ int32_t m_flagsAndFaction [2];
    /* 0020 */ int32_t m_buyPrice;
    /* 0024 */ int32_t m_sellPrice;
    /* 0028 */ int32_t m_invType;
    /* 002C */ int32_t m_allowClass;
    /* 0030 */ int32_t m_allowRace;
    /* 0034 */ int32_t m_itemLvl;
    /* 0038 */ int32_t m_reqLvl;
    /* 003C */ int32_t m_reqSkill;
    /* 0040 */ int32_t m_reqSkillRank;
    /* 0044 */ int32_t m_reqSpell;
    /* 0048 */ int32_t m_reqHonor;
    /* 004C */ int32_t m_reqCityRank;
    /* 0050 */ int32_t m_reqRepFaction;
    /* 0054 */ int32_t m_reqRepRank;
    /* 0058 */ int32_t m_maxCount;
    /* 005C */ int32_t m_stackable;
    /* 0060 */ int32_t m_containerSlots;
    /* 0064 */ int32_t m_statsCount;
    /* 0068 */ int32_t m_stats [10][2];
    /* 00B8 */ int32_t m_scalingStatDistribution;
    /* 00BC */ int32_t m_scalingStatValue;
    /* 00C0 */ float m_sSDDmg1 [2];
    /* 00C8 */ float m_sSDDmg2 [2];
    /* 00D0 */ int32_t m_sSDDmgType [2];
    /* 00D8 */ int32_t m_resistance [7];
    /* 00F4 */ int32_t m_delay;
    /* 00F8 */ int32_t m_ammoType;
    /* 00FC */ float m_rangedModRange;
    /* 0100 */ int32_t m_spellId [5];
    /* 0114 */ int32_t m_spellTrigger [5];
    /* 0128 */ int32_t m_spellCharges [5];
    /* 013C */ int32_t m_spellCooldown [5];
    /* 0150 */ int32_t m_spellCategory [5];
    /* 0164 */ int32_t m_spellCatCooldown [5];
    /* 0178 */ int32_t m_bonding;
    /* 017C */ char* m_description;
    /* 0180 */ int32_t m_pageTextId;
    /* 0184 */ int32_t m_languageId;
    /* 0188 */ int32_t m_pageMaterial;
    /* 018C */ int32_t m_startQuest;
    /* 0190 */ int32_t m_lockId;
    /* 0194 */ int32_t m_material;
    /* 0198 */ int32_t m_sheath;
    /* 019C */ int32_t m_randomProperty;
    /* 01A0 */ int32_t m_randomSuffix;
    /* 01A4 */ int32_t m_block;
    /* 01A8 */ int32_t m_itemSetId;
    /* 01AC */ int32_t m_maxDurability;
    /* 01B0 */ int32_t m_area;
    /* 01B4 */ int32_t m_map;
    /* 01B8 */ int32_t m_bagFamily;
    /* 01BC */ int32_t m_totemCategory;
    /* 01C0 */ int32_t m_socketColor [3];
    /* 01CC */ int32_t m_socketItem [3];
    /* 01D8 */ int32_t m_socketBonus;
    /* 01DC */ int32_t m_gemProperties;
    /* 01E0 */ int32_t m_reqDisenchantSkill;
    /* 01E4 */ float m_armorDmgMod;
    /* 01E8 */ int32_t m_duration;
    /* 01EC */ int32_t m_itemLimitCat;
    /* 01F0 */ int32_t m_holiday;
    /* 01F4 */ char m_name [4][400];
};

#endif
