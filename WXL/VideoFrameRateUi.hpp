#pragma once
namespace wxl::native::ui_compat {
// Runs in either GlueXML or FrameXML, without relying on addon loading.
inline constexpr char FrameRateUi[] = R"lua(
do
 if not WXLNativeFrameRateInstalled then
  WXLNativeFrameRateInstalled=true
  local limits={0,60,90,120,144,165,200,240,360}
  local function label(n) return n==0 and "Unlimited" or tostring(n).." FPS" end
  local function install()
   if not VideoOptionsFrame or not CreateFrame then return false end
   if WXLFrameRateButton then return true end
   local panel=CreateFrame("Frame","WXLFrameRatePanel",VideoOptionsFrame)
   panel:SetWidth(330);panel:SetHeight(220);panel:SetPoint("CENTER")
   panel:SetFrameStrata("DIALOG");panel:EnableMouse(true)
   panel:SetBackdrop({bgFile="Interface\\DialogFrame\\UI-DialogBox-Background",edgeFile="Interface\\DialogFrame\\UI-DialogBox-Border",tile=true,tileSize=32,edgeSize=32,insets={left=8,right=8,top=8,bottom=8}})
   local title=panel:CreateFontString(nil,"OVERLAY","GameFontNormalLarge")
   title:SetPoint("TOP",0,-20);title:SetText("Optional frame rate limits")
   local function row(cvar,name,y)
    local text=panel:CreateFontString(nil,"OVERLAY","GameFontNormal")
    text:SetPoint("TOPLEFT",22,y);text:SetText(name)
    local button=CreateFrame("Button",nil,panel,"UIPanelButtonTemplate")
    button:SetWidth(150);button:SetHeight(24);button:SetPoint("TOPRIGHT",-22,y+3)
    local function refresh()button:SetText(label(tonumber(GetCVar(cvar)) or 0))end
    button:SetScript("OnShow",refresh)
    button:SetScript("OnClick",function()
     local current=tonumber(GetCVar(cvar)) or 0
     local nextValue=0
     for _,value in ipairs(limits)do if value>current then nextValue=value;break end end
     SetCVar(cvar,tostring(nextValue));refresh()
    end)
   end
   row("maxFPS","Foreground",-64);row("maxFPSBk","Background",-104)
   local note=panel:CreateFontString(nil,"OVERLAY","GameFontHighlightSmall")
   note:SetPoint("BOTTOM",0,52);note:SetText("Click to cycle. Changes apply immediately.\nUnlimited imposes no frame rate cap.")
   local close=CreateFrame("Button",nil,panel,"UIPanelButtonTemplate")
   close:SetWidth(100);close:SetHeight(22);close:SetPoint("BOTTOM",0,20);close:SetText(CLOSE or "Close")
   close:SetScript("OnClick",function()panel:Hide()end);panel:Hide()
   local b=CreateFrame("Button","WXLFrameRateButton",VideoOptionsFrame,"UIPanelButtonTemplate")
   b:SetWidth(120);b:SetHeight(22);b:SetPoint("TOPRIGHT",-44,-18);b:SetText("Frame rate")
   b:SetScript("OnClick",function()if panel:IsShown()then panel:Hide()else panel:Show()end end)
   VideoOptionsFrame:HookScript("OnHide",function()panel:Hide()end)
   return true
  end
  if not install()then
   local wait=CreateFrame("Frame")
   wait:SetScript("OnUpdate",function(self)if install()then self:SetScript("OnUpdate",nil);self:Hide()end end)
  end
 end
end
)lua";
}
