-- Uses the native limiters. Loading this addon never changes either CVar.
local choices = {0, 60, 90, 120, 144, 165, 200, 240, 360}
local panel, installed
local function label(value)
    return value == 0 and "Unlimited" or tostring(value) .. " FPS"
end

local function dropdown(parent, title, cvar, y)
    local text = parent:CreateFontString(nil, "OVERLAY", "GameFontNormal")
    text:SetPoint("TOPLEFT", 24, y)
    text:SetText(title)
    local menu = CreateFrame("Frame", "WXLFrameRate_" .. cvar, parent, "UIDropDownMenuTemplate")
    menu:SetPoint("TOPLEFT", 8, y - 18)
    UIDropDownMenu_SetWidth(menu, 240)
    UIDropDownMenu_Initialize(menu, function()
        local current = tonumber(GetCVar(cvar)) or 0
        local found = false
        local function entry(value)
            local info = UIDropDownMenu_CreateInfo()
            info.text = label(value)
            info.checked = current == value
            info.func = function()
                SetCVar(cvar, tostring(value))
                UIDropDownMenu_SetText(menu, label(tonumber(GetCVar(cvar)) or 0))
            end
            UIDropDownMenu_AddButton(info)
        end
        for _, value in ipairs(choices) do
            if value == current then found = true end
            entry(value)
        end
        -- Preserve a custom console value, rather than silently rounding it.
        if not found then entry(current) end
    end)
    menu:SetScript("OnShow", function()
        UIDropDownMenu_SetText(menu, label(tonumber(GetCVar(cvar)) or 0))
    end)
end

local function install()
    if WXLFrameRateButton then installed = true; return end
    if installed or not VideoOptionsFrame then return end
    installed = true
    panel = CreateFrame("Frame", "WXLFrameRatePanel", VideoOptionsFrame)
    panel:SetSize(320, 260)
    panel:SetPoint("CENTER")
    panel:SetFrameStrata("DIALOG")
    panel:EnableMouse(true)
    panel:SetBackdrop({bgFile = "Interface\\DialogFrame\\UI-DialogBox-Background",
        edgeFile = "Interface\\DialogFrame\\UI-DialogBox-Border", tile = true,
        tileSize = 32, edgeSize = 32, insets = {left = 8, right = 8, top = 8, bottom = 8}})
    local heading = panel:CreateFontString(nil, "OVERLAY", "GameFontNormalLarge")
    heading:SetPoint("TOP", 0, -20)
    heading:SetText("Frame rate limits")
    dropdown(panel, "Foreground", "maxFPS", -54)
    dropdown(panel, "Background", "maxFPSBk", -116)
    local note = panel:CreateFontString(nil, "OVERLAY", "GameFontHighlightSmall")
    note:SetPoint("BOTTOM", 0, 48)
    note:SetWidth(270)
    note:SetText("Optional. Changes apply immediately.\nVSync may impose its own limit.")
    local done = CreateFrame("Button", nil, panel, "UIPanelButtonTemplate")
    done:SetSize(100, 22)
    done:SetPoint("BOTTOM", 0, 18)
    done:SetText(CLOSE or "Close")
    done:SetScript("OnClick", function() panel:Hide() end)
    panel:Hide()
    local button = CreateFrame("Button", "WXLFrameRateButton", VideoOptionsFrame, "UIPanelButtonTemplate")
    button:SetSize(115, 22)
    button:SetPoint("TOPRIGHT", -44, -18)
    button:SetText("Frame rate")
    button:SetScript("OnClick", function()
        if panel:IsShown() then panel:Hide() else panel:Show() end
    end)
    VideoOptionsFrame:HookScript("OnHide", function() panel:Hide() end)
end

local events = CreateFrame("Frame")
events:RegisterEvent("PLAYER_LOGIN")
events:RegisterEvent("ADDON_LOADED")
events:SetScript("OnEvent", install)
install()
