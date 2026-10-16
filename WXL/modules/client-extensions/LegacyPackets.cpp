// Typed native Lua5.1 legacy packet facade. No addresses or duplicate transport.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ClientExtensions.hpp"
#include "LegacyPacketsLua.hpp"
#include "WXL/NativeProtocol.hpp"
#include "WXL/NativeFeatures.hpp"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Net/Types.h"
#include <windows.h>
#include <bit>
#include <cmath>
#include <cstring>
#include <limits>
#include <map>
#include <type_traits>
#include <vector>
namespace wxl::native::client_extensions {
namespace {
static_assert(std::endian::native==std::endian::little);
constexpr size_t MaxWriters=128,Quota=8*1024*1024;
constexpr uint32_t IdleMs=300000;
constexpr double MaxExact=9007199254740991.0;
struct Writer {uint16_t opcode{};std::vector<uint8_t> bytes;size_t cursor{};DWORD touched{};bool failed{};};
std::map<uint32_t,Writer> writers;
uint32_t nextId=1;
uint64_t currentSession=0;
size_t allocated=0;
thread_local char outgoing[protocol::MaxPayload];
// Operation IDs are the donor's public _WXL_NETWORK contract;29 is validity.
enum Op {WS,RS,WU8,WI8,WU16,WI16,WU32,WI32,WU64,WI64,WF,WD,WSTR,
 RU8,RI8,RU16,RI16,RU32,RI32,RU64,RI64,RF,RD,RSTR,MAKE,SEND,RESET,OBSERVE,GENERATION,VALID};
bool Integer(lua_State* L,int index,double lo,double hi,double& number){
 if(lua_type(L,index)!=LUA_TNUMBER)return false;
 number=lua_tonumber(L,index);return std::isfinite(number)&&number>=lo&&number<=hi&&std::floor(number)==number;
}
uint64_t Sync(lua_State* L){
 const auto session=ClientExtensionsEnabled()?protocol::SessionGeneration(L):0;
 if(session!=currentSession){writers.clear();allocated=0;currentSession=session;}
 const DWORD now=GetTickCount();
 for(auto it=writers.begin();it!=writers.end();){
  if(now-it->second.touched>=IdleMs){allocated-=it->second.bytes.size();it=writers.erase(it);}else ++it;
 }
 return session;
}
Writer* Find(lua_State* L){
 double id;if(!Integer(L,2,1,UINT32_MAX,id))return nullptr;
 const auto it=writers.find(static_cast<uint32_t>(id));
 if(it==writers.end())return nullptr;it->second.touched=GetTickCount();return &it->second;
}
bool Append(Writer& w,const void* bytes,size_t n){
 if(w.failed || w.cursor>protocol::MaxPayload || n>protocol::MaxPayload-w.cursor){w.failed=true;return false;}
 const auto required=w.cursor+n;
 const auto growth=required>w.bytes.size()?required-w.bytes.size():0;
 if(growth>Quota-allocated){w.failed=true;return false;}
 try{if(growth){w.bytes.resize(required);allocated+=growth;}if(n)std::memcpy(w.bytes.data()+w.cursor,bytes,n);w.cursor=required;return true;}
 catch(...){w.failed=true;return false;}
}
template<class T>bool WriteNumber(lua_State* L,Writer& w){
 if(lua_type(L,3)!=LUA_TNUMBER){w.failed=true;return false;}
 const double n=lua_tonumber(L,3);
 bool valid=std::isfinite(n);
 if constexpr(std::is_integral_v<T>){
  const double lo=sizeof(T)==8?(std::is_signed_v<T>?-MaxExact:0):double(std::numeric_limits<T>::lowest());
  const double hi=sizeof(T)==8?MaxExact:double(std::numeric_limits<T>::max());
  valid=valid&&n>=lo&&n<=hi&&std::floor(n)==n;
 }else valid=valid&&n>=-double(std::numeric_limits<T>::max())&&n<=double(std::numeric_limits<T>::max());
 if(!valid){w.failed=true;return false;}const T value=static_cast<T>(n);return Append(w,&value,sizeof(value));
}
uint32_t Make(lua_State* L){
 double opcode,size;
 if(!Integer(L,2,NUM_MSG_TYPES,65535,opcode)||!Integer(L,3,0,protocol::MaxPayload,size)||writers.size()>=MaxWriters||nextId==0)return 0;
 if(!protocol::CanLegacySend(static_cast<uint16_t>(opcode)) || size>double(Quota-allocated))return 0;
 try{Writer w;w.opcode=static_cast<uint16_t>(opcode);w.bytes.resize(static_cast<size_t>(size));w.touched=GetTickCount();
  const auto id=nextId;writers.emplace(id,std::move(w));++nextId;allocated+=static_cast<size_t>(size);return id;
 }catch(...){return 0;}
}
int Failure(lua_State* L,size_t cursor){lua_pushnil(L);lua_pushnumber(L,double(cursor));return 2;}
template<class T>int ReadNumber(lua_State* L,const char* data,size_t length,size_t cursor){
 if(cursor>length||sizeof(T)>length-cursor)return Failure(L,cursor);
 T value;std::memcpy(&value,data+cursor,sizeof(value));
 const double n=double(value);
 if(!std::isfinite(n))return Failure(L,cursor);
 if constexpr(std::is_integral_v<T> && sizeof(T)==8){if(n < -MaxExact || n > MaxExact)return Failure(L,cursor);}
 lua_pushnumber(L,n);lua_pushnumber(L,double(cursor+sizeof(T)));return 2;
}
int Read(lua_State* L,int operation,uint64_t session){
 size_t length=0;const char* data=lua_type(L,2)==LUA_TSTRING?lua_tolstring(L,2,&length):nullptr;
 double cursorValue,token;
 if(!data||length>protocol::MaxPayload||!Integer(L,3,0,double(length),cursorValue)||
    !Integer(L,4,1,MaxExact,token)||token!=double(session))return Failure(L,0);
 const auto cursor=static_cast<size_t>(cursorValue);
 if(operation==RS){lua_pushnumber(L,double(length));return 1;}
 switch(operation){
 case RU8:return ReadNumber<uint8_t>(L,data,length,cursor);case RI8:return ReadNumber<int8_t>(L,data,length,cursor);
 case RU16:return ReadNumber<uint16_t>(L,data,length,cursor);case RI16:return ReadNumber<int16_t>(L,data,length,cursor);
 case RU32:return ReadNumber<uint32_t>(L,data,length,cursor);case RI32:return ReadNumber<int32_t>(L,data,length,cursor);
 case RU64:return ReadNumber<uint64_t>(L,data,length,cursor);case RI64:return ReadNumber<int64_t>(L,data,length,cursor);
 case RF:return ReadNumber<float>(L,data,length,cursor);case RD:return ReadNumber<double>(L,data,length,cursor);
 case RSTR:{
  if(length-cursor<4)return Failure(L,cursor);uint32_t n;std::memcpy(&n,data+cursor,4);
  if(n>length-cursor-4)return Failure(L,cursor);
  lua_pushlstring(L,data+cursor+4,n);lua_pushnumber(L,double(cursor+4+n));return 2;
 }
 default:return Failure(L,cursor);
 }
}
int Network(lua_State* L){
 const auto session=Sync(L);double operation;
 if(!Integer(L,1,0,VALID,operation)){lua_pushnil(L);return 1;}
 const int op=static_cast<int>(operation);
 if(op==GENERATION){lua_pushnumber(L,double(session));return 1;}
 if(!session){lua_pushnil(L);return 1;}
 if(op==OBSERVE){double opcode;const bool ok=Integer(L,2,NUM_MSG_TYPES,65535,opcode)&&protocol::CanLegacyObserve(static_cast<uint16_t>(opcode));lua_pushboolean(L,ok);return 1;}
 if(op==MAKE){const auto id=Make(L);if(id)lua_pushnumber(L,id);else lua_pushnil(L);return 1;}
 if(op==RS || (op>=RU8&&op<=RSTR))return Read(L,op,session);
 auto* w=Find(L);
 if(op==WS){lua_pushnumber(L,w?double(w->bytes.size()):0);return 1;}
 if(op==VALID){lua_pushboolean(L,w&&!w->failed);return 1;}
 if(!w){lua_pushnil(L);return 1;}
 if(op==SEND){
  // Release owning state before Lua allocations. Lua bootstrap calls the actual
  // WXL_NativeSendCustom entry point, retaining its native-owner/send gates.
  const auto id=static_cast<uint32_t>(lua_tonumber(L,2));const auto opcode=w->opcode;
  const auto n=w->bytes.size();const bool valid=!w->failed;
  if(valid&&n)std::memcpy(outgoing,w->bytes.data(),n);
  allocated-=n;writers.erase(id);
  if(!valid){lua_pushnil(L);return 1;}
  lua_pushinteger(L,opcode);lua_pushlstring(L,outgoing,n);return 2;
 }
 bool ok=false;
 switch(op){
 case WU8:ok=WriteNumber<uint8_t>(L,*w);break;case WI8:ok=WriteNumber<int8_t>(L,*w);break;
 case WU16:ok=WriteNumber<uint16_t>(L,*w);break;case WI16:ok=WriteNumber<int16_t>(L,*w);break;
 case WU32:ok=WriteNumber<uint32_t>(L,*w);break;case WI32:ok=WriteNumber<int32_t>(L,*w);break;
 case WU64:ok=WriteNumber<uint64_t>(L,*w);break;case WI64:ok=WriteNumber<int64_t>(L,*w);break;
 case WF:ok=WriteNumber<float>(L,*w);break;case WD:ok=WriteNumber<double>(L,*w);break;
 case WSTR:{size_t n=0;const char* data=lua_type(L,3)==LUA_TSTRING?lua_tolstring(L,3,&n):nullptr;
  if(!data||n>protocol::MaxPayload-4||w->cursor>protocol::MaxPayload-4-n){w->failed=true;break;}
  const uint32_t length=static_cast<uint32_t>(n);ok=Append(*w,&length,4)&&Append(*w,data,n);break;}
 default:w->failed=true;break;
 }
 lua_pushboolean(L,ok);return 1;
}
}
void InstallPackets(){DestroyPackets();
 // Existing source-owned custom IDs mirrored by the original WXL facade.
 // Builtin spell charges and compiled appearance/paragon owners stay exclusive.
 constexpr uint16_t outgoingIds[]={0x51f,0x521,0x523,0x527,0x529,0x52b,0x52d,0x531,0x533,0x538,0x539,0x53a};
 constexpr uint16_t incomingIds[]={0x520,0x522,0x524,0x528,0x52a,0x52c,0x52e,0x52f,0x530,0x532,0x534,0x535,0x536,0x537,0x53b};
 for(auto id:outgoingIds)protocol::RegisterOpcode(id,true);
 for(auto id:incomingIds)protocol::RegisterOpcode(id,false);
FrameScript_RegisterFunction("_WXL_NETWORK",&Network);FrameScript_Execute(kLegacyPacketsLua,"WXLNativeLegacyPackets",nullptr);}
void StartPacketPump(){FrameScript_Execute("if WXL_StartNativePacketPump then WXL_StartNativePacketPump() end","WXLNativeLegacyPacketPump",nullptr);}
void DestroyPackets()noexcept{writers.clear();allocated=0;currentSession=0;}
}
