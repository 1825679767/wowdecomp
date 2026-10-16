// Native addon diagnostics, command-line arguments, mounted text and descriptions.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ClientExtensions.hpp"
#include "WXL/NativeFeatures.hpp"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Ui/QuestTextParser.h"
#include "common/Log.hpp"
#include <windows.h>
#include <shellapi.h>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>
namespace wxl::native::client_extensions {
namespace {
constexpr size_t MaxText=8*1024*1024;
std::vector<std::string> arguments;
thread_local char textBuffer[MaxText+1];
void ParseArguments(){
 arguments.clear();int n=0;auto** args=CommandLineToArgvW(GetCommandLineW(),&n);if(!args)return;
 try{for(int i=0;i<n;++i){const int bytes=WideCharToMultiByte(CP_UTF8,0,args[i],-1,nullptr,0,nullptr,nullptr);
  if(bytes<=0)continue;std::string value(static_cast<size_t>(bytes),'\0');
  WideCharToMultiByte(CP_UTF8,0,args[i],-1,value.data(),bytes,nullptr,nullptr);value.resize(static_cast<size_t>(bytes-1));arguments.push_back(std::move(value));}}
 catch(...){arguments.clear();}LocalFree(args);
}
const char* Argument(const char* name,size_t n){
 if(!name||!n||std::memchr(name,0,n))return nullptr;
 for(const auto& arg:arguments)if(arg.size()>=n&&std::memcmp(arg.data(),name,n)==0&&(arg.size()==n||arg[n]=='='))return arg.c_str()+n;
 return nullptr;
}
int Version(lua_State* L){lua_pushnumber(L,3);return 1;}
int HasArgument(lua_State* L){size_t n=0;const char* name=lua_type(L,1)==LUA_TSTRING?lua_tolstring(L,1,&n):nullptr;lua_pushboolean(L,Argument(name,n)!=nullptr);return 1;}
int GetArgument(lua_State* L){size_t n=0;const char* name=lua_type(L,1)==LUA_TSTRING?lua_tolstring(L,1,&n):nullptr;const char* value=Argument(name,n);
 if(value&&*value=='=')lua_pushstring(L,value+1);else if(lua_type(L,2)==LUA_TSTRING)lua_pushvalue(L,2);else lua_pushnil(L);return 1;}
int Log(lua_State* L){const auto level=lua_tointeger(L,1);size_t n=0;const char* text=lua_type(L,2)==LUA_TSTRING?lua_tolstring(L,2,&n):nullptr;
 if(text){const int count=static_cast<int>(n>65536?65536:n);
  if(level>=3)WLOG_ERROR("client-extensions: %.*s",count,text);else if(level==2)WLOG_WARN("client-extensions: %.*s",count,text);
  else if(level==0)WLOG_DEBUG("client-extensions: %.*s",count,text);else WLOG_INFO("client-extensions: %.*s",count,text);}
 return 0;
}
// Use mounted native SFile resolution; no absolute-address I/O or archive mutation.
bool ReadText(const char* path,size_t n,size_t& length){
 if(!path||!n||n>1023||std::memchr(path,0,n))return false;
 char owned[1024];std::memcpy(owned,path,n);owned[n]=0;
 SFileImpl* file=nullptr;if(!SFileOpenFile(owned,&file)||!file)return false;
 uint32_t high=0;const uint32_t bytes=SFileGetFileSize(file,&high);bool ok=false;
 if(!high&&bytes<=MaxText){uint32_t read=0;ok=!bytes||SFileReadFile(file,textBuffer,bytes,&read,nullptr,0)&&read==bytes;length=bytes;}
 SFileCloseFile(file);return ok;
}
int ReadFile(lua_State* L){size_t n=0,length=0;const char* path=lua_type(L,1)==LUA_TSTRING?lua_tolstring(L,1,&n):nullptr;
 if(ReadText(path,n,length))lua_pushlstring(L,textBuffer,length);else lua_pushnil(L);return 1;}
bool Describe(uint32_t id,char* buffer){SpellRec spell;if(!g_spellDB.GetRecord(id,&spell)||!spell.m_description||!*spell.m_description)return false;
 SpellParserParseText(&spell,buffer,1024,0,0,0,0,1,0);buffer[1023]=0;return true;}
int Description(lua_State* L){const double id=lua_tonumber(L,1);char text[1024]{};
 if(lua_type(L,1)==LUA_TNUMBER&&std::isfinite(id)&&id>=1&&id<=UINT32_MAX&&std::floor(id)==id&&Describe(static_cast<uint32_t>(id),text))lua_pushstring(L,text);else lua_pushnil(L);return 1;}
constexpr char Bootstrap[]=R"lua(
WXLClientExtensions=WXLClientExtensions or {}
WXLClientExtensions.version=GetWXLClientExtensionsVersion()
local function stringify(...)
 local output=""
 for i=1,select('#',...)do local value=select(i,...);local kind=type(value)
  if kind=='string' or kind=='number' or kind=='boolean'then output=output..tostring(value)else output=output..'['..kind..']'end
 end
 return output
end
function LOG_DEBUG(...)_WXL_CLIENT_LOG(0,stringify(...))end
function LOG_INFO(...)_WXL_CLIENT_LOG(1,stringify(...))end
function LOG_WARN(...)_WXL_CLIENT_LOG(2,stringify(...))end
function LOG_ERROR(...)_WXL_CLIENT_LOG(3,stringify(...))end
)lua";
}
void InstallHelpers(){ParseArguments();
 FrameScript_RegisterFunction("GetSpellDescription",&Description);FrameScript_RegisterFunction("GetWXLClientExtensionsVersion",&Version);
 FrameScript_RegisterFunction("_WXL_CLIENT_LOG",&Log);FrameScript_RegisterFunction("WXL_HasArgument",&HasArgument);
 FrameScript_RegisterFunction("WXL_GetArgument",&GetArgument);FrameScript_RegisterFunction("WXL_ReadClientFileText",&ReadFile);
 FrameScript_Execute(Bootstrap,"WXLNativeClientHelpers",nullptr);
}
void Install(){if(!ClientExtensionsEnabled())return;InstallHelpers();InstallPackets();InstallParagon();}
void UiReady(){if(ClientExtensionsEnabled())StartPacketPump();}
void Destroy()noexcept{DestroyPackets();DestroyParagon();arguments.clear();}
}
