// Donor wxl-radial-ping/RadialPing.cpp C_Ping compatibility bootstrap.
// Copyright (C) 2026 WarcraftXL. GPLv3 or later.
#pragma once
namespace wxl::native::radial_ping {
inline constexpr char Bootstrap[] = R"lua(
do
    wxlwow = wxlwow or {}
    wxlwow.radial_ping = wxlwow.radial_ping or {}
    local RP = wxlwow.radial_ping

    if _WXLWOW_RADIAL_PICK_CURSOR then
        RP.pick_cursor = _WXLWOW_RADIAL_PICK_CURSOR
        _WXLWOW_RADIAL_PICK_CURSOR = nil
    end
    if _WXLWOW_RADIAL_WORLD_TO_SCREEN then
        RP.world_to_screen = _WXLWOW_RADIAL_WORLD_TO_SCREEN
        _WXLWOW_RADIAL_WORLD_TO_SCREEN = nil
    end
    if _WXLWOW_RADIAL_UNIT_POSITION then
        RP.unit_position = _WXLWOW_RADIAL_UNIT_POSITION
        _WXLWOW_RADIAL_UNIT_POSITION = nil
    end
    if _WXLWOW_RADIAL_SEND_SERVER then
        RP.send_server = _WXLWOW_RADIAL_SEND_SERVER
        _WXLWOW_RADIAL_SEND_SERVER = nil
    end
    if _WXLWOW_RADIAL_POP_SERVER then
        RP.pop_server = _WXLWOW_RADIAL_POP_SERVER
        _WXLWOW_RADIAL_POP_SERVER = nil
    end

    -- Ping API adapter. The shared retail-UI extension owns general-purpose
    -- compatibility; this domain module owns the C_Ping/C_PingSecure contract.
    Enum = Enum or {}
    Enum.PingMode = Enum.PingMode or { KeyDown = 0, ClickDrag = 1 }
    Enum.PingSubjectType = Enum.PingSubjectType or {
        Assist = 0, Attack = 1, OnMyWay = 2, Warning = 3,
    }
    Enum.PingResult = Enum.PingResult or {
        Success = 0, FailedGeneric = 1, FailedSpamming = 2,
        FailedDisabledByLeader = 3, FailedDisabledBySettings = 4,
        FailedOutOfPingArea = 5, FailedSquelched = 6,
        FailedUnspecified = 7, FailedSilent = 8,
    }
    Enum.PingSetTargetState = Enum.PingSetTargetState or {
        Ok = 0, Failed = 1, Pending = 2,
    }
    Enum.PingTargetOption = Enum.PingTargetOption or { All = 0, Environment = 1 }

    local subjectNames = {
        [Enum.PingSubjectType.Assist] = "Assist",
        [Enum.PingSubjectType.Attack] = "Attack",
        [Enum.PingSubjectType.OnMyWay] = "OnMyWay",
        [Enum.PingSubjectType.Warning] = "Warning",
    }
    RP.secure = RP.secure or { callbacks = {} }
    local Secure = RP.secure
    local function result(ok, pingType)
        return { result = ok and Enum.PingResult.Success or Enum.PingResult.FailedGeneric,
                 type = pingType }
    end
    local function sendPicked(pingType)
        local hit = Secure.hit
        local addon = RP.addon
        if not hit or not addon or type(addon.SendPing) ~= "function" then
            return result(false, pingType)
        end
        addon:SendPing(subjectNames[pingType] or "Warning",
            hit.x, hit.y, hit.z, hit.guidLow, hit.guidHigh, hit.unitName)
        Secure.hit = nil
        return result(true, pingType)
    end

    C_Ping = C_Ping or {}
    if type(C_Ping.GetCooldownInfo) ~= "function" then
        function C_Ping.GetCooldownInfo() return nil end
    end
    if type(C_Ping.GetDefaultPingOptions) ~= "function" then
        function C_Ping.GetDefaultPingOptions()
            return {
                { type=Enum.PingSubjectType.Attack, uiTextureKitID="Attack", orderIndex=1 },
                { type=Enum.PingSubjectType.Warning, uiTextureKitID="Warning", orderIndex=2 },
                { type=Enum.PingSubjectType.OnMyWay, uiTextureKitID="OnMyWay", orderIndex=3 },
                { type=Enum.PingSubjectType.Assist, uiTextureKitID="Assist", orderIndex=4 },
            }
        end
    end
    if type(C_Ping.GetTextureKitForType) ~= "function" then
        function C_Ping.GetTextureKitForType(pingType)
            return subjectNames[pingType] or "Warning"
        end
    end
    if type(C_Ping.TogglePingListener) ~= "function" then
        function C_Ping.TogglePingListener(enabled)
            local callback = Secure.callbacks.toggleListener
            if callback then callback(enabled and true or false)
            elseif enabled and type(RadialPing_OnBinding) == "function" then
                RadialPing_OnBinding()
            end
        end
    end
    if type(C_Ping.SendMacroPing) ~= "function" then
        function C_Ping.SendMacroPing(info)
            local callback = Secure.callbacks.sendMacro
            if callback then return callback(info) end
            if RP.pick_cursor then
                local x, y, z, guidLow, guidHigh = RP.pick_cursor()
                if x then
                    Secure.hit = { x=x, y=y, z=z, guidLow=guidLow, guidHigh=guidHigh }
                    return sendPicked(info and info.type or Enum.PingSubjectType.Warning)
                end
            end
            return result(false, info and info.type)
        end
    end

