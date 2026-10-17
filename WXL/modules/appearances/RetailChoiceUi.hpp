// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
namespace wxl::native {
inline constexpr char RetailChoiceUi[] = R"lua(
do
  local G = _G
  if type(CreateFrame) ~= "function" then return end
  local UI = { page = 1, rows = {}, options = {}, selected = {} }
  G.WXLNativeRetailOptions = UI
  local function button(parent, text, width)
    local b = CreateFrame("Button", nil, parent)
    b:SetWidth(width); b:SetHeight(22)
    b.text = b:CreateFontString(nil, "OVERLAY", "GameFontNormalSmall")
    b.text:SetAllPoints(b); b.text:SetText(text)
    b:SetHighlightTexture("Interface\\Buttons\\UI-Common-MouseHilight", "ADD")
    return b
  end
  function UI:Hide()
    if self.frame then self.frame:Hide() end
  end
  function UI:Refresh(force)
    local barber = type(WXL_IsBarberRetailPreview)=="function" and WXL_IsBarberRetailPreview()
    if not barber then self:Hide(); return end
    local parent = G.BarberShopFrame
    if not parent or not parent:IsShown() then self:Hide(); return end
    local key = WXL_GetRetailChoiceContext()
    if not key then self:Hide(); return end
    if not self.frame then
      local f = CreateFrame("Frame", "WXLNativeRetailOptionsFrame", parent)
      self.frame = f; f:SetWidth(268); f:SetHeight(290)
      f:SetPoint("BOTTOMRIGHT", parent, "BOTTOMRIGHT", -24, 66)
      f:SetBackdrop({bgFile="Interface\\Tooltips\\UI-Tooltip-Background",
                     edgeFile="Interface\\Tooltips\\UI-Tooltip-Border",tile=true,tileSize=16,edgeSize=12,
                     insets={left=4,right=4,top=4,bottom=4}})
      f:SetBackdropColor(0,0,0,0.82)
      self.title=f:CreateFontString(nil,"OVERLAY","GameFontNormal")
      self.title:SetPoint("TOPLEFT",f,"TOPLEFT",12,-10);self.title:SetText("Retail details")
      self.gear=button(f,"Hide gear",88);self.gear:SetPoint("TOPRIGHT",f,"TOPRIGHT",-8,-6)
      self.gear:SetScript("OnClick",function() local visible=WXL_IsCreationGearVisible();if visible~=nil then WXL_SetCreationGearVisible(visible and 0 or 1);UI:Refresh(true) end end)
      for i=1,6 do
        local rowIndex=i
        local row={}
        row.label=f:CreateFontString(nil,"OVERLAY","GameFontNormalSmall")
        row.label:SetPoint("TOPLEFT",f,"TOPLEFT",12,-28-(i-1)*32);row.label:SetWidth(244)
        row.label:SetJustifyH("LEFT")
        row.value=f:CreateFontString(nil,"OVERLAY","GameFontHighlightSmall")
        row.value:SetPoint("TOPLEFT",row.label,"BOTTOMLEFT",24,-2);row.value:SetWidth(196)
        row.left=button(f,"<",20);row.right=button(f,">",20)
        row.left:SetPoint("RIGHT",row.value,"LEFT",-2,0);row.right:SetPoint("LEFT",row.value,"RIGHT",2,0)
        local function cycle(delta)
          local entry=UI.options[(UI.page-1)*6+rowIndex]
          if not entry then return end
          local choices=WXL_GetRetailChoices(entry.id)
          if not choices or #choices==0 then return end
          local current=UI.selected[entry.id];local at=1
          for n,c in ipairs(choices) do if c.id==current then at=n;break end end
          at=((at-1+delta)%#choices)+1
          if WXL_SetRetailChoice(entry.id,choices[at].id) then UI:Refresh(true) end
        end
        row.left:SetScript("OnClick",function() cycle(-1) end)
        row.right:SetScript("OnClick",function() cycle(1) end)
        UI.rows[i]=row
      end
      self.reset=button(f,"Reset details",108);self.reset:SetPoint("BOTTOMLEFT",f,"BOTTOMLEFT",8,48)
      self.random=button(f,"Randomize",124);self.random:SetPoint("BOTTOMRIGHT",f,"BOTTOMRIGHT",-8,48)
      self.reset:SetScript("OnClick",function() if WXL_ResetRetailChoices() then UI:Refresh(true) end end)
      self.random:SetScript("OnClick",function() if WXL_RandomizeRetailChoices() then UI:Refresh(true) end end)
      self.prev=button(f,"<",24);self.next=button(f,">",24)
      self.prev:SetPoint("BOTTOMLEFT",f,"BOTTOMLEFT",8,22)
      self.next:SetPoint("BOTTOMRIGHT",f,"BOTTOMRIGHT",-8,22)
      self.pages=f:CreateFontString(nil,"OVERLAY","GameFontNormalSmall")
      self.pages:SetPoint("BOTTOM",f,"BOTTOM",0,27)
      self.prev:SetScript("OnClick",function() UI.page=math.max(1,UI.page-1);UI:Refresh(true) end)
      self.next:SetScript("OnClick",function() UI.page=math.min(math.ceil(#UI.options/6),UI.page+1);UI:Refresh(true) end)
      self.form=button(f,"Switch appearance form",160)
      self.form:SetPoint("TOP",f,"BOTTOM",0,-4)
      self.form:SetScript("OnClick",function() if WXL_ShowAlternateAppearance(not WXL_IsAlternateAppearance()) then UI.key=nil;UI:Refresh(true) end end)
      self.note=f:CreateFontString(nil,"OVERLAY","GameFontHighlightSmall")
      self.note:SetPoint("BOTTOM",f,"BOTTOM",0,10);self.note:SetWidth(250)
    end
    if self.frame:GetParent()~=parent then self.frame:SetParent(parent);self.frame:ClearAllPoints();self.frame:SetPoint("BOTTOMRIGHT",parent,"BOTTOMRIGHT",-24,66);self.key=nil end
    if force or self.key~=key then
      local all=WXL_GetRetailOptions() or {};local values=WXL_GetRetailSelection() or {}
      self.options={};self.selected={}
      for i,o in ipairs(all) do
        self.selected[o.id]=values[i]
        if (o.axis==0 or (not barber and WXL_IsAlternateAppearance())) and o.count>1 then self.options[#self.options+1]=o end
      end
      self.key=key;self.page=math.max(1,math.min(self.page,math.ceil(#self.options/6)))
      for i,row in ipairs(self.rows) do
        local o=self.options[(self.page-1)*6+i]
        if o then
          row.label:SetText(o.name or tostring(o.id))
          local text=tostring(self.selected[o.id] or "")
          for _,c in ipairs(WXL_GetRetailChoices(o.id) or {}) do
            if c.id==self.selected[o.id] then text=c.name or tostring(c.id);break end
          end
          row.value:SetText(text);row.label:Show();row.value:Show();row.left:Show();row.right:Show()
        else row.label:Hide();row.value:Hide();row.left:Hide();row.right:Hide() end
      end
      self.pages:SetText(self.page.." / "..math.max(1,math.ceil(#self.options/6)))
    end
    if barber then self.reset:Hide();self.random:Hide() else self.reset:Show();self.random:Show() end
    local gear=not barber and WXL_IsCreationGearVisible()
    if gear~=nil and not barber then self.gear.text:SetText(gear and "Hide gear" or "Show gear");self.gear:Show() else self.gear:Hide() end
    self.note:SetText(barber and "Preview only; Reset restores saved details" or (WXL_RetailSelectionCanSave() and "" or "Preview only: save support unavailable"))
    if not barber and WXL_HasAlternateAppearance() then self.form:Show() else self.form:Hide() end
    if #self.options>0 or (not barber and WXL_HasAlternateAppearance()) then self.frame:Show() else self:Hide() end
  end
end
)lua";
inline constexpr char RetailChoiceDonorUi[] = R"lua(
do
    local owner = _G.WXLModernM2CustomizationBridge or {}
    _G.WXLModernM2CustomizationBridge = owner
    owner.extraBase = 1000
    owner.nativeCycle = _WXL_M2_CUSTOMIZATION_CYCLE
    owner.nativeInfo = _WXL_M2_CUSTOMIZATION_OPTION
    owner.nativeState = _WXL_M2_CUSTOMIZATION_STATE
    owner.nativeChoice = _WXL_M2_CUSTOMIZATION_CHOICE
    owner.nativeSetOption = _WXL_M2_CUSTOMIZATION_OPTION_SET
    owner.nativeCycleOption = _WXL_M2_CUSTOMIZATION_OPTION_CYCLE
    owner.nativeGeneration = _WXL_M2_CUSTOMIZATION_GENERATION
    owner.nativePending = _WXL_M2_CUSTOMIZATION_PENDING
    owner.nativeTiming = _WXL_M2_CUSTOMIZATION_TIMING
    owner.nativeIdentity = _WXL_M2_CUSTOMIZATION_IDENTITY
    owner.nativeBrokenHairColor = _WXL_M2_CUSTOMIZATION_BROKEN_HAIR_COLOR
    owner.nativeBrokenHornStyle = _WXL_M2_CUSTOMIZATION_BROKEN_HORN_STYLE
    owner.nativeTuskarrCreatureFemale = _WXL_M2_CUSTOMIZATION_TUSKARR_CREATURE_FEMALE
    owner.nativeClass = _WXL_M2_CUSTOMIZATION_CLASS
    owner.nativeReset = _WXL_M2_CUSTOMIZATION_RESET
    owner.nativeRandomize = _WXL_M2_CUSTOMIZATION_RANDOMIZE
    owner.nativeDualForm = _WXL_M2_DUAL_FORM_INFO
    local function L(key, fallback)
        local value = _G[key]
        if value and value ~= "" then return value end
        return fallback
    end
    local optionLocaleKeys = {
        ["Skin Color"] = "WXL_CC_OPTION_SKIN_COLOR",
        ["Skin Type"] = "WXL_CC_OPTION_SKIN_TYPE",
        ["Face"] = "WXL_CC_OPTION_FACE",
        ["Hair Style"] = "WXL_CC_OPTION_HAIR_STYLE",
        ["Hair Color"] = "WXL_CC_OPTION_HAIR_COLOR",
        ["Eye Color"] = "WXL_CC_OPTION_EYE_COLOR",
        ["Underclothes Bottom"] = "WXL_CC_OPTION_UNDERCLOTHES_BOTTOM",
        ["Underclothes Top"] = "WXL_CC_OPTION_UNDERCLOTHES_TOP",
        ["Underclothes Color"] = "WXL_CC_OPTION_UNDERCLOTHES_COLOR",
        ["Horns"] = "WXL_CC_OPTION_HORNS",
        ["Horn Style"] = "WXL_CC_OPTION_HORN_STYLE",
        ["Horn Color"] = "WXL_CC_OPTION_HORN_COLOR",
        ["Horn Jewelry"] = "WXL_CC_OPTION_HORN_JEWELRY",
        ["Jewelry Color"] = "WXL_CC_OPTION_JEWELRY_COLOR",
        ["Tendrils"] = "WXL_CC_OPTION_TENDRILS",
        ["Hair & Dress"] = "WXL_CC_OPTION_HAIR_DRESS",
    }
    local choiceLocaleKeys = {
        ["None"] = "WXL_CC_VALUE_NONE",
        ["Swept"] = "WXL_CC_VALUE_SWEPT",
    }
    function owner:LocalizeOption(name)
        local key = name and optionLocaleKeys[name]
        return key and L(key, name) or name
    end
    function owner:LocalizeChoice(name)
        local key = name and choiceLocaleKeys[name]
        return key and L(key, name) or name
    end
    local function selectedClientRace()
        local nativeRace = owner.nativeIdentity and owner.nativeIdentity()
        if nativeRace then return nativeRace end
        local selected = C_CharacterCreation.GetSelectedRace()
        local info = S_CHARACTER_RACES_INFO and S_CHARACTER_RACES_INFO[selected]
        return (info and info.dbcRaceID) or selected
    end
    local function safeModelCall(model, method, ...)
        local callable = model and model[method]
        if type(callable) ~= "function" then return false end
        return pcall(callable, model, ...)
    end
    function owner:EnsureDualFormPreview(needModel)
        local root = _G.CharacterCreate
        local customize = root and root.GenderFrame and root.GenderFrame.CustomizationButton
        if not root or not customize then return false end

        if not self.dualFormButton then
            local button = CreateFrame("Button", "WXLDualFormPreviewButton", root,
                "UIPanelButtonTemplate")
            button:SetSize(170, 24)
            button:SetPoint("BOTTOM", customize, "TOP", 0, 16)
            button:SetFrameStrata("DIALOG")
            button:SetScript("OnClick", function()
                PlaySound("gsCharacterCreationLook")
                owner.dualFormVisible = not owner.dualFormVisible
                owner:RefreshDualFormPreview(true)
            end)
            button:SetScript("OnEnter", function(self)
                if GlueTooltip then
                    GlueTooltip:SetOwner(self, "ANCHOR_RIGHT")
                    GlueTooltip:SetText("Dual Form Preview", 1, 1, 1)
                    GlueTooltip:AddLine(
                        "Shows the alternate form beside the playable preview without changing race or sex.",
                        0.8, 0.8, 0.8)
                    GlueTooltip:Show()
                end
            end)
            button:SetScript("OnLeave", function()
                if GlueTooltip then GlueTooltip:Hide() end
            end)
            self.dualFormButton = button
        end

        if needModel and not self.dualFormModel then
            -- PlayerModel assumes a resident unit/CMO owner. Character selection has neither for
            -- this secondary frame, and clearing that empty owner dereferences null in 0x004E0A30.
            -- A plain Model owns only the explicitly loaded M2 and is created lazily on the click.
            local ok, model = pcall(CreateFrame, "Model",
                "WXLDualFormPreviewModel", root)
            if not ok or not model then return false end
            model:SetSize(360, 540)
            model:SetPoint("BOTTOM", root, "BOTTOM", 250, 105)
            model:SetFrameStrata("MEDIUM")
            model:EnableMouse(true)
            safeModelCall(model, "SetCamera", 0)
            safeModelCall(model, "SetLight", 1, 0, 0, -0.707, -0.707, 0.7,
                1.0, 1.0, 1.0, 0.8, 1.0, 1.0, 0.8)
            safeModelCall(model, "SetFacing", 0)
            safeModelCall(model, "SetSequence", 0)
            model:SetScript("OnMouseDown", function(self, mouseButton)
                if mouseButton == "LeftButton" then
                    self.WXLDragX = GetCursorPosition()
                    local okFacing, facing = pcall(self.GetFacing, self)
                    self.WXLDragFacing = okFacing and facing or 0
                end
            end)
            model:SetScript("OnMouseUp", function(self)
                self.WXLDragX = nil
                self.WXLDragFacing = nil
            end)
            model:SetScript("OnUpdate", function(self)
                if not self.WXLDragX then return end
                local x = GetCursorPosition()
                safeModelCall(self, "SetFacing",
                    (self.WXLDragFacing or 0) + (x - self.WXLDragX) * 0.01)
            end)
            model:Hide()
            self.dualFormModel = model

            local label = root:CreateFontString(nil, "OVERLAY", "GameFontNormalLarge")
            label:SetPoint("BOTTOM", model, "TOP", 0, -18)
            label:SetTextColor(1, 0.82, 0, 1)
            label:Hide()
            self.dualFormLabel = label
        end
        return true
    end

    function owner:HideDualFormPreview(clear)
        if self.dualFormModel then
            self.dualFormModel:Hide()
            if clear and self.dualFormLoaded then
                safeModelCall(self.dualFormModel, "ClearModel")
                self.dualFormLoaded = false
            end
        end
        if self.dualFormLabel then self.dualFormLabel:Hide() end
    end

    function owner:RefreshDualFormPreview(force)
        local pairedRace = _WXL_GILNEAN_CONTEXT and _WXL_GILNEAN_CONTEXT()
        if pairedRace == 12 or pairedRace == 17 or pairedRace == 30 then
            if self.dualFormButton then self.dualFormButton:Hide() end
            self:HideDualFormPreview(true)
            return
        end
        local root = _G.CharacterCreate
        local inCreation = false
        if C_CharacterCreation and C_CharacterCreation.IsInCharacterCreate then
            local ok, active = pcall(C_CharacterCreation.IsInCharacterCreate)
            inCreation = ok and active or false
        end
        local rootVisible = root and root.IsShown and root:IsShown()
        if not root or not (inCreation or rootVisible) then
            if self.dualFormButton then self.dualFormButton:Hide() end
            self.dualFormVisible = false
            self:HideDualFormPreview(true)
            self.dualFormKey = nil
            self.dualFormSelectionKey = nil
            return
        end
        local customization = root and root.CustomizationFrame
        local sex = C_CharacterCreation.GetSelectedSex()
        local path, formName, retailRace, fileDataID, resolutionStatus, race
        if self.nativeDualForm then
            path, formName, retailRace, fileDataID, resolutionStatus, race =
                self.nativeDualForm(sex)
        end
        local eligible = formName and retailRace
        if not eligible then
            if self.dualFormButton then self.dualFormButton:Hide() end
            self.dualFormVisible = false
            self:HideDualFormPreview(true)
            self.dualFormKey = nil
            self.dualFormSelectionKey = nil
            return
        end
        if not self:EnsureDualFormPreview(self.dualFormVisible) then return end

        local key = tostring(race) .. ":" .. tostring(sex) .. ":" ..
            tostring(retailRace) .. ":" .. tostring(fileDataID)
        if self.dualFormSelectionKey ~= key then
            self.dualFormSelectionKey = key
            self.dualFormVisible = false
            self:HideDualFormPreview(true)
            self.dualFormKey = nil
        end
        self.dualFormButton:SetText((self.dualFormVisible and "Hide " or "Show ") ..
            formName .. " Form")
        self.dualFormButton:Show()

        local modelResolved = path and path ~= "" and (resolutionStatus or 0) == 0
        if not modelResolved then
            self.dualFormVisible = false
            self:HideDualFormPreview(false)
            return
        end

        -- The stock customization panel owns the right side and changes the primary camera.
        -- Keep the alternate preview out of that mode without discarding the user's toggle.
        if not self.dualFormVisible or (customization and customization.show) then
            self:HideDualFormPreview(false)
            return
        end

        if force or self.dualFormKey ~= key then
            self.dualFormKey = key
            if self.dualFormLoaded then
                safeModelCall(self.dualFormModel, "ClearModel")
                self.dualFormLoaded = false
            end
            self.dualFormLoaded = safeModelCall(self.dualFormModel, "SetModel", path)
            safeModelCall(self.dualFormModel, "SetCamera", 0)
            safeModelCall(self.dualFormModel, "SetPosition", 0, 0, -1.0)
            safeModelCall(self.dualFormModel, "SetModelScale", race == 12 and 0.78 or 0.62)
            safeModelCall(self.dualFormModel, "SetFacing", 0)
            safeModelCall(self.dualFormModel, "SetSequence", 0)
        end
        self.dualFormLabel:SetText(formName .. " Form")
        self.dualFormLabel:Show()
        self.dualFormModel:Show()
    end
    local function isTuskarrCreatureFemale()
        return owner.nativeTuskarrCreatureFemale and owner.nativeTuskarrCreatureFemale() or false
    end
    local function isMurloc() return false end
    local function isMurlocFemale() return false end
    local function isLegacyBroken() return false end
    owner.legacyBrokenSelection = owner.legacyBrokenSelection or {
        male = { 1, 1, 1, 1, 1 },
        female = { 1, 1, 1, 1, 1, 2 },
    }
    local legacyBrokenCounts = {
        male = { 17, 20, 14, 10, 8 },
        female = { 15, 20, 16, 10, 6, 3 },
    }
    local legacyBrokenSkinSwatches = {
        0xDCEEFF, 0xC8DEFA, 0xB4CEF2, 0x9DBCE8, 0x88A9DC, 0x7697CF,
        0x6585BE, 0x5874AB, 0x4C6398, 0x405486, 0x354674, 0x2D3B64,
        0x253253, 0x202B48, 0x1B253D, 0x171F34, 0x12192B,
    }
    local legacyBrokenHairSwatches = {
        0x81222C, 0x63362A, 0x3E2A37, 0x5C3E2D, 0x843109,
        0xA0A07E, 0x38323B, 0x4C3570, 0x25567E, 0x345539,
    }
    local function legacyBrokenKey()
        -- Native CMO sex is 0/1. Glue GetSelectedSex maps those to 2/3.
        -- Prefer the live identity, and use the actual Glue convention before a model exists.
        if owner.nativeIdentity then
            local race, sex = owner.nativeIdentity()
            if race == 23 and (sex == 0 or sex == 1) then
                return sex == 1 and "female" or "male"
            end
        end
        return C_CharacterCreation.GetSelectedSex() == 3 and "female" or "male"
    end
    local function legacyBrokenState(axis)
        local key = legacyBrokenKey()
        local count = legacyBrokenCounts[key][axis] or 1
        -- The last three private skin rows each ship only the three matching face textures.
        if axis == 2 then
            local skin = owner.legacyBrokenSelection[key][1] or 1
            local compactFaceStart = key == "female" and 13 or 15
            if skin >= compactFaceStart then count = 3 end
        end
        local selected = owner.legacyBrokenSelection[key][axis] or 1
        if selected < 1 or selected > count then selected = 1 end
        return selected, count
    end
    local function resetLegacyBrokenState()
        local key = legacyBrokenKey()
        owner.legacyBrokenSelection[key] = { 1, 1, 1, 1, 1, 2 }
        if owner.nativeBrokenHornStyle then owner.nativeBrokenHornStyle(1) end
        if owner.nativeBrokenHairColor then owner.nativeBrokenHairColor(0) end
    end
    local categoryDefinitions = {
        { key = "head", label = L("WXL_CC_HEAD", "Head"),
            tooltip = L("WXL_CC_HEAD_TOOLTIP", "Head Customizations") },
        { key = "body", label = L("WXL_CC_BODY", "Body"),
            tooltip = L("WXL_CC_BODY_TOOLTIP", "Body Customizations") },
    }
    local function displayCategory(category)
        if category == "body" or category == "accessories" then return "body" end
        if category == "face" or category == "hair" or category == "facial" or category == "head" then return "head" end
        return category
    end
    local function validCategory(category)
        for _, definition in ipairs(categoryDefinitions) do
            if definition.key == category then return true end
        end
        return false
    end
    if not validCategory(owner.category) then owner.category = "body" end

    function owner:RefreshCategoryAvailability()
        if isTuskarrCreatureFemale() or isMurloc() then
            local available = { body = true }
            self.availableCategories = available
            self.category = "body"
            return available
        end
        if isLegacyBroken() then
            local available = { body = true, head = true }
            self.availableCategories = available
            if not available[self.category] then self.category = "body" end
            return available
        end
        local available = {}
        local count = 0
        local ordinal = 1
        while self.nativeInfo do
            local id, _, _, choices, category = self.nativeInfo(ordinal)
            category = displayCategory(category)
            if not id then break end
            if choices and choices > 1 and validCategory(category) and not available[category] then
                available[category] = true
                count = count + 1
            end
            ordinal = ordinal + 1
        end
        -- Model publication and Glue row layout do not become ready in the same callback. A partial
        -- scan must fail open: hiding navigation at that point makes the later DB2 options
        -- unreachable even though their rows still exist. Once two or more categories have been
        -- observed, the race-specific set is stable enough to narrow the strip.
        if count < 2 then
            available = {}
            for _, definition in ipairs(categoryDefinitions) do
                available[definition.key] = true
            end
        end
        self.availableCategories = available
        if not available[self.category] then
            for _, definition in ipairs(categoryDefinitions) do
                if available[definition.key] then
                    self.category = definition.key
                    break
                end
            end
        end
        return available
    end
    if owner.gearVisible == nil then owner.gearVisible = not CHAR_UNDRESSED end

    local function colorChannels(color)
        color = math.floor(color or 0)
        local red = math.floor(color / 65536) % 256
        local green = math.floor(color / 256) % 256
        local blue = color % 256
        return red / 255, green / 255, blue / 255
    end

    function owner:SetSwatch(texture, color, shown)
        if not shown then
            if texture then texture:Hide() end
            return texture
        end
        if not texture then
            texture = self.swatchOwner:CreateTexture(nil, "OVERLAY")
            texture:SetTexture("Interface\\Custom_Glues\\WXLCustomizationSwatch")
            texture:SetTexCoord(0, 0.65625, 0, 0.625)
            texture:SetWidth(42)
            texture:SetHeight(10)
        end
        local red, green, blue = colorChannels(color)
        texture:SetVertexColor(red, green, blue, 1)
        texture:Show()
        return texture
    end

    function owner:HidePalette()
        if self.palette then self.palette:Hide() end
        self.paletteGlueId = nil
        self.paletteLegacyBroken = nil
    end

    function owner:ComposeStock(glueId)
        -- Native Select/SetRetailAxis already marks the owned component dirty.
        return true
    end

    function owner:ApplyPaletteSteps(glueId, delta, steps)
        if steps <= 0 then return end
        -- Keep native arrow choice transitions (including dependent choices), but
        -- submit the synchronous stock composer only for the final selection.
        local cycle = glueId >= self.extraBase and self.nativeCycleOption or self.nativeCycle
        if cycle then
            for _ = 1, steps - 1 do
                if not cycle(glueId, delta) then return end
            end
        else
            for _ = 1, steps - 1 do C_CharacterCreation.SetCustomizationChoice(glueId, delta) end
        end
        C_CharacterCreation.SetCustomizationChoice(glueId, delta)
    end

    function owner:PaletteButton(index)
        local palette = self.palette
        local button = palette.buttons[index]
        if button then return button end

        button = CreateFrame("Button", nil, palette)
        button:SetWidth(24)
        button:SetHeight(24)
        button:SetBackdrop({
            bgFile = "Interface\\Buttons\\WHITE8X8",
            edgeFile = "Interface\\Tooltips\\UI-Tooltip-Border",
            tile = true, tileSize = 8, edgeSize = 9,
            insets = { left = 2, right = 2, top = 2, bottom = 2 },
        })
        button:SetBackdropColor(0.08, 0.08, 0.08, 1)
        button.left = button:CreateTexture(nil, "ARTWORK")
        button.left:SetTexture("Interface\\Buttons\\WHITE8X8")
        button.left:SetPoint("TOPLEFT", button, "TOPLEFT", 4, -4)
        button.left:SetPoint("BOTTOMRIGHT", button, "BOTTOMRIGHT", -4, 4)
        button.right = button:CreateTexture(nil, "OVERLAY")
        button.right:SetTexture("Interface\\Buttons\\WHITE8X8")
        button.right:SetPoint("TOP", button.left, "TOP", 0, 0)
        button.right:SetPoint("BOTTOMRIGHT", button.left, "BOTTOMRIGHT", 0, 0)
        button.right:SetWidth(8)
        button.label = button:CreateFontString(nil, "OVERLAY", "GameFontNormalSmall")
        button.label:SetPoint("LEFT", button, "LEFT", 6, 0)
        button.label:SetPoint("RIGHT", button, "RIGHT", -6, 0)
        button.label:SetJustifyH("CENTER")
        button:SetScript("OnClick", function(self)
            if owner.paletteLegacyBroken and owner.paletteGlueId and self.choiceOrdinal then
                PlaySound("gsCharacterCreationLook")
                local axis = owner.paletteGlueId
                local selected, count = legacyBrokenState(axis)
                local steps = (self.choiceOrdinal - selected) % count
                for _ = 1, steps do C_CharacterCreation.SetCustomizationChoice(axis, 1) end
                owner:HidePalette()
                owner:RefreshRows()
                return
            end
            if owner.paletteGlueId and self.choiceOrdinal and
                C_CharacterCreation and C_CharacterCreation.SetCustomizationChoice then
                PlaySound("gsCharacterCreationLook")
                -- One validated mutation; cycling multiple times in one frame
                -- collides with the native component's pending rebuild owner.
                if owner.nativeSetOption then
                    owner.nativeSetOption(owner.paletteGlueId, self.choiceOrdinal)
                end
                owner:HidePalette()
                owner:RefreshRows()
            end
        end)
        button:SetScript("OnEnter", function(self)
            if not GlueTooltip then return end
            GlueTooltip:SetOwner(self, "ANCHOR_RIGHT")
            GlueTooltip:SetText(self.choiceName and self.choiceName ~= "" and self.choiceName or
                ("Option " .. tostring(self.choiceOrdinal or 0)), 1, 1, 1)
            GlueTooltip:Show()
        end)
        button:SetScript("OnLeave", function()
            if GlueTooltip then GlueTooltip:Hide() end
        end)
        palette.buttons[index] = button
        return button
    end

    function owner:EnsurePalette()
        if self.palette then return self.palette end
        local palette = CreateFrame("Frame", nil, GlueParent)
        palette:SetFrameStrata("DIALOG")
        palette:SetBackdrop({
            bgFile = "Interface\\DialogFrame\\UI-DialogBox-Background",
            edgeFile = "Interface\\DialogFrame\\UI-DialogBox-Border",
            tile = true, tileSize = 32, edgeSize = 24,
            insets = { left = 7, right = 7, top = 7, bottom = 7 },
        })
        palette:SetBackdropColor(0.04, 0.04, 0.04, 0.96)
        palette.title = palette:CreateFontString(nil, "OVERLAY", "GameFontNormal")
        palette.title:SetPoint("TOP", palette, "TOP", 0, -10)
        palette.buttons = {}
        palette:Hide()
        self.palette = palette
        return palette
    end

    function owner:TogglePalette(row)
        if not row or not row.CustomizationIndex then return end
        local legacyBroken = isLegacyBroken() and row.CustomizationIndex < self.extraBase
        if not legacyBroken and
            (not self.nativeState or not self.nativeChoice or not self.nativeSetOption) then return end
        if self.palette and self.palette:IsShown() and
            self.paletteGlueId == row.CustomizationIndex then
            self:HidePalette()
            return
        end

        local name, count, hasSwatch
        if legacyBroken then
            name = row.WXLLegacyName or "Customization"
            _, count = legacyBrokenState(row.CustomizationIndex)
            hasSwatch = row.CustomizationIndex == 1 or row.CustomizationIndex == 4
        else
            name, _, _, count, _, _, hasSwatch = self.nativeState(row.CustomizationIndex)
        end
        if not name or not count or count < 1 then return end
        local palette = self:EnsurePalette()
        self.paletteGlueId = row.CustomizationIndex
        self.paletteLegacyBroken = legacyBroken
        palette.title:SetText(name)
        local columns = hasSwatch and 8 or (count <= 8 and 2 or (count <= 24 and 3 or 4))
        local cellWidth = hasSwatch and 30 or 112
        local cellHeight = hasSwatch and 26 or 25
        local stepX = cellWidth + 3
        local stepY = cellHeight + 3
        local rows = math.ceil(count / columns)
        palette:SetWidth(columns * stepX + 18)
        palette:SetHeight(rows * stepY + 40)
        palette:ClearAllPoints()
        palette:SetPoint("TOPRIGHT", row.Background or row, "TOPLEFT", -8, 4)

        for index = 1, count do
            local choiceName, swatch1, swatch2, selected, choiceHasSwatch
            if legacyBroken then
                local current = legacyBrokenState(row.CustomizationIndex)
                choiceName = tostring(index)
                selected = index == current
                choiceHasSwatch = hasSwatch
                if row.CustomizationIndex == 1 then
                    swatch1 = legacyBrokenSkinSwatches[index]
                elseif row.CustomizationIndex == 4 then
                    swatch1 = legacyBrokenHairSwatches[index]
                end
                swatch2 = swatch1
            else
                choiceName, swatch1, swatch2, selected, choiceHasSwatch =
                    self.nativeChoice(row.CustomizationIndex, index)
            end
            local button = self:PaletteButton(index)
            button.choiceOrdinal = index
            button.choiceName = choiceName
            button:SetWidth(cellWidth)
            button:SetHeight(cellHeight)
            button:ClearAllPoints()
            button:SetPoint("TOPLEFT", palette, "TOPLEFT",
                10 + ((index - 1) % columns) * stepX,
                -30 - math.floor((index - 1) / columns) * stepY)
            if hasSwatch and choiceHasSwatch then
                local red, green, blue = colorChannels(swatch1)
                button.left:SetVertexColor(red, green, blue, 1)
                local second = swatch2 and swatch2 ~= 0 and swatch2 ~= swatch1
                button.left:ClearAllPoints()
                button.left:SetPoint("TOPLEFT", button, "TOPLEFT", 4, -4)
                if second then
                    local red2, green2, blue2 = colorChannels(swatch2)
                    button.left:SetPoint("BOTTOMRIGHT", button, "BOTTOM", 0, 4)
                    button.right:SetVertexColor(red2, green2, blue2, 1)
                    button.right:Show()
                else
                    button.left:SetPoint("BOTTOMRIGHT", button, "BOTTOMRIGHT", -4, 4)
                    button.right:Hide()
                end
                button.left:Show()
                button.label:Hide()
            else
                button.left:Hide()
                button.right:Hide()
                button.label:SetText(choiceName and choiceName ~= "" and
                    self:LocalizeChoice(choiceName) or tostring(index))
                button.label:Show()
            end
            button:SetBackdropBorderColor(selected and 1 or 0.35,
                selected and 0.82 or 0.35, selected and 0 or 0.35, 1)
            button:Show()
        end
        for index = count + 1, #palette.buttons do palette.buttons[index]:Hide() end
        palette:Show()
    end

    function owner:Decorate(row)
        if not row or not row.CustomizationIndex or not self.nativeState then return end
        -- Legacy Broken deliberately uses its private ChrRaces/CharSections contract. Decorating a
        -- stock axis with Retail model 33/34 state replaced the real DBC counts (20 faces, several
        -- skin/hair colours) with the Retail carrier's 1-choice face and female hair rows.
        if isLegacyBroken() and row.CustomizationIndex < self.extraBase then
            local axis = row.CustomizationIndex
            local selected, count = legacyBrokenState(axis)
            local name = row.WXLLegacyAxis == axis and row.WXLLegacyName or
                ((row.CustomizationName and row.CustomizationName:GetText()) or "Customization")
            if axis == 5 then name = legacyBrokenKey() == "female" and "Tendrils" or "Horn Style" end
            if legacyBrokenKey() == "female" and axis == 6 then name = "Horn Style" end
            name = self:LocalizeOption(name)
            row.WXLLegacyName = name
            row.WXLLegacyAxis = axis
            if row.CustomizationName then
                self:SetRowText(row, name, selected .. "/" .. count)
            end
            local hasSwatch = axis == 1 or axis == 4
            local swatch = axis == 1 and legacyBrokenSkinSwatches[selected] or
                (axis == 4 and legacyBrokenHairSwatches[selected] or nil)
            self.swatchOwner = row
            row.WXLSwatch1 = self:SetSwatch(row.WXLSwatch1, swatch, hasSwatch)
            row.WXLSwatch2 = self:SetSwatch(row.WXLSwatch2, nil, false)
            self.swatchOwner = nil
            if row.WXLSwatch1 and hasSwatch then
                row.WXLSwatch1:ClearAllPoints()
                row.WXLSwatch1:SetPoint("CENTER", row, "CENTER", 0, 0)
                row.CustomizationName:SetText("")
            end
            if not row.WXLPaletteButton then
                row.WXLPaletteButton = CreateFrame("Button", nil, row)
                row.WXLPaletteButton:SetPoint("TOPLEFT", row.Background or row, "TOPLEFT", 2, -2)
                row.WXLPaletteButton:SetPoint("BOTTOMRIGHT", row.Background or row,
                    "BOTTOMRIGHT", -2, 2)
                row.WXLPaletteButton:SetScript("OnClick", function(self)
                    owner:TogglePalette(self.ownerRow)
                end)
            end
            row.WXLPaletteButton.ownerRow = row
            row.WXLPaletteButton:Show()
            return
        end
        local name, choiceName, selected, count, swatch1, swatch2, hasSwatch =
            self.nativeState(row.CustomizationIndex)
        if not name then
            local name = self.stockOptionNames and self.stockOptionNames[row.CustomizationIndex]
            if not name or name == "" then name = row.WXLLabel and row.WXLLabel:GetText() end
            if not name or name == "" then name = row.CustomizationName:GetText() end
            if not name or name == "" then name = "Customization" end
            -- Legacy Eredar has no Retail option state. Keep its real stock axis
            -- name separately from the value field instead of clearing/reusing it.
            self:SetRowText(row, name, GetLocale() == "ruRU" and "Стрелки" or "Use arrows")
            if row.WXLSwatch1 then row.WXLSwatch1:Hide() end
            if row.WXLSwatch2 then row.WXLSwatch2:Hide() end
            if row.WXLPaletteButton then row.WXLPaletteButton:Hide() end
            return
        end

        local value = choiceName and choiceName ~= "" and self:LocalizeChoice(choiceName) or
            (tostring(selected or 0) .. "/" .. tostring(count or 0))
        if row.CustomizationName then
            self:SetRowText(row, self:LocalizeOption(name), value)
        end

        self.swatchOwner = row
        row.WXLSwatch1 = self:SetSwatch(row.WXLSwatch1, swatch1, hasSwatch)
        local second = hasSwatch and swatch2 and swatch2 ~= 0 and swatch2 ~= swatch1
        row.WXLSwatch2 = self:SetSwatch(row.WXLSwatch2, swatch2, second)
        self.swatchOwner = nil

        if row.WXLSwatch1 and hasSwatch then
            row.WXLSwatch1:ClearAllPoints()
            row.WXLSwatch1:SetPoint("CENTER", row, "CENTER", second and -14 or 0, 0)
            row.CustomizationName:SetText("")
        end
        if row.WXLSwatch2 and second then
            row.WXLSwatch2:ClearAllPoints()
            row.WXLSwatch2:SetPoint("LEFT", row.WXLSwatch1, "RIGHT", 2, 0)
        end

        if not row.WXLPaletteButton then
            row.WXLPaletteButton = CreateFrame("Button", nil, row)
            row.WXLPaletteButton:SetPoint("TOPLEFT", row.Background or row, "TOPLEFT", 2, -2)
            row.WXLPaletteButton:SetPoint("BOTTOMRIGHT", row.Background or row,
                "BOTTOMRIGHT", -2, 2)
            row.WXLPaletteButton:SetScript("OnClick", function(self)
                owner:TogglePalette(self.ownerRow)
            end)
        end
        row.WXLPaletteButton.ownerRow = row
        if count and count > 1 then row.WXLPaletteButton:Show() else row.WXLPaletteButton:Hide() end
    end

    function owner:RestoreCustomizationStand()
        local preview = _G.WXLCharacterCreationPreview
        if preview and type(preview.EnterCustomization) == "function" then
            preview:EnterCustomization()
        elseif _G._WXL_CC_PREVIEW_PLAY_ANIMATION then
            _WXL_CC_PREVIEW_PLAY_ANIMATION(0)
        end
    end

    function owner:GearIsVisible()
        local visible = WXL_IsCreationGearVisible and WXL_IsCreationGearVisible()
        return visible ~= false
    end

    function owner:SetGearVisible(frame, visible, quiet)
        local setter = _G.WXL_SetCreationGearVisible
        if not setter or not setter(visible and 1 or 0) then return false end
        self.gearVisible = visible and true or false
        if not quiet then self:RestoreCustomizationStand(); self:UpdateTabs(frame) end
        return true
    end

    function owner:UpdateTabs(frame)
        local categoryButtons = frame and frame.WXLCategoryButtons or {}
        for name, button in pairs(categoryButtons) do
            if button.Icon then
                button.Icon:SetAtlas("charactercreate-icon-customize-" .. name ..
                    (name == self.category and "-selected" or ""))
            end
            if name == self.category then
                if button.LockHighlight then button:LockHighlight() end
                local text = button.GetFontString and button:GetFontString()
                if text then text:SetTextColor(1, 0.82, 0, 1) end
            else
                if button.UnlockHighlight then button:UnlockHighlight() end
                local text = button.GetFontString and button:GetFontString()
                if text then text:SetTextColor(1, 1, 1, 1) end
            end
        end
        local gear = frame and frame.ToggleUIButton
        local gearVisible = self:GearIsVisible()
        if gear and gear.ButtonTexture then
            gear.ButtonTexture:SetVertexColor(not gearVisible and 1 or 0.72,
                not gearVisible and 0.82 or 0.72, not gearVisible and 0 or 0.72, 1)
        end
        if gear and gear.WXLLabel then
            gear.WXLLabel:SetText(gearVisible and
                L("WXL_CC_HIDE_GEAR", "Hide Gear") or L("WXL_CC_SHOW_GEAR", "Show Gear"))
        end
        if gear and gear.WXLInnerIcon then gear.WXLInnerIcon:Hide() end
    end

    function owner:SetCategory(category)
        if not validCategory(category) then return end
        self:HidePalette()
        self.category = category
        self.page = 1
        local alternate = _G.WXLGilneanPreview
        if alternate and alternate.front then
            alternate.page = 1
            alternate:RefreshOptions()
            self:UpdateTabs(self.wrappedFrame)
            return
        end
        local frame = self.wrappedFrame
        if frame then
            if type(frame.ResetCamera) == "function" then frame:ResetCamera() end
            if category == "head" and C_CharacterCreation and C_CharacterCreation.ZoomCamera then
                -- Earthen occupies a replacement slot whose Glue camera metadata still belongs to
                -- the taller Night Elf Illidari model. A smaller face delta reproduces Dwarf-like
                -- framing instead of pushing the camera above the head.
                local faceZoom = self.selectedClientRace == 24 and 60 or 80
                C_CharacterCreation.ZoomCamera(faceZoom, 0.5, true)
            end
            self:UpdateTabs(frame)
            if type(frame.UpdateCustomizationButtonFrame) == "function" then
                frame:UpdateCustomizationButtonFrame(false)
            end
        end
    end

    function owner:SetButtonTooltip(button, title, detail)
        button.WXLTooltipTitle = title
        button.WXLTooltipDetail = detail
        button:SetScript("OnEnter", function(self)
            if self.ButtonTexture then self.ButtonTexture:SetVertexColor(1, 0.82, 0, 1) end
            if GlueTooltip then
                GlueTooltip:SetOwner(self, "ANCHOR_RIGHT")
                local tooltipTitle = self.WXLTooltipTitle
                if self.WXLCategory == "gear" then
                    tooltipTitle = owner:GearIsVisible() and
                        L("WXL_CC_HIDE_GEAR", "Hide Gear") or L("WXL_CC_SHOW_GEAR", "Show Gear")
                end
                GlueTooltip:SetText(tooltipTitle, 1, 1, 1)
                if self.WXLTooltipDetail then
                    GlueTooltip:AddLine(self.WXLTooltipDetail, 0.8, 0.8, 0.8)
                end
                GlueTooltip:Show()
            end
        end)
        button:SetScript("OnLeave", function(self)
            owner:UpdateTabs(owner.wrappedFrame)
            if GlueTooltip then GlueTooltip:Hide() end
        end)
        button:SetScript("OnUpdate", nil)
    end

    function owner:LabelButton(button, text)
        if not button.WXLLabel then
            button.WXLLabel = button:CreateFontString(nil, "OVERLAY", "GameFontNormalSmall")
            button.WXLLabel:SetPoint("TOP", button, "BOTTOM", 0, 2)
        end
        button.WXLLabel:SetText(text)
        button.WXLLabel:Show()
    end

    function owner:SetupTabs(frame)
        if not frame or not frame.ToggleUIButton or not frame.SideViewButton or
            not frame.BackViewButton then return end

        frame.SideViewButton:Hide()
        frame.BackViewButton:Hide()
        if frame.SmallButtons and CharacterCreate then
            frame.SmallButtons:ClearAllPoints()
            frame.SmallButtons:SetPoint("TOPLEFT", CharacterCreate, "TOPLEFT", 40, -32)
        end
        frame.WXLCategoryButtons = frame.WXLCategoryButtons or {}
        for key, button in pairs(frame.WXLCategoryButtons) do
            if not validCategory(key) then button:Hide() end
        end
        for i, definition in ipairs(categoryDefinitions) do
            local button = frame.WXLCategoryButtons[definition.key]
            if not button then
                button = CreateFrame("Button", nil, frame)
                button:SetSize(56, 56)
                button.Icon = button:CreateTexture(nil, "ARTWORK")
                button.Icon:SetAllPoints(button)
                button:SetScript("OnClick", function(self)
                    PlaySound("gsCharacterCreationLook")
                    owner:SetCategory(self.WXLCategory)
                end)
                frame.WXLCategoryButtons[definition.key] = button
            end
            button.WXLCategory = definition.key
            button:ClearAllPoints()
            button:SetPoint("TOP", frame, "TOP", (i - 1) * 68 + 38, -65)
            self:LabelButton(button, definition.label)
            self:SetButtonTooltip(button, definition.tooltip)
            button:Show()
        end
        local gear = frame.ToggleUIButton
        gear:ClearAllPoints()
        gear:SetPoint("TOP", frame, "TOP", 174, -65)
        gear.tooltipText = L("WXL_CC_HIDE_GEAR", "Hide Gear")
        gear.uiHidden = false
        gear.WXLCategory = "gear"
        gear:SetAlpha(1)
        gear:Show()
        self:LabelButton(gear, self:GearIsVisible() and
            L("WXL_CC_HIDE_GEAR", "Hide Gear") or L("WXL_CC_SHOW_GEAR", "Show Gear"))
        gear.WXLLabel:ClearAllPoints()
        gear.WXLLabel:SetPoint("TOP", gear, "BOTTOM", 0, 10)
        if gear.WXLInnerIcon then gear.WXLInnerIcon:Hide() end
        self:SetButtonTooltip(gear, L("WXL_CC_HIDE_GEAR", "Hide Gear"),
            L("WXL_CC_GEAR_TOOLTIP", "Toggle equipped preview gear without hiding the controls"))
        gear:SetScript("OnClick", function()
            PlaySound("gsCharacterCreationLook")
            owner:SetGearVisible(frame, not owner:GearIsVisible())
        end)

        if not frame.WXLGearLifecycleHooked and type(frame.HookScript) == "function" then
            frame.WXLGearLifecycleHooked = true
            frame:HookScript("OnHide", function(customizationFrame)
                owner:HidePalette()
                -- Never leak an undressed body atlas into character select or the next recycled CMO.
                if not owner:GearIsVisible() then
                    owner:SetGearVisible(customizationFrame, true, true)
                end
            end)
        end
        self:UpdateTabs(frame)
    end

    function owner:SetRowText(row, name, value)
        if not row.WXLLabel then
            row.WXLLabel = row:CreateFontString(nil, "OVERLAY", "GameFontNormalSmall")
        end
        row.WXLLabel:SetText(name)
        row.WXLLabel:SetTextColor(1, 1, 1)
        row.WXLLabel:ClearAllPoints()
        row.WXLLabel:SetPoint("RIGHT", row, "LEFT", -32, 0)
        row.WXLLabel:SetWidth(116)
        row.WXLLabel:SetJustifyH("RIGHT")
        row.WXLLabel:Show()
        row.CustomizationName:ClearAllPoints()
        row.CustomizationName:SetPoint("CENTER", row, "CENTER", 0, 0)
        row.CustomizationName:SetWidth(116)
        row.CustomizationName:SetText(value)
        row.CustomizationName:SetTextColor(1, 0.82, 0)
    end

    function owner:StyleRow(row)
        row:SetSize(130, 28)
        if row.Background then
            row.Background:ClearAllPoints()
            row.Background:SetPoint("CENTER", row, "CENTER", 0, -1)
            row.Background:SetSize(142, 46)
        end
        for _, item in ipairs({{row.PrevButton, "RIGHT", "LEFT", -4},
                                {row.NextButton, "LEFT", "RIGHT", 4}}) do
            local button = item[1]
            if button then
                button:ClearAllPoints()
                button:SetPoint(item[2], row, item[3], item[4], 0)
                button:SetSize(26, 26)
                if button.Texture then button.Texture:SetSize(26, 26) end
            end
        end
    end

    function owner:StyleDice(button)
        if not button then return end
        if not button.WXLDice then
            -- Remove the red template artwork, retain its existing randomize callback.
            for _, region in ipairs({button:GetRegions()}) do region:Hide() end
            for _, getter in ipairs({"GetNormalTexture", "GetPushedTexture", "GetHighlightTexture", "GetDisabledTexture"}) do
                local texture = button[getter] and button[getter](button)
                if texture then texture:SetAlpha(0) end
            end
            button.WXLDice = button:CreateTexture(nil, "OVERLAY")
            button.WXLDice:SetAllPoints(button)
            button.WXLDice:SetTexture("Interface\\Custom_Glues\\WXLCustomizationDice")
            button.WXLDice:SetTexCoord(0, 1, 0, 1)
            self:SetButtonTooltip(button, "Randomize Appearance")
        end
        button:SetSize(24, 24)
        button.WXLDice:Show()
    end

    function owner:QueueCustomizationWork(callback)
        if self.customizationWork then return false end
        local api = C_CharacterCreation
        self.customizationWork = { callback = callback, frames = 0, settled = 0,
            race = api.GetSelectedRace(), sex = api.GetSelectedSex() }
        self:ShowModelSpinner(true)
        return true
    end

    function owner:UpdateCustomizationWork()
        local work = self.customizationWork
        if not work then return end
        local root, api = CharacterCreate, C_CharacterCreation
        local panel = root and root.CustomizationFrame
        if not root or not root:IsShown() or not panel or not panel:IsShown() or panel.show == false or
            api.GetSelectedRace() ~= work.race or api.GetSelectedSex() ~= work.sex then
            self.customizationWork = nil
            return
        end
        if work.callback then
            work.frames = work.frames + 1
            if work.frames < 2 then return end -- allow the spinner to render before synchronous work
            local callback = work.callback
            work.callback = nil
            self.runningCustomizationWork = true
            local ok, message = pcall(callback)
            self.runningCustomizationWork = false
            if not ok then
                self.customizationWork = nil
                if geterrorhandler then geterrorhandler()(message) end
            end
            return
        end
        if self.nativePending and self.nativePending() then work.settled = 0
        else work.settled = work.settled + 1 end
        if work.settled >= 2 then self.customizationWork = nil end
    end

    function owner:ShowModelSpinner(loading)
        loading = loading or self.customizationWork ~= nil
        local root = _G.CharacterCreate
        local active = root and root:IsShown()
        if not loading or not active then
            if self.modelSpinner then self.modelSpinner:Hide() end
            return
        end
        local spinner = self.modelSpinner
        if not spinner then
            spinner = CreateFrame("Frame", "WXLModelLoadingSpinner", root, "LoadingSpinnerTemplate")
            self.modelSpinner = spinner
            spinner:SetSize(48, 48)
            spinner:SetPoint("CENTER", root, "CENTER", 0, 0)
            spinner:SetFrameStrata("FULLSCREEN_DIALOG")
            spinner:SetFrameLevel(30)
            spinner:EnableMouse(false)
            spinner:SetScript("OnHide", function(self)
                local anim = self.AnimFrame and self.AnimFrame.Anim
                if anim then anim:Stop() end
            end)
            spinner:Hide()
        end
        if not spinner:IsShown() then
            spinner:Show()
            local anim = spinner.AnimFrame and spinner.AnimFrame.Anim
            if anim then anim:Play() end
        end
    end

    function owner:RefreshModelSpinner()
        local preview = _G.WXLCharacterCreationPreview
        if not preview or type(preview.SetModelLoading) ~= "function" then
            self:ShowModelSpinner(false)
            return
        end
        -- The preview schedules this recovery call before every reveal, even when
        -- all slots are already settled. Avoid another synchronous outfit rebuild.
        if type(preview.nativeRestoreEquipment) == "function" and
            preview.nativeRestoreEquipment ~= self.wrappedEquipmentRecovery then
            local restore = preview.nativeRestoreEquipment
            local wrapper = function(...)
                if preview.IsEquipmentReady and preview:IsEquipmentReady() then return true end
                local start = owner.nativeTiming and owner.nativeTiming()
                local result = restore(...)
                if start then owner.nativeTiming(4, start) end
                return result
            end
            self.wrappedEquipmentRecovery = wrapper
            preview.nativeRestoreEquipment = wrapper
        end
        if preview.SetModelLoading ~= self.wrappedModelLoading then
            local original = preview.SetModelLoading
            local wrapper = function(controller, loading)
                local result = original(controller, loading)
                -- Keep the native model visibility/readiness policy, replace only the old overlay.
                if controller.modelLoading then controller.modelLoading:Hide() end
                owner.modelIsLoading = loading and true or false
                owner:ShowModelSpinner(owner.modelIsLoading)
                return result
            end
            self.wrappedModelLoading = wrapper
            preview.SetModelLoading = wrapper
            -- A model may already be awaiting its first reveal when this bridge is registered.
            self.modelIsLoading = preview.rebuildDelay ~= nil or
                (preview.pending and preview.pending.phase == 1 and not preview.pending.revealedAt) or false
        end
        self:ShowModelSpinner(self.modelIsLoading)
    end

    function owner:RefreshRaceInfo()
        local root = CharacterCreate
        local active = root and root:IsShown() and root.CustomizationFrame and root.CustomizationFrame:IsShown() and root.CustomizationFrame.show ~= false
        if not active then
            if self.raceInfoPanel then self.raceInfoPanel:Hide() end
            return
        end
        local api = C_CharacterCreation
        local selected = api and api.GetSelectedRace and api.GetSelectedRace()
        local info = selected and S_CHARACTER_RACES_INFO and S_CHARACTER_RACES_INFO[selected]
        if not info then
            if self.raceInfoPanel then self.raceInfoPanel:Hide() end
            return
        end
        local sex = api.GetSelectedSex and api.GetSelectedSex()
        local token = (sex == 3 and (info.overrideRaceNameFemale or info.raceNameFemale)) or
            info.overrideRaceName or info.raceName
        local name = token and (_G[token] or token) or info.clientFileString or ""
        local tag = string.upper(info.clientFileString or "")
        local description = _G["CHARACTER_CREATE_INFO_RACE_" .. tag .. "_DESC"] or
            _G["RACE_INFO_" .. tag] or ""
        local factionToken = PLAYER_FACTION_GROUP and PLAYER_FACTION_GROUP[info.factionID]
        local faction = factionToken and (_G[string.upper(factionToken)] or factionToken) or ""
        local key = tostring(selected) .. ":" .. tostring(sex) .. ":" .. name .. ":" .. tostring(root:GetHeight())
        local panel = self.raceInfoPanel
        if not panel then
            panel = CreateFrame("Frame", "WXLCustomizationRaceInfo", root)
            self.raceInfoPanel = panel
            panel:SetPoint("TOPLEFT", root, "TOPLEFT", 36, -94)
            panel:SetSize(208, math.max(300, root:GetHeight() - 180))
            panel:SetFrameStrata("FULLSCREEN_DIALOG")
            panel:SetFrameLevel(20)
            panel.title = panel:CreateFontString(nil, "OVERLAY", "GameFontNormalLarge")
            panel.title:SetPoint("TOPLEFT", panel, "TOPLEFT", 0, 0)
            panel.title:SetWidth(208); panel.title:SetJustifyH("LEFT")
            panel.faction = panel:CreateFontString(nil, "OVERLAY", "GameFontNormalSmall")
            panel.faction:SetPoint("TOPLEFT", panel, "TOPLEFT", 0, -26)
            panel.description = panel:CreateFontString(nil, "OVERLAY", "GameFontNormalSmall")
            panel.description:SetPoint("TOPLEFT", panel, "TOPLEFT", 0, -52)
            panel.description:SetWidth(208); panel.description:SetHeight(134)
            panel.description:SetJustifyH("LEFT"); panel.description:SetJustifyV("TOP")
            panel.description:SetTextColor(0.85, 0.85, 0.85)
            panel.descriptionTip = CreateFrame("Button", nil, panel)
            panel.descriptionTip:SetAllPoints(panel.description)
            panel.descriptionTip:SetScript("OnEnter", function()
                if GlueTooltip and panel.fullDescription and panel.fullDescription ~= "" then
                    GlueTooltip:SetOwner(panel.descriptionTip, "ANCHOR_RIGHT")
                    GlueTooltip:SetText(panel.raceName or "", 1, 0.82, 0)
                    GlueTooltip:AddLine(panel.fullDescription, 1, 1, 1, true)
                    GlueTooltip:Show()
                end
            end)
            panel.descriptionTip:SetScript("OnLeave", function() if GlueTooltip then GlueTooltip:Hide() end end)
            panel.heading = panel:CreateFontString(nil, "OVERLAY", "GameFontNormalLarge")
            panel.heading:SetPoint("TOPLEFT", panel, "TOPLEFT", 0, -210)
            panel.rows = {}
            for i = 1, 5 do
                local row = CreateFrame("Button", nil, panel)
                row:SetPoint("TOPLEFT", panel, "TOPLEFT", 0, -245 - (i - 1) * 49)
                row:SetSize(208, 43)
                row.icon = row:CreateTexture(nil, "ARTWORK")
                row.icon:SetSize(32, 32); row.icon:SetPoint("LEFT", row, "LEFT", 0, 0)
                row.text = row:CreateFontString(nil, "OVERLAY", "GameFontNormalSmall")
                row.text:SetPoint("LEFT", row, "LEFT", 42, 0)
                row.text:SetWidth(166); row.text:SetHeight(42)
                row.text:SetJustifyH("LEFT"); row.text:SetJustifyV("MIDDLE")
                row.text:SetTextColor(0.95, 0.95, 0.95)
                row:SetScript("OnEnter", function(self)
                    if GlueTooltip and self.description then
                        GlueTooltip:SetOwner(self, "ANCHOR_RIGHT")
                        GlueTooltip:SetText(self.description, 1, 1, 1, nil, true)
                        GlueTooltip:Show()
                    end
                end)
                row:SetScript("OnLeave", function() if GlueTooltip then GlueTooltip:Hide() end end)
                panel.rows[i] = row
            end
            panel.pages = CreateFrame("Frame", nil, panel, "CharacterCreateCustomizationButtonFrameTemplate")
            panel.pages:SetSize(100, 26); panel.pages.Background:Hide()
            for _, entry in ipairs({{panel.pages.PrevButton, -1}, {panel.pages.NextButton, 1}}) do
                local delta = entry[2]
                entry[1]:SetScript("OnClick", function()
                    panel.page = math.max(1, math.min(panel.pageCount or 1, (panel.page or 1) + delta))
                    owner.raceInfoKey = nil
                    owner:RefreshRaceInfo()
                end)
            end
        end
        panel:SetHeight(math.max(300, root:GetHeight() - 180))
        panel:SetAlpha(1)
        panel:Show()
        if self.raceInfoKey == key then return end
        self.raceInfoKey = key
        if panel.raceID ~= selected then panel.page = 1 end
        panel.raceID = selected
        panel.raceName = name; panel.fullDescription = description
        panel.title:SetText(name); panel.faction:SetText(faction)
        panel.description:SetText(description)
        local traits = {}
        for i = 1, math.min(TOOLTIP_MAX_RACE_ABLILITIES or 10, 20) do
            local text = _G["CHARACTER_CREATE_INFO_RACE_" .. tag .. "_SPELL" .. i .. "_DESC_SHORT"]
            if text and text ~= "" then
                local icon, detail = string.match(text, "([^|]*)|(.+)")
                traits[#traits + 1] = {icon = icon and ("Interface/Icons/" .. icon), text = detail or text}
            end
        end
        if #traits == 0 then
            for i = 1, 10 do
                local text = _G["RACE_INFO_" .. tag .. i]
                if text and text ~= "" then traits[#traits + 1] = {text = text} end
            end
        end
        panel.heading:SetText(#traits > 0 and "Racial Traits" or "Race Details")
        if #traits == 0 then
            if faction ~= "" then traits[#traits + 1] = {text = faction} end
            local alternate = _G.WXLGilneanPreview
            if alternate and alternate.loaded and alternate.primaryName and alternate.alternateName then
                traits[#traits + 1] = {text = alternate.primaryName .. " / " .. alternate.alternateName .. " forms"}
            end
        end
        local size = math.max(1, math.min(5, math.floor((root:GetHeight() - 430) / 49)))
        panel.pageCount = math.max(1, math.ceil(#traits / size))
        panel.page = math.max(1, math.min(panel.page or 1, panel.pageCount))
        for i, row in ipairs(panel.rows) do
            local trait = i <= size and traits[(panel.page - 1) * size + i]
            if trait then
                row.description = trait.text
                row.text:SetText(trait.text)
                row.text:ClearAllPoints()
                row.text:SetPoint("LEFT", row, "LEFT", trait.icon and 42 or 0, 0)
                row.text:SetWidth(trait.icon and 166 or 208)
                if trait.icon then row.icon:SetTexture(trait.icon); row.icon:Show() else row.icon:Hide() end
                row:Show()
            else row:Hide() end
        end
        panel.pages:ClearAllPoints()
        panel.pages:SetPoint("TOP", panel, "TOP", 0, -248 - size * 49)
        panel.pages.CustomizationName:SetText(panel.page .. " / " .. panel.pageCount)
        if panel.pageCount > 1 then panel.pages:Show() else panel.pages:Hide() end
    end

    function owner:LayoutName()
        local root = CharacterCreate
        local nav = root and root.NavigationFrame
        local edit = nav and nav.CreateNameEditBox
        if not edit then return end
        local active = root:IsShown() and root.CustomizationFrame and root.CustomizationFrame:IsShown() and root.CustomizationFrame.show ~= false
        if active then
            if not self.nameOriginal then
                self.nameOriginal = { width = edit:GetWidth(), points = {} }
                for i = 1, edit:GetNumPoints() do
                    self.nameOriginal.points[i] = {edit:GetPoint(i)}
                end
            end
            edit:ClearAllPoints()
            edit:SetPoint("TOP", root, "TOP", 0, -48)
            edit:SetWidth(240)
            if not self.nameLabel then
                self.nameLabel = edit:CreateFontString(nil, "OVERLAY", "GameFontNormalLarge")
                self.nameLabel:SetPoint("BOTTOM", edit, "TOP", 0, 7)
                self.nameLabel:SetText(NAME or "Name")
            end
            self.nameLabel:Show()
        elseif self.nameOriginal then
            edit:ClearAllPoints()
            for _, point in ipairs(self.nameOriginal.points) do edit:SetPoint(unpack(point)) end
            edit:SetWidth(self.nameOriginal.width)
            self.nameOriginal = nil
            if self.nameLabel then self.nameLabel:Hide() end
        end
    end

    function owner:PageSize(frame)
        return math.max(4, math.min(8, math.floor(((frame:GetHeight() or 540) - 220) / 36)))
    end

    function owner:Layout(frame)
        if not frame or not frame.customizationButtonFramePool then return end
        self:SetupTabs(frame)
        self:LayoutName()
        self:RefreshRaceInfo()
        local alternate = _G.WXLGilneanPreview
        if alternate and alternate.front then alternate:EditorVisibility(); return end
        local rows = {}
        local tuskarrCreatureFemale = isTuskarrCreatureFemale()
        local murloc = isMurloc()
        local murlocFemale = isMurlocFemale()
        for row in frame.customizationButtonFramePool:EnumerateActive() do
            if (not tuskarrCreatureFemale or row.CustomizationIndex == self.extraBase) and
                (not murloc or
                    (not murlocFemale and row.CustomizationIndex == 1) or
                    (murlocFemale and row.CustomizationIndex >= self.extraBase and
                        row.CustomizationIndex < self.extraBase + 2)) then
                rows[#rows + 1] = row
            else row:Hide() end
        end
        table.sort(rows, function(a, b) return (a.CustomizationIndex or 0) < (b.CustomizationIndex or 0) end)
        local size = self:PageSize(frame)
        local pages = math.max(1, math.ceil(#rows / size))
        self.page = math.max(1, math.min(self.page or 1, pages))
        if frame.WXLCustomizationScroll then frame.WXLCustomizationScroll:Hide() end
        if frame.WXLCustomizationScrollBar then frame.WXLCustomizationScrollBar:Hide() end
        for i, row in ipairs(rows) do
            self:StyleRow(row)
            self:Decorate(row)
            if row:GetParent() ~= frame then row:SetParent(frame) end
            row:ClearAllPoints()
            row:SetPoint("TOP", frame, "TOP", 82, -155 - ((i - 1) % size) * 36)
            row:SetAlpha(1)
            if i > (self.page - 1) * size and i <= self.page * size then row:Show() else row:Hide() end
        end
        if not frame.WXLPage then
            local page = CreateFrame("Frame", nil, frame, "CharacterCreateCustomizationButtonFrameTemplate")
            frame.WXLPage = page
            page.Background:Hide()
            page:SetSize(100, 28)
            for _, item in ipairs({{page.PrevButton, -1}, {page.NextButton, 1}}) do
                local delta = item[2]
                item[1]:SetScript("OnClick", function()
                    owner.page = math.max(1, math.min(pages, (owner.page or 1) + delta))
                    owner:HidePalette()
                    owner:Layout(frame)
                end)
            end
        end
        -- Rebind the current page count; races and categories can have different lengths.
        local page = frame.WXLPage
        page.PrevButton:SetScript("OnClick", function() owner.page = math.max(1, owner.page - 1); owner:HidePalette(); owner:Layout(frame) end)
        page.NextButton:SetScript("OnClick", function() owner.page = math.min(pages, owner.page + 1); owner:HidePalette(); owner:Layout(frame) end)
        page:ClearAllPoints()
        page:SetPoint("TOP", frame, "TOP", 69, -163 - size * 36)
        page.CustomizationName:SetText(self.page .. " / " .. pages)
        if pages > 1 then page:Show() else page:Hide() end
        local dice = frame.RandomizeCustomizationButton
        if dice then
            self:StyleDice(dice)
            dice:ClearAllPoints()
            dice:SetPoint("TOP", frame, "TOP", -30, -78)
            dice:SetAlpha(1)
            if murlocFemale then dice:Hide() else dice:Show() end
        end
    end

    function owner:RefreshRows()
        local frame = self.wrappedFrame
        if frame then self:Layout(frame) end
    end

    function owner:WrapFrame()
        local frame = _G.CharacterCreateCustomizationFrame
        if not frame and _G.CharacterCreate then
            frame = _G.CharacterCreate.CustomizationFrame
        end
        if not frame or type(frame.UpdateCustomizationButtonFrame) ~= "function" then return end
        if self.wrappedFrame == frame and
            self.wrappedUpdate == frame.UpdateCustomizationButtonFrame then return end

        local original = frame.UpdateCustomizationButtonFrame
        local wrapped = function(customizationFrame, ...)
            local result = original(customizationFrame, ...)
            owner:Layout(customizationFrame)
            return result
        end
        frame.UpdateCustomizationButtonFrame = wrapped
        self.wrappedFrame = frame
        self.wrappedUpdate = wrapped
    end

    function owner:Wrap()
        local api = _G.C_CharacterCreation
        if not api or type(api.SetCustomizationChoice) ~= "function" or
            type(api.GetAvailableCustomizations) ~= "function" then return end
        if self.wrappedTable == api and self.wrappedChoice == api.SetCustomizationChoice and
            self.wrappedAvailable == api.GetAvailableCustomizations then return end

        local originalChoice = api.SetCustomizationChoice
        local originalAvailable = api.GetAvailableCustomizations
        local wrappedChoice = function(optionID, delta)
            -- Worgen has no modern Hair Color axis. Never submit its stale stock
            -- axis 4 to the legacy compositor: it changes body variation textures.
            if owner.selectedClientRace == 12 and optionID == 4 then return false end
            if owner.composeOnly then return originalChoice(optionID, delta) end
            if isLegacyBroken() and optionID and optionID < owner.extraBase then
                local key = legacyBrokenKey()
                local selected, count = legacyBrokenState(optionID)
                local step = delta and delta > 0 and 1 or -1
                selected = ((selected - 1 + step) % count) + 1
                owner.legacyBrokenSelection[key][optionID] = selected
                if optionID == 4 and owner.nativeBrokenHairColor then
                    owner.nativeBrokenHairColor(selected - 1)
                end
                if optionID == 6 and key == "female" then
                    local result = owner.nativeBrokenHornStyle and owner.nativeBrokenHornStyle(selected - 1)
                    if result then owner:ComposeStock(1) end
                    owner:RefreshRows()
                    return result
                end
                local result = originalChoice(optionID, delta)
                owner:RefreshRows()
                return result
            end
            if optionID and optionID >= owner.extraBase then
                if owner.nativeCycleOption then
                    local result = owner.nativeCycleOption(optionID, delta)
                    -- The native override marks the component dirty, but Glue only consumes that
                    -- dirty state reliably when its stock customization composer is submitted too.
                    -- Legacy-axis rows already get that synchronous submission through
                    -- originalChoice; give independent Retail rows (Eyesight, Eye Style, etc.) the
                    -- same pulse so their related-choice materials and geosets update immediately.
                    if result and not isMurlocFemale() then owner:ComposeStock(optionID) end
                    owner:RefreshRows()
                    return result
                end
                return false
            end
            -- Publish the modern override first. The stock call below is the synchronous composer
            -- trigger; doing this afterwards made its first rebuild use the previous modern choice.
            local result
            if owner.nativeState and owner.nativeState(optionID) then
                result = owner.nativeCycle and owner.nativeCycle(optionID, delta)
            else
                result = originalChoice(optionID, delta)
            end
            owner:RefreshRows()
            return result
        end
        api.SetCustomizationChoice = wrappedChoice

        local wrappedAvailable = function(...)
            local stock = originalAvailable(...) or {}
            if WXL_RetailChoiceUsesOwnedOptionsOnly() then stock = {} end
            owner.stockOptionNames = {}
            for _, style in ipairs(stock) do if style.orderIndex and style.name and style.name ~= "" then owner.stockOptionNames[style.orderIndex] = owner:LocalizeOption(style.name) end end
            local styles = {}
            local stockAxis = {}
            local stockCategoryByAxis = {}
            local detectedCategories = {}
            local detectedCount = 0
            local tuskarrCreatureFemale = isTuskarrCreatureFemale()
            local murloc = isMurloc()
            local murlocFemale = isMurlocFemale()
            local legacyBroken = isLegacyBroken()
            for _, style in ipairs(stock) do
                if style and style.orderIndex then stockAxis[style.orderIndex] = true end
                local axis = style and style.orderIndex
                local stockCategory = axis == 1 and "body" or
                    (axis == 2 and "face" or
                    ((axis == 3 or axis == 4) and "hair" or "facial"))
                if axis then stockCategoryByAxis[axis] = stockCategory end
                if legacyBroken and style then
                    if axis == 1 then style.name = "Skin Color" end
                    if axis == 4 then style.name = "Hair Color" end
                    if axis == 5 and legacyBrokenKey() == "male" then style.name = "Horn Style" end
                end
                -- The private legacy Broken rows still inherit several Retail-era labels from the
                -- shared Glue table. None of these four axes has a backing legacy model/material
                -- choice, so presenting them only cycles inert state (for both sexes).
                local brokenDead = legacyBroken and style and style.name and
                    (style.name == "Tattoo" or string.find(style.name, "Eye", 1, true))
                local brokenMeaningful = not legacyBroken or not brokenDead
                if legacyBroken and legacyBrokenKey() == "female" and axis == 5 then
                    -- Verified against the private female model: this axis selects hanging
                    -- facial tendrils (101..105), not the separate forehead/horn group.
                    style.name = "Tendrils"
                end
                if style and style.name then style.name = owner:LocalizeOption(style.name) end
                if legacyBroken and brokenMeaningful then
                    detectedCategories[displayCategory(stockCategory)] = true
                end
                local murlocMeaningful = not murloc or (not murlocFemale and axis == 1)
                local worgenMeaningful = owner.selectedClientRace ~= 12 or axis ~= 4
                if not tuskarrCreatureFemale and brokenMeaningful and murlocMeaningful and worgenMeaningful and
                    owner.category == displayCategory(stockCategory) then
                    styles[#styles + 1] = style
                end
            end
            local mappedLegacyAxis = {}
            local ordinal = 1
            while owner.nativeInfo do
                local id, name, legacyAxis, choices, category = owner.nativeInfo(ordinal)
                if not id then break end
                -- Do not mix Retail Broken model 33/34 choices into the deliberately restored old
                -- models. Their actual five axes are already represented by the stock DBC rows.
                local meaningful = not legacyBroken and (not murloc or murlocFemale) and
                    (not tuskarrCreatureFemale or name == "Skin Color")
                if meaningful and choices and choices > 1 and validCategory(displayCategory(category)) and
                    not detectedCategories[displayCategory(category)] then
                    detectedCategories[displayCategory(category)] = true
                    detectedCount = detectedCount + 1
                end
                local categoryMatches = owner.category == displayCategory(category)
                if meaningful and choices and choices > 1 and categoryMatches then
                    -- The first modern analogue of each legacy axis is already controlled by the
                    -- matching stock row. Later options with the same broad label remain independent.
                    if not tuskarrCreatureFemale and legacyAxis and legacyAxis > 0 and
                        stockAxis[legacyAxis] and stockCategoryByAxis[legacyAxis] == category and
                        not mappedLegacyAxis[legacyAxis] then
                        mappedLegacyAxis[legacyAxis] = true
                    else
                        styles[#styles + 1] = {
                            orderIndex = id, name = owner:LocalizeOption(name)
                        }
                    end
                end
                ordinal = ordinal + 1
            end
            if legacyBroken and legacyBrokenKey() == "female" and owner.category == "head" then
                styles[#styles + 1] = {
                    orderIndex = 6,
                    name = owner:LocalizeOption("Horn Style")
                }
            end
            if legacyBroken then
                detectedCategories = { body = true, head = true }
            elseif murloc then
                detectedCategories = { body = true }
            elseif detectedCount < 2 and not tuskarrCreatureFemale then
                detectedCategories = {}
                for _, definition in ipairs(categoryDefinitions) do
                    detectedCategories[definition.key] = true
                end
            end
            owner.availableCategories = detectedCategories
            return styles
        end
        api.GetAvailableCustomizations = wrappedAvailable

        -- Glue's race enum is selection order, not the private ChrRaces.dbc id used by native model
        -- and appearance code. Passing the ordinal directly makes, for example, selection 11
        -- (Earthen) briefly select client race 11 (Draenei) until the M2 path is observed. Publish
        -- the explicit client id on the selection event so the first composed sheet is already for
        -- the right race. A late Glue identity layer may provide dbcRaceID for replacement slots;
        -- the full table keeps this bridge independent from that optional Lua payload.
        local function wrapReset(name, native, publishContext)
            local original = api[name]
            if type(original) ~= "function" then return end
            api[name] = function(...)
                if name == "RandomizeCharCustomization" and not owner.runningCustomizationWork and
                    CharacterCreate and CharacterCreate:IsShown() and CharacterCreate.CustomizationFrame and
                    CharacterCreate.CustomizationFrame:IsShown() and CharacterCreate.CustomizationFrame.show ~= false then
                    local args = {...}
                    return owner:QueueCustomizationWork(function() api[name](unpack(args)) end)
                end
                if name == "RandomizeCharCustomization" and isTuskarrCreatureFemale() then
                    -- This creature preview has no stock female humanoid axes.
                    -- Use only the verified skin variants, without stock randomize
                    -- or ComposeStock (both enter the legacy humanoid compositor).
                    local result = native and native()
                    owner:RefreshRows()
                    return result
                end
                if name == "RandomizeCharCustomization" and isLegacyBroken() then
                    for axis = 1, (legacyBrokenKey() == "female" and 6 or 5) do
                        local selected, count = legacyBrokenState(axis)
                        local target = math.random(1, count)
                        local steps = (target - selected) % count
                        for _ = 1, steps do api.SetCustomizationChoice(axis, 1) end
                    end
                    owner:RefreshRows()
                    return true
                end
                local measure = publishContext and owner.nativeTiming
                local started = measure and measure()
                local nativeOwnsAction = native and not publishContext
                local result
                if nativeOwnsAction then result = native() else result = original(...) end
                if measure then started = measure(1, started) end
                if native then
                    if publishContext then
                        owner.selectedClientRace = selectedClientRace()
                        native(owner.selectedClientRace, api.GetSelectedSex())
                    elseif not nativeOwnsAction then
                        native()
                    end
                end
                if measure then started = measure(2, started) end
                if isLegacyBroken() then resetLegacyBrokenState() end
                if native == owner.nativeRandomize then owner:ComposeStock(1) end
                if name == "SetSelectedSex" then
                    -- The class-preview wrapper treats a sex rebuild like a fresh showcase and
                    -- starts its combat sequence. We are already inside customization: cancel that
                    -- pending showcase after the model swap and settle Stand once more after Glue.
                    local preview = _G.WXLCharacterCreationPreview
                    if preview and type(preview.EnterCustomization) == "function" then
                        preview:EnterCustomization()
                    else
                        owner:RestoreCustomizationStand()
                    end
                    owner.restoreStandDelay = 0.20
                end
                owner:RefreshRows()
                if measure then measure(3, started) end
                return result
            end
        end
        wrapReset("ResetCharCustomize", owner.nativeReset)
        wrapReset("RandomizeCharCustomization", owner.nativeRandomize)
        wrapReset("SetSelectedRace", owner.nativeReset, true)
        wrapReset("SetSelectedSex", owner.nativeReset, true)

        self.wrappedTable = api
        self.wrappedChoice = wrappedChoice
        self.wrappedAvailable = wrappedAvailable
    end

    -- At Glue startup the stock character-creation singleton does not exist yet.  Generation zero
    -- means CustomizeNoteModel has not observed a real preview model, so remember it without asking
    -- the stock frame to enumerate customizations.  Calling that path during the white-screen
    -- transition dereferences the client's null character-creation global.
    owner.generation = owner.nativeGeneration and owner.nativeGeneration() or 0

    if not owner.frame then
        owner.frame = CreateFrame("Frame")
        owner.frame:SetScript("OnUpdate", function(_, elapsed)
            owner:UpdateCustomizationWork()
            owner:RefreshModelSpinner()
            -- Resolve the independent form control first. The broader customization wrapper can
            -- legitimately be mid-rebuild while Glue changes race/sex; it must not starve this UI.
            owner.dualFormRefreshElapsed = (owner.dualFormRefreshElapsed or 0) +
                (elapsed or 0)
            if owner.dualFormRefreshElapsed >= 0.10 then
                owner.dualFormRefreshElapsed = 0
                owner:RefreshDualFormPreview(false)
            end
            local preview = _G.WXLCharacterCreationPreview
            local classID = preview and preview.classID or _G.SELECTED_CLASS
            if owner.nativeClass and classID and classID ~= owner.classID then
                owner.classID = classID
                if owner.nativeClass(classID) then
                    -- Class-gated eye shells (notably DK 1701) are outside the five stock bytes.
                    -- Submit one harmless stock composition after publishing the class so the
                    -- already-created preview consumes the new geoset decision immediately.
                    owner:ComposeStock(1)
                end
            end
            owner:Wrap()
            owner:WrapFrame()
            owner:LayoutName()
            owner:RefreshRaceInfo()
            local customizationFrame = owner.wrappedFrame
            if owner.palette and owner.palette:IsShown() and customizationFrame and
                not customizationFrame:IsShown() then
                owner:HidePalette()
            end
            if owner.restoreStandDelay then
                owner.restoreStandDelay = owner.restoreStandDelay - (elapsed or 0)
                if owner.restoreStandDelay <= 0 then
                    owner.restoreStandDelay = nil
                    local frame = owner.wrappedFrame
                    if frame and frame:IsShown() then owner:RestoreCustomizationStand() end
                end
            end
            if owner.nativeGeneration then
                local generation = owner.nativeGeneration()
                local frame = owner.wrappedFrame
                if generation and generation > 0 and generation ~= owner.generation and
                    frame and frame:IsShown() and
                    type(frame.UpdateCustomizationButtonFrame) == "function" then
                    -- Set this before entering the stock update: it can run scripts which yield a
                    -- nested frame update, and the refresh must remain one-shot under re-entry.
                    owner.generation = generation
                    if frame then
                        frame:UpdateCustomizationButtonFrame(false)
                    end
                end
            end
        end)
    end
end

do
 local owner = WXLModernM2CustomizationBridge
 function owner:RefreshNativeOwnership()
  local root = CharacterCreate
  local frame = root and root.CustomizationFrame
  if not frame then return end
  if not self.nativeSaveNote then
   self.nativeSaveNote = frame:CreateFontString(nil, "OVERLAY", "GameFontHighlightSmall")
   self.nativeSaveNote:SetPoint("BOTTOM", frame, "BOTTOM", 0, 4)
   self.nativeSaveNote:SetWidth(260)
  end
  self.nativeSaveNote:SetText(WXL_RetailSelectionCanSave() and "" or "Preview only: save support unavailable")
  local customize = root and root.GenderFrame and root.GenderFrame.CustomizationButton
  if not customize then return end
  if not self.nativeFormButton then
   local b = CreateFrame("Button", nil, root)
   b:SetWidth(customize:GetWidth()); b:SetHeight(customize:GetHeight())
   -- Match the mounted retail control's round artwork, not a stock panel button.
   for _,state in ipairs({"Normal","Pushed","Highlight","Disabled"}) do
    local getter=customize["Get"..state.."Texture"]
    local texture=getter and getter(customize)
    if texture and texture:GetTexture() then b["Set"..state.."Texture"](b,texture:GetTexture()) end
   end
   local label=b:CreateFontString(nil,"OVERLAY","GameFontNormalSmall")
   label:SetPoint("TOP",b,"BOTTOM",0,-2);label:SetText("Switch Form")
   b:SetFrameStrata(customize:GetFrameStrata())
   b:SetScript("OnClick", function()
    if WXL_ShowAlternateAppearance(not WXL_IsAlternateAppearance()) then
     owner:HidePalette(); owner.generation = nil; owner:RefreshRows()
    end
   end)
   self.nativeFormButton = b
  end
  self.nativeFormButton:ClearAllPoints()
  self.nativeFormButton:SetPoint("TOP",customize,"BOTTOM",0,-10)
  if root:IsShown() and not frame.show and WXL_HasAlternateAppearance() then self.nativeFormButton:Show() else self.nativeFormButton:Hide() end
 end
 -- Native ownership switches the actual central preview. Retire the old
 -- secondary-model panel/button instead of offering two incompatible controls.
 function owner:RefreshDualFormPreview(force)
  if self.dualFormButton then self.dualFormButton:Hide() end
  self:HideDualFormPreview(false)
  self:RefreshNativeOwnership()
 end
end
)lua";
}
