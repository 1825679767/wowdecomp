// Included within RetailChoices.cpp's private namespace, after the existing Lua APIs.
// All mutations stay with the existing validated component/appearance authorities.
constexpr uint32_t DonorOptionBase=1000;
const Authority* DonorAuthority(CCharacterComponent*& c) {
 c=Current();
 return c&&!barber_appearance::Active()?For(c->m_data.m_preferences,c->m_data.m_model):nullptr;
}
const Option* DonorOption(const Authority& a,uint32_t glue) {
 if(glue>=DonorOptionBase){const auto i=glue-DonorOptionBase;return i<a.options.size()?&a.options[i]:nullptr;}
 // Native private and alternate models have independent IDs, not stock axes.
 if(a.privateProfile||alternate_appearances::IsAlternate()||glue<1||glue>5)return nullptr;
 for(const auto& o:a.options)if(o.axis==int(glue-1))return &o;
 return nullptr;
}
const char* DonorName(const Authority& a,const Option& o) {
 const auto* name=a.privateProfile?private_appearances::OptionName(o.id):db::OptionName(o.id);
 return name?name:"";
}
const char* DonorCategory(const Authority& a,const Option& o,uint32_t race) {
 const auto* name=DonorName(a,o);
 if(race==29&&!std::strcmp(name,"Tattoo"))return "face";
 const char* accessories[]={"Accessory","Accessories","Blindfold","Earring","Jewelry","Jewellery","Piercing","Ring","Horn","Headdress","Decoration"};
 for(auto* term:accessories)if(std::strstr(name,term))return "accessories";
 const char* facial[]={"Facial Hair","FacialHair","Beard","Mustache","Moustache","Sideburn","Chin","Tusk"};
 for(auto* term:facial)if(std::strstr(name,term))return "facial";
 const char* hair[]={"Hair","Fur Color","Fur Style","Quill"};
 for(auto* term:hair)if(std::strstr(name,term))return "hair";
 const char* body[]={"Skin","Body","Torso","Arm","Leg","Hand","Foot","Feet","Tail","Marking","Tattoo","Pattern","Posture","Build"};
 for(auto* term:body)if(std::strstr(name,term))return "body";
 const char* face[]={"Face","Eye","Scar","Nose","Ear","Brow","Forehead","Jaw"};
 for(auto* term:face)if(std::strstr(name,term))return "face";
 const auto secondary=a.privateProfile?0:db::OptionSecondaryOrderIndex(o.id);
 if(secondary==1)return "body";
 if(secondary==2||secondary==5)return "face";
 if(secondary==3||secondary==4)return "hair";
 return "accessories";
}
bool DonorSelected(const CCharacterComponent& c,const Authority& a,const Option& o,uint32_t& selected) {
 std::vector<uint32_t> choices;
 if(!CollectRetailChoices(c,choices)||choices.size()!=a.options.size())return false;
 for(size_t i=0;i<a.options.size();++i)if(a.options[i].id==o.id){
  const auto at=std::find(o.choices.begin(),o.choices.end(),choices[i]);
  if(at==o.choices.end())return false;selected=uint32_t(at-o.choices.begin());return true;
 }
 return false;
}
bool DonorChoice(const CCharacterComponent& c,const Authority& a,const Option& o,uint32_t at,const char*& name,uint32_t& first,uint32_t& second) {
 if(at>=o.choices.size())return false;
 const auto id=o.choices[at];
 if(a.privateProfile){name=private_appearances::ChoiceName(c.m_data.m_preferences.m_raceID,o.id,id);first=private_appearances::ChoiceSwatch(c.m_data.m_preferences.m_raceID,o.id,id);second=0;}
 else{WXL_ChrChoice choice{};if(!db::ChoiceAt(o.id,at,choice)||choice.id!=id)return false;name=db::ChoiceName(id);first=choice.swatchColor;second=db::ChoiceSwatchColor2(id);}
 if(!name)name="";return true;
}
int LuaDonorOption(lua_State* L) {try{
 uint32_t ordinal;CCharacterComponent* c=nullptr;const auto* a=DonorAuthority(c);
 if(!a||!Number(L,1,ordinal)||ordinal<1||ordinal>a->options.size())return 0;
 const auto& o=a->options[ordinal-1];
 lua_pushinteger(L,DonorOptionBase+ordinal-1);lua_pushstring(L,DonorName(*a,o));
 lua_pushinteger(L,a->privateProfile||alternate_appearances::IsAlternate()?0:o.axis+1);
 lua_pushinteger(L,o.choices.size());lua_pushstring(L,DonorCategory(*a,o,c->m_data.m_preferences.m_raceID));return 5;
 }catch(...){return 0;}}
int LuaDonorState(lua_State* L) {try{
 uint32_t glue,selected;CCharacterComponent* c=nullptr;const auto* a=DonorAuthority(c);
 if(!a||!Number(L,1,glue))return 0;const auto* o=DonorOption(*a,glue);
 if(!o||!DonorSelected(*c,*a,*o,selected))return 0;
 const char* name=nullptr;uint32_t first=0,second=0;
 if(!DonorChoice(*c,*a,*o,selected,name,first,second))return 0;
 lua_pushstring(L,DonorName(*a,*o));lua_pushstring(L,name);lua_pushinteger(L,selected+1);
 lua_pushinteger(L,o->choices.size());lua_pushnumber(L,first);lua_pushnumber(L,second);
 bool swatch=first||second;
 for(uint32_t i=0;!swatch&&i<o->choices.size();++i){uint32_t f=0,s=0;const char* n=nullptr;if(DonorChoice(*c,*a,*o,i,n,f,s))swatch=f||s;}
 lua_pushboolean(L,swatch);return 7;
 }catch(...){return 0;}}
