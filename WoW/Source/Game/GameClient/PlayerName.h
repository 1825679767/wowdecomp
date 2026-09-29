#ifndef WOW_SOURCE_GAME_GAMECLIENT_PLAYERNAME_H
#define WOW_SOURCE_GAME_GAMECLIENT_PLAYERNAME_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "WoW/Source/Object/WGUID.h"

struct CVar;
class CGObject_C;
class CM2Lighting;
class CM2Model;
class CImVector;

class PLAYERNAMEDESC {
    public:
    /* 0000 */ uint32_t m_link;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t m_string;
    /* 000C */ uint32_t m_color;
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_flags;
    /* 001C */ uint32_t m_renderFrame;
    /* 0020 */ uint32_t m_worldText [4];
    /* 0030 */ float m_zOffset;
    /* 0034 */ uint32_t unk_0034;

    static int32_t ShouldRender(CGObject_C* object, uint32_t raidTarget);
    static void RenderIfVisible(CM2Model* model, const CM2Lighting* lighting, void* arg);
};

PLAYERNAMEDESC* PlayerNameCreate(const WOWGUID& guid);
void PlayerNameDelete(PLAYERNAMEDESC* desc);
void PlayerNameCreateText(PLAYERNAMEDESC* desc, uint32_t type, const char* text, const CImVector* color, const CImVector* shadowColor);

void PlayerNameTriggerColorUpdate(PLAYERNAMEDESC* desc);
void PlayerNameTriggerNameRegenerate(PLAYERNAMEDESC* desc);
void PlayerNameUpdateDrawCallback(PLAYERNAMEDESC* desc);
bool PlayerNameShowing(PLAYERNAMEDESC* desc);
float PlayerNameComputeScale(CGObject_C* object);

void PlayerNameInitialize();
void PlayerNameShutdown();
void PlayerNameUpdateEarly();
void PlayerNameUpdateLate();
void PlayerNameUpdateWorldText();

bool ObjectNameShowCallback(CVar* var, const char* oldValue, const char* value, void* arg);
void PlayerNameRegisterCVars();

#endif
