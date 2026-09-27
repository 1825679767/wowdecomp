#include "FFXEffects.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FFX/PassBox4.h"
#include "Engine/Source/FFX/PassGauss4.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Game/GameClient/PassDeath.h"

// VERIFY: 874a5f41 manual
// OFFSET: 0x007E86A0
PassDeath::PassDeath(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    this->m_useRect = 0;
    this->m_shaders[0] = nullptr;
    this->m_shaders[1] = nullptr;

    if (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] == 8 || g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] == 9 || g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] == 10) {
        g_theGxDevicePtr->ShaderCreate(this->m_shaders, GxSh_Pixel, "Shaders\\Pixel", FFX::s_useRECT ? "FFXDeath_nvts_rect" : "FFXDeath_nvts_2d", 1);
        return;
    }

    if (FFX::s_useRECT) {
        this->m_useRect = 1;
    }

    g_theGxDevicePtr->ShaderCreate(this->m_shaders, GxSh_Pixel, "Shaders\\Pixel", "FFXDeath", 2);
}

// VERIFY: e2aee2fe auto
// OFFSET: 0x007E8920
PassDeathNVRC::PassDeathNVRC(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    g_theGxDevicePtr->ShaderCreate(&this->m_shader, GxSh_Pixel, "Shaders\\Pixel", "FFXDeath_nvrc", 1);
}

// VERIFY: 22ef7cce manual
// OFFSET: 0x007EA260
EffectDeath::EffectDeath() {
    this->m_cvar = CVar::Register("ffxDeath", "full screen death effect", 1, this->m_defaultEnabled ? "1" : "0", FFX::CVarCallback, 1, false, nullptr, false);

    if (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] == 7) {
        FFX::Tex* inputs[] = { &FFX::s_full[0] };
        FFX::Pass* pass = new (__FILE__, __LINE__) PassDeathNVRC(inputs, 1, &FFX::s_screen, GxBlend_Opaque, true);
        this->m_passes.New(pass);
        return;
    }

    if (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] <= 0) {
        return;
    }

    FFX::Tex* deathInputs[] = { &FFX::s_full[0], &FFX::s_quart[0] };
    FFX::Tex* quartInputs[] = { &FFX::s_quart[0] };

    FFX::Pass* pass = new (__FILE__, __LINE__) PassBox4(deathInputs, 1, &FFX::s_quart[0], GxBlend_Opaque, false);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassGauss4(quartInputs, 1, &FFX::s_quart[0], GxBlend_Opaque, false);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassDeath(deathInputs, 2, &FFX::s_screen, GxBlend_Opaque, true);
    this->m_passes.New(pass);
}
