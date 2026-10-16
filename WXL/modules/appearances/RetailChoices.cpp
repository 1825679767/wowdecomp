// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "RetailChoices.hpp"
#include "NativeFeatures.hpp"
#include "NativeStorage.hpp"
#include "FdidCompat.hpp"
#include "vendor/extensions/wxl-db2/src/schemas/AppearanceStore.hpp"
#include "platform/waystone/CharacterAtlas.hpp"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Glue/CharacterCreation.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <map>
#include <memory>
#include <mutex>
#include <string>
namespace wxl::native {
namespace {
namespace db=wxl::runtime::db2::appearance;
struct Option {uint32_t id{};int axis=-1;std::vector<uint32_t> choices;};
struct Authority {uint32_t chrModel{};std::string path;std::array<uint32_t,5> counts{1,1,1,1,1};std::vector<Option> options;bool ready=false;};
std::string Key(const char* value){std::string s=value?value:"";for(auto& c:s){if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c-'A'+'a');}if(s.ends_with(".mdx"))s.replace(s.size()-4,4,".m2");return s;}
uint32_t& Field(CharacterPreferences& p,RetailAxis axis){switch(axis){case RetailAxis::Skin:return p.m_skinID;case RetailAxis::Face:return p.m_faceId;case RetailAxis::HairStyle:return p.m_hairStyleId;case RetailAxis::HairColor:return p.m_hairColorId;default:return p.m_facialHairId;}}
int Axis(const char* label){const std::string_view n=label?label:"";if(n=="Skin Color"||n=="Skin Type"||n=="Fur Color")return 0;if(n=="Face")return 1;if(n=="Hair Style")return 2;if(n=="Hair Color")return 3;if(wxl::waystone::appearance::FacialOption(n))return 4;return -1;}
const Authority* Get(uint32_t race,uint32_t sex){
 if(!AppearancesEnabled()||sex>1||!RetailRaceId(race)||g_chrRacesDB.m_numRecords<=0)return nullptr;
 // Do not memoize before native storage and required schema tables exist.
 if(!db::Status().loaded)return nullptr;
 static std::mutex mutex;static std::map<uint64_t,std::unique_ptr<Authority>> cache;
 std::lock_guard lock(mutex);const uint64_t key=(uint64_t(race)<<1)|sex;
 if(auto it=cache.find(key);it!=cache.end())return it->second->ready?it->second.get():nullptr;
 auto a=std::make_unique<Authority>();a->chrModel=db::ChrModelForRace(RetailRaceId(race),sex);
 db::Recipe recipe;bool valid=a->chrModel && db::BuildForCharacter(RetailRaceId(race),sex,nullptr,0,recipe);
 const char* path=valid?ResolveModel(recipe.modelFileDataId):nullptr;
 auto* raceRec=g_chrRacesDB.GetRecord(race);auto* display=raceRec?g_creatureDisplayInfoDB.GetRecord(sex?raceRec->m_femaleDisplayID:raceRec->m_maleDisplayID):nullptr;
 auto* model=display?g_creatureModelDataDB.GetRecord(display->m_modelID):nullptr;
 valid=valid&&path&&model&&Key(path)==Key(model->m_modelName);a->path=Key(path);
 std::vector<uint8_t> source;
 valid=valid&&ReadAsset(path,source)&&source.size()>=8&&!std::memcmp(source.data(),"MD21",4);
 const auto count=valid?db::OptionCount(a->chrModel):0;valid=valid&&count&&count<=256;
 std::array<bool,5> assigned{};size_t total=0;
 for(uint32_t i=0;valid&&i<count;++i){WXL_ChrOption o{};valid=db::OptionAt(a->chrModel,i,o);if(!valid)break;
  Option entry;entry.id=o.id;const int axis=Axis(db::OptionName(o.id));const auto n=db::ChoiceCount(o.id);
  if(!n||n>8192||total+n>8192){valid=false;break;}total+=n;
  // Only the first named axis maps onto the five classic bytes. Extra axes keep explicit IDs.
  if(axis>=0&&!assigned[axis]){if(n>256){valid=false;break;}entry.axis=axis;assigned[axis]=true;a->counts[axis]=n;}
  for(uint32_t j=0;j<n;++j){WXL_ChrChoice c{};if(!db::ChoiceAt(o.id,j,c)){valid=false;break;}entry.choices.push_back(c.id);}
  a->options.push_back(std::move(entry));
 }
 a->ready=valid;auto* result=a.get();cache.emplace(key,std::move(a));return valid?result:nullptr;
}
const Authority* For(const CharacterPreferences& p,const CM2Model* model){const auto* a=Get(p.m_raceID,p.m_sexID);if(!a)return nullptr;if(model&&(!model->m_shared||Key(model->m_shared->m_fileName)!=a->path))return nullptr;return a;}
void Dirty(CCharacterComponent& c){c.m_dirtySections=0xffffffffu;c.m_flags|=5u;}
}
struct RetailChoiceState {uint32_t model{},revision=1;std::map<uint32_t,uint32_t> overrides;};
uint32_t RetailRaceId(uint32_t race) noexcept {static constexpr uint32_t ids[]={0,1,2,3,4,5,6,7,8,35,10,11,22,27,17,29,0,76,31,0,30,9,26,0,85,13,25,86,0,34,75,32};return race<std::size(ids)?ids[race]:0;}
uint32_t RetailChoiceCount(uint32_t race,uint32_t sex,RetailAxis axis) noexcept {try{auto* a=Get(race,sex);const auto i=uint32_t(axis);return a&&i<5?a->counts[i]:0;}catch(...){return 0;}}
bool RetailChoicesReady(const CharacterPreferences& p,const CM2Model* model) noexcept {try{return For(p,model)!=nullptr;}catch(...){return false;}}
bool NormalizeRetailPreferences(CharacterPreferences& p,const CM2Model* model) noexcept {try{auto* a=For(p,model);if(!a)return false;for(uint32_t i=0;i<5;++i)Field(p,RetailAxis(i))%=a->counts[i];return true;}catch(...){return false;}}
bool SetRetailAxis(CCharacterComponent& c,RetailAxis axis,int32_t value) noexcept {try{if(c.m_data.m_flags&1u)return false;auto* a=For(c.m_data.m_preferences,c.m_data.m_model);const auto i=uint32_t(axis);if(!a||i>=5)return false;if(value<0||uint32_t(value)>=a->counts[i])return true;
 Field(c.m_data.m_preferences,axis)=uint32_t(value);if(auto* s=c.m_wxlChoices){for(const auto& o:a->options)if(o.axis==int(i))s->overrides.erase(o.id);++s->revision;}Dirty(c);return true;
 }catch(...){return false;}}
