#ifndef WOW_SOURCE_DB_REC_SKILLLINEABILITYREC_H
#define WOW_SOURCE_DB_REC_SKILLLINEABILITYREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct SkillLineAbilityRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_skillLine;
    /* 0008 */ int32_t m_spell;
    /* 000C */ int32_t m_raceMask;
    /* 0010 */ int32_t m_classMask;
    /* 0014 */ int32_t m_excludeRace;
    /* 0018 */ int32_t m_excludeClass;
    /* 001C */ int32_t m_minSkillLineRank;
    /* 0020 */ int32_t m_supercededBySpell;
    /* 0024 */ int32_t m_acquireMethod;
    /* 0028 */ int32_t m_trivialSkillLineRankHigh;
    /* 002C */ int32_t m_trivialSkillLineRankLow;
    /* 0030 */ int32_t m_characterPoints [2];

// VERIFY: 390c2d46 auto
    // OFFSET: 0x008B5CF0
    static const char* GetFilename() {
        return "DBFilesClient\\SkillLineAbility.dbc";
    }

// VERIFY: a51dad42 auto
    // OFFSET: 0x008B5D00
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t skillLineRead = SFileReadFile(file, &this->m_skillLine, sizeof(this->m_skillLine), nullptr, nullptr, 0);
        int32_t spellRead = SFileReadFile(file, &this->m_spell, sizeof(this->m_spell), nullptr, nullptr, 0);
        int32_t raceMaskRead = SFileReadFile(file, &this->m_raceMask, sizeof(this->m_raceMask), nullptr, nullptr, 0);
        int32_t classMaskRead = SFileReadFile(file, &this->m_classMask, sizeof(this->m_classMask), nullptr, nullptr, 0);
        int32_t excludeRaceRead = SFileReadFile(file, &this->m_excludeRace, sizeof(this->m_excludeRace), nullptr, nullptr, 0);
        int32_t excludeClassRead = SFileReadFile(file, &this->m_excludeClass, sizeof(this->m_excludeClass), nullptr, nullptr, 0);
        int32_t minSkillLineRankRead = SFileReadFile(file, &this->m_minSkillLineRank, sizeof(this->m_minSkillLineRank), nullptr, nullptr, 0);
        int32_t supercededBySpellRead = SFileReadFile(file, &this->m_supercededBySpell, sizeof(this->m_supercededBySpell), nullptr, nullptr, 0);
        int32_t acquireMethodRead = SFileReadFile(file, &this->m_acquireMethod, sizeof(this->m_acquireMethod), nullptr, nullptr, 0);
        int32_t trivialSkillLineRankHighRead = SFileReadFile(file, &this->m_trivialSkillLineRankHigh, sizeof(this->m_trivialSkillLineRankHigh), nullptr, nullptr, 0);
        int32_t trivialSkillLineRankLowRead = SFileReadFile(file, &this->m_trivialSkillLineRankLow, sizeof(this->m_trivialSkillLineRankLow), nullptr, nullptr, 0);
        int32_t characterPointsRead = SFileReadFile(file, this->m_characterPoints, sizeof(this->m_characterPoints), nullptr, nullptr, 0);

        if (!idRead
            | !skillLineRead
            | !spellRead
            | !raceMaskRead
            | !classMaskRead
            | !excludeRaceRead
            | !excludeClassRead
            | !minSkillLineRankRead
            | !supercededBySpellRead
            | !acquireMethodRead
            | !trivialSkillLineRankHighRead
            | !trivialSkillLineRankLowRead
            | !characterPointsRead) {
            return false;
        }

        return true;
    }
};

#endif
