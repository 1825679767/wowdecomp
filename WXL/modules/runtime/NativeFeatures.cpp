// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "NativeFeatures.hpp"
#include "CreationEffects.hpp"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include <atomic>
#include <cstdint>
#include <cstdlib>
namespace wxl::native {namespace {
constexpr uint32_t Assets=1,Blp=2,Materials=4,Particles=8,Appearances=16,Protocol=32,Db2=64,AppearanceSave=128,Wmo=256,SplitAdt=512,ClientExtensions=1024,RadialPing=2048,QuestMarker=4096,ChallengeMode=8192,MythicDifficulty=16384;
constexpr uint32_t Compiled=(WXL_ENABLE_NATIVE?1u:0u)*((WXL_ENABLE_ASSETS?Assets:0)|(WXL_ENABLE_BLP?Blp:0)|(WXL_ENABLE_MATERIALS?Materials:0)|(WXL_ENABLE_PARTICLES?Particles:0)|(WXL_ENABLE_APPEARANCES?Appearances:0)|(WXL_ENABLE_PROTOCOL?Protocol:0)|(WXL_ENABLE_DB2?Db2:0)|(WXL_ENABLE_APPEARANCE_SAVE?AppearanceSave:0)|(WXL_ENABLE_WMO?Wmo:0)|(WXL_ENABLE_SPLIT_ADT?SplitAdt:0)|(WXL_ENABLE_CLIENT_EXTENSIONS?ClientExtensions:0)|(WXL_ENABLE_RADIAL_PING?RadialPing:0)|(WXL_ENABLE_QUEST_MARKER?QuestMarker:0)|(WXL_ENABLE_CHALLENGE_MODE?ChallengeMode:0)|(WXL_ENABLE_MYTHIC_DIFFICULTY?MythicDifficulty:0));
std::atomic<uint32_t> enabled{Compiled};
bool Changed(CVar*,const char*,const char* value,void* arg){
 const auto bit=uint32_t(reinterpret_cast<uintptr_t>(arg));
 if(value && std::strtol(value,nullptr,10)!=0 && (Compiled&bit))enabled.fetch_or(bit,std::memory_order_release);
 else enabled.fetch_and(~bit,std::memory_order_release);
 return true;
}
bool On(uint32_t bit) noexcept{return (enabled.load(std::memory_order_acquire)&Compiled&bit)!=0;}
}
void RegisterFeatures(){
 creation_effects::Register();
 CVar::Register("wxlAreaLootAuto","WXL automatic area loot",2,"1",nullptr,0,false,nullptr,false);
 CVar::Register("wxlAreaLootQuality","WXL minimum automatic area loot quality",2,"0",nullptr,0,false,nullptr,false);
 struct Feature{const char* name;const char* help;uint32_t bit;};
 const Feature features[]{
  {"wxlChallengeMode","WXL native server-authoritative Mythic+ API; reload UI after changing",ChallengeMode},
  {"wxlMythicDifficulty","WXL third dungeon difficulty and portal models; reload world/UI after changing",MythicDifficulty},
  {"wxlQuestMarker","WXL native quest marker and objective rings",QuestMarker},
  {"wxlRadialPing","WXL native world radial pings",RadialPing},
  {"wxlClientExtensions","WXL addon helper and legacy packet facade; reload UI after changing",ClientExtensions},
  {"wxlModernWmo","WXL modern WMO byte providers; reload world after changing",Wmo},
  {"wxlSplitAdt","WXL split ADT byte providers; reload world after changing",SplitAdt},
  {"wxlAppearanceSave","WXL negotiated retail appearance saving; matching server support required",AppearanceSave},
  {"wxlRetailDb2","WXL retail DB2 schemas and FDID paths; restart after changing",Db2},
  {"wxlRetailAppearances","WXL retail appearance recipes and body atlas; reload models after changing",Appearances},
  {"wxlCustomOpcodes","WXL native custom opcode transport",Protocol},
  {"wxlModernAssets","WXL modern model and FDID assets; restart after changing",Assets},
  {"wxlModernBlp","WXL modern BLP texture decoding; restart after changing",Blp},
  {"wxlCustomMaterials","WXL custom mesh materials including BlendAdd; reload models after changing",Materials},
  {"wxlParticles","WXL modern Screen particle rendering; reload models after changing",Particles}};
 for(const auto& f:features){auto* cv=CVar::Register(f.name,f.help,2,(Compiled&f.bit)?"1":"0",&Changed,0,false,reinterpret_cast<void*>(uintptr_t(f.bit)),false);
  if(cv)Changed(cv,nullptr,cv->m_stringValue.m_str,reinterpret_cast<void*>(uintptr_t(f.bit)));
 }
}
bool ChallengeModeEnabled() noexcept{return On(ChallengeMode) && ClientExtensionsEnabled() && ProtocolEnabled();}
bool MythicDifficultyEnabled() noexcept{return On(MythicDifficulty) && ClientExtensionsEnabled();}
bool QuestMarkerEnabled() noexcept{return On(QuestMarker) && ClientExtensionsEnabled() && ProtocolEnabled();}
bool RadialPingEnabled() noexcept{return On(RadialPing) && ClientExtensionsEnabled() && ProtocolEnabled();}
bool ClientExtensionsEnabled() noexcept{return On(ClientExtensions);}
bool WmoEnabled() noexcept{return On(Wmo) && AssetsEnabled() && Db2Enabled();}
bool SplitAdtEnabled() noexcept{return On(SplitAdt) && AssetsEnabled() && Db2Enabled();}
bool NativeEnabled() noexcept{return WXL_ENABLE_NATIVE!=0;}
bool Db2Enabled() noexcept{return On(Db2);}
bool AppearancesEnabled() noexcept{return On(Appearances) && On(Assets) && On(Db2) && On(Blp);}
bool AppearanceSaveEnabled() noexcept{return On(AppearanceSave) && AppearancesEnabled() && ProtocolEnabled();}
bool ProtocolEnabled() noexcept{return On(Protocol);}
bool AssetsEnabled() noexcept{return On(Assets);}
bool BlpEnabled() noexcept{return On(Blp);}
bool MaterialsEnabled() noexcept{return On(Materials) && On(Assets);}
bool ParticlesEnabled() noexcept{return On(Particles) && On(Assets);}
}
