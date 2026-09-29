#include "CommentatorFrame.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/WorldFrame.h"

// VERIFY: 892bcb14 manual
// OFFSET: 0x0056B5C0
void CGCommentator::UpdateCameraVelocity(uint32_t controlFlags, int32_t canControl) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__) || !CGWorldFrame::GetActiveCamera()) {
        return;
    }

    if (!canControl) {
        this->m_forwardVelocity = 0.0f;
        this->m_strafeVelocity = 0.0f;
        this->m_verticalVelocity = 0.0f;
        this->m_yawVelocity = 0.0f;
        return;
    }

    int32_t forward = (controlFlags & 0x1000) != 0;

    if (controlFlags & 0x10) {
        forward++;
    }

    if ((controlFlags & 0x1) && (controlFlags & 0x2)) {
        forward++;
    }

    if (controlFlags & 0x20) {
        forward--;
    }

    if (forward) {
        CGPlayer_C::ClearAFK(0);
        this->m_forwardVelocity = forward > 0 ? 1.0f : -1.0f;
    } else {
        this->m_forwardVelocity = 0.0f;
    }

    int32_t strafe = (controlFlags & 0x40) != 0;

    if ((controlFlags & 0x2000001) && (controlFlags & 0x100)) {
        strafe++;
    }

    if (controlFlags & 0x80) {
        strafe--;
    }

    if ((controlFlags & 0x2000001) && (controlFlags & 0x200)) {
        strafe--;
    }

    if (strafe) {
        CGPlayer_C::ClearAFK(0);
        this->m_strafeVelocity = strafe > 0 ? 1.0f : -1.0f;
    } else {
        this->m_strafeVelocity = 0.0f;
    }

    int32_t vertical = (controlFlags & 0x2000) != 0;

    if (controlFlags & 0x4000) {
        vertical--;
    }

    this->m_verticalVelocity = static_cast<float>(vertical);

    int32_t turn = 0;

    if (controlFlags & 0x100) {
        turn = 1;
    }

    if (controlFlags & 0x200) {
        turn--;
    }

    if (controlFlags & 0x2000001) {
        turn = 0;
    }

    this->m_yawVelocity = turn * 0.050000001f;
}
