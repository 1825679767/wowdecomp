#include "NamePlateFrame.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"

// VERIFY: eb556f09 auto
// OFFSET: 0x00715C30
CGNamePlateFrame* CGNamePlateFrame::GetNamePlateFocus() {
    return CGUnit_C::s_namePlateFocus;
}

// VERIFY: 7dfd3d5d auto
// OFFSET: 0x0098E580
void CGNamePlateFrame::UpdateNameDisplay(const CGUnit_C* unit) {
    this->m_unitNameText->SetText(const_cast<CGUnit_C*>(unit)->GetUnitName(nullptr, 1), 1);
}

// VERIFY: ca086f2a manual
// OFFSET: 0x0098E5B0
void CGNamePlateFrame::TriggerDamageFeedback() {
    CImVector color;
    color.b = 0;
    color.g = 0;
    color.r = 0xFF;
    color.a = 0xFF;
    this->unk_02F0 = 5000;
    this->m_unitNameText->SetVertexColor(color);
}
