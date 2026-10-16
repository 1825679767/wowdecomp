// Explicit donor-compatible procedural payload subset, never a retail-ID alias.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "WXL/ProceduralEnchants.hpp"
#include "WXL/EquipmentAttachments.hpp"
#include "WXL/ItemGraph.hpp"
#include "WXL/ProceduralPayload.hpp"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include <array>
#include <algorithm>
#include <bit>
#include <cmath>
#include <memory>
#include <vector>
#ifndef WXL_ENABLE_PROCEDURAL_ENCHANTS
#define WXL_ENABLE_PROCEDURAL_ENCHANTS 1
#endif
namespace wxl::native {
struct ProceduralEnchantState {
 CM2Model* model=nullptr;CM2Scene* scene=nullptr;uint32_t handle=0,startTime=0;
 ProceduralEnchantState* previous=nullptr;ProceduralEnchantState* next=nullptr;
 std::array<float,4> baseline{},last{},wanted{};std::array<bool,4> owned{};
 // Retain exact modern link/procedure/source-selector payload ownership.
 std::vector<ItemGraphProcedural> sources;
};
namespace procedural_enchants {namespace {
CVar* enabled=nullptr;
constexpr uint32_t MaxOwners=1024,MaxProcedures=256;
std::array<float*,4> Fields(CM2Model& m){return {&m.m_diffuseTintR,&m.m_diffuseTintG,&m.m_diffuseTintB,&m.m_fadeAlpha};}
bool SameSource(const ItemGraphProcedural& a,const ItemGraphProcedural& b){
 if(a.kitId!=b.kitId||a.kitEffectId!=b.kitEffectId||a.procedureId!=b.procedureId||a.type!=b.type||a.displayWeaponSubclassId!=b.displayWeaponSubclassId||a.sourceEffectId!=b.sourceEffectId)return false;
 for(uint32_t i=0;i<4;++i)if(a.rawValues[i]!=b.rawValues[i])return false;return true;
}
void Write(ProceduralEnchantState& s){
 const auto fields=Fields(*s.model);
 for(uint32_t i=0;i<4;++i)if(s.owned[i]&&*fields[i]!=s.last[i])s.baseline[i]=*fields[i];
 s.wanted=s.baseline;const auto elapsed=uint32_t(s.scene->m_time-s.startTime);
 for(const auto& row:s.sources){std::array<float,4> value;if(!DecodeProceduralPayload(row,value))continue;
  if(row.type==1||row.type==13){
   const auto rgb=uint32_t(value[0]);const float color[]={float((rgb>>16)&255)/255,float((rgb>>8)&255)/255,float(rgb&255)/255};float factor=1;
   if(row.type==13){
    const auto delay=uint32_t(double(value[1])*1000.0),duration=uint32_t(double(value[2])*1000.0);
    if(elapsed>=uint64_t(delay)+duration)continue;
    // Native GetFlashColor holds full flash until delay, then quantizes its fade.
    if(elapsed>=delay){const auto level=int((1.0-double(elapsed-delay)/duration)*255.0);factor=float(std::clamp(level,0,255))/255.0f;}
   }
   for(uint32_t i=0;i<3;++i)s.wanted[i]+=(color[i]-s.wanted[i])*factor;
  }else if(row.type==14){
   const auto fade=uint32_t(double(value[2])*1000.0);
   const auto factor=fade?std::min(1.0,double(elapsed)/fade):1.0;
   s.wanted[3]=float(s.wanted[3]+(value[0]-s.wanted[3])*factor);
  }
 }
 for(uint32_t i=0;i<4;++i)if(s.owned[i]){*fields[i]=s.wanted[i];s.last[i]=s.wanted[i];}
}
}
void Register(){enabled=CVar::Register("wxlProceduralEnchants","WXL bounded legacy-compatible retail enchant tint/alpha payloads",2,WXL_ENABLE_PROCEDURAL_ENCHANTS?"1":"0",nullptr,0,false,nullptr,false);}
bool Enabled() noexcept{return WXL_ENABLE_PROCEDURAL_ENCHANTS&&equipment_attachments::Enabled()&&enabled&&enabled->m_intValue;}
void Release(CM2Model* model) noexcept{try{
 if(!model||!model->m_wxlProceduralEnchant)return;
 auto* state=model->m_wxlProceduralEnchant;model->m_wxlProceduralEnchant=nullptr;
 if(state->model==model&&state->handle==model->m_handle){const auto fields=Fields(*model);for(uint32_t i=0;i<4;++i)if(state->owned[i]&&*fields[i]==state->last[i])*fields[i]=state->baseline[i];}
 if(state->previous)state->previous->next=state->next;else if(state->scene&&state->scene->m_wxlProceduralEnchantHead==state)state->scene->m_wxlProceduralEnchantHead=state->next;
 if(state->next)state->next->previous=state->previous;
 if(state->scene&&state->scene->m_wxlProceduralEnchantCount)--state->scene->m_wxlProceduralEnchantCount;
 delete state;
 }catch(...){}}
void Apply(CM2Model* model,const ItemGraphVisual& visual,uint32_t subclass) noexcept{try{
 if(!Enabled()||!model||!model->m_scene||visual.procedurals.size()>MaxProcedures){Release(model);return;}
 std::vector<ItemGraphProcedural> selected;selected.reserve(visual.procedurals.size());
 for(const auto& row:visual.procedurals){if(row.displayWeaponSubclassId!=UINT32_MAX&&row.displayWeaponSubclassId!=subclass)continue;std::array<float,4> value;if(DecodeProceduralPayload(row,value))selected.push_back(row);}
 if(const auto* old=model->m_wxlProceduralEnchant;old&&old->scene==model->m_scene&&old->handle==model->m_handle&&old->sources.size()==selected.size()&&std::equal(old->sources.begin(),old->sources.end(),selected.begin(),SameSource))return;
 Release(model);if(model->m_scene->m_wxlProceduralEnchantCount>=MaxOwners)return;
 auto next=std::make_unique<ProceduralEnchantState>();next->model=model;next->handle=model->m_handle;next->scene=model->m_scene;next->startTime=model->m_scene->m_time;next->sources=std::move(selected);
 const auto fields=Fields(*model);for(uint32_t i=0;i<4;++i)next->baseline[i]=next->last[i]=next->wanted[i]=*fields[i];
 for(const auto& row:next->sources){if(row.type==1||row.type==13)next->owned[0]=next->owned[1]=next->owned[2]=true;else if(row.type==14)next->owned[3]=true;}

 if(next->sources.empty())return;
 auto* scene=next->scene;next->next=scene->m_wxlProceduralEnchantHead;if(next->next)next->next->previous=next.get();scene->m_wxlProceduralEnchantHead=next.get();++scene->m_wxlProceduralEnchantCount;
 model->m_wxlProceduralEnchant=next.release();Write(*model->m_wxlProceduralEnchant);
 }catch(...){Release(model);}}
void Clone(const CM2Model& source,CM2Model& destination) noexcept {try{
 const auto* state=source.m_wxlProceduralEnchant;if(!state)return;ItemGraphVisual visual;visual.procedurals=state->sources;
 // Stored rows have already passed the source subclass selector.
 for(auto& row:visual.procedurals)row.displayWeaponSubclassId=UINT32_MAX;Apply(&destination,visual,UINT32_MAX);
 if(auto* copied=destination.m_wxlProceduralEnchant;copied&&source.m_scene){copied->startTime=destination.m_scene->m_time-uint32_t(source.m_scene->m_time-state->startTime);Write(*copied);}
 }catch(...){Release(&destination);}}
void TickScene(CM2Scene* scene) noexcept{try{
 if(!scene)return;auto* state=scene->m_wxlProceduralEnchantHead;uint32_t count=0;
 while(state&&count++<MaxOwners){auto* next=state->next;auto* model=state->model;
  if(!Enabled()||!model||model->m_wxlProceduralEnchant!=state||model->m_handle!=state->handle||model->m_scene!=scene)Release(model);else Write(*state);
  state=next;
 }
 }catch(...){}}
void ReleaseScene(CM2Scene* scene) noexcept{if(!scene)return;uint32_t count=0;while(scene->m_wxlProceduralEnchantHead&&count++<MaxOwners)Release(scene->m_wxlProceduralEnchantHead->model);}
}
}
