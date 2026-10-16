// Adapted from WXL wxl-spell-charges/SpellCharges.cpp.
// Copyright (C) 2026 WarcraftXL. GPLv3 or later.
#pragma once
namespace wxl::native::protocol::consumers {
inline constexpr char SpellChargesBootstrap[] = R"lua(
do
    wxlwow = wxlwow or {}
    wxlwow.request_spell_charges = _WXLWOW_REQUEST_SPELL_CHARGES
    wxlwow.spell_charge_snapshot = _WXLWOW_SPELL_CHARGE_SNAPSHOT
    wxlwow.spell_charge_row = _WXLWOW_SPELL_CHARGE_ROW

    if type(GetActionInfo) ~= "function" or type(HasAction) ~= "function" or
       type(CreateFrame) ~= "function" or type(GetActionCount) ~= "function" or
       type(GetActionCooldown) ~= "function" or type(IsStackableAction) ~= "function" or
       type(IsUsableAction) ~= "function" then return end
    local GCD_SPELL = 61304
    local SC = _G.WXLSpellCharges or {}
    _G.WXLSpellCharges = SC
    SC.spells = SC.spells or {}
    SC.revision = SC.revision or 0
    SC.nativeRevision = SC.nativeRevision or -1

    if not SC.nativeGetActionCount then
        SC.nativeGetActionCount = GetActionCount
        SC.nativeGetActionCooldown = GetActionCooldown
        SC.nativeIsStackableAction = IsStackableAction
        SC.nativeIsUsableAction = IsUsableAction
    end

    local buttonPrefixes = {
        "ActionButton", "BonusActionButton", "MultiBarBottomLeftButton",
        "MultiBarBottomRightButton", "MultiBarLeftButton", "MultiBarRightButton"
    }

    local function dataForAction(action)
        if not action or not HasAction(action) then return nil end
        local kind, actionId, subType, explicitSpellId = GetActionInfo(action)
        if kind ~= "spell" then return nil end
        actionId = tonumber(actionId)
        explicitSpellId = tonumber(explicitSpellId)
        if explicitSpellId and SC.spells[explicitSpellId] then
            return SC.spells[explicitSpellId]
        end
        if actionId then return SC.spells[actionId] end
        return nil
    end

    local function recharge(data)
        local now = GetTime()
        if not data or data.current >= data.maximum or
           not data.rechargeEnd or data.rechargeEnd <= now or
           data.durationMs <= 0 then
            return nil
        end
        local duration = data.durationMs / 1000
        return data.rechargeEnd - duration, duration, 1
    end

    local function gcd(action)
        local start, duration, enable
        if action then
            start, duration, enable = SC.nativeGetActionCooldown(action)
        end
        if start and duration and enable ~= 0 and start > 0 and
           duration > 0 and duration <= 2.5 and
           start + duration > GetTime() then
            return start, duration, enable
        end
        if GetSpellCooldown then
            start, duration, enable = GetSpellCooldown(GCD_SPELL)
            if start and duration and enable ~= 0 and start > 0 and
               duration > 0 and duration <= 2.5 and
               start + duration > GetTime() then
                return start, duration, enable
            end
        end
        return nil
    end

    local function buttonAction(button)
        if not button then return nil end
        local action = button.action
        if not action and button.GetAttribute then
            action = button:GetAttribute("action")
        end
        if not action and ActionButton_GetPagedID then
            action = ActionButton_GetPagedID(button)
        end
        return action
    end

    local function updateButton(button)
        if not button then return end
        local action = buttonAction(button)
        local data = dataForAction(action)
        if not data and not button.wxlCharges then return end
        local name = button:GetName()
        local count = name and _G[name .. "Count"]
        local cooldown = name and _G[name .. "Cooldown"]
        local icon = name and _G[name .. "Icon"]

        if not data then
            if count and button.wxlCharges then count:Hide() end
            if icon and button.wxlCharges then
                icon:SetVertexColor(1.0, 1.0, 1.0)
            end
            button.wxlCharges = nil
            return
        end

        button.wxlCharges = true
        if count and data.maximum > 1 then
            count:SetText(tostring(data.current))
            count:Show()
        elseif count then
            count:Hide()
        end

        local start, duration, enable = gcd(action)
        if not start then start, duration, enable = recharge(data) end
        if cooldown and start then
            if CooldownFrame_SetTimer then
                CooldownFrame_SetTimer(cooldown, start, duration, enable)
            else
                cooldown:SetCooldown(start, duration)
            end
            cooldown:Show()
        elseif cooldown then
            cooldown:Hide()
        end

