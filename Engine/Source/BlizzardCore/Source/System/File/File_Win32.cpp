#include <cstdint>
#include <cstring>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/TempBuffer.h"
#include "Engine/Source/BlizzardCore/Source/System/Unicode/Unicode.h"

// VERIFY: 7c249f3a manual
// OFFSET: 0x00433590
void System_File::FromNativeName(char* buffer, uint32_t buffersize, const char* str) {
    uint32_t length = str ? static_cast<uint32_t>(strlen(str)) : 0;
    Blizzard::Memory::TempBuffer<uint16_t, 300> wide(length + 1);
    wide.m_data[MultiByteToWideChar(CP_ACP, 0, str, length, reinterpret_cast<wchar_t*>(wide.m_data), wide.m_count)] = 0;
    Blizzard::Unicode::ConvertUTF16to8(buffer, buffersize, wide.m_data, -1, nullptr, nullptr);
}
