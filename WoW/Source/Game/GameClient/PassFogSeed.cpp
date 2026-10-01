#include "PassFogSeed.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"

// STATIC: 0x00AF4938
static uint32_t s_deviceRestored;

// VERIFY: beff8e6d auto
// OFFSET: 0x007E7FE0
void __fastcall PassFogSeed::DeviceRestoredCallback() {
    s_deviceRestored = 1;
}

// INLINED
PassFogSeed::PassFogSeed(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    this->unk_0030 = 0;
    this->unk_0034 = 0;
    s_deviceRestored = 1;
    g_theGxDevicePtr->AddDeviceRestoredCallback(PassFogSeed::DeviceRestoredCallback);
}
