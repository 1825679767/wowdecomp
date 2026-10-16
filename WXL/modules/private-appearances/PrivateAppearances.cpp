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
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/HelmetGeosetVisDataRec.h"
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
 bool ready=false;uint32_t color=0,horn=1;std::array<uint32_t,36> nativeSignature{};
 ~PrivateAppearanceState(){for(auto row:textures)if(row.second)HandleClose(reinterpret_cast<HOBJECT>(row.second));for(auto row:original)if(row.second)HandleClose(reinterpret_cast<HOBJECT>(row.second));}
};
namespace private_appearances {namespace {
namespace ap=wxl::appearance;
CVar* enabled=nullptr;
std::string Key(const char* p){std::string s=p?p:"";for(auto& c:s){if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c-'A'+'a');}if(s.size()>4&&s.compare(s.size()-4,4,".mdx")==0)s.replace(s.size()-4,4,".m2");return s;}
bool Path(uint32_t race,uint32_t sex,const std::string& path){
 if(race==23&&sex<2)return path.find(sex?"broken\\female\\brokenfemale":"broken\\male\\brokenmale")!=std::string::npos;
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

CTexture* BoundTexture(CM2Model* model,uint32_t type){
 if(!model||!model->m_shared||!model->m_shared->m_data||!model->m_textures)return nullptr;
 for(uint32_t i=0;i<model->m_shared->m_data->m_textures.m_count;++i)if(model->m_shared->m_data->m_textures[i].m_type==type)return model->m_textures[i];return nullptr;
}
std::array<uint32_t,36> NativeSignature(const CCharacterComponent& c){
 const auto& p=c.m_data.m_preferences;std::array<uint32_t,36> result{};
 result[0]=p.m_skinID;result[1]=p.m_faceId;result[2]=p.m_hairStyleId;result[3]=p.m_hairColorId;result[4]=p.m_facialHairId;
 for(uint32_t i=0;i<12;++i)result[5+i]=c.m_itemDisplayID[i];
 for(uint32_t i=0;i<19;++i)result[17+i]=c.m_data.m_geosets[i];return result;
}
bool NativeBodyReady(const CCharacterComponent& c){
 auto* body=BoundTexture(c.m_data.m_model,1);
 return !c.m_request&&!c.m_dirtySections&&!(c.m_flags&5u)&&body&&TextureGetGxTex(body,0,nullptr);
}
bool Geometry(CCharacterComponent& c,PrivateAppearanceState& state){
 auto* model=c.m_data.m_model;const auto race=c.m_data.m_preferences.m_raceID,sex=c.m_data.m_preferences.m_sexID;
 if(!model->m_skinSections)return false;
 const auto* skin=model->m_shared->m_skinData;uint32_t head=0;std::array<uint32_t,60> lowest{};
 for(uint32_t i=0;i<skin->m_skinSections.m_count;++i){const auto id=skin->m_skinSections[i].m_skinSectionId;
  if(id>0&&id<100&&(!head||id<head))head=id;const auto group=id/100;
  if(group>=20&&group<60&&(!lowest[group]||id<lowest[group]))lowest[group]=id;
 }
 uint32_t crest=1502;
 if(race==23&&sex==1)for(auto id:c.m_data.m_geosets)if(id>=1502&&id<=1506)crest=id;
 // Compute native equipment overlap masks BEFORE writing visibility. This
 // avoids re-show/hide churn each ordinary frame under an equipped helmet.
 std::array<bool,7> hidden{};uint32_t cape=0;
 if(race==23){
  ItemDisplayInfoRec display;
  if(c.m_itemDisplayID[ITEMSLOT_HEAD]&&g_itemDisplayInfoDB.GetRecord(c.m_itemDisplayID[ITEMSLOT_HEAD],&display)&&display.m_modelName[0]&&*display.m_modelName[0]){
   const auto* vis=g_helmetGeosetVisDataDB.GetRecord(display.m_helmetGeosetVisID[sex]);
   const auto equipmentRace=RetailEquipmentRace(c);const auto mask=equipmentRace<32?(1u<<equipmentRace):0u;
   if(vis)for(uint32_t i=0;i<7;++i)hidden[i]=(uint32_t(vis->m_hideGeoset[i])&mask)!=0;
  }
  if(c.m_itemDisplayID[ITEMSLOT_CAPE]&&g_itemDisplayInfoDB.GetRecord(c.m_itemDisplayID[ITEMSLOT_CAPE],&display)&&display.m_geosetGroup[0]){
   if(display.m_geosetGroup[0]<0||display.m_geosetGroup[0]>=99)return false;
   cape=1501+uint32_t(display.m_geosetGroup[0]);
  }
 }
 constexpr uint32_t groups[]={0,1,2,3,7,16,17};bool changed=false;
 for(uint32_t i=0;i<skin->m_skinSections.m_count;++i){const auto id=skin->m_skinSections[i].m_skinSectionId;bool controlled=false,wanted=false;
  if(!id){controlled=true;wanted=true;}
  else if(id<100){controlled=true;wanted=id==head;}
  else if(id>=2000&&id<=6000){controlled=true;wanted=id/100<60&&id==lowest[id/100];}
  else if(race==14&&(id==301||id==1101||id==1102)){controlled=true;wanted=true;}
  if(race==23&&sex==1){
   if(id>=201&&id<=203){controlled=true;wanted=id==201+state.horn;}
   if(id>=1501&&id<=1506){controlled=true;wanted=id==crest;}
   if(id==302){controlled=true;wanted=true;}
  }
  // Exact native legacy helmet placeholders, not retail DB2 hide rules.
  // ID0 is never hidden. Other robe/glove/boot/equipment groups are untouched.
  if(race==23){
   for(uint32_t group=0;group<7;++group)if(hidden[group]&&id>=groups[group]*100+1&&id<=groups[group]*100+99){controlled=true;wanted=id==groups[group]*100+1;}
   if(cape&&id>=1500&&id<=1599){controlled=true;wanted=id==cape;}
  }
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
 if(c.m_data.m_preferences.m_raceID==23){
  // The classic compositor owns body/face/tendrils. Match its validated hair
  // byte to the private palette before waiting for its real body atlas.
  if(c.m_data.m_preferences.m_hairColorId!=s.color){c.SetHairColor(int32_t(s.color),true,nullptr);return false;}
  if(!NativeBodyReady(c))return false;
  if(c.m_data.m_preferences.m_sexID==1){auto* body=BoundTexture(c.m_data.m_model,1);if(!body||!Slot(c.m_data.m_model,2))return false;
   if(BoundTexture(c.m_data.m_model,2)!=body)c.m_data.m_model->ReplaceTexture(2,body);
  }
 }
 if(!Geometry(c,s))return false;s.nativeSignature=NativeSignature(c);s.ready=true;return true;
}
}
void Register(){enabled=CVar::Register("wxlPrivateAppearances","WXL authored private Broken and Tuskarr/Murloc female appearances",2,WXL_ENABLE_PRIVATE_APPEARANCES?"1":"0",nullptr,0,false,nullptr,false);}
bool Enabled() noexcept {return WXL_ENABLE_PRIVATE_APPEARANCES&&NativeEnabled()&&AssetsEnabled()&&BlpEnabled()&&enabled&&enabled->m_intValue;}
uint32_t Identity(uint32_t race,uint32_t sex) noexcept {return ((race==23&&sex<2)||(sex==1&&(race==14||race==28)))?ap::PrivateModel(uint8_t(race),uint8_t(sex)):0;}
bool Describe(uint32_t race,uint32_t sex,const char* mountedNativePath,Description& output) noexcept {try{
 const auto id=Identity(race,sex);const auto path=Key(mountedNativePath);
 if(!Enabled()||!id||!Path(race,sex,path)||!SFile::FileExists(path.c_str()))return false;
 Description next;next.id=id;next.path=path;
 if(race==23){next.options.push_back({ap::PrivateHairColor,10,1,"Hair Color"});if(sex==1)next.options.push_back({ap::PrivateHornStyle,3,2,"Horn Style"});}
 else {next.options.push_back({ap::PrivateSkin,race==14?3u:5u,1,"Skin Color"});if(race==28)next.options.push_back({ap::PrivateOutfit,5,1,"Outfit"});}
 output=std::move(next);return true;
}catch(...){return false;}}
bool Validate(const ap::Selection& selection) noexcept {try{
 if(!Enabled()||!Identity(selection.race,selection.sex)||!selection.alternate.empty())return false;
 if(selection.race==23){
  if(selection.choices.size()!=(selection.sex==1?2u:1u))return false;bool color=false,horn=false;
  for(auto c:selection.choices){if(c.option==ap::PrivateHairColor&&!color&&c.choice>=1&&c.choice<=10)color=true;
   else if(selection.sex==1&&c.option==ap::PrivateHornStyle&&!horn&&c.choice>=1&&c.choice<=3)horn=true;else return false;}
  return color&&(selection.sex==0||horn);
 }
 const auto count=selection.race==14?1u:2u;if(selection.choices.size()!=count)return false;
 bool skin=false,outfit=false;
 for(auto c:selection.choices){
  if(c.option==ap::PrivateSkin&&!skin&&c.choice>=1&&c.choice<=(selection.race==14?3u:5u))skin=true;
  else if(selection.race==28&&c.option==ap::PrivateOutfit&&!outfit&&c.choice>=1&&c.choice<=5)outfit=true;
  else return false;
 }
 return skin&&(selection.race==14||outfit);
}catch(...){return false;}}
const char* OptionName(uint32_t id) noexcept {return id==ap::PrivateSkin?"Skin Color":id==ap::PrivateOutfit?"Outfit":id==ap::PrivateHairColor?"Hair Color":id==ap::PrivateHornStyle?"Horn Style":nullptr;}
const char* ChoiceName(uint32_t race,uint32_t option,uint32_t choice) noexcept {
 static constexpr const char* skins[]={"Verdant","Lagoon","Coral","Violet","Pearl"};
 static constexpr const char* outfits[]={"Brown / Coral","Auburn / Ocean","Deep Sea / Kelp","Seafoam / Royal","Platinum / Ivory"};
 static constexpr const char* tuskarr[]={"Variant 1","Variant 2","Variant 7"};
 static constexpr const char* palette[]={"Palette 0","Palette 1","Palette 2","Palette 3","Palette 4","Palette 5","Palette 6","Palette 7","Palette 8","Palette 9"};
 static constexpr const char* horns[]={"Flat cap","Horn pair","Alternate horn pair"};
 if(race==23&&choice){if(option==ap::PrivateHairColor&&choice<=10)return palette[choice-1];if(option==ap::PrivateHornStyle&&choice<=3)return horns[choice-1];}
 if(!choice)return nullptr;if(race==14&&option==ap::PrivateSkin&&choice<=3)return tuskarr[choice-1];
 if(race==28&&choice<=5)return option==ap::PrivateSkin?skins[choice-1]:option==ap::PrivateOutfit?outfits[choice-1]:nullptr;return nullptr;
}
uint32_t ChoiceSwatch(uint32_t race,uint32_t option,uint32_t choice) noexcept {
 static constexpr uint32_t broken[]={0x81222C,0x63362A,0x3E2A37,0x5C3E2D,0x843109,0xA0A07E,0x38323B,0x4C3570,0x25567E,0x345539};
 if(race==23&&option==ap::PrivateHairColor&&choice&&choice<=10)return broken[choice-1];
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
 if(prefs.m_raceID==23){if(!Slot(model,1)||!Slot(model,6)||!Geoset(model,0))return false;
  if(prefs.m_sexID==1){uint32_t crest=1502;for(auto id:c.m_data.m_geosets)if(id>=1502&&id<=1506)crest=id;
   if(!Slot(model,2)||!Geoset(model,302)||!Geoset(model,crest)||!Geoset(model,201)||!Geoset(model,202)||!Geoset(model,203))return false;
  }
 }
 if(auto* current=c.m_wxlPrivateAppearance;current&&Matches(c,*current))return Bind(c,*current);
 std::vector<RetailChoicePair> rows;if(!CaptureRetailChoicePairs(c,rows))return false;
 ap::Selection selection;selection.race=uint8_t(prefs.m_raceID);selection.sex=uint8_t(prefs.m_sexID);
 uint32_t skin=0,outfit=0;for(auto row:rows){selection.choices.push_back({row.option,row.choice});if(row.option==ap::PrivateSkin)skin=row.choice-1;else if(row.option==ap::PrivateOutfit)outfit=row.choice-1;}
 if(!Validate(selection))return false;
 auto next=std::make_unique<PrivateAppearanceState>();next->model=model;next->handle=model->m_handle;next->serial=c.m_wxlSerial;next->revision=RetailChoiceRevision(c);next->id=desc.id;
 char path[128];
 if(prefs.m_raceID==23){
  for(auto row:rows){if(row.option==ap::PrivateHairColor)next->color=row.choice-1;else if(row.option==ap::PrivateHornStyle)next->horn=row.choice-1;}
  std::snprintf(path,sizeof(path),"character\\broken\\hair00_%02u.blp",next->color);if(!Texture(model,*next,6,path))return false;
 }else if(prefs.m_raceID==14){static constexpr uint32_t ids[]={4039141,4039142,4039147};
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
 if(prefs.m_raceID==23&&prefs.m_sexID==1){CTexture* original=nullptr;bool captured=false;
  if(c.m_wxlPrivateAppearance&&c.m_wxlPrivateAppearance->model==model&&c.m_wxlPrivateAppearance->handle==model->m_handle){const auto it=c.m_wxlPrivateAppearance->original.find(2);if(it!=c.m_wxlPrivateAppearance->original.end()){original=it->second;captured=true;}}
  if(!captured)original=BoundTexture(model,2);
  next->original.emplace(2,original?reinterpret_cast<CTexture*>(HandleDuplicate(reinterpret_cast<HOBJECT>(original))):nullptr);
 }
 // Publish only after every authored resource and required section is valid.
 // A pending texture is retained for a later native readiness retry.
 delete c.m_wxlPrivateAppearance;c.m_wxlPrivateAppearance=next.release();return Bind(c,*c.m_wxlPrivateAppearance);
}catch(...){return false;}}
bool Ready(const CCharacterComponent& c) noexcept {try{return Enabled()&&c.m_wxlPrivateAppearance&&c.m_wxlPrivateAppearance->ready&&Matches(c,*c.m_wxlPrivateAppearance)&&(c.m_data.m_preferences.m_raceID!=23||(NativeBodyReady(c)&&c.m_wxlPrivateAppearance->nativeSignature==NativeSignature(c)));}catch(...){return false;}}
void Release(CCharacterComponent& c) noexcept {try{auto* s=c.m_wxlPrivateAppearance;if(!s)return;
 if(c.m_data.m_model==s->model&&c.m_data.m_model&&c.m_data.m_model->m_handle==s->handle){for(auto row:s->original)c.m_data.m_model->ReplaceTexture(row.first,row.second);c.m_flags|=5u;c.m_dirtySections=0xffffffffu;}
 delete s;c.m_wxlPrivateAppearance=nullptr;
}catch(...){}}
}
}
