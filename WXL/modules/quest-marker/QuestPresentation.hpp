// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
namespace wxl::native::quest_marker {
inline constexpr char Bootstrap[]=R"lua(
do
 wxlwow=wxlwow or {}; wxlwow.quest_tracker=wxlwow.quest_tracker or {}
 local tracker=wxlwow.quest_tracker
 tracker.Request=_WXL_QUEST_TRACKER_REQUEST
 tracker.PopMarker=_WXL_QUEST_TRACKER_POP_MARKER
 tracker.PopKillEntries=_WXL_QUEST_TRACKER_POP_KILL
 tracker.PopCorpse=_WXL_QUEST_TRACKER_POP_CORPSE
 _WXL_QUEST_TRACKER_REQUEST=nil; _WXL_QUEST_TRACKER_POP_MARKER=nil
 _WXL_QUEST_TRACKER_POP_KILL=nil; _WXL_QUEST_TRACKER_POP_CORPSE=nil
 if tracker._FrameXMLReady then tracker._FrameXMLReady() end
end
)lua";
// Use native UI textures so D3D9 and D3D11 share the same presentation path.
// Projection, distance, corpse/quest priority and camera bearing stay in C++.
inline constexpr char Presentation[]=R"lua(
do
 if not CreateFrame or not WorldFrame then return end
 local driver=WXLNativeQuestDriver
 if not driver then
  driver=CreateFrame('Frame','WXLNativeQuestDriver',WorldFrame)
  driver:SetAllPoints(WorldFrame)
  driver:EnableMouse(false)
  driver.marker=driver:CreateTexture(nil,'OVERLAY')
  driver.diamond=driver:CreateTexture(nil,'OVERLAY')
  driver.arrow=driver:CreateTexture(nil,'OVERLAY')
  driver.marker:SetTexture('Interface\\Navigation\\InGameNavigationUI')
  driver.diamond:SetTexture('Interface\\Navigation\\InGameNavigationUI')
  driver.arrow:SetTexture('Interface\\Navigation\\InGameNavigationUI')
  driver.diamond:SetTexCoord(.4395,.8725,.002,.543875)
 end
 local function anchor(texture,x,y,w,h)
  texture:ClearAllPoints()
  texture:SetPoint('CENTER',WorldFrame,'BOTTOMLEFT',
   (x+1)*.5*WorldFrame:GetWidth(),(y+1)*.5*WorldFrame:GetHeight())
  texture:SetWidth(w);texture:SetHeight(h);texture:Show()
 end
 driver:SetScript('OnUpdate',function(self)
  WXL_NativeQuestPump()
  self.arrow:Hide()
  local x,y,scale,alpha,corpse,offscreen,angle=WXL_NativeQuestVisual()
  if x then
   local texture=self.marker
   texture:SetAlpha(corpse and 1 or alpha)
   texture:SetVertexColor(1,1,1)
   if offscreen then
    -- Donor atlas coordinates. Native CSimpleTexture rotates geometry so
    -- no adjacent portion of the atlas leaks into the arrow.
    self.arrow:SetTexCoord(.002,.4355,.543875,.8725)
    self.arrow:SetAlpha(alpha)
    anchor(self.arrow,x,y-27*scale/WorldFrame:GetHeight(),14*scale,11*scale)
    WXL_NativeQuestRotateArrow(self.arrow,angle)
    texture:SetTexCoord(.4395,.8725,.002,.543875)
    anchor(texture,x,y,20*scale,25*scale)
   else
    if corpse then
     texture:SetTexCoord(.002,.4355,.002,.52825)
     anchor(texture,x,y,32*28/34,32)
    else
     texture:SetTexCoord(.4395,.8725,.002,.543875)
     anchor(texture,x,y,25.6*scale,32*scale)
    end
   end
  else self.marker:Hide() end
  local dx,dy,dw,dh=WXL_NativeQuestDiamond()
  if dx then
   anchor(self.diamond,dx,dy,dw*WorldFrame:GetWidth(),dh*WorldFrame:GetHeight())
  else self.diamond:Hide() end
 end)
 driver:Show()
end
)lua";
}
