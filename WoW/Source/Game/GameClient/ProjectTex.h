#ifndef WOW_SOURCE_GAME_GAMECLIENT_PROJECTTEX_H
#define WOW_SOURCE_GAME_GAMECLIENT_PROJECTTEX_H

#include <cstddef>
#include <cstdint>

class C44Matrix;
class CAaBox;

void ProjectTex2dMakeMatrices(C44Matrix& texMatrix, C44Matrix& fadeMatrix, const CAaBox& box, const C44Matrix* rotation, float fade, int32_t local);

#endif
