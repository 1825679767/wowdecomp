// Authored private creature appearances, independent of retail DB2 recipes.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "WXL/PrivateAppearances.hpp"
#include "WXL/RetailChoices.hpp"
#include "WXL/NativeFeatures.hpp"
#include "wxl/AppearanceProtocol.hpp"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include <algorithm>
#include <array>
#include <cstdio>
#include <map>
#include <memory>
#ifndef WXL_ENABLE_PRIVATE_APPEARANCES
#define WXL_ENABLE_PRIVATE_APPEARANCES 1
#endif
namespace wxl::native {
struct PrivateAppearanceState {
 CM2Model* model=nullptr;uint32_t handle=0,revision=0,id=0;uint64_t serial=0;
 std::map<uint32_t,CTexture*> textures,original;
 bool ready=false;
 ~PrivateAppearanceState(){for(auto row:textures)if(row.second)HandleClose(reinterpret_cast<HOBJECT>(row.second));for(auto row:original)if(row.second)HandleClose(reinterpret_cast<HOBJECT>(row.second));}
};
namespace private_appearances {namespace {
namespace ap=wxl::appearance;
CVar* enabled=nullptr;
std::string Key(const char* p){std::string s=p?p:"";for(auto& c:s){if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c-'A'+'a');}if(s.size()>4&&s.compare(s.size()-4,4,".mdx")==0)s.replace(s.size()-4,4,".m2");return s;}
bool Path(uint32_t race,uint32_t sex,const std::string& path){
 if(sex!=1)return false;
 if(race==14)return path.find("tuskarrfemale_hd")!=std::string::npos;
 if(race==28)return path.find("murloc\\male\\murlochd")!=std::string::npos||path.find("murloc\\female\\murlocfemale")!=std::string::npos||path.find("murloc2maria")!=std::string::npos;
 return false;
}
bool Slot(CM2Model* model,uint32_t type){for(uint32_t i=0;i<model->m_shared->m_data->m_textures.m_count;++i)if(model->m_shared->m_data->m_textures[i].m_type==type)return true;return false;}
bool Geoset(CM2Model* model,uint32_t id){const auto* skin=model->m_shared->m_skinData;if(!skin)return false;for(uint32_t i=0;i<skin->m_skinSections.m_count;++i)if(skin->m_skinSections[i].m_skinSectionId==id)return true;return false;}
bool Texture(CM2Model* model,PrivateAppearanceState& state,uint32_t type,const char* path){
 if(!Slot(model,type)||!SFile::FileExists(path))return false;
 CStatus status;auto* texture=TextureCreate(path,CGxTexFlags(),&status,0);if(!texture)return false;
 if(Key(texture->m_filename)!=Key(path)||texture->m_status.m_highestSeverity>=STATUS_ERROR){HandleClose(reinterpret_cast<HOBJECT>(texture));return false;}
 state.textures.emplace(type,texture);return true;
}
bool Geometry(CM2Model* model,uint32_t race){
 if(!model->m_skinSections)return false;
 const auto* skin=model->m_shared->m_skinData;uint32_t head=0;std::array<uint32_t,60> lowest{};
 for(uint32_t i=0;i<skin->m_skinSections.m_count;++i){
  const auto id=skin->m_skinSections[i].m_skinSectionId;
  if(id>0&&id<100&&(!head||id<head))head=id;
  const auto group=id/100;if(group>=20&&group<60&&(!lowest[group]||id<lowest[group]))lowest[group]=id;
 }
 bool changed=false;
 for(uint32_t i=0;i<skin->m_skinSections.m_count;++i){
  const auto id=skin->m_skinSections[i].m_skinSectionId;bool controlled=false,wanted=false;
  if(!id){controlled=true;wanted=true;}
  else if(id<100){controlled=true;wanted=id==head;}
  else if(id>=2000&&id<=6000){controlled=true;wanted=id/100<60&&id==lowest[id/100];}
  else if(race==14&&(id==301||id==1101||id==1102)){controlled=true;wanted=true;}
  // Post-compositor guards also run on ordinary frames. Avoid throwing away
  // and allocating native optimized geometry when the selected mask is intact.
  if(controlled&&bool(model->m_skinSections[i])!=wanted){model->SetGeometryVisible(id,id,wanted?1:0);changed=true;}
 }
 if(changed)model->OptimizeVisibleGeometry();return true;
}
bool Matches(const CCharacterComponent& c,const PrivateAppearanceState& s){auto* m=c.m_data.m_model;return m&&m==s.model&&m->m_handle==s.handle&&c.m_wxlSerial==s.serial&&s.revision==RetailChoiceRevision(c)&&s.id==Identity(c.m_data.m_preferences.m_raceID,c.m_data.m_preferences.m_sexID);}
bool Bind(CCharacterComponent& c,PrivateAppearanceState& s){
 for(auto row:s.textures)if(!TextureGetGxTex(row.second,0,nullptr))return false;
 for(auto row:s.textures){
  bool different=false;auto* model=c.m_data.m_model;
  for(uint32_t i=0;i<model->m_shared->m_data->m_textures.m_count;++i)
   if(model->m_shared->m_data->m_textures[i].m_type==row.first&&model->m_textures[i]!=row.second){different=true;break;}
  if(different)model->ReplaceTexture(row.first,row.second);
 }
 if(!Geometry(c.m_data.m_model,c.m_data.m_preferences.m_raceID))return false;s.ready=true;return true;
}
}
void Register(){enabled=CVar::Register("wxlPrivateAppearances","WXL authored private Tuskarr/Murloc female appearances",2,WXL_ENABLE_PRIVATE_APPEARANCES?"1":"0",nullptr,0,false,nullptr,false);}
bool Enabled() noexcept {return WXL_ENABLE_PRIVATE_APPEARANCES&&NativeEnabled()&&AssetsEnabled()&&BlpEnabled()&&enabled&&enabled->m_intValue;}
uint32_t Identity(uint32_t race,uint32_t sex) noexcept {return sex==1&&(race==14||race==28)?ap::PrivateModel(uint8_t(race),uint8_t(sex)):0;}
bool Describe(uint32_t race,uint32_t sex,const char* mountedNativePath,Description& output) noexcept {try{
 const auto id=Identity(race,sex);const auto path=Key(mountedNativePath);
 if(!Enabled()||!id||!Path(race,sex,path)||!SFile::FileExists(path.c_str()))return false;
 Description next;next.id=id;next.path=path;
 next.options.push_back({ap::PrivateSkin,race==14?3u:5u,1,"Skin Color"});
 if(race==28)next.options.push_back({ap::PrivateOutfit,5,1,"Outfit"});
 output=std::move(next);return true;
}catch(...){return false;}}
bool Validate(const ap::Selection& selection) noexcept {try{
 if(!Enabled()||!Identity(selection.race,selection.sex)||!selection.alternate.empty())return false;
 const auto count=selection.race==14?1u:2u;if(selection.choices.size()!=count)return false;
 bool skin=false,outfit=false;
 for(auto c:selection.choices){
  if(c.option==ap::PrivateSkin&&!skin&&c.choice>=1&&c.choice<=(selection.race==14?3u:5u))skin=true;
  else if(selection.race==28&&c.option==ap::PrivateOutfit&&!outfit&&c.choice>=1&&c.choice<=5)outfit=true;
  else return false;
 }
 return skin&&(selection.race==14||outfit);
}catch(...){return false;}}
const char* OptionName(uint32_t id) noexcept {return id==ap::PrivateSkin?"Skin Color":id==ap::PrivateOutfit?"Outfit":nullptr;}
const char* ChoiceName(uint32_t race,uint32_t option,uint32_t choice) noexcept {
 static constexpr const char* skins[]={"Verdant","Lagoon","Coral","Violet","Pearl"};
 static constexpr const char* outfits[]={"Brown / Coral","Auburn / Ocean","Deep Sea / Kelp","Seafoam / Royal","Platinum / Ivory"};
 static constexpr const char* tuskarr[]={"Variant 1","Variant 2","Variant 7"};
 if(!choice)return nullptr;if(race==14&&option==ap::PrivateSkin&&choice<=3)return tuskarr[choice-1];
 if(race==28&&choice<=5)return option==ap::PrivateSkin?skins[choice-1]:option==ap::PrivateOutfit?outfits[choice-1]:nullptr;return nullptr;
}
uint32_t ChoiceSwatch(uint32_t race,uint32_t option,uint32_t choice) noexcept {
 static constexpr uint32_t skins[]={0x65A84A,0x27B68C,0x2E83CC,0x8A4BD4,0xD7D0C5};
 static constexpr uint32_t outfits[]={0xCF3E50,0x2B83C6,0x3F8E3B,0x7543AE,0xD8D2C8};
 if(race==28&&choice&&choice<=5)return option==ap::PrivateSkin?skins[choice-1]:option==ap::PrivateOutfit?outfits[choice-1]:0;return 0;
}
bool Apply(CCharacterComponent& c) noexcept {try{
 if(c.m_wxlPrivateAppearance)c.m_wxlPrivateAppearance->ready=false;
 auto* model=c.m_data.m_model;const auto& prefs=c.m_data.m_preferences;
 Description desc;
 if(!model||!model->IsLoaded(0,0)||!model->m_shared||!model->m_shared->m_skinData||!model->m_textures||
    (c.m_data.m_flags&1u)||!Describe(prefs.m_raceID,prefs.m_sexID,model->m_shared->m_fileName,desc)) {Release(c);return false;}
 if(!g_theGxDevicePtr||!g_theGxDevicePtr->CapsHasContext(-1)||c.m_request)return false;
 if(prefs.m_raceID==14&&(!Geoset(model,0)||!Geoset(model,301)||!Geoset(model,1101)||!Geoset(model,1102)))return false;
 if(prefs.m_raceID==28&&!Geoset(model,0))return false;
 if(auto* current=c.m_wxlPrivateAppearance;current&&Matches(c,*current))return Bind(c,*current);
 std::vector<RetailChoicePair> rows;if(!CaptureRetailChoicePairs(c,rows))return false;
 ap::Selection selection;selection.race=uint8_t(prefs.m_raceID);selection.sex=uint8_t(prefs.m_sexID);
 uint32_t skin=0,outfit=0;for(auto row:rows){selection.choices.push_back({row.option,row.choice});if(row.option==ap::PrivateSkin)skin=row.choice-1;else if(row.option==ap::PrivateOutfit)outfit=row.choice-1;}
 if(!Validate(selection))return false;
 auto next=std::make_unique<PrivateAppearanceState>();next->model=model;next->handle=model->m_handle;next->serial=c.m_wxlSerial;next->revision=RetailChoiceRevision(c);next->id=desc.id;
 char path[128];
 if(prefs.m_raceID==14){static constexpr uint32_t ids[]={4039141,4039142,4039147};
  std::snprintf(path,sizeof(path),"creature\\tuskarr\\tuskarrfemale_hd_%u.blp",ids[skin]);
  for(uint32_t type=11;type<=13;++type)if(!Texture(model,*next,type,path))return false;
 }else{
  std::snprintf(path,sizeof(path),"creature\\murloc2maria\\murloc2_skin_%02u.blp",skin);if(!Texture(model,*next,11,path))return false;
  std::snprintf(path,sizeof(path),"creature\\murloc2maria\\murloc2_outfit_%02u.blp",outfit);if(!Texture(model,*next,31,path))return false;
 }
 for(auto row:next->textures){CTexture* original=nullptr;bool captured=false;
  if(c.m_wxlPrivateAppearance&&c.m_wxlPrivateAppearance->model==model&&c.m_wxlPrivateAppearance->handle==model->m_handle){const auto it=c.m_wxlPrivateAppearance->original.find(row.first);if(it!=c.m_wxlPrivateAppearance->original.end()){original=it->second;captured=true;}}
  if(!captured)for(uint32_t i=0;i<model->m_shared->m_data->m_textures.m_count;++i)if(model->m_shared->m_data->m_textures[i].m_type==row.first){original=model->m_textures[i];break;}
  next->original.emplace(row.first,original?reinterpret_cast<CTexture*>(HandleDuplicate(reinterpret_cast<HOBJECT>(original))):nullptr);
 }
 // Publish only after every authored resource and required section is valid.
 // A pending texture is retained for a later native readiness retry.
 delete c.m_wxlPrivateAppearance;c.m_wxlPrivateAppearance=next.release();return Bind(c,*c.m_wxlPrivateAppearance);
}catch(...){return false;}}
bool Ready(const CCharacterComponent& c) noexcept {try{return Enabled()&&c.m_wxlPrivateAppearance&&c.m_wxlPrivateAppearance->ready&&Matches(c,*c.m_wxlPrivateAppearance);}catch(...){return false;}}
void Release(CCharacterComponent& c) noexcept {try{auto* s=c.m_wxlPrivateAppearance;if(!s)return;
 if(c.m_data.m_model==s->model&&c.m_data.m_model&&c.m_data.m_model->m_handle==s->handle){for(auto row:s->original)c.m_data.m_model->ReplaceTexture(row.first,row.second);c.m_flags|=5u;c.m_dirtySections=0xffffffffu;}
 delete s;c.m_wxlPrivateAppearance=nullptr;
}catch(...){}}
}
}