bool CycleRetailAxis(CCharacterComponent& c,RetailAxis axis,int32_t delta) noexcept {try{if(c.m_data.m_flags&1u)return false;auto* a=For(c.m_data.m_preferences,c.m_data.m_model);const auto i=uint32_t(axis);if(!a||i>=5)return false;const int64_t n=a->counts[i];auto at=(int64_t(Field(c.m_data.m_preferences,axis))+delta)%n;if(at<0)at+=n;return SetRetailAxis(c,axis,int32_t(at));}catch(...){return false;}}
bool CollectRetailChoices(const CCharacterComponent& c,std::vector<uint32_t>& output) noexcept {try{auto* a=For(c.m_data.m_preferences,c.m_data.m_model);if(!a)return false;std::vector<uint32_t> result;auto p=c.m_data.m_preferences;
 for(const auto& o:a->options){auto selected=o.choices[o.axis<0?0:Field(p,RetailAxis(o.axis))%o.choices.size()];if(auto* s=c.m_wxlChoices;s&&s->model==a->chrModel){auto it=s->overrides.find(o.id);if(it!=s->overrides.end())selected=it->second;}result.push_back(selected);}output.swap(result);return true;}catch(...){return false;}}
bool RetailSelectionNeedsProtocol(const CCharacterComponent& c) noexcept {return c.m_wxlChoices && !c.m_wxlChoices->overrides.empty();}
uint32_t RetailChoiceRevision(const CCharacterComponent& c) noexcept {return c.m_wxlChoices?c.m_wxlChoices->revision:0;}
void TransferRetailChoices(const CCharacterComponent& from,CCharacterComponent& to) noexcept {try{
 if(!from.m_wxlChoices || from.m_data.m_preferences.m_raceID!=to.m_data.m_preferences.m_raceID || from.m_data.m_preferences.m_sexID!=to.m_data.m_preferences.m_sexID)return;
 auto* authority=For(to.m_data.m_preferences,to.m_data.m_model);if(!authority||authority->chrModel!=from.m_wxlChoices->model)return;
 auto next=std::make_unique<RetailChoiceState>(*from.m_wxlChoices);++next->revision;delete to.m_wxlChoices;to.m_wxlChoices=next.release();
 }catch(...){}}
