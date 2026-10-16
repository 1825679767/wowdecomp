// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ModuleApi.hpp"
#include "NativeFeatures.hpp"
#include "CreationEffects.hpp"
#include "Engine/Source/FrameScript/FrameScript.h"
namespace wxl::native {
namespace {
struct Entry {const char* id;const char* dependencies;bool(*enabled)() noexcept;};
const Entry entries[]{
 {"runtime","",&NativeEnabled},{"storage","runtime",&NativeEnabled},
 {"db2","runtime,storage",&Db2Enabled},{"blp","runtime",&BlpEnabled},
 {"materials","runtime,m2",&MaterialsEnabled},{"particles","runtime,m2",&ParticlesEnabled},
 {"m2","runtime,storage;db2 optional for FDID",&AssetsEnabled},
 {"appearances","m2,db2,blp",&AppearancesEnabled},{"protocol","runtime",&ProtocolEnabled},
 {"appearance-session","appearances,protocol",&AppearanceSaveEnabled},
 {"creation-effects","runtime,appearances,storage",&creation_effects::Enabled},
 {"quest-marker","runtime,protocol,client-extensions",&QuestMarkerEnabled},
 {"radial-ping","runtime,protocol,client-extensions",&RadialPingEnabled},{"client-extensions","runtime,protocol",&ClientExtensionsEnabled},{"world-wmo","storage,db2",&WmoEnabled},{"world-adt","storage,db2",&SplitAdtEnabled}};
int LuaModules(lua_State* L){
 lua_createtable(L,int(ModuleCount()),0);
 for(uint32_t i=0;i<ModuleCount();++i){
  ModuleInfo info{};if(!QueryModule(i,info))continue;
  lua_createtable(L,0,4);
  lua_pushstring(L,info.id);lua_setfield(L,-2,"id");
  lua_pushinteger(L,info.apiVersion);lua_setfield(L,-2,"apiVersion");
  lua_pushstring(L,info.dependencies);lua_setfield(L,-2,"dependencies");
  lua_pushboolean(L,info.enabled);lua_setfield(L,-2,"enabled");
  lua_rawseti(L,-2,int(i+1));
 }
 return 1;
}
}
uint32_t ModuleCount() noexcept {return uint32_t(sizeof(entries)/sizeof(entries[0]));}
bool QueryModule(uint32_t index,ModuleInfo& output) noexcept {
 if(index>=ModuleCount())return false;
 const auto& e=entries[index];output={e.id,1,e.dependencies,e.enabled()};return true;
}
void InitializeModuleLua(){FrameScript_RegisterFunction("WXL_GetNativeModules",&LuaModules);}
}
