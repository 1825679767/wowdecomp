// Native, owned DB2 view for the retained portable wardrobe catalog logic.
#include "ExtensionApi.hpp"
#include "wxl/Db2Api.h"
#include "wxl/Db2StringApi.h"
#include "wxl/Db2FilterApi.h"
#include "WXL/modules/db2/items/Db2.hpp"
#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
namespace wxl_retail_ui {namespace {
namespace db=wxl::runtime::db2;
void Error(char* buffer,size_t size,const std::string& text){if(buffer&&size){auto n=std::min(size-1,text.size());std::memcpy(buffer,text.data(),n);buffer[n]=0;}}
void* Load(const WXL_Db2Definition* source,const WXL_Db2Filter* selected,char* error,size_t size){try{
 if(!source||!source->fields||!source->fieldCount||source->fieldCount>256||source->relationCount)return nullptr;
 std::vector<db::Field> fields;fields.reserve(source->fieldCount);for(uint32_t i=0;i<source->fieldCount;++i){const auto& f=source->fields[i];if(!f.name||!f.elements)return nullptr;std::string_view name=f.name;fields.push_back({name,f.elements,1,name.ends_with("_lang")});}
 db::Definition definition{source->name,source->filename,source->layoutHash,fields,{},source->required!=0};auto table=std::make_unique<db::Table>();std::string why;bool ok=false;
 if(selected){if(!selected->values||!selected->valueCount||selected->valueCount>4096||selected->source>2)return nullptr;db::wdc5::SnapshotFilter filter;filter.source=static_cast<db::wdc5::SnapshotFilterSource>(selected->source);filter.values={selected->values,selected->valueCount};filter.element=selected->element;
  if(selected->source==2){size_t i=0;for(;i<fields.size();++i)if(selected->field&&fields[i].name==selected->field)break;if(i==fields.size()||i>UINT16_MAX)return nullptr;filter.field=uint16_t(i);}ok=table->LoadFiltered(definition,filter,&why);
 }else ok=table->Load(definition,&why);
 if(!ok||table->Rows().size()>1000000){Error(error,size,why);return nullptr;}return table.release();
 }catch(...){Error(error,size,"Native wardrobe DB2 load failed");return nullptr;}}
void* LoadAll(const WXL_Db2Definition* d,char* e,size_t n){return Load(d,nullptr,e,n);}void* LoadSome(const WXL_Db2Definition* d,const WXL_Db2Filter* f,char* e,size_t n){return Load(d,f,e,n);}
void Release(void* t){delete static_cast<db::Table*>(t);}uint32_t Count(void* t){return t?uint32_t(static_cast<db::Table*>(t)->Rows().size()):0;}
const void* At(void* t,uint32_t i){if(!t)return nullptr;auto& rows=static_cast<db::Table*>(t)->Rows();return i<rows.size()?&rows[i]:nullptr;}
const void* Find(void* t,uint32_t i){return t?static_cast<db::Table*>(t)->Find(i):nullptr;}
uint32_t Id(const void* row){return row?static_cast<const db::wdc5::Row*>(row)->id:0;}uint32_t Parent(const void* row){return row?static_cast<const db::wdc5::Row*>(row)->parentId:0;}
uint32_t Value(void* t,const void* r,const char* f,uint32_t e){return t&&r&&f?static_cast<db::Table*>(t)->Value(*static_cast<const db::wdc5::Row*>(r),f,e):0;}
size_t Index(void* t,const char* f){return t&&f?static_cast<db::Table*>(t)->FieldIndex(f):size_t(-1);}size_t Elements(void* t,const char* f){return t&&f?static_cast<db::Table*>(t)->ElementCount(f):0;}
uint32_t Relation(void* t,const void* r,const char* f){return t&&r&&f?static_cast<db::Table*>(t)->RelationKey(*static_cast<const db::wdc5::Row*>(r),f):0;}
uint32_t Layout(void* t){return t?static_cast<db::Table*>(t)->LayoutHash():0;}uint32_t Hash(void* t){return t?static_cast<db::Table*>(t)->TableHash():0;}
const char* String(void* t,const void* r,const char* f,uint32_t e){if(!t||!r||!f)return "";auto value=static_cast<db::Table*>(t)->String(*static_cast<const db::wdc5::Row*>(r),f,e);return value.empty()?"":value.data();}
const WXL_Db2Api api{sizeof(WXL_Db2Api),WXL_DB2_API_VERSION,&LoadAll,&Release,&Count,&At,&Find,&Id,&Parent,&Value,&Index,&Elements,&Relation,&Layout,&Hash,&String};
const WXL_Db2StringApi strings{sizeof(WXL_Db2StringApi),WXL_DB2_STRING_API_VERSION,&String};const WXL_Db2FilterApi filtered{sizeof(WXL_Db2FilterApi),WXL_DB2_FILTER_API_VERSION,&LoadSome};
const void* Get(const char* name,uint32_t version){if(!name||version!=1)return nullptr;if(!std::strcmp(name,"wxl.db2"))return &api;if(!std::strcmp(name,"wxl.db2.strings"))return &strings;if(!std::strcmp(name,"wxl.db2.filtered"))return &filtered;return nullptr;}
const NativeInterfaces interfaces{&Get};
}
const NativeInterfaces* g_api=&interfaces;
}
