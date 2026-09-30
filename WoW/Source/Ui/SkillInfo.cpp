#include "SkillInfo.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00C22AE8
uint32_t CGSkillInfo::m_numLines;

// STATIC: 0x00C22AEC
uint32_t CGSkillInfo::m_numTypes;

// STATIC: 0x00C22AF0
uint32_t CGSkillInfo::m_numSkillLines;

// STATIC: 0x00C22AF4
int32_t CGSkillInfo::m_selectedSkill;

// STATIC: 0x00C22AF8
int32_t CGSkillInfo::unk_C22AF8;

// STATIC: 0x00C22B00
int32_t CGSkillInfo::m_skillUpPoints;

// STATIC: 0x00C22B04
TSFixedArray<SkillLineInfo*> CGSkillInfo::m_lines;

// STATIC: 0x00C22B10
TSFixedArray<SkillTypeInfo*> CGSkillInfo::m_types;

// VERIFY: 8c16c042 manual
// OFFSET: 0x005CE880
void CGSkillInfo::InitializeGame() {
    uint32_t numTypes = g_skillLineCategoryDB.m_numRecords;
    uint32_t numLines = numTypes + 128;

    CGSkillInfo::m_lines.SetCount(numLines);

    for (uint32_t i = 0; i < numLines; i++) {
        CGSkillInfo::m_lines.m_data[i] = static_cast<SkillLineInfo*>(SMemAlloc(sizeof(SkillLineInfo), __FILE__, __LINE__, 0));
        CGSkillInfo::m_lines.m_data[i]->m_skillLineID = 0;
        CGSkillInfo::m_lines.m_data[i]->m_categoryID = 0;
        CGSkillInfo::m_lines.m_data[i]->m_expanded = 1;
    }

    CGSkillInfo::m_types.SetCount(numTypes);

    for (uint32_t i = 0; i < numTypes; i++) {
        CGSkillInfo::m_types.m_data[i] = static_cast<SkillTypeInfo*>(SMemAlloc(sizeof(SkillTypeInfo), __FILE__, __LINE__, 0));
        CGSkillInfo::m_types.m_data[i]->m_categoryID = 0;
        CGSkillInfo::m_types.m_data[i]->unk_0004 = 0;
    }

    CGSkillInfo::m_numLines = 0;
    CGSkillInfo::m_numTypes = 0;
    CGSkillInfo::m_numSkillLines = 0;
    CGSkillInfo::m_selectedSkill = 0;
    CGSkillInfo::m_skillUpPoints = 0;
    CGSkillInfo::unk_C22AF8 = -1;
}

// VERIFY: c9c769d1 manual
// OFFSET: 0x005CD360
int32_t CGSkillInfo::GetSelectedIndex() {
    if (!CGSkillInfo::m_selectedSkill || !CGSkillInfo::m_numSkillLines) {
        return -1;
    }

    for (uint32_t i = 0; i < CGSkillInfo::m_numSkillLines; i++) {
        if (CGSkillInfo::m_lines.m_data[i]->m_skillLineID == CGSkillInfo::m_selectedSkill) {
            return i;
        }
    }

    return -1;
}
