#include "CommentatorFrame.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00ACE4A8
CGCommentator CGCommentator::s_Commentator;

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

// VERIFY: 3035fcb9 manual
// OFFSET: 0x0056BC80
void CGCommentator::Init() {
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->unk_0028.m_guid = 0;
    this->m_forwardVelocity = 0.0f;
    this->unk_0030.m_guid = 0;
    this->m_strafeVelocity = 0.0f;
    this->m_verticalVelocity = 0.0f;
    this->m_facing = 0.0f;
    this->m_cameraCollision = 1;
    this->m_yawVelocity = 0.0f;
    this->unk_0060 = 0;
    this->m_pitch = 0.0f;
    this->m_distance = 12.0f;
    this->m_fov = 1.5707964f;
    this->m_targetFov = 1.5707964f;
    this->unk_0044 = 10.0f;
    this->m_targetHeightOffset = 0.0f;
    this->unk_0068 = 13;
    this->unk_006C[0] = 0;
    this->unk_0064 = 0;
    this->unk_006C[1] = 0;
    this->unk_006C[2] = 0;
    this->m_battlemaster.m_guid = 0;
    this->m_maps.Clear();
    this->m_skirmishQueue.Clear();
    this->m_skirmishMode = 1;
}

// VERIFY: 59295854 auto
// OFFSET: 0x0056C150
void CGCommentator::InitializeGame() {
    CGCommentator::s_Commentator.Init();
    ClientServices::SetMessageHandler(SMSG_COMMENTATOR_PLAYER_INFO, CGCommentator::OnReceivePlayerInfo, nullptr);
    ClientServices::SetMessageHandler(SMSG_COMMENTATOR_MAP_INFO, CGCommentator::OnReceiveMapInfo, nullptr);
    ClientServices::SetMessageHandler(SMSG_COMMENTATOR_STATE_CHANGED, CGCommentator::OnStateChanged, nullptr);
    ClientServices::SetMessageHandler(SMSG_COMMENTATOR_SKIRMISH_QUEUE_RESULT1, CGCommentator::OnSkirmishPlayersInQueue, nullptr);
    ClientServices::SetMessageHandler(SMSG_COMMENTATOR_SKIRMISH_QUEUE_RESULT2, CGCommentator::OnSkirmishPlayersModeState, nullptr);
}

// VERIFY: 4126a5f5 manual
// OFFSET: 0x0056B510
void CGCommentator::SnapCameraToPlayer() {
    if (!CGWorldFrame::s_currentWorldFrame) {
        return;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!camera) {
        return;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    this->m_pitch = player->GetPitch();
    this->m_facing = player->GetFacing();
    this->m_position = player->GetPosition();
    this->m_position.z += 1.5f;
    camera->CommentatorSetViewAndPosition(this->m_position, this->m_facing, this->m_pitch);
}

// VERIFY: a88d79fe auto
// OFFSET: 0x0056B830
void CGCommentator::PostInit(CGPlayer_C* player) {
    if (player && player->IsCommentatorUberOrInArena()) {
        World::ObjectSetShadow(player->m_worldObject, 2, 2);

        if (CGWorldFrame::s_currentWorldFrame) {
            CGWorldFrame::GetActiveCamera()->SetView(6, 2, 0);
            CGCommentator::s_Commentator.SnapCameraToPlayer();
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_COMMENTATOR_ENTER_WORLD, nullptr);
}

// VERIFY: acad482e auto
// OFFSET: 0x00568340
void CGCommentator::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_COMMENTATOR_PLAYER_INFO);
    ClientServices::ClearMessageHandler(SMSG_COMMENTATOR_MAP_INFO);
    ClientServices::ClearMessageHandler(SMSG_COMMENTATOR_STATE_CHANGED);
    ClientServices::ClearMessageHandler(SMSG_COMMENTATOR_SKIRMISH_QUEUE_RESULT1);
    ClientServices::ClearMessageHandler(SMSG_COMMENTATOR_SKIRMISH_QUEUE_RESULT2);
}
