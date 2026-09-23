#ifndef WOW_SOURCE_OBJECT_ITEM_H
#define WOW_SOURCE_OBJECT_ITEM_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGItemData;

class CGItem {
    public:
    /* 0000 */ CGItemData* m_item;
    /* 0004 */ void* m_itemMirror;
};

class CGItemData {
    public:
    /* 0000 */ WGUID m_itemFieldOwner;
    /* 0008 */ WGUID m_itemFieldContained;
    /* 0010 */ WGUID m_itemFieldCreator;
    /* 0018 */ WGUID m_itemFieldGiftcreator;
    /* 0020 */ uint32_t m_itemFieldStackCount;
    /* 0024 */ uint32_t m_itemFieldDuration;
    /* 0028 */ uint32_t m_itemFieldSpellCharges [5];
    /* 003C */ uint32_t m_itemFieldFlags;
    /* 0040 */ uint32_t m_itemFieldEnchantment11 [2];
    /* 0048 */ uint16_t m_itemFieldEnchantment13 [2];
    /* 004C */ uint32_t m_itemFieldEnchantment21 [2];
    /* 0054 */ uint16_t m_itemFieldEnchantment23 [2];
    /* 0058 */ uint32_t m_itemFieldEnchantment31 [2];
    /* 0060 */ uint16_t m_itemFieldEnchantment33 [2];
    /* 0064 */ uint32_t m_itemFieldEnchantment41 [2];
    /* 006C */ uint16_t m_itemFieldEnchantment43 [2];
    /* 0070 */ uint32_t m_itemFieldEnchantment51 [2];
    /* 0078 */ uint16_t m_itemFieldEnchantment53 [2];
    /* 007C */ uint32_t m_itemFieldEnchantment61 [2];
    /* 0084 */ uint16_t m_itemFieldEnchantment63 [2];
    /* 0088 */ uint32_t m_itemFieldEnchantment71 [2];
    /* 0090 */ uint16_t m_itemFieldEnchantment73 [2];
    /* 0094 */ uint32_t m_itemFieldEnchantment81 [2];
    /* 009C */ uint16_t m_itemFieldEnchantment83 [2];
    /* 00A0 */ uint32_t m_itemFieldEnchantment91 [2];
    /* 00A8 */ uint16_t m_itemFieldEnchantment93 [2];
    /* 00AC */ uint32_t m_itemFieldEnchantment101 [2];
    /* 00B4 */ uint16_t m_itemFieldEnchantment103 [2];
    /* 00B8 */ uint32_t m_itemFieldEnchantment111 [2];
    /* 00C0 */ uint16_t m_itemFieldEnchantment113 [2];
    /* 00C4 */ uint32_t m_itemFieldEnchantment121 [2];
    /* 00CC */ uint16_t m_itemFieldEnchantment123 [2];
    /* 00D0 */ uint32_t m_itemFieldPropertySeed;
    /* 00D4 */ uint32_t m_itemFieldRandomPropertiesId;
    /* 00D8 */ uint32_t m_itemFieldDurability;
    /* 00DC */ uint32_t m_itemFieldMaxdurability;
    /* 00E0 */ uint32_t m_itemFieldCreatePlayedTime;
    /* 00E4 */ uint32_t m_itemFieldPad;
};

#endif
