#include "WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"

// STATIC: 0x00B7436C
CGWorldFrame* CGWorldFrame::s_currentWorldFrame;

// VERIFY: d8525c88 auto
// OFFSET: 0x004F5960
CGCamera* CGWorldFrame::GetActiveCamera() {
    if (CGWorldFrame::s_currentWorldFrame) {
        return CGWorldFrame::s_currentWorldFrame->m_camera;
    }

    return nullptr;
}

// VERIFY: 0b3308f3 manual
// OFFSET: 0x004F6650
void CGWorldFrame::GetCameraPosition(C3Vector* position) {
    *position = CGWorldFrame::s_currentWorldFrame->m_camera->m_position;
}