        if icon then
            local shade = data.current > 0 and 1.0 or 0.4
            icon:SetVertexColor(shade, shade, shade)
        end
    end

    local function updateBars()
        for _, prefix in ipairs(buttonPrefixes) do
            for index = 1, 12 do
                updateButton(_G[prefix .. index])
            end
        end
    end

    local function syncSnapshot(force)
        if not wxlwow.spell_charge_snapshot or
           not wxlwow.spell_charge_row then return false end
        local count, revision = wxlwow.spell_charge_snapshot()
        count = tonumber(count) or 0
        revision = tonumber(revision) or 0
        if not force and revision == SC.nativeRevision then return false end

        local nextState = {}
        local now = GetTime()
        for index = 1, count do
            local spellId, groupId, maximum, current, durationMs, remainingMs =
                wxlwow.spell_charge_row(index)
            if spellId and maximum and maximum > 0 then
                current = math.max(0, math.min(maximum, current or 0))
                nextState[spellId] = {
                    spellId = spellId,
                    groupId = groupId or 0,
                    maximum = maximum,
                    current = current,
                    durationMs = durationMs or 0,
                    rechargeEnd = remainingMs and remainingMs > 0 and
                        (now + remainingMs / 1000) or 0
                }
            end
        end
        SC.spells = nextState
        SC.nativeRevision = revision
        SC.revision = SC.revision + 1
        return true
    end

    SC._NativeChanged = function()
        if syncSnapshot(true) then updateBars() end
    end

    SC.Request = function()
        if wxlwow.request_spell_charges then
            return wxlwow.request_spell_charges()
        end
        return false
    end

    GetActionCount = function(action)
        local data = dataForAction(action)
        if data then return data.current end
        return SC.nativeGetActionCount(action)
    end

    IsStackableAction = function(action)
        if dataForAction(action) then return 1 end
        return SC.nativeIsStackableAction(action)
    end

    GetActionCooldown = function(action)
        local data = dataForAction(action)
        if data then
            local start, duration, enable = gcd(action)
            if start then return start, duration, enable end
            start, duration, enable = recharge(data)
            if start then return start, duration, enable end
        end
        return SC.nativeGetActionCooldown(action)
    end

    IsUsableAction = function(action)
        local usable, noMana = SC.nativeIsUsableAction(action)
        local data = dataForAction(action)
        if data and data.current <= 0 then return false, noMana end
        return usable, noMana
    end

    function WXL_GetSpellCharges(spellId)
        local data = SC.spells[tonumber(spellId)]
        if not data then return nil end
        local start, duration = recharge(data)
        return data.current, data.maximum, start or 0, duration or 0, 1
    end

    if not _G.GetSpellCharges then
        _G.GetSpellCharges = WXL_GetSpellCharges
    end

    if not SC.frame then SC.frame = CreateFrame("Frame") end
    SC.frame:RegisterEvent("PLAYER_LOGIN")
    SC.frame:RegisterEvent("PLAYER_ENTERING_WORLD")
    SC.frame:RegisterEvent("LEARNED_SPELL_IN_TAB")
    SC.frame:RegisterEvent("SPELLS_CHANGED")
    SC.frame:RegisterEvent("ACTIONBAR_SLOT_CHANGED")
    SC.frame:SetScript("OnEvent", function(_, event)
        syncSnapshot(false)
        updateBars()
        if event ~= "ACTIONBAR_SLOT_CHANGED" then SC.Request() end
    end)
    SC.frame:SetScript("OnUpdate", function(_, elapsed)
        SC.elapsed = (SC.elapsed or 0) + elapsed
        if SC.elapsed >= 0.1 then
            SC.elapsed = 0
            syncSnapshot(false)
            updateBars()
        end
    end)

    if type(SlashCmdList) == "table" then
        SLASH_WXLSPELLCHARGES1 = "/wxlcharges"
        SlashCmdList.WXLSPELLCHARGES = function()
            SC.Request()
            local count = 0
            for _ in pairs(SC.spells) do count = count + 1 end
            if DEFAULT_CHAT_FRAME then
                DEFAULT_CHAT_FRAME:AddMessage(
                    "WXL spell charges: " .. count ..
                    " managed spell(s), revision " .. SC.revision)
            end
        end
    end

    syncSnapshot(true)
end
)lua";
}
