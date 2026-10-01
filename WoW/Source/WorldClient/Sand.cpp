#include "Sand.h"
#include "WoW/Source/WorldClient/CPortalView.h"
#include "WoW/Source/WorldClient/World.h"

// VERIFY: 185d2837 auto
// OFFSET: 0x00783FE0
CAaBox Sand::GetBoundingBox() const {
    return this->m_bbox;
}

// OFFSET: 0x0078C3E0
void Sand::Render() {
    this->UpdatePackets();

    if (CWorldScene::s_unk_ADF570.m_maxViewDepth < 0.0f) {
        return;
    }

    if (this->m_useWeatherShaders) {
        this->RenderPacketsShader();
    } else {
        this->RenderPackets();
    }

    this->m_mists.Render();
}
