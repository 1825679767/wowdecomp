// Native SpellChainEffects beam owner for the authored creation Priest channel.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "WXL/CreationBeams.hpp"
#include "WXL/CreationEffects.hpp"
#include "WXL/AlternateAppearances.hpp"
#include "WXL/NativeFeatures.hpp"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Common/Lightning.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/Glue/CharacterCreation.h"
#include "WoW/Source/Glue/CSimpleModelFFX.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#ifndef WXL_ENABLE_CREATION_BEAMS
#define WXL_ENABLE_CREATION_BEAMS 1
#endif
namespace wxl::native::creation_beams {namespace {
constexpr uint32_t MindFlay=15407,NoAttachment=0xffffffffu,MaxBolts=12,RetryMs=250;
// Exact native CGUnit_C effect attachment table. No unit pointer/layout access.
constexpr uint32_t EffectAttachments[]={20,34,19,21,22,17,23,24,25,15,16,37,38,22,47,48};
constexpr uint32_t CastEvent=0x4c534324;
CVar* enabled=nullptr;
uint32_t Now(){return OsGetAsyncTimeMs();}
bool Reached(uint32_t now,uint32_t deadline){return int32_t(now-deadline)>=0;}
bool Finite(const C3Vector& value){return std::isfinite(value.x)&&std::isfinite(value.y)&&std::isfinite(value.z);}
std::string Key(const char* raw){std::string path=raw?raw:"";for(auto& c:path){if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c-'A'+'a');}return path;}
bool ValidChain(const SpellChainEffectsRec& r){
 for(float value:{r.m_avgSegLen,r.m_width,r.m_noiseScale,r.m_texCoordScale,r.m_jointOffsetRadius,r.m_minorJointScale,r.m_majorJointScale,r.m_jointMoveSpeed,r.m_jointSmoothness,r.m_minDurationBetweenJointJumps,r.m_maxDurationBetweenJointJumps,r.m_waveHeight,r.m_waveFreq,r.m_waveSpeed,r.m_minWaveAngle,r.m_maxWaveAngle,r.m_minWaveSpin,r.m_maxWaveSpin,r.m_arcHeight,r.m_minArcAngle,r.m_maxArcAngle,r.m_minArcSpin,r.m_maxArcSpin,r.m_delayBetweenEffects,r.m_minFlickerOnDuration,r.m_maxFlickerOnDuration,r.m_minFlickerOffDuration,r.m_maxFlickerOffDuration,r.m_pulseSpeed,r.m_pulseOnLength,r.m_pulseFadeLength,r.m_textureLength,r.m_wavePhase})
  if(!std::isfinite(value)||std::fabs(value)>10000.0f)return false;
 if(r.m_width<=0||r.m_renderLayer<0||r.m_renderLayer>3||uint8_t(r.m_blendMode)>=GxBlends_Last||r.m_segDelay<0||r.m_segDelay>30000||r.m_segDuration<0||r.m_segDuration>30000)return false;
 if(r.m_flags&1){if(r.m_jointCount<0||r.m_jointCount>999)return false;}else if(r.m_avgSegLen<=0)return false;
 if((r.m_flags&2)&&(r.m_jointsPerMinorJoint<=0||r.m_minorJointsPerMajorJoint<=0||r.m_jointsPerMinorJoint>1000||r.m_minorJointsPerMajorJoint>1000))return false;
 if((r.m_flags&0x100)&&r.m_textureLength<=0)return false;
 if((r.m_flags&0x20)&&(r.m_pulseFadeLength<=0||r.m_pulseOnLength<0))return false;
 return r.m_texture&&*r.m_texture;
}
struct Chain {
 SpellChainEffectsRec row;std::string texturePath;CTexture* texture=nullptr;uint32_t index=0;bool persist=false;
 ~Chain(){if(texture)HandleClose(reinterpret_cast<HOBJECT>(texture));}
};
struct State {
 CM2Model* actor=nullptr;CM2Scene* scene=nullptr;uint64_t serial=0;uint32_t handle=0,race=0,sex=0,started=0,expires=0,lastTick=0,retryAt=0;
 uint32_t sourceAttachment=NoAttachment;bool raw=false;C3Vector sourceOffset;
 std::vector<std::unique_ptr<Chain>> chains;std::unique_ptr<CLightningManager> manager;
 ~State(){manager.reset();chains.clear();if(actor)actor->Release();}
};
std::unique_ptr<State> state;
bool Owner(const State& s){
 const auto* component=CCharacterCreation::m_character;
 return Enabled()&&creation_effects::Enabled()&&!alternate_appearances::IsAlternate()&&CCharacterCreation::m_existingCharacterIndex<0&&component&&component->m_wxlSerial==s.serial
  &&component->m_data.m_model==s.actor&&component->m_data.m_preferences.m_classId==5&&component->m_data.m_preferences.m_raceID==s.race&&component->m_data.m_preferences.m_sexID==s.sex
  &&s.actor->m_handle==s.handle&&s.actor->m_scene==s.scene&&s.actor->m_baseAlpha>0;
}
bool AddChain(State& s,const SpellChainEffectsRec& record,bool persist){
 if(s.chains.size()>=MaxBolts||!ValidChain(record))return false;
 auto chain=std::make_unique<Chain>();chain->row=record;chain->persist=persist;chain->texturePath=Key(record.m_texture);
 if(chain->texturePath.empty()||chain->texturePath.size()>=260||!SFile::FileExists(chain->texturePath.c_str()))return false;
 CStatus status;chain->texture=TextureCreate(chain->texturePath.c_str(),CGxTexFlags(GxTex_Linear,1,0,0,0,0,1),&status,0);
 if(!chain->texture||Key(chain->texture->m_filename)!=chain->texturePath||chain->texture->m_status.m_highestSeverity>=STATUS_ERROR)return false;
 // The manager borrows only this stable owned row; no DB row/string escapes.
 chain->row.m_texture=chain->texturePath.c_str();chain->row.m_combo="";
 s.chains.push_back(std::move(chain));return true;
}
bool Build(State& s){
 SpellRec spell;if(!g_spellDB.GetRecord(MindFlay,&spell))return false;
 const auto* visual=GetSpellVisual(&spell);const auto* kit=visual?g_spellVisualKitDB.GetRecord(visual->m_channelKit):nullptr;
 if(!visual||!kit)return false;
 s.sourceOffset=C3Vector(visual->m_missileCastOffset[0],-visual->m_missileCastOffset[1],visual->m_missileCastOffset[2]);
 if(!Finite(s.sourceOffset))return false;s.sourceAttachment=uint32_t(visual->m_missileAttachment);s.raw=(visual->m_flags&0x200)!=0;
 std::vector<std::unique_ptr<Chain>> previous;previous.swap(s.chains);
 for(uint32_t proc=0;proc<4;++proc){
  if(kit->m_charProc[proc]!=0&&kit->m_charProc[proc]!=12)continue;
  const auto id=kit->m_charParamZero[proc];if(!std::isfinite(id)||id<=0||double(id)>double(INT32_MAX)||std::trunc(id)!=id)continue;
  const auto* root=g_spellChainEffectsDB.GetRecord(int32_t(id));if(!root)continue;
  const bool persist=kit->m_charParamTwo[proc]!=0;
  if(!AddChain(s,*root,persist)){s.chains.clear();return false;}
  // Native CreateFromSpell walks up to twelve encoded combo words from the
  // root's NUL-terminated binary-safe string. Read only complete words.
  for(uint32_t offset=0;root->m_combo&&offset<48;offset+=4){
   uint32_t encoded=0;bool complete=true;for(uint32_t byte=0;byte<4;++byte){const auto value=uint8_t(root->m_combo[offset+byte]);if(!value){complete=false;break;}encoded|=uint32_t(value)<<(byte*8);}
   if(!complete)break;const auto next=CLightning::DecodeChainEffectsID(encoded);if(!next)break;
   if(s.chains.size()>=MaxBolts){s.chains.clear();return false;}
   const auto* record=g_spellChainEffectsDB.GetRecord(next);if(!record||!AddChain(s,*record,persist)){s.chains.clear();return false;}
  }
 }
 if(s.chains.empty())return false;
 s.manager=std::make_unique<CLightningManager>();
 for(auto& chain:s.chains){chain->index=s.manager->Add(nullptr,nullptr,&chain->row);auto* bolt=s.manager->GetTempPtr(chain->index);if(!bolt){s.manager.reset();s.chains.clear();return false;}bolt->SetTexture(chain->texture);s.manager->SetBoltVisible(chain->index,false);}
 return true;
}
bool Source(const State& s,C3Vector& output){
 auto* actor=s.actor;if(!actor->IsLoaded(0,0))return false;
 uint32_t attachment=s.sourceAttachment;
 if(!s.raw&&attachment!=NoAttachment){
  if(attachment>=sizeof(EffectAttachments)/sizeof(EffectAttachments[0]))return false;
  if(attachment==13&&actor->HasAttachment(EffectAttachments[4]))for(auto* model=actor->m_attachList;model;model=model->m_attachNext)if(model->m_attachmentId==1){
   for(int32_t point=4;point>=0;--point)if(model->HasAttachment(point)){output=s.sourceOffset*model->GetAttachmentWorldTransform(point);return Finite(output);}break;}
  attachment=EffectAttachments[attachment==13?4:attachment];
 }
 if(attachment!=NoAttachment&&actor->HasAttachment(attachment)){output=s.sourceOffset*actor->GetAttachmentWorldTransform(attachment);return Finite(output);}
 if(actor->HasEvent(CastEvent)){output=s.sourceOffset*actor->GetEventWorldTransform(CastEvent);return Finite(output);}
 // Glue has no server unit-height descriptor. Never fabricate a fallback
 // height when this model lacks its authored attachment and cast event.
 return false;
}
struct WorldScope {
 CGxMatrixStack& world;
 explicit WorldScope(CGxDevice& device):world(device.m_xforms[GxXform_World]){device.XformPushAndSet(GxXform_World,C44Matrix());}
 ~WorldScope(){if(world.m_level)--world.m_level;world.m_dirty=1;}
};
}
void Register(){enabled=CVar::Register("wxlCreationBeams","WXL native metadata-driven creation spell beams",0,WXL_ENABLE_CREATION_BEAMS?"1":"0",nullptr,0,false,nullptr,false);}
bool Enabled() noexcept{return WXL_ENABLE_CREATION_BEAMS&&NativeEnabled()&&BlpEnabled()&&enabled&&enabled->m_intValue;}
void Stop() noexcept{state.reset();}
void ReleaseScene(CM2Scene* scene) noexcept{if(state&&state->scene==scene)Stop();}
bool StartMindFlay(CCharacterComponent& component,uint32_t duration) noexcept{try{
 Stop();if(!Enabled()||!duration||duration>10000||component.m_data.m_preferences.m_classId!=5||!component.m_data.m_model||!component.m_data.m_model->m_scene)return false;
 auto next=std::make_unique<State>();next->actor=component.m_data.m_model;++next->actor->m_refCount;next->scene=next->actor->m_scene;next->serial=component.m_wxlSerial;next->handle=next->actor->m_handle;next->race=component.m_data.m_preferences.m_raceID;next->sex=component.m_data.m_preferences.m_sexID;
 next->started=next->lastTick=Now();next->expires=next->started+duration;next->retryAt=next->started;
 if(!Owner(*next))return false;state=std::move(next);return true;
 }catch(...){Stop();return false;}}
