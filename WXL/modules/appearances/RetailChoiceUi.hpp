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
    local parent = barber and G.BarberShopFrame or G.CharacterCreate
    if not parent or not parent:IsShown() then self:Hide(); return end
    local key = WXL_GetRetailChoiceContext()
    if not key then self:Hide(); return end
    if not self.frame then
      local f = CreateFrame("Frame", "WXLNativeRetailOptionsFrame", parent)
      self.frame = f; f:SetWidth(268); f:SetHeight(264)
      f:SetPoint("BOTTOMRIGHT", parent, "BOTTOMRIGHT", -24, 66)
      f:SetBackdrop({bgFile="Interface\\Tooltips\\UI-Tooltip-Background",
                     edgeFile="Interface\\Tooltips\\UI-Tooltip-Border",tile=true,tileSize=16,edgeSize=12,
                     insets={left=4,right=4,top=4,bottom=4}})
      f:SetBackdropColor(0,0,0,0.82)
      self.title=f:CreateFontString(nil,"OVERLAY","GameFontNormal")
      self.title:SetPoint("TOP",f,"TOP",0,-10);self.title:SetText("Retail details")
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
    self.note:SetText(barber and "Preview only; Reset restores saved details" or (WXL_RetailSelectionCanSave() and "" or "Preview only: save support unavailable"))
    if not barber and WXL_HasAlternateAppearance() then self.form:Show() else self.form:Hide() end
    if #self.options>0 or (not barber and WXL_HasAlternateAppearance()) then self.frame:Show() else self:Hide() end
  end
end
)lua";
}
