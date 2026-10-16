// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include "platform/waystone/CharacterRoster.hpp"
#include <array>
#include <utility>
namespace wxl::waystone::appearance {
// Creature appearances bind whole skins, never a retail body atlas. This pure
// policy is shared by the legacy bridge and the future build4 owner adapter;
// it does not enable old offsets or admit a native model.
struct PrivateCreatureTexturePlan {uint32_t count{};std::array<uint32_t,3> types{};std::array<std::string,3> paths{};bool operator==(const PrivateCreatureTexturePlan&)const=default;};
inline bool PlanPrivateCreatureTextures(uint32_t race,uint32_t sex,uint32_t skin,uint32_t outfit,PrivateCreatureTexturePlan& output){
 if(race==28 && sex==0){if(skin>=3 || outfit)return false;try{PrivateCreatureTexturePlan next;next.count=1;next.types[0]=8;constexpr const char* names[]{"character\\murloc\\male\\sahauginskin.blp","character\\murloc\\male\\sahauginskinblue.blp","character\\murloc\\male\\sahauginskingray.blp"};next.paths[0]=names[skin];output=std::move(next);return true;}catch(...){return false;}}
 if(sex!=1 || (race!=14 && race!=28) || (race==14 && (skin>=3 || outfit)) || (race==28 && (skin>=5 || outfit>=5)))return false;
 try{PrivateCreatureTexturePlan next;
  if(race==14){constexpr uint32_t ids[]{4039141,4039142,4039147};next.count=3;next.types={11,12,13};const auto path=std::string("creature\\tuskarr\\tuskarrfemale_hd_")+std::to_string(ids[skin])+".blp";next.paths.fill(path);}
  else{next.count=2;next.types={11,31,0};next.paths[0]=std::string("creature\\murloc2maria\\murloc2_skin_0")+std::to_string(skin)+".blp";next.paths[1]=std::string("creature\\murloc2maria\\murloc2_outfit_0")+std::to_string(outfit)+".blp";}
  output=std::move(next);return true;
 }catch(...){return false;}
}
inline bool PrivateEnabled(){const char* v=std::getenv("WXL64_PRIVATE_RACES");return v && v[0]=='1' && !v[1];}
inline const char* PrivatePath(uint32_t race,uint32_t sex){if(sex>1)return nullptr;switch(race){
case 14:return sex?"character\\tuskarr\\tuskarrfemale_hd.m2":"character\\tuskarr\\male\\tuskarrmale.m2";
case 16:return sex?"character\\eredar\\female\\race_eredarfemale.m2":"character\\eredar\\male\\race_eredarmale.m2";
case 19:return sex?"character\\ogre\\female\\ogrefemale.m2":"character\\ogre\\male\\ogremale.m2";
case 23:return sex?"character\\broken\\female\\brokenfemale.m2":"character\\broken\\male\\brokenmale_hd.m2";
case 28:return sex?"creature\\murloc2maria\\murloc2maria.m2":"character\\murloc\\male\\murlochd.m2";default:return nullptr;}}
// Exact Maria atlas promotion from x32 InjectTxidNames. Restrict by model
// identity as well as FDID; another model sharing this atlas keeps its contract.
inline uint32_t PrivateCreatureTextureSlot(std::string_view path,uint32_t fdid,uint32_t type){
 return type==0 && fdid==1502339u && PathKey(path)==PrivatePath(28,1)?31u:type;
}
inline bool PrivateBody(uint32_t race,uint32_t sex){return PrivatePath(race,sex) && !(race==14 && sex==0);}
inline bool PrivateModel(std::string_view path){const auto key=PathKey(path);for(uint32_t race:{14u,16u,19u,23u,28u})for(uint32_t sex=0;sex<2;++sex)if(key==PrivatePath(race,sex))return true;return false;}
inline bool PrivateAsset(std::string_view path){const auto key=PathKey(path);for(uint32_t race:{14u,16u,19u,23u,28u})for(uint32_t sex=0;sex<2;++sex){std::string stem=PrivatePath(race,sex);stem.resize(stem.size()-3);if(key.starts_with(stem))return true;}return false;}
inline bool PrivateRedirect(std::string_view input,std::string& output){auto key=PathKey(input);if(key.rfind("hd\\",0)==0)key.erase(0,3);
 struct Alias{const char* from;uint32_t race,sex;};constexpr Alias aliases[]={
 {"character\\tuskarr\\tuskarrmale_hd",14,0},{"character\\tuskarr\\female\\tuskarrfemale",14,1},{"character\\tuskarr\\tuskarrfemale",14,1},
 {"character\\murloc\\female\\murlocfemale",28,1}};
 for(auto rule:aliases){std::string_view stem=rule.from;if(!key.starts_with(stem))continue;const auto rest=key.substr(stem.size());bool valid=rest==".m2" || rest==".mdx" || (rest.size()==7 && rest[0]>='0' && rest[0]<='9' && rest[1]>='0' && rest[1]<='9' && rest.substr(2)==".skin") || (rest.size()==12 && rest[4]=='-' && rest.substr(7)==".anim" && std::all_of(rest.begin(),rest.begin()+4,[](char c){return c>='0' && c<='9';}) && rest[5]>='0' && rest[5]<='9' && rest[6]>='0' && rest[6]<='9');if(!valid)continue;output=PrivatePath(rule.race,rule.sex);output.resize(output.size()-3);output+=(rest==".mdx"?".m2":rest);return output!=key;}
 return false;
}
}
