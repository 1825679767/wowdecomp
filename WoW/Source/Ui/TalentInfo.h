#ifndef WOW_SOURCE_UI_TALENTINFO_H
#define WOW_SOURCE_UI_TALENTINFO_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"

class CDataStore;

struct lua_State;

struct TalentTabInfo;

struct TalentGroupTab {
    /* 0000 */ int32_t m_tabId;
    /* 0004 */ uint32_t m_pointsSpent;
    /* 0008 */ int32_t m_previewPointsSpent;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
};

struct CGTalentItem {
    /* 0000 */ uint8_t unk_0000[24];
    /* 0018 */ int32_t m_talentId;
    /* 001C */ int32_t m_rank;
    /* 0020 */ int32_t m_previewRank;
};

class TalentRec;

class CGTalentGroup_C {
    public:
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint8_t unk_000C[4];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[24];
    /* 002C */ TSFixedArray<TalentGroupTab> m_tabs;
    /* 0038 */ uint32_t m_unspentPoints;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;

    void ResetAllPreviewTalentPoints();
    TalentGroupTab* GetTabFromID(int32_t tabId);
    CGTalentItem* GetTalentItem(int32_t talentId);
    int32_t CanAddPreviewTalentPoints(const TalentRec* talent, int32_t delta, int32_t* count, int32_t pet);
};

class CGTalentInfo {
    public:
    static uint32_t m_activeTalentGroup;
    static uint32_t m_numTalentGroups;
    static int32_t m_petTalentTab;
    static TSFixedArray<TalentTabInfo*> m_tabs;
    static TSFixedArray<TalentTabInfo*> m_petTabs;
    static CGTalentGroup_C m_talentGroups[2];
    static CGTalentGroup_C m_petTalentGroup;
    static void EnterWorld();
    static uint32_t GetActiveTalentGroup(int32_t pet);
    static uint32_t GetUnspentTalentPoints(uint32_t index, int32_t pet);
    static CGTalentGroup_C* GetTalentGroup(uint32_t index, int32_t pet);
    static void RecalculatePreviewPoints(int32_t pet);
    static int32_t SumPreviewPointsSpent(int32_t pet);
    static void UpdateTalentData(CDataStore* msg);
    static void UpdatePetTalentData(CDataStore* msg);
};


int32_t Script_GetNumTalentTabs(lua_State* L);
int32_t Script_GetTalentTabInfo(lua_State* L);
int32_t Script_GetNumTalents(lua_State* L);
int32_t Script_GetTalentInfo(lua_State* L);
int32_t Script_GetTalentLink(lua_State* L);
int32_t Script_GetTalentPrereqs(lua_State* L);
int32_t Script_LearnTalent(lua_State* L);
int32_t Script_GetUnspentTalentPoints(lua_State* L);
int32_t Script_GetNumTalentGroups(lua_State* L);
int32_t Script_SetActiveTalentGroup(lua_State* L);
int32_t Script_GetGroupPreviewTalentPointsSpent(lua_State* L);
int32_t Script_AddPreviewTalentPoints(lua_State* L);
int32_t Script_ResetPreviewTalentPoints(lua_State* L);
int32_t Script_ResetGroupPreviewTalentPoints(lua_State* L);
int32_t Script_LearnPreviewTalents(lua_State* L);
void TalentInfoRegisterScriptFunctions();
int32_t TalentInfoIsSpellKnown(int32_t spellId, int32_t isInspect, int32_t isPet);

class CGTalentInfo_Inspect {
    public:
    static uint32_t m_activeTalentGroup;
    static void EnterWorld();
    static WOWGUID m_inspectTarget;
};

#endif
