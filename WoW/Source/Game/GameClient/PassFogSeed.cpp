#include "PassFogSeed.h"

// STATIC: 0x00AF4938
static uint32_t s_deviceRestored;

// VERIFY: beff8e6d auto
// OFFSET: 0x007E7FE0
void __fastcall PassFogSeed::DeviceRestoredCallback() {
    s_deviceRestored = 1;
}
