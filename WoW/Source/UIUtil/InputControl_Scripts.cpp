#include "InputControl.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/UIBindings.h"
#include "WoW/Source/Ui/WorldFrame.h"

// VERIFY: 46af036c manual
// OFFSET: 0x005FBF80
static int32_t Script_JumpOrAscendStart(lua_State* L) {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x2000, 1, CSimpleTop::s_eventTime, 0);

    if (mover->m_movement->m_flags & 0x2200000) {
        return 0;
    }

    CheckToCancelCurrentChannelSpell();

    if (mover->m_unit->m_unitFieldHealth <= 0 || mover->IsOnSpline() || mover->IsAlteredFormTransitionPreventingMovement() || !mover->CanMove() || mover->AnimSuppressesMovement()) {
        return 0;
    }

    uint32_t time = CSimpleTop::s_eventTime;

    if (mover->GetClientStandState()) {
        mover->ChangeStandState(0);
    }

    if (ClntObjMgrGetActivePlayerObj()) {
        CGPlayer_C::ClearAFK(0);
    }

    if (mover->GetCanFly()) {
        mover->OnFlightLocal(time, 1);
    } else {
        mover->TryJumpOrAscend(time);
    }

    return 0;
}

// VERIFY: 1ad525fe manual
// OFFSET: 0x005FC0A0
static int32_t Script_AscendStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x2000, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: d72c7867 manual
// OFFSET: 0x005FC140
static int32_t Script_DescendStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x4000, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 8076179f manual
// OFFSET: 0x005FAAE0
static int32_t Script_ToggleRun(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    uint32_t time = CSimpleTop::s_eventTime;

    if (mover->m_unit->m_unitFieldHealth > 0 && !mover->m_movement->IsOnSpline() && !mover->IsAlteredFormTransitionPreventingMovement() && mover->CanMove()) {
        mover->ToggleRunModeLocal(time);
    }

    return 0;
}

// VERIFY: d7467083 manual
// OFFSET: 0x005FC190
static int32_t Script_ToggleAutoRun(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;
    int32_t enable = !(input->m_flags & 0x1000);

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x1000, enable, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 8852ef68 manual
// OFFSET: 0x005FC200
static int32_t Script_MoveForwardStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x10, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 662e594c manual
// OFFSET: 0x005FC250
static int32_t Script_MoveForwardStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x10, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: f8b5bc47 manual
// OFFSET: 0x005FC290
static int32_t Script_MoveBackwardStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x20, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 3c08cf9a manual
// OFFSET: 0x005FC2E0
static int32_t Script_MoveBackwardStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x20, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 4e295838 manual
// OFFSET: 0x005FC320
static int32_t Script_TurnLeftStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x100, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 9ce3a768 manual
// OFFSET: 0x005FC360
static int32_t Script_TurnLeftStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x100, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 7e560f07 manual
// OFFSET: 0x005FC3B0
static int32_t Script_TurnRightStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x200, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 519204d4 manual
// OFFSET: 0x005FC3F0
static int32_t Script_TurnRightStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x200, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 0be9d4b2 manual
// OFFSET: 0x005FC440
static int32_t Script_StrafeLeftStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x40, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 8954b43d manual
// OFFSET: 0x005FC490
static int32_t Script_StrafeLeftStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x40, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 7ddc8426 manual
// OFFSET: 0x005FC4D0
static int32_t Script_StrafeRightStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x80, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 080d2052 manual
// OFFSET: 0x005FC520
static int32_t Script_StrafeRightStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x80, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: bc849210 manual
// OFFSET: 0x005FC570
static int32_t Script_VehicleAimUpStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x400, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 994e08c2 manual
// OFFSET: 0x005FC5C0
static int32_t Script_VehicleAimDownStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x800, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 03a4e142 manual
// OFFSET: 0x005FC8E0
static int32_t Script_VehicleAimUpStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x400, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: ddbbfc22 manual
// OFFSET: 0x005FC920
static int32_t Script_VehicleAimDownStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x800, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// VERIFY: 5df2c0ab auto
// OFFSET: 0x005FC610
int32_t Script_TurnOrActionStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (CGGameUI::CanPerformAction(0)) {
        uint32_t time = CSimpleTop::s_eventTime;

        if (!(input->m_flags & 0x3)) {
            input->m_pendingDefaultAction = PENDING_ACTION_RIGHT;
            CGWorldFrame::s_currentWorldFrame->SetupDefaultAction();
        }

        if (input->SetControlBit(1, time)) {
            if ((input->m_flags & 0x1) && (input->m_flags & 0x2)) {
                CheckToCancelCurrentChannelSpell();
            }

            input->UpdatePlayer(time, 1);
        }
    }

    return 0;
}

// VERIFY: 1b123333 auto
// OFFSET: 0x005FC680
int32_t Script_TurnOrActionStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (CGGameUI::CanPerformAction(0)) {
        uint32_t time = CSimpleTop::s_eventTime;

        if (input->UnsetControlBit(1, time, 0)) {
            input->UpdatePlayer(time, 1);
        }
    }

    return 0;
}

// VERIFY: 4ed23fe7 auto
// OFFSET: 0x005FC6C0
int32_t Script_CameraOrSelectOrMoveStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (CGGameUI::CanPerformAction(0)) {
        uint32_t time = CSimpleTop::s_eventTime;

        if (!(input->m_flags & 0x3)) {
            input->m_pendingDefaultAction = PENDING_ACTION_LEFT;
            CGWorldFrame::s_currentWorldFrame->SetupDefaultAction();
        }

        if (input->SetControlBit(2, time)) {
            if ((input->m_flags & 0x1) && (input->m_flags & 0x2)) {
                CheckToCancelCurrentChannelSpell();
            }

            input->UpdatePlayer(time, 1);
        }
    }

    return 0;
}

