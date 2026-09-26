#ifndef WOW_SOURCE_DB_REC_SPELLITEMENCHANTMENTCONDITIONREC_H
#define WOW_SOURCE_DB_REC_SPELLITEMENCHANTMENTCONDITIONREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SpellItemEnchantmentConditionRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int8_t m_ltOperandType [5];
    /* 000C */ int32_t m_ltOperand [5];
    /* 0020 */ int8_t m_operator [5];
    /* 0025 */ int8_t m_rtOperandType [5];
    /* 002C */ int32_t m_rtOperand [5];
    /* 0040 */ int8_t m_logic [5];

// VERIFY: f2dca7d7 auto
    // OFFSET: 0x008B81F0
    static const char* GetFilename() {
        return "DBFilesClient\\SpellItemEnchantmentCondition.dbc";
    }

// VERIFY: 9c616829 manual
    // OFFSET: 0x008B8200
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t ltOperandTypeRead = SFileReadFile(file, this->m_ltOperandType, sizeof(this->m_ltOperandType), nullptr, nullptr, 0);
        int32_t ltOperandRead = SFileReadFile(file, this->m_ltOperand, sizeof(this->m_ltOperand), nullptr, nullptr, 0);
        int32_t operatorRead = SFileReadFile(file, this->m_operator, sizeof(this->m_operator), nullptr, nullptr, 0);
        int32_t rtOperandTypeRead = SFileReadFile(file, this->m_rtOperandType, sizeof(this->m_rtOperandType), nullptr, nullptr, 0);
        int32_t rtOperandRead = SFileReadFile(file, this->m_rtOperand, sizeof(this->m_rtOperand), nullptr, nullptr, 0);
        int32_t logicRead = SFileReadFile(file, this->m_logic, sizeof(this->m_logic), nullptr, nullptr, 0);

        if (!idRead
            | !ltOperandTypeRead
            | !ltOperandRead
            | !operatorRead
            | !rtOperandTypeRead
            | !rtOperandRead
            | !logicRead) {
            return false;
        }

        return true;
    }
};

#endif
