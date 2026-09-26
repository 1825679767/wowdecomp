#ifndef ENGINE_SOURCE_FRAMESCRIPT_FRAMESCRIPT_H
#define ENGINE_SOURCE_FRAMESCRIPT_FRAMESCRIPT_H

#include <cstddef>
#include <cstdint>

void FrameScript_FindTextSectionBounds();
const char* FrameScript_GetText(const char* token, int32_t count, int32_t gender);
void FrameScript_InvalidPtrCheck(const void* ptr);

#endif
