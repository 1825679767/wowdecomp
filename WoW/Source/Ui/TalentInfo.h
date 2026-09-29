#ifndef WOW_SOURCE_UI_TALENTINFO_H
#define WOW_SOURCE_UI_TALENTINFO_H

#include <cstddef>
#include <cstdint>

class CDataStore;

struct lua_State;

class CGTalentInfo {
    public:
    static void EnterWorld();
    static void RecalculatePreviewPoints(int32_t pet);
    static int32_t SumPreviewPointsSpent(int32_t pet);
    static void UpdateTalentData(CDataStore* msg);
    static void UpdatePetTalentData(CDataStore* msg);
};

class CGTalentGroup_C {
    public:
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint8_t unk_000C[4];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[24];
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
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
int32_t Script_GetActiveTalentGroup(lua_State* L);
int32_t Script_SetActiveTalentGroup(lua_State* L);
int32_t Script_GetPreviewTalentPointsSpent(lua_State* L);
int32_t Script_GetGroupPreviewTalentPointsSpent(lua_State* L);
int32_t Script_AddPreviewTalentPoints(lua_State* L);
int32_t Script_ResetPreviewTalentPoints(lua_State* L);
int32_t Script_ResetGroupPreviewTalentPoints(lua_State* L);
int32_t Script_LearnPreviewTalents(lua_State* L);
void TalentInfoRegisterScriptFunctions();

class CGTalentInfo_Inspect {
    public:
    static void EnterWorld();
};

#endif
