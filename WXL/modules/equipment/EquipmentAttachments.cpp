#include "WXL/ProceduralEnchants.hpp"
// Typed native consumer of the owned retail item graph. No renderer hooks.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "WXL/EquipmentAttachments.hpp"
#include "EnchantModelTimeline.hpp"
#include "WXL/ItemGraph.hpp"
#include "WXL/RetailChoices.hpp"
#include "WXL/NativeFeatures.hpp"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Common/TextureCache.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/ItemVisualsRec.h"
#include "WoW/Source/DB/Rec/ItemVisualEffectsRec.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include <cmath>
#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>
#ifndef WXL_ENABLE_EQUIPMENT_ATTACHMENTS
#define WXL_ENABLE_EQUIPMENT_ATTACHMENTS 1
#endif
namespace wxl::native {
namespace {
constexpr uint32_t Missing=0xffffffffu;
CVar* enabled=nullptr;
std::string Key(std::string value){for(auto& c:value){if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c-'A'+'a');}if(value.size()>4&&value.compare(value.size()-4,4,".mdx")==0)value.replace(value.size()-4,4,".m2");return value;}
bool SafePart(const std::string& value){return !value.empty()&&value.size()<240&&value.find("..") == std::string::npos&&value.find(':')==std::string::npos&&value.front()!='\\'&&value.front()!='/';}
std::string ObjectPath(const std::string& folder,const std::string& name,bool texture){
 if(!SafePart(folder)||!SafePart(name))return {};auto path=Key("Item\\ObjectComponents\\"+folder+"\\"+name);
 if(texture&&(path.size()<4||path.compare(path.size()-4,4,".blp")))path+=".blp";return path.size()<260?path:std::string{};
}
struct Child {
 CM2Model* model=nullptr;CM2Model* parent=nullptr;uint32_t parentHandle=0;bool skinned=false;std::string path,variant;
 ~Child(){if(model){
  auto* actual=skinned?model->m_skinningSource:model->m_attachParent;
  // DetachFromParent consumes an attachment reference even when unbound.
  // Failed validation owns creation only; a moved child belongs to its new owner.
  if(actual&&actual==parent&&actual->m_handle==parentHandle){
   model->SetEmittersEnabled(0);model->SetRibbonsEnabled(0);
   if(skinned)model->SetSkinningSource(nullptr);else model->DetachFromParent();
  }
  model->Release(); // exactly the creation reference held by this Child
 }}
};
struct Slot {
 uint32_t display=0,retryAt=0;uint64_t generation=0;bool ready=false;
 std::vector<std::unique_ptr<Child>> children;
};
template<class T> int Rank(const T& row,uint32_t race,uint32_t fallback,uint32_t sex){
 int rank=0;if(row.raceId){if(row.raceId==race)rank=6;else if(row.raceId==fallback)rank=4;else return -1;}
 if(row.genderId!=Missing&&row.genderId!=sex)return -1;return rank+(row.genderId==sex?1:0);
}
bool Preferred(const std::vector<ItemGraphModel>& entries,const ItemGraphModel& row,uint32_t race,uint32_t fallback,uint32_t sex){
 const auto rank=Rank(row,race,fallback,sex);if(rank<0)return false;
 for(const auto& other:entries){if(&other==&row||other.modelSlot!=row.modelSlot||other.attachId!=row.attachId||other.modelIndex!=row.modelIndex)continue;
  const auto next=Rank(other,race,fallback,sex);if(next>rank||(next==rank&&other.model<row.model))return false;}
 return true;
}
bool HasSlot(CM2Model& model,uint32_t type){for(uint32_t i=0;i<model.m_shared->m_data->m_textures.m_count;++i)if(model.m_shared->m_data->m_textures[i].m_type==type)return true;return false;}
bool BindTexture(CM2Model& model,uint32_t type,const std::string& path){
 if(path.empty()||!HasSlot(model,type)||!SFile::FileExists(path.c_str()))return false;
 CStatus status;auto* texture=TextureCreate(path.c_str(),CGxTexFlags(),&status,0);if(!texture)return false;
 const bool valid=Key(texture->m_filename)==path&&texture->m_status.m_highestSeverity<STATUS_ERROR&&TextureGetGxTex(texture,0,nullptr);
 if(valid)model.ReplaceTexture(type,texture);HandleClose(reinterpret_cast<HOBJECT>(texture));return valid;
}
bool Materials(CM2Model& model,const ItemGraphModel& entry,const std::vector<ItemGraphMaterial>& rows,uint32_t race,uint32_t fallback,uint32_t sex){
 if(!entry.texture.empty()&&HasSlot(model,2)&&!BindTexture(model,2,ObjectPath(entry.folder,entry.texture,true)))return false;
 std::array<const ItemGraphMaterial*,32> selected{};std::array<int,32> ranks;ranks.fill(-1);
 for(const auto& row:rows){if(!row.textureType||row.textureType>=32||!HasSlot(model,row.textureType)||(row.modelIndex!=Missing&&row.modelIndex!=entry.modelIndex))continue;
  const auto rank=Rank(row,race,fallback,sex);if(rank<0||rank<ranks[row.textureType])continue;
  selected[row.textureType]=&row;ranks[row.textureType]=rank;}
 for(uint32_t type=1;type<32;++type)if(selected[type]){
  // TextureFilePath already owns the complete mounted spelling. Rebuilding
  // it as ObjectComponents loses non-object and nested material locations.
  const auto& row=*selected[type];
  const auto path=row.texturePath.empty()?ObjectPath(row.folder,row.texture,true):Key(row.texturePath);
  if(!SafePart(path)||!path.ends_with(".blp")||!BindTexture(model,type,path))return false;
 }
 return true;
}
bool SlotOwnsGroup(uint32_t inventory,uint32_t group){
 switch(inventory){case 1:return group==27;case 3:return group==26;case 4:return group==10;case 5:return group==22||group==28;
 case 6:return group==18;case 7:return group==11||group==13;case 8:return group==5||group==20;case 9:return group==8;
 case 10:return group==4||group==23;case 16:return group==15;case 19:return group==9;case 20:return group==22||group==13;default:return false;}
}
bool Filter(CM2Model& model,const ItemGraphModel& entry,uint32_t inventory,bool skinned){
 if(entry.geoFilter.count>16||!model.m_shared->m_skinData||!model.m_skinSections)return false;
 if(!entry.geoFilter.count&&!skinned){model.SetGeometryVisible(0,65535,1);model.OptimizeVisibleGeometry();return true;}
 const auto& sections=model.m_shared->m_skinData->m_skinSections;
 for(uint32_t i=0;i<entry.geoFilter.count;++i){bool found=false;for(uint32_t j=0;j<sections.m_count;++j)found|=sections[j].m_skinSectionId==entry.geoFilter.ids[i];if(!found)return false;}
 std::vector<uint16_t> selected;
 for(uint32_t i=0;i<sections.m_count;++i){const auto id=sections[i].m_skinSectionId;
  bool keep=!entry.geoFilter.count;for(uint32_t j=0;j<entry.geoFilter.count;++j)keep|=id==entry.geoFilter.ids[j];
  if(keep&&(!skinned||(id&&SlotOwnsGroup(inventory,id/100))))selected.push_back(id);}
 if(selected.empty())return false;
 model.SetGeometryVisible(0,65535,0);for(auto id:selected)model.SetGeometryVisible(id,id,1);model.OptimizeVisibleGeometry();return true;
}
uint32_t Fallback(uint32_t race,uint32_t nativeRace){
 // The actual mounted model owns native suffix identity. Only the donor's
 // explicit allied-family fallback applies when that identity is unavailable.
 if(race==24)return 3;if(race==27)return 4;
 const auto* rec=nativeRace?g_chrRacesDB.GetRecord(nativeRace):nullptr;
 const auto prefix=rec&&rec->m_clientPrefix?Key(rec->m_clientPrefix):std::string{};
 struct Family{const char* prefix;const char* shortName;uint32_t race;};
 constexpr Family families[]={{"hum","hu",1},{"orc","or",2},{"dwa","dw",3},{"nig","ni",4},{"sco","sc",5},{"und","sc",5},{"tau","ta",6},{"gno","gn",7},{"tro","tr",8},{"gob","go",9},{"blo","be",10},{"dra","dr",11}};
 for(const auto& family:families)if(prefix==family.shortName||prefix.compare(0,3,family.prefix)==0)return family.race;
 switch(race){case 24:return 3;case 27:return 4;case 22:case 26:return 24;case 12:return 22;case 9:return 35;case 13:return 27;case 15:return 29;case 18:return 31;case 20:case 30:case 84:return 11;case 29:return 34;case 31:return 32;case 34:return 3;case 36:return 2;default:return race;}
}
bool SameOwner(const CCharacterComponent& c,const EquipmentAttachmentState& state);
}
struct HelmetSection {uint32_t index=0,id=0,visible=0;};
struct EquipmentAttachmentState {
 CM2Model* model=nullptr;uint32_t handle=0,race=0,fallback=0,sex=0;uint64_t serial=0;uint32_t lastWork=0;bool worked=false;
 std::array<Slot,11> slots;
 uint32_t helmetDisplay=0;uint64_t helmetGeneration=0;bool helmetMetadataReady=false;
 ItemGraphHelmet helmet;const M2SkinProfile* helmetSkin=nullptr;
 std::vector<HelmetSection> helmetSections;std::vector<uint8_t> helmetCaptured;
};
namespace {
bool SameOwner(const CCharacterComponent& c,const EquipmentAttachmentState& s){auto* model=c.m_data.m_model;return model&&model==s.model&&model->m_handle==s.handle&&c.m_wxlSerial==s.serial;}
bool HelmetActor(const CCharacterComponent& c,const EquipmentAttachmentState& owner){
 const auto wire=c.m_data.m_preferences.m_raceID;
 if(!owner.race||wire==16||wire==23||(c.m_data.m_flags&1u))return false;
 auto path=Key(owner.model->m_shared->m_fileName);
 if(path.find("eredar")!=std::string::npos||path.find("broken")!=std::string::npos)return false;
 if(path.starts_with("hd\\"))path.erase(0,3);
 if(wire<=11&&!owner.model->m_shared->m_wxlAssets&&!path.ends_with("_hd.m2"))return false;
 return path.starts_with("character\\")||path.starts_with("creature\\");
}
bool HelmetRuleMatches(const ItemGraphHelmetRule& rule,uint32_t race){
 return rule.raceId?rule.raceId==race:race&&(rule.raceBitSelection&3u)==3u;
}
void ApplyHelmet(CCharacterComponent& c,EquipmentAttachmentState& owner){
 const auto display=c.m_itemDisplayID[ITEMSLOT_HEAD];const auto generation=ItemGraphGeneration();
 if(owner.helmetDisplay!=display||owner.helmetGeneration!=generation){
  equipment_attachments::BeginGeometry(c);owner.helmetDisplay=display;owner.helmetGeneration=generation;owner.helmetMetadataReady=false;owner.helmet={};
 }
 if(!display||!ItemGraphOwnsDisplay(display)||!HelmetActor(c,owner)) {equipment_attachments::BeginGeometry(c);return;}
 // A model-less/pending head row must not hide the character's hair or ears.
 const auto& head=owner.slots[ITEMSLOT_HEAD];
 if(!head.ready||head.display!=display||head.children.empty()){equipment_attachments::BeginGeometry(c);return;}
 if(!owner.helmetMetadataReady){if(!ItemGraphHelmetData(display,owner.sex,owner.race,owner.helmet))return;owner.helmetMetadataReady=true;}
 auto* model=owner.model;const auto* skin=model->m_shared->m_skinData;
 if(!skin||!model->m_skinSections||skin->m_skinSections.m_count>16384)return;
 if(owner.helmetSkin!=skin){owner.helmetSections.clear();owner.helmetCaptured.clear();owner.helmetSkin=skin;}
 if(owner.helmetCaptured.size()!=skin->m_skinSections.m_count)owner.helmetCaptured.assign(skin->m_skinSections.m_count,0);
 std::array<bool,100> hidden{};
 for(const auto& rule:owner.helmet.rules)if(rule.hideGroup<hidden.size()&&HelmetRuleMatches(rule,owner.race))hidden[rule.hideGroup]=true;
 bool changed=false;
 for(uint32_t i=0;i<skin->m_skinSections.m_count;++i){const auto id=skin->m_skinSections[i].m_skinSectionId;
  // N01..N99 only. Mandatory body ID0 and N00 remain untouched.
  if(!id||id%100==0||id/100>=hidden.size()||!hidden[id/100]||!model->m_skinSections[i])continue;
  if(!owner.helmetCaptured[i]){owner.helmetSections.push_back({i,id,model->m_skinSections[i]});owner.helmetCaptured[i]=1;}
  changed=true;
 }
 if(changed){for(uint32_t group=0;group<hidden.size();++group)if(hidden[group])model->SetGeometryVisible(group*100+1,group*100+99,0);model->OptimizeVisibleGeometry();}
}
bool BuildSlot(CCharacterComponent& c,EquipmentAttachmentState& owner,uint32_t slot,uint32_t display,Slot& output){
 std::vector<ItemGraphModel> models;std::vector<ItemGraphMaterial> materials;
 if(!ItemGraphModels(display,models)||!ItemGraphMaterials(display,materials))return false;
 uint32_t inventory=0;if(!ItemGraphInventoryType(display,inventory)||models.size()>256||materials.size()>256)return false;
 // Donor Murloc has weapon-only equipment; dragon forms support shoulders
 // and waist attachments only. Humanoid body layers remain native-owned.
 const auto wire=c.m_data.m_preferences.m_raceID;
 if(wire==28||((owner.race==52||owner.race==70)&&slot!=1&&slot!=4)){output.ready=true;return true;}
 std::vector<std::string> claimed;
 for(uint32_t prior=0;prior<slot;++prior)for(const auto& child:owner.slots[prior].children)if(child->skinned)claimed.push_back(child->variant);
 for(const auto& row:models){
  if((row.modelSlot!=Missing&&row.modelSlot!=slot)||row.attachId==Missing||!Preferred(models,row,owner.race,owner.fallback,owner.sex))continue;
  const bool skinned=row.attachId==19;const auto path=ObjectPath(row.folder,row.model,false);
  if(path.empty()||!SFile::FileExists(path.c_str()))return false;
  const auto variant=skinned?path+"|"+std::to_string(inventory):path;
  if(skinned&&std::find(claimed.begin(),claimed.end(),variant)!=claimed.end())continue;
  if(!skinned&&!owner.model->HasAttachment(row.attachId))return false;
  auto child=std::make_unique<Child>();child->parent=owner.model;child->parentHandle=owner.handle;child->path=path;child->variant=variant;child->skinned=skinned;child->model=owner.model->m_scene->CreateModel(path.c_str(),0);
  auto* model=child->model;
  if(!model||!model->IsLoaded(1,0)||!model->m_shared||!model->m_shared->m_data||!model->m_textures||Key(model->m_shared->m_fileName)!=path)return false;
  if(!Materials(*model,row,materials,owner.race,owner.fallback,owner.sex)||!Filter(*model,row,inventory,skinned))return false;
  if(skinned){if(!model->SetSkinningSource(owner.model,CM2Model::SkinningSourceMode::Equipment,wire==18&&owner.sex==1?1.0f:0.0f))return false;claimed.push_back(variant);}
  else {model->AttachToParent(owner.model,row.attachId,nullptr,0);if(model->m_attachParent!=owner.model)return false;}
  model->SetBoneSequence(0xffffffffu,0,0xffffffffu,0,1.0f,1,1);
  equipment_attachments::ApplyItemVisual(model,0,display);
  output.children.push_back(std::move(child));
 }
 output.ready=true;return true;
}
}
namespace equipment_attachments {
void Register(){procedural_enchants::Register();RegisterTimedItemVisuals();enabled=CVar::Register("wxlEquipmentAttachments","WXL native retail armor models and native item visual validation",0,WXL_ENABLE_EQUIPMENT_ATTACHMENTS?"1":"0",nullptr,0,false,nullptr,false);}
bool Enabled() noexcept{return WXL_ENABLE_EQUIPMENT_ATTACHMENTS&&NativeEnabled()&&AssetsEnabled()&&Db2Enabled()&&BlpEnabled()&&enabled&&enabled->m_intValue;}
bool SuppressNativeArmor(const CCharacterComponent& c,uint32_t slot) noexcept{
 if(!Enabled()||slot>=11||(c.m_data.m_flags&1u))return false;
 const auto race=RetailRecipeRace(c);return c.m_data.m_preferences.m_raceID==28||race==52||race==70;
}
void BeginGeometry(CCharacterComponent& c) noexcept{try{
 auto* state=c.m_wxlEquipmentAttachments;if(!state)return;
 if(SameOwner(c,*state)&&state->helmetSkin&&state->model->m_shared&&state->helmetSkin==state->model->m_shared->m_skinData&&state->model->m_skinSections){
  const auto& sections=state->helmetSkin->m_skinSections;bool changed=false;
  for(const auto& saved:state->helmetSections)if(saved.index<sections.m_count&&sections[saved.index].m_skinSectionId==saved.id&&!state->model->m_skinSections[saved.index]){
   state->model->m_skinSections[saved.index]=saved.visible;changed=true;
  }
  if(changed){state->model->UnoptimizeVisibleGeometry();state->model->OptimizeVisibleGeometry();}
 }
 state->helmetSections.clear();state->helmetCaptured.clear();state->helmetSkin=nullptr;
 }catch(...){}}
void InvalidateSlot(CCharacterComponent& c,uint32_t slot) noexcept{
 if(!c.m_wxlEquipmentAttachments||slot>=11)return;
 if(slot==ITEMSLOT_HEAD)BeginGeometry(c);
 auto& slots=c.m_wxlEquipmentAttachments->slots;slots[slot]=Slot{};
 for(uint32_t next=slot+1;next<11;++next){slots[next].ready=false;slots[next].retryAt=0;}
}
bool SelectBodyLayers(const CCharacterComponent& c,uint32_t display,std::array<std::string,8>& paths) noexcept{try{
 paths={};if(!Enabled()||!ItemGraphOwnsDisplay(display)||(c.m_data.m_flags&1u)||c.m_data.m_preferences.m_sexID>1)return false;
 // Keep the component-layer identity contract: these are ComponentSection
 // values, not replaceable model texture types or native compositor priorities.
 std::vector<ItemGraphMaterial> rows;if(!ItemGraphMaterials(display,rows)||rows.size()>256)return false;
 if(SuppressNativeArmor(c,0))return true;
 const auto& prefs=c.m_data.m_preferences;auto race=RetailRecipeRace(c);if(!race)race=RetailRaceId(prefs.m_raceID);
 const auto fallback=Fallback(prefs.m_raceID,RetailEquipmentRace(c));
 std::array<const ItemGraphMaterial*,8> selected{};std::array<int,8> ranks;ranks.fill(-1);
 for(const auto& row:rows){if(row.layer>=8||row.textureType!=Missing||row.modelIndex!=Missing)continue;
  const auto rank=Rank(row,race,fallback,prefs.m_sexID);if(rank<0||rank<ranks[row.layer])continue;
  selected[row.layer]=&row;ranks[row.layer]=rank;
 }
 std::array<std::string,8> result;
 for(uint32_t region=0;region<8;++region)if(selected[region]){
  auto path=Key(selected[region]->texturePath);
  // CACHEENTRY owns a bounded 128-byte filename. Reject a long authority
  // path instead of letting its native cache silently truncate to another file.
  if(!SafePart(path)||path.size()>=sizeof(((CACHEENTRY*)nullptr)->m_fileName)||path.size()<4||path.compare(path.size()-4,4,".blp")||!SFile::FileExists(path.c_str()))return false;
  result[region]=std::move(path);
 }
 paths.swap(result);return true;
 }catch(...){paths={};return false;}}
void BindNativeMaterials(CM2Model* model,uint32_t display) noexcept{try{
 if(!Enabled()||!model)return;std::vector<ItemGraphModel> entries;std::vector<ItemGraphMaterial> materials;
 if(!ItemGraphModels(display,entries)||!ItemGraphMaterials(display,materials))return;
 if(!model->IsLoaded(1,0)||!model->m_shared||!model->m_shared->m_data||!model->m_textures)return;
 const auto actual=Key(model->m_shared->m_fileName);
 auto stem=[](const std::string& path){const auto start=path.find_last_of("\\/");auto value=path.substr(start==std::string::npos?0:start+1);const auto ext=value.find_last_of('.');return value.substr(0,ext);};
 for(const auto& entry:entries)if(entry.modelSlot==Missing&&entry.attachId==Missing&&entry.raceId==0&&entry.genderId==Missing&&stem(Key(entry.model))==stem(actual)){
  Materials(*model,entry,materials,0,0,Missing);return;
 }
 }catch(...){}}
void Release(CCharacterComponent& c) noexcept{const bool restore=c.m_wxlEquipmentAttachments&&!c.m_wxlEquipmentAttachments->helmetSections.empty();BeginGeometry(c);delete c.m_wxlEquipmentAttachments;c.m_wxlEquipmentAttachments=nullptr;if(restore)c.m_flags|=4u;}
void Apply(CCharacterComponent& c) noexcept{try{
 auto* model=c.m_data.m_model;const auto& prefs=c.m_data.m_preferences;
 if(!Enabled()||(c.m_data.m_flags&1u)||prefs.m_sexID>1||!model){Release(c);return;}
 bool needed=false;for(uint32_t slot=0;slot<11;++slot)needed|=c.m_itemDisplayID[slot]&&ItemGraphOwnsDisplay(c.m_itemDisplayID[slot]);if(!needed){Release(c);return;}
 if(!model->IsLoaded(0,0)||!model->m_shared||!model->m_shared->m_data||!model->m_scene||!g_theGxDevicePtr||!g_theGxDevicePtr->CapsHasContext(-1))return;
 auto race=RetailRecipeRace(c);if(!race)race=RetailRaceId(prefs.m_raceID);const auto fallback=Fallback(prefs.m_raceID,RetailEquipmentRace(c));const auto sex=(race==52||race==70)?0u:prefs.m_sexID;
 auto* state=c.m_wxlEquipmentAttachments;
 if(state&&(!SameOwner(c,*state)||state->race!=race||state->fallback!=fallback||state->sex!=sex)){Release(c);state=nullptr;}
 if(!state){auto next=std::make_unique<EquipmentAttachmentState>();next->model=model;next->handle=model->m_handle;next->serial=c.m_wxlSerial;next->race=race;next->fallback=fallback;next->sex=sex;c.m_wxlEquipmentAttachments=state=next.release();}
 // Clear unequipped/replaced slot children before any asynchronous graph work.
 for(uint32_t slot=0;slot<11;++slot)if(state->slots[slot].display!=c.m_itemDisplayID[slot]){state->slots[slot]=Slot{};state->slots[slot].display=c.m_itemDisplayID[slot];
  // Lower slots own shared collection paths; changed lower ownership wakes
  // higher slots previously suppressed by that owner, independent of equip order.
  for(uint32_t higher=slot+1;higher<11;++higher){state->slots[higher].ready=false;state->slots[higher].retryAt=0;}}
 for(auto& slot:state->slots)if(slot.ready)for(const auto& child:slot.children){
  if(child->skinned?child->model->m_skinningSource!=model:child->model->m_attachParent!=model){slot.ready=false;slot.retryAt=0;break;}}
 ApplyHelmet(c,*state);
 const auto now=OsGetAsyncTimeMs();if(state->worked&&uint32_t(now-state->lastWork)<16)return;state->worked=true;state->lastWork=now;
 uint32_t work=0;
 for(uint32_t slot=0;slot<11&&work<2;++slot){auto& old=state->slots[slot];if(!old.display||old.ready||(old.retryAt&&int32_t(now-old.retryAt)<0))continue;
  const auto id=old.display;
  if(id<=uint32_t(INT32_MAX)&&g_itemDisplayInfoDB.m_loaded&&int32_t(id)>=g_itemDisplayInfoDB.m_minID&&int32_t(id)<=g_itemDisplayInfoDB.m_maxID&&g_itemDisplayInfoDB.m_recordsById&&g_itemDisplayInfoDB.m_recordsById[id-g_itemDisplayInfoDB.m_minID]){old.ready=true;continue;}
  ++work;Slot next;next.display=old.display;next.generation=ItemGraphGeneration();
  if(BuildSlot(c,*state,slot,old.display,next)){old=std::move(next);}else{old.children.clear();old.retryAt=now+500;}
  if(slot==ITEMSLOT_HEAD)ApplyHelmet(c,*state);
 }
 }catch(...){Release(c);}}
void ApplyItemVisual(CM2Model* parent,int32_t visual,uint32_t display,uint32_t weaponSubclass) noexcept{try{
 if(!parent)return;
 if(!parent->m_scene){procedural_enchants::Release(parent);ReleaseItemVisuals(*parent);return;}
 auto* record=visual>0?g_itemVisualsDB.GetRecord(visual):nullptr;
 ItemGraphVisual modern;uint32_t subclass=weaponSubclass;
 if(subclass==UINT32_MAX)ItemGraphWeaponSubclass(display,subclass);
 const bool resolved=!record&&Enabled()&&(visual>0?ItemGraphVisualData(uint32_t(visual),modern):ItemGraphDisplayVisual(display,modern));
 if(resolved)procedural_enchants::Apply(parent,modern,subclass);else procedural_enchants::Release(parent);
 if(resolved&&SameTimedItemVisual(*parent,visual,display,subclass))return;
 RetireTimedItemVisuals(*parent);
 // The item owns its visual children. Retiring timeline children keep their
 // own source end animation; unrelated old native visual children are cleared.
 for(auto* child=parent->m_attachList;child;){auto* next=child->m_attachNext;
  if(!OwnsTimedItemVisual(*parent,child)){child->SetEmittersEnabled(0);child->SetRibbonsEnabled(0);child->DetachFromParent();}child=next;
 }
 if(!record){
  if(!resolved)return;
  struct Choice {uint32_t point;std::string path;float scale;float alpha=1,offset[3]{};uint32_t animation=0;};
  std::vector<Choice> choices;choices.reserve(modern.models.size()+modern.effects.size()+modern.kitModels.size());
  for(const auto& effect:modern.models)choices.push_back({effect.attachmentId,effect.path,1.0f});
  if(subclass!=UINT32_MAX)for(const auto& effect:modern.effects){
   if(effect.displayWeaponSubclassId!=subclass||!effect.fileDataId||effect.path.empty()||!std::isfinite(effect.scale)||effect.scale<=0)continue;
   // Kit-only rows have no model. A separately named direct attachment model
   // remains usable without interpreting its kit ID as a WotLK visual ID.
   choices.push_back({effect.attachmentId,effect.path,effect.scale});
  }
  InstallTimedItemVisuals(*parent,visual,display,subclass,modern.kitModels);
  for(const auto& effect:choices){
   if(!parent->IsLoaded(1,0)||!parent->HasAttachment(effect.point))continue;
   const auto path=Key(effect.path);if(!SafePart(path)||path.size()>=260||!SFile::FileExists(path.c_str()))continue;
   // Before attachment the creation reference is the only owner. Do not use
   // Child's attached-model destructor on a not-yet-attached visual.
   auto release=[](CM2Model* model){if(model)model->Release();};
   std::unique_ptr<CM2Model,decltype(release)> owner(parent->m_scene->CreateModel(path.c_str(),0),release);
   auto* child=owner.get();if(!child)continue;
   if(child->IsLoaded(1,0)&&child->m_shared&&Key(child->m_shared->m_fileName)==path){
    child->SetBoneSequence(0xffffffffu,effect.animation,0xffffffffu,0,1.0f,1,1);
    child->m_baseAlpha=effect.alpha;
    child->AttachToParent(parent,effect.point,nullptr,0);
    if(child->m_attachParent==parent)child->SetWorldTransform(C3Vector(effect.offset[0],effect.offset[1],effect.offset[2]),0,effect.scale);
   }
   // Owner drops the creation reference; successful attachment retained its own.
  }
  return;
 }
 for(uint32_t point=0;point<5;++point){if(!parent->IsLoaded(1,0)||!parent->HasAttachment(point))continue;
  const auto* effect=g_itemVisualEffectsDB.GetRecord(record->m_slot[point]);if(!effect||!effect->m_model||!*effect->m_model)continue;
  const auto path=Key(effect->m_model);if(path.size()>=260||!SFile::FileExists(path.c_str()))continue;
  auto release=[](CM2Model* model){if(model)model->Release();};
  std::unique_ptr<CM2Model,decltype(release)> child(parent->m_scene->CreateModel(path.c_str(),0),release);
  if(!child||!child->IsLoaded(1,0)||!child->m_shared||Key(child->m_shared->m_fileName)!=path)continue;
  child->AttachToParent(parent,point,nullptr,0);
  // Attachment retains the visual. The RAII owner drops only creation's ref.
 }
 }catch(...){}}
}
}
