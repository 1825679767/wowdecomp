// Native single-owner paragon request/state bridge. Wire schema retained.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ClientExtensions.hpp"
#include "WXL/NativeProtocol.hpp"
#include "WXL/NativeFeatures.hpp"
#include "Engine/Source/FrameScript/FrameScript.h"
#include <array>
#include <cstring>
#include <deque>
#include <mutex>
#include <string>
namespace wxl::native::client_extensions {
namespace {
constexpr uint16_t Request=0x554,State=0x555;
struct Line{std::string bytes;uint64_t generation;};
std::mutex queueMutex;std::deque<Line> queue;bool registered=false;
void Reset(void*)noexcept{try{std::lock_guard lock(queueMutex);queue.clear();}catch(...){}}
void StatePacket(uint16_t,const uint8_t* data,uint32_t size,uint64_t generation,void*)noexcept{
 if(!ClientExtensionsEnabled()||!data||!size||size>8192)return;
 for(uint32_t i=0;i<size;++i)if((data[i]<0x20&&data[i]!='\n')||data[i]>0x7e)return;
 size_t begin=0;for(size_t i=0;i<=size;++i){if(i!=size&&data[i]!='\n')continue;if(i==begin||i-begin>512)return;begin=i+1;}
 try{std::lock_guard lock(queueMutex);begin=0;
  for(size_t i=0;i<=size;++i){if(i!=size&&data[i]!='\n')continue;if(queue.size()==256)queue.pop_front();
   queue.push_back({std::string(reinterpret_cast<const char*>(data+begin),i-begin),generation});begin=i+1;}
 }catch(...){Reset(nullptr);}
}
int Send(lua_State* L){size_t n=0;const char* text=lua_type(L,1)==LUA_TSTRING?lua_tolstring(L,1,&n):nullptr;
 bool valid=ClientExtensionsEnabled()&&registered&&protocol::SessionGeneration(L)&&text&&n&&n<=64;
 if(valid)for(size_t i=0;i<n;++i)if(static_cast<unsigned char>(text[i])<0x20||static_cast<unsigned char>(text[i])>0x7e){valid=false;break;}
 const bool sent=valid&&protocol::Send(Request,text,static_cast<uint32_t>(n));lua_pushboolean(L,sent);return 1;
}
bool CopyLine(uint64_t generation,char* bytes,size_t& length){
 std::lock_guard lock(queueMutex);
 while(!queue.empty()&&queue.front().generation!=generation)queue.pop_front();
 if(queue.empty())return false;
 length=queue.front().bytes.size();std::memcpy(bytes,queue.front().bytes.data(),length);queue.pop_front();return true;
}
int Pop(lua_State* L){char line[512];size_t n=0;const auto generation=protocol::SessionGeneration(L);
 if(generation&&CopyLine(generation,line,n))lua_pushlstring(L,line,n);else lua_pushnil(L);return 1;}
int Clear(lua_State* L){if(protocol::SessionGeneration(L))Reset(nullptr);return 0;}
}
bool InstallParagon(){
 if(!registered){const protocol::NativeDeclaration entries[]={{Request,true},{State,false,&StatePacket,&Reset,nullptr}};
  registered=protocol::RegisterNativeDispatch(entries,2);}
 if(!registered)return false;
 FrameScript_RegisterFunction("_WXLWOW_PARAGON_SEND",&Send);FrameScript_RegisterFunction("_WXLWOW_PARAGON_POP",&Pop);
 FrameScript_RegisterFunction("_WXLWOW_PARAGON_CLEAR",&Clear);return true;
}
void DestroyParagon()noexcept{Reset(nullptr);}
}
