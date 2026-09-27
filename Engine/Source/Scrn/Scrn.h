#ifndef ENGINE_SOURCE_SCRN_SCRN_H
#define ENGINE_SOURCE_SCRN_SCRN_H

#include <cstdint>
#include "Engine/Source/Scrn/Types.h"
#include "Engine/Source/Storm/H/Types.h"

void ScrnLayerCreate(const RECTF* rect, float zorder, uint32_t flags, void* param, void (*paintFunc)(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec), HLAYER* layer);

#endif
