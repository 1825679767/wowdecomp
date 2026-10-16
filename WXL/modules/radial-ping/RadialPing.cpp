// Native world-pick/projection and authoritative ping bridge.
// Adapted from extensions/wxl-radial-ping/src/RadialPing.cpp.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "RadialPing.hpp"
#include "RadialBootstrap.hpp"
#include "NativeFeatures.hpp"
#include "NativeProtocol.hpp"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Base/Coordinate.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstring>
#include <deque>
#include <limits>
#include <mutex>
#include <span>
#include <vector>

namespace wxl::native::radial_ping {
namespace {
static_assert(std::endian::native == std::endian::little);
constexpr uint16_t Request=0x521,Response=0x522;
constexpr size_t MaxName=64,MaxQueued=32;
struct Ping {
 uint8_t type=0;double x=0,y=0,z=0;uint32_t guidLow=0,guidHigh=0;
 std::array<char,MaxName> unit{},sender{};uint32_t unitLength=0,senderLength=0;
};
std::mutex mutex;
std::deque<Ping> pending;
uint64_t session=0;
uint32_t mapId=0;
bool installed=false,changed=false;
bool FinitePosition(double x,double y,double z){const double max=std::numeric_limits<float>::max();return std::isfinite(x)&&std::isfinite(y)&&std::isfinite(z)&&std::abs(x)<=max&&std::abs(y)<=max&&std::abs(z)<=max;}
struct Reader {
 std::span<const uint8_t> bytes;size_t cursor=0;
 template<class T>bool Read(T& value){if(cursor>bytes.size()||sizeof(T)>bytes.size()-cursor)return false;std::memcpy(&value,bytes.data()+cursor,sizeof(T));cursor+=sizeof(T);return true;}
 bool Name(std::array<char,MaxName>& out,uint32_t& length){if(!Read(length)||length>MaxName||length>bytes.size()-cursor)return false;if(length)std::memcpy(out.data(),bytes.data()+cursor,length);cursor+=length;return true;}
};
void Reset(void*) noexcept {try{std::lock_guard lock(mutex);pending.clear();session=0;mapId=0;changed=false;}catch(...){}}
void Receive(uint16_t,const uint8_t* bytes,uint32_t size,uint64_t epoch,void*) noexcept {
 if(!RadialPingEnabled()||!epoch||!bytes||size<41||size>41+2*MaxName||!ClntObjMgrGetCurrent()||!ClntObjMgrGetActivePlayer())return;
 try{Reader reader{{bytes,size}};Ping ping;
  if(!reader.Read(ping.type)||ping.type>3||!reader.Read(ping.x)||!reader.Read(ping.y)||!reader.Read(ping.z)||
     !reader.Read(ping.guidLow)||!reader.Read(ping.guidHigh)||!reader.Name(ping.unit,ping.unitLength)||
     !reader.Name(ping.sender,ping.senderLength)||reader.cursor!=size||!FinitePosition(ping.x,ping.y,ping.z))return;
  const auto currentMap=ClntObjMgrGetMapID();
  std::lock_guard lock(mutex);if(session!=epoch||mapId!=currentMap){pending.clear();session=epoch;mapId=currentMap;}
  if(pending.size()==MaxQueued)pending.pop_front();pending.push_back(ping);changed=true;
 }catch(...){Reset(nullptr);}
}
uint64_t Current(lua_State* L){return RadialPingEnabled()?protocol::SessionGeneration(L):0;}
bool ReadyWorld(lua_State* L){return Current(L)&&ClntObjMgrGetCurrent()&&bool(ClntObjMgrGetActivePlayer());}
bool Integer(lua_State* L,int index,double maximum,uint32_t& value){const double number=lua_tonumber(L,index);if(lua_type(L,index)!=LUA_TNUMBER||!std::isfinite(number)||number<0||number>maximum||std::floor(number)!=number)return false;value=uint32_t(number);return true;}
bool Point(lua_State* L,int first,C3Vector& p){
 for(int i=0;i<3;++i)if(lua_type(L,first+i)!=LUA_TNUMBER)return false;
 const auto x=lua_tonumber(L,first),y=lua_tonumber(L,first+1),z=lua_tonumber(L,first+2);if(!FinitePosition(x,y,z))return false;p={float(x),float(y),float(z)};return true;
}
int Pick(lua_State* L){
 auto* frame=CGWorldFrame::s_currentWorldFrame;HITTESTRESULT hit{};bool picked=false;
 if(ReadyWorld(L)&&frame&&frame->m_camera&&frame->m_simpleTop){
  float x=0,y=0;NDCToDDC(frame->m_simpleTop->m_mousePosition.m_x,frame->m_simpleTop->m_mousePosition.m_y,&x,&y);
  picked=std::isfinite(x)&&std::isfinite(y)&&frame->HitTestPoint(x,y,0,&hit)!=0&&FinitePosition(hit.m_point.x,hit.m_point.y,hit.m_point.z);
 }
 if(!picked){lua_pushnil(L);return 1;}
 // Only resolved unit/player GUIDs can attach. Terrain/objects remain ground pings.
 auto guid=hit.m_guid;
 if(!guid||!ClntObjMgrObjectPtr(guid,TypeMask(TYPEMASK_UNIT|TYPEMASK_PLAYER),__FILE__,__LINE__))guid={};
 lua_pushnumber(L,hit.m_point.x);lua_pushnumber(L,hit.m_point.y);lua_pushnumber(L,hit.m_point.z);
 lua_pushnumber(L,uint32_t(guid.m_guid));lua_pushnumber(L,uint32_t(guid.m_guid>>32));return 5;
}
int UnitPosition(lua_State* L){
 uint32_t lo=0,hi=0;C3Vector position{};bool found=false;
 if(ReadyWorld(L)&&Integer(L,1,UINT32_MAX,lo)&&Integer(L,2,UINT32_MAX,hi)){
  WOWGUID guid{uint64_t(lo)|(uint64_t(hi)<<32)};
  auto* object=ClntObjMgrObjectPtr(guid,TypeMask(TYPEMASK_UNIT|TYPEMASK_PLAYER),__FILE__,__LINE__);
  if(object){const auto base=object->GetPosition();const auto name=object->GetNamePosition();const float dx=name.x-base.x,dy=name.y-base.y,dz=name.z-base.z;
   position=FinitePosition(name.x,name.y,name.z)&&dx*dx+dy*dy<4&&dz>0.1f&&dz<100?name:base;
   found=FinitePosition(position.x,position.y,position.z);
  }
 }
 if(!found){lua_pushnil(L);return 1;}lua_pushnumber(L,position.x);lua_pushnumber(L,position.y);lua_pushnumber(L,position.z);return 3;
}
int Project(lua_State* L){
 C3Vector position{},screen{};auto* frame=CGWorldFrame::s_currentWorldFrame;
 if(!ReadyWorld(L)||!Point(L,1,position)||!frame||!frame->m_camera){lua_pushnil(L);return 1;}
 uint32_t clip=0;const bool visible=frame->GetScreenCoordinates(position,&screen,&clip);
 const float width=NDCToDDCWidth(1),height=NDCToDDCHeight(1);
 // Near-clipped projections leave screen.z zero in the native implementation.
 if(!(screen.z>0)||!FinitePosition(screen.x,screen.y,screen.z)||!(width>0)||!(height>0)){lua_pushnil(L);return 1;}
 lua_pushnumber(L,screen.x);lua_pushnumber(L,screen.y);lua_pushboolean(L,visible);lua_pushnumber(L,width);lua_pushnumber(L,height);return 5;
}
template<class T>void Append(std::vector<uint8_t>& out,const T& value){const auto* p=reinterpret_cast<const uint8_t*>(&value);out.insert(out.end(),p,p+sizeof(T));}
bool SendPing(lua_State* L){
 if(!Current(L))return false;uint32_t type=0,lo=0,hi=0;C3Vector check{};
 if(!Integer(L,1,3,type)||!Point(L,2,check)||!Integer(L,5,UINT32_MAX,lo)||!Integer(L,6,UINT32_MAX,hi))return false;
 size_t length=0;const char* name=lua_type(L,7)==LUA_TSTRING?lua_tolstring(L,7,&length):nullptr;
 length=std::min(length,MaxName);std::array<char,MaxName> clean{};
 for(size_t i=0;i<length;++i){const unsigned char c=static_cast<unsigned char>(name[i]);clean[i]=(c<0x20||c==':')?' ':char(c);}
 std::vector<uint8_t> wire;wire.reserve(37+length);Append(wire,uint8_t(type));
 // Retain wire double precision rather than serializing the projection float copy.
 for(int i=2;i<=4;++i)Append(wire,double(lua_tonumber(L,i)));Append(wire,lo);Append(wire,hi);Append(wire,uint32_t(length));wire.insert(wire.end(),clean.data(),clean.data()+length);
 return protocol::Send(Request,wire.data(),uint32_t(wire.size()));
}
int Send(lua_State* L){bool sent=false;try{sent=SendPing(L);}catch(...){}lua_pushboolean(L,sent);return 1;}
bool Pop(lua_State* L,Ping& ping){const auto epoch=ReadyWorld(L)?Current(L):0;const auto currentMap=epoch?ClntObjMgrGetMapID():0;
 std::lock_guard lock(mutex);if(!epoch||epoch!=session||currentMap!=mapId){pending.clear();session=epoch;mapId=currentMap;changed=false;return false;}if(pending.empty())return false;ping=pending.front();pending.pop_front();return true;}
int PopLua(lua_State* L){Ping ping;bool present=false;try{present=Pop(L,ping);}catch(...){}if(!present)return 0;
 lua_pushnumber(L,ping.type);lua_pushnumber(L,ping.x);lua_pushnumber(L,ping.y);lua_pushnumber(L,ping.z);lua_pushnumber(L,ping.guidLow);lua_pushnumber(L,ping.guidHigh);lua_pushlstring(L,ping.unit.data(),ping.unitLength);lua_pushlstring(L,ping.sender.data(),ping.senderLength);return 8;
}
int Pump(lua_State* L){const auto epoch=ReadyWorld(L)?Current(L):0;const auto currentMap=epoch?ClntObjMgrGetMapID():0;bool notify=false;
 {std::lock_guard lock(mutex);if(!epoch||epoch!=session||currentMap!=mapId){pending.clear();session=epoch;mapId=currentMap;changed=false;}else{notify=changed;changed=false;}}
 // Owner/UI pump runs after capture and every lock is released before Lua.
 if(notify)FrameScript_Execute("if wxlwow and wxlwow.radial_ping and wxlwow.radial_ping._NativeChanged then wxlwow.radial_ping._NativeChanged() end","native-radial-ping-update",nullptr);
 return 0;
}
}
bool Install(){
 if(!installed){const protocol::NativeDeclaration entries[]={{Request,true},{Response,false,Receive,Reset}};installed=protocol::RegisterNativeDispatch(entries,2);}
 if(!installed)return false;
 FrameScript_RegisterFunction("_WXLWOW_RADIAL_PICK_CURSOR",Pick);FrameScript_RegisterFunction("_WXLWOW_RADIAL_WORLD_TO_SCREEN",Project);
 FrameScript_RegisterFunction("_WXLWOW_RADIAL_UNIT_POSITION",UnitPosition);FrameScript_RegisterFunction("_WXLWOW_RADIAL_SEND_SERVER",Send);
 FrameScript_RegisterFunction("_WXLWOW_RADIAL_POP_SERVER",PopLua);FrameScript_RegisterFunction("WXL_NativeRadialPump",Pump);
 FrameScript_Execute(Bootstrap,"native-radial-ping-api",nullptr);return true;
}
void UiReady(){FrameScript_Execute("if CreateFrame then if not WXLNativeRadialDriver then WXLNativeRadialDriver=CreateFrame('Frame') end; WXLNativeRadialDriver:SetScript('OnUpdate',function() WXL_NativeRadialPump() end) end","native-radial-ping-pump",nullptr);}
void Destroy() noexcept {Reset(nullptr);}
}
