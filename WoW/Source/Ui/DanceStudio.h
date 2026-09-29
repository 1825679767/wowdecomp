#ifndef WOW_SOURCE_UI_DANCESTUDIO_H
#define WOW_SOURCE_UI_DANCESTUDIO_H

#include <cstddef>
#include <cstdint>

struct lua_State;

class CGDanceStudio {
    public:
    static void InitializeGame();
    static void RegisterScriptFunctions();
};

int32_t Script_PlayDance(lua_State* L);

#endif
