// Source-owned retail enchant model phases. No worker-thread mutation.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "EnchantModelTimeline.hpp"
#include "WXL/EquipmentAttachments.hpp"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/DB/Rec/SpellVisualKitModelAttachRec.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
namespace wxl::native {
namespace {
CVar* timed=nullptr;
constexpr uint32_t MaxModels=512;
bool On(){return equipment_attachments::Enabled()&&timed&&timed->m_intValue;}
bool Reached(uint32_t now,uint32_t deadline){return int32_t(now-deadline)>=0;}
std::string Key(std::string path){for(auto& c:path){if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c-'A'+'a');}if(path.size()>4&&path.compare(path.size()-4,4,".mdx")==0)path.replace(path.size()-4,4,".m2");return path;}
bool Safe(const std::string& path){return !path.empty()&&path.size()<240&&path.front()!='\\'&&path.front()!='/'&&path.find(':')==std::string::npos&&path.find("..") == std::string::npos;}
enum class Phase {Pending,Start,Loop,End,Done};
struct Entry {
 ItemGraphKitModel recipe;
 CM2Model* child=nullptr;CM2Model* parent=nullptr;Phase phase=Phase::Pending;
 uint64_t epoch=0;uint32_t due=0,phaseEnd=0;
 ~Entry(){if(child){if(!child->m_attachParent||child->m_attachParent==parent){child->SetEmittersEnabled(0);child->SetRibbonsEnabled(0);if(child->m_attachParent==parent)child->DetachFromParent();}child->Release();}}
};
}
struct EnchantAttachmentOwner {
 CM2Model* parent=nullptr;CM2Scene* scene=nullptr;
 EnchantAttachmentOwner* next=nullptr;EnchantAttachmentOwner** prev=nullptr;
 CM2Model* root=nullptr;uint64_t rootHandle=0,parentHandle=0,generation=0,epoch=0;
 bool rootBound=false,identified=false;int32_t visual=0;uint32_t display=0,subclass=UINT32_MAX;
 std::vector<std::unique_ptr<Entry>> entries;
};
namespace {
void Unlink(EnchantAttachmentOwner& owner){if(owner.prev)*owner.prev=owner.next;if(owner.next)owner.next->prev=owner.prev;owner.prev=nullptr;owner.next=nullptr;}
void Erase(EnchantAttachmentOwner* owner){if(!owner)return;Unlink(*owner);if(owner->parent&&owner->parent->m_wxlEnchantAttachments==owner)owner->parent->m_wxlEnchantAttachments=nullptr;delete owner;}
bool FinishPhase(Entry& entry,uint32_t now){M2BoneSequenceInfo info{};if(!entry.child||!entry.child->GetBoneSequenceInfo(UINT32_MAX,info))return true;return info.m_finished||Reached(now,entry.phaseEnd);}
bool Sequence(Entry& entry,uint32_t id,uint32_t now){
 if(!entry.child||!entry.child->HasSequence(id))return false;
 entry.child->SetBoneSequence(UINT32_MAX,id,0,0,1.0f,0,1);
 M2BoneSequenceInfo info{};if(!entry.child->GetBoneSequenceInfo(UINT32_MAX,info))return false;
 // Use the actual selected native sequence's repeat/end timing, not a guessed
 // fixed startup duration. Wrap-safe waits remain bounded to signed time range.
 const auto duration=uint32_t(info.m_endTime-info.m_startTime);
 if(duration>uint32_t(INT32_MAX))return false;
 entry.phaseEnd=info.m_endTime;return true;
}
void End(Entry& entry,uint32_t now){
 if(!entry.child||entry.phase==Phase::Pending){entry.phase=Phase::Done;return;}
 if(entry.recipe.endAnimationId>=0&&Sequence(entry,uint32_t(entry.recipe.endAnimationId),now))entry.phase=Phase::End;
 else entry.phase=Phase::Done;
}
void Placement(CM2Model& model,const ItemGraphKitModel& recipe){
 C44Matrix matrix;matrix.Identity();matrix.Scale(recipe.scale);
 SpellVisualKitModelAttachRec native;
 native.m_offset[0]=recipe.offset[0];native.m_offset[1]=recipe.offset[1];native.m_offset[2]=recipe.offset[2];
 native.m_yaw=recipe.yaw;native.m_pitch=recipe.pitch;native.m_roll=recipe.roll;
 // The native model-attach contract is Z(-yaw),Y(pitch),X(roll), then
 // offset(X,-Y,Z) in attachment-local coordinates (facing zero).
 CEffect::ApplyModelAttachRecToMatrix(&native,matrix,0.0f);
 if(recipe.positioner){
  if(!recipe.positioner->staticCompatible)return;
  for(const auto& state:recipe.positioner->states){
   C44Matrix local;local.Identity();local.Scale(state.scale);SpellVisualKitModelAttachRec positioned;
   for(uint32_t i=0;i<3;++i)positioned.m_offset[i]=state.position[i];positioned.m_yaw=state.yaw;positioned.m_pitch=state.pitch;positioned.m_roll=state.roll;
   CEffect::ApplyModelAttachRecToMatrix(&positioned,local,0.0f);
   // Compatibility port: static positioner is local to the authored attachment.
   matrix=local*matrix;
  }
 }
 model.SetWorldTransform(matrix);
}
bool Activate(EnchantAttachmentOwner& owner,Entry& entry,uint32_t now){
 auto& parent=*owner.parent;auto& recipe=entry.recipe;
 if(!parent.IsLoaded(1,0)||!parent.HasAttachment(recipe.attachmentId)||!Safe(recipe.path)||!SFile::FileExists(recipe.path.c_str()))return false;
 entry.child=owner.scene->CreateModel(recipe.path.c_str(),0);auto* child=entry.child;
 if(!child||!child->IsLoaded(1,0)||!child->m_shared||!child->m_shared->m_data||!child->m_shared->m_data->m_bones.m_count||Key(child->m_shared->m_fileName)!=recipe.path)return false;
 if(recipe.startAnimationId>=0&&!child->HasSequence(uint32_t(recipe.startAnimationId)))return false;
 if(!child->HasSequence(recipe.animationId))return false;
 child->m_baseAlpha=recipe.alpha;Placement(*child,recipe);child->AttachToParent(&parent,recipe.attachmentId,nullptr,0);
 if(child->m_attachParent!=&parent)return false;
 if(recipe.startAnimationId>=0){if(!Sequence(entry,uint32_t(recipe.startAnimationId),now))return false;entry.phase=Phase::Start;}
 else {if(!Sequence(entry,recipe.animationId,now))return false;entry.phase=Phase::Loop;}
 return true;
}
bool LiveOwner(EnchantAttachmentOwner& owner){
 if(owner.parent->m_scene!=owner.scene||owner.parent->m_handle!=owner.parentHandle)return false;
 auto* root=owner.parent;unsigned depth=0;while((root->m_attachParent||root->m_skinningSource)&&depth++<64)root=root->m_attachParent?root->m_attachParent:root->m_skinningSource;
 if(root->m_attachParent||root->m_skinningSource)return false;
 if(!owner.rootBound){owner.root=root;owner.rootHandle=root->m_handle;owner.rootBound=true;}
 return root==owner.root&&root->m_handle==owner.rootHandle;
}
}
namespace equipment_attachments {
void RegisterTimedItemVisuals(){timed=CVar::Register("wxlTimedEnchantModels","Source enchant model start/loop/end phases; StartDelay seconds compatibility policy",2,"1",nullptr,0,false,nullptr,false);}
bool SameTimedItemVisual(const CM2Model& parent,int32_t visual,uint32_t display,uint32_t subclass) noexcept {
 const auto* owner=parent.m_wxlEnchantAttachments;return On()&&owner&&owner->identified&&owner->parentHandle==parent.m_handle&&owner->scene==parent.m_scene&&owner->visual==visual&&owner->display==display&&owner->subclass==subclass&&owner->generation==ItemGraphVisualGeneration();
}
bool OwnsTimedItemVisual(const CM2Model& parent,const CM2Model* child) noexcept {
 const auto* owner=parent.m_wxlEnchantAttachments;if(!owner)return false;for(const auto& entry:owner->entries)if(entry->child==child)return true;return false;
}
void RetireTimedItemVisuals(CM2Model& parent) noexcept {
 auto* owner=parent.m_wxlEnchantAttachments;if(!owner)return;
 if(!On()||parent.m_scene!=owner->scene){Erase(owner);return;}
 owner->identified=false;++owner->epoch;for(auto& entry:owner->entries)if(entry->phase!=Phase::End&&entry->phase!=Phase::Done)End(*entry,owner->scene->m_time);
 std::erase_if(owner->entries,[](const auto& entry){return entry->phase==Phase::Done;});
}
void InstallTimedItemVisuals(CM2Model& parent,int32_t visual,uint32_t display,uint32_t subclass,const std::vector<ItemGraphKitModel>& models){
 if(!On()||!parent.m_scene)return;
 auto* owner=parent.m_wxlEnchantAttachments;
 if(owner&&(owner->scene!=parent.m_scene||owner->parentHandle!=parent.m_handle)){Erase(owner);owner=nullptr;}
 if(!owner){auto next=std::make_unique<EnchantAttachmentOwner>();next->parent=&parent;next->scene=parent.m_scene;next->parentHandle=parent.m_handle;
  next->next=next->scene->m_wxlEnchantOwners;next->prev=&next->scene->m_wxlEnchantOwners;if(next->next)next->next->prev=&next->next;
  parent.m_wxlEnchantAttachments=owner=next.release();owner->scene->m_wxlEnchantOwners=owner;
 }
 owner->identified=true;owner->visual=visual;owner->display=display;owner->subclass=subclass;owner->generation=ItemGraphVisualGeneration();++owner->epoch;
 for(auto recipe:models){if(owner->entries.size()>=MaxModels)break;if(recipe.displayWeaponSubclassId!=UINT32_MAX&&recipe.displayWeaponSubclassId!=subclass)continue;
  if(recipe.sourceFlags||!std::isfinite(recipe.startDelay)||recipe.startDelay<0||double(recipe.startDelay)*1000.0>double(INT32_MAX))continue;
  recipe.path=Key(std::move(recipe.path));auto entry=std::make_unique<Entry>();entry->parent=&parent;entry->recipe=std::move(recipe);entry->epoch=owner->epoch;
  // Compatibility policy: retail's float StartDelay is interpreted as seconds.
  // No donor/runtime code recovered its unit; the CVar is an explicit rollback.
  const auto delay=uint32_t(double(entry->recipe.startDelay)*1000.0);
  entry->due=owner->scene->m_time+delay;owner->entries.push_back(std::move(entry));
 }
}
bool CloneItemVisuals(const CM2Model& source,CM2Model& destination) noexcept {
 const auto* owner=source.m_wxlEnchantAttachments;if(!owner)return false;
 // Never turn a delayed/retiring source child into an unowned permanent clone.
 // A new native parent starts a new independent source timeline and epoch.
 if(!owner->identified||!On())return true;
 try{
  std::vector<ItemGraphKitModel> models;models.reserve(owner->entries.size());
  for(const auto& entry:owner->entries)if(entry->epoch==owner->epoch&&entry->phase!=Phase::End&&entry->phase!=Phase::Done)models.push_back(entry->recipe);
  InstallTimedItemVisuals(destination,owner->visual,owner->display,owner->subclass,models);
 }catch(...){ReleaseItemVisuals(destination);}
 return true;
}
void ReleaseItemVisuals(CM2Model& parent) noexcept {Erase(parent.m_wxlEnchantAttachments);}
void ReleaseSceneItemVisuals(CM2Scene& scene) noexcept {while(scene.m_wxlEnchantOwners)Erase(scene.m_wxlEnchantOwners);}
void TickItemVisuals(CM2Scene& scene) noexcept {
 // Snapshot retained native model refs before mutations: releasing a parent's
 // child can also unlink another timeline owner. Never walk a saved raw next
 // state pointer across recursive native child destruction.
 struct Release {void operator()(CM2Model* model)const{if(model)model->Release();}};
 try{
  std::vector<std::unique_ptr<CM2Model,Release>> parents;
  for(auto* owner=scene.m_wxlEnchantOwners;owner;owner=owner->next){
   ++owner->parent->m_refCount;std::unique_ptr<CM2Model,Release> keep(owner->parent);parents.push_back(std::move(keep));
  }
  for(const auto& keep:parents){auto* owner=keep->m_wxlEnchantAttachments;if(!owner||owner->scene!=&scene)continue;
   try{
    if(!On()||!LiveOwner(*owner)){Erase(owner);continue;}
    if(owner->identified&&owner->generation!=ItemGraphVisualGeneration()){
     auto* parent=owner->parent;const auto visual=owner->visual;const auto display=owner->display,subclass=owner->subclass;
     Erase(owner);ApplyItemVisual(parent,visual,display,subclass);continue;
    }
    const auto now=scene.m_time;
    for(auto& entry:owner->entries){
     if(entry->phase==Phase::Pending){if(entry->epoch!=owner->epoch)entry->phase=Phase::Done;else if(Reached(now,entry->due)&&!Activate(*owner,*entry,now))entry->phase=Phase::Done;}
     else if(!entry->child||entry->child->m_attachParent!=owner->parent||entry->child->m_scene!=&scene)entry->phase=Phase::Done;
     else if(entry->phase==Phase::Start&&FinishPhase(*entry,now)){if(Sequence(*entry,entry->recipe.animationId,now))entry->phase=Phase::Loop;else entry->phase=Phase::Done;}
     else if(entry->phase==Phase::Loop&&FinishPhase(*entry,now)){if(!Sequence(*entry,entry->recipe.animationId,now))entry->phase=Phase::Done;}
     else if(entry->phase==Phase::End&&FinishPhase(*entry,now))entry->phase=Phase::Done;
    }
    std::erase_if(owner->entries,[](const auto& entry){return entry->phase==Phase::Done;});
    if(!owner->identified&&owner->entries.empty())Erase(owner);
   }catch(...){Erase(owner);}
  }
 }catch(...){ReleaseSceneItemVisuals(scene);}
}
}
}