    C_PingSecure = C_PingSecure or {}
    local function callbackSetter(key)
        return function(callback) Secure.callbacks[key] = callback end
    end
    C_PingSecure.SetPendingPingOffScreenCallback = C_PingSecure.SetPendingPingOffScreenCallback or callbackSetter("offscreen")
    C_PingSecure.SetPingCooldownStartedCallback = C_PingSecure.SetPingCooldownStartedCallback or callbackSetter("cooldown")
    C_PingSecure.SetPingPinFrameAddedCallback = C_PingSecure.SetPingPinFrameAddedCallback or callbackSetter("pinAdded")
    C_PingSecure.SetPingPinFrameRemovedCallback = C_PingSecure.SetPingPinFrameRemovedCallback or callbackSetter("pinRemoved")
    C_PingSecure.SetPingPinFrameScreenClampStateUpdatedCallback = C_PingSecure.SetPingPinFrameScreenClampStateUpdatedCallback or callbackSetter("pinClamp")
    C_PingSecure.SetPingRadialWheelCreatedCallback = C_PingSecure.SetPingRadialWheelCreatedCallback or callbackSetter("wheelCreated")
    C_PingSecure.SetSendMacroPingCallback = C_PingSecure.SetSendMacroPingCallback or callbackSetter("sendMacro")
    C_PingSecure.SetTogglePingListenerCallback = C_PingSecure.SetTogglePingListenerCallback or callbackSetter("toggleListener")
    if type(C_PingSecure.CreateFrame) ~= "function" then
        function C_PingSecure.CreateFrame()
            local callback = Secure.callbacks.wheelCreated
            if callback then callback(WorldFrame or UIParent) end
            return true
        end
    end
    if type(C_PingSecure.ClearHitTestPingInfo) ~= "function" then
        function C_PingSecure.ClearHitTestPingInfo() Secure.hit = nil end
    end
    if type(C_PingSecure.SetHitTestPingTarget) ~= "function" then
        function C_PingSecure.SetHitTestPingTarget()
            if not RP.pick_cursor then return Enum.PingSetTargetState.Failed end
            local x, y, z, guidLow, guidHigh = RP.pick_cursor()
            if not x then return Enum.PingSetTargetState.Failed end
            Secure.hit = { x=x, y=y, z=z, guidLow=guidLow, guidHigh=guidHigh }
            return Enum.PingSetTargetState.Ok
        end
    end
    if type(C_PingSecure.SendHitTestPing) ~= "function" then
        function C_PingSecure.SendHitTestPing(pingType) return sendPicked(pingType) end
    end
    if type(C_PingSecure.SetHitTestTargetAndSendPing) ~= "function" then
        function C_PingSecure.SetHitTestTargetAndSendPing()
            local state = C_PingSecure.SetHitTestPingTarget()
            if state ~= Enum.PingSetTargetState.Ok then return result(false) end
            return sendPicked(Enum.PingSubjectType.Warning)
        end
    end
    if type(C_PingSecure.GetTargetPingReceiver) ~= "function" then
        function C_PingSecure.GetTargetPingReceiver() return nil end
    end
    if type(C_PingSecure.DisplayError) ~= "function" then
        function C_PingSecure.DisplayError(message)
            if UIErrorsFrame and UIErrorsFrame.AddMessage then
                UIErrorsFrame:AddMessage(message or "Unable to ping that target", 1, 0.1, 0.1)
            end
        end
    end
    local function unsupportedPing() return result(false) end
    C_PingSecure.SendPlayerItemPing = C_PingSecure.SendPlayerItemPing or unsupportedPing
    C_PingSecure.SendPlayerSpellCategoryPing = C_PingSecure.SendPlayerSpellCategoryPing or unsupportedPing
    C_PingSecure.SendPlayerSpellPing = C_PingSecure.SendPlayerSpellPing or unsupportedPing
    C_PingSecure.SendUnitPing = C_PingSecure.SendUnitPing or unsupportedPing
end
)lua";
}