void Render(CSimpleModel* frame,const C3Vector& camera) noexcept{try{
 if(!state)return;
 if(!Owner(*state)||Reached(Now(),state->expires)){Stop();return;}
 auto& s=*state;
 if(frame!=CCharacterCreation::m_charCustomizeFrame||!frame->m_isVisible||frame->GetScene()!=s.scene||!Finite(camera)||!g_theGxDevicePtr||!g_theGxDevicePtr->CapsHasContext(-1))return;
 const auto now=Now();if(!s.manager){if(!Reached(now,s.retryAt))return;s.retryAt=now+RetryMs;if(!Build(s))return;}
 C3Vector source;if(!Source(s,source))return;
 // Exact authored target of the existing ManaFunnel impact, in actor space.
 C3Vector target=C3Vector(-6.85f,0.0f,5.45f)*s.actor->GetAnimatedWorldTransform();
 if(!Finite(target))return;const auto dx=source.x-target.x,dy=source.y-target.y,dz=source.z-target.z;
 const auto distance2=dx*dx+dy*dy+dz*dz;if(!std::isfinite(distance2)||distance2<0.000001f||distance2>100000000.0f)return;
 for(auto& chain:s.chains){const auto begin=s.started+uint32_t(chain->row.m_segDelay),end=begin+uint32_t(chain->row.m_segDuration);
  const bool ready=chain->texture->m_status.m_highestSeverity<STATUS_ERROR&&TextureGetGxTex(chain->texture,0,nullptr);
  s.manager->Move(chain->index,&source,&target);s.manager->SetBoltVisible(chain->index,ready&&Reached(now,begin)&&(chain->persist||!Reached(now,end)));
 }
 const auto elapsed=(std::min)(uint32_t(now-s.lastTick),250u);s.lastTick=now;s.manager->Update(float(elapsed)*0.001f);
 // Native Glue projection already uses a camera-relative view. Supply an
 // identity world base, then let the real manager subtract this camera once.
 auto& device=*g_theGxDevicePtr;if(device.m_xforms[GxXform_World].m_level>1||device.m_xforms[GxXform_Tex0].m_level>2)return;
 WorldScope scope(device);s.manager->Render(camera,false);
 }catch(...){Stop();}}
}
