#ifndef WOW_SOURCE_DB_REC_SCALINGSTATVALUESREC_H
#define WOW_SOURCE_DB_REC_SCALINGSTATVALUESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ScalingStatValuesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_charlevel;
    /* 0008 */ int32_t m_shoulderBudget;
    /* 000C */ int32_t m_trinketBudget;
    /* 0010 */ int32_t m_weaponBudget1h;
    /* 0014 */ int32_t m_rangedBudget;
    /* 0018 */ int32_t m_clothShoulderArmor;
    /* 001C */ int32_t m_leatherShoulderArmor;
    /* 0020 */ int32_t m_mailShoulderArmor;
    /* 0024 */ int32_t m_plateShoulderArmor;
    /* 0028 */ int32_t m_weaponDps1h;
    /* 002C */ int32_t m_weaponDps2h;
    /* 0030 */ int32_t m_spellcasterDps1h;
    /* 0034 */ int32_t m_spellcasterDps2h;
    /* 0038 */ int32_t m_rangedDps;
    /* 003C */ int32_t m_wandDps;
    /* 0040 */ int32_t m_spellPower;
    /* 0044 */ int32_t m_primaryBudget;
    /* 0048 */ int32_t m_tertiaryBudget;
    /* 004C */ int32_t m_clothCloakArmor;
    /* 0050 */ int32_t m_clothChestArmor;
    /* 0054 */ int32_t m_leatherChestArmor;
    /* 0058 */ int32_t m_mailChestArmor;
    /* 005C */ int32_t m_plateChestArmor;

// VERIFY: 38325a7d auto
    // OFFSET: 0x008B5720
    static const char* GetFilename() {
        return "DBFilesClient\\ScalingStatValues.dbc";
    }

// VERIFY: a3e9b803 auto
    // OFFSET: 0x008B5730
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t charlevelRead = SFileReadFile(file, &this->m_charlevel, sizeof(this->m_charlevel), nullptr, nullptr, 0);
        int32_t shoulderBudgetRead = SFileReadFile(file, &this->m_shoulderBudget, sizeof(this->m_shoulderBudget), nullptr, nullptr, 0);
        int32_t trinketBudgetRead = SFileReadFile(file, &this->m_trinketBudget, sizeof(this->m_trinketBudget), nullptr, nullptr, 0);
        int32_t weaponBudget1hRead = SFileReadFile(file, &this->m_weaponBudget1h, sizeof(this->m_weaponBudget1h), nullptr, nullptr, 0);
        int32_t rangedBudgetRead = SFileReadFile(file, &this->m_rangedBudget, sizeof(this->m_rangedBudget), nullptr, nullptr, 0);
        int32_t clothShoulderArmorRead = SFileReadFile(file, &this->m_clothShoulderArmor, sizeof(this->m_clothShoulderArmor), nullptr, nullptr, 0);
        int32_t leatherShoulderArmorRead = SFileReadFile(file, &this->m_leatherShoulderArmor, sizeof(this->m_leatherShoulderArmor), nullptr, nullptr, 0);
        int32_t mailShoulderArmorRead = SFileReadFile(file, &this->m_mailShoulderArmor, sizeof(this->m_mailShoulderArmor), nullptr, nullptr, 0);
        int32_t plateShoulderArmorRead = SFileReadFile(file, &this->m_plateShoulderArmor, sizeof(this->m_plateShoulderArmor), nullptr, nullptr, 0);
        int32_t weaponDps1hRead = SFileReadFile(file, &this->m_weaponDps1h, sizeof(this->m_weaponDps1h), nullptr, nullptr, 0);
        int32_t weaponDps2hRead = SFileReadFile(file, &this->m_weaponDps2h, sizeof(this->m_weaponDps2h), nullptr, nullptr, 0);
        int32_t spellcasterDps1hRead = SFileReadFile(file, &this->m_spellcasterDps1h, sizeof(this->m_spellcasterDps1h), nullptr, nullptr, 0);
        int32_t spellcasterDps2hRead = SFileReadFile(file, &this->m_spellcasterDps2h, sizeof(this->m_spellcasterDps2h), nullptr, nullptr, 0);
        int32_t rangedDpsRead = SFileReadFile(file, &this->m_rangedDps, sizeof(this->m_rangedDps), nullptr, nullptr, 0);
        int32_t wandDpsRead = SFileReadFile(file, &this->m_wandDps, sizeof(this->m_wandDps), nullptr, nullptr, 0);
        int32_t spellPowerRead = SFileReadFile(file, &this->m_spellPower, sizeof(this->m_spellPower), nullptr, nullptr, 0);
        int32_t primaryBudgetRead = SFileReadFile(file, &this->m_primaryBudget, sizeof(this->m_primaryBudget), nullptr, nullptr, 0);
        int32_t tertiaryBudgetRead = SFileReadFile(file, &this->m_tertiaryBudget, sizeof(this->m_tertiaryBudget), nullptr, nullptr, 0);
        int32_t clothCloakArmorRead = SFileReadFile(file, &this->m_clothCloakArmor, sizeof(this->m_clothCloakArmor), nullptr, nullptr, 0);
        int32_t clothChestArmorRead = SFileReadFile(file, &this->m_clothChestArmor, sizeof(this->m_clothChestArmor), nullptr, nullptr, 0);
        int32_t leatherChestArmorRead = SFileReadFile(file, &this->m_leatherChestArmor, sizeof(this->m_leatherChestArmor), nullptr, nullptr, 0);
        int32_t mailChestArmorRead = SFileReadFile(file, &this->m_mailChestArmor, sizeof(this->m_mailChestArmor), nullptr, nullptr, 0);
        int32_t plateChestArmorRead = SFileReadFile(file, &this->m_plateChestArmor, sizeof(this->m_plateChestArmor), nullptr, nullptr, 0);

        if (!idRead
            | !charlevelRead
            | !shoulderBudgetRead
            | !trinketBudgetRead
            | !weaponBudget1hRead
            | !rangedBudgetRead
            | !clothShoulderArmorRead
            | !leatherShoulderArmorRead
            | !mailShoulderArmorRead
            | !plateShoulderArmorRead
            | !weaponDps1hRead
            | !weaponDps2hRead
            | !spellcasterDps1hRead
            | !spellcasterDps2hRead
            | !rangedDpsRead
            | !wandDpsRead
            | !spellPowerRead
            | !primaryBudgetRead
            | !tertiaryBudgetRead
            | !clothCloakArmorRead
            | !clothChestArmorRead
            | !leatherChestArmorRead
            | !mailChestArmorRead
            | !plateChestArmorRead) {
            return false;
        }

        return true;
    }
};

#endif