void ReleaseRetailChoices(CCharacterComponent& c) noexcept {delete c.m_wxlChoices;c.m_wxlChoices=nullptr;}
namespace {
CCharacterComponent* Current(){auto* c=CCharacterCreation::m_character;return c&&RetailChoicesReady(c->m_data.m_preferences,c->m_data.m_model)?c:nullptr;}
bool Number(lua_State* L,int at,uint32_t& value){const auto n=lua_tonumber(L,at);if(lua_type(L,at)!=LUA_TNUMBER||!(n>=0&&n<=4294967295.0)||n!=uint32_t(n))return false;value=uint32_t(n);return true;}
int LuaOptions(lua_State* L){auto* c=Current();if(!c)return 0;const Authority* a=nullptr;try{a=For(c->m_data.m_preferences,c->m_data.m_model);}catch(...){return 0;}if(!a)return 0;lua_createtable(L,int(a->options.size()),0);
 for(size_t i=0;i<a->options.size();++i){const auto& o=a->options[i];lua_createtable(L,0,4);lua_pushinteger(L,o.id);lua_setfield(L,-2,"id");lua_pushstring(L,db::OptionName(o.id));lua_setfield(L,-2,"name");lua_pushinteger(L,o.choices.size());lua_setfield(L,-2,"count");lua_pushinteger(L,o.axis+1);lua_setfield(L,-2,"axis");lua_rawseti(L,-2,int(i+1));}return 1;}
int LuaChoices(lua_State* L){uint32_t id;if(!Number(L,1,id))return 0;auto* c=Current();if(!c)return 0;const Authority* a=nullptr;try{a=For(c->m_data.m_preferences,c->m_data.m_model);}catch(...){return 0;}if(!a)return 0;const Option* o=nullptr;for(const auto& entry:a->options)if(entry.id==id)o=&entry;if(!o)return 0;
 lua_createtable(L,int(o->choices.size()),0);for(size_t i=0;i<o->choices.size();++i){const auto choice=o->choices[i];lua_createtable(L,0,4);lua_pushinteger(L,choice);lua_setfield(L,-2,"id");lua_pushstring(L,db::ChoiceName(choice));lua_setfield(L,-2,"name");WXL_ChrChoice info{};db::ChoiceAt(id,uint32_t(i),info);lua_pushnumber(L,info.swatchColor);lua_setfield(L,-2,"swatchColor");lua_pushnumber(L,db::ChoiceSwatchColor2(choice));lua_setfield(L,-2,"swatchColor2");lua_rawseti(L,-2,int(i+1));}return 1;}
bool Select(CCharacterComponent& c,uint32_t id,uint32_t choice){auto* a=For(c.m_data.m_preferences,c.m_data.m_model);if(!a)return false;for(const auto& o:a->options)if(o.id==id){auto it=std::find(o.choices.begin(),o.choices.end(),choice);if(it==o.choices.end())return false;if(o.axis>=0)return SetRetailAxis(c,RetailAxis(o.axis),int32_t(it-o.choices.begin()));
 auto* s=c.m_wxlChoices;if(!s){c.m_wxlChoices=new RetailChoiceState;s=c.m_wxlChoices;s->model=a->chrModel;}if(s->model!=a->chrModel)return false;if(choice==o.choices.front())s->overrides.erase(id);else s->overrides[id]=choice;++s->revision;Dirty(c);return true;}return false;}
int LuaSelect(lua_State* L){uint32_t option,choice;bool ok=false;if(Number(L,1,option)&&Number(L,2,choice)){try{if(auto* c=Current())ok=Select(*c,option,choice);}catch(...){}}lua_pushboolean(L,ok);return 1;}
int LuaCanSave(lua_State* L){auto* c=Current();lua_pushboolean(L,c&&!RetailSelectionNeedsProtocol(*c));return 1;}
int LuaSelection(lua_State* L){static thread_local std::vector<uint32_t> selected;auto* c=Current();if(!c||!CollectRetailChoices(*c,selected))return 0;lua_createtable(L,int(selected.size()),0);for(size_t i=0;i<selected.size();++i){lua_pushinteger(L,selected[i]);lua_rawseti(L,-2,int(i+1));}return 1;}
}
void InitializeRetailChoiceLua(){FrameScript_RegisterFunction("WXL_RetailSelectionCanSave",&LuaCanSave);FrameScript_RegisterFunction("WXL_GetRetailOptions",&LuaOptions);FrameScript_RegisterFunction("WXL_GetRetailChoices",&LuaChoices);FrameScript_RegisterFunction("WXL_SetRetailChoice",&LuaSelect);FrameScript_RegisterFunction("WXL_GetRetailSelection",&LuaSelection);}
}