// VERIFY: 1a649142 auto
// OFFSET: 0x005FC730
int32_t Script_CameraOrSelectOrMoveStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (CGGameUI::CanPerformAction(0)) {
        uint32_t time = CSimpleTop::s_eventTime;

        if (input->UnsetControlBit(2, time, StringToBOOL(L, 1, 0))) {
            input->UpdatePlayer(time, 1);
        }
    }

    return 0;
}

// VERIFY: 29eb6c98 auto
// OFFSET: 0x005FC890
int32_t Script_MouselookStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;
    uint32_t time = CSimpleTop::s_eventTime;
    input->m_pendingDefaultAction = PENDING_ACTION_NONE;

    if (input->UnsetControlBit(1, time, 0)) {
        input->UpdatePlayer(time, 1);
    }

    CGUIBindings::s_bindings->ClearOverrideBindings(input, false);
    return 0;
}

// VERIFY: 8eb6ff5b manual
// OFFSET: 0x005FCC10
int32_t Script_MouselookStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;
    uint32_t time = CSimpleTop::s_eventTime;
    input->m_pendingDefaultAction = PENDING_ACTION_NONE;

    if (input->SetControlBit(1, time)) {
        if ((input->m_flags & 0x1) && (input->m_flags & 0x2)) {
            CheckToCancelCurrentChannelSpell();
        }

        input->UpdatePlayer(time, 1);
    }

    input->SetMouselookOverrideBindings();
    return 0;
}

// STATIC: 0x00AD1938
static FrameScript_Method s_ScriptFunctions[] = {
    { "JumpOrAscendStart", &Script_JumpOrAscendStart },
    { "AscendStop", &Script_AscendStop },
    { "DescendStop", &Script_DescendStop },
    { "ToggleRun", &Script_ToggleRun },
    { "ToggleAutoRun", &Script_ToggleAutoRun },
    { "MoveForwardStart", &Script_MoveForwardStart },
    { "MoveForwardStop", &Script_MoveForwardStop },
    { "MoveBackwardStart", &Script_MoveBackwardStart },
    { "MoveBackwardStop", &Script_MoveBackwardStop },
    { "TurnLeftStart", &Script_TurnLeftStart },
    { "TurnLeftStop", &Script_TurnLeftStop },
    { "TurnRightStart", &Script_TurnRightStart },
    { "TurnRightStop", &Script_TurnRightStop },
    { "StrafeLeftStart", &Script_StrafeLeftStart },
    { "StrafeLeftStop", &Script_StrafeLeftStop },
    { "StrafeRightStart", &Script_StrafeRightStart },
    { "StrafeRightStop", &Script_StrafeRightStop },
    { "PitchUpStart", &Script_VehicleAimUpStart },
    { "PitchUpStop", &Script_VehicleAimUpStop },
    { "PitchDownStart", &Script_VehicleAimDownStart },
    { "PitchDownStop", &Script_VehicleAimDownStop },
    { "TurnOrActionStart", &Script_TurnOrActionStart },
    { "TurnOrActionStop", &Script_TurnOrActionStop },
    { "CameraOrSelectOrMoveStart", &Script_CameraOrSelectOrMoveStart },
    { "CameraOrSelectOrMoveStop", &Script_CameraOrSelectOrMoveStop },
    { "MoveAndSteerStart", &Script_MoveAndSteerStart },
    { "MoveAndSteerStop", &Script_MoveAndSteerStop },
    { "SetMouselookOverrideBinding", &Script_SetMouselookOverrideBinding },
    { "MouselookStart", &Script_MouselookStart },
    { "MouselookStop", &Script_MouselookStop },
    { "IsMouselooking", &Script_IsMouselooking },
    { "VehicleExit", &Script_VehicleExit },
    { "VehiclePrevSeat", &Script_VehiclePrevSeat },
    { "VehicleNextSeat", &Script_VehicleNextSeat },
    { "VehicleAimUpStart", &Script_VehicleAimUpStart },
    { "VehicleAimUpStop", &Script_VehicleAimUpStop },
    { "VehicleAimDownStart", &Script_VehicleAimDownStart },
    { "VehicleAimDownStop", &Script_VehicleAimDownStop },
    { "VehicleAimIncrement", &Script_VehicleAimIncrement },
    { "VehicleAimDecrement", &Script_VehicleAimDecrement },
    { "VehicleAimRequestAngle", &Script_VehicleAimRequestAngle },
    { "VehicleAimGetAngle", &Script_VehicleAimGetAngle },
    { "VehicleAimRequestNormAngle", &Script_VehicleAimRequestNormAngle },
    { "VehicleAimGetNormAngle", &Script_VehicleAimGetNormAngle },
    { "VehicleAimSetNormPower", &Script_VehicleAimSetNormPower },
    { "VehicleAimGetNormPower", &Script_VehicleAimGetNormPower },
    { "IsUsingVehicleControls", &Script_IsUsingVehicleControls },
    { "CanExitVehicle", &Script_CanExitVehicle },
    { "CanSwitchVehicleSeats", &Script_CanSwitchVehicleSeats },
    { "IsVehicleAimAngleAdjustable", &Script_IsVehicleAimAngleAdjustable },
    { "IsVehicleAimPowerAdjustable", &Script_IsVehicleAimPowerAdjustable },
    { "DetectWowMouse", &Script_DetectWowMouse },
};

// VERIFY: ee8cb67b auto
// OFFSET: 0x005F9580
void InputControlRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