int LuaDonorChoice(lua_State* L) {try{
 uint32_t glue,ordinal,selected;CCharacterComponent* c=nullptr;const auto* a=DonorAuthority(c);
 if(!a||!Number(L,1,glue)||!Number(L,2,ordinal)||!ordinal)return 0;const auto* o=DonorOption(*a,glue);
 if(!o||!DonorSelected(*c,*a,*o,selected))return 0;
 const char* name=nullptr;uint32_t first=0,second=0;
 if(!DonorChoice(*c,*a,*o,ordinal-1,name,first,second))return 0;
 lua_pushstring(L,name);lua_pushnumber(L,first);lua_pushnumber(L,second);lua_pushboolean(L,selected==ordinal-1);lua_pushboolean(L,first||second);return 5;
 }catch(...){return 0;}}
int LuaDonorSet(lua_State* L) {bool ok=false;try{
 uint32_t glue,ordinal;CCharacterComponent* c=nullptr;const auto* a=DonorAuthority(c);
 if(a&&Number(L,1,glue)&&Number(L,2,ordinal)&&ordinal){const auto* o=DonorOption(*a,glue);if(o&&ordinal<=o->choices.size())ok=Select(*c,o->id,o->choices[ordinal-1]);}
 }catch(...){}lua_pushboolean(L,ok);return 1;}
int LuaDonorCycle(lua_State* L) {bool ok=false;try{
 uint32_t glue,selected;CCharacterComponent* c=nullptr;const auto* a=DonorAuthority(c);
 const auto delta=lua_tonumber(L,2);
 if(a&&Number(L,1,glue)&&lua_type(L,2)==LUA_TNUMBER&&(delta>0||delta<0)&&std::isfinite(delta)){
  const auto* o=DonorOption(*a,glue);
  if(o&&!o->choices.empty()&&DonorSelected(*c,*a,*o,selected)){
   const auto n=uint32_t(o->choices.size());const auto next=delta>0?(selected+1)%n:(selected+n-1)%n;
   ok=Select(*c,o->id,o->choices[next]);
  }
 }
 }catch(...){}lua_pushboolean(L,ok);return 1;}
int LuaDonorIdentity(lua_State* L){try{CCharacterComponent* c=nullptr;if(!DonorAuthority(c))return 0;lua_pushinteger(L,c->m_data.m_preferences.m_raceID);lua_pushinteger(L,c->m_data.m_preferences.m_sexID);return 2;}catch(...){return 0;}}
int LuaDonorGeneration(lua_State* L){
 static std::string previous;static uint32_t generation=0;
 const auto top=lua_gettop(L);const auto count=LuaContext(L);
 std::string key=count&&lua_type(L,-1)==LUA_TSTRING?lua_tostring(L,-1):"";lua_settop(L,top);
 if(key!=previous){previous=std::move(key);if(++generation==0)generation=1;}
 lua_pushinteger(L,generation);return 1;
}
int LuaDonorPending(lua_State* L){try{CCharacterComponent* c=nullptr;const auto* a=DonorAuthority(c);lua_pushboolean(L,a&&(c->m_dirtySections||(c->m_flags&5u)));return 1;}catch(...){lua_pushboolean(L,true);return 1;}}
int LuaDonorTiming(lua_State* L){lua_pushnumber(L,OsGetAsyncTimeMs());return 1;}
int LuaDonorOnly(lua_State* L){try{CCharacterComponent* c=nullptr;const auto* a=DonorAuthority(c);lua_pushboolean(L,a&&(a->privateProfile||alternate_appearances::IsAlternate()));return 1;}catch(...){lua_pushboolean(L,true);return 1;}}
void InitializeDonorLua(){
 FrameScript_RegisterFunction("_WXL_M2_CUSTOMIZATION_OPTION",LuaDonorOption);
 FrameScript_RegisterFunction("_WXL_M2_CUSTOMIZATION_STATE",LuaDonorState);
 FrameScript_RegisterFunction("_WXL_M2_CUSTOMIZATION_CHOICE",LuaDonorChoice);
 FrameScript_RegisterFunction("_WXL_M2_CUSTOMIZATION_OPTION_SET",LuaDonorSet);
 FrameScript_RegisterFunction("_WXL_M2_CUSTOMIZATION_OPTION_CYCLE",LuaDonorCycle);
 FrameScript_RegisterFunction("_WXL_M2_CUSTOMIZATION_CYCLE",LuaDonorCycle);
 FrameScript_RegisterFunction("_WXL_M2_CUSTOMIZATION_GENERATION",LuaDonorGeneration);
 FrameScript_RegisterFunction("_WXL_M2_CUSTOMIZATION_PENDING",LuaDonorPending);
 FrameScript_RegisterFunction("_WXL_M2_CUSTOMIZATION_TIMING",LuaDonorTiming);
 FrameScript_RegisterFunction("_WXL_M2_CUSTOMIZATION_IDENTITY",LuaDonorIdentity);
 FrameScript_RegisterFunction("WXL_RetailChoiceUsesOwnedOptionsOnly",LuaDonorOnly);
}
