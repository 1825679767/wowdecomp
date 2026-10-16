// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
namespace wxl::waystone::appearance {
// The same three native / retail form pairs as the x32 Gilnean owner.
inline uint32_t AlternateRace(uint32_t native){return native==12?23:native==17?70:native==30?52:0;}
inline uint32_t RetailRace(uint32_t client){
 constexpr uint32_t ids[]={0,1,2,3,4,5,6,7,8,35,10,11,22,27,17,29,0,76,31,0,30,9,26,0,85,13,25,86,0,34,75,32};
 return client<sizeof(ids)/sizeof(ids[0])?ids[client]:0;
}
// Native fixtures gate private creature routes. Child families require the owned-child adapter. Inherited
// model-owned tracks passed both-sex native fixtures; CPU recipes alone do not admit a playable root.
inline bool SupportedRace(uint32_t race){const char* audit=std::getenv("WXL64_CHILD_MODELS_PROBE");return RetailRace(race) && (race!=14 || (std::getenv("WXL64_PRIVATE_RACES") && std::string_view(std::getenv("WXL64_PRIVATE_RACES"))=="1")) && ((audit && audit[0]=='1' && !audit[1]) || (race!=17 && race!=27 && race!=30));}
inline std::string PathKey(std::string_view path){std::string s(path);for(char& c:s){if(c=='/')c='\\';c=char(std::tolower(static_cast<unsigned char>(c)));}return s;}
inline uint64_t PathTag(std::string_view path){auto key=PathKey(path);uint64_t value=14695981039346656037ull;for(unsigned char c:key)value=(value^c)*1099511628211ull;return value;}
// Match root filenames only. Textures, effects and child actor stems never route.
inline bool RootIdentity(std::string_view path,uint32_t& race,uint32_t& sex,size_t& tail){
 const auto key=PathKey(path);if(key.rfind("character\\",0)!=0 && key.rfind("hd\\character\\",0)!=0)return false;
 const auto file=key.find_last_of('\\')+1;
 struct Family {const char* name;uint32_t race;};
 constexpr Family families[]={{"human",1},{"orc",2},{"dwarf",3},{"nightelf",4},{"scourge",5},{"tauren",6},{"gnome",7},{"troll",8},{"vulpera",9},{"bloodelf",10},{"draenei",11},{"worgen",12},{"nightborne",13},{"tuskarr",14},{"voidelf",15},{"dracthyrvisage",30},{"dracthyr",17},{"zandalaritroll",18},{"lightforgeddraenei",20},{"lightforged",20},{"naga_",25},{"naga",25},{"goblin",21},{"pandaren",26},{"earthendwarf",24},{"harronir",27},{"darkirondwarf",29},{"kultiran",31}};
 // Native display rows use abbreviated legacy filenames. Scope the aliases
 // to their authored directories; texture and child suffixes stay excluded.
 struct Alias{const char* directory;const char* stem;uint32_t race,sex;};
 constexpr Alias aliases[]={{"character\\goblin\\male\\","goblinm",21,0},{"character\\goblin\\female\\","goblinfm",21,1},{"character\\pandaren\\male\\","pandamale",26,0},{"character\\pandaren\\female\\","pandafemale",26,1}};
 for(auto a:aliases){const auto directory=key.rfind("hd\\",0)==0?std::string_view(key).substr(3,file-3):std::string_view(key).substr(0,file);if(directory!=a.directory || key.compare(file,std::strlen(a.stem),a.stem))continue;const auto at=file+std::strlen(a.stem);const auto rest=std::string_view(key).substr(at);const bool companion=(rest.size()==7 && std::isdigit(static_cast<unsigned char>(rest[0])) && std::isdigit(static_cast<unsigned char>(rest[1])) && rest.substr(2)==".skin") || (rest.size()==12 && rest[4]=='-' && rest.substr(7)==".anim" && std::all_of(rest.begin(),rest.begin()+4,[](char c){return c>='0' && c<='9';}) && std::isdigit(static_cast<unsigned char>(rest[5])) && std::isdigit(static_cast<unsigned char>(rest[6])));if(rest!=".m2" && rest!=".mdx" && !companion)continue;race=a.race;sex=a.sex;tail=at;return true;}
 for(auto f:families)for(uint32_t s=0;s<2;++s){std::string stem=std::string(f.name)+(s?"female":"male");if(key.compare(file,stem.size(),stem))continue;
  size_t at=file+stem.size();if(key.compare(at,3,"_hd")==0 || key.compare(at,3,"_dh")==0)at+=3;
  const auto rest=std::string_view(key).substr(at);
  const bool model=rest==".m2" || rest==".mdx";
  const bool skin=rest.size()==7 && std::isdigit(static_cast<unsigned char>(rest[0])) && std::isdigit(static_cast<unsigned char>(rest[1])) && rest.substr(2)==".skin";
  const bool anim=rest.size()==12 && rest[4]=='-' && rest.substr(7)==".anim" && std::all_of(rest.begin(),rest.begin()+4,[](char c){return c>='0' && c<='9';}) && rest[5]>='0' && rest[5]<='9' && rest[6]>='0' && rest[6]<='9';
  if(!model && !skin && !anim)continue;race=f.race;if(key.find("character\\nightelf_dh\\")!=std::string::npos)race=24;if(key.find("character\\bloodelf_dh\\")!=std::string::npos)race=27;sex=s;tail=at;return true;
 }
 return false;
}
}
