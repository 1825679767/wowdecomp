// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
namespace wxl::native::client_extensions {
inline constexpr char kLegacyPacketsLua[]=R"wxlua(
WXL_OPCODES={
 CMSG_QUEST_MARKER_REQUEST=0x051F,SMSG_QUEST_MARKER_UPDATE=0x0520,
 CMSG_RADIAL_PING=0x0521,SMSG_RADIAL_PING=0x0522,
 CMSG_RETAIL_ITEM_VARIANTS=0x0523,SMSG_RETAIL_ITEM_VARIANTS=0x0524,
 CMSG_SPELL_CHARGES_REQUEST=0x0525,SMSG_SPELL_CHARGES_UPDATE=0x0526,
 CMSG_SKYRIDING=0x0527,SMSG_SKYRIDING=0x0528,
 CMSG_MOVE_ADD_IMPULSE_ACK=0x0529,SMSG_MOVE_ADD_IMPULSE=0x052A,
 CMSG_CHALLENGE_MODE_REQUEST_STATE=0x052B,SMSG_CHALLENGE_MODE_STATE=0x052C,
 CMSG_START_CHALLENGE_MODE=0x052D,SMSG_CHALLENGE_MODE_BOSS_INFO=0x052E,
 SMSG_CHALLENGE_MODE_KEYSTONE_INFO=0x052F,SMSG_CHALLENGE_MODE_COMPLETE=0x0530,
 CMSG_CHALLENGE_MODE_REQUEST_HISTORY=0x0531,SMSG_CHALLENGE_MODE_HISTORY=0x0532,
 CMSG_SET_LOOT_SPECIALIZATION=0x0533,SMSG_LOOT_SPECIALIZATION=0x0534,
 SMSG_DISPLAY_TOAST=0x0535,SMSG_QUEST_KILL_ENTRIES=0x0536,
 SMSG_QUEST_CORPSE_POSITION=0x0537
}
-- Preserve the compact legacy keys above while also accepting the exact v1.1 transport names.
-- This lets older addons and newer native-module documentation refer to the same assignments.
WXL_OPCODES.CMSG_WXL_QUEST_TRACKER_REQUEST=WXL_OPCODES.CMSG_QUEST_MARKER_REQUEST
WXL_OPCODES.SMSG_WXL_QUEST_TRACKER_MARKER=WXL_OPCODES.SMSG_QUEST_MARKER_UPDATE
WXL_OPCODES.CMSG_WXL_RADIAL_PING=WXL_OPCODES.CMSG_RADIAL_PING
WXL_OPCODES.SMSG_WXL_RADIAL_PING=WXL_OPCODES.SMSG_RADIAL_PING
WXL_OPCODES.CMSG_WXL_RETAIL_ITEM_VARIANTS=WXL_OPCODES.CMSG_RETAIL_ITEM_VARIANTS
WXL_OPCODES.SMSG_WXL_RETAIL_ITEM_VARIANTS=WXL_OPCODES.SMSG_RETAIL_ITEM_VARIANTS
WXL_OPCODES.CMSG_WXL_SPELL_CHARGES_REQUEST=WXL_OPCODES.CMSG_SPELL_CHARGES_REQUEST
WXL_OPCODES.SMSG_WXL_SPELL_CHARGES_UPDATE=WXL_OPCODES.SMSG_SPELL_CHARGES_UPDATE
WXL_OPCODES.CMSG_WXL_SKYRIDING=WXL_OPCODES.CMSG_SKYRIDING
WXL_OPCODES.SMSG_WXL_SKYRIDING=WXL_OPCODES.SMSG_SKYRIDING
WXL_OPCODES.CMSG_WXL_CHALLENGE_MODE_REQUEST_STATE=WXL_OPCODES.CMSG_CHALLENGE_MODE_REQUEST_STATE
WXL_OPCODES.SMSG_WXL_CHALLENGE_MODE_STATE=WXL_OPCODES.SMSG_CHALLENGE_MODE_STATE
WXL_OPCODES.CMSG_WXL_CHALLENGE_MODE_REQUEST_HISTORY=WXL_OPCODES.CMSG_CHALLENGE_MODE_REQUEST_HISTORY
WXL_OPCODES.SMSG_WXL_CHALLENGE_MODE_HISTORY=WXL_OPCODES.SMSG_CHALLENGE_MODE_HISTORY
WXL_OPCODES.SMSG_WXL_CHALLENGE_MODE_BOSS_INFO=WXL_OPCODES.SMSG_CHALLENGE_MODE_BOSS_INFO
WXL_OPCODES.SMSG_WXL_CHALLENGE_MODE_KEYSTONE_INFO=WXL_OPCODES.SMSG_CHALLENGE_MODE_KEYSTONE_INFO
WXL_OPCODES.SMSG_WXL_QUEST_TRACKER_KILL_ENTRIES=WXL_OPCODES.SMSG_QUEST_KILL_ENTRIES
WXL_OPCODES.SMSG_WXL_QUEST_TRACKER_CORPSE=WXL_OPCODES.SMSG_QUEST_CORPSE_POSITION

local N=_WXL_NETWORK
local SendNative=WXL_NativeSendCustom
local PopNative=WXL_NativePopCustom
local writers={WriteUInt8=2,WriteInt8=3,WriteUInt16=4,WriteInt16=5,WriteUInt32=6,WriteInt32=7,WriteUInt64=8,WriteInt64=9,WriteFloat=10,WriteDouble=11,WriteString=12}
local readers={ReadUInt8=13,ReadInt8=14,ReadUInt16=15,ReadInt16=16,ReadUInt32=17,ReadInt32=18,ReadUInt64=19,ReadInt64=20,ReadFloat=21,ReadDouble=22,ReadString=23}
function CreateWXLPacket(opcode,size)
 local id=N(24,opcode,size or 0)
 if not id then return nil end
 local w={id=id}
 for name,operation in pairs(writers)do local op=operation;w[name]=function(self,value)N(op,self.id,value)return self end end
 function w:Size()return N(0,self.id)end
 function w:IsValid()return N(29,self.id)end
 function w:Send()
  local op,bytes=N(25,self.id)
  self.lastSendSucceeded=op and SendNative and SendNative(op,bytes) or false
  return self
 end
 return w
end
local function Reader(data,session)
 local r={data=data,cursor=0,session=session}
 for name,operation in pairs(readers)do local op=operation;r[name]=function(self)
  local value,cursor=N(op,self.data,self.cursor,self.session)
  if value~=nil then self.cursor=cursor end
  return value
 end end
 function r:Size()return N(1,self.data,self.cursor,self.session)end
 return r
end
-- Capture callback storage privately; addons cannot replace another owner's list.
local callbacks={}
function OnWXLPacket(opcode,callback)
 if type(callback)~="function" or not N(27,opcode) then return false end
 local list=callbacks[opcode] or {}
 if #list>=64 then return false end
 callbacks[opcode]=list;list[#list+1]=callback;return true
end
function __FireWXLPacket(opcode,data,session)
 local list=callbacks[opcode]
 if not list or type(data)~="string" then return end
 for i=1,#list do
  -- Each observer owns a fresh cursor; callback errors do not affect others.
  local ok,err=pcall(list[i],Reader(data,session))
  if not ok and geterrorhandler then pcall(geterrorhandler(),err)end
 end
end
local pump
function WXL_StartNativePacketPump()
 if pump or not CreateFrame or not PopNative then return end
 pump=CreateFrame("Frame")
 pump:SetScript("OnUpdate",function()
  local session=N(28)
  if not session or session==0 then return end
  -- Transport already routes builtin and compiled owners exclusively before Pop.
  for i=1,128 do
   local opcode,data=PopNative()
   if not opcode then break end
   if N(27,opcode)then __FireWXLPacket(opcode,data,session)end
  end
 end)
end

)wxlua";
}
